/*
* Authors: Federico Terraneo, Stefano Sinigardi, Alessandro Fabbri
* Distributed under the Boost Software License, Version 1.0.
*/


#include "serial_tools.h"


TimeoutSerial::ReadSetupParameters::ReadSetupParameters() : fixedSize(false), delim(""), data(0), size(0) {}
TimeoutSerial::ReadSetupParameters::ReadSetupParameters(const std::string& delim) : fixedSize(false), delim(delim), data(0), size(0) { }
TimeoutSerial::ReadSetupParameters::ReadSetupParameters(char *data, size_t size) : fixedSize(true), delim(""), data(data), size(size) { }

SimpleSerial::SimpleSerial(std::string port, unsigned int baud_rate) : io(), serial(io, port)
{
  serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
}

void SimpleSerial::writeString(std::string s)
{
  boost::asio::write(serial, boost::asio::buffer(s.c_str(), s.size()));
}

std::string SimpleSerial::readLine()
{
  using namespace boost;
  char c;
  std::string result;
  for (;;)
  {
    asio::read(serial, asio::buffer(&c, 1));
    switch (c)
    {
    case '\r':
      break;
    case '\n':
      return result;
    default:
      result += c;
    }
  }
}

timeout_exception::timeout_exception(const std::string& arg) : runtime_error(arg) {}
TimeoutException::TimeoutException(const std::string& arg) : failure(arg) {}

