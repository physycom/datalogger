#pragma once

#define WIN32_LEAN_AND_MEAN
#include <FL/Fl.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <FL/Fl_Slider.H>
#include <FL/Fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Gl_Window.H>


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


void draw_scene();
void draw_init();
void CreateMyWindow();

class Frame : public Fl_Gl_Window {
  void draw();
  int handle(int);
public:
  Frame(int x, int y, int w, int h, const char *l = 0) : Fl_Gl_Window(x, y, w, h, l) {}
};




//fltkgl.lib; fltk.lib; opengl32.lib; glu32.lib; comctl32.lib;


