// Landmark unit tests: list, find, apply, overlay rendering
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
#include "landmarks.h"
#include "view.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(cond) do { \
    if (cond) { ++tests_passed; } \
    else { std::cerr << "FAIL at " << __LINE__ << "\n"; ++tests_failed; } \
} while(0)

#define TEST_NEAR(a, b, eps) do { \
    if (std::fabs((a) - (b)) < (eps)) { ++tests_passed; } \
    else { std::cerr << "FAIL at " << __LINE__ \
                    << " expected " << (b) << " got " << (a) << "\n"; \
          ++tests_failed; } \
} while(0)

static void test_landmark_list() {
    const auto& list = get_landmarks();
    // Should have at least 8 landmarks
    TEST(static_cast<int>(list.size()) >= 8);

    // Each landmark has a non-empty name
    for (const auto& lm : list) {
        TEST(lm.name != nullptr && lm.name[0] != '\0');
        TEST(lm.vertical_span > 0.0);
    }
}

static void test_find_landmark() {
    // Exact prefix match
    TEST(find_landmark("Seahorse") >= 0);
    TEST(find_landmark("seahorse") >= 0); // case-insensitive
    TEST(find_landmark("Elephant") >= 0);
    TEST(find_landmark("Whirlpool") >= 0);

    // Non-existent landmark
    TEST(find_landmark("zzzznonexistent") == -1);

    // Empty prefix returns -1
    TEST(find_landmark("") == -1);
}

static void test_apply_landmark() {
    const auto& list = get_landmarks();
    View view;

    int idx = find_landmark("Seahorse Valley");
    TEST(idx >= 0);

    bool ok = apply_landmark(list, idx, view);
    TEST(ok);
    TEST_NEAR(view.centre_re, -0.75, 0.001);
    TEST_NEAR(view.centre_im, 0.0, 0.001);
    TEST_NEAR(view.vertical_span, 0.15, 0.001);

    // Invalid index should fail
    TEST(!apply_landmark(list, -1, view));
    TEST(!apply_landmark(list, 999, view));
}

static void test_overlay_rendering() {
    auto rows = render_landmark_overlay(80, 24, "", 0);
    TEST(static_cast<int>(rows.size()) == 24);

    // First row should contain "Landmarks" header
    TEST(rows[0].find("Landmarks") != std::string::npos);

    // Filtered overlay should show fewer items
    auto filtered = render_landmark_overlay(80, 24, "Seahorse", 0);
    TEST(static_cast<int>(filtered.size()) == 24);
    TEST(filtered[0].find("filter: Seahorse") != std::string::npos);
}

int main() {
    test_landmark_list();
    test_find_landmark();
    test_apply_landmark();
    test_overlay_rendering();

    std::cout << "landmark_test: " << tests_passed << " passed, "
              << tests_failed << " failed.\n";
    return tests_failed > 0 ? 1 : 0;
}
