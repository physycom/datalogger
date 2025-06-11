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
    return 0;
}
