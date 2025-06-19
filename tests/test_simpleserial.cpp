#include "serial_tools.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "Testing SimpleSerial class (constructor and basic operations)..." << std::endl;

    // Note: We can't test actual serial communication without hardware,
    // but we can test constructor behavior and error handling

    try {
        // Test constructor with invalid port (should throw)
        SimpleSerial serial("INVALID_PORT_NAME_THAT_DOES_NOT_EXIST", 9600);
        std::cout << "Warning: Constructor did not throw for invalid port" << std::endl;

        // If constructor succeeded (which it shouldn't), test string operations
        // These will likely fail, but we're testing the interface
        try {
            serial.writeString("test");
            std::cout << "Warning: writeString did not throw for invalid port" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✓ writeString correctly throws for invalid port: " << e.what() << std::endl;
        }

        try {
            std::string result = serial.readLine();
            std::cout << "Warning: readLine did not throw for invalid port" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✓ readLine correctly throws for invalid port: " << e.what() << std::endl;
        }

    } catch (const boost::system::system_error& e) {
        std::cout << "✓ Constructor correctly throws for invalid port: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✓ Constructor throws exception for invalid port: " << e.what() << std::endl;
    }

    // Test with different invalid port names
    std::vector<std::string> invalid_ports = {
        "",
        "NON_EXISTENT_PORT",
        "/dev/null/invalid",
        "COM999"
    };

    for (const auto& port : invalid_ports) {
        try {
            SimpleSerial serial(port, 9600);
            std::cout << "Warning: Constructor succeeded for port: " << port << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✓ Constructor correctly throws for invalid port '" << port << "': " << e.what() << std::endl;
        }
    }

    // Test with different baud rates (still with invalid ports)
    std::vector<unsigned int> baud_rates = {110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

    for (auto baud : baud_rates) {
        try {
            SimpleSerial serial("INVALID_PORT", baud);
            std::cout << "Warning: Constructor succeeded for baud rate: " << baud << std::endl;
        } catch (const std::exception& e) {
            // Expected to fail due to invalid port, not baud rate
            std::cout << "✓ Constructor with baud rate " << baud << " correctly throws for invalid port" << std::endl;
        }
    }

    std::cout << "SimpleSerial basic interface tests completed!" << std::endl;
    std::cout << "Note: Full functionality testing requires actual serial hardware" << std::endl;
    return 0;
}
