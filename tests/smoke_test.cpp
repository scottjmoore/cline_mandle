// Smoke test: render a small Mandelbrot frame without terminal setup
#include <iostream>
#include <string>
#include "view.h"
#include "palette.h"
#include "render.h"

int main() {
    constexpr uint32_t ROWS = 20;
    constexpr uint32_t COLS = 40;
    constexpr uint32_t MAX_ITER = 100;

    View view;
    Palette palette = get_palette(PaletteId::Thermal);

    std::cout << "# Mandelbrot Smoke Test: " << COLS << "x" << ROWS
              << " iter=" << MAX_ITER << "\n";

    bool all_rows_ok = true;
    for (uint32_t r = 0; r < ROWS; ++r) {
        std::string row_str = render_row(view, r, COLS, ROWS, MAX_ITER, palette);
        if (row_str.empty()) {
            std::cerr << "Row " << r << " is empty!\n";
            all_rows_ok = false;
        }
        std::cout << row_str;
    }

    if (!all_rows_ok) {
        std::cerr << "Smoke test FAILED: some rows were empty.\n";
        return 1;
    }

    std::cout << "# Smoke test PASSED.\n";
    return 0;
}
