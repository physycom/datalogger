// Copyright 2014 Stefano Sinigardi
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


#include "infomobility.h"

//using namespace boost;

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
    case resultTimeoutExpired:
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
    case resultTimeoutExpired:
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
  if (!error && result == resultInProgress) result = resultTimeoutExpired;
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


//class GPSData
//{
//private:
//  unsigned short year;
//  unsigned char month, day, hour, min, sec, fixType;
//  long nano, lat, lon, gSpeed;
//public:
//  GPSData();
//};



class ACCData
{
private:
  short AccX, AccY, AccZ;
public:
  void setAccX(short accx);
  void setAccY(short accy);
  void setAccZ(short accz);
  short getAccX();
  short getAccY();
  short getAccZ();
};



void ACCData::setAccX(short accx)
{
  AccX = accx;
}



void ACCData::setAccY(short accy)
{
  AccY = accy;
}



void ACCData::setAccZ(short accz)
{
  AccZ = accz;
}



short ACCData::getAccX()
{
  return AccX;
}



short ACCData::getAccY()
{
  return AccY;
}



short ACCData::getAccZ()
{
  return AccZ;
}



class InfomobilityData
{
private:
  char h1, h2, hclass, hid;
  short hlength;
  char f1, f2;
  char * payload;
  bool isGPS; //depends on hlength, initialized on loadheader(): if 92 then true, if 14 then false
  ACCData accdata;
  //GPSData gpsdata;
public:
  void saveheader(std::ofstream& outputfile);
  void printheader();
  void loadheader(std::ifstream& inputfile);
  void loadheaderS(TimeoutSerial& serial);
  void savefooter(std::ofstream& outputfile);
  void printfooter();
  void loadfooter(std::ifstream& inputfile);
  void loadfooterS(TimeoutSerial& serial);
  int getPayloadSize();
  bool isGPSData();
  void allocatePayload(int sizePayload);
  void deAllocatePayload();
  void readPayload(std::ifstream& inputfile);
  void readPayloadS(TimeoutSerial& serial);
  bool checkfooter();
  void recordGPSDataFromPayload();
  void recordACCDataFromPayload();
  void printGPSData();
  void printACCData();
};



void InfomobilityData::saveheader(std::ofstream& outputfile)
{
  outputfile.write(&h1, sizeof(h1));
  outputfile.write(&h2, sizeof(h2));
  outputfile.write(&hclass, sizeof(hclass));
  outputfile.write(&hid, sizeof(hid));
  outputfile.write((char*)&hlength, sizeof(hlength));
}



void InfomobilityData::printheader()
{
  std::cout << "h1=" << h1 << "; h2=" << h2 << "hclass=" << hclass << "; hid=" << hid << std::endl;
  std::cout << "Payload length: " << hlength << std::endl;
}



void InfomobilityData::loadheader(std::ifstream& inputfile)
{
  h1 = 0, h2 = 0;
  while (h1 != 0xB5 || h2 != 0x62)
  {
    inputfile.read(&h1, sizeof(h1));
    inputfile.read(&h2, sizeof(h2));
  }
  inputfile.read(&hclass, sizeof(hclass));
  inputfile.read(&hid, sizeof(hid));
  inputfile.read((char*)&hlength, sizeof(hlength));
  switch (hlength)
  {
  case 14:
    isGPS = false;
    break;
  case 92:
    isGPS = true;
    break;
  default:
    std::cout << "Error: packet size not recognized!" << std::endl;
    break;
  }
}



void InfomobilityData::loadheaderS(TimeoutSerial& serial)
{
  h1 = 0, h2 = 0;
  while (h1 != 0xB5 || h2 != 0x62)
  {
    serial.read(&h1, sizeof(h1));
    serial.read(&h2, sizeof(h2));
  }
  serial.read(&hclass, sizeof(hclass));
  serial.read(&hid, sizeof(hid));
  serial.read((char*)&hlength, sizeof(hlength));
  switch (hlength)
  {
  case 14:
    isGPS = false;
    break;
  case 92:
    isGPS = true;
    break;
  default:
    std::cout << "Error: packet size not recognized!" << std::endl;
    break;
  }
}



int InfomobilityData::getPayloadSize()
{
  return (int)hlength;
}



void InfomobilityData::savefooter(std::ofstream& outputfile)
{
  outputfile.write(&f1, sizeof(f1));
  outputfile.write(&f2, sizeof(f2));
}



