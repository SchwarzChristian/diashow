#include <iostream>
#include <GL/glut.h>
#include <string>
#include <dirent.h>
#include <math.h>

#include "Image.hpp"

#define FPS 30.0

Image *curr, *next;
float a = 0.0;

namespace font {
  unsigned char def[102][9] = {
    #include "font.inc"
  };
}

namespace settings {
  const float standing = 10.0; // sec
  const float blending = 1.0; // sec

  bool renderText = true;
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
    exit(0);
    break;
  }
}

class DirStream {
private:
  DIR    *_dir;
  string  _path;
public:
  DirStream() { _path = "."; rewind(); };
  DirStream(string path) { _path = path; rewind(); };
  dirent*    next() { return readdir(_dir); };
  DirStream& rewind() { _dir = opendir(_path.c_str()); return *this; };
  string     path() { return _path; };
};

void loadImage() {
  static DirStream  **dir      = NULL;
  static unsigned int level    = 0;
  static int          imgFound = 0;

  struct dirent *ent;

  bool success = false;

  if (not dir) {
    // first call
    dir = new DirStream* [1];
    dir[0] = new DirStream;
  }

  while (not success) {
    ent = dir[level]->next();
    if (ent) {
      // one more entry
      if (ent->d_name[0] == '.') continue;
      if (ent->d_type == DT_DIR) {
	// got a directory
	DirStream **tmp = dir;
	level++;
	dir = new DirStream* [level + 1];
	for (unsigned int i = 0; i < level; i++) dir[i] = tmp[i];
	delete [] tmp;
	string test = dir[level-1]->path() + "/" + string(ent->d_name);
	dir[level] = new DirStream(dir[level-1]->path() + "/" + string(ent->d_name));
      } else {
	// got a file
	if (next->load(dir[level]->path() + "/" + string(ent->d_name)))
	  // successfully loaded
	  success = true;
      }
    } else {
      // we're at the end of this dir
      if (level) {
	// we're in a subdir
	delete dir[level];
	level--;
      } else {
	// we're in working directory
	if (imgFound) {
	  // we got something to show
	  imgFound = 0;
	  dir[level]->rewind();
	} else {
	  // nothing to show
	  cerr << "no images found!" << endl;
	  exit(-1);
	}
      }
    }
  }
  imgFound++;
}

void update(int c) {
  if (c < floor(settings::blending * 1000)) {
    // blending
    glutTimerFunc(1000 / FPS, update, c + floor(FPS));
    a = c / 1000.0;
  } else {
    // standing
    glutTimerFunc(1000 * settings::standing, update, 0);
    a = 0.0;
    swap(curr, next);
    loadImage();
  }
  glutPostRedisplay();
}

//Draws the 3D scene
void renderScene() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  glBlendColor(0.0, 0.0, 0.0, 1.0 - a);
  curr->draw(window::w, window::h);
  if (settings::renderText) {
    glRasterPos2i(10, 10);
    glPixelZoom(10.0, 10.0);
    glColor3f(1.0, 1.0, 1.0);
    for (const char *c = curr->name().c_str(); *c; c++)
      if (*c >= 32 and *c < 127)
	glBitmap(5, 9, 0.0, 0.0, 10, 0, font::def[*c - 32]);
  }
  glBlendColor(0.0, 0.0, 0.0, a);
  next->draw(window::w, window::h);
  if (settings::renderText) {
    glRasterPos2i(10, 10);
    glPixelZoom(10.0, 10.0);
    glColor3f(1.0, 1.0, 1.0);
    for (const char *c = next->name().c_str(); *c; c++)
      if (*c >= 32 and *c < 127)
	glBitmap(5, 9, 0.0, 0.0, 10, 0, font::def[*c - 32]);
  }
  
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

  curr = new Image;
  next = new Image;

  loadImage();
  
  update(0);

  glutMainLoop();
  return 0;
}
