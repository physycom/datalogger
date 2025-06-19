#include "serial_tools.h"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Testing ReadResult enum..." << std::endl;

    // Test enum values
    ReadResult result;

    // Test assignment and comparison
    result = resultInProgress;
    assert(result == resultInProgress);
    assert(result != resultSuccess);
    assert(result != resultError);
    assert(result != resultTimeout);
    std::cout << "✓ resultInProgress works correctly" << std::endl;

    result = resultSuccess;
    assert(result == resultSuccess);
    assert(result != resultInProgress);
    assert(result != resultError);
    assert(result != resultTimeout);
    std::cout << "✓ resultSuccess works correctly" << std::endl;

    result = resultError;
    assert(result == resultError);
    assert(result != resultInProgress);
    assert(result != resultSuccess);
    assert(result != resultTimeout);
    std::cout << "✓ resultError works correctly" << std::endl;

    result = resultTimeout;
    assert(result == resultTimeout);
    assert(result != resultInProgress);
    assert(result != resultSuccess);
    assert(result != resultError);
    std::cout << "✓ resultTimeout works correctly" << std::endl;

    // Test in array/switch context
    ReadResult results[] = {resultInProgress, resultSuccess, resultError, resultTimeout};
    int count = 0;

    for (auto r : results) {
        switch (r) {
            case resultInProgress:
                assert(count == 0);
                break;
            case resultSuccess:
                assert(count == 1);
                break;
            case resultError:
                assert(count == 2);
                break;
            case resultTimeout:
                assert(count == 3);
                break;
            default:
                assert(false); // Should not reach here
        }
        count++;
    }
    std::cout << "✓ ReadResult enum works correctly in switch statements" << std::endl;

    // Test copying
    ReadResult result_copy = resultSuccess;
    assert(result_copy == resultSuccess);

    result = result_copy;
    assert(result == resultSuccess);
    std::cout << "✓ ReadResult enum copying works correctly" << std::endl;

    // Test function parameter passing
    auto test_function = [](ReadResult r) -> bool {
        return r == resultError;
    };

    assert(test_function(resultError) == true);
    assert(test_function(resultSuccess) == false);
    std::cout << "✓ ReadResult enum works correctly as function parameter" << std::endl;

    std::cout << "All ReadResult enum tests passed!" << std::endl;
    return 0;
}
