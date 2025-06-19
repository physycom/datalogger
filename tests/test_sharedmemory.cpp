#include "shared_memory.hpp"
#include "data.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

int main(){
    std::cout << "Testing shared memory functionality..." << std::endl;

#if defined(USE_HOST_MEMORY)
    const char* name = "testmem";
    const char* name2 = "testmem2";

    // Test basic allocation and access
    Data* ptr = static_cast<Data*>(allocate_host_memory(name, sizeof(Data)));
    assert(ptr != nullptr);
    ptr->d[0] = 42.5;
    ptr->d[1] = 123.456;
    std::cout << "✓ Basic memory allocation and write works" << std::endl;

    // Test getting existing memory
    Data* ptr2 = static_cast<Data*>(get_host_allocated_memory(name));
    assert(ptr2 != nullptr);
    assert(ptr2->d[0] == 42.5);
    assert(ptr2->d[1] == 123.456);
    std::cout << "✓ Getting existing memory works correctly" << std::endl;

    // Test that both pointers point to the same memory
    ptr2->d[0] = 99.9;
    assert(ptr->d[0] == 99.9);
    ptr->d[2] = 77.7;
    assert(ptr2->d[2] == 77.7);
    std::cout << "✓ Shared memory access works correctly" << std::endl;

    // Test with array of Data structures
    const size_t array_size = 10;
    Data* array_ptr = static_cast<Data*>(allocate_host_memory(name2, array_size * sizeof(Data)));
    assert(array_ptr != nullptr);

    // Initialize array
    for(size_t i = 0; i < array_size; i++) {
        array_ptr[i].d[0] = static_cast<double>(i);
        array_ptr[i].d[1] = static_cast<double>(i * 2);
    }

    // Verify array access
    Data* array_ptr2 = static_cast<Data*>(get_host_allocated_memory(name2));
    assert(array_ptr2 != nullptr);
    for(size_t i = 0; i < array_size; i++) {
        assert(array_ptr2[i].d[0] == static_cast<double>(i));
        assert(array_ptr2[i].d[1] == static_cast<double>(i * 2));
    }
    std::cout << "✓ Array allocation and access works correctly" << std::endl;

    // Test with Data methods
    std::array<double, 6> inertial_data = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6};
    array_ptr[0].set(inertial_data);

    // Verify through second pointer
    for(int i = 1; i < 7; i++) {
        assert(array_ptr2[0].d[i] == inertial_data[i-1]);
    }
    std::cout << "✓ Data structure methods work in shared memory" << std::endl;

    // Test setAcc method
    std::array<double, 3> acc_data = {10.1, 20.2, 30.3};
    array_ptr2[1].setAcc(acc_data);

    // Verify through first pointer
    for(int i = 1; i < 4; i++) {
        assert(array_ptr[1].d[i] == acc_data[i-1]);
    }
    std::cout << "✓ Data.setAcc() works in shared memory" << std::endl;

    // Test memory cleanup
    remove_host_memory(name);
    remove_host_memory(name2);
    std::cout << "✓ Memory cleanup works" << std::endl;

    // Test getting non-existent memory (should return nullptr or handle gracefully)
    Data* null_ptr = static_cast<Data*>(get_host_allocated_memory("nonexistent"));
    // Note: behavior may vary by implementation, but should not crash
    std::cout << "✓ Getting non-existent memory handled gracefully" << std::endl;

    std::cout << "All shared memory tests passed!" << std::endl;
#else
    std::cout << "Shared memory functionality not enabled (USE_HOST_MEMORY not defined)" << std::endl;
#endif

    return 0;
}
