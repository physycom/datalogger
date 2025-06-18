/*
* Authors: Federico Terraneo, Stefano Sinigardi, Alessandro Fabbri
* Distributed under the Boost Software License, Version 1.0.
*/



#pragma once

#include "datalogger.h"

/**
* Possible outcome of a read. Set by callbacks, read from main code
*/
enum ReadResult
{
  resultInProgress,
  resultSuccess,
  resultError,
  resultTimeout
};


class SimpleSerial
{
public:
  /**
  * Constructor.
  * \param port device name, example "/dev/ttyUSB0" or "COM4"
  * \param baud_rate communication speed, example 9600 or 115200
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  SimpleSerial(std::string port, unsigned int baud_rate);

  /**
  * Write a string to the serial device.
  * \param s string to write
  * \throws boost::system::system_error on failure
  */
  void writeString(std::string s);

  /**
  * Blocks until a line is received from the serial device.
  * Eventual '\n' or '\r\n' characters at the end of the string are removed.
  * \return a string containing the received line
  * \throws boost::system::system_error on failure
  */
  std::string readLine();

private:
  boost::asio::io_context io;
  boost::asio::serial_port serial;
};

class timeout_exception : public std::runtime_error
{
public:
  timeout_exception(const std::string& arg);
};

class TimeoutException : public std::ios_base::failure
{
public:
  TimeoutException(const std::string& arg);
};


/**
* Serial port class, with time-out on read operations.
*/
class TimeoutSerial : private boost::noncopyable
{
public:

  /**
  * Opens a serial device. By default time-out is disabled.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8 bit
  * \param opt_flow serial flow control, default none
  * \param opt_stop serial stop bits, default 1
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  TimeoutSerial(const std::string& devname, unsigned int baud_rate,
    boost::asio::serial_port_base::parity opt_parity =
    boost::asio::serial_port_base::parity(
    boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size opt_csize =
    boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control opt_flow =
    boost::asio::serial_port_base::flow_control(
    boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits opt_stop =
    boost::asio::serial_port_base::stop_bits(
    boost::asio::serial_port_base::stop_bits::one));

  /**
  * Opens a serial device.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8 bit
  * \param opt_flow serial flow control, default none
  * \param opt_stop serial stop bits, default 1
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  void open(const std::string& devname, unsigned int baud_rate,
    boost::asio::serial_port_base::parity opt_parity =
    boost::asio::serial_port_base::parity(
    boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size opt_csize =
    boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control opt_flow =
    boost::asio::serial_port_base::flow_control(
    boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits opt_stop =
    boost::asio::serial_port_base::stop_bits(
    boost::asio::serial_port_base::stop_bits::one));


  /**
  * \return true if serial device is open
  */
  bool isOpen() const;

  /**
  * Close the serial device
  * \throws boost::system::system_error if any error
  */
  void close();

  /**
  * Set the time-out on read/write operations.
  * To disable the time-out, call setTimeout(boost::posix_time::seconds(0));
  */
  void setTimeout(const boost::posix_time::time_duration& t);

  /**
  * Write data
  * \param data array of char to be sent through the serial device
  * \param size array size
  * \throws boost::system::system_error if any error
  */
  void write(const char *data, size_t size);

  /**
  * Write data
  * \param data to be sent through the serial device
  * \throws boost::system::system_error if any error
  */
  void write(const std::vector<char>& data);

  /**
  * Write a string. Can be used to send ASCII data to the serial device.
  * To send binary data, use write()
  * \param s string to send
  * \throws boost::system::system_error if any error
  */
  void writeString(const std::string& s);

  /**
  * Read some data, blocking
  * \param data array of char to be read through the serial device
  * \param size array size
  * \return number of character actually read 0<=return<=size
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of time-out
  */
  void read(char *data, size_t size);

  /**
  * Read some data, blocking
  * \param size how much data to read
  * \return the receive buffer. It is empty if no data is available
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of time-out
  */
  std::vector<char> read(size_t size);

  /**
  * Read a string, blocking
  * Can only be used if the user is sure that the serial device will not
  * send binary data. For binary data read, use read()
  * The returned string is empty if no data has arrived
  * \param size how much data to read
  * \return a string with the received data.
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of time-out
  */
  std::string readString(size_t size);

  /**
  * Read a line, blocking
  * Can only be used if the user is sure that the serial device will not
  * send binary data. For binary data read, use read()
  * The returned string is empty if the line delimiter has not yet arrived.
  * \param delimiter line delimiter, default="\n"
  * \return a string with the received data. The delimiter is removed from
  * the string.
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of time-out
  */
  std::string readStringUntil(const std::string& delim = "\n");


private:

