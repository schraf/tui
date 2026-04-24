// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_core.c -- Lifecycle, context management, ID hashing            ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ ID Hashing                                                         │
// ╰────────────────────────────────────────────────────────────────────╯

uint32_t
TUI_Id(const char* str)
{
    // FNV-1a hash
    uint32_t hash = 2166136261u;
    for (const char* p = str; *p; p++)
    {
        hash ^= (uint32_t)(unsigned char)*p;
        hash *= 16777619u;
    }
    return hash;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Lifecycle                                                          │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_Init(TUI_Context* ctx)
{
    // Allocate and zero-init context
    *ctx = (TUI_Context)calloc(1, sizeof(struct TUI_Context));
    TUI_Context c = *ctx;

    // Default margin between widgets
    c->Margin    = 1;
    c->LayoutDir = TUI_DIRECTION_VERTICAL;

    // Current drawing attribute
    c->CurrentAttr = TUI_DEFAULT_ATTR;

    // Initialize output buffer
    TUI_OutInit(c);

    // Enter raw mode and detect terminal size
    TUI_TerminalEnterRaw(c);
    TUI_TerminalDetectSize(c);

    // Allocate cell buffers
    TUI_RenderAlloc(c, c->ScreenWidth, c->ScreenHeight);

    // Set up initial clip rect to cover entire screen
    c->ClipRect.X = 0;
    c->ClipRect.Y = 0;
    c->ClipRect.W = c->ScreenWidth;
    c->ClipRect.H = c->ScreenHeight;
    c->ClipPtr    = 0;

    // Set the render context for TUI_SetColor/TUI_SetAttr
    TUI_RenderSetCtx(c);

    // Switch to alternate screen buffer, hide cursor, clear screen
    const char* initSeq = TUI_ALT_BUF_ON TUI_CUR_HIDE TUI_CLR_SCR TUI_CUR_HOME;
    write(STDOUT_FILENO, initSeq, strlen(initSeq));

    // Force full redraw on first frame by marking front buffer as dirty
    int count = c->BufWidth * c->BufHeight;
    for (int i = 0; i < count; i++)
    {
        c->FrontBuf[i].Ch[0] = 0;
        c->FrontBuf[i].Attr  = 0xFF;
    }
}

void
TUI_Shutdown(TUI_Context* ctx)
{
    if (!ctx || !*ctx)
        return;

    TUI_Context c = *ctx;

    // Restore terminal
    TUI_TerminalExitRaw(c);

    // Show cursor, reset colors, leave alternate screen buffer
    const char* shutdownSeq = TUI_RESET TUI_CUR_SHOW TUI_ALT_BUF_OFF;
    write(STDOUT_FILENO, shutdownSeq, strlen(shutdownSeq));

    // Free resources
    TUI_RenderFree(c);
    TUI_OutFree(c);
    TUI_RenderSetCtx(NULL);

    free(c);
    *ctx = NULL;
}

void
TUI_Begin(TUI_Context ctx)
{
    // Set the render context
    TUI_RenderSetCtx(ctx);

    // Check for terminal resize
    TUI_TerminalDetectSize(ctx);

    if (ctx->Resized)
    {
        // Reallocate buffers to new size
        TUI_RenderFree(ctx);
        TUI_RenderAlloc(ctx, ctx->ScreenWidth, ctx->ScreenHeight);

        // Force full redraw
        int count = ctx->BufWidth * ctx->BufHeight;
        for (int i = 0; i < count; i++)
        {
            ctx->FrontBuf[i].Ch[0] = 0;
            ctx->FrontBuf[i].Attr  = 0xFF;
        }

        // Update clip rect
        ctx->ClipRect.X = 0;
        ctx->ClipRect.Y = 0;
        ctx->ClipRect.W = ctx->ScreenWidth;
        ctx->ClipRect.H = ctx->ScreenHeight;

        ctx->Resized = false;
    }

    // Clear back buffer
    TUI_RenderClear(ctx, TUI_DEFAULT_ATTR);

    // Poll input
    ctx->LastKey = TUI_TerminalPollKey();

    // Reset per-frame interaction state
    ctx->HotId = 0;

    // Reset focus chain tracking for this frame
    ctx->FirstId      = 0;
    ctx->LastId       = 0;
    ctx->NextFocusId  = 0;
    ctx->PrevFocusId  = 0;
    ctx->FocusChanged = false;

    // Reset layout
    ctx->Origin.X         = 0;
    ctx->Origin.Y         = 0;
    ctx->Cursor.X         = 0;
    ctx->Cursor.Y         = 0;
    ctx->LastWidgetWidth  = 0;
    ctx->LastWidgetHeight = 0;
    ctx->RowMaxHeight     = 0;
    ctx->LayoutDir        = TUI_DIRECTION_VERTICAL;
    ctx->StackPtr         = 0;
    ctx->ClipPtr          = 0;
    ctx->ClipRect.X       = 0;
    ctx->ClipRect.Y       = 0;
    ctx->ClipRect.W       = ctx->ScreenWidth;
    ctx->ClipRect.H       = ctx->ScreenHeight;

    // Reset attribute to default
    ctx->CurrentAttr = TUI_DEFAULT_ATTR;
}

void
TUI_End(TUI_Context ctx)
{
    // Handle Tab / Shift+Tab focus navigation after all widgets have registered
    if (ctx->LastKey == TUI_KEY_TAB)
    {
        if (ctx->NextFocusId != 0)
        {
            ctx->FocusId = ctx->NextFocusId;
        }
        else if (ctx->FirstId != 0)
        {
            ctx->FocusId = ctx->FirstId; // Wrap around
        }
    }
    else if (ctx->LastKey == TUI_KEY_BACKTAB)
    {
        if (ctx->PrevFocusId != 0)
        {
            ctx->FocusId = ctx->PrevFocusId;
        }
        else if (ctx->LastId != 0)
        {
            ctx->FocusId = ctx->LastId; // Wrap around
        }
    }

    // Diff and flush to terminal
    TUI_RenderFlush(ctx);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Screen Query                                                       │
// ╰────────────────────────────────────────────────────────────────────╯

int
TUI_GetScreenWidth(TUI_Context ctx)
{
    return ctx->ScreenWidth;
}

int
TUI_GetScreenHeight(TUI_Context ctx)
{
    return ctx->ScreenHeight;
}

int
TUI_GetKey(TUI_Context ctx)
{
    return ctx->LastKey;
}
