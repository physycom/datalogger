// Copyright 2014, 2015 Stefano Sinigardi, Alessandro Fabbri
// for any question, please mail stefano.sinigardi@gmail.com

#include "serial_tools.h"
#include <array>

class NavData{
  std::vector<std::string> nav_data; // {0=time, 1=ax, 2=ay, 3=az, 4=gx, 5=gy, 6=gz, 7=lat, 8=lon, 9=alt, 10=speed, 11=heading, 12=qlt, 13=HDOP}
public:
  NavData();
  void setTime(time_t);
  void setTime_s(std::string time);
  void setTime(struct tm &gps_time, int nano);
  std::string getTime() const;

  void setAcc_s(std::string * acc_data);
  void setAcc(double * acc_data);
  void setAcc(float * acc_data);
  std::array<std::string,3> getAcc_s() const;
  std::array<double,3> getAcc() const;
  double getAcc(int index);

  void setGyr_s(std::string * gyr_data);
  void setGyr(double * gyr_data);
  void setGyr(float * gyr_data);
  std::array<std::string,3> getGyr_s() const;
  std::array<double,3> getGyr() const;
  double getGyr(int index);

  void setInertial_s(std::string * inertial_data);
  std::array<double,6> getInertial() const;

  void setLat_s(std::string lat);
  void setLat(double lat);
  std::string getLat_s();
  double getLat();

  void setLon_s(std::string lon);
  void setLon(double lon);
  std::string getLon_s();
  double getLon();

  void setAlt_s(std::string alt);
  void setAlt(double alt);
  std::string getAlt_s();
  double getAlt();

  void setSpeed_s(std::string speed);
  void setSpeed(double speed);
  std::string getSpeed_s();
  double getSpeed();

  void setHead_s(std::string head);
  void setHead(double head);
  std::string getHead_s();
  double getHead();

  void setQlt_s(std::string qlt);
  void setQlt(double qlt);
  std::string getQlt_s();
  double getQlt();

  void setHDOP_s(std::string hdop);
  void setHDOP(double hdop);
  std::string getHDOP_s();
  double getHDOP();

  std::string to_string();
};

NavData::NavData(){
  nav_data.resize(POS_COUNT);
}

void NavData::setTime(time_t tnow){
  struct tm * now = localtime(&tnow);
  std::stringstream date;
  date << now->tm_year + 1900 << TIME_SEPARATION_VALUE << (now->tm_mon + 1) << TIME_SEPARATION_VALUE << now->tm_mday << TIME_SEPARATION_VALUE << now->tm_hour << TIME_SEPARATION_VALUE << now->tm_min << TIME_SEPARATION_VALUE << now->tm_sec;
  nav_data[POS_TIME] = date.str();
};

void NavData::setTime_s(std::string time){
  nav_data[POS_TIME] = time;
};

void NavData::setTime(struct tm &gps_time, int nano){
  time_t gps_time_t = mktime(&gps_time);
  time_t tnow = (int)(gps_time_t + nano * 1e-9);
  struct tm * now = localtime(&tnow);
  std::stringstream date;
  date << now->tm_year + 1900 << TIME_SEPARATION_VALUE << (now->tm_mon + 1) << TIME_SEPARATION_VALUE << now->tm_mday << TIME_SEPARATION_VALUE << now->tm_hour << TIME_SEPARATION_VALUE << now->tm_min << TIME_SEPARATION_VALUE
    << std::fixed << std::setprecision(3) << now->tm_sec + nano*1e-9;
  nav_data[POS_TIME] = date.str();
}

std::string NavData::getTime() const {
  return nav_data[POS_TIME];
}

void NavData::setAcc_s(std::string * acc_data){
  for (int i = 0; i < 3; i++) nav_data[i + POS_AX] = acc_data[i];
};

void NavData::setAcc(double * acc_data){
  for (int i = 0; i < 3; i++) nav_data[i + POS_AX] = boost::lexical_cast<std::string>(acc_data[i]);
};

