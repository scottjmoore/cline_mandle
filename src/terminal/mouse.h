#ifndef TERMINAL_MOUSE_H
#define TERMINAL_MOUSE_H

#include <cstdint>

/// Bounds computed from a drag-selection rectangle.
struct DragBounds {
    uint32_t col_min;
    uint32_t col_max;
    uint32_t row_min;
    uint32_t row_max;
    uint32_t span_cols;
    uint32_t span_rows;

    /// True if the drag is large enough to warrant a zoom.
    bool is_significant() const {
        return span_cols > 2 || span_rows > 2;
    }

    /// Zoom factor derived from the fraction of screen covered.
    double zoom_factor(uint32_t total_cols, uint32_t total_rows) const {
        if (!is_significant()) return 1.0;
        double frac_cols = static_cast<double>(span_cols) / static_cast<double>(total_cols);
        double frac_rows = static_cast<double>(span_rows) / static_cast<double>(total_rows);
        double frac = (frac_cols > frac_rows) ? frac_cols : frac_rows;
        if (frac > 0.01 && frac < 1.0) return 1.0 / frac;
        return 1.0;
    }

    /// Centre column of the selection.
    uint32_t centre_col() const { return (col_min + col_max) / 2; }

    /// Centre row of the selection.
    uint32_t centre_row() const { return (row_min + row_max) / 2; }
};

/// Compute drag bounds from start and end coordinates.
DragBounds compute_drag_bounds(uint32_t col_start, uint32_t row_start,
                               uint32_t col_end, uint32_t row_end);

/// Clamp a mouse coordinate to the drawing area.
inline uint32_t clamp_mouse(uint32_t val, uint32_t max) {
    return (val >= max) ? (max > 0 ? max - 1 : 0) : val;
}

#endif // TERMINAL_MOUSE_H
