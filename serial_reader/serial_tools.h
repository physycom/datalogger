/*
* Authors: Federico Terraneo, Stefano Sinigardi, Alessandro Fabbri
* Distributed under the Boost Software License, Version 1.0.
*/



#pragma once

#include "datalogger.h"

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
  boost::asio::io_service io;
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
* Serial port class, with timeout on read operations.
*/
class TimeoutSerial : private boost::noncopyable
{
public:

  /**
  * Opens a serial device. By default timeout is disabled.
  * \param devname serial device name, example "/dev/ttyS0" or "COM1"
  * \param baud_rate serial baud rate
  * \param opt_parity serial parity, default none
  * \param opt_csize serial character size, default 8bit
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
  * \param opt_csize serial character size, default 8bit
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
  * Set the timeout on read/write operations.
  * To disable the timeout, call setTimeout(boost::posix_time::seconds(0));
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
  * \return numbr of character actually read 0<=return<=size
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of timeout
  */
  void read(char *data, size_t size);

  /**
  * Read some data, blocking
  * \param size how much data to read
  * \return the receive buffer. It iempty if no data is available
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of timeout
  */
  std::vector<char> read(size_t size);

  /**
  * Read a string, blocking
  * Can only be used if the user is sure that the serial device will not
  * send binary data. For binary data read, use read()
  * The returned string is empty if no data has arrived
  * \param size hw much data to read
  * \return a string with the received data.
  * \throws boost::system::system_error if any error
  * \throws timeout_exception in case of timeout
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
  * \throws timeout_exception in case of timeout
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
  * Callack called either when the read timeout is expired or canceled.
  * If called because timeout expired, sets result to resultTimeoutExpired
  */
  void timeoutExpired(const boost::system::error_code& error);

  /*
  * Callback called either if a read complete or read error occurs
  * If called because of read complete, sets result to resultSuccess
  * If called because read error, sets result to resultError
  */

  void readCompleted(const boost::system::error_code& error,
    const size_t bytesTransferred);


  boost::asio::io_service io;               ///< Io service object
  boost::asio::serial_port port;            ///< Serial port object
  boost::asio::deadline_timer timer;        ///< Timer for timeout
  boost::posix_time::time_duration timeout; ///< Read/write timeout
  boost::asio::streambuf readData;          ///< Holds eventual read but not consumed
  enum ReadResult result;                   ///< Used by read with timeout
  size_t bytesTransferred;                  ///< Used by async read callback
  ReadSetupParameters setupParameters;      ///< Global because used in the OSX fix
};


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
  * \param timeout timeout when reading, use zero to disable
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
  * Setter and getter for timeout
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

  boost::asio::io_service io; ///< Io service object
  boost::asio::serial_port port; ///< Serial port object
  boost::asio::deadline_timer timer; ///< Timer for timeout
  boost::posix_time::time_duration timeout; ///< Read/write timeout
  enum ReadResult result;  ///< Used by read with timeout
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
  * \throws TimeoutException on timeout, or ios_base::failure if there are
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
  * Callack called either when the read timeout is expired or canceled.
  * If called because timeout expired, sets result to resultTimeoutExpired
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
* this class *always* throws exceptions on error (timeout, failure, etc..)
* so after creating an instance of this class you should alway enable
* exceptions with the exceptions() member function:
* \code SerialStream serial; serial.exceptions(ios::failbit | ios::badbit);
* \endcode
* If you don't, functions like getline() will swallow the exceptions, while
* operator >> will throw, leading to unconsistent behaviour.
*/
typedef boost::iostreams::stream<SerialDevice> SerialStream;


