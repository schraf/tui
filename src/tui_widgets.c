// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_widgets.c -- All interactive and display widgets                ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <string.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_Label                                                          │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_Label(TUI_Context ctx, const char* text)
{
    int w = (int)strlen(text);
    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_DrawText(ctx, theme.Label.Normal, pos.X, pos.Y, text);
    TUI_LayoutAdvance(ctx, w, 1);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_Button                                                         │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_Button(TUI_Context ctx, uint32_t id, int w, const char* label)
{
    bool focused  = TUI_FocusRegister(ctx, id);
    int  labelLen = (int)strlen(label);

    // Clamp button width: at minimum " X " = label + 2
    if (w < labelLen + 2)
    {
        w = labelLen + 2;
    }

    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    int x = pos.X;
    int y = pos.Y;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = focused ? theme.Button.Hot : theme.Button.Normal;

    if (focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' '))
    {
        attr = theme.Button.Active;
    }

    // Draw button: " label " padded to width w
    TUI_DrawChar(ctx, attr, x, y, " ");

    // Center label within the button
    int innerW  = w - 2; // Space between " " and " "
    int padLeft = (innerW - labelLen) / 2;

    for (int i = 0; i < innerW; i++)
    {
        if (i >= padLeft && i < padLeft + labelLen)
        {
            char temp[2] = { label[i - padLeft], '\0' };
            TUI_DrawChar(ctx, attr, x + 1 + i, y, temp);
        }
        else
        {
            TUI_DrawChar(ctx, attr, x + 1 + i, y, " ");
        }
    }

    TUI_DrawChar(ctx, attr, x + w - 1, y, " ");

    TUI_LayoutAdvance(ctx, w, 1);

    // Activate on Enter or Space
    return focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' ');
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_Checkbox                                                       │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_Checkbox(TUI_Context ctx, uint32_t id, const char* label, bool* value)
{
    bool focused = TUI_FocusRegister(ctx, id);
    bool changed = false;

    // Toggle on Enter or Space
    if (focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' '))
    {
        *value  = !(*value);
        changed = true;
    }

    int w = 4 + (int)strlen(label);
    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    int x = pos.X;
    int y = pos.Y;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = focused ? theme.Input.Hot : theme.Input.Normal;

    if (focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' '))
    {
        attr = theme.Input.Active;
    }

    // Draw: "[X] label" or "[ ] label"
    TUI_DrawChar(ctx, attr, x, y, "[");
    TUI_DrawChar(ctx, attr, x + 1, y, *value ? "X" : " ");
    TUI_DrawChar(ctx, attr, x + 2, y, "]");
    TUI_DrawChar(ctx, attr, x + 3, y, " ");
    TUI_DrawText(ctx, attr, x + 4, y, label);

    TUI_LayoutAdvance(ctx, w, 1);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_RadioButton                                                    │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_RadioButton(TUI_Context ctx, uint32_t id,
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
    int w = 4 + (int)strlen(label);
    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    int x = pos.X;
    int y = pos.Y;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = focused ? theme.Input.Hot : theme.Input.Normal;

    if (focused && (ctx->LastKey == TUI_KEY_ENTER || ctx->LastKey == ' '))
    {
        attr = theme.Input.Active;
    }

    // Draw: "(·) label" or "( ) label"
    TUI_DrawChar(ctx, attr, x, y, "(");
    TUI_DrawChar(ctx, attr, x + 1, y, isSelected ? "·" : " ");
    TUI_DrawChar(ctx, attr, x + 2, y, ")");
    TUI_DrawChar(ctx, attr, x + 3, y, " ");
    TUI_DrawText(ctx, attr, x + 4, y, label);

    TUI_LayoutAdvance(ctx, w, 1);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_ProgressBar                                                    │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_ProgressBar(TUI_Context ctx, int w, float fraction)
{
    if (fraction < 0.0f) 
    {
        fraction = 0.0f;
    }

    if (fraction > 1.0f) 
    {
        fraction = 1.0f;
    }

    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    int x = pos.X;
    int y = pos.Y;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = theme.ProgressBar.Normal;

    int filled = (int)(fraction * (float)w + 0.5f);

    for (int i = 0; i < w; i++)
    {
        TUI_DrawChar(ctx, attr, x + 1 + i, y, (i < filled) ? "\u2588" : "\u2591");
    }

    TUI_LayoutAdvance(ctx, w, 1);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_TextInput                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_TextInput(TUI_Context ctx, uint32_t id, int w,
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

    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    int x = pos.X;
    int y = pos.Y;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = focused ? theme.Input.Hot : theme.Input.Normal;

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
        if (bufIdx < len)
        {
            char temp[2] = { buffer[bufIdx], '\0' };
            TUI_DrawChar(ctx, attr, x + i, y, temp);
        }
        else
        {
            TUI_DrawChar(ctx, attr, x + i, y, " ");
        }
    }

    // Draw cursor indicator at end of text (if focused and within bounds)
    if (focused)
    {
        int cursorPos = len - visibleStart;

        if (cursorPos >= 0 && cursorPos < w)
        {
            TUI_Attr cursorAttr = theme.Input.Active;

            int absX = ctx->Origin.X + x + cursorPos;
            int absY = ctx->Origin.Y + y;
            TUI_RenderPut(ctx, absX, absY, " ", cursorAttr);
        }
    }

    TUI_LayoutAdvance(ctx, w, 1);

    return changed;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_ListBox                                                        │
// ╰────────────────────────────────────────────────────────────────────╯

bool
TUI_ListBox(TUI_Context ctx, uint32_t id, int w, int h,
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

    TUI_Pos pos = TUI_LayoutGetCursor(ctx, w);
    int x = pos.X;
    int y = pos.Y;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr borderAttr = theme.Input.Normal;
    
    // Draw border
    TUI_DrawBox(ctx, borderAttr, x, y, w, h, false);

    // Draw items
    int innerW = w - 2;

    for (int row = 0; row < visibleRows && (*scrollOffset + row) < itemCount; row++)
    {
        int itemIdx = *scrollOffset + row;
        bool isSelected = (itemIdx == *selected);

        TUI_Attr itemAttr;
        if (isSelected && focused)
        {
            itemAttr = theme.Selection.Active;
        }
        else if (isSelected)
        {
            itemAttr = theme.Selection.Normal;
        }
        else
        {
            itemAttr = borderAttr;
        }

        // Draw item text, truncated and padded to fill width
        const char* item    = items[itemIdx];
        int         itemLen = (int)strlen(item);

        for (int col = 0; col < innerW; col++)
        {
            if (col < itemLen)
            {
                char temp[2] = { item[col], '\0' };
                TUI_DrawChar(ctx, itemAttr, x + 1 + col, y + 1 + row, temp);
            }
            else
            {
                TUI_DrawChar(ctx, itemAttr, x + 1 + col, y + 1 + row, " ");
            }
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
            const char* ch = (row == thumbPos) ? "\u2588" : "\u2502";
            TUI_DrawChar(ctx, borderAttr, x + w - 1, y + 1 + row, ch);
        }
    }

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
    TUI_Pos  savedOrig  = ctx->Origin;
    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr bgAttr = theme.Window.Normal;
    TUI_Attr selAttr = theme.Selection.Normal;

    // Menu bar background: fill the entire top row
    TUI_FillRect(ctx, bgAttr, 0, 0, ctx->ScreenWidth, 1, " ");

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
        TUI_Attr itemAttr = (i == *selected) ? selAttr : bgAttr;

        TUI_DrawChar(ctx, itemAttr, xPos, 0, " ");
        TUI_DrawText(ctx, itemAttr, xPos + 1, 0, items[i]);
        TUI_DrawChar(ctx, itemAttr, xPos + 1 + itemLen, 0, " ");

        xPos += itemLen + 3; // " item " + 1 spacing
    }

    ctx->Origin = savedOrig;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_TitleBar                                                       │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_TitleBar(TUI_Context ctx, const char* text)
{
    // TitleBar is drawn at the absolute top row, ignoring Origin
    TUI_Pos savedOrig = ctx->Origin;

    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = theme.TitleBar.Normal;

    TUI_FillRect(ctx, attr, 0, 0, ctx->ScreenWidth, 1, " ");

    TUI_DrawChar(ctx, attr, 0, 0, " ");
    TUI_DrawText(ctx, attr, 1, 0, text);

    ctx->Origin = savedOrig;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ TUI_StatusBar                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_StatusBar(TUI_Context ctx, const char* text)
{
    // StatusBar is drawn at the absolute bottom row, ignoring Origin
    TUI_Pos savedOrig = ctx->Origin;

    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    int row = ctx->ScreenHeight - 1;
    TUI_Theme theme = TUI_GetActiveTheme(ctx);
    TUI_Attr attr = theme.StatusBar.Normal;

    TUI_FillRect(ctx, attr, 0, row, ctx->ScreenWidth, 1, " ");

    TUI_DrawChar(ctx, attr, 0, row, " ");
    TUI_DrawText(ctx, attr, 1, row, text);

    ctx->Origin = savedOrig;
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
        totalBtnW += (int)strlen(buttons[i]) + 4; // " btn " + borders
    }

    totalBtnW += (buttonCount - 1) * 2; // spacing between buttons

    // Box width: max of message width, button width, title width + some padding
    int titleLen = title ? (int)strlen(title) : 0;
    int boxW     = msgLen + 4;
    if (totalBtnW + 4 > boxW) boxW = totalBtnW + 4;
    if (titleLen + 6 > boxW)  boxW = titleLen + 6;
    if (boxW < 20) boxW = 20;

    int boxH = 7; // border(1) + padding(1) + message(1) + padding(1) + buttons(1) + padding(1) + border(1)

    // Center on screen (coordinates are relative to current Origin, but we
    // need absolute screen-center positioning, so zero the origin first).
    TUI_Pos savedOrig = ctx->Origin;

    ctx->Origin.X = 0;
    ctx->Origin.Y = 0;

    int boxX = (ctx->ScreenWidth - boxW) / 2;
    int boxY = (ctx->ScreenHeight - boxH) / 2;

    // Use the window widget to draw the box, shadow, title, and set up the
    // interior origin/clip/layout automatically.
    TUI_WindowBegin(ctx, boxX, boxY, boxW, boxH, title);

    // Interior dimensions (inside the 1-cell border)
    int innerW = boxW - 2;
    int innerH = boxH - 2;

    // Message on the first content row (row 1 inside interior = 1 row of padding)
    TUI_LayoutSetCursor(ctx, (innerW - msgLen) / 2, 1);
    TUI_Label(ctx, message);

    // Buttons on the last content row (1 row of padding from bottom border)
    TUI_LayoutSetCursor(ctx, (innerW - totalBtnW) / 2, innerH - 2);
    TUI_LayoutRowBegin(ctx);

    int result = -1;

    for (int i = 0; i < buttonCount; i++)
    {
        int      btnW  = (int)strlen(buttons[i]) + 4;
        uint32_t btnId = TUI_Id(buttons[i]) ^ (uint32_t)(i + 1);

        if (TUI_Button(ctx, btnId, btnW, buttons[i]))
        {
            result = i;
        }

        // Add spacing between buttons (except after the last one)
        if (i < buttonCount - 1)
        {
            TUI_LayoutSpace(ctx, 2);
        }
    }

    TUI_LayoutRowEnd(ctx);
    TUI_WindowEnd(ctx);

    ctx->Origin = savedOrig;

    return result;
}
