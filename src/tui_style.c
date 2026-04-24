// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_style.c -- Style & Theme management                            ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

void TUI_SetTheme(TUI_Context ctx, const TUI_Theme theme)
{
	ctx->DefaultTheme = theme;
}

TUI_Theme TUI_GetActiveTheme(TUI_Context ctx)
{
    if (ctx->ThemeStackPtr > 0) 
	{
        return ctx->ThemeStack[ctx->ThemeStackPtr - 1];
    }
    
    return ctx->DefaultTheme;
}

void TUI_PushTheme(TUI_Context ctx, const TUI_Theme theme)
{
	if (ctx->ThemeStackPtr < TUI_MAX_THEME_STACK) 
	{
        ctx->ThemeStack[ctx->ThemeStackPtr] = theme;
        ctx->ThemeStackPtr++;
    }
}

void TUI_PopTheme(TUI_Context ctx)
{
	if (ctx->ThemeStackPtr > 0)
	{
        ctx->ThemeStackPtr--;
	}
}

TUI_Theme TUI_GetClassicTheme(void) 
{
    TUI_Theme theme;

    // --- Window Style (White text on Blue background) ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Button Style (White on Cyan usually, or shadowed blue) ---
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE,  TUI_COLOR_CYAN);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_CYAN);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_WHITE,  TUI_COLOR_BLUE);

    // --- Input fields (Black on Gray for that "sunken" field look) ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLUE,  TUI_COLOR_WHITE);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE);

    // --- Labels (Transparent-ish, matches window) ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_WHITE,  TUI_COLOR_BLUE);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_BLUE);
    theme.Label.Active = TUI_MakeAttr(TUI_COLOR_WHITE,  TUI_COLOR_BLUE);

    // --- List Selections ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_CYAN);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE);
    theme.Selection.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    // --- Shadow ---
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK);

    return theme;
}

TUI_Theme TUI_GetTurboTheme(void) 
{
    TUI_Theme theme;

    // Window: Light Gray background with Black text (very readable)
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // Button: Deep Blue with White text, highlights in Yellow
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE,  TUI_COLOR_BLUE);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_BLUE);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLUE,   TUI_COLOR_WHITE);

    // Input: Sunken look (White background)
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLUE,  TUI_COLOR_WHITE);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE);

    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);

    // Selection: The classic "Inverse" look
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_CYAN);

    theme.Shadow = TUI_MakeAttr(TUI_COLOR_DARKGRAY, TUI_COLOR_BLACK);

    return theme;
}

TUI_Theme TUI_GetPaperTheme(void) 
{
    TUI_Theme theme;

    // Everything is a variation of Gray, White, and Black
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,     TUI_COLOR_WHITE);
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE,     TUI_COLOR_BLACK);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_LIGHTGRAY, TUI_COLOR_BLACK);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLACK,     TUI_COLOR_LIGHTGRAY);

    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE);

    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_DARKGRAY);

    theme.Shadow = TUI_MakeAttr(TUI_COLOR_LIGHTGRAY, TUI_COLOR_BLACK);

    return theme;
}

TUI_Theme TUI_GetMidnightTheme(void) 
{
    TUI_Theme theme;

    // Deep Green on Black
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK);

    // Buttons pop with Bright Green
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,      TUI_COLOR_GREEN);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK,      TUI_COLOR_LIGHTGREEN);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_LIGHTGREEN, TUI_COLOR_BLACK);

    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_LIGHTGREEN, TUI_COLOR_BLACK);
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_GREEN,      TUI_COLOR_BLACK);

    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGREEN);

    // No shadow for this one, or use a very subtle dark gray
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK);

    return theme;
}

TUI_Theme TUI_GetMonochromeTheme(void) 
{
    TUI_Theme theme;

    // --- Window: White text on Black background ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Button: Uses "Inverse Video" for focus ---
    // Normal: [ OK ] (White on Black)
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    // Hot: [ OK ] (Black on White - Inverse)
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE);
    // Active: [ OK ] (Bright White on Black)
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_DARKGRAY);

    // --- Input: Light Gray background to distinguish it from the window ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    // --- Labels: Just plain white text ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    // --- Selection: Pure Inverse ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY);

    // --- Shadow: In B&W, we usually don't have "colors",
    // so we use a dithered pattern (if your DrawBox supports it)
    // or just leave it empty.
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK);

    return theme;
}

