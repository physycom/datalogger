#include "serial_tools.h"
#include <cassert>
#include <iostream>

int main(){
    std::cout << "Testing SerialOptions class..." << std::endl;

    // Test default constructor
    SerialOptions opt;
    assert(opt.getBaudrate() == 9600);
    assert(opt.getDevice() == "");
    assert(opt.getParity() == SerialOptions::noparity);
    assert(opt.getCsize() == 8);
    assert(opt.getFlowControl() == SerialOptions::noflow);
    assert(opt.getStopBits() == SerialOptions::one);
    std::cout << "✓ Default constructor sets correct default values" << std::endl;

    // Test device setting
    opt.setDevice("/dev/ttyS0");
    assert(opt.getDevice() == "/dev/ttyS0");
    std::cout << "✓ Device name set/get works correctly" << std::endl;

    // Test baudrate setting
    opt.setBaudrate(115200);
    assert(opt.getBaudrate() == 115200u);
    std::cout << "✓ Baudrate set/get works correctly" << std::endl;

    // Test parity setting
    opt.setParity(SerialOptions::even);
    assert(opt.getParity() == SerialOptions::even);
    opt.setParity(SerialOptions::odd);
    assert(opt.getParity() == SerialOptions::odd);
    opt.setParity(SerialOptions::noparity);
    assert(opt.getParity() == SerialOptions::noparity);
    std::cout << "✓ Parity set/get works correctly" << std::endl;

    // Test character size setting
    opt.setCsize(7);
    assert(opt.getCsize() == 7);
    opt.setCsize(8);
    assert(opt.getCsize() == 8);
    std::cout << "✓ Character size set/get works correctly" << std::endl;

    // Test flow control setting
    opt.setFlowControl(SerialOptions::hardware);
    assert(opt.getFlowControl() == SerialOptions::hardware);
    opt.setFlowControl(SerialOptions::software);
    assert(opt.getFlowControl() == SerialOptions::software);
    opt.setFlowControl(SerialOptions::noflow);
    assert(opt.getFlowControl() == SerialOptions::noflow);
    std::cout << "✓ Flow control set/get works correctly" << std::endl;

    // Test stop bits setting
    opt.setStopBits(SerialOptions::two);
    assert(opt.getStopBits() == SerialOptions::two);
    opt.setStopBits(SerialOptions::onepointfive);
    assert(opt.getStopBits() == SerialOptions::onepointfive);
    opt.setStopBits(SerialOptions::one);
    assert(opt.getStopBits() == SerialOptions::one);
    std::cout << "✓ Stop bits set/get works correctly" << std::endl;

    // Test Windows COM port
    opt.setDevice("COM1");
    assert(opt.getDevice() == "COM1");
    std::cout << "✓ Windows COM port name works correctly" << std::endl;

    // Test various baudrates
    std::vector<unsigned int> common_baudrates = {110, 300, 600, 1200, 2400, 4800, 9600,
                                                 14400, 19200, 28800, 38400, 57600, 115200,
                                                 230400, 460800, 921600};
    for(auto baudrate : common_baudrates) {
        opt.setBaudrate(baudrate);
        assert(opt.getBaudrate() == baudrate);
    }
    std::cout << "✓ Common baudrates work correctly" << std::endl;

    // Test timeout setting
    opt.setTimeout(boost::posix_time::seconds(5));
    assert(opt.getTimeout() == boost::posix_time::seconds(5));
    opt.setTimeout(boost::posix_time::milliseconds(500));
    assert(opt.getTimeout() == boost::posix_time::milliseconds(500));
    std::cout << "✓ Timeout set/get works correctly" << std::endl;

    // Test copy constructor
    SerialOptions opt2(opt);
    assert(opt2.getDevice() == opt.getDevice());
    assert(opt2.getBaudrate() == opt.getBaudrate());
    assert(opt2.getParity() == opt.getParity());
    assert(opt2.getCsize() == opt.getCsize());
    assert(opt2.getFlowControl() == opt.getFlowControl());
    assert(opt2.getStopBits() == opt.getStopBits());
    assert(opt2.getTimeout() == opt.getTimeout());
    std::cout << "✓ Copy constructor works correctly" << std::endl;

    std::cout << "All SerialOptions tests passed!" << std::endl;
    return 0;
}
