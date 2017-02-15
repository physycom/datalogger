
#include "draw.h"
#include "datalogger.h"

using namespace boost::algorithm;

extern Data **data;
extern int * indiceData;

extern Frame *scene;


void idle_cb(void*) {
  scene->redraw();
}



int main(int argc, char **argv) {

#if defined (USE_HOST_MEMORY)
  std::vector<std::string> box_types({ "Infomobility", "MagnetiMarelli", "Texa", "ViaSat", "MetaSystem", "UBX", "Octo", "NMEA", "MagnetiMarelli_v2" });
  data = new Data*[box_types.size()];
  indiceData = new int[box_types.size()];

  unsigned int counter = 0;
  for (auto box_type : box_types) {
    remove_host_memory(box_type.c_str());
    data[counter++] = (Data*)allocate_host_memory(box_type.c_str(), (DIMENSIONE_MAX+1)*sizeof(Data));
  }

  //replacement for system("Pause");
  //std::cin.sync();
  //std::cin.ignore();

  CreateMyWindow();
  Fl::add_idle(idle_cb, 0);
  Fl::run();
#else
  std::cout << "This program can only work with shared memory enabled" << std::endl;
#endif

  return 0;
}


