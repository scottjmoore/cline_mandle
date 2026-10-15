#include "input.h"
#include "terminal.h"
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <cctype>

namespace {

/// Read bytes from stdin with a short timeout, returning available data.
std::string read_available() {
    struct pollfd pfd{};
    pfd.fd     = STDIN_FILENO;
    pfd.events = POLLIN;

    int ret = poll(&pfd, 1, 50); // 50 ms timeout
    if (ret <= 0) return {};

    char buf[256];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) return {};
    return std::string(buf, static_cast<size_t>(n));
}

/// Parse a complete escape sequence or single char from the buffer.
/// Returns the event and advances the buffer.
std::optional<InputEvent> parse_one(const std::string& buf, size_t& pos) {
    if (pos >= buf.size()) return std::nullopt;

    char c = buf[pos];

    // Escape sequence
    if (c == '\x1b') {
        ++pos;
        if (pos >= buf.size()) return std::nullopt; // incomplete

        if (buf[pos] == '[') { // CSI
            ++pos;
            if (pos >= buf.size()) return std::nullopt;

            // Collect intermediate and parameter bytes
            std::string params;
            while (pos < buf.size() && (buf[pos] & 0x80) == 0 && buf[pos] < 0x20 && buf[pos] != 0x1b) {
                params += buf[pos];
                ++pos;
            }
            if (pos >= buf.size()) return std::nullopt;

            char final = buf[pos];
            ++pos;

            // Resize: CSI 8 ; height ; width t
            if (final == 't' && params.size() > 2) {
                std::istringstream iss(params);
                char mode;
                iss >> mode; // '8'
                char sep1;
                iss >> sep1; // ';'
                uint32_t h, w;
                iss >> h >> sep1 >> w;
                if (h > 0 && w > 0) {
                    return InputEvent{InputKind::Resize, "", w, h};
                }
            }

            // Arrow keys: CSI A/B/C/D
            if (final == 'A') return InputEvent{InputKind::Key, "Up", 0, 0};
            if (final == 'B') return InputEvent{InputKind::Key, "Down", 0, 0};
            if (final == 'C') return InputEvent{InputKind::Key, "Right", 0, 0};
            if (final == 'D') return InputEvent{InputKind::Key, "Left", 0, 0};

            // SGR mouse: CSI < M ; R ; C m
            //   M = button code: 0=left press, 1=right press, 2=move,
            //                   3=left release, 64=wheel up, 65=wheel down
            //   R = row (1-based), C = column (1-based)
            if (final == 'm' && !params.empty() && params[0] == '<') {
                std::string inner = params.substr(1); // strip '<'
                size_t p1 = inner.find(';');
                size_t p2 = inner.find(';', p1 != std::string::npos ? p1 + 1 : 0);
                if (p1 != std::string::npos && p2 != std::string::npos) {
                    int btn = 0, row = 0, col = 0;
                    try {
                        btn = std::stoi(inner.substr(0, p1));
                        row = std::stoi(inner.substr(p1 + 1, p2 - p1 - 1));
                        col = std::stoi(inner.substr(p2 + 1));
                    } catch (...) {
                        return InputEvent{InputKind::None, "", 0, 0};
                    }
                    // Convert 1-based to 0-based
                    uint32_t c = static_cast<uint32_t>(col > 0 ? col - 1 : 0);
                    uint32_t r = static_cast<uint32_t>(row > 0 ? row - 1 : 0);

                    if (btn == 64) return InputEvent{InputKind::MouseWheel, "up", c, r};
                    if (btn == 65) return InputEvent{InputKind::MouseWheel, "down", c, r};
                    if (btn == 0)  return InputEvent{InputKind::MouseDrag, "start", c, r};
                    if (btn == 3)  return InputEvent{InputKind::MouseDrag, "end", c, r};
                    if (btn == 2)  return InputEvent{InputKind::MouseDrag, "move", c, r};
                }
            }

            // Function keys: CSI 1 ~ etc. (ignore for now)
            return InputEvent{InputKind::None, "", 0, 0};
        }
        // Other escape sequences: ignore
        return InputEvent{InputKind::None, "", 0, 0};
    }

    // Regular key
    if (c == 'q' || c == 'Q') {
        ++pos;
        return InputEvent{InputKind::Quit, std::string(1, c), 0, 0};
    }
    if (c >= 32 && c < 127) {
        ++pos;
        return InputEvent{InputKind::Key, std::string(1, c), 0, 0};
    }

    ++pos;
    return InputEvent{InputKind::None, "", 0, 0};
}

} // anonymous namespace

std::optional<InputEvent> input_read() {
    std::string buf = read_available();
    if (buf.empty()) return std::nullopt;

    size_t pos = 0;
    return parse_one(buf, pos);
}

bool input_pending_resize() {
    struct pollfd pfd{};
    pfd.fd     = STDIN_FILENO;
    pfd.events = POLLIN;
    int ret = poll(&pfd, 1, 0);
    if (ret <= 0) return false;

    char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n > 0 && c == '\x1b') {
        // Peek ahead for CSI resize
        return true;
    }
    return false;
}
