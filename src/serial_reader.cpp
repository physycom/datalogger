// Copyright 2014, 2015 Stefano Sinigardi, Alessandro Fabbri
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



#include "datalogger.h"
#include "serial_tools.h"
#include "swap_tools.hpp"
#include "data_tools.hpp"



int main(int argc, char ** argv)
{
  size_t systeminfo = 0;
  std::cout << "Datalogger v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
  std::cout << "Usage: " << argv[0] << " -p [serial_port] -b [baudrate] -t [box_type] -h (shows help and quit)" << std::endl;
  std::cout << "\t- [serial_port] serial port name (COMx on WIN, /dev/ttyUSBx on UNIX)" << std::endl;
  std::cout << "\t- [baudrate] " << std::endl;
  std::cout << "\t- [box_type] " << std::endl;
  std::cout << "new: general fixes and improvements\n" << std::endl;

  std::string serial_port = "";
  int baudrate = -1;
  bool serial_port_found = false;
  bool baudrate_found = false;

  if (argc > 1) { /* Parse arguments, if there are arguments supplied */
    for (int i = 1; i < argc; i++) {
      if ((argv[i][0] == '-') || (argv[i][0] == '/')) {       // switches or options...
        switch (tolower(argv[i][1])) {
        case 'p':
          serial_port = argv[++i];
          break;
        case 'b':
          baudrate = atoi(argv[++i]);
          break;
        case 't':
          systeminfo = atoi(argv[++i]);
          break;
        case 'h':
          exit(777);
        default:    // no match...
          std::cout << argv[i] << " not recognized" << std::endl;
          break;
        }
      }
      else {
        std::cout << argv[i] << " not recognized" << std::endl;
        break;
      }
    }
  }
  else { std::cout << "Using default parameters" << std::endl; }

  std::vector<std::string> box_types({ "Infomobility", "MagnetiMarelli", "Texa", "ViaSat", "MetaSystem", "UBX", "Octo", "NMEA", "MagnetiMarelli_v2", "MetaSystem_v2" });

  while (systeminfo < 1 || systeminfo > box_types.size()) {
    std::cout << "Which kind of system is attached? Answer with the number" << std::endl;
    for (size_t i = 0; i < box_types.size(); i++) std::cout << i + 1 << ". " << box_types[i] << std::endl;
    std::cin >> systeminfo;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }


  std::vector<int> baudrates({ 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200, 230400, 460800, 921600 });
  for (auto br : baudrates) {
    baudrate_found = (br == baudrate);
    if (baudrate_found) break;
  }
  while (!baudrate_found) {
    std::cout << "Baud rate: " << std::endl;
    std::cin >> baudrate;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    for (auto br : baudrates)
    {
      baudrate_found = (br == baudrate);
      if (baudrate_found) break;
    }
  }


  std::vector<std::string> serial_port_names;
  for (int i = 0; i < 256; i++) {
#ifdef _WIN32
    std::string serial_port_name = std::string("COM") + std::to_string(i);
#else
    std::string serial_port_name = std::string("/dev/ttyUSB") + std::to_string(i);
#endif
    serial_port_names.push_back(serial_port_name);
  }
  for (auto port : serial_port_names)
  {
    serial_port_found = (port == serial_port);
    if (serial_port_found) break;
  }
  while (!serial_port_found) {
    std::cout << "Serial port name: " << std::endl;
    std::cin >> serial_port;
    for (auto port : serial_port_names)
    {
      serial_port_found = (port == serial_port);
      if (serial_port_found) break;
    }
  }

  std::cout << "Connecting to box TYPE " << box_types[systeminfo - 1] << " on PORT " << serial_port << " with BAUDRATE " << baudrate << std::endl;

  NavData navdata;
  int indiceData = 0;
  size_t counter = 0;
  bool exit = false;
  std::ofstream logfile;
  time_t tnow;


  SerialOptions portacom;
  portacom.setDevice(serial_port);
  portacom.setBaudrate(baudrate);
  portacom.setTimeout(boost::posix_time::seconds(SERIAL_PORT_TIMEOUT_SECONDS));
  portacom.setFlowControl(SerialOptions::noflow);
  portacom.setParity(SerialOptions::noparity);
  portacom.setCsize(8);
  portacom.setStopBits(SerialOptions::one);

#ifndef WRITE_ON_STDOUT
  logfile.open(box_types[systeminfo - 1] + ".log", std::ofstream::out);
  /*
  to redirect automatically stdout to logfile the following line is enough. In "coutbuf" there will be saved the cout original buffer in case
  it is required to go back to stdout at a certain point during the program execution. Maybe this trick can be useful to reduce output functions
  */
  //auto coutbuf = std::cout.rdbuf(logfile.rdbuf());
#endif

#if defined (USE_HOST_MEMORY)
  Data *data;
  data = (Data*)get_host_allocated_memory(box_types[systeminfo - 1].c_str());
#endif

  InfomobilityData idato;
  MetasystemData mdato;
  GPSData gdato;
  OctoData odato;
  std::vector<boost::regex> patterns;
  //std::vector<std::string> pattern_names({ "GSV", "GLL", "RMC", "VTG", "GGA", "GSA" });
  std::vector<std::string> pattern_names({ "RMC" });
  bool extracted_data;

  switch (systeminfo) {
  case 1: //Infomobility
  {
    TimeoutSerial serial(portacom.getDevice(), portacom.getBaudrate());
    serial.setTimeout(boost::posix_time::seconds(0));

    try {

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        idato.loadheaderS(serial);
        idato.allocatePayload(idato.getPayloadSize());
        idato.readPayloadS(serial);
        idato.loadfooterS(serial);
        idato.checkfooter();
        //idato.printheader();
        //idato.printfooter();
        idato.saveheader(logfile);
        idato.savefooter(logfile);
        //idato.deAllocatePayload();
        if (idato.isGPSData())
        {
          idato.recordGPSDataFromPayload();
#ifdef WRITE_ON_STDOUT
          idato.printGPSData();
#else 
          idato.saveGPSData(logfile);
#endif
        }
        else
        {
          idato.recordACCDataFromPayload();
#ifdef WRITE_ON_STDOUT
          idato.printACCData();
#else
          idato.saveACCData(logfile);
#endif
        }

#ifdef ENABLE_SLEEP
        boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
      }
    }

    catch (boost::system::system_error& e) {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }
  case 2: //MagnetiMarelli
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {
      std::string sst;
      std::vector<std::string> strs;

      while (exit == false) {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        try {
          std::getline(sserial, sst);
        }
        catch (TimeoutException&) {
          sserial.clear(); //Don't forget to clear error flags after a timeout
          std::cerr << "Timeout occurred" << std::endl;
        }

        tnow = time(NULL);

        if (sst[0] == '{') {
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("{}; "));
          navdata.setTime(tnow);
          navdata.setAcc_s(&strs[0]);
#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif
        }
        else {
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("; "));
          double gyr_data[3];
          for (size_t i = 0; i < strs.size(); i++) gyr_data[i] = atof(strs[i].c_str()) / 256.;
          navdata.setTime(tnow);
          navdata.setGyr(gyr_data);
#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif
        }
        break;

#if defined (USE_HOST_MEMORY)
        if (navdata.getAcc_s()[2].size()) {
          data[indiceData].d[0] = (double)counter++;
          data[indiceData].set(navdata.getInertial());
          indiceData = (indiceData + 1) % DIMENSIONE_MAX;
        }
#endif

#ifdef ENABLE_SLEEP
        boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  case 3: // Texa
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {
      std::string sst;
      std::vector<std::string> strs;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        try {
          std::getline(sserial, sst);
        }
        catch (TimeoutException&) {
          sserial.clear(); //Don't forget to clear error flags after a timeout
          std::cerr << "Timeout occurred" << std::endl;
        }

        tnow = time(NULL);
        boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(";"));
        if (strs.size() == 21 && strs[0] != "Index") {
          navdata.setInertial_s(&strs[strs.size() - 9]);
          navdata.setTime(tnow);
        }

#ifdef WRITE_ON_STDOUT
        std::cout << navdata.to_string() << std::endl;
#else
        logfile << navdata.to_string() << std::endl;
#endif

#if defined (USE_HOST_MEMORY)
        if (navdata.getAcc_s()[2].size()) {
          data[indiceData].d[0] = (double)counter++;
          data[indiceData].set(navdata.getInertial());
          indiceData = (indiceData + 1) % DIMENSIONE_MAX;
        }
#endif

#ifdef ENABLE_SLEEP
        boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }
  case 4: // ViaSat
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {
      std::string sst;
      std::vector<std::string> strs;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        try {
          std::getline(sserial, sst);
        }
        catch (TimeoutException&) {
          sserial.clear(); //Don't forget to clear error flags after a timeout
          std::cerr << "Timeout occurred" << std::endl;
        }

        tnow = time(NULL);

        if (sst[1] == 'G')
        {
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("{}; "));

          navdata.setGyr_s(&strs[0]);
          navdata.setTime(tnow);

#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif
        }
        else
        {
          sst.insert(8, " ", 1);
          sst.insert(15, " ", 1);
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("$A* "));

          double data_temp[3];
          for (size_t i = 0; i < 3; i++) data_temp[i] = atof(strs[i].c_str()) / 1e3;
          navdata.setAcc(data_temp);
          navdata.setTime(tnow);

#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif
        }

