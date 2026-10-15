#include "status_bar.h"
#include <sstream>
#include <cstdio>
#include <cmath>

std::string status_bar(const View& view, uint32_t cols, uint32_t /*rows*/,
                       uint32_t max_iter, PaletteId palette_id) {
    char buf[512];
    bool zoomed = is_zoom_limit_reached(view);
    std::snprintf(buf, sizeof(buf),
        " Centre: (%.6f, %.6fi)  Span: %.6f  Iter: %u  Palette: %s  "
        "[Arrows] pan  [+]/-] zoom  [p] palette  [i] iterations  "
        "[r] reset  [h] help  [q] quit",
        view.centre_re, view.centre_im,
        view.vertical_span,
        max_iter,
        palette_name(palette_id));

    // Pad to terminal width
    std::string line(buf);
    while (static_cast<uint32_t>(line.size()) < cols)
        line += ' ';

    // Yellow on dark background (red background if zoom-limit warning)
    if (zoomed) {
        return "\x1b[41m\x1b[33m " + line + " \x1b[0m";
    }
    return "\x1b[44m\x1b[33m " + line + " \x1b[0m";
}

std::string status_bar_rendering(const View& view, uint32_t cols,
                                 uint32_t rows,
                                 uint32_t current_row,
                                 uint32_t max_iter,
                                 PaletteId palette_id) {
    char buf[512];
    uint32_t total = rows;
    uint32_t pct = total > 0 ? (current_row * 100) / total : 0;
    std::snprintf(buf, sizeof(buf),
        " Rendering... row %u/%u (%u%%)  Centre: (%.6f, %.6fi)  Span: %.6f  "
        "Iter: %u  Palette: %s",
        current_row, total, pct,
        view.centre_re, view.centre_im,
        view.vertical_span,
        max_iter,
        palette_name(palette_id));

    std::string line(buf);
    while (static_cast<uint32_t>(line.size()) < cols)
        line += ' ';

    return "\x1b[44m\x1b[33m " + line + " \x1b[0m";
}

bool is_zoom_limit_reached(const View& view) {
    // Double-precision has ~15-16 significant decimal digits.
    // When the span drops below ~1e-12, further zooming produces
    // visible artefacts (pixel noise / banding).
    constexpr double ZOOM_LIMIT = 1e-12;
    return view.vertical_span < ZOOM_LIMIT;
}