void NavData::setAcc(float * acc_data){
  for (int i = 0; i < 3; i++) nav_data[i + POS_AX] = boost::lexical_cast<std::string>(acc_data[i]);
};

std::array<std::string,3> NavData::getAcc_s() const{
  std::array<std::string,3> acc{};
  for(int i=0;i<3;i++) acc[i]=nav_data[i + POS_AX];
  return acc;
};

std::array<double,3> NavData::getAcc() const{
  std::array<double,3> acc{};
  for(int i=0;i<3;i++) acc[i]=atof(nav_data[i + POS_AX].c_str());
  return acc;
};

double NavData::getAcc(int index){
  return atof(nav_data[index + POS_AX].c_str());
};

void NavData::setGyr_s(std::string * gyr_data){
  for (int i = 0; i < 3; i++) nav_data[i + POS_GX] = gyr_data[i];
};

void NavData::setGyr(double * gyr_data){
  for (int i = 0; i < 3; i++) nav_data[i + POS_GX] = boost::lexical_cast<std::string>(gyr_data[i]);
};

void NavData::setGyr(float * gyr_data){
  for (int i = 0; i < 3; i++) nav_data[i + POS_GX] = boost::lexical_cast<std::string>(gyr_data[i]);
};

std::array<std::string,3> NavData::getGyr_s() const{
  std::array<std::string,3> gyr{};
  for(int i=0;i<3;i++) gyr[i]=nav_data[i + POS_GX];
  return gyr;
};

std::array<double,3> NavData::getGyr() const{
  std::array<double,3> gyr{};
  for(int i=0;i<3;i++) gyr[i]=atof(nav_data[i + POS_GX].c_str());
  return gyr;
};

double NavData::getGyr(int index){
  return atof(nav_data[index + POS_GX].c_str());
};

void NavData::setInertial_s(std::string * inertial_data){
  for (int i = 0; i < 6; i++) nav_data[i + POS_AX] = inertial_data[i];
};

std::array<double,6> NavData::getInertial() const{
  std::array<double,6> data{};
  for(int i=0;i<6;i++) data[i] = atof(nav_data[POS_AX + i].c_str());
  return data;
};

void NavData::setLat_s(std::string lat){
  nav_data[POS_LAT] = lat;
};

void NavData::setLat(double lat){
  nav_data[POS_LAT] = boost::lexical_cast<std::string>(lat);
};

std::string NavData::getLat_s(){
  return nav_data[POS_LAT];
};

double NavData::getLat(){
  return atof(nav_data[POS_LAT].c_str());
};

void NavData::setLon_s(std::string lon){
  nav_data[POS_LON] = lon;
};

void NavData::setLon(double lon){
  nav_data[POS_LON] = boost::lexical_cast<std::string>(lon);
};

std::string NavData::getLon_s(){
  return nav_data[POS_LON];
};

double NavData::getLon(){
  return atof(nav_data[POS_LON].c_str());
};

void NavData::setAlt_s(std::string alt){
  nav_data[POS_ALT] = alt;
};

void NavData::setAlt(double alt){
  nav_data[POS_ALT] = boost::lexical_cast<std::string>(alt);
};

std::string NavData::getAlt_s(){
  return nav_data[POS_ALT];
};

double NavData::getAlt(){
  return atof(nav_data[POS_ALT].c_str());
};

void NavData::setSpeed_s(std::string speed){
  nav_data[POS_SPEED] = speed;
};

void NavData::setSpeed(double speed){
  nav_data[POS_SPEED] = boost::lexical_cast<std::string>(speed);
};

std::string NavData::getSpeed_s(){
  return nav_data[POS_SPEED];
};

double NavData::getSpeed(){
  return atof(nav_data[POS_SPEED].c_str());
};

void NavData::setHead_s(std::string head){
  nav_data[POS_HEAD] = head;
};

void NavData::setHead(double head){
  nav_data[POS_HEAD] = boost::lexical_cast<std::string>(head);
};

std::string NavData::getHead_s(){
  return nav_data[POS_HEAD];
};

