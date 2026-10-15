#ifndef TERMINAL_INPUT_H
#define TERMINAL_INPUT_H

#include <string>
#include <optional>
#include <cstdint>

/// Parsed keyboard or mouse event.
enum class InputKind {
    Key,
    Resize,
    Quit,
    MouseWheel,  // scroll wheel: width=col+1, height=row+1, key="up"/"down"
    MouseDrag,   // drag start/move/end: key="start"/"move"/"end", width=col+1, height=row+1
    None
};

struct InputEvent {
    InputKind kind;
    std::string key;       // for Key events: "Up", "Down", "q", etc.
    uint32_t  width;       // for Resize events
    uint32_t  height;
};

/// Read one input event from stdin (blocking).
/// Returns nullopt on read error / EOF.
std::optional<InputEvent> input_read();

/// Check if there is a pending resize event without blocking.
bool input_pending_resize();

#endif // TERMINAL_INPUT_H