#if defined (USE_HOST_MEMORY)
        if (navdata.getAcc_s()[2].size()) {
          data[indiceData].d[0] = (double)counter++;
          data[indiceData].set(navdata.getInertial());
          indiceData = (indiceData + 1) % DIMENSIONE_MAX;
        }
#endif

#ifdef ENABLE_SLEEP
        boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  case 5: // MetaSystem
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {

      while (exit == false)
      {
        std::string sst;
        std::vector<std::string> strs;
        raw raw_data[3];
        std::vector<float> acc(3);

#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        try {
          std::getline(sserial, sst, mdato.getAlignChar());
        }
        catch (TimeoutException&) {
          sserial.clear(); //Don't forget to clear error flags after a timeout
          std::cerr << "Timeout occurred" << std::endl;
        }

        tnow = time(NULL);


        if (sst.size() == 6) {
          raw_data[0].value_ch[0] = sst[0];
          raw_data[0].value_ch[1] = sst[1];
          raw_data[1].value_ch[0] = sst[2];
          raw_data[1].value_ch[1] = sst[3];
          raw_data[2].value_ch[0] = sst[4];
          raw_data[2].value_ch[1] = sst[5];

          raw_data[0].value_sh = (raw_data[0].value_ush << 1);
          //if (raw_data[0].value_ch[1] & 0x4000) raw_data[0].value_ch[1] |= 0x8000; // PaoloPariani mod, not working
          raw_data[1].value_sh = (raw_data[1].value_ush << 1);
          raw_data[2].value_sh = (raw_data[2].value_ush << 1);

          for (size_t i = 0; i < acc.size(); i++) acc[i] = ((float)raw_data[i].value_sh) / 1e3f;

          navdata.setAcc(&acc[0]);
          navdata.setTime(tnow);

#ifdef WRITE_ON_STDOUT

          std::cout << navdata.to_string() << std::endl;
#else

          logfile << navdata.to_string() << std::endl;
#endif

#if defined (USE_HOST_MEMORY)

          if (navdata.getAcc_s()[2].size()) {
            data[indiceData].d[0] = (double)counter++;
            data[indiceData].set(navdata.getInertial());
            indiceData = (indiceData + 1) % DIMENSIONE_MAX;
          }
#endif
        }

#ifdef ENABLE_SLEEP
        boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
      }
    }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  case 6: // UBX
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        gdato.readDataStr(sserial);
        navdata.setTime(gdato.gps_time, gdato.nano);

