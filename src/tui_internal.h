// ╔════════════════════════════════════════════════════════════════════╗
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#ifndef TUI_INTERNAL_H
#define TUI_INTERNAL_H

#include "tui.h"
#include <termios.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ Utility Types                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

typedef struct
{
    int X;
    int Y;
} TUI_Pos;

typedef struct
{
    int X, Y, W, H;
} TUI_Rect;

// A single cell in the screen buffer.
typedef struct
{
    char     Ch[4]; // The character to display (UTF-8, up to 3 bytes + null)
    TUI_Attr Attr;  // Packed fg/bg color attribute
} TUI_Cell;

// ╭────────────────────────────────────────────────────────────────────╮
// │ Constants                                                          │
// ╰────────────────────────────────────────────────────────────────────╯

#define TUI_MAX_STACK       16
#define TUI_MAX_THEME_STACK 8
#define TUI_OUT_BUF_INIT    8192

// ╭────────────────────────────────────────────────────────────────────╮
// │ State Tracking                                                     │
// ╰────────────────────────────────────────────────────────────────────╯

struct TUI_Context
{
    // --- Terminal State ---
    struct termios OrigTermios; // Saved terminal settings for restore

    // --- Input & Interaction ---
    int      LastKey;  // Last keypress caught during the frame
    uint32_t ActiveId; // ID of the widget currently being interacted with
    uint32_t HotId;    // ID of the widget currently under the hover/cursor

    // --- Focus ---
    uint32_t FocusId;     // Widget that currently has keyboard focus
    uint32_t FirstId;     // First interactive widget ID this frame (for wrap)
    uint32_t LastId;      // Last interactive widget ID this frame (for wrap)
    uint32_t NextFocusId; // Calculated next focus target (for Tab)
    uint32_t PrevFocusId; // Calculated prev focus target (for Shift+Tab)
    bool     FocusChanged;

    // --- Screen Dimensions ---
    int  ScreenWidth;  // Total terminal columns
    int  ScreenHeight; // Total terminal rows
    bool Resized;      // Flag set by SIGWINCH handler

    // --- Double-Buffered Cell Grid ---
    TUI_Cell* FrontBuf; // What's currently on screen
    TUI_Cell* BackBuf;  // What we're drawing this frame
    int       BufWidth;
    int       BufHeight;

    // --- Output Buffer (batched terminal writes) ---
    char* OutBuf;
    int   OutBufLen;
    int   OutBufCap;

    // --- Coordinate System & Window Stacking ---
    TUI_Pos  Origin;                       // Current drawing origin offset
    TUI_Pos  OffsetStack[TUI_MAX_STACK];
    int      StackPtr;

    // --- Clipping ---
    TUI_Rect ClipStack[TUI_MAX_STACK];
    int      ClipPtr;
    TUI_Rect ClipRect; // Current active clipping rectangle (screen coords)

    // --- Layout Engine ---
    TUI_Pos       Cursor;          // Next widget position relative to Origin
    int           LastWidgetWidth;
    int           LastWidgetHeight;
    int           RowMaxHeight;    // Tallest widget in current horizontal row
    int           Margin;          // Default padding between widgets
    TUI_Direction LayoutDir;
    int           Indent;          // Persistent X indent applied at each new line
    bool          AlignCenter;     // One-shot: center the next widget horizontally

	// --- Theming & Style Stack ---
    TUI_Theme DefaultTheme;  // The base theme used when no overrides are present
    TUI_Theme ThemeStack[TUI_MAX_THEME_STACK];
    int ThemeStackPtr;
};

// ╭────────────────────────────────────────────────────────────────────╮
// │ Internal Functions - Drawing (tui_draw.c)                          │
// ╰────────────────────────────────────────────────────────────────────╯

