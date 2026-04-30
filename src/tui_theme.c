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

// ╭────────────────────────────────────────────────────────────────────╮
// │ Themes                                                             │
// ╰────────────────────────────────────────────────────────────────────╯

TUI_Theme TUI_GetClassicTheme(void) 
{
    TUI_Theme theme;

    theme.Background = TUI_COLOR_BLUE;

    // --- Window Style ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Title Bar Style ---
    theme.TitleBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.TitleBar.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.TitleBar.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_TEXT_BOLD);

    // --- Status Bar Style ---
    theme.StatusBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.StatusBar.Hot    = theme.StatusBar.Normal;
    theme.StatusBar.Active = theme.StatusBar.Normal;

    // --- Button Style ---
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_BLACK,  TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_YELLOW, TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLACK,  TUI_COLOR_GREEN, TUI_TEXT_BOLD);

    // --- Input fields ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_DARKGRAY, TUI_TEXT_BOLD);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLUE,  TUI_COLOR_WHITE, TUI_TEXT_BOLD);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLUE, TUI_TEXT_BOLD);

    // --- Labels ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, 0);
    theme.Label.Active = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);

    // --- List Selections ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_TEXT_BOLD);
    theme.Selection.Active = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);

    // --- Progress Bar ---
    theme.ProgressBar.Normal = TUI_MakeAttr(TUI_COLOR_DARKGRAY, TUI_COLOR_LIGHTGRAY, 0);
    theme.ProgressBar.Hot    = theme.ProgressBar.Normal;
    theme.ProgressBar.Active = theme.ProgressBar.Normal;

    // --- Shadow ---
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK, 0);

    return theme;
}

TUI_Theme TUI_GetPaperTheme(void) 
{
    TUI_Theme theme;

    theme.Background = TUI_COLOR_LIGHTGRAY;

    // --- Window Style ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, TUI_TEXT_BOLD);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Title Bar Style ---
    theme.TitleBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, TUI_TEXT_BOLD);
    theme.TitleBar.Hot    = theme.TitleBar.Normal;
    theme.TitleBar.Active = theme.TitleBar.Normal;

    // --- Status Bar Style ---
    theme.StatusBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, TUI_TEXT_BOLD);
    theme.StatusBar.Hot    = theme.StatusBar.Normal;
    theme.StatusBar.Active = theme.StatusBar.Normal;

    // --- Button Style ---
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);

    // --- Input fields ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);

    // --- Labels ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);
    theme.Label.Active = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_WHITE, 0);

    // --- List Selections ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);
    theme.Selection.Active = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, TUI_TEXT_BOLD);

    // --- Progress Bar ---
    theme.ProgressBar.Normal = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_LIGHTGRAY, 0);
    theme.ProgressBar.Hot    = theme.ProgressBar.Normal;
    theme.ProgressBar.Active = theme.ProgressBar.Normal;

    // --- Shadow ---
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK, 0);

    return theme;
}

TUI_Theme TUI_GetMidnightTheme(void) 
{
    TUI_Theme theme;

    theme.Background = TUI_COLOR_BLACK;

    // --- Window Style ---
    theme.Window.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_TEXT_BOLD);
    theme.Window.Hot    = theme.Window.Normal;
    theme.Window.Active = theme.Window.Normal;

    // --- Title Bar Style ---
    theme.TitleBar.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_TEXT_BOLD);
    theme.TitleBar.Hot    = theme.TitleBar.Normal;
    theme.TitleBar.Active = theme.TitleBar.Normal;

    // --- Status Bar Style ---
    theme.StatusBar.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_TEXT_BOLD);
    theme.StatusBar.Hot    = theme.StatusBar.Normal;
    theme.StatusBar.Active = theme.StatusBar.Normal;

    // --- Button Style ---
    theme.Button.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);
    theme.Button.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Button.Active = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_TEXT_BOLD);

    // --- Input fields ---
    theme.Input.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);
    theme.Input.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Input.Active = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_TEXT_BOLD);

    // --- Labels ---
    theme.Label.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);
    theme.Label.Hot    = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);
    theme.Label.Active = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);

    // --- List Selections ---
    theme.Selection.Normal = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, 0);
    theme.Selection.Hot    = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_GREEN, TUI_TEXT_BOLD);
    theme.Selection.Active = TUI_MakeAttr(TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_TEXT_BOLD);

    // --- Progress Bar ---
    theme.ProgressBar.Normal = TUI_MakeAttr(TUI_COLOR_LIGHTGREEN, TUI_COLOR_BLACK, 0);
    theme.ProgressBar.Hot    = theme.ProgressBar.Normal;
    theme.ProgressBar.Active = theme.ProgressBar.Normal;

    // --- Shadow ---
    theme.Shadow = TUI_MakeAttr(TUI_COLOR_BLACK, TUI_COLOR_BLACK, 0);

    return theme;
}

