#define _CRT_SECURE_NO_WARNINGS

#include "draw.h"

#define XY_AXIS   4

extern int NDAT;

extern Data *dataM; extern int indiceDataM;
extern Data *dataT; extern int indiceDataT;
extern Data *dataV; extern int indiceDataV;


extern int tt;
bool redraw = true;


// ********************************************************************************************************
void draw_init(void){ }
//----------------------------------------------------------------------------------------
void drawTextMain(double x, double y)
{
  static char messaggio[100] = { 0 };
  gl_color(FL_WHITE);
  glDisable(GL_DEPTH_TEST);
  sprintf(messaggio, "Ciao %d %d", indiceDataM, tt);
  gl_draw(messaggio, float(x + 1.0), float(y - 0.0));

  glEnable(GL_DEPTH_TEST);
  gl_color(FL_WHITE);
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void drawAcc(){
  glColor3d(1.0, 1.0, 0.0); glLineWidth(1.0);
  fprintf(stdout, " %d %d\n", indiceDataM, tt);
  glPushMatrix();

  glColor3d(1.0, 0.0, 0.0);
  for (int k = 0; k < 5; k++) {
    glBegin(GL_LINES); glVertex3d(k, -2.0, 0.0); glVertex3d(k, +2.0, 0.0); glEnd();
  }
  for (int k = 0; k < 5; k++) {
    glBegin(GL_LINES); glVertex3d(0.0, -2.0 + k, 0.0); glVertex3d(5.0, -2.0 + k, 0.0); glEnd();
  }


  double dt = 5.0 / (NDAT - tt);
  glColor3d(1.0, 0.0, 0.0);
  glBegin(GL_LINE_STRIP);
  for (int k = tt; k < NDAT; k++) glVertex3d((k - tt)*dt, dataT[(k + indiceDataT + 1) % NDAT].d[3], 0.1);// asse x
  glEnd();
  //glColor3d(0.0,1.0,0.0); 
  //glBegin(GL_LINE_STRIP);
  //for (int k = tt; k < NDAT; k++) glVertex3d((k-tt)*dt, dataT[(k+indiceDataT+1)%NDAT].d[2], 0.1);// asse y
  //glEnd();
  glColor3d(0.0, 1.0, 0.0);
  glBegin(GL_LINE_STRIP);
  for (int k = tt; k < NDAT; k++) glVertex3d((k - tt)*dt, dataM[(k + indiceDataM + 1) % NDAT].d[3], 0.1);   // x
  glEnd();
  //glColor3d(0.0,1.0,0.0); 
  //glBegin(GL_LINE_STRIP);
  //for (int k = tt; k < NDAT; k++) glVertex3d((k-tt)*dt, dataM[(k+indiceDataM+1)%NDAT].d[4], 0.1);   // y
  //   glEnd();
  glColor3d(1.0, 1.0, 0.0);
  glBegin(GL_LINE_STRIP);
  for (int k = tt; k < NDAT; k++) glVertex3d((k - tt)*dt, dataV[(k + indiceDataV + 1) % NDAT].d[0], 0.1);   // x
  glEnd();
  //glColor3d(0.0,1.0,0.0); 
  //glBegin(GL_LINE_STRIP);
  //for (int k = tt; k < NDAT; k++) glVertex3d((k-tt)*dt, dataV[(k+indiceDataV+1)%NDAT].d[1], 0.1);   // y
  //   glEnd();





  glPopMatrix();
  glColor3d(1.0, 1.0, 1.0); glLineWidth(1.0);
}
//----------------------------------------------------------------------------------------
void draw_scene(void){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  indiceDataM = int(dataM[NDAT].d[0] + 0.333);
  indiceDataT = int(dataT[NDAT].d[0] + 0.333);
  indiceDataV = int(dataV[NDAT].d[0] + 0.333);
  glPushMatrix();
  //glScaled(0.8, 0.8, 1.0);

  //drawTextMain( 0.0, -0.0);

  drawAcc();

  glPopMatrix();

}
// ********************************************************************************************************