TimeoutSerial::TimeoutSerial(const std::string& devname, unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
  : io(), port(io), timer(io), timeout(boost::posix_time::seconds(0))
{
  open(devname, baud_rate, opt_parity, opt_csize, opt_flow, opt_stop);
}

void TimeoutSerial::open(const std::string& devname, unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
{
  if (isOpen()) close();
  port.open(devname);
  port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
  port.set_option(opt_parity);
  port.set_option(opt_csize);
  port.set_option(opt_flow);
  port.set_option(opt_stop);
}

bool TimeoutSerial::isOpen() const
{
  return port.is_open();
}

void TimeoutSerial::close()
{
  if (isOpen() == false) return;
  port.close();
}

void TimeoutSerial::setTimeout(const boost::posix_time::time_duration& t)
{
  timeout = t;
}

void TimeoutSerial::write(const char *data, size_t size)
{
  boost::asio::write(port, boost::asio::buffer(data, size));
}

void TimeoutSerial::write(const std::vector<char>& data)
{
  boost::asio::write(port, boost::asio::buffer(&data[0], data.size()));
}

void TimeoutSerial::writeString(const std::string& s)
{
  boost::asio::write(port, boost::asio::buffer(s.c_str(), s.size()));
}

void TimeoutSerial::read(char *data, size_t size)
{
  if (readData.size() > 0)//If there is some data from a previous read
  {
    std::istream is(&readData);
    size_t toRead = std::min(readData.size(), size);//How many bytes to read?
    is.read(data, toRead);
    data += toRead;
    size -= toRead;
    if (size == 0) return;//If read data was enough, just return
  }

  setupParameters = ReadSetupParameters(data, size);
  performReadSetup(setupParameters);

  //For this code to work, there should always be a timeout, so the
  //request for no timeout is translated into a very long timeout
  if (timeout != boost::posix_time::seconds(0)) timer.expires_from_now(timeout);
  else timer.expires_from_now(boost::posix_time::hours(100000));

  timer.async_wait(boost::bind(&TimeoutSerial::timeoutExpired, this,
    boost::asio::placeholders::error));

  result = resultInProgress;
  bytesTransferred = 0;
  for (;;)
  {
    io.run_one();
    switch (result)
    {
    case resultSuccess:
      timer.cancel();
      return;
    case resultTimeout:
      port.cancel();
      throw(timeout_exception("Timeout expired"));
    case resultError:
      timer.cancel();
      port.cancel();
      throw(boost::system::system_error(boost::system::error_code(),
        "Error while reading"));
      //if resultInProgress remain in the loop
    }
  }
}

std::vector<char> TimeoutSerial::read(size_t size)
{
  std::vector<char> result(size, '\0');//Allocate a vector with the desired size
  read(&result[0], size);//Fill it with values
  return result;
}

std::string TimeoutSerial::readString(size_t size)
{
  std::string result(size, '\0');//Allocate a string with the desired size
  read(&result[0], size);//Fill it with values
  return result;
}

std::string TimeoutSerial::readStringUntil(const std::string& delim)
{
  // Note: if readData contains some previously read data, the call to
  // async_read_until (which is done in performReadSetup) correctly handles
  // it. If the data is enough it will also immediately call readCompleted()
  setupParameters = ReadSetupParameters(delim);
  performReadSetup(setupParameters);

  //For this code to work, there should always be a timeout, so the
  //request for no timeout is translated into a very long timeout
  if (timeout != boost::posix_time::seconds(0)) timer.expires_from_now(timeout);
  else timer.expires_from_now(boost::posix_time::hours(100000));

  timer.async_wait(boost::bind(&TimeoutSerial::timeoutExpired, this,
    boost::asio::placeholders::error));

  result = resultInProgress;
  bytesTransferred = 0;
  for (;;)
  {
    io.run_one();
    switch (result)
    {
    case resultSuccess:
    {
      timer.cancel();
      bytesTransferred -= delim.size();//Don't count delim
      std::istream is(&readData);
      std::string result(bytesTransferred, '\0');//Alloc string
      is.read(&result[0], bytesTransferred);//Fill values
      is.ignore(delim.size());//Remove delimiter from stream
      return result;
    }
    case resultTimeout:
      port.cancel();
      throw(timeout_exception("Timeout expired"));
    case resultError:
      timer.cancel();
      port.cancel();
      throw(boost::system::system_error(boost::system::error_code(),
        "Error while reading"));
      //if resultInProgress remain in the loop
    }
  }
}

void TimeoutSerial::performReadSetup(const ReadSetupParameters& param)
{
  if (param.fixedSize)
  {
    boost::asio::async_read(port, boost::asio::buffer(param.data, param.size), boost::bind(
      &TimeoutSerial::readCompleted, this, boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
  }
  else {
    boost::asio::async_read_until(port, readData, param.delim, boost::bind(
      &TimeoutSerial::readCompleted, this, boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
  }
}

void TimeoutSerial::timeoutExpired(const boost::system::error_code& error)
{
  if (!error && result == resultInProgress) result = resultTimeout;
}

void TimeoutSerial::readCompleted(const boost::system::error_code& error,
  const size_t bytesTransferred)
{
  if (!error)
  {
    result = resultSuccess;
    this->bytesTransferred = bytesTransferred;
    return;
  }

  //In case a asynchronous operation is cancelled due to a timeout,
  //each OS seems to have its way to react.
#ifdef _WIN32
  if (error.value() == 995) return; //Windows spits out error 995
#elif defined(__APPLE__)
  if (error.value() == 45)
  {
    //Bug on OS X, it might be necessary to repeat the setup
    //http://osdir.com/ml/lib.boost.asio.user/2008-08/msg00004.html
    performReadSetup(setupParameters);
    return;
  }
#else //Linux
  if (error.value() == 125) return; //Linux outputs error 125
#endif

  result = resultError;
}

void COMport::set_portname(std::string port)
{
  portname = port;
}

void COMport::set_portname_stdin()
{
  std::cout << "Insert portname (on Win, tipically it is a COMxx, on Unix-like it is a /dev/ttyUSBxx): ";
  std::cin >> portname;
}

std::string COMport::get_portname()
{
  return portname;
}

void COMport::set_baudrate(int baud)
{
  baudrate = baud;
}

void COMport::set_baudrate_stdin()
{
  std::cout << "Insert baudrate (tipically 115200): ";
  std::cin >> baudrate;
}

int COMport::get_baudrate()
{
  return baudrate;
}


SerialOptions::SerialOptions() : device(), baudrate(9600), timeout(seconds(0)), parity(noparity), csize(8), flow(noflow), stop(one) {}
SerialOptions::SerialOptions(const std::string& device, unsigned int baudrate,time_duration timeout = seconds(0), Parity parity = noparity, unsigned char csize = 8, FlowControl flow = noflow, StopBits stop = one) : device(device), baudrate(baudrate), timeout(timeout), parity(parity), csize(csize), flow(flow), stop(stop) {}

void SerialOptions::setDevice(const std::string& device) { this->device = device; }
std::string SerialOptions::getDevice() const { return this->device; }

/**
* Setter and getter for baudrate
*/
void SerialOptions::setBaudrate(unsigned int baudrate) { this->baudrate = baudrate; }
unsigned int SerialOptions::getBaudrate() const { return this->baudrate; }

/**
* Setter and getter for timeout
*/
void SerialOptions::setTimeout(time_duration timeout) { this->timeout = timeout; }
boost::posix_time::time_duration SerialOptions::getTimeout() const { return this->timeout; }

/**
* Setter and getter for parity
*/
void SerialOptions::setParity(Parity parity) { this->parity = parity; }
SerialOptions::Parity SerialOptions::getParity() const { return this->parity; }

/**
* Setter and getter character size
*/
void SerialOptions::setCsize(unsigned char csize) { this->csize = csize; }
unsigned char SerialOptions::getCsize() const { return this->csize; }

/**
* Setter and getter flow control
*/
void SerialOptions::setFlowControl(FlowControl flow) { this->flow = flow; }
SerialOptions::FlowControl SerialOptions::getFlowControl() const { return this->flow; }

/**
* Setter and getter for stop bits
*/
void SerialOptions::setStopBits(StopBits stop) { this->stop = stop; }
SerialOptions::StopBits SerialOptions::getStopBits() const { return this->stop; }



SerialDeviceImpl::SerialDeviceImpl(const SerialOptions& options)
  : io(), port(io), timer(io), timeout(options.getTimeout()),
  result(resultError), bytesTransferred(0), readBuffer(0),
  readBufferSize(0)
{
  try {
    //For this code to work, there should always be a timeout, so the
    //request for no timeout is translated into a very long timeout
    if (timeout == boost::posix_time::seconds(0)) timeout = boost::posix_time::hours(100000);

    port.open(options.getDevice());//Port must be open before setting option

    port.set_option(boost::asio::serial_port_base::baud_rate(options.getBaudrate()));

    switch (options.getParity())
    {
    case SerialOptions::odd:
      port.set_option(boost::asio::serial_port_base::parity(
        boost::asio::serial_port_base::parity::odd));
      break;
    case SerialOptions::even:
      port.set_option(boost::asio::serial_port_base::parity(
        boost::asio::serial_port_base::parity::even));
      break;
    default:
      port.set_option(boost::asio::serial_port_base::parity(
        boost::asio::serial_port_base::parity::none));
      break;
    }

    port.set_option(boost::asio::serial_port_base::character_size(options.getCsize()));

    switch (options.getFlowControl())
    {
    case SerialOptions::hardware:
      port.set_option(boost::asio::serial_port_base::flow_control(
        boost::asio::serial_port_base::flow_control::hardware));
      break;
    case SerialOptions::software:
      port.set_option(boost::asio::serial_port_base::flow_control(
        boost::asio::serial_port_base::flow_control::software));
      break;
    default:
      port.set_option(boost::asio::serial_port_base::flow_control(
        boost::asio::serial_port_base::flow_control::none));
      break;
    }

    switch (options.getStopBits())
    {
    case SerialOptions::onepointfive:
      port.set_option(boost::asio::serial_port_base::stop_bits(
        boost::asio::serial_port_base::stop_bits::onepointfive));
      break;
    case SerialOptions::two:
      port.set_option(boost::asio::serial_port_base::stop_bits(
        boost::asio::serial_port_base::stop_bits::two));
      break;
    default:
      port.set_option(boost::asio::serial_port_base::stop_bits(
        boost::asio::serial_port_base::stop_bits::one));
      break;
    }
  }
  catch (std::exception& e)
  {
    throw std::ios::failure(e.what());
  }
}



SerialDevice::SerialDevice(const SerialOptions& options)
  : pImpl(new SerialDeviceImpl(options)) {}


std::streamsize SerialDevice::read(char *s, std::streamsize n)
{
  pImpl->result = resultInProgress;
  pImpl->bytesTransferred = 0;
  pImpl->readBuffer = s;
  pImpl->readBufferSize = n;

  pImpl->timer.expires_from_now(pImpl->timeout);
  pImpl->timer.async_wait(boost::bind(&SerialDevice::timeoutExpired, this,
    boost::asio::placeholders::error));

  pImpl->port.async_read_some(boost::asio::buffer(s, n), boost::bind(&SerialDevice::readCompleted,
    this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

  for (;;)
  {
    pImpl->io.run_one();
    switch (pImpl->result)
    {
    case resultSuccess:
      pImpl->timer.cancel();
      return pImpl->bytesTransferred;
    case resultTimeout:
      pImpl->port.cancel();
      throw(TimeoutException("Timeout expired"));
    case resultError:
      pImpl->port.cancel();
      pImpl->timer.cancel();
      throw(std::ios_base::failure("Error while reading"));
    default:
      //if resultInProgress remain in the loop
      break;
    }
  }
}

std::streamsize SerialDevice::write(const char *s, std::streamsize n)
{
  try {
    boost::asio::write(pImpl->port, boost::asio::buffer(s, n));
  }
  catch (std::exception& e)
  {
    throw(std::ios_base::failure(e.what()));
  }
  return n;
}

void SerialDevice::timeoutExpired(const boost::system::error_code& error)
{
  if (!error && pImpl->result == resultInProgress) pImpl->result = resultTimeout;
}

void SerialDevice::readCompleted(const boost::system::error_code& error,
  const size_t bytesTransferred)
{
  if (!error)
  {
    pImpl->result = resultSuccess;
    pImpl->bytesTransferred = bytesTransferred;
    return;
  }

  //In case a asynchronous operation is cancelled due to a timeout,
  //each OS seems to have its way to react.
#ifdef _WIN32
  if (error.value() == 995) return; //Windows spits out error 995
#elif defined(__APPLE__)
  if (error.value() == 45)
  {
    //Bug on OS X, it might be necessary to repeat the setup
    //http://osdir.com/ml/lib.boost.asio.user/2008-08/msg00004.html
    pImpl->port.async_read_some(
      asio::buffer(pImpl->readBuffer, pImpl->readBufferSize),
      boost::bind(&SerialDevice::readCompleted, this, boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
    return;
  }
#else //Linux
  if (error.value() == 125) return; //Linux outputs error 125
#endif

  pImpl->result = resultError;
}



