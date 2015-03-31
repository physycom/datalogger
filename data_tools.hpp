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

struct Data{
  double d[NDATA];
  //int i[NDATA];
};


union raw {
  unsigned char value_ch[2];
  unsigned short value_ush;
  short value_sh;
  short value_15 : 15, : 1;
};



class GPSData
{
private:
  unsigned char align_A;
  unsigned char align_B;
  unsigned char ubx_class;
  unsigned char ubx_id;
  int16_t ubx_length;
  std::vector<char> payload;
  unsigned char ubx_chk_A;
  unsigned char ubx_chk_B;

public:
  GPSData();
  void readData(std::ifstream& inputfile);
  void readDataS(TimeoutSerial& serial);
  void printData();
  void saveData(std::ofstream& outputfile);
};


GPSData::GPSData() {
  align_A = (unsigned char)0xB5;
  align_B = (unsigned char)0x62;
}


void GPSData::readData(std::ifstream& inputfile)
{
  unsigned char buffer[2] = { 0, 0 };
  do {
    inputfile.read((char*)&buffer, sizeof(align_A) + sizeof(align_B));
    if (buffer[0] == align_A && buffer[1] == align_B) break;
    if (buffer[1] == align_A) {
      buffer[0] = buffer[1];
      inputfile.read((char*)&buffer[1], sizeof(align_B));
    }
    //printf("%02x", buffer);
  } while (buffer[0] != align_A && buffer[1] != align_B);
  inputfile.read((char*)&ubx_class, sizeof(ubx_class));
  inputfile.read((char*)&ubx_id, sizeof(ubx_id));
  inputfile.read((char*)&ubx_length, sizeof(ubx_length));

  char * temp = new char[ubx_length];
  payload.resize(ubx_length);
  inputfile.read((char*)&temp, ubx_length*sizeof(char));
  size_t ii = 0;
  for (auto i : payload) i = temp[ii++];
  delete[] temp;

  inputfile.read((char*)&ubx_chk_A, sizeof(ubx_chk_A));
  inputfile.read((char*)&ubx_chk_A, sizeof(ubx_chk_A));
}


void GPSData::readDataS(TimeoutSerial& serial)
{
  unsigned char buffer[2] = { 0, 0 };
  do {
    serial.read((char*)&buffer, sizeof(align_A) + sizeof(align_B));
    if (buffer[0] == align_A && buffer[1] == align_B) break;
    if (buffer[1] == align_A) {
      buffer[0] = buffer[1];
      serial.read((char*)&buffer[1], sizeof(align_B));
    }
    //printf("%02x", buffer);
  } while (buffer[0] != align_A && buffer[1] != align_B);
  serial.read((char*)&ubx_class, sizeof(ubx_class));
  serial.read((char*)&ubx_id, sizeof(ubx_id));
  serial.read((char*)&ubx_length, sizeof(ubx_length));

  char * temp = new char[ubx_length];
  payload.resize(ubx_length);
  serial.read((char*)&temp, ubx_length*sizeof(char));
  size_t ii = 0;
  for (auto i : payload) i = temp[ii++];
  delete[] temp;

  serial.read((char*)&ubx_chk_A, sizeof(ubx_chk_A));
  serial.read((char*)&ubx_chk_A, sizeof(ubx_chk_A));
}


void GPSData::printData()
{
  printf("%02x:%02x - %02x:%02x - PL: %s - %02x:%02x\n", align_A, align_B, ubx_class, ubx_id, payload, ubx_chk_A, ubx_chk_B);
}


void GPSData::saveData(std::ofstream& outputfile)
{
  outputfile << std::hex << std::setw(2) << align_A << ':' << align_B << " - " << ubx_class << ':' << ubx_id << " - PL: ";
  for (auto i : payload) outputfile << i;
  outputfile << " - " << ubx_chk_A << ':' << ubx_chk_B << std::endl;
}




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

