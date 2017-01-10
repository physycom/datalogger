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

SerialOptions::SerialOptions() : device(), baudrate(9600), timeout(seconds(0)), parity(noparity), csize(8), flow(noflow), stop(one) {}
SerialOptions::SerialOptions(const std::string& device, unsigned int baudrate, time_duration timeout = seconds(0), Parity parity = noparity, unsigned char csize = 8, FlowControl flow = noflow, StopBits stop = one) : device(device), baudrate(baudrate), timeout(timeout), parity(parity), csize(csize), flow(flow), stop(stop) {}

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

  pImpl->port.async_read_some(boost::asio::buffer(s, (size_t)n), boost::bind(&SerialDevice::readCompleted,
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
    boost::asio::write(pImpl->port, boost::asio::buffer(s, (size_t)n));
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
      boost::asio::buffer(pImpl->readBuffer, pImpl->readBufferSize),
      boost::bind(&SerialDevice::readCompleted, this, boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
    return;
  }
#else //Linux
  if (error.value() == 125) return; //Linux outputs error 125
#endif

  pImpl->result = resultError;
}


#ifndef __APPLE__

class AsyncSerialImpl : private boost::noncopyable
{
public:
  AsyncSerialImpl() : io(), port(io), backgroundThread(), open(false),
    error(false) {}

  boost::asio::io_service io; ///< Io service object
  boost::asio::serial_port port; ///< Serial port object
  boost::thread backgroundThread; ///< Thread that runs read/write operations
  bool open; ///< True if port open
  bool error; ///< Error flag
  mutable boost::mutex errorMutex; ///< Mutex for access to error

  /// Data are queued here before they go in writeBuffer
  std::vector<char> writeQueue;
  boost::shared_array<char> writeBuffer; ///< Data being written
  size_t writeBufferSize; ///< Size of writeBuffer
  boost::mutex writeQueueMutex; ///< Mutex for access to writeQueue
  char readBuffer[AsyncSerial::readBufferSize]; ///< data being read

  /// Read complete callback
  boost::function<void(const char*, size_t)> callback;
};

AsyncSerial::AsyncSerial() : pimpl(new AsyncSerialImpl)
{

}

AsyncSerial::AsyncSerial(const std::string& devname, unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
  : pimpl(new AsyncSerialImpl)
{
  open(devname, baud_rate, opt_parity, opt_csize, opt_flow, opt_stop);
}

void AsyncSerial::open(const std::string& devname, unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
{
  if (isOpen()) close();

  setErrorStatus(true);//If an exception is thrown, error_ remains true
  pimpl->port.open(devname);
  pimpl->port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
  pimpl->port.set_option(opt_parity);
  pimpl->port.set_option(opt_csize);
  pimpl->port.set_option(opt_flow);
  pimpl->port.set_option(opt_stop);

  //This gives some work to the io_service before it is started
  pimpl->io.post(boost::bind(&AsyncSerial::doRead, this));

  boost::thread t(boost::bind(&boost::asio::io_service::run, &pimpl->io));
  pimpl->backgroundThread.swap(t);
  setErrorStatus(false);//If we get here, no error
  pimpl->open = true; //Port is now open
}

bool AsyncSerial::isOpen() const
{
  return pimpl->open;
}

bool AsyncSerial::errorStatus() const
{
  boost::lock_guard<boost::mutex> l(pimpl->errorMutex);
  return pimpl->error;
}

void AsyncSerial::close()
{
  if (!isOpen()) return;

  pimpl->open = false;
  pimpl->io.post(boost::bind(&AsyncSerial::doClose, this));
  pimpl->backgroundThread.join();
  pimpl->io.reset();
  if (errorStatus())
  {
    throw(boost::system::system_error(boost::system::error_code(),
      "Error while closing the device"));
  }
}

void AsyncSerial::write(const char *data, size_t size)
{
  {
    boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
    pimpl->writeQueue.insert(pimpl->writeQueue.end(), data, data + size);
  }
  pimpl->io.post(boost::bind(&AsyncSerial::doWrite, this));
}

void AsyncSerial::write(const std::vector<char>& data)
{
  {
    boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
    pimpl->writeQueue.insert(pimpl->writeQueue.end(), data.begin(),
      data.end());
  }
  pimpl->io.post(boost::bind(&AsyncSerial::doWrite, this));
}

void AsyncSerial::writeString(const std::string& s)
{
  {
    boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
    pimpl->writeQueue.insert(pimpl->writeQueue.end(), s.begin(), s.end());
  }
  pimpl->io.post(boost::bind(&AsyncSerial::doWrite, this));
}

AsyncSerial::~AsyncSerial()
{
  if (isOpen())
  {
    try {
      close();
    }
    catch (...)
    {
      //Don't throw from a destructor
    }
  }
}

void AsyncSerial::doRead()
{
  pimpl->port.async_read_some(boost::asio::buffer(pimpl->readBuffer, readBufferSize),
    boost::bind(&AsyncSerial::readEnd, this,
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred));
}

void AsyncSerial::readEnd(const boost::system::error_code& error,
  size_t bytes_transferred)
{
  if (error)
  {
#ifdef __APPLE__
    if (error.value() == 45)
    {
      //Bug on OS X, it might be necessary to repeat the setup
      //http://osdir.com/ml/lib.boost.asio.user/2008-08/msg00004.html
      doRead();
      return;
    }
#endif //__APPLE__
    //error can be true even because the serial port was closed.
    //In this case it is not a real error, so ignore
    if (isOpen())
    {
      doClose();
      setErrorStatus(true);
    }
  }
  else {
    if (pimpl->callback) pimpl->callback(pimpl->readBuffer,
      bytes_transferred);
    doRead();
  }
}

void AsyncSerial::doWrite()
{
  //If a write operation is already in progress, do nothing
  if (pimpl->writeBuffer == 0)
  {
    boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
    pimpl->writeBufferSize = pimpl->writeQueue.size();
    pimpl->writeBuffer.reset(new char[pimpl->writeQueue.size()]);
    copy(pimpl->writeQueue.begin(), pimpl->writeQueue.end(),
      pimpl->writeBuffer.get());
    pimpl->writeQueue.clear();
    async_write(pimpl->port, boost::asio::buffer(pimpl->writeBuffer.get(),
      pimpl->writeBufferSize),
      boost::bind(&AsyncSerial::writeEnd, this, boost::asio::placeholders::error));
  }
}

void AsyncSerial::writeEnd(const boost::system::error_code& error)
{
  if (!error)
  {
    boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
    if (pimpl->writeQueue.empty())
    {
      pimpl->writeBuffer.reset();
      pimpl->writeBufferSize = 0;

      return;
    }
    pimpl->writeBufferSize = pimpl->writeQueue.size();
    pimpl->writeBuffer.reset(new char[pimpl->writeQueue.size()]);
    copy(pimpl->writeQueue.begin(), pimpl->writeQueue.end(),
      pimpl->writeBuffer.get());
    pimpl->writeQueue.clear();
    async_write(pimpl->port, boost::asio::buffer(pimpl->writeBuffer.get(),
      pimpl->writeBufferSize),
      boost::bind(&AsyncSerial::writeEnd, this, boost::asio::placeholders::error));
  }
  else {
    setErrorStatus(true);
    doClose();
  }
}

void AsyncSerial::doClose()
{
  boost::system::error_code ec;
  pimpl->port.cancel(ec);
  if (ec) setErrorStatus(true);
  pimpl->port.close(ec);
  if (ec) setErrorStatus(true);
}

void AsyncSerial::setErrorStatus(bool e)
{
  boost::lock_guard<boost::mutex> l(pimpl->errorMutex);
  pimpl->error = e;
}

void AsyncSerial::setReadCallback(const boost::function<void(const char*, size_t)>& callback)
{
  pimpl->callback = callback;
}

void AsyncSerial::clearReadCallback()
{
  pimpl->callback.clear();
}

#else //__APPLE__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

class AsyncSerialImpl : private boost::noncopyable
{
public:
  AsyncSerialImpl() : backgroundThread(), open(false), error(false) {}

  boost::thread backgroundThread; ///< Thread that runs read operations
  bool open; ///< True if port open
  bool error; ///< Error flag
  mutable boost::mutex errorMutex; ///< Mutex for access to error

  int fd; ///< File descriptor for serial port

  char readBuffer[AsyncSerial::readBufferSize]; ///< data being read

  /// Read complete callback
  boost::function<void(const char*, size_t)> callback;
};

AsyncSerial::AsyncSerial() : pimpl(new AsyncSerialImpl)
{

}

AsyncSerial::AsyncSerial(const std::string& devname, unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
  : pimpl(new AsyncSerialImpl)
{
  open(devname, baud_rate, opt_parity, opt_csize, opt_flow, opt_stop);
}

void AsyncSerial::open(const std::string& devname, unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
{
  if (isOpen()) close();

  setErrorStatus(true);//If an exception is thrown, error remains true

  struct termios new_attributes;
  speed_t speed;
  int status;

  // Open port
  pimpl->fd = ::open(devname.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (pimpl->fd<0) throw(boost::system::system_error(
    boost::system::error_code(), "Failed to open port"));

  // Set Port parameters.
  status = tcgetattr(pimpl->fd, &new_attributes);
  if (status<0 || !isatty(pimpl->fd))
  {
    ::close(pimpl->fd);
    throw(boost::system::system_error(
      boost::system::error_code(), "Device is not a tty"));
  }
  new_attributes.c_iflag = IGNBRK;
  new_attributes.c_oflag = 0;
  new_attributes.c_lflag = 0;
  new_attributes.c_cflag = (CS8 | CREAD | CLOCAL);//8 data bit,Enable receiver,Ignore modem
  /* In non canonical mode (Ctrl-C and other disabled, no echo,...) VMIN and VTIME work this way:
  if the function read() has'nt read at least VMIN chars it waits until has read at least VMIN
  chars (even if VTIME timeout expires); once it has read at least vmin chars, if subsequent
  chars do not arrive before VTIME expires, it returns error; if a char arrives, it resets the
  timeout, so the internal timer will again start from zero (for the nex char,if any)*/
  new_attributes.c_cc[VMIN] = 1;// Minimum number of characters to read before returning error
  new_attributes.c_cc[VTIME] = 1;// Set timeouts in tenths of second

  // Set baud rate
  switch (baud_rate)
  {
  case 50:speed = B50; break;
  case 75:speed = B75; break;
  case 110:speed = B110; break;
  case 134:speed = B134; break;
  case 150:speed = B150; break;
  case 200:speed = B200; break;
  case 300:speed = B300; break;
  case 600:speed = B600; break;
  case 1200:speed = B1200; break;
  case 1800:speed = B1800; break;
  case 2400:speed = B2400; break;
  case 4800:speed = B4800; break;
  case 9600:speed = B9600; break;
  case 19200:speed = B19200; break;
  case 38400:speed = B38400; break;
  case 57600:speed = B57600; break;
  case 115200:speed = B115200; break;
  case 230400:speed = B230400; break;
  default:
  {
    ::close(pimpl->fd);
    throw(boost::system::system_error(
      boost::system::error_code(), "Unsupported baud rate"));
  }
  }

  cfsetospeed(&new_attributes, speed);
  cfsetispeed(&new_attributes, speed);

  //Make changes effective
  status = tcsetattr(pimpl->fd, TCSANOW, &new_attributes);
  if (status<0)
  {
    ::close(pimpl->fd);
    throw(boost::system::system_error(
      boost::system::error_code(), "Can't set port attributes"));
  }

  //These 3 lines clear the O_NONBLOCK flag
  status = fcntl(pimpl->fd, F_GETFL, 0);
  if (status != -1) fcntl(pimpl->fd, F_SETFL, status & ~O_NONBLOCK);

  setErrorStatus(false);//If we get here, no error
  pimpl->open = true; //Port is now open

  boost::thread t(boost::bind(&AsyncSerial::doRead, this));
  pimpl->backgroundThread.swap(t);
}

bool AsyncSerial::isOpen() const
{
  return pimpl->open;
}

bool AsyncSerial::errorStatus() const
{
  boost::lock_guard<boost::mutex> l(pimpl->errorMutex);
  return pimpl->error;
}

void AsyncSerial::close()
{
  if (!isOpen()) return;

  pimpl->open = false;

  ::close(pimpl->fd); //The thread waiting on I/O should return

  pimpl->backgroundThread.join();
  if (errorStatus())
  {
    throw(boost::system::system_error(boost::system::error_code(),
      "Error while closing the device"));
  }
}

void AsyncSerial::write(const char *data, size_t size)
{
  if (::write(pimpl->fd, data, size) != size) setErrorStatus(true);
}

void AsyncSerial::write(const std::vector<char>& data)
{
  if (::write(pimpl->fd, &data[0], data.size()) != data.size())
    setErrorStatus(true);
}

void AsyncSerial::writeString(const std::string& s)
{
  if (::write(pimpl->fd, &s[0], s.size()) != s.size()) setErrorStatus(true);
}

AsyncSerial::~AsyncSerial()
{
  if (isOpen())
  {
    try {
      close();
    }
    catch (...)
    {
      //Don't throw from a destructor
    }
  }
}

void AsyncSerial::doRead()
{
  //Read loop in spawned thread
  for (;;)
  {
    int received = ::read(pimpl->fd, pimpl->readBuffer, readBufferSize);
    if (received<0)
    {
      if (isOpen() == false) return; //Thread interrupted because port closed
      else {
        setErrorStatus(true);
        continue;
      }
    }
    if (pimpl->callback) pimpl->callback(pimpl->readBuffer, received);
  }
}

void AsyncSerial::readEnd(const boost::system::error_code& error,
  size_t bytes_transferred)
{
  //Not used
}

void AsyncSerial::doWrite()
{
  //Not used
}

void AsyncSerial::writeEnd(const boost::system::error_code& error)
{
  //Not used
}

void AsyncSerial::doClose()
{
  //Not used
}

void AsyncSerial::setErrorStatus(bool e)
{
  boost::lock_guard<boost::mutex> l(pimpl->errorMutex);
  pimpl->error = e;
}

void AsyncSerial::setReadCallback(const
  boost::function<void(const char*, size_t)>& callback)
{
  pimpl->callback = callback;
}

void AsyncSerial::clearReadCallback()
{
  pimpl->callback.clear();
}

#endif //__APPLE__

//
//Class CallbackAsyncSerial
//

CallbackAsyncSerial::CallbackAsyncSerial() : AsyncSerial()
{

}

CallbackAsyncSerial::CallbackAsyncSerial(const std::string& devname,
  unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
  : AsyncSerial(devname, baud_rate, opt_parity, opt_csize, opt_flow, opt_stop)
{

}

void CallbackAsyncSerial::setCallback(const
  boost::function<void(const char*, size_t)>& callback)
{
  setReadCallback(callback);
}

void CallbackAsyncSerial::clearCallback()
{
  clearReadCallback();
}

CallbackAsyncSerial::~CallbackAsyncSerial()
{
  clearReadCallback();
}


BufferedAsyncSerial::BufferedAsyncSerial() : AsyncSerial()
{
  setReadCallback(boost::bind(&BufferedAsyncSerial::readCallback, this, _1, _2));
}

BufferedAsyncSerial::BufferedAsyncSerial(const std::string& devname,
  unsigned int baud_rate,
  boost::asio::serial_port_base::parity opt_parity,
  boost::asio::serial_port_base::character_size opt_csize,
  boost::asio::serial_port_base::flow_control opt_flow,
  boost::asio::serial_port_base::stop_bits opt_stop)
  : AsyncSerial(devname, baud_rate, opt_parity, opt_csize, opt_flow, opt_stop)
{
  setReadCallback(boost::bind(&BufferedAsyncSerial::readCallback, this, _1, _2));
}

size_t BufferedAsyncSerial::read(char *data, size_t size)
{
  boost::lock_guard<boost::mutex> l(readQueueMutex);
  size_t result = std::min(size, readQueue.size());
  std::vector<char>::iterator it = readQueue.begin() + result;
  std::copy(readQueue.begin(), it, data);
  readQueue.erase(readQueue.begin(), it);
  return result;
}

std::vector<char> BufferedAsyncSerial::read()
{
  boost::lock_guard<boost::mutex> l(readQueueMutex);
  std::vector<char> result;
  result.swap(readQueue);
  return result;
}

std::string BufferedAsyncSerial::readString()
{
  boost::lock_guard<boost::mutex> l(readQueueMutex);
  std::string result(readQueue.begin(), readQueue.end());
  readQueue.clear();
  return result;
}

std::string BufferedAsyncSerial::readStringUntil(const std::string delim)
{
  boost::lock_guard<boost::mutex> l(readQueueMutex);
  std::vector<char>::iterator it = findStringInVector(readQueue, delim);
  if (it == readQueue.end()) return "";
  std::string result(readQueue.begin(), it);
  it += delim.size();//Do remove the delimiter from the queue
  readQueue.erase(readQueue.begin(), it);
  return result;
}

void BufferedAsyncSerial::readCallback(const char *data, size_t len)
{
  boost::lock_guard<boost::mutex> l(readQueueMutex);
  readQueue.insert(readQueue.end(), data, data + len);
}

std::vector<char>::iterator BufferedAsyncSerial::findStringInVector(
  std::vector<char>& v, const std::string& s)
{
  if (s.size() == 0) return v.end();

  std::vector<char>::iterator it = v.begin();
  for (;;)
  {
    std::vector<char>::iterator result = find(it, v.end(), s[0]);
    if (result == v.end()) return v.end();//If not found return

    for (size_t i = 0; i<s.size(); i++)
    {
      std::vector<char>::iterator temp = result + i;
      if (temp == v.end()) return v.end();
      if (s[i] != *temp) goto mismatch;
    }
    //Found
    return result;

  mismatch:
    it = result + 1;
  }
}

BufferedAsyncSerial::~BufferedAsyncSerial()
{
  clearReadCallback();
}


