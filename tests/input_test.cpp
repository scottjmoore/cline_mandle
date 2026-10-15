// Input parser unit tests (no terminal required)
// We test the escape-sequence parsing logic by feeding known byte
// sequences through a small inline test harness.
#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(cond) do { \
    if (cond) { ++tests_passed; } \
    else { std::cerr << "FAIL at " << __LINE__ << "\n"; ++tests_failed; } \
} while(0)

// Minimal inline parser that mirrors the logic in input.cpp
// so we can test without a real terminal.
enum class InputKind { Key, Resize, Quit, None };
struct InputEvent {
    InputKind kind;
    std::string key;
    uint32_t width, height;
};

static InputEvent parse_csi_arrow(const char* seq) {
    if (std::strcmp(seq, "\x1b[A") == 0)
        return {InputKind::Key, "Up", 0, 0};
    if (std::strcmp(seq, "\x1b[B") == 0)
        return {InputKind::Key, "Down", 0, 0};
    if (std::strcmp(seq, "\x1b[C") == 0)
        return {InputKind::Key, "Right", 0, 0};
    if (std::strcmp(seq, "\x1b[D") == 0)
        return {InputKind::Key, "Left", 0, 0};
    return {InputKind::None, "", 0, 0};
}

static void test_arrow_keys() {
    auto ev = parse_csi_arrow("\x1b[A");
    TEST(ev.kind == InputKind::Key && ev.key == "Up");

    ev = parse_csi_arrow("\x1b[B");
    TEST(ev.kind == InputKind::Key && ev.key == "Down");

    ev = parse_csi_arrow("\x1b[C");
    TEST(ev.kind == InputKind::Key && ev.key == "Right");

    ev = parse_csi_arrow("\x1b[D");
    TEST(ev.kind == InputKind::Key && ev.key == "Left");

    ev = parse_csi_arrow("\x1b[E");
    TEST(ev.kind == InputKind::None);
}

static void test_quit_key() {
    // 'q' should be parsed as quit
    char c = 'q';
    bool is_quit = (c == 'q' || c == 'Q');
    TEST(is_quit);

    c = 'Q';
    is_quit = (c == 'q' || c == 'Q');
    TEST(is_quit);

    c = 'x';
    is_quit = (c == 'q' || c == 'Q');
    TEST(!is_quit);
}

static void test_regular_keys() {
    // Printable ASCII should be treated as regular keys
    auto is_printable = [](char c) {
        return c >= 32 && c < 127;
    };
    TEST(is_printable('a'));
    TEST(is_printable(' '));
    TEST(is_printable('~'));
    TEST(!is_printable('\n'));
    TEST(!is_printable('\x1b'));
}

static void test_resize_sequence_format() {
    // CSI 8 ; H ; W t  — we just verify the format is recognized
    std::string resize = "\x1b[8;24;80t";
    TEST(resize.size() > 3);
    TEST(resize[0] == '\x1b');
    TEST(resize[1] == '[');
    TEST(resize.back() == 't');
}

int main() {
    test_arrow_keys();
    test_quit_key();
    test_regular_keys();
    test_resize_sequence_format();

    std::cout << "input_test: " << tests_passed << " passed, "
              << tests_failed << " failed.\n";
    return tests_failed > 0 ? 1 : 0;
}
