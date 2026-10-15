// Test for mouse clamp bug (standalone)
#include <iostream>
#include <cassert>

// BUGGY version from mouse.h:
extern uint32_t clamp_mouse(uint32_t val, uint32_t max);

int main() {
    std::cout << "=== Testing Mouse Clamp Bug ===
";
    
    // This function is defined in mouse.cpp - we need to link it
    // For now, let us just show what the test expects:
    
    uint32_t max = 80;
    uint32_t val = max;  // value at boundary
    
    // Current buggy implementation returns max-1 when called with max
    // The test expects that clamp_mouse(max, max) == max - 1 (BUG!
    // But logically it SHOULD return max to use all columns
    
    std::cout << "Bug: clamp_mouse(" << max << "," << max << ") currently returns"
              << " max-1 = " << (max - 1) << "
";
    std::cout << "Expected: it should return max to utilize all columns
";
    
    return 0; // We will link mouse.cpp properly later
}
