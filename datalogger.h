// Copyright 2014 Stefano Sinigardi and others
// for any question, please mail stefano.sinigardi@gmail.com

/************************************************************************
* This program is free software: you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/






#define USE_SERIAL_PORT
//#define USE_BINARY_FILE

#define DIMENSIONE_MAX 1000
#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif

#define MAJOR_VERSION 0
#define MINOR_VERSION 6

//#define USE_HOST_MEMORY
#define DEBUG
#define NDATA 17



#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iomanip>
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>



#if defined(USE_HOST_MEMORY)
#ifdef _WIN32
#define BOOST_NO_RVALUE_REFERENCES
#include <boost/interprocess/windows_shared_memory.hpp>
#else
#include <boost/interprocess/shared_memory_object.hpp>
#endif
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;


void remove_host_memory(const char* name)
{
#if !defined(_WIN32)
  bool non_esiste = shared_memory_object::remove(name);
  if (!non_esiste) std::cout << " ho rimosso " << name << "  \n";
  else            std::cout << " non_esiste " << name << "  \n";
#endif
}



void* allocate_host_memory(const char* name, size_t bytes)
{
  remove_host_memory(name);

#ifdef _WIN32
  windows_shared_memory* shm = new windows_shared_memory(create_only, name, read_write, bytes);
#else
  shared_memory_object* shm = new shared_memory_object(create_only, name, read_write);
  shm->truncate(bytes);
#endif

  mapped_region* region = new mapped_region(*shm, read_write);//, 0, bytes-1);
  return static_cast<void*>(region->get_address());
}


void* get_host_allocated_memory(const char* name){

#ifdef _WIN32
  windows_shared_memory shm(open_only, name, read_write);
#else
  shared_memory_object shm(open_only, name, read_write);
#endif

  mapped_region* region = new mapped_region(shm, read_write); //or read_only
  return static_cast<void*>(region->get_address());
}
#endif


struct Data{
  double d[NDATA];
  //int i[NDATA];
};


void swap_endian_s(short* in_s, int n)
{
  int i;
  union { short smio; char arr[2]; }x;
  char buff;

  for (i = 0; i < n; i++)
  {
    x.smio = in_s[i];
    buff = x.arr[0];
    x.arr[0] = x.arr[1];
    x.arr[1] = buff;
    in_s[i] = x.smio;
  }
}


void swap_endian_i(int* in_i, int n)
{
  int i;
  union { int imio; float fmio; char arr[4]; }x;
  char buff;
  for (i = 0; i < n; i++)
  {
    x.imio = in_i[i];
    buff = x.arr[0];
    x.arr[0] = x.arr[3];
    x.arr[3] = buff;
    buff = x.arr[1];
    x.arr[1] = x.arr[2];
    x.arr[2] = buff;
    in_i[i] = x.imio;
  }
}


void swap_endian_f(float* in_f, int n)
{
  int i;
  union { int imio; float fmio; char arr[4]; }x;
  char buff;
  for (i = 0; i < n; i++)
  {
    x.fmio = in_f[i];
    buff = x.arr[0];
    x.arr[0] = x.arr[3];
    x.arr[3] = buff;
    buff = x.arr[1];
    x.arr[1] = x.arr[2];
    x.arr[2] = buff;
    in_f[i] = x.fmio;
  }
}

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
  SimpleSerial(std::string port, unsigned int baud_rate) : io(), serial(io, port)
  {
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
  }

  /**
  * Write a string to the serial device.
  * \param s string to write
  * \throws boost::system::system_error on failure
  */
  void writeString(std::string s)
  {
    boost::asio::write(serial, boost::asio::buffer(s.c_str(), s.size()));
  }

  /**
  * Blocks until a line is received from the serial device.
  * Eventual '\n' or '\r\n' characters at the end of the string are removed.
  * \return a string containing the received line
  * \throws boost::system::system_error on failure
  */
  std::string readLine()
  {
    //Reading data char by char, code is optimized for simplicity, not speed
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

private:
  boost::asio::io_service io;
  boost::asio::serial_port serial;
};


class timeout_exception : public std::runtime_error
{
public:
  timeout_exception(const std::string& arg) : runtime_error(arg) {}
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
    ReadSetupParameters() : fixedSize(false), delim(""), data(0), size(0) {}

    explicit ReadSetupParameters(const std::string& delim) :
      fixedSize(false), delim(delim), data(0), size(0) { }

    ReadSetupParameters(char *data, size_t size) : fixedSize(true),
      delim(""), data(data), size(size) { }

    //Using default copy constructor, operator=

    bool fixedSize; ///< True if need to read a fixed number of parameters
    std::string delim; ///< String end delimiter (valid if fixedSize=false)
    char *data; ///< Pointer to data array (valid if fixedSize=true)
    size_t size; ///< Array size (valid if fixedSize=true)
  };

  /**
  * This member function sets up a read operation, both reading a specified
  * number of characters and reading until a delimiter string.
  */
  void performReadSetup(const ReadSetupParameters& param);

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
  void readCompleted(const boost::system::error_code& error,
    const size_t bytesTransferred);

  /**
  * Possible outcome of a read. Set by callbacks, read from main code
  */
  enum ReadResult
  {
    resultInProgress,
    resultSuccess,
    resultError,
    resultTimeoutExpired
  };

  boost::asio::io_service io; ///< Io service object
  boost::asio::serial_port port; ///< Serial port object
  boost::asio::deadline_timer timer; ///< Timer for timeout
  boost::posix_time::time_duration timeout; ///< Read/write timeout
  boost::asio::streambuf readData; ///< Holds eventual read but not consumed
  enum ReadResult result;  ///< Used by read with timeout
  size_t bytesTransferred; ///< Used by async read callback
  ReadSetupParameters setupParameters; ///< Global because used in the OSX fix
};


