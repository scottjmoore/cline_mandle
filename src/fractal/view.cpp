#include "view.h"

void View::pixel_to_complex(uint32_t col, uint32_t row,
                            uint32_t cols, uint32_t rows,
                            double& out_re, double& out_im) const {
    double hspan = horizontal_span(cols, rows);
    // Left edge
    double left = centre_re - hspan / 2.0;
    // Top edge
    double top  = centre_im - vertical_span / 2.0;
    out_re = left + static_cast<double>(col) * (hspan / static_cast<double>(cols));
    out_im = top  + static_cast<double>(row) * (vertical_span / static_cast<double>(rows));
}

void View::zoom_to_point(uint32_t col, uint32_t row,
                         uint32_t cols, uint32_t rows,
                         double factor) {
    double point_re, point_im;
    pixel_to_complex(col, row, cols, rows, point_re, point_im);
    centre_re = centre_re + (point_re - centre_re) * (1.0 - 1.0 / factor);
    centre_im = centre_im + (point_im - centre_im) * (1.0 - 1.0 / factor);
    vertical_span /= factor;
}
