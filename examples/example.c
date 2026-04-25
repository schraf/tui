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
    
    //TUI_SetTheme(ctx, TUI_GetTurboTheme());

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

        // Title bar
        TUI_TitleBar(ctx, " TUI Example Program");

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

        switch (page)
        {
        // ── Welcome ──────────────────────────────────────────────
        case PAGE_WELCOME:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Setup - MyApp v1.0");

            TUI_LayoutSetCursor(ctx, 1, 1);
            TUI_Label(ctx, "Welcome to the MyApp Setup Wizard.");
            TUI_LayoutSpace(ctx, 1);
            TUI_Label(ctx, "This wizard will guide you through");
            TUI_Label(ctx, "the installation of MyApp on your");
            TUI_Label(ctx, "computer.");
            TUI_LayoutSpace(ctx, 1);
            TUI_Label(ctx, "It is recommended that you close");
            TUI_Label(ctx, "all other applications before");
            TUI_Label(ctx, "continuing.");

            TUI_LayoutSetCursor(ctx, 1, 13);
            TUI_LayoutRowBegin(ctx);
            TUI_LayoutSpace(ctx, 28);
            if (TUI_Button(ctx, TUI_Id("next_welcome"), 14, "Next >"))
                page = PAGE_LICENSE;
            TUI_LayoutSpace(ctx, 1);
            if (TUI_Button(ctx, TUI_Id("cancel_welcome"), 12, "Cancel"))
                showQuitDialog = true;
            TUI_LayoutRowEnd(ctx);

            TUI_WindowEnd(ctx);
            break;
        }

        // ── License Agreement ────────────────────────────────────
        case PAGE_LICENSE:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "License Agreement");

            TUI_LayoutSetCursor(ctx, 1, 1);
            TUI_Label(ctx, "Please review the license terms");
            TUI_Label(ctx, "before installing MyApp.");
            TUI_Separator(ctx);
            TUI_LayoutSpace(ctx, 1);
            TUI_Label(ctx, "MIT License");
            TUI_Label(ctx, "Copyright (c) 2026");
            TUI_Label(ctx, "Permission is hereby granted,");
            TUI_Label(ctx, "free of charge, to any person...");

            TUI_LayoutSetCursor(ctx, 1, 11);
            TUI_Checkbox(ctx, TUI_Id("accept"),
                         "I accept the license agreement", &acceptLicense);

            TUI_LayoutSetCursor(ctx, 1, 13);
            TUI_LayoutRowBegin(ctx);
            if (TUI_Button(ctx, TUI_Id("back_license"), 12, "< Back"))
                page = PAGE_WELCOME;
            TUI_LayoutSpace(ctx, 17);
            if (TUI_Button(ctx, TUI_Id("next_license"), 14, "Next >"))
            {
                if (acceptLicense)
                    page = PAGE_OPTIONS;
            }
            TUI_LayoutSpace(ctx, 1);
            if (TUI_Button(ctx, TUI_Id("cancel_license"), 12, "Cancel"))
                showQuitDialog = true;
            TUI_LayoutRowEnd(ctx);

            TUI_WindowEnd(ctx);
            break;
        }

        // ── Installation Options ─────────────────────────────────
        case PAGE_OPTIONS:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Installation Options");

            TUI_LayoutSetCursor(ctx, 1, 1);
            TUI_Label(ctx, "Choose installation type:");

            TUI_LayoutSetCursor(ctx, 3, 3);
            TUI_RadioButton(ctx, TUI_Id("typical"),
                            "Typical  - Recommended features", &installType, 0);
            TUI_RadioButton(ctx, TUI_Id("custom"),
                            "Custom   - Choose components", &installType, 1);
            TUI_RadioButton(ctx, TUI_Id("minimal"),
                            "Minimal  - Core files only", &installType, 2);

            TUI_LayoutSetCursor(ctx, 1, 7);
            TUI_Label(ctx, "Install path:");
            TUI_TextInput(ctx, TUI_Id("path"), winW - 4,
                          installPath, (int)sizeof(installPath));

            TUI_LayoutSetCursor(ctx, 1, 13);
            TUI_LayoutRowBegin(ctx);
            if (TUI_Button(ctx, TUI_Id("back_options"), 12, "< Back"))
                page = PAGE_LICENSE;
            TUI_LayoutSpace(ctx, 15);
            if (TUI_Button(ctx, TUI_Id("install"), 16, "Install >"))
            {
                page     = PAGE_INSTALL;
                progress = 0.0f;
            }
            TUI_LayoutSpace(ctx, 1);
            if (TUI_Button(ctx, TUI_Id("cancel_options"), 12, "Cancel"))
                showQuitDialog = true;
            TUI_LayoutRowEnd(ctx);

            TUI_WindowEnd(ctx);
            break;
        }

        // ── Installing ──────────────────────────────────────────
        case PAGE_INSTALL:
        {
            TUI_WindowBegin(ctx, winX, winY, winW, winH, "Installing...");

            TUI_LayoutSetCursor(ctx, 1, 1);
            TUI_Label(ctx, "Installing MyApp, please wait...");
            TUI_LayoutSpace(ctx, 1);
            TUI_Label(ctx, "Extracting files:");
            TUI_LayoutSpace(ctx, 1);
            TUI_ProgressBar(ctx, winW - 6, progress);

            TUI_LayoutSetCursor(ctx, 1, 7);
            if (progress < 0.25f)
                TUI_Label(ctx, "  myapp.exe");
            else if (progress < 0.50f)
                TUI_Label(ctx, "  libcore.dll");
            else if (progress < 0.75f)
                TUI_Label(ctx, "  data/config.ini");
            else
                TUI_Label(ctx, "  README.txt");

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

            TUI_LayoutSetCursor(ctx, 1, 1);
            TUI_Label(ctx, "MyApp has been successfully");
            TUI_Label(ctx, "installed on your computer.");
            TUI_LayoutSpace(ctx, 1);
            TUI_Label(ctx, "Install path:");
            TUI_LayoutSetCursor(ctx, 3, 5);
            TUI_Label(ctx, installPath);

            TUI_LayoutSetCursor(ctx, 1, 7);
            TUI_Label(ctx, "Thank you for choosing MyApp!");

            TUI_LayoutSetCursor(ctx, 1, 13);
            TUI_LayoutAlignCenter(ctx);
            if (TUI_Button(ctx, TUI_Id("finish"), 14, "Finish"))
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
