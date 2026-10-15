#include "render.h"
#include "mandelbrot.h"
#include "dither.h"
#include <sstream>
#include <cstdio>

std::string render_row(const View& view, uint32_t row,
                       uint32_t cols, uint32_t rows,
                       uint32_t max_iter, const Palette& palette) {
    std::ostringstream oss;

    for (uint32_t col = 0; col < cols; ++col) {
        double cre, cim;
        view.pixel_to_complex(col, row, cols, rows, cre, cim);

        double smooth = mandelbrot_escape_time(cre, cim, max_iter);

        uint8_t ansi_colour;
        const char* glyph;
        dither_map(smooth, max_iter, palette, col, row, ansi_colour, glyph);

        // Emit SGR colour code then the glyph
        oss << "\x1b[38;5;" << static_cast<int>(ansi_colour) << "m" << glyph;
    }
    // Reset colour at end of row
    oss << "\x1b[0m\n";
    return oss.str();
}
