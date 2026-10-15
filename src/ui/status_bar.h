#ifndef UI_STATUS_BAR_H
#define UI_STATUS_BAR_H

#include <string>
#include <cstdint>
#include "view.h"
#include "palette.h"

/// Render the status/help strip as a single-line string.
std::string status_bar(const View& view, uint32_t cols, uint32_t rows,
                       uint32_t max_iter, PaletteId palette_id);

/// Render a progressive "rendering..." status bar showing row progress.
std::string status_bar_rendering(const View& view, uint32_t cols, uint32_t rows,
                                uint32_t current_row, uint32_t max_iter,
                                PaletteId palette_id);

/// Return true if the view is zoomed past the safe double-precision limit.
bool is_zoom_limit_reached(const View& view);

#endif // UI_STATUS_BAR_H