#ifdef WRITE_ON_STDOUT
        std::cout << navdata.to_string() << std::endl;
#else
        logfile << navdata.to_string() << std::endl;
#endif

      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  case 7: // Octo
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }
        extracted_data = false;
        odato.readDataStr(sserial, extracted_data);

        if (extracted_data) {
          tnow = time(NULL);

          double data_temp[3];
          switch (odato.type) {
          case '1':
            for (size_t i = 0; i < 3; i++) data_temp[i] = odato.acc_data[i] / 1e3;
            navdata.setAcc(data_temp);
            navdata.setTime(tnow);
            break;
          case '2':
            for (size_t i = 0; i < 3; i++) data_temp[i] = odato.gyr_data[i] / 1e3;
            navdata.setGyr(data_temp);
            navdata.setTime(tnow);
            break;
          case '3':
            // TODO
          default:
            break;
          }

#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif


#if defined (USE_HOST_MEMORY)
          if (navdata.getAcc_s()[2].size()) {
            data[indiceData].d[0] = (double)counter++;
            data[indiceData].set(navdata.getInertial());
            indiceData = (indiceData + 1) % DIMENSIONE_MAX;
          }
#endif

          odato.data_v.clear();
#ifdef ENABLE_SLEEP
          boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
        }
      }
    }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  case 8: // NMEA
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    for (auto i : pattern_names) patterns.push_back(boost::regex(i));

    try {
      std::string sst;
      std::vector<std::string> strs;
      int nterm = 0;
      int indiceData = 0;
      bool exit = false;
      bool found = false;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        try {
          std::getline(sserial, sst);
        }
        catch (TimeoutException&) {
          sserial.clear(); //Don't forget to clear error flags after a timeout
          std::cerr << "Timeout occurred" << std::endl;
        }

        tnow = time(NULL);

        boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(","));
        found = false;
        for (auto i : patterns) if (boost::regex_search(strs[0], i)) found = true;

        // TODO: save NMEA data into navdata

