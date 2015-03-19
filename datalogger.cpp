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

#include "datalogger.h"
#include "serial_tools.hpp"
#include "swap_tools.hpp"
#include "data_tools.hpp"

#define WRITE_ON_STDOUT

int main(int argc, char ** argv)
{
  short systeminfo = 0;
  std::cout << "Datalogger v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
  std::cout << "Usage: %s -p [serial_port] -b [baudrate] -t [box_type]" << std::endl;
  std::cout << "\t- [serial_port] serial port number COMx" << std::endl;
  std::cout << "\t- [baudrate] " << std::endl;
  std::cout << "\t- [box_type] " << std::endl;
  std::cout << "\t- default SERIAL_PORT:COM4 \t BAUDRATE:115200" << std::endl;
  std::cout << "new: general fixes and improvements\n" << std::endl;

  std::string serial_port = "COM4";
  int baudrate = 115200;
  if (argc > 1){ /* Parse arguments, if there are arguments supplied */
    for (int i = 1; i < argc; i++){
      if ((argv[i][0] == '-') || (argv[i][0] == '/')){       // switches or options...
        switch (tolower(argv[i][1])){                     // Change to lower...if any
        case 'p':   // if -i or /i
          serial_port = argv[++i];
          break;
        case 'b':   // if -f or /f
          baudrate = atoi(argv[++i]);
          break;
        case 't':   // if -t or /t
          systeminfo = atoi(argv[++i]);
          break;
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

  std::vector<std::string> box_types({"Infomobility","MagnetiMarelli","Texa","ViaSat","MetaSystem"});
  while (systeminfo < 1 || systeminfo > 5){
    std::cout << "Which kind of system is attached? Answer with the number" << std::endl;
    for (size_t i = 0; i < box_types.size(); i++) std::cout << i + 1 << ". " << box_types[i] << std::endl;
    std::cin >> systeminfo;
  }

  std::cout << "Connecting to box TYPE " << box_types[systeminfo-1] << " on PORT " << serial_port << " with BAUDRATE " << baudrate << std::endl;

  Data *data;
  std::ofstream logfile;

  COMport portacom;
  //portacom.set_portname_stdin();
  //portacom.set_baudrate_stdin();
  portacom.set_portname(serial_port);
  portacom.set_baudrate(baudrate);

  if (systeminfo == 1) //Infomobility
  {
    InfomobilityData dato;
    TimeoutSerial serial(portacom.get_portname(), portacom.get_baudrate());
    serial.setTimeout(boost::posix_time::seconds(0));



#if defined (USE_HOST_MEMORY)
    remove_host_memory("I_DATA");
    data = (Data*)allocate_host_memory("I_DATA", (DIMENSIONE_MAX + 1)*sizeof(Data));
#else
    data = new Data[(DIMENSIONE_MAX + 1)*sizeof(Data)];
#endif
    logfile.open("i_data.log", std::ofstream::out);

    try
    {
      bool exit = false;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        dato.loadheaderS(serial);
        dato.allocatePayload(dato.getPayloadSize());
        dato.readPayloadS(serial);
        dato.loadfooterS(serial);
        dato.checkfooter();
        //dato.printheader();
        //dato.printfooter();
        dato.saveheader(logfile);
        dato.savefooter(logfile);
        //dato.deAllocatePayload();
        if (dato.isGPSData())
        {
          dato.recordGPSDataFromPayload();
#ifdef WRITE_ON_STDOUT
          dato.printGPSData();
#else
          dato.saveGPSData(logfile);
#endif
        }
        else
        {
          dato.recordACCDataFromPayload();
#ifdef WRITE_ON_STDOUT
          dato.printACCData();
#else
          dato.saveACCData(logfile);
#endif
        }
      }
    }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }

  else if (systeminfo == 2) //MagnetiMarelli
  {
    SimpleSerial sserial(portacom.get_portname(), portacom.get_baudrate());

#if defined (USE_HOST_MEMORY)
    remove_host_memory("MM_DATA");
    data = (Data*)allocate_host_memory("MM_DATA", (DIMENSIONE_MAX + 1)*sizeof(Data));
#else
    data = new Data[(DIMENSIONE_MAX + 1)*sizeof(Data)];
#endif
    logfile.open("mm_data.log", std::ofstream::out);

    try {
      std::string sst;
      std::vector<std::string> strs;
      int nterm = 0;
      Data dw;
      int indiceData = 0;
      bool exit = false;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        sst = sserial.readLine();
        //sst = serial.readStringUntil("\n");


        if (sst[0] == '{')
        {

          replace(sst.begin(), sst.end(), '{', ' ');
          replace(sst.begin(), sst.end(), '}', ' ');
          replace(sst.begin(), sst.end(), ';', ' ');
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(" "));
          nterm = int(strs.size());
          for (int i = 0; i < nterm; i++) dw.d[i] = atof(strs[i].c_str());
#ifdef WRITE_ON_STDOUT
          for (int i = 0; i < nterm; i++) std::cout << dw.d[i] << " "; std::cout << std::endl;
#else
          for (int i = 0; i < nterm; i++) logfile << dw.d[i] << " "; logfile << std::endl;
#endif
        }
        else
        {
          std::replace(sst.begin(), sst.end(), ';', ' ');
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(" "));
          nterm = int(strs.size());
          for (int i = 0; i < nterm; i++) dw.d[i] = atof(strs[i].c_str());
          for (int i = 0; i < nterm; i++) dw.d[i] /= 256.;
#ifdef WRITE_ON_STDOUT
          for (int i = 0; i < nterm; i++) std::cout << dw.d[i] << " "; std::cout << std::endl;
#else
          for (int i = 0; i < nterm; i++) logfile << dw.d[i] << " "; logfile << std::endl;
#endif
        }
        break;


        data[indiceData] = dw;
        data[DIMENSIONE_MAX].d[0] = indiceData;
        indiceData = (indiceData + 1) % DIMENSIONE_MAX;
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }


  else if (systeminfo == 3) // Texa
  {

    SimpleSerial sserial(portacom.get_portname(), portacom.get_baudrate());

#if defined (USE_HOST_MEMORY)
    remove_host_memory("T_DATA");
    data = (Data*)allocate_host_memory("T_DATA", (DIMENSIONE_MAX + 1)*sizeof(Data));
#else
    data = new Data[(DIMENSIONE_MAX + 1)*sizeof(Data)];
#endif
    logfile.open("t_data.log", std::ofstream::out);

    try {
      std::string sst;
      std::vector<std::string> strs;
      int nterm = 0;
      Data dw;
      int indiceData = 0;
      bool exit = false;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }


        sst = sserial.readLine();
        //sst = serial.readStringUntil("\n");


        boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(";"));
        nterm = int(strs.size());

        for (int i = 0; i < nterm; i++) dw.d[i] = atof(strs[i].c_str());

#ifdef WRITE_ON_STDOUT
        //for (int i = 0; i < nterm; i++) std::cout << boost::lexical_cast<std::string>(dw.d[i]) << " "; std::cout << std::endl;
        for (int i = 0; i < nterm; i++) std::cout << dw.d[i] << " "; std::cout << std::endl;
#else
        for (int i = 0; i < nterm; i++) logfile << dw.d[i] << " "; logfile << std::endl;
#endif

        data[indiceData] = dw;
        data[DIMENSIONE_MAX].d[0] = indiceData;
        indiceData = (indiceData + 1) % DIMENSIONE_MAX;
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }

  else if (systeminfo == 4) // ViaSat
  {
    SimpleSerial sserial(portacom.get_portname(), portacom.get_baudrate());

#if defined (USE_HOST_MEMORY)
    remove_host_memory("V_DATA");
    data = (Data*)allocate_host_memory("V_DATA", (DIMENSIONE_MAX + 1)*sizeof(Data));
#else
    data = new Data[(DIMENSIONE_MAX + 1)*sizeof(Data)];
#endif
    logfile.open("v_data.log", std::ofstream::out);

    try {
      std::string sst;
      std::vector<std::string> strs;
      int nterm = 0;
      Data dw;
      int indiceData = 0;
      bool exit = false;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        sst = sserial.readLine();
        //sst = serial.readStringUntil("\n");

        if (sst[1] == 'G')
        {
          replace(sst.begin(), sst.end(), '{', ' ');
          replace(sst.begin(), sst.end(), '}', ' ');
          replace(sst.begin(), sst.end(), ';', ' ');
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(" "));
          nterm = int(strs.size());

          for (int i = 0; i < nterm; i++) dw.d[i] = atof(strs[i].c_str());

#ifdef WRITE_ON_STDOUT
          for (int i = 0; i < nterm; i++) std::cout << dw.d[i] << " "; std::cout << std::endl;
#else
          for (int i = 0; i < nterm; i++) logfile << dw.d[i] << " "; logfile << std::endl;
#endif
        }
        else
        {
          std::replace(sst.begin(), sst.end(), '$', ' ');
          std::replace(sst.begin(), sst.end(), 'A', ' ');
          std::replace(sst.begin(), sst.end(), '*', ' ');
          sst.insert(8, " ", 1);
          sst.insert(15, " ", 1);
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(" "));
          nterm = int(strs.size());

          for (int i = 0; i < nterm; i++) dw.d[i] = atof(strs[i].c_str());
          for (int i = 0; i < 3; i++) dw.d[i] /= 1000.;

#ifdef WRITE_ON_STDOUT
          for (int i = 0; i < nterm; i++) std::cout << dw.d[i] << " "; std::cout << std::endl;
#else
          for (int i = 0; i < nterm; i++) logfile << dw.d[i] << " "; logfile << std::endl;
#endif
        }

        data[indiceData] = dw;
        data[DIMENSIONE_MAX].d[0] = indiceData;
        indiceData = (indiceData + 1) % DIMENSIONE_MAX;
      }
    }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }

  else if (systeminfo == 5) // MetaSystem
  {
    MetasystemData dato;
    TimeoutSerial serial(portacom.get_portname(), portacom.get_baudrate());
    serial.setTimeout(boost::posix_time::seconds(0));

#if defined (USE_HOST_MEMORY)
    remove_host_memory("MS_DATA");
    data = (Data*)allocate_host_memory("MS_DATA", (DIMENSIONE_MAX + 1)*sizeof(Data));
#else
    data = new Data[(DIMENSIONE_MAX + 1)*sizeof(Data)];
#endif
    logfile.open("ms_data.log", std::ofstream::out);

    try
    {
      bool exit = false;

      while (exit == false)
      {
#ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        dato.readDataS(serial);
#ifdef WRITE_ON_STDOUT
        dato.printData();
#else
        dato.saveData(logfile);
#endif
      }
    }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }


  else
  {
    std::cout << "Error: unidentified object #" << systeminfo << std::endl;
  }


  return 0;
}

