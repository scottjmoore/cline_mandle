#ifndef COLOUR_PALETTE_H
#define COLOUR_PALETTE_H

#include <cstdint>
#include <array>

/// 256-entry palette: each entry is an ANSI colour index (0-255).
using Palette = std::array<uint8_t, 256>;

/// Built-in palette names.
enum class PaletteId {
    Thermal,
    Electric,
    Grayscale,
    Ocean,
    COUNT
};

/// Return a named palette.
Palette get_palette(PaletteId id);

/// Return the count of built-in palettes.
constexpr int palette_count() { return static_cast<int>(PaletteId::COUNT); }

/// Return the display name of a palette.
const char* palette_name(PaletteId id);

#endif // COLOUR_PALETTE_H