double NavData::getHead(){
  return atof(nav_data[POS_HEAD].c_str());
};

void NavData::setQlt_s(std::string qlt){
  nav_data[POS_QLT] = qlt;
};

void NavData::setQlt(double qlt){
  nav_data[POS_QLT] = boost::lexical_cast<std::string>(qlt);
};

std::string NavData::getQlt_s(){
  return nav_data[POS_QLT];
};

double NavData::getQlt(){
  return atof(nav_data[POS_QLT].c_str());
};

void NavData::setHDOP_s(std::string hdop){
  nav_data[POS_HDOP] = hdop;
};

void NavData::setHDOP(double hdop){
  nav_data[POS_HDOP] = boost::lexical_cast<std::string>(hdop);
};

std::string NavData::getHDOP_s(){
  return nav_data[POS_HDOP];
};

double NavData::getHDOP(){
  return atof(nav_data[POS_HDOP].c_str());
};

std::string NavData::to_string(){
  std::string str("");
  for (auto data : nav_data) str += data + COMMA_SEPARATION_VALUE;
  return str.substr(0, str.size() - 1);
};

//***************************************************************************************************************

union raw {
  unsigned char value_ch[2];
  unsigned short value_ush;
  short value_sh;
  short value_15 : 15, : 1;
};

class GPSData {
private:
  unsigned char align_A;
  unsigned char align_B;
  unsigned char ubx_class;
  unsigned char ubx_id;
  int16_t ubx_length;
  std::vector<char> payload;
  unsigned char ubx_chk_A;
  unsigned char ubx_chk_B;
  unsigned char ubx_navpvt_class;
  unsigned char ubx_navpvt_id;
  unsigned char fix;
  int32_t lon;
  int32_t lat;
  int32_t alt;
  int32_t speed;
  uint32_t heading;
public:
  GPSData();
  int nano;
  struct tm gps_time;
  void readData(std::ifstream& inputfile);
  void readDataStr(SerialStream& serial);
};


GPSData::GPSData() {
  align_A = (unsigned char)0xB5;
  align_B = (unsigned char)0x62;
  ubx_navpvt_class = 0x01;
  ubx_navpvt_id = 0x07;
}


void GPSData::readData(std::ifstream& inputfile) {
  unsigned char buffer;
  bool found = false;
  char * payload;

  while (!found) {
    inputfile.read((char*)&buffer, sizeof(buffer));
    if (buffer == align_A) {
      if (inputfile.read((char*)&buffer, sizeof(buffer)).gcount() < sizeof(buffer)) break;
      if (buffer == align_B) {
        if (inputfile.read((char*)&ubx_class, sizeof(ubx_class)).gcount() < sizeof(ubx_class)) break;
        if (inputfile.read((char*)&ubx_id, sizeof(ubx_id)).gcount() < sizeof(ubx_id)) break;
        if (inputfile.read((char*)&ubx_length, sizeof(ubx_length)).gcount() < sizeof(ubx_length)) break;

        if (ubx_length > 0){
          payload = new char[ubx_length];
          if (inputfile.read(payload, ubx_length*sizeof(char)).gcount() < ubx_length*sizeof(char)) break;

          if (ubx_class == ubx_navpvt_class && ubx_id == ubx_navpvt_id){
            unsigned char uc_temp; unsigned short us_temp;

            memcpy((void *)&us_temp, &payload[UBX_YEAR_OFFSET], sizeof(us_temp));
            gps_time.tm_year = ((int)us_temp) - 1900;
            memcpy((void *)&uc_temp, &payload[UBX_MONTH_OFFSET], sizeof(uc_temp));
            gps_time.tm_mon = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_DAY_OFFSET], sizeof(uc_temp));
            gps_time.tm_mday = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_HOUR_OFFSET], sizeof(uc_temp));
            gps_time.tm_hour = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_MIN_OFFSET], sizeof(uc_temp));
            gps_time.tm_min = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_SEC_OFFSET], sizeof(uc_temp));
            gps_time.tm_sec = (int)uc_temp;
            memcpy((void *)&nano, &payload[UBX_NANO_OFFSET], sizeof(nano));
            memcpy((void *)&fix, &payload[UBX_FIX_OFFSET], sizeof(fix));
            memcpy((void *)&lon, &payload[UBX_LON_OFFSET], sizeof(lon));
            memcpy((void *)&lat, &payload[UBX_LAT_OFFSET], sizeof(lat));
            memcpy((void *)&alt, &payload[UBX_ALT_OFFSET], sizeof(alt));
            memcpy((void *)&speed, &payload[UBX_SPEED_OFFSET], sizeof(speed));
            memcpy((void *)&heading, &payload[UBX_HEAD_OFFSET], sizeof(heading));
          }

          delete[] payload;
        }

        if (inputfile.read((char*)&ubx_chk_A, sizeof(ubx_chk_A)).gcount() < sizeof(ubx_chk_A)) break;
        if (inputfile.read((char*)&ubx_chk_B, sizeof(ubx_chk_B)).gcount() < sizeof(ubx_chk_B)) break;

        found = true;
        printf("%02x - %02x:%02x - %02x:%02x - Len: %hi - %02x:%02x\n", found, align_A, align_B, ubx_class, ubx_id, ubx_length, ubx_chk_A, ubx_chk_B);
      }
      else printf("align_B not valid: %02x : %02x\n", align_A, align_B);
    }
    else printf("align_A not valid : %02x : %02x\n", align_A, align_B);
  }
}



