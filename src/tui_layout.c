// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_layout.c -- Layout engine, window stacking, focus              ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <string.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ Focus Management                                                   │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_SetFocus(TUI_Context ctx, uint32_t id)
{
    ctx->FocusId = id;
}

uint32_t
TUI_GetFocus(TUI_Context ctx)
{
    return ctx->FocusId;
}

bool
TUI_FocusRegister(TUI_Context ctx, uint32_t id)
{
    // Track first and last widget IDs for wrap-around
    if (ctx->FirstId == 0)
    {
        ctx->FirstId = id;
    }

    ctx->LastId = id;

    // If nothing is focused yet, focus the first interactive widget
    if (ctx->FocusId == 0)
    {
        ctx->FocusId = id;
    }

    // Build the focus chain: find what comes after and before the current focus
    static uint32_t s_PrevId       = 0;
    static bool     s_FoundCurrent = false;

    // Reset statics at the start of each frame (when FirstId just got set to this id)
    if (ctx->FirstId == id)
    {
        s_PrevId       = 0;
        s_FoundCurrent = false;
    }

    if (id == ctx->FocusId)
    {
        // This is the currently focused widget
        s_FoundCurrent   = true;
        ctx->PrevFocusId = s_PrevId;
    }
    else if (s_FoundCurrent && ctx->NextFocusId == 0)
    {
        // First widget after the focused one
        ctx->NextFocusId = id;
    }

    s_PrevId = id;

    return id == ctx->FocusId;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Layout Cursor Advancement                                          │
// ╰────────────────────────────────────────────────────────────────────╯

TUI_Pos
TUI_LayoutGetCursor(TUI_Context ctx, int widgetW)
{
    TUI_Pos pos;

    if (ctx->AlignCenter)
    {
        // Center within the clip rect (window interior width)
        int availW = ctx->ClipRect.W;
        int x = (availW - widgetW) / 2;
        if (x < 0) x = 0;
        pos.X = x;
        ctx->AlignCenter = false;
    }
    else
    {
        pos.X = ctx->Cursor.X + ctx->Indent;
    }

    pos.Y = ctx->Cursor.Y;
    return pos;
}

void
TUI_LayoutAdvance(TUI_Context ctx, int w, int h)
{
    ctx->LastWidgetWidth  = w;
    ctx->LastWidgetHeight = h;

    if (ctx->LayoutDir == TUI_DIRECTION_VERTICAL)
    {
        ctx->Cursor.Y += h + ctx->Margin;
    }
    else // HORIZONTAL
    {
        ctx->Cursor.X += w + ctx->Margin;

        if (h > ctx->RowMaxHeight)
        {
            ctx->RowMaxHeight = h;
        }
    }
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Layout API                                                         │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_LayoutSetCursor(TUI_Context ctx, int x, int y)
{
    ctx->Cursor.X = x;
    ctx->Cursor.Y = y;
}

void
TUI_LayoutRowBegin(TUI_Context ctx)
{
    ctx->LayoutDir    = TUI_DIRECTION_HORIZONTAL;
    ctx->RowMaxHeight = 0;
}

void
TUI_LayoutRowEnd(TUI_Context ctx)
{
    ctx->LayoutDir = TUI_DIRECTION_VERTICAL;
    
    // Advance Y past the tallest widget in the row
    ctx->Cursor.Y += ctx->RowMaxHeight + ctx->Margin;
    ctx->Cursor.X  = 0; // Reset X to left edge (indent re-applied by GetCursor)
    
    ctx->RowMaxHeight = 0;
}

void
TUI_LayoutSpace(TUI_Context ctx, int size)
{
    if (ctx->LayoutDir == TUI_DIRECTION_VERTICAL)
    {
        ctx->Cursor.Y += size;
    }
    else
    {
        ctx->Cursor.X += size;
    }
}

void
TUI_LayoutIndent(TUI_Context ctx, int indent)
{
    ctx->Indent = indent;
}

void
TUI_LayoutAlignCenter(TUI_Context ctx)
{
    ctx->AlignCenter = true;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Window Stacking                                                    │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_WindowBegin(TUI_Context ctx, int x, int y, int w, int h, const char* title)
{
    // Push current origin and clip rect
    if (ctx->StackPtr < TUI_MAX_STACK)
    {
        ctx->OffsetStack[ctx->StackPtr] = ctx->Origin;

        if (ctx->ClipPtr < TUI_MAX_STACK)
        {
            ctx->ClipStack[ctx->ClipPtr++] = ctx->ClipRect;
        }

        ctx->StackPtr++;
    }

    // Compute absolute position of window
    int absX = ctx->Origin.X + x;
    int absY = ctx->Origin.Y + y;

    // Draw the window shadow
    TUI_DrawShadow(ctx, x, y, w, h);

    // Draw the window box
    TUI_DrawBox(ctx, x, y, w, h, true);

    // Fill interior
    for (int row = 1; row < h - 1; row++)
    {
        for (int col = 1; col < w - 1; col++)
        {
            TUI_RenderPut(ctx, absX + col, absY + row, " ", ctx->CurrentAttr);
        }
    }

    // Draw title centered in top border
    if (title && title[0])
    {
        int titleLen = (int)strlen(title);
        int maxLen   = w - 4; // Space for " title "
                              //
        if (titleLen > maxLen)
        {
            titleLen = maxLen;
        }

        int tx = x + (w - titleLen - 2) / 2;

        TUI_DrawChar(ctx, tx, y, " ");
        
        for (int i = 0; i < titleLen; i++)
        {
            char temp[2] = { title[i], '\0' };

            TUI_DrawChar(ctx, tx + 1 + i, y, temp);
        }

        TUI_DrawChar(ctx, tx + 1 + titleLen, y, " ");
    }

    // Set new origin to inside the window (1 cell padding from border)
    ctx->Origin.X = absX + 1;
    ctx->Origin.Y = absY + 1;

    // Set clip rect to window interior
    ctx->ClipRect.X = absX + 1;
    ctx->ClipRect.Y = absY + 1;
    ctx->ClipRect.W = w - 2;
    ctx->ClipRect.H = h - 2;

    // Reset layout cursor for the new window scope
    ctx->Cursor.X         = 0;
    ctx->Cursor.Y         = 0;
    ctx->LastWidgetWidth  = 0;
    ctx->LastWidgetHeight = 0;
    ctx->RowMaxHeight     = 0;
    ctx->LayoutDir        = TUI_DIRECTION_VERTICAL;
    ctx->Indent           = 0;
    ctx->AlignCenter      = false;
}

void
TUI_WindowEnd(TUI_Context ctx)
{
    // Pop origin
    if (ctx->StackPtr > 0)
    {
        ctx->StackPtr--;
        ctx->Origin = ctx->OffsetStack[ctx->StackPtr];
    }

    // Pop clip rect
    if (ctx->ClipPtr > 0)
    {
        ctx->ClipPtr--;
        ctx->ClipRect = ctx->ClipStack[ctx->ClipPtr];
    }
    else
    {
        // Restore to full screen
        ctx->ClipRect.X = 0;
        ctx->ClipRect.Y = 0;
        ctx->ClipRect.W = ctx->ScreenWidth;
        ctx->ClipRect.H = ctx->ScreenHeight;
    }
}
