# Mandelbrot Console Explorer — Project Specification

## 1. Project summary

Create a Linux text-console application for exploring the Mandelbrot set. Render directly in a terminal using ANSI 256-colour codes and deterministic character dithering. Users can explore with the keyboard, drag-select a region to zoom with the mouse, and jump to named landmarks from a terminal drop-down menu.

There is no existing code. Begin by proposing the project structure and implementation plan, then build the application in small, tested milestones.

## 2. Goals

- Render a readable, visually rich Mandelbrot set in a terminal.
- Make panning, zooming, and returning to known locations easy.
- Keep the display responsive during expensive renders and terminal resizes.
- Preserve terminal state on normal exit, errors, and interrupt signals.
- Keep the mathematics, view state, terminal input/output, and UI state cleanly separated.

## 3. Target platform and implementation

- Platform: Linux terminal emulators that support ANSI/VT control sequences and xterm SGR mouse reporting.
- Language: C++.
- Display: ANSI 256-colour mode only; do not use a GUI window, true-colour RGB escape codes, or a browser.
- Keep POSIX terminal setup and signal handling in a small, isolated platform layer. Prefer C++ facilities where sufficient; use a narrowly scoped C/POSIX shim only where necessary.
- Detect terminal size and colour capability at startup. If 256-colour output or mouse reporting is unavailable, explain the limitation and continue with a useful keyboard-only fallback where possible.
- Do not write ANSI control codes into redirected output. If stdout is not a TTY, print a concise usage/error message and exit cleanly.

## 4. User experience

On launch, switch to the alternate screen, hide the cursor, and draw a full-screen interactive view. Keep a compact status/help strip visible with the current centre coordinate, vertical span or zoom, iteration limit, selected palette, and key controls.

The landmark selector is a terminal drop-down: a compact overlay opened by a key, navigable with arrow keys or type-to-filter, confirmed with Enter, and dismissed with Escape. It must not depend on mouse support.

On exit, restore the original terminal screen, cursor visibility, input mode, and mouse reporting state.

## 5. Mandelbrot rendering

### 5.1 Mathematical definition

Render the parameter plane for z₀ = 0 and zₙ₊₁ = zₙ² + c. For each screen sample, treat its complex-plane position as c = x + yi. A sample is inside the set if it has not escaped radius 2 before the iteration limit. Use a configurable iteration limit, with a practical default of 200 and higher limits available after zooming.

Use smooth escape-time colouring for escaped samples where numerically stable; keep the inside-set colour distinct (default: black or near-black). Avoid integer overflow and handle non-finite floating-point values safely.

### 5.2 View and sampling

- Store the view as complex-plane centre plus vertical span; derive horizontal span from terminal dimensions.
- Compensate for terminal cell aspect ratio so the set is not visibly stretched. Make the assumed cell width-to-height ratio a named setting that can be adjusted.
- Map samples deterministically from terminal coordinates to complex coordinates. Resizing must preserve the view centre and span.
- Support double-precision coordinates for ordinary and moderate deep zooms. Show a clear notice when double precision limits further useful zoom; arbitrary-precision arithmetic is out of scope for the first release.
- Zooming must preserve the complex-plane point under the cursor or drag-selection centre where applicable.

### 5.3 Colour and character dithering

- Use xterm 256-colour ANSI palette indices, emitted with standard foreground/background sequences such as 38;5;n and 48;5;n. Do not emit 24-bit colour sequences.
- Provide at least three selectable palettes, including a restrained monochrome/blue palette and two contrasting colour palettes. The inside-set colour must remain visually clear in each.
- Use a configurable density ramp, with an ASCII-safe default such as space, period, colon, dash, equals, plus, asterisk, hash, percent, at-sign, to represent fractional escape-time intensity.
- Apply a fixed ordered dithering matrix, for example a 4×4 Bayer matrix, to choose between adjacent ramp characters and, where useful, adjacent palette shades. The pattern must be deterministic and stable across redraws; do not use random noise.
- Keep dithering calculations separate from escape-time iteration and palette mapping so each can be tested independently.
- Reset ANSI attributes at row boundaries and on exit. Minimise redundant escape sequences to reduce flicker.

## 6. Controls

Provide an on-screen help overlay and keep the default controls discoverable.

| Input | Action |
|---|---|
| Arrow keys | Pan the view |
| + / - | Zoom in/out around the view centre |
| r | Return to the full-set overview |
| l | Open the landmark drop-down |
| p | Cycle or select palette |
| i | Increase iteration limit |
| I | Decrease iteration limit |
| ? | Show/hide help |
| q / Escape | Quit, unless Escape is dismissing an overlay |