void GPSData::readDataStr(SerialStream& serial) {
  unsigned char buffer;
  bool found = false;
  char * payload;

  while (!found) {
    serial.read((char*)&buffer, sizeof(buffer));
    if (buffer == align_A) {
      if (serial.read((char*)&buffer, sizeof(buffer)).gcount() < sizeof(buffer)) break;
      if (buffer == align_B) {
        if (serial.read((char*)&ubx_class, sizeof(ubx_class)).gcount() < sizeof(ubx_class)) break;
        if (serial.read((char*)&ubx_id, sizeof(ubx_id)).gcount() < sizeof(ubx_id)) break;
        if (serial.read((char*)&ubx_length, sizeof(ubx_length)).gcount() < sizeof(ubx_length)) break;

        if (ubx_length > 0){
          payload = new char[ubx_length];
          if (serial.read(payload, ubx_length*sizeof(char)).gcount() < ubx_length*sizeof(char)) break;

          if (ubx_class == ubx_navpvt_class && ubx_id == ubx_navpvt_id){
            unsigned char uc_temp; unsigned short us_temp;

            memcpy((void *)&us_temp, &payload[UBX_YEAR_OFFSET], sizeof(us_temp));
            gps_time.tm_year = ((int)us_temp) - 1900;
            memcpy((void *)&uc_temp, &payload[UBX_MONTH_OFFSET], sizeof(uc_temp));
            gps_time.tm_mon = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_DAY_OFFSET], sizeof(uc_temp));
            gps_time.tm_mday = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_HOUR_OFFSET], sizeof(uc_temp));
            gps_time.tm_hour = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_MIN_OFFSET], sizeof(uc_temp));
            gps_time.tm_min = (int)uc_temp;
            memcpy((void *)&uc_temp, &payload[UBX_SEC_OFFSET], sizeof(uc_temp));
            gps_time.tm_sec = (int)uc_temp;
            memcpy((void *)&nano, &payload[UBX_NANO_OFFSET], sizeof(nano));
            memcpy((void *)&fix, &payload[UBX_FIX_OFFSET], sizeof(fix));
            memcpy((void *)&lon, &payload[UBX_LON_OFFSET], sizeof(lon));
            memcpy((void *)&lat, &payload[UBX_LAT_OFFSET], sizeof(lat));
            memcpy((void *)&alt, &payload[UBX_ALT_OFFSET], sizeof(alt));
            memcpy((void *)&speed, &payload[UBX_SPEED_OFFSET], sizeof(speed));
            memcpy((void *)&heading, &payload[UBX_HEAD_OFFSET], sizeof(heading));
          }

          delete[] payload;
        }

        if (serial.read((char*)&ubx_chk_A, sizeof(ubx_chk_A)).gcount() < sizeof(ubx_chk_A)) break;
        if (serial.read((char*)&ubx_chk_B, sizeof(ubx_chk_B)).gcount() < sizeof(ubx_chk_B)) break;

        found = true;
        printf("%02x - %02x:%02x - %02x:%02x - Len: %hi - %02x:%02x\n", found, align_A, align_B, ubx_class, ubx_id, ubx_length, ubx_chk_A, ubx_chk_B);
      }
      else printf("align_B not valid: %02x : %02x\n", align_A, align_B);
    }
    else printf("align_A not valid : %02x : %02x\n", align_A, align_B);
  }
}


