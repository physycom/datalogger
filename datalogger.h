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


#define USE_SERIAL_PORT
//#define USE_BINARY_FILE

#define DIMENSIONE_MAX 1000
#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif

#define MAJOR_VERSION 0
#define MINOR_VERSION 8

//#define USE_HOST_MEMORY
#define DEBUG
#define NDATA 17
#define META_CONVERSION 0.002

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <bitset>
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
