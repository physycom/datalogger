#include "swap_tools.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

int main() {
    std::cout << "Testing swap_tools functionality..." << std::endl;

    // Test endianness detection
    int endian = is_big_endian();
    std::cout << "System is " << (endian ? "big" : "little") << " endian" << std::endl;

    // Test short (16-bit) endian swapping
    short test_shorts[] = {
        static_cast<short>(0x1234),
        static_cast<short>(0x5678),
        static_cast<short>(0xABCD),
        static_cast<short>(0xEF01)
    };
    short original_shorts[4];
    memcpy(original_shorts, test_shorts, sizeof(test_shorts));

    swap_endian_s(test_shorts, 4);

    // Verify swapping occurred
    assert(test_shorts[0] == static_cast<short>(0x3412));
    assert(test_shorts[1] == static_cast<short>(0x7856));
    assert(test_shorts[2] == static_cast<short>(0xCDAB));
    assert(test_shorts[3] == static_cast<short>(0x01EF));
    std::cout << "✓ Short endian swapping works correctly" << std::endl;

    // Test double swapping returns to original
    swap_endian_s(test_shorts, 4);
    for(int i = 0; i < 4; i++) {
        assert(test_shorts[i] == original_shorts[i]);
    }
    std::cout << "✓ Double short endian swapping returns to original" << std::endl;

    // Test integer (32-bit) endian swapping
    int test_ints[] = {
        static_cast<int>(0x12345678),
        static_cast<int>(0x9ABCDEF0),
        static_cast<int>(0x11223344),
        static_cast<int>(0x55667788)
    };
    int original_ints[4];
    memcpy(original_ints, test_ints, sizeof(test_ints));

    swap_endian_i(test_ints, 4);

    // Verify swapping occurred
    assert(test_ints[0] == static_cast<int>(0x78563412));
    assert(test_ints[1] == static_cast<int>(0xF0DEBC9A));
    assert(test_ints[2] == static_cast<int>(0x44332211));
    assert(test_ints[3] == static_cast<int>(0x88776655));
    std::cout << "✓ Integer endian swapping works correctly" << std::endl;

    // Test double swapping returns to original
    swap_endian_i(test_ints, 4);
    for(int i = 0; i < 4; i++) {
        assert(test_ints[i] == original_ints[i]);
    }
    std::cout << "✓ Double integer endian swapping returns to original" << std::endl;

    // Test float endian swapping
    float test_floats[] = {1.0f, 2.5f, -3.14159f, 1e6f};
    float original_floats[4];
    memcpy(original_floats, test_floats, sizeof(test_floats));

    // Since float swapping changes the bit pattern, we can't easily test the exact values
    // But we can test that double swapping returns to original
    swap_endian_f(test_floats, 4);
    swap_endian_f(test_floats, 4);

    for(int i = 0; i < 4; i++) {
        assert(test_floats[i] == original_floats[i]);
    }
    std::cout << "✓ Double float endian swapping returns to original" << std::endl;

    // Test edge cases - zero values
    short zero_shorts[] = {0, 0, 0};
    short zero_orig[3];
    memcpy(zero_orig, zero_shorts, sizeof(zero_shorts));

    swap_endian_s(zero_shorts, 3);
    for(int i = 0; i < 3; i++) {
        assert(zero_shorts[i] == zero_orig[i]); // Zero should remain zero
    }
    std::cout << "✓ Zero values remain unchanged after swapping" << std::endl;

    // Test single element arrays
    short single_short = static_cast<short>(0x1234);
    swap_endian_s(&single_short, 1);
    assert(single_short == static_cast<short>(0x3412));
    std::cout << "✓ Single element swapping works correctly" << std::endl;

    int single_int = static_cast<int>(0x12345678);
    swap_endian_i(&single_int, 1);
    assert(single_int == static_cast<int>(0x78563412));
    std::cout << "✓ Single integer swapping works correctly" << std::endl;

    float single_float = 1.0f;
    float single_orig = single_float;
    swap_endian_f(&single_float, 1);
    swap_endian_f(&single_float, 1);
    assert(single_float == single_orig);
    std::cout << "✓ Single float double swapping works correctly" << std::endl;

    // Test with n=0 (should do nothing)
    short test_n0[] = {static_cast<short>(0x1234)};
    short orig_n0 = test_n0[0];
    swap_endian_s(test_n0, 0);
    assert(test_n0[0] == orig_n0);
    std::cout << "✓ Swapping with n=0 does nothing" << std::endl;

    std::cout << "All swap_tools tests passed!" << std::endl;
    return 0;
}
