#ifndef FRACTAL_VIEW_H
#define FRACTAL_VIEW_H

#include <cstdint>

/// View state: centre in the complex plane and vertical span.
struct View {
    double centre_re;
    double centre_im;
    double vertical_span;
    double cell_ratio; // assumed terminal cell width / height

    View()
        : centre_re(-0.5), centre_im(0.0),
          vertical_span(3.0), cell_ratio(0.6) {}

    /// Horizontal span derived from terminal dimensions and cell ratio.
    double horizontal_span(uint32_t cols, uint32_t rows) const {
        return vertical_span * static_cast<double>(cols)
               / static_cast<double>(rows) * cell_ratio;
    }

    /// Map terminal column/row to complex-plane coordinate.
    void pixel_to_complex(uint32_t col, uint32_t row,
                          uint32_t cols, uint32_t rows,
                          double& out_re, double& out_im) const;

    /// Zoom towards a specific terminal position by the given factor.
    void zoom_to_point(uint32_t col, uint32_t row,
                       uint32_t cols, uint32_t rows,
                       double factor);
};

#endif // FRACTAL_VIEW_H
