// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_draw.c -- Drawing primitives                                   ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <string.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ Box-Drawing Characters                                             │
// ╰────────────────────────────────────────────────────────────────────╯

// ASCII box-drawing characters for maximum compatibility.

// Single-line box chars
#define BOX_S_TL '+'
#define BOX_S_TR '+'
#define BOX_S_BL '+'
#define BOX_S_BR '+'
#define BOX_S_H  '-'
#define BOX_S_V  '|'

// Double-line box chars (ASCII approximation)
#define BOX_D_TL '*'
#define BOX_D_TR '*'
#define BOX_D_BL '*'
#define BOX_D_BR '*'
#define BOX_D_H  '='
#define BOX_D_V  '|'

// ╭────────────────────────────────────────────────────────────────────╮
// │ Helper: Put a character at an origin-relative position             │
// ╰────────────────────────────────────────────────────────────────────╯

static void
Put(TUI_Context ctx, int x, int y, char ch)
{
    TUI_RenderPut(ctx, ctx->Origin.X + x, ctx->Origin.Y + y, ch, ctx->CurrentAttr);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Drawing Primitives                                                 │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_DrawChar(TUI_Context ctx, int x, int y, char c)
{
    Put(ctx, x, y, c);
}

void
TUI_DrawText(TUI_Context ctx, int x, int y, const char* text)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        Put(ctx, x + i, y, text[i]);
    }
}

void
TUI_DrawBox(TUI_Context ctx, int x, int y, int w, int h, bool doubleLine)
{
    if (w < 2 || h < 2)
        return;

    char tl, tr, bl, br, hz, vt;

    if (doubleLine)
    {
        tl = BOX_D_TL; tr = BOX_D_TR;
        bl = BOX_D_BL; br = BOX_D_BR;
        hz = BOX_D_H;  vt = BOX_D_V;
    }
    else
    {
        tl = BOX_S_TL; tr = BOX_S_TR;
        bl = BOX_S_BL; br = BOX_S_BR;
        hz = BOX_S_H;  vt = BOX_S_V;
    }

    // Corners
    Put(ctx, x, y, tl);
    Put(ctx, x + w - 1, y, tr);
    Put(ctx, x, y + h - 1, bl);
    Put(ctx, x + w - 1, y + h - 1, br);

    // Top and bottom edges
    for (int i = 1; i < w - 1; i++)
    {
        Put(ctx, x + i, y, hz);
        Put(ctx, x + i, y + h - 1, hz);
    }

    // Left and right edges
    for (int i = 1; i < h - 1; i++)
    {
        Put(ctx, x, y + i, vt);
        Put(ctx, x + w - 1, y + i, vt);
    }
}

void
TUI_FillRect(TUI_Context ctx, int x, int y, int w, int h, char c)
{
    for (int row = 0; row < h; row++)
    {
        for (int col = 0; col < w; col++)
        {
            Put(ctx, x + col, y + row, c);
        }
    }
}

void
TUI_DrawHLine(TUI_Context ctx, int x, int y, int len, char c)
{
    for (int i = 0; i < len; i++)
    {
        Put(ctx, x + i, y, c);
    }
}

void
TUI_DrawVLine(TUI_Context ctx, int x, int y, int len, char c)
{
    for (int i = 0; i < len; i++)
    {
        Put(ctx, x, y + i, c);
    }
}

void
TUI_DrawShadow(TUI_Context ctx, int x, int y, int w, int h)
{
    // Save current attribute, switch to dark gray on black for shadow
    TUI_Attr saved = ctx->CurrentAttr;
    ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_DARKGRAY, TUI_COLOR_BLACK);

    // Right edge shadow (2 cells wide, offset by 1 down)
    for (int i = 1; i < h; i++)
    {
        Put(ctx, x + w, y + i, ' ');
        Put(ctx, x + w + 1, y + i, ' ');
    }

    // Bottom edge shadow (offset by 2 right to account for right shadow columns)
    for (int i = 2; i < w + 2; i++)
    {
        Put(ctx, x + i, y + h, ' ');
    }

    ctx->CurrentAttr = saved;
}

void
TUI_Separator(TUI_Context ctx, int x, int y, int w)
{
    for (int i = 0; i < w; i++)
    {
        Put(ctx, x + i, y, '-');
    }
}