#ifdef WRITE_ON_STDOUT
        std::cout << navdata.to_string() << std::endl;
#else
        logfile << navdata.to_string() << std::endl;
#endif
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  case 9: // MagnetiMarelli_v2 //Octo-clone
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    try {

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }
        extracted_data = false;
        odato.readDataStr(sserial, extracted_data);

        if (extracted_data) {
          tnow = time(NULL);

          double data_temp[3];
          switch (odato.type) {
          case '1':
            for (size_t i = 0; i < 3; i++) data_temp[i] = odato.acc_data[i] / 1e3;
            navdata.setAcc(data_temp);
            navdata.setTime(tnow);
            break;
          case '2':
            for (size_t i = 0; i < 3; i++) data_temp[i] = odato.gyr_data[i] / 1e3;
            navdata.setGyr(data_temp);
            navdata.setTime(tnow);
            break;
          case '3':
            // TODO
          default:
            break;
          }

#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif


#if defined (USE_HOST_MEMORY)
          if (navdata.getAcc_s()[2].size()) {
            data[indiceData].d[0] = (double)counter++;
            data[indiceData].set(navdata.getInertial());
            indiceData = (indiceData + 1) % DIMENSIONE_MAX;
          }
#endif

          odato.data_v.clear();
#ifdef ENABLE_SLEEP
          boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
        }
      }
    }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }


  case 10: // MetaSystem_v2
  {
    SerialStream sserial(portacom);
    sserial.exceptions(std::ios::badbit | std::ios::failbit);

    float acc[3], gyro[3];

    try {
      std::string sst;
      std::vector<std::string> strs;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#elif __APPLE__
        if (getc_unlocked(stdin) == 'q')   
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        try {
          std::getline(sserial, sst);
        }
        catch (TimeoutException&) {
          sserial.clear(); //Don't forget to clear error flags after a timeout
          std::cerr << "Timeout occurred" << std::endl;
        }


        //utcTime;nano;lat;lon;tV;fV;pdop;spd(mms);head;ax;ay;az;gx;gy;gz;cnt;rtctime

        tnow = time(NULL);
        boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(";"));
        //std::cout << sst << std::endl;
        //std::cout << strs.size() << std::endl;
        if ((BYPASS_CHECK && (strs.size() > 14)) || ((strs.size() == 18) && (strs[0] != "utcTime"))) {
          acc[0] = (float)(atof(strs[9].c_str()) / 1000.);
          acc[1] = (float)(atof(strs[10].c_str()) / 1000.);
          acc[2] = (float)(atof(strs[11].c_str()) / 1000.);
          gyro[0] = (float)(atof(strs[12].c_str()) / 60.);
          gyro[1] = (float)(atof(strs[13].c_str()) / 60.);
          gyro[2] = (float)(atof(strs[14].c_str()) / 60.);
          navdata.setAcc(acc);
          navdata.setGyr(gyro);
          //navdata.setTime(tnow);
          //navdata.setTime_s(strs[0] + '.' + strs[1]);
          navdata.setTime_s(strs[0]);
#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif

#if defined (USE_HOST_MEMORY)
          if (navdata.getAcc_s()[2].size()) {
            data[indiceData].d[0] = (double)counter++;
            data[indiceData].set(navdata.getInertial());
            indiceData = (indiceData + 1) % DIMENSIONE_MAX;
          }
#endif
        }

#ifdef ENABLE_SLEEP
        boost::this_thread::sleep(boost::posix_time::microseconds((int64_t)(SLEEP_TIME_MICROSECONDS)));
#endif
      }
    }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    break;
  }

  default:
    std::cout << "Error: unidentified object #" << systeminfo << std::endl;
    break;
  }

#ifndef WRITE_ON_STDOUT
  logfile.close();
#endif

  return 0;
}



