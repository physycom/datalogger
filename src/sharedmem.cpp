// Copyright 2014, 2015 Stefano Sinigardi, Alessandro Fabbri
// for any question, please mail stefano.sinigardi@gmail.com

#include "datalogger.h"

#if defined(USE_HOST_MEMORY)

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


void Data::set(const std::array<double,6>& data) {
  for (int i = 1; i < 7; i++) d[i] = data[i - 1];
}

void Data::setAcc(const std::array<double,3>& data) {
  for (int i = 1; i < 4; i++) d[i] = data[i - 1];
}

