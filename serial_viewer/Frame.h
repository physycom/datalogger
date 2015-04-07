#pragma once

#include "draw.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <FL/Fl_Gl_Window.H>

class Frame : public Fl_Gl_Window {
  void draw();
  int handle(int);
public:
  Frame(int x, int y, int w, int h, const char *l = 0) : Fl_Gl_Window(x, y, w, h, l) {}
};


