#include "terminal.h"
#include "input.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <csignal>
#include <atomic>

namespace {

// Original terminal state for restoration
struct TermiosState {
    termios saved;
    int saved_flags;
    bool was_set = false;
};

static TermiosState g_orig;
static std::atomic<bool> g_initialised{false};

// Signal flag: set on SIGINT / SIGTERM
static std::atomic<bool> g_shutdown{false};

static void signal_handler(int /*sig*/) {
    g_shutdown.store(true);
}

static void setup_signal_handlers() {
    struct sigaction sa{};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT,  &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
}

} // anonymous namespace

bool isatty_stdout() {
    return isatty(STDOUT_FILENO);
}

bool terminal_init() {
    if (!isatty_stdout()) {
        return false;
    }

    // Save terminal state
    if (tcgetattr(STDIN_FILENO, &g_orig.saved) == 0) {
        g_orig.saved_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        g_orig.was_set = true;
    }

    // Set raw mode: no echo, no canonical, no signal chars
    termios raw = g_orig.saved;
    raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    raw.c_iflag &= ~(ICRNL | INLCR | IGNCR | ISTRIP | INPCK);
    raw.c_oflag &= ~OPOST;
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // Non-blocking stdin
    fcntl(STDIN_FILENO, F_SETFL, g_orig.saved_flags | O_NONBLOCK);

    setup_signal_handlers();

    // Switch to alternate screen buffer
    printf("\x1b[?1049h");
    // Hide cursor
    printf("\x1b[?25l");
    // Enable SGR mouse reporting
    printf("\x1b[?1006h");
    // Enable mouse reporting
    printf("\x1b[?1002h");
    // Enable bracketed paste mode (helpful)
    printf("\x1b[?2004h");
    fflush(stdout);

    g_initialised.store(true);
    return true;
}

void terminal_restore() {
    if (!g_initialised.load()) return;

    // Show cursor
    printf("\x1b[?25h");
    // Disable mouse reporting
    printf("\x1b[?1002l");
    printf("\x1b[?1006l");
    // Disable bracketed paste
    printf("\x1b[?2004l");
    // Leave alternate screen
    printf("\x1b[?1049l");
    // Reset all SGR attributes
    printf("\x1b[0m");
    fflush(stdout);

    if (g_orig.was_set) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_orig.saved);
        fcntl(STDIN_FILENO, F_SETFL, g_orig.saved_flags);
    }

    g_initialised.store(false);
}

TermSize terminal_size() {
    struct winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0) {
        return {ws.ws_row, ws.ws_col};
    }
    return {24, 80}; // fallback
}

void terminal_write(const std::string& s) {
    fwrite(s.data(), 1, s.size(), stdout);
    fflush(stdout);
}

void terminal_clear() {
    printf("\x1b[2J\x1b[H");
    fflush(stdout);
}

void terminal_cursor_move(uint32_t col, uint32_t row) {
    // CSI (row+1);(col+1) H  (1-based)
    char buf[32];
    std::snprintf(buf, sizeof(buf), "\x1b[%u;%uH", row + 1, col + 1);
    fwrite(buf, 1, std::strlen(buf), stdout);
    fflush(stdout);
}

/// Check if a shutdown signal was received.
bool terminal_should_quit() {
    return g_shutdown.load();
}
