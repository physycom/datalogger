// Copyright 2014, 2015 Stefano Sinigardi, Alessandro Fabbri
// for any question, please mail stefano.sinigardi@gmail.com

#pragma once

#include "datalogger.h"

#ifdef __CYGWIN__  
#include <windef.h> 
#endif
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/Fl_Slider.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Gl_Window.H>




void draw_scene();
void draw_init();
void CreateMyWindow();

class Frame : public Fl_Gl_Window {
  void draw();
  int handle(int);
public:
  Frame(int x, int y, int w, int h, const char *l = 0) : Fl_Gl_Window(x, y, w, h, l) {}
};




