#include "Image.hpp"

Image::Image() {
  _data = NULL;
  _filename = "";
  _w = _h = 1;
}

string toupper(string s) {
  string ret = "";
  for (char const* c = s.c_str(); *c; c++)
    ret += toupper(*c);
  return ret;
}

bool Image::load(string filename) {
  string ending;
  char const *tmp = NULL, *s;
  for (s = filename.c_str(); *s; s++)
    if (*s == '.') tmp = s;
  ending = toupper(string(tmp));
  if (tmp) {
    if (ending == ".JPG") return loadJpeg(filename);
    if (ending == ".BMP") return loadBmp(filename);
  }
  return false;
}

struct __attribute__((__packed__)) BmpHeader {
  uint16_t type;
  uint32_t _unused[2];
  uint32_t dataOff;
  uint32_t infoSize;
  int32_t  width;
  int32_t  height;
  uint16_t __unused;
  uint16_t bpp;
  uint32_t compression;
  uint32_t dataSize;
};

bool Image::loadBmp(string filename) {
  struct BmpHeader header;
  
  FILE    *in = fopen(filename.c_str(), "rb");
  GLubyte *ptr;

  fread(&header, sizeof(header), 1, in);

  if (header.type != 0x4D42 /* "BM" */) {
    cout << "not a bitmap (" << filename << ")" << endl;
    //    return false;
  }

  _w = header.width;
  _h = header.height;
  _ratio = (float)_w / (float)_h;

  if (header.bpp != 24) {
    cout << "wrong bitcount: " << header.bpp << ", need 24 (" << filename << ")" << endl;
    //    return false;
  }
  if (header.compression != 0) {
    cout << "can't handle compressed bitmaps (" << filename << ")" << endl;
    //    return false;
  }

  if (_data) delete [] _data;
  fseek(in, header.dataOff, SEEK_SET);
  ptr = _data = new GLubyte [_w * _h * 3 + 4];

  for (GLuint i = 0; i < _h; i++) {
    fread(ptr, 1, _w * 3, in);
    ptr += _w * 3;
    //fread(ptr, 1, (-_w * 3) % 4, in);
  }

  fclose(in);

  _filename = filename;
  return true;
}

bool Image::loadJpeg(string filename) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  GLubyte *row_pointer, *tmp;
  FILE *infile = fopen( filename.c_str(), "rb" );

  delete [] _data;

  if (not infile) {
    // file not loaded
    cerr << "Error opening jpeg file: " << filename << endl;
    return false;
  }

  cinfo.err = jpeg_std_error( &jerr );
  jpeg_create_decompress( &cinfo );
  jpeg_stdio_src( &cinfo, infile );
  jpeg_read_header( &cinfo, TRUE );

  _w = cinfo.image_width;
  _h = cinfo.image_height;

  jpeg_start_decompress( &cinfo );

  _data = new GLubyte [_w * _h * 3];
  row_pointer = new GLubyte [_w * 3];

  tmp = _data;
  for(int i = _h - 1; i >= 0; i--) {
    tmp = _data + i * _w * 3;
    jpeg_read_scanlines(&cinfo, &tmp, 1);
  }

  jpeg_finish_decompress( &cinfo );
  jpeg_destroy_decompress( &cinfo );
  delete [] row_pointer;
  fclose(infile);

  _ratio = (double)_w / (double)_h;
  _filename = filename;

  return true;
}

void Image::draw(int w, int h) {
  if (not _data) return;
  float x = 0, y = 0, zoom = 1.0, ratio = (float)w / (float)h;
  
  if (ratio < _ratio) {
    // zoom image to fit the screens width
    zoom = w / (float)_w;
    y = (h - _h * zoom) / 2.0;
  } else {
    // zoom image to fit the screens height
    zoom = h / (float)_h;
    x = (w - _w * zoom) / 2.0;
  }
    
  glRasterPos2i(x, y);
  glPixelZoom(zoom, zoom);
  glDrawPixels(_w, _h, GL_RGB, GL_UNSIGNED_BYTE, _data); 

}
