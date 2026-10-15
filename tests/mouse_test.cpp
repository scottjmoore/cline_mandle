// Mouse / drag-selection unit tests
#include <cassert>
#include <cmath>
#include <iostream>
#include "mouse.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(cond) do { \
    if (cond) { ++tests_passed; } \
    else { std::cerr << "FAIL at " << __LINE__ << "\n"; ++tests_failed; } \
} while(0)

#define TEST_NEAR(a, b, eps) do { \
    if (std::fabs((a) - (b)) < (eps)) { ++tests_passed; } \
    else { std::cerr << "FAIL at " << __LINE__ \
                    << " expected " << (b) << " got " << (a) << "\n"; \
          ++tests_failed; } \
} while(0)

// --- Drag bounds tests ---
static void test_drag_bounds() {
    auto b = compute_drag_bounds(10, 5, 30, 15);
    TEST(b.col_min == 10);
    TEST(b.col_max == 30);
    TEST(b.row_min == 5);
    TEST(b.row_max == 15);
    TEST(b.span_cols == 20);
    TEST(b.span_rows == 10);

    // Reversed order — bounds still correct
    auto b2 = compute_drag_bounds(40, 20, 10, 5);
    TEST(b2.col_min == 10);
    TEST(b2.col_max == 40);
    TEST(b2.row_min == 5);
    TEST(b2.row_max == 20);

    // Zero extent
    auto b3 = compute_drag_bounds(10, 10, 10, 10);
    TEST(b3.span_cols == 0);
    TEST(b3.span_rows == 0);
    TEST(!b3.is_significant());

    // Trivial drag
    auto b4 = compute_drag_bounds(10, 10, 11, 11);
    TEST(!b4.is_significant());

    // Significant drag
    auto b5 = compute_drag_bounds(10, 10, 20, 12);
    TEST(b5.is_significant());

    // Centre computation
    auto b6 = compute_drag_bounds(10, 5, 30, 15);
    TEST(b6.centre_col() == 20);
    TEST(b6.centre_row() == 10);
}

// --- Zoom factor tests ---
static void test_zoom_factor() {
    // Half-screen drag → factor ≈ 2
    auto b1 = compute_drag_bounds(0, 0, 40, 12);
    TEST_NEAR(b1.zoom_factor(80, 24), 2.0, 0.01);

    // Quarter-screen drag → factor ≈ 4
    auto b2 = compute_drag_bounds(0, 0, 20, 6);
    TEST_NEAR(b2.zoom_factor(80, 24), 4.0, 0.01);

    // Tiny drag → factor 1.0 (no zoom)
    auto b3 = compute_drag_bounds(10, 10, 11, 11);
    TEST(b3.zoom_factor(80, 24) == 1.0);
}

// --- Clamp tests ---
static void test_clamp() {
    TEST(clamp_mouse(10, 80) == 10);
    TEST(clamp_mouse(80, 80) == 79);
    TEST(clamp_mouse(100, 80) == 79);
}

int main() {
    test_drag_bounds();
    test_zoom_factor();
    test_clamp();

    std::cout << "mouse_test: " << tests_passed << " passed, "
              << tests_failed << " failed.\n";
    return tests_failed > 0 ? 1 : 0;
}
