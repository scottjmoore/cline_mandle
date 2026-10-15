#ifndef COLOUR_DITHER_H
#define COLOUR_DITHER_H

#include <cstdint>
#include <cstddef>
#include "palette.h"

/// Number of glyphs in the dither ramp.
extern const std::size_t DITHER_RAMP_LEN;

/// Character density ramp (low → high) — each entry is a full UTF-8 string.
extern const char* const DITHER_RAMP[];

/// Deterministic 2×2 Bayer threshold matrix (values 0-3).
extern const uint8_t BAYER[2][2];

/// Map a smooth escape-time value to an ANSI colour index and a dither
/// glyph from the ramp.  The colour index comes from the palette; the
/// glyph is chosen from the fractional remainder so repeated renders
/// of the same view produce identical output.
///
/// @param smooth_val  smooth iteration count from mandelbrot_escape_time
/// @param max_iter    iteration limit used for the render
/// @param palette     the active palette
/// @param col         terminal column (for Bayer phase)
/// @param row         terminal row (for Bayer phase)
/// @param[out] ansi_colour  resulting 256-colour index (0-255)
/// @param[out] glyph        UTF-8 string to print
void dither_map(double smooth_val, uint32_t max_iter,
                const Palette& palette,
                uint32_t col, uint32_t row,
                uint8_t& ansi_colour, const char*& glyph);

#endif // COLOUR_DITHER_H
