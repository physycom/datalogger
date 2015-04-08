#define _CRT_SECURE_NO_WARNINGS

#include "draw.h"

#define NUMERO_SCATOLETTE 8

extern Data **data; 
extern int *indiceData;
extern int tt;

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
  glColor3d(1.0, 1.0, 0.0); glLineWidth(1.0);
  glPushMatrix();

  glColor3d(1.0, 0.0, 0.0);
  for (int k = 0; k < 5; k++) {
    glBegin(GL_LINES); glVertex3d(k, -2.0, 0.0); glVertex3d(k, +2.0, 0.0); glEnd();
  }
  for (int k = 0; k < 5; k++) {
    glBegin(GL_LINES); glVertex3d(0.0, -2.0 + k, 0.0); glVertex3d(5.0, -2.0 + k, 0.0); glEnd();
  }

  double dt = 5.0 / (DIMENSIONE_MAX - tt);

  for (int i = 0; i < NUMERO_SCATOLETTE; i++) {
    glColor3d(1.0 + i, 0.0 + i, 0.0 + i);
    glBegin(GL_LINE_STRIP);
    for (int k = tt; k < DIMENSIONE_MAX; k++) glVertex3d((k - tt)*dt, data[i][(k + indiceData[i] + 1) % DIMENSIONE_MAX].d[3], 0.1);
    glEnd();
  }

  glPopMatrix();
  glColor3d(1.0, 1.0, 1.0); glLineWidth(1.0);
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


