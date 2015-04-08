#include "form.h"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400


Fl_Window            *form;
Frame                *scene;
Fl_Value_Slider      *tempo;


int tt = 0;
bool redraw = true;


static void quit_cb(Fl_Widget *w, void *v) {
  exit(0); 
}

void tempo_cb(Fl_Widget*) {
  tt = int(tempo->value()); redraw = true;
}


void CreateMyWindow(void) {

  int w_est, h_est;

  w_est = 23 + SCREEN_WIDTH + 23;   h_est = 23 + SCREEN_HEIGHT + 73;

  form = new Fl_Window(w_est, h_est, "Texa");
  new Fl_Box(FL_DOWN_FRAME, 20, 20, SCREEN_WIDTH + 6, SCREEN_HEIGHT + 6, "");

  scene = new Frame(23, 23, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  tempo = new Fl_Value_Slider(23, 23 + SCREEN_HEIGHT + 30, SCREEN_WIDTH - 53, 30, "");
  tempo->type(FL_HOR_SLIDER);  tempo->bounds(0, 990); tempo->value(tt);
  tempo->callback(tempo_cb);

  form->resizable(scene);

  form->end();
  form->show();
  scene->show();

}

