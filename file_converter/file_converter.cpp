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

#include "data_tools.hpp"



int main(int argc, char ** argv)
{
  std::string filename;
  size_t systeminfo = 0;
  std::ifstream datafile;
  std::vector<std::string> box_types({ "Infomobility", "MagnetiMarelli", "Texa", "ViaSat", "MetaSystem", "UBX", "Octo", "NMEA", "MagnetiMarelli_v2" });

  std::cout << "Datalogger v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
  std::cout << "Usage: " << argv[0] << " -f [filename] -t [box_type] -h (shows help and quit)" << std::endl;
  std::cout << "new: general fixes and improvements\n" << std::endl;


  if (argc > 1){ /* Parse arguments, if there are arguments supplied */
    for (int i = 1; i < argc; i++){
      if ((argv[i][0] == '-') || (argv[i][0] == '/')){       // switches or options...
        switch (tolower(argv[i][1])){
        case 'f':
          filename = argv[++i];
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

  while (filename == ""){
    std::cout << "Filename: " << std::endl;
    std::cin >> filename;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  std::cout << "Opening file " << filename << std::endl;

  while (systeminfo < 1 || systeminfo > box_types.size()){
    std::cout << "Which kind of system was used to generate the file? Answer with the number" << std::endl;
    for (size_t i = 0; i < box_types.size(); i++) std::cout << i + 1 << ". " << box_types[i] << std::endl;
    std::cin >> systeminfo;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  NavData navdata;
  int indiceData = 0;
  size_t counter = 0;
  bool exit = false;
  std::ofstream logfile;

  logfile.open(box_types[systeminfo - 1] + ".log", std::ofstream::out);

#if defined (USE_HOST_MEMORY)
  Data *data;
  data = (Data*)get_host_allocated_memory(box_types[systeminfo - 1].c_str());
#endif

  if (systeminfo == 1) //Infomobility
  {
    InfomobilityData dato;

    datafile.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {

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

        dato.loadheader(datafile);
        if (datafile.eof()) break;
        dato.allocatePayload(dato.getPayloadSize());

        dato.readPayload(datafile);
        if (datafile.eof()) break;

        dato.loadfooter(datafile);
        if (datafile.eof()) break;

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
    datafile.open(filename.c_str(), std::ios::in);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {
      std::string sst;
      std::vector<std::string> strs;

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

        std::getline(datafile, sst);
        if (datafile.eof()) break;

        if (sst[0] == '{')
        {
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("{}; "));
          navdata.setAcc_s(&strs[0]);
#ifdef WRITE_ON_STDOUT
          std::cout << navdata.to_string() << std::endl;
#else
          logfile << navdata.to_string() << std::endl;
#endif
        }
        else
        {
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("; "));
          double gyr_data[3];
          for (size_t i = 0; i < strs.size(); i++) gyr_data[i] = atof(strs[i].c_str()) / 256.;
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
    datafile.open(filename.c_str(), std::ios::in);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {
      std::string sst;
      std::vector<std::string> strs;

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

        std::getline(datafile, sst);
        if (datafile.eof()) break;

        boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(";"));
        if (strs.size() == 21) navdata.setInertial_s(&strs[strs.size() - 9]);

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
      }
    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
    }

  else if (systeminfo == 4) // ViaSat
  {
    datafile.open(filename.c_str(), std::ios::in);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {
      std::string sst;
      std::vector<std::string> strs;

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

        std::getline(datafile, sst);
        if (datafile.eof()) break;

        if (sst[1] == 'G')
        {
          boost::algorithm::split(strs, sst, boost::algorithm::is_any_of("{}; "));

          navdata.setAcc_s(&strs[0]);

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
    datafile.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {

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

        dato.readData(datafile, 1);
        if (datafile.eof()) break;

        for (size_t i = 0; i < dato.acc_v.size(); i++) {
          navdata.setAcc(&dato.acc_v[i][0]);

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
        dato.acc_v.clear();
        }
      }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }


  else if (systeminfo == 6) // UBX
  {
    GPSData dato;
    datafile.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {

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

        dato.readData(datafile);
        if (datafile.eof()) break;
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
  }

  else if (systeminfo == 7) // Octo
  {
    OctoData dato;
    datafile.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {

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

        dato.readData(datafile, 1);
        if (datafile.eof()) break;

        double data_temp[3];
        switch (dato.type){
        case '1':
          for (size_t i = 0; i < 3; i++) data_temp[i] = dato.acc_data[i] / 1e3;
          navdata.setAcc(data_temp);
          break;
        case '2':
          for (size_t i = 0; i < 3; i++) data_temp[i] = dato.gyr_data[i] / 1e3;
          navdata.setGyr(data_temp);
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

        dato.data_v.clear();
      }
  }

    catch (boost::system::system_error& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return 1;
    }
  }


  else if (systeminfo == 8) // NMEA
  {
    GPSData dato;
    datafile.open(filename.c_str(), std::ios::in);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    std::vector<boost::regex> patterns;
    //std::vector<std::string> pattern_names({ "GSV", "GLL", "RMC", "VTG", "GGA", "GSA" });
    std::vector<std::string> pattern_names({ "RMC" });
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
#else
        if (fgetc_unlocked(stdin) == 'q')  // da implementare con fgetc_unlocked, questo e' solo un tentativo alla cieca, non so come funzioni!
#endif
        {
          exit = true;
        }

        std::getline(datafile, sst);
        if (datafile.eof()) break;

        boost::algorithm::split(strs, sst, boost::algorithm::is_any_of(","));
        found = false;
        for (auto i : patterns) if (boost::regex_search(strs[0], i)) found = true;

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
    }



  else if (systeminfo == 9) // MagnetiMarelli_v2 //Octo-clone
  {
    OctoData dato;
    datafile.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (datafile.fail()) {
      std::cout << "Unable to open file" << std::endl;
      std::exit(5);
    }

    try {

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

        dato.readData(datafile, 1);
        if (datafile.eof()) break;

        double data_temp[3];
        switch (dato.type){
        case '1':
          for (size_t i = 0; i < 3; i++) data_temp[i] = dato.acc_data[i] / 1e3;
          navdata.setAcc(data_temp);
          break;
        case '2':
          for (size_t i = 0; i < 3; i++) data_temp[i] = dato.gyr_data[i] / 1e3;
          navdata.setGyr(data_temp);
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

        dato.data_v.clear();
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


  logfile.close();
  datafile.close();

  return 0;
}

