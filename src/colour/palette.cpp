#include "palette.h"
#include <cmath>

namespace {

/// Build a palette by mapping index i through a colour function.
void fill_palette(Palette& p, uint8_t (*fn)(int)) {
    for (int i = 0; i < 256; ++i)
        p[i] = fn(i);
}

/// Clamp a double into the ANSI 6×6×6 colour-cube range [0, 5].
static uint8_t cube(double v) {
    return static_cast<uint8_t>(
        std::min(5.0, std::max(0.0, std::round(v))));
}

/// Thermal: black → red → yellow → white
uint8_t thermal_fn(int i) {
    double t = static_cast<double>(i) / 255.0;
    uint8_t r = cube(t * 5.0);
    uint8_t g = cube(std::max(0.0, (t - 0.33) * 5.0));
    uint8_t b = cube(std::max(0.0, (t - 0.66) * 5.0));
    return static_cast<uint8_t>(16 + r * 36 + g * 6 + b);
}

/// Electric: blue → cyan → green → yellow
uint8_t electric_fn(int i) {
    double t = static_cast<double>(i) / 255.0;
    uint8_t r = cube(std::max(0.0, (t - 0.5) * 5.0));
    uint8_t g = cube(t * 5.0);
    uint8_t b = cube((1.0 - t) * 5.0);
    return static_cast<uint8_t>(16 + r * 36 + g * 6 + b);
}

/// Grayscale ramp (ANSI indices 232-255, extended by repeating)
uint8_t grayscale_fn(int i) {
    return static_cast<uint8_t>(232 + (i * 24) / 256);
}

/// Ocean: deep blue → teal → aqua → white
uint8_t ocean_fn(int i) {
    double t = static_cast<double>(i) / 255.0;
    uint8_t r = cube(std::max(0.0, (t - 0.6) * 5.0));
    uint8_t g = cube(std::max(0.0, (t - 0.2) * 5.0));
    uint8_t b = cube(std::max(0.0, std::max(1.0, t * 2.0) * 5.0));
    return static_cast<uint8_t>(16 + r * 36 + g * 6 + b);
}

} // anonymous namespace

Palette get_palette(PaletteId id) {
    Palette p{};
    switch (id) {
        case PaletteId::Thermal:   fill_palette(p, thermal_fn);   break;
        case PaletteId::Electric:  fill_palette(p, electric_fn);  break;
        case PaletteId::Grayscale: fill_palette(p, grayscale_fn); break;
        case PaletteId::Ocean:     fill_palette(p, ocean_fn);     break;
        default:                  fill_palette(p, thermal_fn);   break;
    }
    return p;
}

const char* palette_name(PaletteId id) {
    switch (id) {
        case PaletteId::Thermal:   return "thermal";
        case PaletteId::Electric:  return "electric";
        case PaletteId::Grayscale: return "grayscale";
        case PaletteId::Ocean:     return "ocean";
        default:                  return "thermal";
    }
}