class MetasystemData
{
private:
  unsigned char align_char;
public:
  MetasystemData();
  std::vector<std::vector<float>> acc_v;
  void readData(std::ifstream& inputfile);
  void readDataS(TimeoutSerial& serial, size_t sampleCounter);
};


MetasystemData::MetasystemData() {
  align_char = (unsigned char)0xFF;
}


void MetasystemData::readData(std::ifstream& inputfile) {
  unsigned char buffer;
  size_t internal_counter = 0, external_counter = 0;
  std::vector<float> acc(3);
  raw data[3];

  while (!inputfile.eof()) {
    inputfile.read((char*)&buffer, sizeof(buffer));
    if (buffer != align_char) {
      data[external_counter].value_ch[internal_counter++] = buffer;
      if (internal_counter == 2) {
        data[external_counter].value_sh = (data[external_counter].value_ush << 1);
        internal_counter = 0;
        external_counter++;
      }
    }
    else {
      internal_counter = 0;
      external_counter = 0;
    }

    if (external_counter == 3) {
      inputfile.read((char*)&buffer, sizeof(buffer));
      if (buffer == align_char) {
        for (size_t i = 0; i < acc.size(); i++) acc[i] = data[i].value_sh;
        acc_v.push_back(acc);
      }
      else {
        printf("Stream is corrupted!\n");
      }
      external_counter = 0;
    }
  }
}