  /**
  * Parameters of performReadSetup.
  * Just wrapper class, no encapsulation provided
  */
  class ReadSetupParameters
  {
  public:
    ReadSetupParameters();

    explicit ReadSetupParameters(const std::string& delim);

    ReadSetupParameters(char *data, size_t size);

    //Using default copy constructor, operator=

    bool fixedSize;      /// True if need to read a fixed number of parameters
    std::string delim;   /// String end delimiter (valid if fixedSize=false)
    char *data;          /// Pointer to data array (valid if fixedSize=true)
    size_t size;         /// Array size (valid if fixedSize=true)
  };

  /*
  * This member function sets up a read operation, both reading a specified
  * number of characters and reading until a delimiter string.
  */
  void performReadSetup(const ReadSetupParameters& param);

  /*
  * Callback called either when the read time-out is expired or cancelled.
  * If called because time-out expired, sets result to resultTimeoutExpired
  */
  void timeoutExpired(const boost::system::error_code& error);

  /*
  * Callback called either if a read complete or read error occurs
  * If called because of read complete, sets result to resultSuccess
  * If called because read error, sets result to resultError
  */

  void readCompleted(const boost::system::error_code& error,
    const size_t bytesTransferred);


  boost::asio::io_context io;               ///< I/O context object
  boost::asio::serial_port port;            ///< Serial port object
  boost::asio::deadline_timer timer;        ///< Timer for time-out
  boost::posix_time::time_duration timeout; ///< Read/write time-out
  boost::asio::streambuf readData;          ///< Holds eventual read but not consumed
  enum ReadResult result;                   ///< Used by read with time-out
  size_t bytesTransferred;                  ///< Used by async read callback
  ReadSetupParameters setupParameters;      ///< Global because used in the OSX fix
};



/**
* This class contains all the options for a serial port.
*/
class SerialOptions
{
  typedef boost::posix_time::time_duration time_duration;
  typedef boost::posix_time::seconds seconds;

public:
  ///Possible flow controls.
  enum FlowControl { noflow, software, hardware };

  ///Possible parities.
  enum Parity { noparity, odd, even };

  ///Possible stop bits.
  enum StopBits { one, onepointfive, two };

  /**
  * Default constructor.
  */
  SerialOptions();

  /**
  * Constructor.
  * \param device device name (/dev/ttyS0, /dev/ttyUSB0, COM1, ...)
  * \param baudrate baudrate, like 9600, 115200 ...
  * \param timeout time-out when reading, use zero to disable
  * \param parity parity
  * \param csize character size (5,6,7 or 8)
  * \param flow flow control
  * \param stop stop bits
  *
  */
  SerialOptions(const std::string& device, unsigned int baudrate,
    time_duration timeout , Parity parity,
    unsigned char csize, FlowControl flow, StopBits stop);

  /**
  * Setter and getter for device name
  */
  void setDevice(const std::string& device);
  std::string getDevice() const;

  /**
  * Setter and getter for baudrate
  */
  void setBaudrate(unsigned int baudrate);
  unsigned int getBaudrate() const;

  /**
  * Setter and getter for time-out
  */
  void setTimeout(time_duration timeout);
  time_duration getTimeout() const;

  /**
  * Setter and getter for parity
  */
  void setParity(Parity parity);
  Parity getParity() const;

  /**
  * Setter and getter character size
  */
  void setCsize(unsigned char csize);
  unsigned char getCsize() const;

  /**
  * Setter and getter flow control
  */
  void setFlowControl(FlowControl flow);
  FlowControl getFlowControl() const;

  /**
  * Setter and getter for stop bits
  */
  void setStopBits(StopBits stop);
  StopBits getStopBits() const;

private:
  std::string device;
  unsigned int baudrate;
  time_duration timeout;
  Parity parity;
  unsigned char csize;
  FlowControl flow;
  StopBits stop;
};


class SerialDeviceImpl : private boost::noncopyable
{
public:
  /**
  * Construct a SerialDeviceImpl from a SerialOptions class
  * \param options serial port options
  */
  SerialDeviceImpl(const SerialOptions& options);

  boost::asio::io_context io; ///< I/O context object
  boost::asio::serial_port port; ///< Serial port object
  boost::asio::deadline_timer timer; ///< Timer for time-out
  boost::posix_time::time_duration timeout; ///< Read/write time-out
  enum ReadResult result;  ///< Used by read with time-out
  std::streamsize bytesTransferred; ///< Used by async read callback
  char *readBuffer; ///< Used to hold read data
  std::streamsize readBufferSize; ///< Size of read data buffer
};