class ACCData {
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

void ACCData::setAccX(short accx) {
  AccX = accx;
}

void ACCData::setAccY(short accy) {
  AccY = accy;
}

void ACCData::setAccZ(short accz) {
  AccZ = accz;
}

short ACCData::getAccX() {
  return AccX;
}

short ACCData::getAccY() {
  return AccY;
}

short ACCData::getAccZ() {
  return AccZ;
}

class MetasystemData {
private:
  unsigned char align_char;
public:
  MetasystemData();
  std::vector<std::vector<float>> acc_v;
  void readData(std::ifstream& inputfile, size_t sampleCounter);
  void readDataAsync(const char *data, unsigned int len);
  const char getAlignChar();
};


MetasystemData::MetasystemData() {
  align_char = (unsigned char)0xFF;
}

const char MetasystemData::getAlignChar() {
  return (const char) align_char;
}

void MetasystemData::readData(std::ifstream& inputfile, size_t sampleCounter) {
  unsigned char buffer;
  size_t internal_counter = 0, external_counter = 0;
  std::vector<float> acc(3);
  raw data[3];

  while (acc_v.size() < sampleCounter) {
    inputfile.read((char*)&buffer, sizeof(buffer));

    if (inputfile.eof()) break;

    if (buffer != align_char) {
      data[external_counter].value_ch[internal_counter++] = buffer;
      if (internal_counter == 2) {
        data[external_counter].value_sh = (data[external_counter].value_ush << 1);
        //if (data[external_counter].value_ch[1] & 0x4000) data[external_counter].value_ch[1] |= 0x8000; // PaoloPariani mod, not working
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
      if (inputfile.eof()) break;
      if (buffer == align_char) {
        for (size_t i = 0; i < acc.size(); i++) acc[i] = ((float)data[i].value_sh) / 1e3f;
        acc_v.push_back(acc);
      }
      else {
        printf("%02x %02x | %02x %02x | %02x %02x | %02x - stream corrupted, not ff terminated!\n", data[0].value_ch[0], data[0].value_ch[1], data[1].value_ch[0], data[1].value_ch[1], data[2].value_ch[0], data[2].value_ch[1], buffer);
      }
      external_counter = 0;
    }
  }
}



void MetasystemData::readDataAsync(const char *serial_data, unsigned int len) {
  std::list<unsigned char> v(serial_data, serial_data + len);
  size_t internal_counter = 0, external_counter = 0;
  std::vector<float> acc(3);
  raw data[3];

  unsigned char poppd;

  while( v.size() ) {
    poppd = v.front();
    v.pop_front();

    if (poppd != align_char) {
      data[external_counter].value_ch[internal_counter++] = poppd;
      if (internal_counter == 2) {
        data[external_counter].value_sh = (data[external_counter].value_ush << 1);
        //if (data[external_counter].value_ch[1] & 0x4000) data[external_counter].value_ch[1] |= 0x8000; // PaoloPariani mod, not working
        internal_counter = 0;
        external_counter++;
        continue;
      }
    }
    else {
      internal_counter = 0;
      external_counter = 0;
    }

    if (external_counter == 3 && poppd == align_char) {
      for (size_t i = 0; i < acc.size(); i++) acc[i] = ((float)data[i].value_sh) / 1e3f;
      acc_v.push_back(acc);
    }
    else external_counter = 0;
  }
}


class InfomobilityData {
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

void InfomobilityData::saveheader(std::ofstream& outputfile) {
  outputfile.write(&h1, sizeof(h1));
  outputfile.write(&h2, sizeof(h2));
  outputfile.write(&hclass, sizeof(hclass));
  outputfile.write(&hid, sizeof(hid));
  outputfile.write((char*)&hlength, sizeof(hlength));
}

void InfomobilityData::printheader() {
  std::cout << "h1=" << h1 << "; h2=" << h2 << "hclass=" << hclass << "; hid=" << hid << std::endl;
  std::cout << "Payload length: " << hlength << std::endl;
}

void InfomobilityData::loadheader(std::ifstream& inputfile) {
  h1 = 0, h2 = 0;
  while (h1 != 0xB5 || h2 != 0x62)
  {
    inputfile.read(&h1, sizeof(h1));
    if (inputfile.eof()) break;
    inputfile.read(&h2, sizeof(h2));
    if (inputfile.eof()) break;
  }
  inputfile.read(&hclass, sizeof(hclass));
  if (inputfile.eof()) return;
  inputfile.read(&hid, sizeof(hid));
  if (inputfile.eof()) return;
  inputfile.read((char*)&hlength, sizeof(hlength));
  if (inputfile.eof()) return;
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
  if (inputfile.eof()) return;
  inputfile.read(&f2, sizeof(f2));
  if (inputfile.eof()) return;
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
  if (inputfile.eof()) return;
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
  void readData(std::ifstream& inputfile, size_t sampleCounter);
  void readDataStr(SerialStream& serial, bool extracted_data);
};

OctoData::OctoData(){
  header_acc[0] = 'A'; header_acc[1] = 'C'; header_acc[2] = 'C';
  header_gyr[0] = 'G'; header_gyr[1] = 'Y'; header_gyr[2] = 'R';
  header_gps[0] = 'G'; header_gps[1] = 'P'; header_gps[2] = 'S';
};

void OctoData::readData(std::ifstream& inputfile, size_t sampleCounter) {
  unsigned char buffer;
  std::vector<float> dato(3);

  while (data_v.size() < sampleCounter) {
    inputfile.read((char*)&buffer, sizeof(buffer));
    if (inputfile.eof()) break;

    if (buffer == header_acc[0] || buffer == header_gyr[0]) {
      inputfile.read((char*)&buffer, sizeof(buffer));
      if (inputfile.eof()) break;
      if (buffer == header_acc[1] || buffer == header_gyr[1]) {
        inputfile.read((char*)&buffer, sizeof(buffer));
        if (inputfile.eof()) break;
        if (buffer == header_acc[2]) {
          type = '1';
          inputfile.read((char*)&id, sizeof(id));
          if (inputfile.eof()) break;
          inputfile.read((char*)acc_data, 3 * sizeof(std::int16_t));
          if (inputfile.eof()) break;
          for (size_t i = 0; i < dato.size(); i++) dato[i] = acc_data[i];
          data_v.push_back(dato);
        }
        else if (buffer == header_gyr[2]) {
          type = '2';
          inputfile.read((char*)&id, sizeof(id));
          if (inputfile.eof()) break;
          inputfile.read((char*)gyr_data, 3 * sizeof(std::int16_t));
          if (inputfile.eof()) break;
          for (size_t i = 0; i < dato.size(); i++) dato[i] = acc_data[i];
          data_v.push_back(dato);
        }
      }
    }
    else if (buffer == header_gps[0]) {
      inputfile.read((char*)&buffer, sizeof(buffer));
      if (inputfile.eof()) break;
      if (buffer == header_gps[1]) {
        inputfile.read((char*)&buffer, sizeof(buffer));
        if (inputfile.eof()) break;
        if (buffer == header_gps[2]) {
          type = '3';
          inputfile.read((char*)&id, sizeof(id));
          if (inputfile.eof()) break;
          inputfile.read((char*)&timestamp, sizeof(timestamp));
          if (inputfile.eof()) break;
          inputfile.read((char*)&nav, sizeof(nav));
          if (inputfile.eof()) break;
          inputfile.read((char*)&heading, sizeof(heading));
          if (inputfile.eof()) break;
          inputfile.read((char*)&speed, sizeof(speed));
          if (inputfile.eof()) break;
          inputfile.read((char*)&lat, sizeof(lat));
          if (inputfile.eof()) break;
          inputfile.read((char*)&lon, sizeof(lon));
          if (inputfile.eof()) break;
        }
      }
    }
  }
}



void OctoData::readDataStr(SerialStream& serial, bool extracted_data) {
  unsigned char buffer;
  std::vector<float> dato(3);
  std::streamsize extracted;
  extracted_data = false;

  while (!extracted_data) {
    extracted = serial.read((char*)&buffer, sizeof(buffer)).gcount();
    if (extracted < sizeof(buffer)) break;

    if (buffer == header_acc[0] || buffer == header_gyr[0]) {
      extracted = serial.read((char*)&buffer, sizeof(buffer)).gcount();
      if (extracted < sizeof(buffer)) break;
      if (buffer == header_acc[1] || buffer == header_gyr[1]) {
        extracted = serial.read((char*)&buffer, sizeof(buffer)).gcount();
        if (extracted < sizeof(buffer)) break;
        if (buffer == header_acc[2]) {
          type = '1';
          extracted = serial.read((char*)&id, sizeof(id)).gcount();
          if (extracted < sizeof(id)) break;
          extracted = serial.read((char*)acc_data, 3 * sizeof(std::int16_t)).gcount();
          if (extracted < sizeof(std::int16_t)) break;
          for (size_t i = 0; i < dato.size(); i++) dato[i] = acc_data[i];
          data_v.push_back(dato);
          extracted_data = true;
        }
        else if (buffer == header_gyr[2]) {
          type = '2';
          extracted = serial.read((char*)&id, sizeof(id)).gcount();
          if (extracted < sizeof(id)) break;
          extracted = serial.read((char*)gyr_data, 3 * sizeof(std::int16_t)).gcount();
          if (extracted < sizeof(std::int16_t)) break;
          for (size_t i = 0; i < dato.size(); i++) dato[i] = acc_data[i];
          data_v.push_back(dato);
          extracted_data = true;
        }
      }
    }
    else if (buffer == header_gps[0]) {
      extracted = serial.read((char*)&buffer, sizeof(buffer)).gcount();
      if (extracted < sizeof(buffer)) break;
      if (buffer == header_gps[1]) {
        extracted = serial.read((char*)&buffer, sizeof(buffer)).gcount();
        if (extracted < sizeof(buffer)) break;
        if (buffer == header_gps[2]) {
          type = '3';
          extracted = serial.read((char*)&id, sizeof(id)).gcount();
          if (extracted < sizeof(id)) break;
          extracted = serial.read((char*)&timestamp, sizeof(timestamp)).gcount();
          if (extracted < sizeof(timestamp)) break;
          extracted = serial.read((char*)&nav, sizeof(nav)).gcount();
          if (extracted < sizeof(nav)) break;
          extracted = serial.read((char*)&heading, sizeof(heading)).gcount();
          if (extracted < sizeof(heading)) break;
          extracted = serial.read((char*)&speed, sizeof(speed)).gcount();
          if (extracted < sizeof(speed)) break;
          extracted = serial.read((char*)&lat, sizeof(lat)).gcount();
          if (extracted < sizeof(lat)) break;
          extracted = serial.read((char*)&lon, sizeof(lon)).gcount();
          if (extracted < sizeof(lon)) break;
          // data not saved anywhere for now
          extracted_data = true;
        }
      }
    }
  }
}
