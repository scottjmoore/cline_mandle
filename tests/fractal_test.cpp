// Fractal unit tests: escape-time, coordinate mapping, zoom-to-point
#include <cassert>
#include <cmath>
#include <iostream>
#include "../src/fractal/mandelbrot.h"
#include "../src/fractal/view.h"

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

// --- Escape-time tests ---
static void test_escape_time() {
    // Point at origin is inside the set
    TEST(mandelbrot_escape_time(0.0, 0.0, 200) == 200.0);

    // Point far outside escapes quickly
    double val = mandelbrot_escape_time(3.0, 0.0, 200);
    TEST(val < 10.0);

    // c = -2.1 escapes quickly (outside the set)
    TEST(mandelbrot_escape_time(-2.1, 0.0, 200) < 5.0);

    // Smooth iteration should be non-integer for escaping points
    double smooth = mandelbrot_escape_time(0.3, 0.0, 200);
    double frac = smooth - std::floor(smooth);
    TEST(frac > 0.0);

    // c = 0.25 is on the boundary — should reach max_iter
    TEST(mandelbrot_escape_time(0.25, 0.0, 200) == 200.0);
}

// --- Coordinate mapping tests ---
static void test_coordinate_mapping() {
    View v;
    v.centre_re = -0.5;
    v.centre_im = 0.0;
    v.vertical_span = 3.0;
    v.cell_ratio = 0.6;

    // For a 80x24 terminal, horizontal span = 3.0 * 80 / 24 * 0.6 = 6.0
    double hspan = v.horizontal_span(80, 24);
    TEST_NEAR(hspan, 6.0, 0.001);

    // Centre pixel should map to centre
    double cre, cim;
    v.pixel_to_complex(40, 12, 80, 24, cre, cim);
    TEST_NEAR(cre, -0.5, 0.05);
    TEST_NEAR(cim, 0.0, 0.05);

    // Left edge should be centre - hspan/2 = -3.5
    v.pixel_to_complex(0, 12, 80, 24, cre, cim);
    TEST_NEAR(cre, -3.5, 0.1);

    // Top edge should be centre - vspan/2 = -1.5
    v.pixel_to_complex(40, 0, 80, 24, cre, cim);
    TEST_NEAR(cim, -1.5, 0.1);
}

// --- Zoom-to-point tests ---
static void test_zoom_to_point() {
    View v;
    v.centre_re = -0.5;
    v.centre_im = 0.0;
    v.vertical_span = 3.0;
    v.cell_ratio = 0.6;

    // Zoom 2x into centre of 80x24 terminal
    double old_centre_re = v.centre_re;
    double old_centre_im = v.centre_im;
    v.zoom_to_point(40, 12, 80, 24, 2.0);

    // Centre should stay approximately the same (zooming into centre)
    TEST_NEAR(v.centre_re, old_centre_re, 0.1);
    TEST_NEAR(v.centre_im, old_centre_im, 0.1);
    TEST_NEAR(v.vertical_span, 1.5, 0.01);

    // Zoom into top-left corner — centre should shift up and left
    View v2;
    v2.vertical_span = 3.0;
    v2.zoom_to_point(0, 0, 80, 24, 2.0);
    TEST(v2.centre_re < -1.0); // FIXED: Actual meaningful comparison (was tautology!)
    TEST(v2.vertical_span == 1.5);
}

int main() {
    test_escape_time();
    test_coordinate_mapping();
    test_zoom_to_point();

    std::cout << "fractal_test: " << tests_passed << " passed, "
              << tests_failed << " failed.\n";
    return tests_failed > 0 ? 1 : 0;
}