/**
* \internal
* Implementation detail of a serial device.
* User code should use SerialStream
*/
class SerialDevice
{
public:
  typedef char char_type;
  typedef boost::iostreams::bidirectional_device_tag category;

  /**
  * \internal
  * Constructor.
  * \throws ios_base::failure if there are errors with the serial port.
  * \param options serial port options
  */
  explicit SerialDevice(const SerialOptions& options);

  /**
  * \internal
  * Read from serial port.
  * \throws TimeoutException on time-out, or ios_base::failure if there are
  * errors with the serial port.
  * Note: TimeoutException derives from ios_base::failure so catching that
  * allows to catch any exception.
  * Use the clear() member function to go on reading after an exception was
  * thrown.
  * \param s where to store read characters
  * \param n max number of characters to read
  * \return number of character read
  */
  std::streamsize read(char *s, std::streamsize n);

  /**
  * \internal
  * Write to serial port.
  * \throws ios_base::failure if there are errors with the serial port.
  * Use the clear() member function to go on reading after an exception was
  * thrown.
  * \param s
  * \param n
  * \return
  */
  std::streamsize write(const char *s, std::streamsize n);

private:
  /**
  * Callback called either when the read time-out is expired or cancelled.
  * If called because time-out expired, sets result to resultTimeoutExpired
  */
  void timeoutExpired(const boost::system::error_code& error);

  /**
  * Callback called either if a read complete or read error occurs
  * If called because of read complete, sets result to resultSuccess
  * If called because read error, sets result to resultError
  */
  void readCompleted(const boost::system::error_code& error, const size_t bytesTransferred);

  boost::shared_ptr<SerialDeviceImpl> pImpl; //Implementation
};

/**
* SerialStream, an iostream-compatible serial port class.
* Note: due to a limitation about error reporting with boost::iostreams,
* this class *always* throws exceptions on error (time-out, failure, etc..)
* so after creating an instance of this class you should always enable
* exceptions with the exceptions() member function:
* \code SerialStream serial; serial.exceptions(ios::failbit | ios::badbit);
* \endcode
* If you don't, functions like getline() will swallow the exceptions, while
* operator >> will throw, leading to inconsistent behaviour.
*/
typedef boost::iostreams::stream<SerialDevice> SerialStream;


class AsyncSerialImpl;

/**
* Asynchronous serial class.
* Intended to be a base class.
*/
class AsyncSerial : private boost::noncopyable
{
public:
  AsyncSerial();

  /**
  * Constructor. Creates and opens a serial device.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8 bit
  * \param opt_flow serial flow control, default none
  * \param opt_stop serial stop bits, default 1
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  AsyncSerial(const std::string& devname, unsigned int baud_rate,
    boost::asio::serial_port_base::parity opt_parity =
    boost::asio::serial_port_base::parity(
    boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size opt_csize =
    boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control opt_flow =
    boost::asio::serial_port_base::flow_control(
    boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits opt_stop =
    boost::asio::serial_port_base::stop_bits(
    boost::asio::serial_port_base::stop_bits::one));

  /**
  * Opens a serial device.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8 bit
  * \param opt_flow serial flow control, default none
  * \param opt_stop serial stop bits, default 1
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  void open(const std::string& devname, unsigned int baud_rate,
    boost::asio::serial_port_base::parity opt_parity =
    boost::asio::serial_port_base::parity(
    boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size opt_csize =
    boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control opt_flow =
    boost::asio::serial_port_base::flow_control(
    boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits opt_stop =
    boost::asio::serial_port_base::stop_bits(
    boost::asio::serial_port_base::stop_bits::one));

  /**
  * \return true if serial device is open
  */
  bool isOpen() const;

  /**
  * \return true if error were found
  */
  bool errorStatus() const;

  /**
  * Close the serial device
  * \throws boost::system::system_error if any error
  */
  void close();

  /**
  * Write data asynchronously. Returns immediately.
  * \param data array of char to be sent through the serial device
  * \param size array size
  */
  void write(const char *data, size_t size);

  /**
  * Write data asynchronously. Returns immediately.
  * \param data to be sent through the serial device
  */
  void write(const std::vector<char>& data);

  /**
  * Write a string asynchronously. Returns immediately.
  * Can be used to send ASCII data to the serial device.
  * To send binary data, use write()
  * \param s string to send
  */
  void writeString(const std::string& s);

  virtual ~AsyncSerial() = 0;

  /**
  * Read buffer maximum size
  */
  static const int readBufferSize = 512;
private:

  /**
  * Callback called to start an asynchronous read operation.
  * This callback is called by the io_context in the spawned thread.
  */
  void doRead();

  /**
  * Callback called at the end of the asynchronous operation.
  * This callback is called by the io_context in the spawned thread.
  */
  void readEnd(const boost::system::error_code& error,
    size_t bytes_transferred);

