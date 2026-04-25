// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_theme.c -- Style & Theme management                            ║
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

    theme.Background = TUI_COLOR_BLUE;

    // --- Window Style (White text on Blue background) ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Title Bar Style (Black text on Gray background) ---
    theme.TitleBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.TitleBar.Hot    = theme.TitleBar.Normal;
    theme.TitleBar.Active = theme.TitleBar.Normal;

    // --- Status Bar Style (Black text on Gray background) ---
    theme.StatusBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.StatusBar.Hot    = theme.StatusBar.Normal;
    theme.StatusBar.Active = theme.StatusBar.Normal;

    // --- Button Style (White on Cyan usually, or shadowed blue) ---
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,  TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLACK,  TUI_COLOR_GREEN, TUI_TEXT_BOLD);

    // --- Input fields (Black on Gray for that "sunken" field look) ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLUE,  TUI_COLOR_WHITE, TUI_TEXT_BOLD);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE, TUI_TEXT_BOLD);

    // --- Labels (Transparent-ish, matches window) ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,  TUI_COLOR_LIGHTGRAY, 0);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_LIGHTGRAY, 0);
    theme.Label.Active = TUI_MakeAttr(TUI_COLOR_BLACK,  TUI_COLOR_LIGHTGRAY, 0);

    // --- List Selections ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_CYAN, 0);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    theme.Selection.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);

    // --- Progress Bar ---
    theme.ProgressBar.Normal = TUI_MakeAttr(TUI_COLOR_DARKGRAY, TUI_COLOR_LIGHTGRAY, 0);
    theme.ProgressBar.Hot    = theme.ProgressBar.Normal;
    theme.ProgressBar.Active = theme.ProgressBar.Normal;

    // --- Shadow ---
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK, 0);

    return theme;
}

TUI_Theme TUI_GetTurboTheme(void) 
{
    TUI_Theme theme;

    // Window: Light Gray background with Black text (very readable)
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // Button: Deep Blue with White text, highlights in Yellow
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE,  TUI_COLOR_BLUE, 0);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_BLUE, 0);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLUE,   TUI_COLOR_WHITE, 0);

    // Input: Sunken look (White background)
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLUE,  TUI_COLOR_WHITE, 0);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE, 0);

    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);

    // Selection: The classic "Inverse" look
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE, 0);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_CYAN, 0);

    theme.Shadow = TUI_MakeAttr(TUI_COLOR_DARKGRAY, TUI_COLOR_BLACK, 0);

    return theme;
}

TUI_Theme TUI_GetPaperTheme(void) 
{
    TUI_Theme theme;

    // Everything is a variation of Gray, White, and Black
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,     TUI_COLOR_WHITE, 0);
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE,     TUI_COLOR_BLACK, 0);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_LIGHTGRAY, TUI_COLOR_BLACK, 0);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLACK,     TUI_COLOR_LIGHTGRAY, 0);

    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);

    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_DARKGRAY, 0);

    theme.Shadow = TUI_MakeAttr(TUI_COLOR_LIGHTGRAY, TUI_COLOR_BLACK, 0);

    return theme;
}

TUI_Theme TUI_GetMidnightTheme(void) 
{
    TUI_Theme theme;

    // Deep Green on Black
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);

    // Buttons pop with Bright Green
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,      TUI_COLOR_GREEN, 0);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK,      TUI_COLOR_LIGHTGREEN, 0);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_LIGHTGREEN, TUI_COLOR_BLACK, 0);

    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_LIGHTGREEN, TUI_COLOR_BLACK, 0);
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_GREEN,      TUI_COLOR_BLACK, 0);

    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGREEN, 0);

    // No shadow for this one, or use a very subtle dark gray
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK, 0);

    return theme;
}

TUI_Theme TUI_GetMonochromeTheme(void) 
{
    TUI_Theme theme;

    // --- Window: White text on Black background ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Button: Uses "Inverse Video" for focus ---
    // Normal: [ OK ] (White on Black)
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);
    // Hot: [ OK ] (Black on White - Inverse)
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    // Active: [ OK ] (Bright White on Black)
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_DARKGRAY, 0);

    // --- Input: Light Gray background to distinguish it from the window ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);

    // --- Labels: Just plain white text ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);

    // --- Selection: Pure Inverse ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);

    // --- Shadow: In B&W, we usually don't have "colors",
    // so we use a dithered pattern (if your DrawBox supports it)
    // or just leave it empty.
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK, 0);

    return theme;
}

