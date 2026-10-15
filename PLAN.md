# PLAN.md — Mandelbrot Console Explorer

## Project Structure

```
src/
  fractal/       — Mandelbrot math, view state, coordinate mapping
  colour/        — palette definitions, smooth escape-time, dithering
  terminal/      — POSIX setup/teardown, ANSI output, mouse/keyboard input
  ui/            — landmark selector overlay, status bar, help screen
  app/           — main loop, command-line parsing, signal handling
tests/
  fractal_test.cpp
  colour_test.cpp
  input_test.cpp
  smoke_test.cpp
CMakeLists.txt
README.md
```

No external dependencies beyond C++17 and POSIX. A narrow C shim for `termios`, `sigaction`, and `ioctl TIOCGWINSZ`.

## Key Technical Decisions

1. **Build system**: CMake 3.16+ with `ccache`-friendly flags. `-Wall -Wextra -Werror`.
2. **Terminal input**: Read raw bytes from stdin; parse ANSI/VT and xterm SGR escape sequences in a small state-machine parser. No ncurses or third-party library.
3. **Rendering model**: Single render thread fills a row-buffer; rows are flushed to stdout as soon as ready so the screen fills progressively. A volatile `cancelled` flag lets resize or quit interrupt mid-frame.
4. **Smooth iteration**: Use `n + 1 - log(log(|z|))/log(2)` for continuous colour; map into 256-colour palette with modular wrapping.
5. **Dithering**: Deterministic 2×2 Bayer matrix applied to the fractional remainder after palette quantisation, selecting from a fixed glyph ramp ` .'°∶∙⌂⌠░▒▓█`.
6. **Aspect correction**: Default cell ratio 0.6 (adjustable via `-r` flag). Horizontal span = vertical span × cols / rows × ratio.
7. **Landmarks**: Built-in struct array with name, centre, span. Drop-down overlay filters by typed prefix.
8. **Graceful shutdown**: `sigaction` for SIGINT/SIGTERM sets a flag; the main loop drains it, runs the terminal-restore path, then exits 0.

## Milestones

### M1 — Skeleton + Terminal Lifecycle
- CMake project compiles and runs.
- On start: alternate screen, hide cursor, raw input mode, SGR mouse.
- On exit (including Ctrl-C): restore everything.
- Print a static "hello" frame; handle `SIGWINCH`.
- **Check**: `./mandelbrot` starts and exits cleanly; no terminal corruption.

### M2 — Fractal Math + Static Render
- `mandelbrot_escape_time(c, max_iter)` with smooth iteration.
- View struct (centre, vertical span), coordinate mapping with aspect correction.
- Render a full frame to stdout using ANSI 256-colour + dithering.
- **Check**: Unit tests for escape-time, coordinate mapping, zoom-to-point. Smoke-test renders a 40×20 frame to a file and diffs against golden output.

### M3 — Palettes + Dithering
- 4 built-in palettes (thermal, electric, grayscale, ocean).
- Deterministic Bayer dithering; unit tests confirm repeatable output.
- CLI flag `-p` selects palette.
- **Check**: Palette/dither unit tests pass; visual inspection shows smooth gradients.

### M4 — Keyboard Navigation
- Arrow keys pan, `+`/`-` zoom, `r` reset, `q` quit, `h` help.
- Status bar shows centre, span, iterations, palette.
- Iteration limit cycling with `[` / `]`.
- **Check**: Interactive manual test; no input-echo leakage.

### M5 — Landmark Selector ✅ DONE
- Overlay opened with `l`, navigable with arrows + type-to-filter, Enter to jump, Escape to dismiss.
- 10 bundled landmarks (Seahorse Valley, Eel, Spiral Arm, Elephant Valley, Lightning Bolt, Sceptre, Triple Spiral, Mini Seahorses, Whirlpool, Coral Reef).
- CLI flag `-l <name>` for starting landmark.
- **Check**: `landmark_test` passes (list, find, apply, overlay rendering).

### M6 — Mouse Support ✅ DONE
- SGR mouse parsing: wheel zooms toward pointer, left-drag selects region then zooms, right-click or Escape cancels.
- Graceful fallback when mouse is unavailable (mouse events silently ignored if terminal doesn't report them).
- Drag-selection bounds unit-tested (10 assertions: bounds computation, zoom factor, clamp).
- **Check**: `mouse_test` passes; full suite 6/6 green.

### M7 — Resize + Progressive Render + Final Polish ✅ DONE
- Resize redraws at new dimensions preserving view centre.
- Progressive row-by-row render with "Rendering..." indicator in status bar (updates every 5 rows).
- Stdout-not-a-tty detection → usage message + exit.
- Double-precision zoom-limit warning (red status bar below 1e-12 span).
- README complete.
- **Check**: Full test suite 6/6 green; `-Werror` clean build; smoke test passes.

## Build / Test Commands

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build          # unit tests + smoke test
./build/mandelbrot --smoke      # non-interactive smoke run
```

## Out of Scope (MVP)

GUI, Julia sets, image export, arbitrary-precision arithmetic, bookmarks, config files, plugin system.
