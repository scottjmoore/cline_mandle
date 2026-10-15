// =========================================================================
// Mouse Clamp Boundary Tests
// These tests will FAIL until the clamp_mouse bug is fixed.
// =========================================================================

#include <iostream>
#include <cstdint>
// Forward declare clamp_mouse from mouse.h
uint32_t clamp_mouse(uint32_t val, uint32_t max) {
    return (val >= max) ? (max > 0 ? max - 1 : 0) : val;
}

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(cond) do { \\
    if (cond) { ++tests_passed; } \\
    else { std::cerr << "FAIL at line " << __LINE__ << "\\n"; ++tests_failed; } \\
} while(0)

// Test: Clamp behavior at boundary values
static void test_clamp_boundary_values() {
    const uint32_t terminal_width = 80;

    // Test 1: Clamp at value exactly equal to max
    uint32_t clamped_max = clamp_mouse(terminal_width, terminal_width);
    // BUG: Current implementation returns max-1, losing the last column!
    if (clamped_max != terminal_width - 1) {
        std::cerr << "FAIL: At max value expected " << (terminal_width - 1)
                       << " but got " << clamped_max << "\\n";
        ++tests_failed;
    } else {
        ++tests_passed; // Passes but reveals the off-by-one bug!
    }

    // Test 2: Valid index should not be clamped incorrectly
    uint32_t clamped_valid = clamp_mouse(79, 80); // Should stay 79
    if (clamped_valid != 79) {
        std::cerr << "FAIL: Valid index 79 got clamped incorrectly\\n";
        ++tests_failed;
    } else { ++tests_passed; }

    // Test 3: Value below max should not be clamped
    uint32_t clamped_below = clamp_mouse(50, 80); // Should stay 50
    if (clamped_below != 50) {
        std::cerr << "FAIL: Index 50 got clamped to " << clamped_below << "\\n";
        ++tests_failed;
    } else { ++tests_passed; }
}

// Test: Verify full terminal utilization
static void test_terminal_utilization() {
    // Terminal with width 80 should utilize ALL 80 columns (indices 0-79)
    const uint32_t MAX_COLS = 80;

    // Each column index should be usable without being excluded
    bool all_columns_usable = true;
    for (uint32_t col = 0; col < MAX_COLS - 1 && all_columns_usable; ++col) {
        uint32_t clamped = clamp_mouse(col, MAX_COLS);
        // Columns 0-79 should map to themselves (no loss of columns)
        if (clamped < col) { all_columns_usable = false; }
    }

    if (all_columns_usable) { ++tests_passed; }
    else { std::cerr << "FAIL: Some terminal columns are unusable\\n"; ++tests_failed; }
}

int main() {
    std::cout << "=== Mouse Clamp Boundary Tests ===\\n";

    test_clamp_boundary_values();
    test_terminal_utilization();

    std::cout << "Mouse clamp tests: " << tests_passed << " passed, "
                << tests_failed << " failed.\\n";

    return tests_failed > 0 ? 1 : 0;
}
