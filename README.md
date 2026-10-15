# Mandelbrot Console Explorer

A fully interactive Mandelbrot set viewer in the terminal. Progressive row-by-row rendering, mouse and keyboard navigation, landmark selection, and smooth colour palettes — all with zero external dependencies.

## Features

- **Smooth escape-time rendering** with continuous colour (`n + 1 − log₂(log₂|z|)`)
- **4 built-in palettes**: thermal, electric, grayscale, ocean
- **Deterministic Bayer dithering** for smooth gradients on text terminals
- **Keyboard navigation**: arrow keys pan, `+`/`-` zoom, `[`/`]` cycle iteration limits
- **Mouse support**: scroll-wheel zoom, click-drag region selection, right-click to cancel
- **Landmark selector**: 10 bundled locations (Seahorse Valley, Eel, Spiral Arm, etc.)
- **Progressive rendering**: row-by-row output with live status-bar progress indicator
- **Graceful shutdown**: handles SIGINT/SIGTERM, restores terminal on exit
- **Resize handling**: redraws at new dimensions preserving view centre
- **Zoom-limit warning**: red status bar when double-precision artefacts appear

## Requirements

- C++17 compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.16+
- POSIX terminal (Linux, macOS, WSL)

No external dependencies. A narrow C shim provides `termios`, `sigaction`, and `ioctl TIOCGWINSZ`.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Usage

```bash
# Interactive mode (requires a terminal)
./build/mandelbrot

# Start at a landmark
./build/mandelbrot -l seahorse

# Non-interactive smoke test (render to stdout)
./build/mandelbrot --smoke

# Run unit tests
ctest --test-dir build
```

### Command-Line Flags

| Flag | Description |
|------|-------------|
| `-p <name>` | Palette: `thermal`, `electric`, `grayscale`, `ocean` (default: thermal) |
| `-l <name>` | Start at a landmark by name |
| `-i <n>` | Iteration limit: 50, 100, 200, 500, 1000, 2000, 5000 (default: 200) |
| `-r <ratio>` | Cell aspect ratio (default: 0.6) |
| `--smoke` | Non-interactive render to stdout |
| `--smoke-rows <n>` | Smoke test row count (default: 20) |
| `--smoke-cols <n>` | Smoke test column count (default: 80) |

### Keyboard Controls

| Key | Action |
|-----|--------|
| `↑` `↓` `←` `→` | Pan view |
| `+` / `-` | Zoom in / out |
| `[` / `]` | Decrease / increase iteration limit |
| `p` | Cycle palette |
| `l` | Open landmark selector |
| `r` | Reset to default view |
| `h` | Toggle help overlay |
| `q` / `Ctrl-C` | Quit |

### Mouse Controls

| Action | Effect |
|--------|--------|
| Scroll wheel | Zoom toward cursor position |
| Left-click drag | Select region, release to zoom into it |
| Right-click / Escape | Cancel drag selection |

## Architecture

```
src/
  fractal/       — Mandelbrot math, view state, coordinate mapping
  colour/        — palette definitions, smooth escape-time, dithering
  terminal/      — POSIX setup/teardown, ANSI output, mouse/keyboard input
  ui/            — landmark selector overlay, status bar, help screen
  app/           — main loop, command-line parsing, signal handling
tests/
  fractal_test.cpp   — escape-time, coordinate mapping, zoom
  colour_test.cpp    — palette, dithering
  input_test.cpp     — ANSI escape sequence parser
  smoke_test.cpp     — non-interactive render output
  landmark_test.cpp  — landmark list, search, apply
  mouse_test.cpp     — drag-selection bounds, zoom factor, clamp
```

## Technical Details

- **Build system**: CMake 3.16+ with `-Wall -Wextra -Werror`
- **Terminal input**: Raw byte reading with ANSI/VT and xterm SGR state-machine parser (no ncurses)
- **Rendering**: Single-threaded row-buffer flushed progressively; volatile `cancelled` flag for interrupt
- **Dithering**: 2×2 Bayer matrix over glyph ramp ` .'°∙⌂⌠░▒▓█`
- **Aspect correction**: Default cell ratio 0.6, adjustable via `-r`
- **Zoom limit**: ~1e-12 span — beyond this double-precision produces visible artefacts

## License

MIT
