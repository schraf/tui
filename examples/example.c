// ╔════════════════════════════════════════════════════════════════════╗
// ║ example.c -- Mock installer demonstrating the TUI library          ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include <tui.h>
#include <string.h>
#include <unistd.h>

// Application state
typedef enum {
    PAGE_WELCOME,
    PAGE_LICENSE,
    PAGE_OPTIONS,
    PAGE_INSTALL,
    PAGE_DONE,
    PAGE_QUIT
} Page;

int
main(void)
{
    TUI_Context ctx;
    TUI_Init(&ctx);

    // State
    Page  page           = PAGE_WELCOME;
    bool  acceptLicense  = false;
    int   installType    = 0; // 0=Typical, 1=Custom, 2=Minimal
    char  installPath[128] = "/usr/local/myapp";
    float progress       = 0.0f;
    bool  showQuitDialog = false;
    int   quitChoice     = -1;

    bool running = true;
    while (running)
    {
        TUI_Begin(ctx);
        int sw = TUI_GetScreenWidth(ctx);
        int sh = TUI_GetScreenHeight(ctx);

        // Blue desktop background
        TUI_SetColor(TUI_COLOR_WHITE, TUI_COLOR_BLUE);
        //TUI_FillRect(ctx, 0, 0, sw, sh, " ");

        // Status bar
        TUI_StatusBar(ctx, " Press Tab to navigate | Enter to select | Esc to quit");

        // Handle Esc globally
        if (TUI_GetKey(ctx) == TUI_KEY_ESC && !showQuitDialog)
            showQuitDialog = true;

        // Center a window on screen
        int winW = 60;
        int winH = 18;
        int winX = (sw - winW) / 2;
        int winY = (sh - winH) / 2;

        TUI_SetColor(TUI_COLOR_WHITE, TUI_COLOR_CYAN);

        switch (page)
        {
        // ── Welcome ──────────────────────────────────────────────
        case PAGE_WELCOME:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Setup - MyApp v1.0");

            TUI_LayoutSetCursor(ctx, 1, 1);
            TUI_Label(ctx, 1, 1, "Welcome to the MyApp Setup Wizard.");
            TUI_Label(ctx, 1, 3, "This wizard will guide you through");
            TUI_Label(ctx, 1, 4, "the installation of MyApp on your");
            TUI_Label(ctx, 1, 5, "computer.");
            TUI_Label(ctx, 1, 7, "It is recommended that you close");
            TUI_Label(ctx, 1, 8, "all other applications before");
            TUI_Label(ctx, 1, 9, "continuing.");

            TUI_LayoutSetCursor(ctx, 1, 13);
            TUI_LayoutRowBegin(ctx);

            if (TUI_Button(ctx, TUI_Id("next_welcome"), 30, 13, 14, "Next >"))
                page = PAGE_LICENSE;

            if (TUI_Button(ctx, TUI_Id("cancel_welcome"), 45, 13, 12, "Cancel"))
                showQuitDialog = true;

            TUI_LayoutRowEnd(ctx);
            TUI_WindowEnd(ctx);
            break;
        }

        // ── License Agreement ────────────────────────────────────
        case PAGE_LICENSE:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "License Agreement");

            TUI_Label(ctx, 1, 1, "Please review the license terms");
            TUI_Label(ctx, 1, 2, "before installing MyApp.");
            TUI_Separator(ctx, 1, 3, winW - 4);
            TUI_Label(ctx, 1, 5, "MIT License");
            TUI_Label(ctx, 1, 6, "Copyright (c) 2026");
            TUI_Label(ctx, 1, 7, "Permission is hereby granted,");
            TUI_Label(ctx, 1, 8, "free of charge, to any person...");

            TUI_Checkbox(ctx, TUI_Id("accept"), 1, 11,
                         "I accept the license agreement", &acceptLicense);

            TUI_LayoutRowBegin(ctx);
            if (TUI_Button(ctx, TUI_Id("back_license"), 1, 13, 12, "< Back"))
                page = PAGE_WELCOME;

            if (TUI_Button(ctx, TUI_Id("next_license"), 30, 13, 14, "Next >"))
            {
                if (acceptLicense)
                    page = PAGE_OPTIONS;
            }

            if (TUI_Button(ctx, TUI_Id("cancel_license"), 45, 13, 12, "Cancel"))
                showQuitDialog = true;
            TUI_LayoutRowEnd(ctx);

            TUI_WindowEnd(ctx);
            break;
        }

        // ── Installation Options ─────────────────────────────────
        case PAGE_OPTIONS:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Installation Options");

            TUI_Label(ctx, 1, 1, "Choose installation type:");

            TUI_RadioButton(ctx, TUI_Id("typical"), 3, 3,
                            "Typical  - Recommended features", &installType, 0);
            TUI_RadioButton(ctx, TUI_Id("custom"), 3, 4,
                            "Custom   - Choose components", &installType, 1);
            TUI_RadioButton(ctx, TUI_Id("minimal"), 3, 5,
                            "Minimal  - Core files only", &installType, 2);

            TUI_Label(ctx, 1, 7, "Install path:");
            TUI_TextInput(ctx, TUI_Id("path"), 1, 8, winW - 4,
                          installPath, (int)sizeof(installPath));

            TUI_LayoutRowBegin(ctx);
            if (TUI_Button(ctx, TUI_Id("back_options"), 1, 13, 12, "< Back"))
                page = PAGE_LICENSE;

            if (TUI_Button(ctx, TUI_Id("install"), 28, 13, 16, "Install >"))
            {
                page     = PAGE_INSTALL;
                progress = 0.0f;
            }

            if (TUI_Button(ctx, TUI_Id("cancel_options"), 45, 13, 12, "Cancel"))
                showQuitDialog = true;
            TUI_LayoutRowEnd(ctx);

            TUI_WindowEnd(ctx);
            break;
        }

        // ── Installing ──────────────────────────────────────────
        case PAGE_INSTALL:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Installing...");

            TUI_Label(ctx, 1, 1, "Installing MyApp, please wait...");
            TUI_Label(ctx, 1, 3, "Extracting files:");
            TUI_ProgressBar(ctx, 1, 5, winW - 4, progress);

            // Show current "file" being installed
            if (progress < 0.25f)
                TUI_Label(ctx, 1, 7, "  myapp.exe");
            else if (progress < 0.50f)
                TUI_Label(ctx, 1, 7, "  libcore.dll");
            else if (progress < 0.75f)
                TUI_Label(ctx, 1, 7, "  data/config.ini");
            else
                TUI_Label(ctx, 1, 7, "  README.txt");

            // Simulate progress
            if (progress < 1.0f)
                progress += 0.005f;
            else
                page = PAGE_DONE;

            TUI_WindowEnd(ctx);
            break;
        }

        // ── Done ─────────────────────────────────────────────────
        case PAGE_DONE:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Installation Complete");

            TUI_Label(ctx, 1, 1, "MyApp has been successfully");
            TUI_Label(ctx, 1, 2, "installed on your computer.");
            TUI_Label(ctx, 1, 4, "Install path:");
            TUI_Label(ctx, 3, 5, installPath);

            TUI_Label(ctx, 1, 7, "Thank you for choosing MyApp!");

            if (TUI_Button(ctx, TUI_Id("finish"), 22, 13, 14, "Finish"))
                running = false;

            TUI_WindowEnd(ctx);
            break;
        }

        case PAGE_QUIT:
            running = false;
            break;
        }

        // ── Quit confirmation dialog (modal overlay) ─────────────
        if (showQuitDialog)
        {
            const char* quitBtns[] = { "Yes", "No" };
            quitChoice = TUI_MessageBox(ctx,
                "Confirm Exit",
                "Are you sure you want to quit?",
                quitBtns, 2);

            if (quitChoice == 0)
                running = false;
            else if (quitChoice == 1)
                showQuitDialog = false;
        }

        TUI_End(ctx);

        // Small sleep to avoid busy-spinning (~60fps)
        usleep(16000);
    }

    TUI_Shutdown(&ctx);
    return 0;
}
