// ╔════════════════════════════════════════════════════════════════════╗
// ║ tui_terminal.c -- Raw mode, input polling, terminal size           ║
// ╠════════════════════════════════════════════════════════════════════╣
// ║ Copyright (c) 2026 Marc Schraffenberger.                           ║
// ║ SPDX-License-Identifier: MIT                                       ║
// ╚════════════════════════════════════════════════════════════════════╝

#include "tui_internal.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// ╭────────────────────────────────────────────────────────────────────╮
// │ Resize Signal Handling                                             │
// ╰────────────────────────────────────────────────────────────────────╯

// Global flag set by SIGWINCH -- checked in TUI_Begin.
static volatile sig_atomic_t s_Resized = 0;

static void
SignalWinchHandler(int sig)
{
    (void)sig;
    s_Resized = 1;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Raw Mode                                                           │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_TerminalEnterRaw(TUI_Context ctx)
{
    tcgetattr(STDIN_FILENO, &ctx->OrigTermios);

    struct termios raw = ctx->OrigTermios;

    // Input: disable break signal, CR-to-NL, parity, strip, flow control
    raw.c_iflag &= (tcflag_t) ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // Output: disable post-processing
    raw.c_oflag &= (tcflag_t) ~(OPOST);

    // Control: set 8-bit chars
    raw.c_cflag |= (tcflag_t)(CS8);

    // Local: disable echo, canonical mode, signals, extended input
    raw.c_lflag &= (tcflag_t) ~(ECHO | ICANON | ISIG | IEXTEN);

    // Non-blocking read: return immediately with 0 bytes if nothing available
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    // Install SIGWINCH handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SignalWinchHandler;
    sa.sa_flags   = SA_RESTART;
    sigaction(SIGWINCH, &sa, NULL);
}

void
TUI_TerminalExitRaw(TUI_Context ctx)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->OrigTermios);

    // Restore default SIGWINCH
    signal(SIGWINCH, SIG_DFL);
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Terminal Size Detection                                            │
// ╰────────────────────────────────────────────────────────────────────╯

void
TUI_TerminalDetectSize(TUI_Context ctx)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0)
    {
        ctx->ScreenWidth  = ws.ws_col;
        ctx->ScreenHeight = ws.ws_row;
    }
    else
    {
        // Fallback: assume standard 80x24
        ctx->ScreenWidth  = 80;
        ctx->ScreenHeight = 24;
    }

    // Transfer signal flag into context
    ctx->Resized = s_Resized != 0;
    s_Resized = 0;
}

// ╭────────────────────────────────────────────────────────────────────╮
// │ Input Polling                                                      │
// ╰────────────────────────────────────────────────────────────────────╯

// Read a single byte from stdin. Returns -1 if nothing available.
static int
ReadByte(void)
{
    unsigned char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);

    return (n == 1) ? (int)c : -1;
}

// Try to read an escape sequence and return the corresponding TUI_KEY_*.
// Called after we've already consumed '\033'.
static int
ParseEscapeSequence(void)
{
    int c1 = ReadByte();

    if (c1 < 0)
    {
        return TUI_KEY_ESC; // Just ESC by itself
    }

    if (c1 == '[')
    {
        int c2 = ReadByte();

        if (c2 < 0)
        {
            return TUI_KEY_ESC;
        }

        // CSI sequences with a number prefix: \033[1~ through \033[24~
        if (c2 >= '0' && c2 <= '9')
        {
            // Accumulate digits
            int num = c2 - '0';
            int c3  = ReadByte();

            while (c3 >= '0' && c3 <= '9')
            {
                num = num * 10 + (c3 - '0');
                c3  = ReadByte();
            }

            if (c3 == '~')
            {
                switch (num)
                {
                case 1: return TUI_KEY_HOME;
                case 2: return TUI_KEY_INSERT;
                case 3: return TUI_KEY_DELETE;
                case 4: return TUI_KEY_END;
                case 5: return TUI_KEY_PGUP;
                case 6: return TUI_KEY_PGDN;
                case 7: return TUI_KEY_HOME;
                case 8: return TUI_KEY_END;
                case 11: return TUI_KEY_F1;
                case 12: return TUI_KEY_F2;
                case 13: return TUI_KEY_F3;
                case 14: return TUI_KEY_F4;
                case 15: return TUI_KEY_F5;
                case 17: return TUI_KEY_F6;
                case 18: return TUI_KEY_F7;
                case 19: return TUI_KEY_F8;
                case 20: return TUI_KEY_F9;
                case 21: return TUI_KEY_F10;
                case 23: return TUI_KEY_F11;
                case 24: return TUI_KEY_F12;
                default: return TUI_KEY_NONE;
                }
            }

            // \033[1;2X sequences (modified keys)
            if (c3 == ';')
            {
                int mod = ReadByte(); // modifier digit
                int fin = ReadByte(); // final character
                                     
                (void)mod;

                if (fin == 'Z')
                {
                    return TUI_KEY_BACKTAB;
                }

                return TUI_KEY_NONE;
            }

            return TUI_KEY_NONE;
        }

        // Simple CSI sequences: \033[A, \033[B, etc.
        switch (c2)
        {
        case 'A': return TUI_KEY_UP;
        case 'B': return TUI_KEY_DOWN;
        case 'C': return TUI_KEY_RIGHT;
        case 'D': return TUI_KEY_LEFT;
        case 'H': return TUI_KEY_HOME;
        case 'F': return TUI_KEY_END;
        case 'Z': return TUI_KEY_BACKTAB; // Shift+Tab on many terminals
        default: return TUI_KEY_NONE;
        }
    }

    // \033O sequences (SS3 -- function keys on some terminals)
    if (c1 == 'O')
    {
        int c2 = ReadByte();

        if (c2 < 0)
        {
            return TUI_KEY_ESC;
        }

        switch (c2)
        {
        case 'P': return TUI_KEY_F1;
        case 'Q': return TUI_KEY_F2;
        case 'R': return TUI_KEY_F3;
        case 'S': return TUI_KEY_F4;
        case 'H': return TUI_KEY_HOME;
        case 'F': return TUI_KEY_END;
        default: return TUI_KEY_NONE;
        }
    }

    return TUI_KEY_NONE;
}

int
TUI_TerminalPollKey(void)
{
    int c = ReadByte();

    if (c < 0)
    {
        return TUI_KEY_NONE;
    }

    if (c == 27) // ESC
    {
        return ParseEscapeSequence();
    }

    // Map common control codes
    if (c == '\r' || c == '\n')
    {
        return TUI_KEY_ENTER;
    }

    if (c == '\t')
    {
        return TUI_KEY_TAB;
    }

    if (c == 127 || c == 8) // DEL or BS
    {
        return TUI_KEY_BACKSPACE;
    }

    // Printable ASCII or other control
    return c;
}
