// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_widgets.c -- All interactive and display widgets                ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <string.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ Helper: Resolve widget position                                    │
// ╰────────────────────────────────────────────────────────────────────╯

// If x/y are -1, use layout cursor. Otherwise use explicit coords.
// Returns absolute screen position.
static TUI_Pos
ResolvePos(TUI_Context ctx, int x, int y)
{
    TUI_Pos p;

    p.X = ctx->Origin.X + ((x >= 0) ? x : ctx->Cursor.X);
    p.Y = ctx->Origin.Y + ((y >= 0) ? y : ctx->Cursor.Y);
    
    return p;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_Label                                                          │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_Label(TUI_Context ctx, int x, int y, const char* text)
{
    TUI_DrawText(ctx, x, y, text);
    TUI_LayoutAdvance(ctx, (int)strlen(text), 1);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_Button                                                         │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_Button(TUI_Context ctx, uint32_t id, int x, int y, int w, const char* label)
{
    bool focused = TUI_FocusRegister(ctx, id);

    int labelLen = (int)strlen(label);

    // Clamp button width: at minimum "[ X ]" = label + 4
    if (w < labelLen + 4)
    {
        w = labelLen + 4;
    }

    // Save current attr, set highlight if focused
    TUI_Attr saved = ctx->CurrentAttr;

    if (focused)
    {
        // Invert colors for focused button
        int fg = (saved >> 4) & 0x0F;
        int bg = saved & 0x0F;

        ctx->CurrentAttr = (TUI_Attr)((bg << 4) | fg);
    }

    // Draw button: "[ label ]" padded to width w
    TUI_DrawChar(ctx, x, y, '[');
    TUI_DrawChar(ctx, x + 1, y, ' ');

    // Center label within the button
    int innerW  = w - 4; // Space between "[ " and " ]"
    int padLeft = (innerW - labelLen) / 2;

    for (int i = 0; i < innerW; i++)
    {
        if (i >= padLeft && i < padLeft + labelLen)
        {
            TUI_DrawChar(ctx, x + 2 + i, y, label[i - padLeft]);
        }
        else
        {
            TUI_DrawChar(ctx, x + 2 + i, y, ' ');
        }
    }

    TUI_DrawChar(ctx, x + w - 2, y, ' ');
    TUI_DrawChar(ctx, x + w - 1, y, ']');

    ctx->CurrentAttr = saved;
    TUI_LayoutAdvance(ctx, w, 1);

    // Activate on Enter or Space
    return focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' ');
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_Checkbox                                                       │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_Checkbox(TUI_Context ctx, uint32_t id, int x, int y, const char* label, bool* value)
{
    bool focused = TUI_FocusRegister(ctx, id);
    bool changed = false;

    // Toggle on Enter or Space
    if (focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' '))
    {
        *value  = !(*value);
        changed = true;
    }

    // Save current attr, highlight if focused
    TUI_Attr saved = ctx->CurrentAttr;

    if (focused)
    {
        int fg = (saved >> 4) & 0x0F;
        int bg = saved & 0x0F;

        ctx->CurrentAttr = (TUI_Attr)((bg << 4) | fg);
    }

    // Draw: "[X] label" or "[ ] label"
    TUI_DrawChar(ctx, x, y, '[');
    TUI_DrawChar(ctx, x + 1, y, *value ? 'X' : ' ');
    TUI_DrawChar(ctx, x + 2, y, ']');
    TUI_DrawChar(ctx, x + 3, y, ' ');
    TUI_DrawText(ctx, x + 4, y, label);

    ctx->CurrentAttr = saved;
    TUI_LayoutAdvance(ctx, 4 + (int)strlen(label), 1);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_RadioButton                                                    │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_RadioButton(TUI_Context ctx, uint32_t id, int x, int y,
                const char* label, int* selected, int value)
{
    bool focused = TUI_FocusRegister(ctx, id);
    bool changed = false;

    // Select on Enter or Space
    if (focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' '))
    {
        if (*selected != value)
        {
            *selected = value;
            changed   = true;
        }
    }

    bool isSelected = (*selected == value);

    // Save current attr, highlight if focused
    TUI_Attr saved = ctx->CurrentAttr;

    if (focused)
    {
        int fg = (saved >> 4) & 0x0F;
        int bg = saved & 0x0F;

        ctx->CurrentAttr = (TUI_Attr)((bg << 4) | fg);
    }

    // Draw: "(o) label" or "( ) label"
    TUI_DrawChar(ctx, x, y, '(');
    TUI_DrawChar(ctx, x + 1, y, isSelected ? 'o' : ' ');
    TUI_DrawChar(ctx, x + 2, y, ')');
    TUI_DrawChar(ctx, x + 3, y, ' ');
    TUI_DrawText(ctx, x + 4, y, label);

    ctx->CurrentAttr = saved;
    TUI_LayoutAdvance(ctx, 4 + (int)strlen(label), 1);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_ProgressBar                                                    │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_ProgressBar(TUI_Context ctx, int x, int y, int w, float fraction)
{
    if (fraction < 0.0f) 
    {
        fraction = 0.0f;
    }

    if (fraction > 1.0f) 
    {
        fraction = 1.0f;
    }

    // Outer brackets
    TUI_DrawChar(ctx, x, y, '[');
    TUI_DrawChar(ctx, x + w - 1, y, ']');

    // Inner fill area
    int innerW = w - 2;
    int filled = (int)(fraction * (float)innerW + 0.5f);

    for (int i = 0; i < innerW; i++)
    {
        TUI_DrawChar(ctx, x + 1 + i, y, (i < filled) ? '#' : '.');
    }

    TUI_LayoutAdvance(ctx, w, 1);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_TextInput                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_TextInput(TUI_Context ctx, uint32_t id, int x, int y, int w,
              char* buffer, int bufferSize)
{
    bool focused = TUI_FocusRegister(ctx, id);
    bool changed = false;
    int  len     = (int)strlen(buffer);

    if (focused)
    {
        int key = ctx->LastKey;

        // Printable character input
        if (key >= 32 && key < 127 && len < bufferSize - 1)
        {
            buffer[len]     = (char)key;
            buffer[len + 1] = '\0';
            changed = true;
        }
        // Backspace
        else if (key == TUI_KEY_BACKSPACE && len > 0)
        {
            buffer[len - 1] = '\0';
            changed = true;
        }
        // Delete clears the field
        else if (key == TUI_KEY_DELETE)
        {
            buffer[0] = '\0';
            changed   = true;
        }
    }

    // Recalculate length after possible edit
    len = (int)strlen(buffer);

    // Save current attr
    TUI_Attr saved = ctx->CurrentAttr;

    // Draw input field background
    if (focused)
    {
        // Bright white on blue for focused input
        ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE);
    }
    else
    {
        // White on dark gray for unfocused input
        ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_DARKGRAY);
    }

    // The visible area is w characters wide
    // Show the tail of the string if it's longer than the field
    int visibleStart = 0;
    
    if (len > w)
    {
        visibleStart = len - w;
    }

    for (int i = 0; i < w; i++)
    {
        int bufIdx = visibleStart + i;
        char ch = (bufIdx < len) ? buffer[bufIdx] : ' ';

        TUI_DrawChar(ctx, x + i, y, ch);
    }

    // Draw cursor indicator at end of text (if focused and within bounds)
    if (focused)
    {
        int cursorPos = len - visibleStart;

        if (cursorPos >= 0 && cursorPos < w)
        {
            // Invert the cursor position
            TUI_Attr cursorAttr = TUI_MakeAttr(TUI_COLOR_BLUE, TUI_COLOR_WHITE);

            int absX = ctx->Origin.X + x + cursorPos;
            int absY = ctx->Origin.Y + y;

            TUI_RenderPut(ctx, absX, absY, ' ', cursorAttr);
        }
    }

    ctx->CurrentAttr = saved;
    TUI_LayoutAdvance(ctx, w, 1);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_ListBox                                                        │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_ListBox(TUI_Context ctx, uint32_t id, int x, int y, int w, int h,
            const char** items, int itemCount,
            int* selected, int* scrollOffset)
{
    bool focused = TUI_FocusRegister(ctx, id);
    bool changed = false;

    // Handle keyboard navigation when focused
    if (focused)
    {
        if (ctx->LastKey == TUI_KEY_UP && *selected > 0)
        {
            (*selected)--;
            changed = true;
        }
        else if (ctx->LastKey == TUI_KEY_DOWN && *selected < itemCount - 1)
        {
            (*selected)++;
            changed = true;
        }
        else if (ctx->LastKey == TUI_KEY_PGUP)
        {
            *selected -= (h - 2);
            if (*selected < 0) *selected = 0;
            changed = true;
        }
        else if (ctx->LastKey == TUI_KEY_PGDN)
        {
            *selected += (h - 2);
            if (*selected >= itemCount) *selected = itemCount - 1;
            changed = true;
        }
        else if (ctx->LastKey == TUI_KEY_HOME)
        {
            *selected = 0;
            changed   = true;
        }
        else if (ctx->LastKey == TUI_KEY_END)
        {
            *selected = itemCount - 1;
            changed   = true;
        }
    }

    // Ensure selection is valid
    if (*selected < 0) 
    {
        *selected = 0;
    }

    if (*selected >= itemCount) 
    {
        *selected = itemCount - 1;
    }

    // Adjust scroll offset to keep selection visible
    int visibleRows = h - 2; // Account for top/bottom border

    if (visibleRows < 1) 
    {
        visibleRows = 1;
    }

    if (*selected < *scrollOffset)
    {
        *scrollOffset = *selected;
    }

    if (*selected >= *scrollOffset + visibleRows)
    {
        *scrollOffset = *selected - visibleRows + 1;
    }

    if (*scrollOffset < 0)
    {
        *scrollOffset = 0;
    }

    // Draw border
    TUI_Attr saved = ctx->CurrentAttr;
    TUI_DrawBox(ctx, x, y, w, h, false);

    // Draw items
    int innerW = w - 2;

    for (int row = 0; row < visibleRows && (*scrollOffset + row) < itemCount; row++)
    {
        int itemIdx = *scrollOffset + row;
        bool isSelected = (itemIdx == *selected);

        if (isSelected && focused)
        {
            // Highlighted selection
            int fg = (saved >> 4) & 0x0F;
            int bg = saved & 0x0F;

            ctx->CurrentAttr = (TUI_Attr)((bg << 4) | fg);
        }
        else if (isSelected)
        {
            // Selected but not focused -- subtle highlight
            ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_DARKGRAY);
        }
        else
        {
            ctx->CurrentAttr = saved;
        }

        // Draw item text, truncated and padded to fill width
        const char* item    = items[itemIdx];
        int         itemLen = (int)strlen(item);

        for (int col = 0; col < innerW; col++)
        {
            char ch = (col < itemLen) ? item[col] : ' ';

            TUI_DrawChar(ctx, x + 1 + col, y + 1 + row, ch);
        }
    }

    // Draw scrollbar indicator on right border if content overflows
    if (itemCount > visibleRows)
    {
        int thumbPos = 0;

        if (itemCount > 1)
        {
            thumbPos = (*scrollOffset * (visibleRows - 1)) / (itemCount - visibleRows);
        }

        if (thumbPos < 0) 
        {
            thumbPos = 0;
        }

        if (thumbPos >= visibleRows) 
        {
            thumbPos = visibleRows - 1;
        }

        // Draw scrollbar track and thumb on the right border
        for (int row = 0; row < visibleRows; row++)
        {
            char ch = (row == thumbPos) ? '#' : '|';

            TUI_DrawChar(ctx, x + w - 1, y + 1 + row, ch);
        }
    }

    ctx->CurrentAttr = saved;
    TUI_LayoutAdvance(ctx, w, h);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_MenuBar                                                        │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_MenuBar(TUI_Context ctx, const char** items, int itemCount, int* selected)
{
    // MenuBar is drawn at absolute row 0, ignoring Origin
    TUI_Attr saved      = ctx->CurrentAttr;
    TUI_Pos  savedOrig  = ctx->Origin;
    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    // Menu bar background: fill the entire top row
    ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);
    TUI_FillRect(ctx, 0, 0, ctx->ScreenWidth, 1, ' ');

    // Handle keyboard: Left/Right to navigate
    if (ctx->LastKey == TUI_KEY_LEFT && *selected > 0)
    {
        (*selected)--;
    }

    if (ctx->LastKey == TUI_KEY_RIGHT && *selected < itemCount - 1)
    {
        (*selected)++;
    }

    // Draw menu items
    int xPos = 1;

    for (int i = 0; i < itemCount; i++)
    {
        int itemLen = (int)strlen(items[i]);

        if (i == *selected)
        {
            ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);
        }
        else
        {
            ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);
        }

        TUI_DrawChar(ctx, xPos, 0, ' ');
        TUI_DrawText(ctx, xPos + 1, 0, items[i]);
        TUI_DrawChar(ctx, xPos + 1 + itemLen, 0, ' ');

        xPos += itemLen + 3; // " item " + 1 spacing
    }

    ctx->CurrentAttr = saved;
    ctx->Origin      = savedOrig;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_StatusBar                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_StatusBar(TUI_Context ctx, const char* text)
{
    // StatusBar is drawn at the absolute bottom row, ignoring Origin
    TUI_Attr saved      = ctx->CurrentAttr;
    TUI_Pos  savedOrig  = ctx->Origin;

    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    int row = ctx->ScreenHeight - 1;

    ctx->CurrentAttr = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);

    TUI_FillRect(ctx, 0, row, ctx->ScreenWidth, 1, ' ');

    TUI_DrawChar(ctx, 0, row, ' ');
    TUI_DrawText(ctx, 1, row, text);

    ctx->CurrentAttr = saved;
    ctx->Origin      = savedOrig;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_MessageBox                                                     │
