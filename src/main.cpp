// c++ includes
#include <iostream>
#include <string>

// c includes
#include <GL/glut.h>
#include <math.h>
#include <time.h>

// own includes
#include "macros.hpp"
#include "Settings.hpp"
#include "Image.hpp"

#ifdef __DEBUG__
#include "Memwatch.hpp"
#endif

#define FPS 30.0

Settings *settings;
Image    *curr, *next;
float     a = 0.0;

namespace font {
  unsigned char def[102][9] = {
    #include "font.inc"
  };
}

namespace key {
  const unsigned char
    skip = ' ',
    quit = 'q'
  ;
}

namespace window {
  int w = 0;
  int h = 0;
}

using namespace std;

void quit(int ret) {
  delete settings;
  delete curr;
  delete next;
  DEBUG(cout << "mem in use: " << __usedMem() << endl);
  exit(ret);
}

void reshape(int w, int h) {
  window::w = w;
  window::h = h;

  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, window::w, 0, window::h);
  glMatrixMode(GL_MODELVIEW);
}

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
  switch (key) {
  case key::skip:
    break;
  case key::quit:
    quit(0);
    break;
  }
}

void loadImage() {
  static bool last = false;
  string tmp = "";

  DEBUG(clock_t t = clock());

  if (last) quit(0);

  swap(curr, next);
  do {
    tmp = settings->next();
    if (tmp == "done.") {
      // no more pictures to load, turn off the lights and go home ;)
      delete next;
      next = new Image;
      last = true;
      break;
    }
  } while (not next->load(tmp));
  a = 0.0;
  DEBUG(t = clock() - t;
	if (tmp != "done.") cout << tmp << " loaded in " << t * 1000 / CLOCKS_PER_SEC << "ms" << endl;
        cout << "mem in use: " << __usedMem() << endl;
  );
}

void update(int c) {
  if (c < floor(settings->blendtime() * 1000)) {
    // blending
    glutTimerFunc(1000 / FPS, update, c + floor(FPS));
    a = c / 1000.0;
  } else {
    // standing
    loadImage();
    glutTimerFunc(1000 * settings->showtime(), update, 0);
  }

  glutPostRedisplay();
}

void renderScene() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  glBlendColor(0.0, 0.0, 0.0, 1.0 - a);
  curr->draw(window::w, window::h);

  glRasterPos2i(10, 10);
  glPixelZoom(10.0, 10.0);
  glColor3f(1.0, 1.0, 1.0);
  for (const char *c = curr->name().c_str(); *c; c++)
    if (*c >= 32 and *c < 127)
      glBitmap(5, 9, 0.0, 0.0, 10, 0, font::def[*c - 32]);

  glBlendColor(0.0, 0.0, 0.0, a);
  next->draw(window::w, window::h);

  glRasterPos2i(10, 10);
  glPixelZoom(10.0, 10.0);
  glColor3f(1.0, 1.0, 1.0);
  for (const char *c = next->name().c_str(); *c; c++)
    if (*c >= 32 and *c < 127)
      glBitmap(5, 9, 0.0, 0.0, 10, 0, font::def[*c - 32]);
  
  glutSwapBuffers();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
  glutCreateWindow ("diashow");

  glEnable(GL_BLEND);
  glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  //set callback functions
  glutDisplayFunc(renderScene);
  glutKeyboardFunc(handleKeypress);
  glutReshapeFunc(reshape);

  settings = new Settings;

  curr = new Image;
  next = new Image;

  settings->load(".");
  loadImage();
  
  update(0);

  glutMainLoop();
  // should not be reached
  quit(0);			// just to be sure
  return 0;
}