void InfomobilityData::printfooter()
{
  std::cout << "f1=" << f1 << "; f2=" << f2 << std::endl;
}



void InfomobilityData::loadfooter(std::ifstream& inputfile)
{
  inputfile.read(&f1, sizeof(f1));
  inputfile.read(&f2, sizeof(f2));
}



void InfomobilityData::loadfooterS(TimeoutSerial& serial)
{
  serial.read(&f1, sizeof(f1));
  serial.read(&f2, sizeof(f2));
}



void InfomobilityData::allocatePayload(int sizePayload)
{
  payload = new char[sizePayload];
}



void InfomobilityData::deAllocatePayload()
{
  delete[] payload;
}



void InfomobilityData::readPayload(std::ifstream& inputfile)
{
  inputfile.read(payload, sizeof(payload));
}



void InfomobilityData::readPayloadS(TimeoutSerial& serial)
{
  serial.read(payload, sizeof(payload));
}



bool InfomobilityData::checkfooter()
{
  char ck_a = 0, ck_b = 0;
  ck_a = ck_a + hclass;
  ck_b = ck_b + ck_a;
  ck_a = ck_a + hid;
  ck_b = ck_b + ck_a;
  ck_a = ck_a + hlength;
  ck_b = ck_b + ck_a;
  for (int i = 0; i < sizeof(payload); i++)
  {
    ck_a = ck_a + payload[i];
    ck_b = ck_b + ck_a;
  }

  return ((ck_a == f1) && (ck_b == f2));
}



bool InfomobilityData::isGPSData()
{
  return isGPS;
}



void InfomobilityData::recordACCDataFromPayload()
{
  accdata.setAccX(*(short*)payload);
  accdata.setAccY(*(short*)(payload + sizeof(short)));
  accdata.setAccZ(*(short*)(payload + 2 * sizeof(short)));
}



void InfomobilityData::recordGPSDataFromPayload()
{
  return; // will store here proper gpsdata object using payload
}



void InfomobilityData::printACCData()
{
  std::cout << "AccX=" << accdata.getAccX() << ", AccY=" << accdata.getAccY() << ", AccZ=" << accdata.getAccZ() << std::endl;
}



void InfomobilityData::printGPSData()
{
  return;
}



int main(int narg, char ** args)
{
  InfomobilityData* dati;
  dati = new InfomobilityData[DIMENSIONE_MAX];
  int i = 0;

#if defined(USE_BINARY_FILE)
  // change next two lines with another input source when needed
  std::ifstream inputfile;
  inputfile.open(args[1], std::ios::binary | std::ios::in);

  while (1)
  {
    if (inputfile.eof()) break;
    if (i >= DIMENSIONE_MAX) break;
    dati[i].loadheader(inputfile);
    dati[i].allocatePayload(dati[i].getPayloadSize());
    dati[i].readPayload(inputfile);
    dati[i].loadfooter(inputfile);
    dati[i].checkfooter();
    //dati[i].printheader();
    //dati[i].printfooter();
    //dati[i].deAllocatePayload();
    if (dati[i].isGPSData())
    {
      dati[i].recordGPSDataFromPayload();
      dati[i].printGPSData();
    }
    else
    {
      dati[i].recordACCDataFromPayload();
      dati[i].printACCData();
    }

    i++;
  }
#elif defined(USE_SERIAL_PORT)
  TimeoutSerial serial("COM26", 115200);
  serial.setTimeout(boost::posix_time::seconds(0));

  std::ifstream inputfile;
  inputfile.open(args[1], std::ios::binary | std::ios::in);

  try
  {
    dati[i].loadheaderS(serial);
    dati[i].allocatePayload(dati[i].getPayloadSize());
    dati[i].readPayloadS(serial);
    dati[i].loadfooterS(serial);
    dati[i].checkfooter();
    //dati[i].printheader();
    //dati[i].printfooter();
    //dati[i].deAllocatePayload();
    if (dati[i].isGPSData())
    {
      dati[i].recordGPSDataFromPayload();
      dati[i].printGPSData();
    }
    else
    {
      dati[i].recordACCDataFromPayload();
      dati[i].printACCData();
    }

    i++;
  }
  catch (boost::system::system_error& e)
  {
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }
#else
  std::cout << "No valid method" << std::endl;
#endif

  return 0;

}


