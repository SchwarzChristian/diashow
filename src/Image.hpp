#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <cstdio>
#include <iostream>
#include <string>
#include <GL/glut.h>
#include <jpeglib.h>

#include "macros.hpp"

using namespace std;

class Image {
protected:
  GLubyte      *_data;
  unsigned long _w;
  unsigned long _h;
  float         _ratio;
  string        _filename;
  bool          loadJpeg(string filename);
public:
   Image() {
     _data = NULL;
     _filename = "";
     _w = _h = 1;
   }
  ~Image() { delete [] _data; };
  bool   load(string filename);
  void   draw(int w, int h);
  string name() { return _filename; };
};

#endif
