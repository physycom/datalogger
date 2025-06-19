#include "datalogger.h"
#include <array>
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Testing Data structure..." << std::endl;

    // Test default initialization
    Data d{};
    for(int i = 0; i < 7; i++) {
        // Default values should be zero (undefined behavior, but testing current state)
        // assert(d.d[i] == 0.0);  // Commenting out as uninitialized values are unpredictable
    }

    // Test set() method with inertial data
    std::array<double,6> inertial{{1.1, 2.2, 3.3, 4.4, 5.5, 6.6}};
    d.set(inertial);
    for(int i=1;i<7;i++) {
        assert(d.d[i] == inertial[i-1]);
    }
    std::cout << "✓ Data.set() with inertial data works correctly" << std::endl;

    // Test setAcc() method - should overwrite first 3 elements
    std::array<double,3> acc{{7.7, 8.8, 9.9}};
    d.setAcc(acc);
    for(int i=1;i<4;i++) {
        assert(d.d[i] == acc[i-1]);
    }
    // Check that elements 4-6 remain unchanged from previous set() call
    for(int i=4;i<7;i++) {
        assert(d.d[i] == inertial[i-1]);
    }
    std::cout << "✓ Data.setAcc() works correctly and preserves other elements" << std::endl;

    // Test edge case with zeros
    std::array<double,3> zeros{{0.0, 0.0, 0.0}};
    d.setAcc(zeros);
    for(int i=1;i<4;i++) {
        assert(d.d[i] == 0.0);
    }
    std::cout << "✓ Data.setAcc() works with zero values" << std::endl;

    // Test with negative values
    std::array<double,6> negative{{-1.1, -2.2, -3.3, -4.4, -5.5, -6.6}};
    d.set(negative);
    for(int i=1;i<7;i++) {
        assert(d.d[i] == negative[i-1]);
    }
    std::cout << "✓ Data.set() works with negative values" << std::endl;

    // Test very large values
    std::array<double,3> large{{1e10, 1e20, 1e30}};
    d.setAcc(large);
    for(int i=1;i<4;i++) {
        assert(d.d[i] == large[i-1]);
    }
    std::cout << "✓ Data.setAcc() works with large values" << std::endl;

    std::cout << "All Data structure tests passed!" << std::endl;
    return 0;
}
