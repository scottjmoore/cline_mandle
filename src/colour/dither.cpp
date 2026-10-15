#include "dither.h"
#include <cmath>

/// Character density ramp (low → high) — each entry is a full UTF-8 string.
const char* const DITHER_RAMP[] = {
    "░", "█"
};

const std::size_t DITHER_RAMP_LEN = sizeof(DITHER_RAMP) / sizeof(DITHER_RAMP[0]);

const uint8_t BAYER[2][2] = {
    {0, 2},
    {3, 1}
};

void dither_map(double smooth_val, uint32_t max_iter,
                const Palette& palette,
                uint32_t col, uint32_t row,
                uint8_t& ansi_colour, const char*& glyph) {
    // Points inside the set
    if (smooth_val >= static_cast<double>(max_iter)) {
        ansi_colour = 0; // black
        glyph = " ";
        return;
    }

    // Map smooth value into palette range [0, 255].
    // Use a logarithmic scale so colour variation is visible regardless
    // of iteration count — most exterior points escape in far fewer
    // iterations than max_iter, so a linear mapping squashes them all
    // into the same colour.  Reverse the mapping so points far from the
    // set (low escape time) get bright colours, and points near the
    // boundary (high escape time) get dark colours.
    double log_t = std::log(smooth_val + 1.0) / std::log(static_cast<double>(max_iter) + 1.0);
    double t = 1.0 - log_t;
    double scaled = t * 256.0;

    int base = static_cast<int>(std::floor(scaled));
    double frac = scaled - std::floor(scaled);

    // Bayer dither: add threshold / 4.0 to fractional part
    int bayer_val = BAYER[row % 2][col % 2];
    double dithered = frac + static_cast<double>(bayer_val) / 4.0;

    int colour_idx = base;
    if (dithered >= 1.0) {
        colour_idx = base + 1;
        if (colour_idx > 255) colour_idx = 255;
    }

    // Ensure exterior points never map to pure black (index 0 → ANSI 16).
    // Black is reserved for points inside the set.
    if (colour_idx < 1) colour_idx = 1;

    ansi_colour = palette[colour_idx];

    // Glyph from ramp based on fractional part — ramp is an array of
    // full UTF-8 strings so multi-byte characters are handled correctly.
    int ramp_idx = static_cast<int>(dithered * DITHER_RAMP_LEN);
    if (ramp_idx >= static_cast<int>(DITHER_RAMP_LEN)) ramp_idx = static_cast<int>(DITHER_RAMP_LEN) - 1;
    glyph = DITHER_RAMP[ramp_idx];
}
