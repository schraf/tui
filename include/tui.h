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

// ╭────────────────────────────────────────────────────────────────────╮
// │ Styles & Themes                                                    │
// ╰────────────────────────────────────────────────────────────────────╯

// A Style defines the look of a widget across its interaction states
typedef struct {
    TUI_Attr Normal;   // Default state
    TUI_Attr Hot;      // Focused or Hovered
    TUI_Attr Active;   // Currently being pressed/interacted with
} TUI_Style;

// A Theme is a collection of styles for the entire UI
typedef struct {
    TUI_Style Window;      // Frame and background
    TUI_Style Button;      // Standard buttons
    TUI_Style Input;       // Text fields and checkboxes
    TUI_Style Label;       // Static text
    TUI_Style Selection;   // Highlighted items in lists/menus
    TUI_Attr  Shadow;      // Color of the window drop-shadow
} TUI_Theme;

// Global theme control
void TUI_SetTheme(TUI_Context ctx, const TUI_Theme theme);

// Push a specific style override for the next widgets
void TUI_PushTheme(TUI_Context ctx, const TUI_Theme theme);
void TUI_PopTheme(TUI_Context ctx);

// Helpers to grab standard themes
TUI_Theme TUI_GetClassicTheme(void);
TUI_Theme TUI_GetTurboTheme(void);
TUI_Theme TUI_GetPaperTheme(void); 
TUI_Theme TUI_GetMidnightTheme(void);
TUI_Theme TUI_GetMonochromeTheme(void);

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
bool TUI_Button(TUI_Context ctx, uint32_t id, int w, const char* label);

// Static text display
void TUI_Label(TUI_Context ctx, const char* text);

// Returns true if value changed
bool TUI_Checkbox(TUI_Context ctx, uint32_t id, const char* label, bool* value);

// Radio button. Returns true if selection changed.
// `selected` points to the group's selected value; `value` is this button's value.
bool TUI_RadioButton(TUI_Context ctx, uint32_t id,
                     const char* label, int* selected, int value);

// A simple progress bar (0.0f to 1.0f)
void TUI_ProgressBar(TUI_Context ctx, int w, float fraction);

// Single-line text input field. Returns true if the text changed.
bool TUI_TextInput(TUI_Context ctx, uint32_t id, int w,
                   char* buffer, int bufferSize);

// Scrollable list box. Returns true if selection changed.
bool TUI_ListBox(TUI_Context ctx, uint32_t id, int w, int h,
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

// Horizontal separator spanning the full interior width of the current window.
void TUI_Separator(TUI_Context ctx);

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

// Manually add space between widgets (cells along the current axis)
void TUI_LayoutSpace(TUI_Context ctx, int size);

// Set a persistent indent level (added to Cursor.X at the start of each line).
// Pass 0 to reset.
void TUI_LayoutIndent(TUI_Context ctx, int indent);

// Center the next widget horizontally within the current window.
// This is a one-shot flag: it applies only to the immediately following widget.
void TUI_LayoutAlignCenter(TUI_Context ctx);

#endif