// ╰────────────────────────────────────────────────────────────────────╯

int
TUI_MessageBox(TUI_Context ctx, const char* title, const char* message,
               const char** buttons, int buttonCount)
{
    // Calculate message box dimensions
    int msgLen = (int)strlen(message);

    // Calculate total button width
    int totalBtnW = 0;
    
    for (int i = 0; i < buttonCount; i++)
    {
        totalBtnW += (int)strlen(buttons[i]) + 4; // "[ btn ]"
    }
                                                
    totalBtnW += (buttonCount - 1) * 2; // spacing between buttons

    // Box width: max of message width, button width, title width + some padding
    int titleLen = title ? (int)strlen(title) : 0;
    int boxW     = msgLen + 4;
    
    if (totalBtnW + 4 > boxW) 
    {
        boxW = totalBtnW + 4;
    }

    if (titleLen + 6 > boxW)
    {
        boxW = titleLen + 6;
    }

    if (boxW < 20) 
    {
        boxW = 20;
    }

    int boxH = 7; // border(1) + padding(1) + message(1) + padding(1) + buttons(1) + padding(1) + border(1)

    // Center on screen
    int boxX = (ctx->ScreenWidth - boxW) / 2;
    int boxY = (ctx->ScreenHeight - boxH) / 2;

    // Save origin
    TUI_Pos savedOrig = ctx->Origin;

    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    // Draw shadow
    TUI_DrawShadow(ctx, boxX, boxY, boxW, boxH);

    // Draw box with filled interior
    TUI_Attr saved = ctx->CurrentAttr;
    TUI_FillRect(ctx, boxX, boxY, boxW, boxH, ' ');
    TUI_DrawBox(ctx, boxX, boxY, boxW, boxH, true);

    // Draw title
    if (title && title[0])
    {
        int tx = boxX + (boxW - titleLen - 2) / 2;

        TUI_DrawChar(ctx, tx, boxY, ' ');
        TUI_DrawText(ctx, tx + 1, boxY, title);
        TUI_DrawChar(ctx, tx + 1 + titleLen, boxY, ' ');
    }

    // Draw message centered
    int mx = boxX + (boxW - msgLen) / 2;
    TUI_DrawText(ctx, mx, boxY + 2, message);

    // Draw buttons centered at bottom
    int btnY = boxY + boxH - 2;
    int btnX = boxX + (boxW - totalBtnW) / 2;
    int result = -1;

    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    for (int i = 0; i < buttonCount; i++)
    {
        int btnW     = (int)strlen(buttons[i]) + 4;
        uint32_t btnId = TUI_Id(buttons[i]) ^ (uint32_t)(i + 1);

        if (TUI_Button(ctx, btnId, btnX, btnY, btnW, buttons[i]))
        {
            result = i;
        }

        btnX += btnW + 2;
    }

    ctx->CurrentAttr = saved;
    ctx->Origin      = savedOrig;

    return result;
}
