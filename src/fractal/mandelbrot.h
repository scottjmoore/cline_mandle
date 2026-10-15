#ifndef FRACTAL_MANDELBROT_H
#define FRACTAL_MANDELBROT_H

#include <cstdint>

/// Compute the smooth escape-time value for point c = cre + i*cim.
///
/// Returns the iteration count at which the orbit escaped radius 2,
/// plus a fractional component for smooth colouring.
/// If the point did not escape within max_iter, returns max_iter.
double mandelbrot_escape_time(double cre, double cim, uint32_t max_iter);

#endif // FRACTAL_MANDELBROT_H