void TUI_DrawChar(TUI_Context ctx, TUI_Attr attr, int x, int y, const char* c);
void TUI_DrawText(TUI_Context ctx, TUI_Attr attr, int x, int y, const char* text);
void TUI_DrawBox(TUI_Context ctx, TUI_Attr attr, int x, int y, int w, int h, bool doubleLine);
void TUI_FillRect(TUI_Context ctx, TUI_Attr attr, int x, int y, int w, int h, const char* c);
void TUI_DrawHLine(TUI_Context ctx, TUI_Attr attr, int x, int y, int len, const char* c);
void TUI_DrawVLine(TUI_Context ctx, TUI_Attr attr, int x, int y, int len, const char* c);
void TUI_DrawShadow(TUI_Context ctx, TUI_Attr attr, int x, int y, int w, int h);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Internal Functions - Terminal (tui_terminal.c)                      │
// ╰────────────────────────────────────────────────────────────────────╯

void TUI_TerminalEnterRaw(TUI_Context ctx);
void TUI_TerminalExitRaw(TUI_Context ctx);
void TUI_TerminalDetectSize(TUI_Context ctx);
int  TUI_TerminalPollKey(void);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Internal Functions - Render (tui_render.c)                         │
// ╰────────────────────────────────────────────────────────────────────╯

void TUI_RenderAlloc(TUI_Context ctx, int w, int h);
void TUI_RenderFree(TUI_Context ctx);
void TUI_RenderClear(TUI_Context ctx, TUI_Attr attr);
void TUI_RenderPut(TUI_Context ctx, int x, int y, const char* ch, TUI_Attr attr);
void TUI_RenderFlush(TUI_Context ctx);
void TUI_RenderSetCtx(TUI_Context ctx);

// Output buffer helpers
void TUI_OutInit(TUI_Context ctx);
void TUI_OutFree(TUI_Context ctx);
void TUI_OutAppend(TUI_Context ctx, const char* data, int len);
void TUI_OutAppendf(TUI_Context ctx, const char* fmt, ...);
void TUI_OutFlush(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Internal Functions - Themes & Style (tui_style.c)                  │
// ╰────────────────────────────────────────────────────────────────────╯

TUI_Theme TUI_GetActiveTheme(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Internal Functions - Utility                                       │
// ╰────────────────────────────────────────────────────────────────────╯

// Advance layout cursor after placing a widget of given size
void TUI_LayoutAdvance(TUI_Context ctx, int w, int h);

// Returns the (x, y) position at which the next widget should draw.
// Applies the current Indent and consumes the one-shot AlignCenter flag.
// Must be called once per widget, before drawing, passing the widget's width.
TUI_Pos TUI_LayoutGetCursor(TUI_Context ctx, int widgetW);

// Register a widget ID for focus chain tracking.
// Call this from every interactive widget. Returns true if this widget has focus.
bool TUI_FocusRegister(TUI_Context ctx, uint32_t id);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Terminal Escape Codes                                              │
// ╰────────────────────────────────────────────────────────────────────╯

// --- Control Sequences ---
#define TUI_ESC "\033"
#define TUI_CSI "\033["

// --- Screen & Cursor ---
#define TUI_CLR_SCR     TUI_CSI "2J"    // Clear entire screen
#define TUI_CLR_LINE    TUI_CSI "2K"    // Clear current line
#define TUI_CUR_HOME    TUI_CSI "H"     // Move cursor to 0,0
#define TUI_CUR_HIDE    TUI_CSI "?25l"  // Hide cursor
#define TUI_CUR_SHOW    TUI_CSI "?25h"  // Show cursor

// --- Text Styles ---
#define TUI_RESET       TUI_CSI "0m"
#define TUI_BOLD        TUI_CSI "1m"
#define TUI_DIM         TUI_CSI "2m"
#define TUI_ITALIC      TUI_CSI "3m"
#define TUI_UNDERLINE   TUI_CSI "4m"
#define TUI_BLINK       TUI_CSI "5m"
#define TUI_RIS         TUI_ESC "c"     // Reset terminal to initial state

// --- Alternate Screen Buffer ---
#define TUI_ALT_BUF_ON  TUI_CSI "?1049h"
#define TUI_ALT_BUF_OFF TUI_CSI "?1049l"

#endif
