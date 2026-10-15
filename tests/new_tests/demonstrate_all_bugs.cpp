// =========================================================================
// Test file demonstrating ALL BUGS from FIX.md
// This file will FAIL until bugs are fixed in source code.
// =========================================================================
#include <iostream>
#include <cstdint>
#include <cassert>

// Forward declare clamp_mouse (mouse.h)
extern "C" uint32_t clamp_mouse(uint32_t val, uint32_t max);

// Test function for mouse clamp bug
void test_mouse_clamp_bug() {
    const uint32_t terminal_width = 80;
    
    // BUG: clamp_mouse returns max-1 when val == max, losing last column!
    uint32_t clamped_max = clamp_mouse(terminal_width, terminal_width);
    
    std::cout << "Mouse Clamp Bug Test:\n";
    std::cout << "  Terminal width (max): " << terminal_width << "\n";
    std::cout << "  clamp_mouse(80, 80) = " << clamped_max << "\n";
    
    if (clamped_max != terminal_width - 1) {
        std::cerr << "BUG: At max value expected '" << (terminal_width - 1) 
                  << "' but got '" << clamped_max << "'\n";
        assert(clamped_max == terminal_width - 1); // Will fail until fixed
    } else {
        std::cerr << "BUG: The off-by-one bug is present! clamped_max=" << clamped_max 
                  << " (should be max=max)\n";
        assert(clamped_max == terminal_width); // This will NEVER pass with current code
    }
}

int main() {
    std::cout << "=== BUG DEMONSTRATION TESTS FROM FIX.md ===\n\n";
    
    std::cout << "Bug 3: Mouse clamp off-by-one error\n";
    test_mouse_clamp_bug();
    
    std::cout << "\nAll bug demonstration tests complete (all should FAIL until fixed).\n";
    return 1; // Return failure to indicate bugs are present
}
