// Copyright 2014, 2015 Stefano Sinigardi, Alessandro Fabbri
// for any question, please mail stefano.sinigardi@gmail.com

#pragma once

#define USE_HOST_MEMORY
#define WRITE_ON_STDOUT

#define BYPASS_CHECK true

//#define ENABLE_SLEEP
#define SLEEP_TIME_MICROSECONDS 2500
#define SERIAL_PORT_TIMEOUT_SECONDS 3

//#define COMMA_SEPARATION_VALUE ','
#define COMMA_SEPARATION_VALUE ';'
#define TIME_SEPARATION_VALUE ':'

#define DIMENSIONE_MAX 1000

#include "version.h"

#define EPOCH_TIME_2000 946684800

#define UBX_YEAR_OFFSET    4
#define UBX_MONTH_OFFSET   6
#define UBX_DAY_OFFSET     7
#define UBX_HOUR_OFFSET    8
#define UBX_MIN_OFFSET     9
#define UBX_SEC_OFFSET    10
#define UBX_NANO_OFFSET   16
#define UBX_FIX_OFFSET    20
#define UBX_LON_OFFSET    24
#define UBX_LAT_OFFSET    28
#define UBX_ALT_OFFSET    36
#define UBX_SPEED_OFFSET  60
#define UBX_HEAD_OFFSET   60

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

#ifdef _WIN32
#if !defined (_WIN32_WINNT)
#define _WIN32_WINNT 0x0501
#endif
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

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
#include <cmath>



#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
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
#include <array>
#include "data.hpp"
#include "shared_memory.hpp"

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





