// ╔════════════════════════════════════════════════════════════════════╗
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#ifndef TUI_H
#define TUI_H

#include <stdbool.h>
#include <stdint.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ State Tracking                                                     │
// ╰────────────────────────────────────────────────────────────────────╯

struct TUI_Context;
typedef struct TUI_Context* TUI_Context;

// ╭────────────────────────────────────────────────────────────────────╮
// │ Lifecycle                                                          │
// ╰────────────────────────────────────────────────────────────────────╯

// Initializes the terminal (enters raw mode, hides cursor, etc.)
void TUI_Init(TUI_Context* ctx);

// Restores terminal state
void TUI_Shutdown(TUI_Context* ctx);

// Starts a new frame. Clears internal hit-testing state.
void TUI_Begin(TUI_Context ctx);

// Finalizes the frame and flushes output to the terminal.
void TUI_End(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Input                                                              │
// ╰────────────────────────────────────────────────────────────────────╯

// Key constants (values chosen to avoid collision with printable ASCII)
enum {
    TUI_KEY_NONE      = 0,
    TUI_KEY_ENTER     = 13,
    TUI_KEY_TAB       = 9,
    TUI_KEY_BACKSPACE = 127,
    TUI_KEY_ESC       = 27,

    // Extended keys start at 256 to avoid ASCII collisions
    TUI_KEY_UP        = 256,
    TUI_KEY_DOWN      = 257,
    TUI_KEY_LEFT      = 258,
    TUI_KEY_RIGHT     = 259,
    TUI_KEY_HOME      = 260,
    TUI_KEY_END       = 261,
    TUI_KEY_PGUP      = 262,
    TUI_KEY_PGDN      = 263,
    TUI_KEY_DELETE    = 264,
    TUI_KEY_INSERT    = 265,
    TUI_KEY_BACKTAB   = 266, // Shift+Tab

    TUI_KEY_F1        = 280,
    TUI_KEY_F2        = 281,
    TUI_KEY_F3        = 282,
    TUI_KEY_F4        = 283,
    TUI_KEY_F5        = 284,
    TUI_KEY_F6        = 285,
    TUI_KEY_F7        = 286,
    TUI_KEY_F8        = 287,
    TUI_KEY_F9        = 288,
    TUI_KEY_F10       = 289,
    TUI_KEY_F11       = 290,
    TUI_KEY_F12       = 291,
};

// Returns the key pressed this frame, or TUI_KEY_NONE if nothing was pressed.
int TUI_GetKey(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Colors & Attributes                                                │
// ╰────────────────────────────────────────────────────────────────────╯

// CGA/EGA-compatible 16-color palette
typedef enum {
    TUI_COLOR_BLACK        = 0,
    TUI_COLOR_BLUE         = 1,
    TUI_COLOR_GREEN        = 2,
    TUI_COLOR_CYAN         = 3,
    TUI_COLOR_RED          = 4,
    TUI_COLOR_MAGENTA      = 5,
    TUI_COLOR_BROWN        = 6,
    TUI_COLOR_LIGHTGRAY    = 7,
    TUI_COLOR_DARKGRAY     = 8,
    TUI_COLOR_LIGHTBLUE    = 9,
    TUI_COLOR_LIGHTGREEN   = 10,
    TUI_COLOR_LIGHTCYAN    = 11,
    TUI_COLOR_LIGHTRED     = 12,
    TUI_COLOR_LIGHTMAGENTA = 13,
    TUI_COLOR_YELLOW       = 14,
    TUI_COLOR_WHITE        = 15
} TUI_Color;

// Packed color attribute: low nibble = foreground, high nibble = background
// Mirrors the DOS video attribute byte layout.
typedef uint8_t TUI_Attr;

TUI_Attr TUI_MakeAttr(TUI_Color fg, TUI_Color bg);

// Set the current drawing color using separate fg/bg values.
void TUI_SetColor(TUI_Color fg, TUI_Color bg);

// Set the current drawing color using a packed attribute.
void TUI_SetAttr(TUI_Attr attr);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Utility                                                            │
// ╰────────────────────────────────────────────────────────────────────╯

// Generate a widget ID from a string (FNV-1a hash). Useful for creating
// stable IDs for widgets: TUI_Button(ctx, TUI_Id("ok"), ...)
uint32_t TUI_Id(const char* str);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Screen Query                                                       │
// ╰────────────────────────────────────────────────────────────────────╯

int TUI_GetScreenWidth(TUI_Context ctx);
int TUI_GetScreenHeight(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Drawing Primitives                                                 │
// ╰────────────────────────────────────────────────────────────────────╯

void TUI_DrawChar(TUI_Context ctx, int x, int y, const char* c);
void TUI_DrawText(TUI_Context ctx, int x, int y, const char* text);
void TUI_DrawBox(TUI_Context ctx, int x, int y, int w, int h, bool doubleLine);
void TUI_FillRect(TUI_Context ctx, int x, int y, int w, int h, const char* c);
void TUI_DrawHLine(TUI_Context ctx, int x, int y, int len, const char* c);
void TUI_DrawVLine(TUI_Context ctx, int x, int y, int len, const char* c);
void TUI_DrawShadow(TUI_Context ctx, int x, int y, int w, int h);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Focus                                                              │
// ╰────────────────────────────────────────────────────────────────────╯

// Set keyboard focus to a specific widget.
void TUI_SetFocus(TUI_Context ctx, uint32_t id);

// Get the currently focused widget ID.
uint32_t TUI_GetFocus(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Widgets                                                            │
// ╰────────────────────────────────────────────────────────────────────╯

// Returns true if activated (Enter pressed while focused)
bool TUI_Button(TUI_Context ctx, uint32_t id, int x, int y, int w, const char* label);

// Static text display
void TUI_Label(TUI_Context ctx, int x, int y, const char* text);

// Returns true if value changed
bool TUI_Checkbox(TUI_Context ctx, uint32_t id, int x, int y, const char* label, bool* value);

// Radio button. Returns true if selection changed.
// `selected` points to the group's selected value; `value` is this button's value.
bool TUI_RadioButton(TUI_Context ctx, uint32_t id, int x, int y,
                     const char* label, int* selected, int value);

// A simple progress bar (0.0f to 1.0f)
void TUI_ProgressBar(TUI_Context ctx, int x, int y, int w, float fraction);

// Single-line text input field. Returns true if the text changed.
bool TUI_TextInput(TUI_Context ctx, uint32_t id, int x, int y, int w,
                   char* buffer, int bufferSize);

// Scrollable list box. Returns true if selection changed.
bool TUI_ListBox(TUI_Context ctx, uint32_t id, int x, int y, int w, int h,
                 const char** items, int itemCount,
                 int* selected, int* scrollOffset);

// Menu bar drawn at the top of the screen.
void TUI_MenuBar(TUI_Context ctx, const char** items, int itemCount,
                 int* selected);

// Status bar drawn at the bottom of the screen.
void TUI_StatusBar(TUI_Context ctx, const char* text);

// Modal message box. Returns button index (0-based) when confirmed, or -1.
int TUI_MessageBox(TUI_Context ctx, const char* title, const char* message,
                   const char** buttons, int buttonCount);

// Horizontal separator / divider line.
void TUI_Separator(TUI_Context ctx, int x, int y, int w);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Windows                                                            │
// ╰────────────────────────────────────────────────────────────────────╯

// A window/panel container with a title
void TUI_WindowBegin(TUI_Context ctx, int x, int y, int w, int h, const char* title);

// Restores the previous draw offset and ends the current clipping region.
void TUI_WindowEnd(TUI_Context ctx);

// ╭────────────────────────────────────────────────────────────────────╮
// │ Layout                                                             │
// ╰────────────────────────────────────────────────────────────────────╯

typedef enum {
    TUI_DIRECTION_VERTICAL,
    TUI_DIRECTION_HORIZONTAL
} TUI_Direction;

// Sets the cursor to a specific starting point inside a window
void TUI_LayoutSetCursor(TUI_Context ctx, int x, int y);

// Switch to horizontal placement (widgets will appear side-by-side)
void TUI_LayoutRowBegin(TUI_Context ctx);

// Switch back to vertical placement (the default)
void TUI_LayoutRowEnd(TUI_Context ctx);

// Manually add space between widgets
void TUI_LayoutSpace(TUI_Context ctx, int size);

#endif
