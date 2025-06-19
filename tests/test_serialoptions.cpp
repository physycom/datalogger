#include "serial_tools.h"
#include <cassert>

int main(){
    SerialOptions opt;
    assert(opt.getBaudrate() == 9600);
    opt.setDevice("/dev/ttyS0");
    assert(opt.getDevice() == "/dev/ttyS0");
    opt.setBaudrate(115200);
    assert(opt.getBaudrate() == 115200u);
    opt.setParity(SerialOptions::even);
    assert(opt.getParity() == SerialOptions::even);
    opt.setCsize(7);
    assert(opt.getCsize() == 7);
    opt.setFlowControl(SerialOptions::hardware);
    assert(opt.getFlowControl() == SerialOptions::hardware);
    opt.setStopBits(SerialOptions::two);
    assert(opt.getStopBits() == SerialOptions::two);
    return 0;
}
