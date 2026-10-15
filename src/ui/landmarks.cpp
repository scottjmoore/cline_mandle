#include "landmarks.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstdio>

namespace {

const std::vector<Landmark> s_landmarks = {
    // name, centre_re, centre_im, vertical_span
    {"Seahorse Valley",       -0.75,  0.0,       0.15},
    {"Eel (Mini Mandelbrot)", -0.74367, 0.131402, 0.003},
    {"Spiral Arm",            -0.7463, -0.10806, 0.002},
    {"Elephant Valley",       0.275,  0.0,       0.4},
    {"Lightning Bolt",        -0.1967, 0.0071,   0.0006},
    {"Sceptre",               -0.082, -0.658,    0.015},
    {"Triple Spiral",         -0.089,  0.654,    0.02},
    {"Mini Seahorses",        -1.25066, 0.02012, 0.0008},
    {"Whirlpool",             -0.10108, 0.04507, 0.0012},
    {"Coral Reef",            -0.23512, 0.81738, 0.003},
};

} // anonymous namespace

const std::vector<Landmark>& get_landmarks() {
    return s_landmarks;
}

int find_landmark(const std::string& prefix) {
    if (prefix.empty()) return -1;
    std::string lower = prefix;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    for (int i = 0; i < static_cast<int>(s_landmarks.size()); ++i) {
        std::string name = s_landmarks[i].name;
        std::string lower_name = name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (lower_name.rfind(lower, 0) == 0) { // starts with
            return i;
        }
    }
    return -1;
}

bool apply_landmark(const std::vector<Landmark>& list, int idx, View& view) {
    if (idx < 0 || idx >= static_cast<int>(list.size())) return false;
    const auto& lm = list[idx];
    view.centre_re = lm.centre_re;
    view.centre_im = lm.centre_im;
    view.vertical_span = lm.vertical_span;
    return true;
}

std::vector<std::string> render_landmark_overlay(
    uint32_t cols, uint32_t rows,
    const std::string& filter, int selected_idx) {

    const auto& list = get_landmarks();
    std::vector<std::string> result;
    result.reserve(rows);

    // Collect matching indices
    std::vector<int> matches;
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
        if (filter.empty()) {
            matches.push_back(i);
        } else {
            std::string name = list[i].name;
            std::string lower_name = name;
            std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            std::string lower_filter = filter;
            std::transform(lower_filter.begin(), lower_filter.end(), lower_filter.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            if (lower_name.rfind(lower_filter, 0) == 0) {
                matches.push_back(i);
            }
        }
    }

    // Header row
    {
        char buf[256];
        std::snprintf(buf, sizeof(buf), " \x1b[1mLandmarks\x1b[0m  (type to filter: %s) [Enter] select [Esc] cancel",
                      filter.c_str());
        result.emplace_back(buf);
    }

    // Data rows
    uint32_t data_rows = rows > 1 ? rows - 1 : 0;
    for (uint32_t r = 0; r < data_rows && r < static_cast<uint32_t>(matches.size()); ++r) {
        int idx = matches[r];
        const auto& lm = list[idx];
        char buf[256];
        if (static_cast<int>(r) == selected_idx) {
            std::snprintf(buf, sizeof(buf), " \x1b[7m> %-24s  (%.6f, %.6fi)  span %.6f\x1b[0m",
                          lm.name, lm.centre_re, lm.centre_im, lm.vertical_span);
        } else {
            std::snprintf(buf, sizeof(buf), "  %-24s  (%.6f, %.6fi)  span %.6f",
                          lm.name, lm.centre_re, lm.centre_im, lm.vertical_span);
        }
        // Pad to terminal width
        std::string line(buf);
        while (static_cast<uint32_t>(line.size()) < cols)
            line += ' ';
        result.push_back(line);
    }

    // Fill remaining rows with blanks
    while (static_cast<uint32_t>(result.size()) < rows) {
        std::string blank(cols, ' ');
        result.push_back(blank);
    }

    return result;
}
