#ifndef UI_LANDMARKS_H
#define UI_LANDMARKS_H

#include <string>
#include <vector>
#include <cstdint>
#include "view.h"

/// A named location in the Mandelbrot set.
struct Landmark {
    const char* name;
    double centre_re;
    double centre_im;
    double vertical_span;
};

/// Return the built-in landmark list.
const std::vector<Landmark>& get_landmarks();

/// Find a landmark by name prefix (case-insensitive).
/// Returns index or -1 if not found.
int find_landmark(const std::string& prefix);

/// Apply landmark at index to the view. Returns false if index is invalid.
bool apply_landmark(const std::vector<Landmark>& list, int idx, View& view);

/// Render the landmark selector overlay as ANSI strings (one per row).
/// Returns a vector of strings, one per visible row.
/// If filter is non-empty, only landmarks whose name starts with filter are shown.
/// selected_idx is the currently highlighted entry.
std::vector<std::string> render_landmark_overlay(
    uint32_t cols, uint32_t rows,
    const std::string& filter, int selected_idx);

#endif // UI_LANDMARKS_H