Mouse behavior, when supported:

- Left-button drag draws a visible selection rectangle; release zooms to that rectangle.
- Adjust the rectangle to the current view aspect ratio before zooming, so the selected view fills the display without distortion.
- Escape or right-click cancels an in-progress drag.
- Mouse wheel zooms in/out around the pointer.
- Ignore malformed or out-of-range mouse reports without crashing.

## 7. Landmark drop-down

Bundle an editable landmark table. Each entry contains a display name, centre coordinate, vertical span, recommended iteration limit, and a short description. The following are initial presets; coordinates and spans are approximate starting views and should be visually checked during implementation.

| Landmark | Centre (real, imaginary) | Vertical span |
|---|---:|---:|
| Full Set | (-0.5, 0.0) | 3.0 |
| Seahorse Valley | (-0.743643887, 0.131825904) | 0.0015 |
| Elephant Valley | (0.275, 0.0075) | 0.05 |
| Triple Spiral Valley | (-0.0894, 0.6543) | 0.004 |
| Mini Mandelbrot | (-1.7497, 0.0) | 0.0007 |

The selector must display the selected entry’s name and a brief description. Selecting a preset updates the centre, span, iteration limit, and render.

## 8. Responsiveness and failure handling

- Keep terminal input responsive during rendering. Render into a frame buffer and draw completed frames; a newer view request should cancel or supersede stale work.
- Debounce rapid resize events and render again using the latest terminal size.
- Do not leave the screen blank while a new frame is being computed; retain the previous frame and show a subtle rendering indicator.
- Handle Ctrl-C and common termination signals through the same terminal-cleanup path as normal quit.
- On invalid terminal dimensions, unsupported escape sequences, or allocation/render errors, show a concise message when safe and restore terminal state before exiting.
- Do not silently swallow errors or claim a render completed when it was cancelled.

## 9. Configuration and scope

For the first release, provide command-line options for initial palette, iteration limit, and starting landmark. Keep the preset list and palettes in data structures that are straightforward to extend. Saving custom bookmarks and user configuration files are desirable follow-up features, not MVP requirements.

Out of scope for MVP: GUI rendering, animation, Julia-set mode, image export, arbitrary-precision deep zoom, network access, and a plugin system.

## 10. Quality requirements

- Use clear module boundaries for fractal math, view/coordinate mapping, colour and dithering, terminal input/output, landmark data, and application control.
- Document build and run instructions, supported terminal assumptions, controls, and known limitations in the README.
- Add unit tests for escape-time calculations, coordinate mapping/aspect correction, zoom-to-point, drag-selection bounds, palette mapping, and dithering.
- Add tests for terminal-input parsing that do not require an interactive terminal.
- Provide a non-interactive smoke-test mode that renders a small fixed frame or validates a preset without entering raw terminal mode.
- Include formatter, static checks, and a repeatable test command in the build instructions. Keep warnings and compiler errors at zero.
- Before each milestone is complete, run the relevant tests and build. The final milestone must pass the full test suite and smoke test.

## 11. Acceptance criteria

1. It builds and runs on Linux in a supported terminal without opening a GUI.
2. The full Mandelbrot set is recognizable, proportioned correctly, and rendered using only ANSI 256-colour codes plus the character ramp.
3. Dithering is deterministic and visibly selects glyph density levels; repeated redraws of the same view produce the same frame.
4. The keyboard can pan, zoom, change iterations/palette, reset, open help, use the landmark selector, and quit.
5. In an SGR-mouse-capable terminal, drag selection zooms accurately, the wheel zooms around the pointer, and cancellation works.
6. Each bundled landmark opens a recognisable view and reports its centre/span.
7. Resizing redraws at the new dimensions without losing the current view.
8. The terminal is restored after normal quit, Ctrl-C, and handled errors.
9. Unit tests, formatter/static checks, build, and non-interactive smoke test pass.

## 12. Cline project workflow

1. Inspect this specification and propose the C++ project layout, terminal-input strategy, and any required dependencies or C shim. Do not start coding until the plan is written and the key design choices are explained.
2. Save the implementation plan in PLAN.md with milestones and acceptance checks.
3. Implement the MVP as small vertical slices: terminal lifecycle and static overview; fractal renderer; colour/dithering; keyboard navigation; landmark selector; mouse drag/wheel; resize and cleanup; final verification.
4. Keep each task scoped to one slice. After each slice, run its tests and build, inspect the diff, and update PLAN.md.
5. Preserve a working application at every milestone; do not defer all integration until the end.
6. If a platform or library limitation changes the design, stop and update the plan before proceeding.

