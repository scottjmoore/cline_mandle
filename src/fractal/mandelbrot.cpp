#include "mandelbrot.h"
#include <cmath>
#include <cfloat>

double mandelbrot_escape_time(double cre, double cim, uint32_t max_iter) {
    double zr = 0.0;
    double zi = 0.0;
    double zr2 = 0.0;
    double zi2 = 0.0;

    for (uint32_t n = 0; n < max_iter; ++n) {
        // z = z^2 + c  (expand to avoid complex multiplication)
        zi = 2.0 * zr * zi + cim;
        zr = zr2 - zi2 + cre;
        zr2 = zr * zr;
        zi2 = zi * zi;
        double r2 = zr2 + zi2;
        if (r2 > 4.0) {
            // Smooth iteration count: n + 1 - log2(log2(|z|))
            double log_zn = std::log(r2 * 0.25);
            double nu     = std::log(log_zn) / std::log(2.0);
            return static_cast<double>(n) + 1.0 - nu;
        }
    }
    return static_cast<double>(max_iter);
}
