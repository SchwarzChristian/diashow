#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <cstdio>
#include <iostream>
#include <string>
#include <GL/glut.h>
#include <jpeglib.h>

using namespace std;

class Image {
private:
  GLubyte      *_data;
  unsigned long _w;
  unsigned long _h;
  float         _ratio;
  string        _filename;
  bool          loadJpeg(string filename);
public:
         Image();
        ~Image() { delete [] _data; };
  bool   load(string filename);
  void   draw(int w, int h);
  string name() { return _filename; };
};

#endif
