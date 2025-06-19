#include "datalogger.h"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Testing datalogger constants and macros..." << std::endl;

    // Test position constants
    assert(POS_TIME == 0);
    assert(POS_AX == 1);
    assert(POS_AY == 2);
    assert(POS_AZ == 3);
    assert(POS_GX == 4);
    assert(POS_GY == 5);
    assert(POS_GZ == 6);
    assert(POS_LAT == 7);
    assert(POS_LON == 8);
    assert(POS_ALT == 9);
    assert(POS_SPEED == 10);
    assert(POS_HEAD == 11);
    assert(POS_QLT == 12);
    assert(POS_HDOP == 13);
    assert(POS_COUNT == 14);
    std::cout << "✓ Position constants are correctly defined" << std::endl;

    // Test UBX offset constants
    assert(UBX_YEAR_OFFSET == 4);
    assert(UBX_MONTH_OFFSET == 6);
    assert(UBX_DAY_OFFSET == 7);
    assert(UBX_HOUR_OFFSET == 8);
    assert(UBX_MIN_OFFSET == 9);
    assert(UBX_SEC_OFFSET == 10);
    assert(UBX_NANO_OFFSET == 16);
    assert(UBX_FIX_OFFSET == 20);
    assert(UBX_LON_OFFSET == 24);
    assert(UBX_LAT_OFFSET == 28);
    assert(UBX_ALT_OFFSET == 36);
    assert(UBX_SPEED_OFFSET == 60);
    assert(UBX_HEAD_OFFSET == 60);
    std::cout << "✓ UBX offset constants are correctly defined" << std::endl;

    // Test time constants
    assert(EPOCH_TIME_2000 == 946684800);
    std::cout << "✓ Epoch time constant is correct" << std::endl;

    // Test separator constants
    assert(COMMA_SEPARATION_VALUE == ';');
    assert(TIME_SEPARATION_VALUE == ':');
    std::cout << "✓ Separator constants are correctly defined" << std::endl;

    // Test dimension constant
    assert(DIMENSIONE_MAX == 1000);
    assert(DIMENSIONE_MAX > 0);
    std::cout << "✓ DIMENSIONE_MAX constant is correct" << std::endl;

    // Test sleep time constant
    assert(SLEEP_TIME_MICROSECONDS == 2500);
    assert(SLEEP_TIME_MICROSECONDS > 0);
    std::cout << "✓ Sleep time constant is correct" << std::endl;

    // Test serial port timeout
    assert(SERIAL_PORT_TIMEOUT_SECONDS == 3);
    assert(SERIAL_PORT_TIMEOUT_SECONDS > 0);
    std::cout << "✓ Serial port timeout constant is correct" << std::endl;

    // Test that position constants can be used as array indices
    double test_array[POS_COUNT];
    for(int i = 0; i < POS_COUNT; i++) {
        test_array[i] = static_cast<double>(i);
    }

    assert(test_array[POS_TIME] == 0.0);
    assert(test_array[POS_AX] == 1.0);
    assert(test_array[POS_HDOP] == 13.0);
    std::cout << "✓ Position constants work as array indices" << std::endl;

    // Test that constants are in logical order
    assert(POS_AX == POS_TIME + 1);
    assert(POS_AY == POS_AX + 1);
    assert(POS_AZ == POS_AY + 1);
    assert(POS_GX == POS_AZ + 1);
    assert(POS_GY == POS_GX + 1);
    assert(POS_GZ == POS_GY + 1);
    std::cout << "✓ Position constants are in logical sequence" << std::endl;

    // Test preprocessor flags (just verify they compile)
    #ifdef USE_HOST_MEMORY
    std::cout << "✓ USE_HOST_MEMORY is defined" << std::endl;
    #else
    std::cout << "✓ USE_HOST_MEMORY is not defined" << std::endl;
    #endif

    #ifdef WRITE_ON_STDOUT
    std::cout << "✓ WRITE_ON_STDOUT is defined" << std::endl;
    #else
    std::cout << "✓ WRITE_ON_STDOUT is not defined" << std::endl;
    #endif

    #ifdef BYPASS_CHECK
    if (BYPASS_CHECK) {
        std::cout << "✓ BYPASS_CHECK is true" << std::endl;
    } else {
        std::cout << "✓ BYPASS_CHECK is false" << std::endl;
    }
    #else
    std::cout << "✓ BYPASS_CHECK is not defined" << std::endl;
    #endif

    // Test separator usage in string operations
    std::string test_csv = "1";
    test_csv += COMMA_SEPARATION_VALUE;
    test_csv += "2";
    test_csv += COMMA_SEPARATION_VALUE;
    test_csv += "3";
    assert(test_csv == "1;2;3");
    std::cout << "✓ Comma separator works in string operations" << std::endl;

    std::string test_time = "2023";
    test_time += TIME_SEPARATION_VALUE;
    test_time += "12";
    test_time += TIME_SEPARATION_VALUE;
    test_time += "25";
    assert(test_time == "2023:12:25");
    std::cout << "✓ Time separator works in string operations" << std::endl;

    std::cout << "All datalogger constants and macros tests passed!" << std::endl;
    return 0;
}
