#include "serial_tools.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "Testing timeout and exception classes..." << std::endl;

    // Test timeout_exception
    try {
        throw timeout_exception("Test timeout message");
        assert(false); // Should not reach here
    } catch (const timeout_exception& e) {
        std::string msg = e.what();
        assert(msg == "Test timeout message");
        std::cout << "✓ timeout_exception works correctly" << std::endl;
    } catch (...) {
        assert(false); // Should catch timeout_exception specifically
    }

    // Test that timeout_exception inherits from std::runtime_error
    try {
        throw timeout_exception("Runtime error test");
        assert(false);
    } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        assert(msg == "Runtime error test");
        std::cout << "✓ timeout_exception inherits from std::runtime_error" << std::endl;
    } catch (...) {
        assert(false);
    }

    // Test TimeoutException
    try {
        throw TimeoutException("Test timeout exception message");
        assert(false);
    } catch (const TimeoutException& e) {
        std::string msg = e.what();
        assert(msg == "Test timeout exception message");
        std::cout << "✓ TimeoutException works correctly" << std::endl;
    } catch (...) {
        assert(false);
    }

    // Test that TimeoutException inherits from std::ios_base::failure
    try {
        throw TimeoutException("iOS failure test");
        assert(false);
    } catch (const std::ios_base::failure& e) {
        std::string msg = e.what();
        assert(msg == "iOS failure test");
        std::cout << "✓ TimeoutException inherits from std::ios_base::failure" << std::endl;
    } catch (...) {
        assert(false);
    }

    // Test empty message
    try {
        throw timeout_exception("");
        assert(false);
    } catch (const timeout_exception& e) {
        std::string msg = e.what();
        assert(msg == "");
        std::cout << "✓ timeout_exception works with empty message" << std::endl;
    }

    try {
        throw TimeoutException("");
        assert(false);
    } catch (const TimeoutException& e) {
        std::string msg = e.what();
        assert(msg == "");
        std::cout << "✓ TimeoutException works with empty message" << std::endl;
    }

    // Test long message
    std::string long_msg = "This is a very long error message that tests whether the exception classes can handle longer strings without issues. It should work fine.";
    try {
        throw timeout_exception(long_msg);
        assert(false);
    } catch (const timeout_exception& e) {
        std::string msg = e.what();
        assert(msg == long_msg);
        std::cout << "✓ timeout_exception works with long message" << std::endl;
    }

    try {
        throw TimeoutException(long_msg);
        assert(false);
    } catch (const TimeoutException& e) {
        std::string msg = e.what();
        assert(msg == long_msg);
        std::cout << "✓ TimeoutException works with long message" << std::endl;
    }

    // Test with special characters
    std::string special_msg = "Error: Special chars !@#$%^&*()_+{}|:<>?[];',./";
    try {
        throw timeout_exception(special_msg);
        assert(false);
    } catch (const timeout_exception& e) {
        std::string msg = e.what();
        assert(msg == special_msg);
        std::cout << "✓ timeout_exception works with special characters" << std::endl;
    }

    std::cout << "All timeout and exception tests passed!" << std::endl;
    return 0;
}
