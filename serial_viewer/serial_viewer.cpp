#define _CRT_SECURE_NO_WARNINGS


#include "draw.h"
#include "frame.h"
#include "form.h"
#include <FL/Fl.H>

using namespace boost::algorithm;




#define R2D 57.29578
FILE* fp_log;
int NDAT = 1000;


Data *dataM; int indiceDataM;
Data *dataT; int indiceDataT;
Data *dataV; int indiceDataV;



extern Frame *scene;

//----------------------------------------------------------------------------------------
void idle_cb(void*) { scene->redraw(); }
//----------------------------------------------------------------
int main(int argc, char **argv) {


  dataM = (Data*)get_host_allocated_memory("M_DATA");
  dataT = (Data*)get_host_allocated_memory("T_DATA");
  dataV = (Data*)get_host_allocated_memory("V_DATA");


  indiceDataM = int(dataM[NDAT].d[0] + 0.333);
  indiceDataT = int(dataT[NDAT].d[0] + 0.333);
  indiceDataV = int(dataV[NDAT].d[0] + 0.333);

  std::cout << indiceDataM << std::endl;
  std::cout << indiceDataM << std::endl;
  std::cout << indiceDataM << std::endl;

  //replacement for system("Pause");
  std::cin.sync();
  std::cin.ignore();


  CreateMyWindow();
  Fl::add_idle(idle_cb, 0);
  Fl::run();
  return 0;
}
//----------------------------------------------------------------
