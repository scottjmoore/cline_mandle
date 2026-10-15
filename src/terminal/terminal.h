#ifndef TERMINAL_TERMINAL_H
#define TERMINAL_TERMINAL_H

#include <cstdint>
#include <string>

/// Terminal dimensions.
struct TermSize {
    uint32_t rows;
    uint32_t cols;
};

/// Initialise the terminal: alternate screen, hidden cursor, raw input,
/// SGR mouse reporting.  Returns false if stdout is not a tty.
bool terminal_init();

/// Restore the terminal to its original state.
void terminal_restore();

/// Get current terminal dimensions.
TermSize terminal_size();

/// Check whether stdout is a terminal.
bool isatty_stdout();

/// Write a string to stdout and flush.
void terminal_write(const std::string& s);

/// Clear the entire visible screen.
void terminal_clear();

/// Move cursor to (col, row) 0-based.
void terminal_cursor_move(uint32_t col, uint32_t row);

/// Check if a shutdown signal was received.
bool terminal_should_quit();

#endif // TERMINAL_TERMINAL_H
