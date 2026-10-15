#ifndef COLOUR_RENDER_H
#define COLOUR_RENDER_H

#include <string>
#include <cstdint>
#include "view.h"
#include "palette.h"

/// Render one row of the Mandelbrot set as an ANSI-escaped string.
///
/// @param view       current view state
/// @param row        terminal row index (0-based)
/// @param cols       terminal column count
/// @param rows       terminal row count
/// @param max_iter   iteration limit
/// @param palette    active palette
/// @return           a std::string containing the row with SGR colour codes
std::string render_row(const View& view, uint32_t row,
                       uint32_t cols, uint32_t rows,
                       uint32_t max_iter, const Palette& palette);

#endif // COLOUR_RENDER_H
