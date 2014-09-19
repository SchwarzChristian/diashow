#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <string>

#include <dirent.h>
#include <stdio.h>
#include <ctype.h>

#include <List.hpp>

using namespace std;

class Settings {
protected:
  bool         _loop;
  char         _order;         // 'a': alphabetic, 'r': random, 'l': list
  List<string> _playlist;
  FILE        *_playlistfile;
  string       _path;
  float        _blending;
  float        _standing;
  Settings    *_sub;

  void   load(string path, Settings* inherit);
  string check(string filename);

public:
         Settings()        { _playlistfile = NULL;  _sub = NULL; }
        ~Settings()        { if (_playlistfile) fclose(_playlistfile); if (_sub) delete _sub; }
  void   load(string path) { load(path, NULL); }
  float  blendtime()       { if (_sub) return _sub->_blending; else return _blending; }
  float  showtime()        { if (_sub) return _sub->_standing; else return _standing; }
  string next();
};


#endif
