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

#pragma once

#define USE_HOST_MEMORY
//#define WRITE_ON_STDOUT


//#define ENABLE_SLEEP
#define SLEEP_TIME_MICROSECONDS 2500
#define SERIAL_PORT_TIMEOUT_SECONDS 3 

//#define COMMA_SEPARATION_VALUE ','
#define COMMA_SEPARATION_VALUE ';'
#define TIME_SEPARATION_VALUE ':'

#define DIMENSIONE_MAX 1000
#ifdef _WIN32
#if !defined (_WIN32_WINNT)
#define _WIN32_WINNT 0x0501
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MAJOR_VERSION 0
#define MINOR_VERSION 11


#define EPOCH_TIME_2000 946684800

#define POS_TIME    0
#define POS_AX      1 
#define POS_AY      2
#define POS_AZ      3
#define POS_GX      4
#define POS_GY      5
#define POS_GZ      6
#define POS_LAT     7
#define POS_LON     8
#define POS_ALT     9
#define POS_SPEED  10
#define POS_HEAD   11
#define POS_QLT    12
#define POS_HDOP   13
#define POS_COUNT  14

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/function.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <bitset>
#include <vector>
#include <list>
#include <ctime>
#include <stdexcept>


#if defined(USE_HOST_MEMORY)
#ifdef _WIN32
#define BOOST_NO_RVALUE_REFERENCES
#include <boost/interprocess/windows_shared_memory.hpp>
#else
#include <boost/interprocess/shared_memory_object.hpp>
#endif
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;
#endif



struct Data{
  double d[7]; //{0=index, 1-6:acc e gyr}
  void set(double *);
  void setAcc(double *);
};



#if defined(USE_HOST_MEMORY)

void remove_host_memory(const char*);
void* allocate_host_memory(const char* , size_t );
void* get_host_allocated_memory(const char* );

#endif