void MetasystemData::readDataS(TimeoutSerial& serial, size_t sampleCounter) {
  unsigned char buffer;
  size_t internal_counter = 0, external_counter = 0;
  std::vector<float> acc(3);
  raw data[3];

  while (acc_v.size() < sampleCounter) {
    serial.read((char*)&buffer, sizeof(buffer));
    if (buffer != align_char) {
      data[external_counter].value_ch[internal_counter++] = buffer;
      if (internal_counter == 2) {
        data[external_counter].value_sh = (data[external_counter].value_ush << 1);
        //if (data[external_counter].value_ch[1] & 0x4000) data[external_counter].value_ch[1] |= 0x8000; // PP mod, not working
        internal_counter = 0;
        external_counter++;
      }
    }
    else {
      internal_counter = 0;
      external_counter = 0;
    }

    if (external_counter == 3) {
      serial.read((char*)&buffer, sizeof(buffer));
      if (buffer == align_char) {
        for (size_t i = 0; i < acc.size(); i++) acc[i] = data[i].value_sh;
        acc_v.push_back(acc);
      }
      else {
        printf("Stream is corrupted!\n");
      }
      external_counter = 0;
    }
  }
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
  void saveGPSData(std::ofstream& outputfile);
  void printACCData();
  void saveACCData(std::ofstream& outputfile);
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

void InfomobilityData::saveACCData(std::ofstream& outputfile)
{
  outputfile << "AccX=" << accdata.getAccX() << ", AccY=" << accdata.getAccY() << ", AccZ=" << accdata.getAccZ() << std::endl;
}

void InfomobilityData::printGPSData()
{
  return;
}

void InfomobilityData::saveGPSData(std::ofstream& outputfile)
{
  return;
}

class OctoData{
private:
  char header_acc[3], header_gyr[3], header_gps[3];
public:
  OctoData();
  char type;
  char header[3];
  unsigned char id;
  std::int16_t acc_data[3];
  std::int16_t gyr_data[3];
  std::uint32_t timestamp;
  unsigned char nav;
  unsigned char heading;
  unsigned char speed;
  std::int32_t lat, lon;
  std::vector<std::vector<float>> data_v;
  void readData(std::ifstream& inputfile);
  void readDataS(TimeoutSerial& serial, size_t sampleCounter);
};

OctoData::OctoData(){
  header_acc[0] = 'A'; header_acc[1] = 'C'; header_acc[2] = 'C';
  header_gyr[0] = 'G'; header_gyr[1] = 'Y'; header_gyr[2] = 'R';
  header_gps[0] = 'G'; header_gps[1] = 'P'; header_gps[2] = 'S';
};

void OctoData::readData(std::ifstream& inputfile) {
  unsigned char buffer;
  size_t internal_counter = 0, external_counter = 0;

  while (!inputfile.eof()) {
    inputfile.read((char*)&buffer, sizeof(buffer));


    if (buffer == header_acc[0]) {
      inputfile.read((char*)&buffer, sizeof(buffer));
      if (buffer == header_acc[1]) {
        inputfile.read((char*)&buffer, sizeof(buffer));
        if (buffer == header_acc[2]) {
          type = '1';
          inputfile.read((char*)&id, sizeof(id));
          inputfile.read((char*)acc_data, 3 * sizeof(std::int16_t));
        }
      }
    }
    else if (buffer == header_gyr[0]) {
      inputfile.read((char*)&buffer, sizeof(buffer));
      if (buffer == header_gyr[1]) {
        inputfile.read((char*)&buffer, sizeof(buffer));
        if (buffer == header_gyr[2]) {
          type = '2';
          inputfile.read((char*)&id, sizeof(id));
          inputfile.read((char*)gyr_data, 3 * sizeof(std::int16_t));
        }
      }
    }
    else if (buffer == header_gps[0]) {
      inputfile.read((char*)&buffer, sizeof(buffer));
      if (buffer == header_gps[1]) {
        inputfile.read((char*)&buffer, sizeof(buffer));
        if (buffer == header_gps[2]) {
          type = '3';
          inputfile.read((char*)&id, sizeof(id));
          inputfile.read((char*)&timestamp, sizeof(timestamp));
          inputfile.read((char*)&nav, sizeof(nav));
          inputfile.read((char*)&heading, sizeof(heading));
          inputfile.read((char*)&speed, sizeof(speed));
          inputfile.read((char*)&lat, sizeof(lat));
          inputfile.read((char*)&lon, sizeof(lon));
        }
      }
    }
    else
      printf("Unrecognized type: %02x\n",buffer);
  }
}


void OctoData::readDataS(TimeoutSerial& serial, size_t sampleCounter) {
  unsigned char buffer;
  size_t internal_counter = 0, external_counter = 0;

  while (data_v.size() < sampleCounter) {
    serial.read((char*)&buffer, sizeof(buffer));


    if (buffer == header_acc[0]) {
      serial.read((char*)&buffer, sizeof(buffer));
      if (buffer == header_acc[1]) {
        serial.read((char*)&buffer, sizeof(buffer));
        if (buffer == header_acc[2]) {
          type = '1';
          serial.read((char*)&id, sizeof(id));
          serial.read((char*)acc_data, 3 * sizeof(std::int16_t));
        }
      }
    }
    else if (buffer == header_gyr[0]) {
      serial.read((char*)&buffer, sizeof(buffer));
      if (buffer == header_gyr[1]) {
        serial.read((char*)&buffer, sizeof(buffer));
        if (buffer == header_gyr[2]) {
          type = '2';
          serial.read((char*)&id, sizeof(id));
          serial.read((char*)gyr_data, 3 * sizeof(std::int16_t));
        }
      }
    }
    else if (buffer == header_gps[0]) {
      serial.read((char*)&buffer, sizeof(buffer));
      if (buffer == header_gps[1]) {
        serial.read((char*)&buffer, sizeof(buffer));
        if (buffer == header_gps[2]) {
          type = '3';
          serial.read((char*)&id, sizeof(id));
          serial.read((char*)&timestamp, sizeof(timestamp));
          serial.read((char*)&nav, sizeof(nav));
          serial.read((char*)&heading, sizeof(heading));
          serial.read((char*)&speed, sizeof(speed));
          serial.read((char*)&lat, sizeof(lat));
          serial.read((char*)&lon, sizeof(lon));
        }
      }
    }
    else
      printf("Unrecognized type: %02x\n", buffer);
  }
}
