#define _CRT_SECURE_NO_WARNINGS

#include "draw.h"

#define NUMERO_SCATOLETTE 8

extern Data **data; 
extern int *indiceData;
extern int tt;

//#include "FL/fl_types.h"
//const Fl_Color FL_BLACK = 56;
//const Fl_Color FL_RED = 88;
//const Fl_Color FL_GREEN = 63;
//const Fl_Color FL_YELLOW = 95;
//const Fl_Color FL_BLUE = 216;
//const Fl_Color FL_MAGENTA = 248;
//const Fl_Color FL_CYAN = 223;
//const Fl_Color FL_DARK_RED = 72;
//const Fl_Color FL_DARK_GREEN = 60;
//const Fl_Color FL_DARK_YELLOW = 76;
//const Fl_Color FL_DARK_BLUE = 136;
//const Fl_Color FL_DARK_MAGENTA = 152;
//const Fl_Color FL_DARK_CYAN = 140;
//const Fl_Color FL_WHITE = 255;
//std::vector<Fl_Color> colornames({ 56, 88, 60, 216, 248, 72, 76, 140});

//GLdouble colorvalues[8][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0 } };
GLdouble colorvalues[8][3] = { { 0.0, 71.0/255.0, 178.0/255.0 }, { 138.0/255.0, 230.0/255.0, 46.0/255.0 }, { 184.0/255.0, 147.0/255.0, 0.0 }, { 138.0/255.0, 46.0/255.0, 230.0/255.0 }, 
                               { 46.0/255.0, 156.0/255.0, 193.0/255.0 }, { 153.0/255.0, 0.0, 0.0 }, { 230.0/255.0, 92.0/255.0, 0.0 }, { 102.0/255.0, 102.0/255.0, 51.0/255.0 } };

void draw_init() {
  return;
}



void drawTextMain(double x, double y) {
  static char messaggio[100] = { 0 };
  gl_color(FL_WHITE);
  glDisable(GL_DEPTH_TEST);
  gl_draw(messaggio, float(x + 1.0), float(y - 0.0));

  glEnable(GL_DEPTH_TEST);
  gl_color(FL_WHITE);
}



void drawAcc() {
  glPushMatrix();
  glColor3d(102.0/255.0, 102.0/255.0, 102.0/255.0); //grey
  glLineWidth(2.0);
  for (int k = 0; k < 5; k++) {
    glBegin(GL_LINES); 
    glVertex3d(k, -5.0, 0.0); 
    glVertex3d(k, +5.0, 0.0); 
    glEnd();
  }
  for (int k = 0; k < 10; k++) {
    glBegin(GL_LINES); 
    glVertex3d(0.0, -5.0 + k, 0.0); 
    glVertex3d(5.0, -5.0 + k, 0.0); 
    glEnd();
  }
  double dt = 5.0 / (DIMENSIONE_MAX - tt);
  for (int i = 0; i < NUMERO_SCATOLETTE; i++) {
    //fl_color(colornames[i]);
    glColor3d(colorvalues[i][0], colorvalues[i][1], colorvalues[i][2]);
    glBegin(GL_LINE_STRIP);
    for (int k = tt; k < DIMENSIONE_MAX; k++) 
      glVertex3d((k - tt)*dt, data[i][(k + indiceData[i] + 1) % DIMENSIONE_MAX].d[3], 0.1);
    glEnd();
  }

  glPopMatrix();
}



void draw_scene(void){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int i = 0; i < NUMERO_SCATOLETTE; i++) {
    indiceData[i] = int(data[i][DIMENSIONE_MAX].d[0] + 0.333);
  }
  glPushMatrix();
  drawAcc();
  glPopMatrix();

}


