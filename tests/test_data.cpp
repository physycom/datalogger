#include "datalogger.h"
#include <array>
#include <cassert>

int main() {
    Data d{};
    std::array<double,6> inertial{{1,2,3,4,5,6}};
    d.set(inertial);
    for(int i=1;i<7;i++) assert(d.d[i]==inertial[i-1]);
    std::array<double,3> acc{{7,8,9}};
    d.setAcc(acc);
    for(int i=1;i<4;i++) assert(d.d[i]==acc[i-1]);
    return 0;
}
