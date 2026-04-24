// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_render.c -- Cell buffer, double-buffering, diff & flush        ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ Color Mapping                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

// Map TUI_Color (0-15) -> ANSI foreground escape code number
static const int s_FgAnsi[16] = {
    30, // 0  BLACK
    34, // 1  BLUE
    32, // 2  GREEN
    36, // 3  CYAN
    31, // 4  RED
    35, // 5  MAGENTA
    33, // 6  BROWN  (= dark yellow)
    37, // 7  LIGHTGRAY
    90, // 8  DARKGRAY (= bright black)
    94, // 9  LIGHTBLUE
    92, // 10 LIGHTGREEN
    96, // 11 LIGHTCYAN
    91, // 12 LIGHTRED
    95, // 13 LIGHTMAGENTA
    93, // 14 YELLOW
    97, // 15 WHITE
};

// Map TUI_Color (0-15) -> ANSI background escape code number
static const int s_BgAnsi[16] = {
    40,  // 0  BLACK
    44,  // 1  BLUE
    42,  // 2  GREEN
    46,  // 3  CYAN
    41,  // 4  RED
    45,  // 5  MAGENTA
    43,  // 6  BROWN
    47,  // 7  LIGHTGRAY
    100, // 8  DARKGRAY
    104, // 9  LIGHTBLUE
    102, // 10 LIGHTGREEN
    106, // 11 LIGHTCYAN
    101, // 12 LIGHTRED
    105, // 13 LIGHTMAGENTA
    103, // 14 YELLOW
    107, // 15 WHITE
};

// ╭────────────────────────────────────────────────────────────────────╮
// │ Public Color API                                                   │
// ╰────────────────────────────────────────────────────────────────────╯

TUI_Attr
TUI_MakeAttr(TUI_Color fg, TUI_Color bg)
{
    return (TUI_Attr)(((bg & 0x0F) << 4) | (fg & 0x0F));
}

// File-scope pointer so TUI_SetColor/TUI_SetAttr can work without
// a context parameter. Set each frame by TUI_RenderSetCtx().
static TUI_Context s_CurrentCtx = NULL;

void
TUI_RenderSetCtx(TUI_Context ctx)
{
    s_CurrentCtx = ctx;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Output Buffer                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_OutInit(TUI_Context ctx)
{
    ctx->OutBufCap = TUI_OUT_BUF_INIT;
    ctx->OutBufLen = 0;
    ctx->OutBuf    = (char*)malloc((size_t)ctx->OutBufCap);
}

void
TUI_OutFree(TUI_Context ctx)
{
    free(ctx->OutBuf);

    ctx->OutBuf    = NULL;
    ctx->OutBufLen = 0;
    ctx->OutBufCap = 0;
}

void
TUI_OutAppend(TUI_Context ctx, const char* data, int len)
{
    if (ctx->OutBufLen + len > ctx->OutBufCap)
    {
        while (ctx->OutBufLen + len > ctx->OutBufCap)
        {
            ctx->OutBufCap *= 2;
        }

        ctx->OutBuf = (char*)realloc(ctx->OutBuf, (size_t)ctx->OutBufCap);
    }

    memcpy(ctx->OutBuf + ctx->OutBufLen, data, (size_t)len);
    ctx->OutBufLen += len;
}

void
TUI_OutAppendf(TUI_Context ctx, const char* fmt, ...)
{
    char    buf[256];
    va_list args;

    va_start(args, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (n > 0)
    {
        TUI_OutAppend(ctx, buf, n);
    }
}

void
TUI_OutFlush(TUI_Context ctx)
{
    if (ctx->OutBufLen > 0)
    {
        write(STDOUT_FILENO, ctx->OutBuf, (size_t)ctx->OutBufLen);
        ctx->OutBufLen = 0;
    }
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Cell Buffer Management                                             │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_RenderAlloc(TUI_Context ctx, int w, int h)
{
    size_t count  = (size_t)(w * h);
    ctx->FrontBuf = (TUI_Cell*)malloc(count * sizeof(TUI_Cell));
    ctx->BackBuf  = (TUI_Cell*)malloc(count * sizeof(TUI_Cell));
    ctx->BufWidth  = w;
    ctx->BufHeight = h;

    // Initialize both buffers to spaces with default attribute
    for (size_t i = 0; i < count; i++)
    {
        ctx->FrontBuf[i].Ch[0] = ' ';
        ctx->FrontBuf[i].Ch[1] = '\0';
        ctx->FrontBuf[i].Attr  = TUI_DEFAULT_ATTR;

        ctx->BackBuf[i].Ch[0]  = ' ';
        ctx->BackBuf[i].Ch[1]  = '\0';
        ctx->BackBuf[i].Attr   = TUI_DEFAULT_ATTR;
    }
}

void
TUI_RenderFree(TUI_Context ctx)
{
    free(ctx->FrontBuf);
    free(ctx->BackBuf);

    ctx->FrontBuf  = NULL;
    ctx->BackBuf   = NULL;
    ctx->BufWidth  = 0;
    ctx->BufHeight = 0;
}

void
TUI_RenderClear(TUI_Context ctx, TUI_Attr attr)
{
    int count = ctx->BufWidth * ctx->BufHeight;

    for (int i = 0; i < count; i++)
    {
        ctx->BackBuf[i].Ch[0] = ' ';
        ctx->BackBuf[i].Ch[1] = '\0';
        ctx->BackBuf[i].Attr  = attr;
    }
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Cell Write (with clipping)                                         │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_RenderPut(TUI_Context ctx, int x, int y, const char* ch, TUI_Attr attr)
{
    // Bounds check against buffer
    if (x < 0 || y < 0 || x >= ctx->BufWidth || y >= ctx->BufHeight)
    {
        return;
    }

    // Clip check
    TUI_Rect* clip = &ctx->ClipRect;
    if (x < clip->X || y < clip->Y || x >= clip->X + clip->W || y >= clip->Y + clip->H)
    {
        return;
    }

    int idx = y * ctx->BufWidth + x;

    strncpy(ctx->BackBuf[idx].Ch, ch, 3);
    
    ctx->BackBuf[idx].Ch[3] = '\0';
    ctx->BackBuf[idx].Attr   = attr;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Diff & Flush                                                       │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_RenderFlush(TUI_Context ctx)
{
    int       w        = ctx->BufWidth;
    int       h        = ctx->BufHeight;
    TUI_Cell* front    = ctx->FrontBuf;
    TUI_Cell* back     = ctx->BackBuf;
    TUI_Attr  lastAttr = 0xFF; // Force first color emission

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int idx = y * w + x;

            if (strcmp(back[idx].Ch, front[idx].Ch) == 0 && back[idx].Attr == front[idx].Attr)
            {
                continue; // No change
            }

            // Move cursor to (x, y) -- ANSI is 1-based
            TUI_OutAppendf(ctx, "\033[%d;%dH", y + 1, x + 1);

            // Emit color if changed
            if (back[idx].Attr != lastAttr)
            {
                int fg = back[idx].Attr & 0x0F;
                int bg = (back[idx].Attr >> 4) & 0x0F;

                TUI_OutAppendf(ctx, "\033[%d;%dm", s_FgAnsi[fg], s_BgAnsi[bg]);
                lastAttr = back[idx].Attr;
            }

            // Emit character
            TUI_OutAppend(ctx, back[idx].Ch, (int)strlen(back[idx].Ch));

            // Update front buffer
            strcpy(front[idx].Ch, back[idx].Ch);
            front[idx].Attr = back[idx].Attr;
        }
    }

    TUI_OutFlush(ctx);
}
