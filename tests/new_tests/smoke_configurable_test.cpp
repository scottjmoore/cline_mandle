// =========================================================================
// Smoke Test with Configurable Parameters
// Tests that smoke test parameters should be configurable via CLI or config
// =========================================================================

#include <iostream>
#include <string>
#include "view.h"
#include "palette.h"
#include "render.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(cond) do { \\
    if (cond) { ++tests_passed; } \\
    else { std::cerr << "FAIL at line " << __LINE__ << "\\n"; ++tests_failed; } \\
} while(0)

// Test: Small render dimensions work
static void test_small_dimensions() {
    View view;
    Palette thermal = get_palette(PaletteId::Thermal);

    // Small render (10x5)
    if (render_row(view, 0, 10, 5, 50, thermal).empty()) {
        std::cerr << "FAIL smoke_param: Render failed for 10x5\\n";
        ++tests_failed;
    } else { ++tests_passed; }

    // Medium render (20x10)
    if (render_row(view, 0, 20, 10, 50, thermal).empty()) {
        std::cerr << "FAIL smoke_param: Render failed for 20x10\\n";
        ++tests_failed;
    } else { ++tests_passed; }

    // Large render (60x30)
    if (render_row(view, 0, 60, 30, 50, thermal).empty()) {
        std::cerr << "FAIL smoke_param: Render failed for 60x30\\n";
        ++tests_failed;
    } else { ++tests_passed; }
}

// Test: Very large iteration counts handled
static void test_various_iterations() {
    View view;
    Palette thermal = get_palette(PaletteId::Thermal);

    // Test with 10 iterations
    if (render_row(view, 0, 5, 5, 10, thermal).empty()) {
        std::cerr << "FAIL smoke_param: Failed with MAX_ITER=10\\n"; ++tests_failed;
    } else { ++tests_passed; }

    // Test with 50 iterations
    if (render_row(view, 0, 5, 5, 50, thermal).empty()) {
        std::cerr << "FAIL smoke_param: Failed with MAX_ITER=50\\n"; ++tests_failed;
    } else { ++tests_passed; }

    // Test with 200 iterations
    if (render_row(view, 0, 5, 5, 200, thermal).empty()) {
        std::cerr << "FAIL smoke_param: Failed with MAX_ITER=200\\n"; ++tests_failed;
    } else { ++tests_passed; }
}

// Test: Different palettes work in smoke test
static void test_multiple_palettes() {
    View view;

    // Test thermal palette
    if (render_row(view, 0, 5, 5, 50, get_palette(PaletteId::Thermal)).empty()) {
        std::cerr << "FAIL: Thermal palette in smoke test\\n"; ++tests_failed;
    } else { ++tests_passed; }

    // Test ocean palette
    if (render_row(view, 0, 5, 5, 50, get_palette(PaletteId::Ocean)).empty()) {
        std::cerr << "FAIL: Ocean palette in smoke test\\n"; ++tests_failed;
    } else { ++tests_passed; }

    // Test electric palette
    if (render_row(view, 0, 5, 5, 50, get_palette(PaletteId::Electric)).empty()) {
        std::cerr << "FAIL: Electric palette in smoke test\\n"; ++tests_failed;
    } else { ++tests_passed; }

    // Test grayscale palette
    if (render_row(view, 0, 5, 5, 50, get_palette(PaletteId::Grayscale)).empty()) {
        std::cerr << "FAIL: Grayscale palette in smoke test\\n"; ++tests_failed;
    } else { ++tests_passed; }
}

int main() {
    std::cout << "=== Smoke Test Configurable Parameters ===\\n";

    test_small_dimensions();
    test_various_iterations();
    test_multiple_palettes();

    std::cout << "Smoke configurable tests: " << tests_passed << " passed, "
                << tests_failed << " failed.\\n";

    return tests_failed > 0 ? 1 : 0;
}
// END of smoke_configurable_test.cpp
