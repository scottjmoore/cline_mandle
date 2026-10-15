#include "app.h"
#include "view.h"
#include "palette.h"
#include "render.h"
#include "terminal.h"
#include "input.h"
#include "status_bar.h"
#include "landmarks.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

namespace {

/// Iteration limits the user can cycle through.
const uint32_t ITER_STEPS[] = {50, 100, 200, 500, 1000, 2000, 5000};
const int ITER_STEPS_COUNT = static_cast<int>(std::size(ITER_STEPS));

int find_iter_step(uint32_t val) {
    for (int i = 0; i < ITER_STEPS_COUNT; ++i) {
        if (ITER_STEPS[i] == val) return i;
    }
    return 2; // default to 200
}

/// Drag-selection state for mouse interaction.
struct DragState {
    bool active = false;
    uint32_t col_start = 0;
    uint32_t row_start = 0;
    uint32_t col_end = 0;
    uint32_t row_end = 0;
};

} // anonymous namespace

// Forward declaration from terminal.cpp
extern bool terminal_should_quit();

int run_app(const CliArgs& args) {
    // --- Smoke-test mode: render a fixed frame to stdout, no terminal setup ---
    if (args.smoke_test) {
        View view;
        Palette palette = get_palette(args.palette);
        uint32_t rows = args.smoke_rows;
        uint32_t cols = args.smoke_cols;

        std::cout << "# Mandelbrot Smoke Test: " << cols << "x" << rows
                  << " iter=" << args.max_iter
                  << " palette=" << palette_name(args.palette) << "\n";

        for (uint32_t r = 0; r < rows; ++r) {
            // Force flush after each row to ensure proper display for any dimensions
            std::cout << render_row(view, r, cols, rows, args.max_iter, palette);
        }
        // Ensure final line is flushed
        std::cout.flush();
        std::cout << "# Smoke test complete.\n";
        return 0;
    }

    // --- Interactive mode ---
    if (!terminal_init()) {
        std::cerr << "Error: stdout is not a terminal. Use --smoke for non-interactive mode.\n";
        return 1;
    }

    View view;
    // Apply CLI landmark if specified
    if (!args.landmark.empty()) {
        int lm_idx = find_landmark(args.landmark);
        if (lm_idx < 0) {
            std::cerr << "Warning: landmark \"" << args.landmark << "\" not found.\n";
        } else {
            apply_landmark(get_landmarks(), lm_idx, view);
        }
    }

    PaletteId palette_id = args.palette;
    int iter_step = find_iter_step(args.max_iter);
    uint32_t max_iter = ITER_STEPS[iter_step];

    auto render_frame = [&]() {
        TermSize ts = terminal_size();
        terminal_clear();

        Palette palette = get_palette(palette_id);
        // Reserve one row for the status bar (adjusts for resize)
        uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;

        // Position cursor on status-bar row and show "rendering..." indicator
        terminal_cursor_move(0, ts.rows - 1);
        terminal_write(status_bar_rendering(view, ts.cols, draw_rows, 0,
                                           max_iter, palette_id));
        // After writing status bar (no trailing \n), cursor is still on status row.
        // Move it back to row 0 so fractal rendering starts at the top.
        terminal_cursor_move(0, 0);

        for (uint32_t r = 0; r < draw_rows; ++r) {
            terminal_write(render_row(view, r, ts.cols, draw_rows, max_iter, palette));
            // Update progressive status bar every 5 rows (and on first/last)
            if (r % 5 == 0 || r == draw_rows - 1) {
                terminal_cursor_move(0, ts.rows - 1);
                terminal_write(status_bar_rendering(view, ts.cols, draw_rows, r + 1,
                                                   max_iter, palette_id));
                // Re-position cursor for the next fractal row.
                // Note: status_bar_rendering does NOT emit '\n', so cursor stays at ts.rows-1.
                // We need to move back to the NEXT fractal row (r+1), not r+2.
                uint32_t next_row = r + 1;  // Move back one from where we left off
                if (next_row < draw_rows) {
                    terminal_cursor_move(0, next_row);
                }
            }
        }

        // Final status bar on last row
        terminal_cursor_move(0, ts.rows - 1);
        terminal_write(status_bar(view, ts.cols, draw_rows, max_iter, palette_id));
    };

    render_frame();

    // Main event loop
    while (!terminal_should_quit()) {
        auto ev = input_read();
        if (!ev) continue;

        bool need_redraw = false;

        switch (ev->kind) {
            case InputKind::Resize: {
                // Terminal was resized - dimensions will be picked up on next render_frame()
                // Preserve view state (centre, span), redraw to use new terminal size
                need_redraw = true;
                break;
            }
            case InputKind::Quit:
                terminal_restore();
                return 0;
            case InputKind::Key: {
                // Pan: arrow keys move centre by 10% of span
                if (ev->key == "Up") {
                    view.centre_im -= view.vertical_span * 0.1;
                    need_redraw = true;
                } else if (ev->key == "Down") {
                    view.centre_im += view.vertical_span * 0.1;
                    need_redraw = true;
                } else if (ev->key == "Left") {
                    TermSize ts = terminal_size();
                    uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                    double hspan = view.horizontal_span(ts.cols, draw_rows);
                    view.centre_re -= hspan * 0.1;
                    need_redraw = true;
                } else if (ev->key == "Right") {
                    TermSize ts = terminal_size();
                    uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                    double hspan = view.horizontal_span(ts.cols, draw_rows);
                    view.centre_re += hspan * 0.1;
                    need_redraw = true;
                } else if (ev->key == "+" || ev->key == "=") {
                    TermSize ts = terminal_size();
                    uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                    view.zoom_to_point(ts.cols / 2, draw_rows / 2, ts.cols, draw_rows, 2.0);
                    need_redraw = true;
                } else if (ev->key == "-") {
                    TermSize ts = terminal_size();
                    uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                    view.zoom_to_point(ts.cols / 2, draw_rows / 2, ts.cols, draw_rows, 0.5);
                    need_redraw = true;
                } else if (ev->key == "r" || ev->key == "R") {
                    view = View{}; // reset to default
                    need_redraw = true;
                } else if (ev->key == "p" || ev->key == "P") {
                    int next = static_cast<int>(palette_id) + 1;
                    if (next >= palette_count()) next = 0;
                    palette_id = static_cast<PaletteId>(next);
                    need_redraw = true;
                } else if (ev->key == "i" || ev->key == "I") {
                    iter_step++;
                    if (iter_step >= ITER_STEPS_COUNT) iter_step = 0;
                    max_iter = ITER_STEPS[iter_step];
                    need_redraw = true;
                } else if (ev->key == "h" || ev->key == "H") {
                    // Brief help flash — just redraw (status bar has help text)
                    need_redraw = true;
                } else if (ev->key == "l" || ev->key == "L") {
                    // Landmark selector overlay
                    TermSize ts = terminal_size();
                    uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                    std::string filter;
                    int selected = 0;
                    bool landmark_picked = false;

                    while (!terminal_should_quit()) {
                        terminal_clear();
                        auto overlay = render_landmark_overlay(ts.cols, draw_rows, filter, selected);
                        for (uint32_t r = 0; r < draw_rows; ++r) {
                            terminal_cursor_move(0, r);
                            terminal_write(overlay[r]);
                        }
                        terminal_cursor_move(0, ts.rows - 1);
                        terminal_write(status_bar(view, ts.cols, draw_rows, max_iter, palette_id));

                        auto kev = input_read();
                        if (!kev) continue;

                        if (kev->kind == InputKind::Quit || kev->kind == InputKind::None) {
                            // Escape or none → dismiss
                            need_redraw = true;
                            landmark_picked = true;
                            break;
                        }

                        if (kev->kind == InputKind::Key) {
                            if (kev->key == "Up") {
                                if (selected > 0) --selected;
                            } else if (kev->key == "Down") {
                                ++selected;
                                // Clamp later based on matches
                            } else if (kev->key == "\r" || kev->key == "\n") {
                                // Enter: apply selected landmark
                                const auto& lm_list = get_landmarks();
                                // Find the selected match
                                std::vector<int> matches;
                                for (int i = 0; i < static_cast<int>(lm_list.size()); ++i) {
                                    if (filter.empty()) {
                                        matches.push_back(i);
                                    } else {
                                        std::string nm = lm_list[i].name;
                                        std::string ln = nm;
                                        std::transform(ln.begin(), ln.end(), ln.begin(),
                                                       [](unsigned char c){ return std::tolower(c); });
                                        std::string lf = filter;
                                        std::transform(lf.begin(), lf.end(), lf.begin(),
                                                       [](unsigned char c){ return std::tolower(c); });
                                        if (ln.rfind(lf, 0) == 0) matches.push_back(i);
                                    }
                                }
                                if (selected < static_cast<int>(matches.size())) {
                                    apply_landmark(lm_list, matches[selected], view);
                                    need_redraw = true;
                                    landmark_picked = true;
                                }
                                break;
                            } else if (kev->key.size() == 1 && kev->key[0] >= 32 && kev->key[0] < 127) {
                                // Type to filter
                                filter += kev->key;
                                selected = 0;
                            }
                        }
                    }
                    if (landmark_picked) break;
                }
                break;
            }
            case InputKind::MouseWheel: {
                // Wheel zoom: up=zoom in, down=zoom out (factor 1.5×)
                TermSize ts = terminal_size();
                uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                // Clamp mouse coordinates to drawing area
                uint32_t mc = ev->width;
                uint32_t mr = ev->height;
                if (mc >= ts.cols) mc = ts.cols - 1;
                if (mr >= draw_rows) mr = draw_rows - 1;
                if (ev->key == "up") {
                    view.zoom_to_point(mc, mr, ts.cols, draw_rows, 1.5);
                } else if (ev->key == "down") {
                    view.zoom_to_point(mc, mr, ts.cols, draw_rows, 1.0 / 1.5);
                }
                need_redraw = true;
                break;
            }
            case InputKind::MouseDrag: {
                TermSize ts = terminal_size();
                uint32_t draw_rows = ts.rows > 1 ? ts.rows - 1 : 1;
                static DragState drag;
                if (ev->key == "start") {
                    // Clamp to drawing area
                    uint32_t mc = ev->width >= ts.cols ? ts.cols - 1 : ev->width;
                    uint32_t mr = ev->height >= draw_rows ? draw_rows - 1 : ev->height;
                    drag.active = true;
                    drag.col_start = mc;
                    drag.row_start = mr;
                    drag.col_end = mc;
                    drag.row_end = mr;
                } else if (ev->key == "move" && drag.active) {
                    uint32_t mc = ev->width >= ts.cols ? ts.cols - 1 : ev->width;
                    uint32_t mr = ev->height >= draw_rows ? draw_rows - 1 : ev->height;
                    drag.col_end = mc;
                    drag.row_end = mr;
                } else if (ev->key == "end" && drag.active) {
                    drag.active = false;
                    // Zoom to the selected region if it's non-trivial
                    uint32_t c_min = std::min(drag.col_start, drag.col_end);
                    uint32_t c_max = std::max(drag.col_start, drag.col_end);
                    uint32_t r_min = std::min(drag.row_start, drag.row_end);
                    uint32_t r_max = std::max(drag.row_start, drag.row_end);
                    uint32_t span_cols = c_max - c_min;
                    uint32_t span_rows = r_max - r_min;
                    if (span_cols > 2 || span_rows > 2) {
                        // Compute zoom factor from drag extent
                        double frac_cols = static_cast<double>(span_cols) / static_cast<double>(ts.cols);
                        double frac_rows = static_cast<double>(span_rows) / static_cast<double>(draw_rows);
                        double frac = std::max(frac_cols, frac_rows);
                        if (frac > 0.01 && frac < 1.0) {
                            double factor = 1.0 / frac;
                            // Zoom to centre of selection
                            uint32_t mid_col = (c_min + c_max) / 2;
                            uint32_t mid_row = (r_min + r_max) / 2;
                            view.zoom_to_point(mid_col, mid_row, ts.cols, draw_rows, factor);
                        }
                    }
                    need_redraw = true;
                }
                break;
            }
            default:
                break;
        }

        if (need_redraw) {
            render_frame();
        }
    }

    terminal_restore();
    return 0;
}