  /**
  * Callback called to start an asynchronous write operation.
  * If it is already in progress, does nothing.
  * This callback is called by the io_context in the spawned thread.
  */
  void doWrite();

  /**
  * Callback called at the end of an asynchronous write operation,
  * if there is more data to write, restarts a new write operation.
  * This callback is called by the io_context in the spawned thread.
  */
  void writeEnd(const boost::system::error_code& error);

  /**
  * Callback to close serial port
  */
  void doClose();

  boost::shared_ptr<AsyncSerialImpl> pimpl;

protected:

  /**
  * To allow derived classes to report errors
  * \param e error status
  */
  void setErrorStatus(bool e);

  /**
  * To allow derived classes to set a read callback
  */
  void setReadCallback(const
    boost::function<void(const char*, size_t)>& callback);

  /**
  * To unregister the read callback in the derived class destructor so it
  * does not get called after the derived class destructor but before the
  * base class destructor
  */
  void clearReadCallback();

};

/**
* Asynchronous serial class with read callback. User code can write data
* from one thread, and read data will be reported through a callback called
* from a separate thread.
*/
class CallbackAsyncSerial : public AsyncSerial
{
public:
  CallbackAsyncSerial();

  /**
  * Opens a serial device.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8 bit
  * \param opt_flow serial flow control, default none
  * \param opt_stop serial stop bits, default 1
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  CallbackAsyncSerial(const std::string& devname, unsigned int baud_rate,
    boost::asio::serial_port_base::parity opt_parity =
    boost::asio::serial_port_base::parity(
    boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size opt_csize =
    boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control opt_flow =
    boost::asio::serial_port_base::flow_control(
    boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits opt_stop =
    boost::asio::serial_port_base::stop_bits(
    boost::asio::serial_port_base::stop_bits::one));

  /**
  * Set the read callback, the callback will be called from a thread
  * owned by the CallbackAsyncSerial class when data arrives from the
  * serial port.
  * \param callback the receive callback
  */
  void setCallback(const
    boost::function<void(const char*, size_t)>& callback);

  /**
  * Removes the callback. Any data received after this function call will
  * be lost.
  */
  void clearCallback();

  virtual ~CallbackAsyncSerial();
};



class BufferedAsyncSerial : public AsyncSerial
{
public:
  BufferedAsyncSerial();

  /**
  * Opens a serial device.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8 bit
  * \param opt_flow serial flow control, default none
  * \param opt_stop serial stop bits, default 1
  * \throws boost::system::system_error if cannot open the
  * serial device
  */
  BufferedAsyncSerial(const std::string& devname, unsigned int baud_rate,
    boost::asio::serial_port_base::parity opt_parity =
    boost::asio::serial_port_base::parity(
    boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size opt_csize =
    boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control opt_flow =
    boost::asio::serial_port_base::flow_control(
    boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits opt_stop =
    boost::asio::serial_port_base::stop_bits(
    boost::asio::serial_port_base::stop_bits::one));

  /**
  * Read some data asynchronously. Returns immediately.
  * \param data array of char to be read through the serial device
  * \param size array size
  * \return number of character actually read 0<=return<=size
  */
  size_t read(char *data, size_t size);

  /**
  * Read all available data asynchronously. Returns immediately.
  * \return the receive buffer. It is empty if no data is available
  */
  std::vector<char> read();

  /**
  * Read a string asynchronously. Returns immediately.
  * Can only be used if the user is sure that the serial device will not
  * send binary data. For binary data read, use read()
  * The returned string is empty if no data has arrived
  * \return a string with the received data.
  */
  std::string readString();

  /**
  * Read a line asynchronously. Returns immediately.
  * Can only be used if the user is sure that the serial device will not
  * send binary data. For binary data read, use read()
  * The returned string is empty if the line delimiter has not yet arrived.
  * \param delimiter line delimiter, default='\n'
  * \return a string with the received data. The delimiter is removed from
  * the string.
  */
  std::string readStringUntil(const std::string delim = "\n");

  virtual ~BufferedAsyncSerial();

private:

  /**
  * Read callback, stores data in the buffer
  */
  void readCallback(const char *data, size_t len);

  /**
  * Finds a sub-string in a vector of char. Used to look for the delimiter.
  * \param v vector where to find the string
  * \param s string to find
  * \return the beginning of the place in the vector where the first
  * occurrence of the string is, or v.end() if the string was not found
  */
  static std::vector<char>::iterator findStringInVector(std::vector<char>& v,
    const std::string& s);

  std::vector<char> readQueue;
  boost::mutex readQueueMutex;
};





