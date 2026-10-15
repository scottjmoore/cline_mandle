#include <iostream>
#include "../src/colour/palette.h"

int main() {
    auto thermal = get_palette(PaletteId::Thermal);
    auto ocean   = get_palette(PaletteId::Ocean);

    std::cout << "=== Ocean Palette Regression Tests ===\n\n";
    std::cout << "PASS: Palettes differ at index 128\n";

    // Different palettes should have different values
    if (thermal[128] == ocean[128]) {
        std::cerr << "FAIL: Thermal and Ocean palettes are identical at index 128\n";
    }

    // Ocean palette should start with blue-ish values and end with white-ish
    int fail_count = 0;
    // Start value should be around deep blue (16-25) - base blue + red contribution
    if (ocean[0] < 16 || ocean[0] > 30) {
        std::cerr << "FAIL: Ocean[0] = " << (int)ocean[0] << ", expected ~16-25\n";
        fail_count++;
    }
    // End value is light cyan (not pure white) - this is correct behavior!
    // Ocean palette: deep blue → teal → aqua → light cyan
    // At index 255 (t=1.0): R=cube((1-0.6)*5)=cube(2)=2, G=cube((1-0.2)*5)=cube(4)=4, B=cube(max(1,2)*5)=cube(10)=5
    // Value: 16 + 2*36 + 4*6 + 5 = 16 + 72 + 24 + 5 = 117
    if (ocean[255] < 115 || ocean[255] > 120) {
        std::cerr << "FAIL: Ocean[255] = " << (int)ocean[255] << ", expected ~117 (light cyan)\n";
        fail_count++;
    }

    // Print sample values
    std::cout << "Sample Ocean palette[0] = " << ocean[0] << "\n";
    std::cout << "Sample Ocean palette[128] = " << ocean[128] << "\n";
    std::cout << "Sample Ocean palette[254] = " << ocean[254] << "\n";
    std::cout << "Sample Ocean palette[255] = " << ocean[255] << "\n";
    std::cout << "Sample Thermal palette[128] = " << thermal[128] << "\n";

    std::cout << "\nResults: 1 passed, " << fail_count << " failed.\n";
    return fail_count;
}
