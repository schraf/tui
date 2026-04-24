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

// --- Single-line Box Characters ---
#define BOX_S_TL "\u250C" // ┌
#define BOX_S_TR "\u2510" // ┐
#define BOX_S_BL "\u2514" // └
#define BOX_S_BR "\u2518" // ┘
#define BOX_S_H  "\u2500" // ─
#define BOX_S_V  "\u2502" // │
#define BOX_S_TM "\u252C" // ┬ (Top Middle/Junction)
#define BOX_S_BM "\u2534" // ┴ (Bottom Middle/Junction)

// --- Double-line Box Characters ---
#define BOX_D_TL "\u2554" // ╔
#define BOX_D_TR "\u2557" // ╗
#define BOX_D_BL "\u255A" // ╚
#define BOX_D_BR "\u255D" // ╝
#define BOX_D_H  "\u2550" // ═
#define BOX_D_V  "\u2551" // ║
#define BOX_D_TM "\u2566" // ╦
#define BOX_D_BM "\u2569" // ╩

// --- The "Shadow" Characters (Essential for that DOS pop) ---
// These are dithered blocks used to create drop shadows on windows.
#define BOX_SHADE_LIGHT "\u2591" // ░
#define BOX_SHADE_MED   "\u2592" // ▒
#define BOX_SHADE_DARK  "\u2593" // ▓
#define BOX_FULL_BLOCK  "\u2588" // █

// ╭────────────────────────────────────────────────────────────────────╮
// │ Helper: Put a character at an origin-relative position             │
// ╰────────────────────────────────────────────────────────────────────╯

static void
Put(TUI_Context ctx, int x, int y, const char* ch)
{
    TUI_RenderPut(ctx, ctx->Origin.X + x, ctx->Origin.Y + y, ch, ctx->CurrentAttr);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Drawing Primitives                                                 │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_DrawChar(TUI_Context ctx, int x, int y, const char* c)
{
    Put(ctx, x, y, c);
}

void
TUI_DrawText(TUI_Context ctx, int x, int y, const char* text)
{
    // Need to handle UTF-8 properly if text contains it, but for simple ASCII text:
    for (int i = 0; text[i] != '\0'; i++)
    {
        char temp[2] = { text[i], '\0' };
        Put(ctx, x + i, y, temp);
    }
}

void
TUI_DrawBox(TUI_Context ctx, int x, int y, int w, int h, bool doubleLine)
{
    if (w < 2 || h < 2)
        return;

    const char *tl, *tr, *bl, *br, *hz, *vt;

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
TUI_FillRect(TUI_Context ctx, int x, int y, int w, int h, const char* c)
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
TUI_DrawHLine(TUI_Context ctx, int x, int y, int len, const char* c)
{
    for (int i = 0; i < len; i++)
    {
        Put(ctx, x + i, y, c);
    }
}

void
TUI_DrawVLine(TUI_Context ctx, int x, int y, int len, const char* c)
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
        Put(ctx, x + w, y + i, BOX_SHADE_DARK);
        Put(ctx, x + w + 1, y + i, BOX_SHADE_DARK);
    }

    // Bottom edge shadow (offset by 2 right to account for right shadow columns)
    for (int i = 2; i < w + 2; i++)
    {
        Put(ctx, x + i, y + h, BOX_SHADE_DARK);
    }

    ctx->CurrentAttr = saved;
}

void
TUI_Separator(TUI_Context ctx)
{
    // Span the full interior width of the current clip region
    int     w   = ctx->ClipRect.W;
    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);

    for (int i = 0; i < w; i++)
    {
        Put(ctx, pos.X + i, pos.Y, BOX_S_H);
    }

    TUI_LayoutAdvance(ctx, w, 1);
}
