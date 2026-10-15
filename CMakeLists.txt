cmake_minimum_required(VERSION 3.16)
project(mandelbrot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include paths for all sub-modules
include_directories(src/fractal src/colour src/terminal src/ui src/app)

add_compile_options(-Wall -Wextra -Werror)

# Source files
set(SOURCES
    src/app/main.cpp
    src/app/cli.cpp
    src/app/app.cpp
    src/fractal/mandelbrot.cpp
    src/fractal/view.cpp
    src/colour/palette.cpp
    src/colour/dither.cpp
    src/colour/render.cpp
    src/terminal/terminal.cpp
    src/terminal/input.cpp
    src/terminal/mouse.cpp
    src/ui/status_bar.cpp
    src/ui/landmarks.cpp
)

# Executable
add_executable(mandelbrot ${SOURCES})

# Tests
enable_testing()

# Fractal unit tests
add_executable(fractal_test tests/fractal_test.cpp
    src/fractal/mandelbrot.cpp
    src/fractal/view.cpp)
add_test(NAME fractal_test COMMAND fractal_test)

# Colour unit tests
add_executable(colour_test tests/colour_test.cpp
    src/colour/palette.cpp
    src/colour/dither.cpp
    src/fractal/mandelbrot.cpp)
add_test(NAME colour_test COMMAND colour_test)

# Input parser unit tests
add_executable(input_test tests/input_test.cpp
    src/terminal/input.cpp)
add_test(NAME input_test COMMAND input_test)

# Smoke test (non-interactive render to stdout)
add_executable(smoke_test tests/smoke_test.cpp
    src/fractal/mandelbrot.cpp
    src/fractal/view.cpp
    src/colour/palette.cpp
    src/colour/dither.cpp
    src/colour/render.cpp)
add_test(NAME smoke_test COMMAND smoke_test)

# Landmark unit tests
add_executable(landmark_test tests/landmark_test.cpp
    src/ui/landmarks.cpp
    src/fractal/view.cpp)
add_test(NAME landmark_test COMMAND landmark_test)

# Mouse / drag-selection unit tests
add_executable(mouse_test tests/mouse_test.cpp
    src/terminal/mouse.cpp)
add_test(NAME mouse_test COMMAND mouse_test)
