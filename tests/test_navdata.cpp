#include "data_tools.hpp"
#include <cassert>
#include <iostream>
#include <ctime>
#include <sstream>

int main() {
    std::cout << "Testing NavData class..." << std::endl;

    NavData nav;

    // Test acceleration data
    double acc[3] = {1.0, 2.0, 3.0};
    nav.setAcc(acc);
    auto val = nav.getAcc();
    assert(val[0] == 1.0);
    assert(val[1] == 2.0);
    assert(val[2] == 3.0);
    std::cout << "✓ Acceleration data set/get works correctly" << std::endl;

    // Test gyroscope data
    double gyr[3] = {4.0, 5.0, 6.0};
    nav.setGyr(gyr);
    auto gv = nav.getGyr();
    assert(gv[0] == 4.0);
    assert(gv[1] == 5.0);
    assert(gv[2] == 6.0);
    std::cout << "✓ Gyroscope data set/get works correctly" << std::endl;

    // Test individual accelerometer access
    assert(nav.getAcc(0) == 1.0);
    assert(nav.getAcc(1) == 2.0);
    assert(nav.getAcc(2) == 3.0);
    std::cout << "✓ Individual accelerometer access works correctly" << std::endl;

    // Test individual gyroscope access
    assert(nav.getGyr(0) == 4.0);
    assert(nav.getGyr(1) == 5.0);
    assert(nav.getGyr(2) == 6.0);
    std::cout << "✓ Individual gyroscope access works correctly" << std::endl;

    // Test speed
    nav.setSpeed(1.5);
    assert(nav.getSpeed() == 1.5);
    std::cout << "✓ Speed set/get works correctly" << std::endl;

    // Test GPS coordinates
    nav.setLat(45.123456);
    nav.setLon(9.654321);
    nav.setAlt(100.5);
    assert(nav.getLat() == 45.123456);
    assert(nav.getLon() == 9.654321);
    assert(nav.getAlt() == 100.5);
    std::cout << "✓ GPS coordinates set/get work correctly" << std::endl;

    // Test heading, quality, HDOP
    nav.setHead(180.5);
    nav.setQlt(3.0);
    nav.setHDOP(1.2);
    assert(nav.getHead() == 180.5);
    assert(nav.getQlt() == 3.0);
    assert(nav.getHDOP() == 1.2);
    std::cout << "✓ Heading, quality, and HDOP set/get work correctly" << std::endl;

    // Test time string
    nav.setTime_s("2020:01:02:03:04:05");
    assert(nav.getTime() == "2020:01:02:03:04:05");
    std::cout << "✓ Time string set/get works correctly" << std::endl;

    // Test time_t setting
    time_t test_time = 1577836800; // 2020-01-01 00:00:00 UTC
    nav.setTime(test_time);
    std::string time_str = nav.getTime();
    assert(!time_str.empty());
    std::cout << "✓ Time_t setting works correctly" << std::endl;

    // Test inertial data retrieval
    auto inertial = nav.getInertial();
    assert(inertial.size() == 6);
    assert(inertial[0] == 1.0); // acc[0]
    assert(inertial[1] == 2.0); // acc[1]
    assert(inertial[2] == 3.0); // acc[2]
    assert(inertial[3] == 4.0); // gyr[0]
    assert(inertial[4] == 5.0); // gyr[1]
    assert(inertial[5] == 6.0); // gyr[2]
    std::cout << "✓ Inertial data retrieval works correctly" << std::endl;

    // Test string representation
    auto str = nav.to_string();
    assert(str.find("2020:01:02:03:04:05") == 0 || str.find(time_str) == 0);
    assert(str.find("1;2;3") != std::string::npos);
    assert(str.find("4;5;6") != std::string::npos);
    std::cout << "✓ String representation contains expected data" << std::endl;

    // Test with float arrays
    float acc_f[3] = {10.1f, 20.2f, 30.3f};
    float gyr_f[3] = {40.4f, 50.5f, 60.6f};
    nav.setAcc(acc_f);
    nav.setGyr(gyr_f);
    auto val_f = nav.getAcc();
    auto gv_f = nav.getGyr();
    assert(abs(val_f[0] - 10.1) < 0.001);
    assert(abs(val_f[1] - 20.2) < 0.001);
    assert(abs(val_f[2] - 30.3) < 0.001);
    assert(abs(gv_f[0] - 40.4) < 0.001);
    assert(abs(gv_f[1] - 50.5) < 0.001);
    assert(abs(gv_f[2] - 60.6) < 0.001);
    std::cout << "✓ Float array input works correctly" << std::endl;

    // Test negative values
    double neg_acc[3] = {-1.0, -2.0, -3.0};
    nav.setAcc(neg_acc);
    auto neg_val = nav.getAcc();
    assert(neg_val[0] == -1.0);
    assert(neg_val[1] == -2.0);
    assert(neg_val[2] == -3.0);
    std::cout << "✓ Negative values work correctly" << std::endl;

    // Test zero values
    double zero_acc[3] = {0.0, 0.0, 0.0};
    nav.setAcc(zero_acc);
    auto zero_val = nav.getAcc();
    assert(zero_val[0] == 0.0);
    assert(zero_val[1] == 0.0);
    assert(zero_val[2] == 0.0);
    std::cout << "✓ Zero values work correctly" << std::endl;

    std::cout << "All NavData tests passed!" << std::endl;
    return 0;
}
