
#include "Frame.h"


void Frame::draw() {

  if (!valid()) {
    glClearColor(0.0, 0.0, 0.0, 1);                        // Turn the background color black
    glViewport(0, 0, w(), h());                            // Make our viewport the whole window
    glMatrixMode(GL_PROJECTION);                           // Select The Projection Matrix
    glLoadIdentity();                                      // Reset The Projection Matrix
    gluOrtho2D(0.0, 5.0, -2.0, 2.0);
    glMatrixMode(GL_MODELVIEW);                            // Select The Modelview Matrix
    glLoadIdentity();                                      // Reset The Modelview Matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Clear The Screen And The Depth Buffer
    glLoadIdentity();                                      // Reset The View
    glEnable(GL_DEPTH_TEST);

    draw_init();
    valid(1);
  }
  gl_font(FL_HELVETICA_BOLD, 20);
  draw_scene();
}


int Frame::handle(int evento) {
  return 1;
}
