#include "data_tools.hpp"
#include <cassert>

int main() {
    NavData nav;
    double acc[3] = {1.0, 2.0, 3.0};
    nav.setAcc(acc);
    auto val = nav.getAcc();
    assert(val[0] == 1.0);
    assert(val[1] == 2.0);
    assert(val[2] == 3.0);

    double gyr[3] = {4.0, 5.0, 6.0};
    nav.setGyr(gyr);
    auto gv = nav.getGyr();
    assert(gv[0] == 4.0);
    assert(gv[1] == 5.0);
    assert(gv[2] == 6.0);

    nav.setSpeed(1.5);
    assert(nav.getSpeed() == 1.5);

    nav.setTime_s("2020:01:02:03:04:05");
    assert(nav.getTime() == "2020:01:02:03:04:05");

    auto str = nav.to_string();
    assert(str.find("2020:01:02:03:04:05") == 0);
    assert(str.find("1;2;3") != std::string::npos);
    assert(str.find("4;5;6") != std::string::npos);
    return 0;
}
