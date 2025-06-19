#include "version.h"
#include <cassert>
#include <iostream>
#include <string>

int main() {
    std::cout << "Testing version constants..." << std::endl;

    // Test that version constants are defined
    std::cout << "Major version: " << MAJOR_VERSION << std::endl;
    std::cout << "Minor version: " << MINOR_VERSION << std::endl;

    // Test that they are numeric values
    assert(MAJOR_VERSION >= 0);
    assert(MINOR_VERSION >= 0);
    std::cout << "✓ Version constants are non-negative integers" << std::endl;

    // Test that they can be used in arithmetic
    int total_version = MAJOR_VERSION * 100 + MINOR_VERSION;
    assert(total_version >= 0);
    std::cout << "✓ Version constants can be used in arithmetic operations" << std::endl;

    // Test that they can be used in comparisons
    bool is_major_zero = (MAJOR_VERSION == 0);
    bool is_minor_zero = (MINOR_VERSION == 0);
    std::cout << "✓ Version constants can be used in comparisons" << std::endl;

    // Test that they can be used in string formatting
    std::string version_string = std::string("v") + std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION);
    assert(!version_string.empty());
    assert(version_string.find("v") == 0);
    assert(version_string.find(".") != std::string::npos);
    std::cout << "✓ Version string: " << version_string << std::endl;

    // Test preprocessor concatenation (if needed)
    #define STRINGIFY(x) #x
    #define TOSTRING(x) STRINGIFY(x)

    std::string major_str = TOSTRING(MAJOR_VERSION);
    std::string minor_str = TOSTRING(MINOR_VERSION);
    assert(!major_str.empty());
    assert(!minor_str.empty());
    std::cout << "✓ Version constants work with preprocessor stringification" << std::endl;

    // Test that constants are compile-time constants (can be used in array size)
    int test_array[MAJOR_VERSION + MINOR_VERSION + 1];
    test_array[0] = 42;
    assert(test_array[0] == 42);
    std::cout << "✓ Version constants are compile-time constants" << std::endl;

    std::cout << "All version constant tests passed!" << std::endl;
    return 0;
}
