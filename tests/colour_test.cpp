// Colour unit tests: palette mapping, dithering determinism
#include <cassert>
#include <iostream>
#include <set>
#include "palette.h"
#include "dither.h"
#include "mandelbrot.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(cond) do { \
    if (cond) { ++tests_passed; } \
    else { std::cerr << "FAIL at " << __LINE__ << "\n"; ++tests_failed; } \
} while(0)

// --- Palette tests ---
static void test_palettes() {
    // Each palette should have 256 entries
    for (int i = 0; i < palette_count(); ++i) {
        PaletteId id = static_cast<PaletteId>(i);
        Palette p = get_palette(id);
        TEST(static_cast<int>(p.size()) == 256);

        // All entries should be valid ANSI indices (0-255)
        for (auto val : p) {
            TEST(val <= 255);
        }

        // Palette name should not be empty
        const char* name = palette_name(id);
        TEST(name != nullptr && name[0] != '\0');
    }
}

// --- Dithering tests ---
static void test_dithering() {
    Palette p = get_palette(PaletteId::Thermal);

    // Inside-set point should produce black + space
    uint8_t colour;
    const char* glyph;
    dither_map(200.0, 200, p, 0, 0, colour, glyph);
    TEST(colour == 0);
    TEST(glyph && glyph[0] == ' ' && glyph[1] == '\0');

    // Determinism: same inputs produce same outputs
    uint8_t c1, c2;
    const char* g1, *g2;
    dither_map(50.5, 200, p, 5, 3, c1, g1);
    dither_map(50.5, 200, p, 5, 3, c2, g2);
    TEST(c1 == c2);
    TEST(g1 == g2);

    // Glyph should be from the ramp
    bool found = false;
    for (std::size_t i = 0; i < DITHER_RAMP_LEN; ++i) {
        if (DITHER_RAMP[i] == g1) { found = true; break; }
    }
    TEST(found);

    // Different Bayer positions may produce different results
    dither_map(50.5, 200, p, 0, 0, c1, g1);
    dither_map(50.5, 200, p, 1, 1, c2, g2);
    // They might be the same or different — just verify no crash
    TEST(c1 <= 255 && c2 <= 255);
}

// --- Smooth iteration values ---
static void test_smooth_iteration() {
    // Points well inside set
    TEST(mandelbrot_escape_time(0.0, 0.0, 100) == 100.0);

    // Points well outside
    double val = mandelbrot_escape_time(10.0, 10.0, 100);
    TEST(val < 20.0);

    // Smooth values should be >= 0
    for (int i = 0; i < 50; ++i) {
        double v = mandelbrot_escape_time(
            -2.0 + (i * 4.0) / 50.0,
            -1.5 + (i * 3.0) / 50.0,
            100);
        TEST(v >= 0.0);
    }
}

int main() {
    test_palettes();
    test_dithering();
    test_smooth_iteration();

    std::cout << "colour_test: " << tests_passed << " passed, "
              << tests_failed << " failed.\n";
    return tests_failed > 0 ? 1 : 0;
}
