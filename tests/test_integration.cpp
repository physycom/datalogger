#include "datalogger.h"
#include "data_tools.hpp"
#include "serial_tools.h"
#include "swap_tools.hpp"
#include <cassert>
#include <iostream>
#include <array>
#include <vector>

int main() {
    std::cout << "Running integration tests..." << std::endl;

    // Test 1: Data structure with NavData integration
    {
        std::cout << "\n--- Test 1: Data and NavData Integration ---" << std::endl;

        Data data_struct;
        NavData nav_data;

        // Set some test data in NavData
        double acc[3] = {1.5, 2.5, 3.5};
        double gyr[3] = {4.5, 5.5, 6.5};
        nav_data.setAcc(acc);
        nav_data.setGyr(gyr);
        nav_data.setSpeed(15.3);
        nav_data.setLat(45.123);
        nav_data.setLon(9.456);
        nav_data.setTime_s("2023:12:25:10:30:45");

        // Get inertial data and set it in Data structure
        auto inertial = nav_data.getInertial();
        data_struct.set(inertial);

        // Verify the data transfer
        for(int i = 1; i < 7; i++) {
            assert(data_struct.d[i] == inertial[i-1]);
        }

        // Verify specific values
        assert(data_struct.d[POS_AX] == 1.5);
        assert(data_struct.d[POS_AY] == 2.5);
        assert(data_struct.d[POS_AZ] == 3.5);
        assert(data_struct.d[POS_GX] == 4.5);
        assert(data_struct.d[POS_GY] == 5.5);
        assert(data_struct.d[POS_GZ] == 6.5);

        std::cout << "✓ Data and NavData integration works correctly" << std::endl;
    }

    // Test 2: SerialOptions with realistic configurations
    {
        std::cout << "\n--- Test 2: SerialOptions Realistic Configurations ---" << std::endl;

        // Test common GPS receiver configuration
        SerialOptions gps_config;
        gps_config.setDevice("COM3");
        gps_config.setBaudrate(4800);
        gps_config.setParity(SerialOptions::noparity);
        gps_config.setCsize(8);
        gps_config.setStopBits(SerialOptions::one);
        gps_config.setFlowControl(SerialOptions::noflow);
        gps_config.setTimeout(boost::posix_time::seconds(SERIAL_PORT_TIMEOUT_SECONDS));

        assert(gps_config.getDevice() == "COM3");
        assert(gps_config.getBaudrate() == 4800);
        assert(gps_config.getParity() == SerialOptions::noparity);
        assert(gps_config.getCsize() == 8);
        assert(gps_config.getStopBits() == SerialOptions::one);
        assert(gps_config.getFlowControl() == SerialOptions::noflow);

        // Test high-speed data logger configuration
        SerialOptions datalogger_config;
        datalogger_config.setDevice("/dev/ttyUSB0");
        datalogger_config.setBaudrate(115200);
        datalogger_config.setParity(SerialOptions::even);
        datalogger_config.setCsize(8);
        datalogger_config.setStopBits(SerialOptions::one);
        datalogger_config.setFlowControl(SerialOptions::hardware);

        assert(datalogger_config.getDevice() == "/dev/ttyUSB0");
        assert(datalogger_config.getBaudrate() == 115200);
        assert(datalogger_config.getParity() == SerialOptions::even);
        assert(datalogger_config.getFlowControl() == SerialOptions::hardware);

        std::cout << "✓ Realistic SerialOptions configurations work correctly" << std::endl;
    }

    // Test 3: Endian swapping with data arrays
    {
        std::cout << "\n--- Test 3: Endian Swapping Integration ---" << std::endl;

        // Create test data that mimics sensor readings
        std::vector<short> sensor_data = {
            static_cast<short>(0x1234),
            static_cast<short>(0x5678),
            static_cast<short>(0x9ABC),
            static_cast<short>(0xDEF0)
        };
        std::vector<short> original_data = sensor_data;

        // Simulate receiving data from a different endian system
        swap_endian_s(sensor_data.data(), sensor_data.size());

        // Verify data was swapped
        assert(sensor_data[0] == static_cast<short>(0x3412));
        assert(sensor_data[1] == static_cast<short>(0x7856));

        // Swap back to original
        swap_endian_s(sensor_data.data(), sensor_data.size());

        // Verify we got back to original
        for(size_t i = 0; i < sensor_data.size(); i++) {
            assert(sensor_data[i] == original_data[i]);
        }

        std::cout << "✓ Endian swapping integration works correctly" << std::endl;
    }

    // Test 4: Exception handling workflow
    {
        std::cout << "\n--- Test 4: Exception Handling Workflow ---" << std::endl;

        bool timeout_caught = false;
        bool ios_failure_caught = false;

        // Test timeout_exception in a typical serial communication scenario
        try {
            // Simulate a timeout scenario
            throw timeout_exception("Serial read timeout after 3 seconds");
        } catch (const timeout_exception& e) {
            timeout_caught = true;
            std::string msg = e.what();
            assert(msg.find("timeout") != std::string::npos);
        } catch (const std::runtime_error& e) {
            assert(false); // Should catch more specific exception
        }

        // Test TimeoutException in an I/O scenario
        try {
            // Simulate an I/O timeout
            throw TimeoutException("Device communication timeout");
        } catch (const TimeoutException& e) {
            ios_failure_caught = true;
            std::string msg = e.what();
            assert(msg.find("timeout") != std::string::npos);
        } catch (const std::ios_base::failure& e) {
            assert(false); // Should catch more specific exception
        }

        assert(timeout_caught);
        assert(ios_failure_caught);

        std::cout << "✓ Exception handling workflow works correctly" << std::endl;
    }

    // Test 5: Constants and position mapping
    {
        std::cout << "\n--- Test 5: Constants and Position Mapping ---" << std::endl;

        // Create a data array using the position constants
        double sensor_readings[POS_COUNT];

        // Fill with test data using position constants
        sensor_readings[POS_TIME] = 1234567890.0;  // timestamp
        sensor_readings[POS_AX] = 0.5;             // acceleration X
        sensor_readings[POS_AY] = -0.3;            // acceleration Y
        sensor_readings[POS_AZ] = 9.8;             // acceleration Z (gravity)
        sensor_readings[POS_GX] = 0.1;             // gyro X
        sensor_readings[POS_GY] = -0.05;           // gyro Y
        sensor_readings[POS_GZ] = 0.02;            // gyro Z
        sensor_readings[POS_LAT] = 45.123456;      // latitude
        sensor_readings[POS_LON] = 9.654321;       // longitude
        sensor_readings[POS_ALT] = 123.45;         // altitude
        sensor_readings[POS_SPEED] = 25.5;         // speed
        sensor_readings[POS_HEAD] = 180.0;         // heading
        sensor_readings[POS_QLT] = 3.0;            // GPS quality
        sensor_readings[POS_HDOP] = 1.2;           // HDOP

        // Verify all positions are within bounds
        for(int i = 0; i < POS_COUNT; i++) {
            assert(sensor_readings[i] != 0.0 || i == POS_TIME); // All set except possibly time
        }

        // Test specific accessor patterns
        assert(sensor_readings[POS_AZ] > 9.0); // Should be close to gravity
        assert(sensor_readings[POS_LAT] > 0.0 && sensor_readings[POS_LAT] < 90.0); // Valid latitude
        assert(sensor_readings[POS_LON] > 0.0 && sensor_readings[POS_LON] < 180.0); // Valid longitude

        std::cout << "✓ Constants and position mapping work correctly" << std::endl;
    }

    // Test 6: Shared memory with Data structures (if enabled)
    {
        std::cout << "\n--- Test 6: Shared Memory Integration ---" << std::endl;

#if defined(USE_HOST_MEMORY)
        const char* test_name = "integration_test_memory";

        // Allocate shared memory for an array of Data structures
        const size_t num_samples = 10;
        Data* shared_data = static_cast<Data*>(
            allocate_host_memory(test_name, num_samples * sizeof(Data))
        );

        // Fill with test data
        for(size_t i = 0; i < num_samples; i++) {
            std::array<double, 6> test_inertial = {
                static_cast<double>(i), static_cast<double>(i+1), static_cast<double>(i+2),
                static_cast<double>(i+3), static_cast<double>(i+4), static_cast<double>(i+5)
            };
            shared_data[i].set(test_inertial);
        }

        // Access from "another process" (same process, different pointer)
        Data* shared_data2 = static_cast<Data*>(get_host_allocated_memory(test_name));

        // Verify data integrity across shared memory access
        for(size_t i = 0; i < num_samples; i++) {
            for(int j = 1; j < 7; j++) {
                assert(shared_data2[i].d[j] == shared_data[i].d[j]);
                assert(shared_data2[i].d[j] == static_cast<double>(i + j - 1));
            }
        }

        // Clean up
        remove_host_memory(test_name);

        std::cout << "✓ Shared memory integration works correctly" << std::endl;
#else
        std::cout << "✓ Shared memory integration skipped (USE_HOST_MEMORY not defined)" << std::endl;
#endif
    }

    // Test 7: String formatting with separators
    {
        std::cout << "\n--- Test 7: String Formatting Integration ---" << std::endl;

        // Create a CSV-like string using the defined separators
        std::string data_line;
        data_line += "2023";
        data_line += TIME_SEPARATION_VALUE;
        data_line += "12";
        data_line += TIME_SEPARATION_VALUE;
        data_line += "25";
        data_line += TIME_SEPARATION_VALUE;
        data_line += "10";
        data_line += TIME_SEPARATION_VALUE;
        data_line += "30";
        data_line += TIME_SEPARATION_VALUE;
        data_line += "45";

        // Add sensor data
        data_line += COMMA_SEPARATION_VALUE;
        data_line += "1.5";
        data_line += COMMA_SEPARATION_VALUE;
        data_line += "2.5";
        data_line += COMMA_SEPARATION_VALUE;
        data_line += "3.5";

        std::string expected = "2023:12:25:10:30:45;1.5;2.5;3.5";
        assert(data_line == expected);

        // Test NavData string generation
        NavData nav;
        nav.setTime_s("2023:12:25:10:30:45");
        double acc[3] = {1.5, 2.5, 3.5};
        nav.setAcc(acc);

        std::string nav_string = nav.to_string();
        assert(nav_string.find("2023:12:25:10:30:45") == 0);
        assert(nav_string.find("1.5;2.5;3.5") != std::string::npos);

        std::cout << "✓ String formatting integration works correctly" << std::endl;
    }

    std::cout << "\n=== All Integration Tests Passed! ===" << std::endl;
    std::cout << "✓ Data structures work together correctly" << std::endl;
    std::cout << "✓ Configuration classes are properly integrated" << std::endl;
    std::cout << "✓ Utility functions support the main functionality" << std::endl;
    std::cout << "✓ Error handling is consistent across components" << std::endl;
    std::cout << "✓ Constants and enums work in realistic scenarios" << std::endl;
    std::cout << "✓ Memory management integrates with data structures" << std::endl;
    std::cout << "✓ String formatting supports data serialization" << std::endl;

    return 0;
}
