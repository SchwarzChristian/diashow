#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "Settings.hpp"

using namespace std;

void Settings::load(string path, Settings* inherit) {
  FILE   *f = fopen((path + "/diashow.conf").c_str(), "rt");
  FILE   *rnd = NULL;
  DIR    *dir;
  dirent *ent;
  char    buf[1024], *c, *key, *val;
  bool    ignore = false, timing = false;

  _path = path;
  _sub = NULL;
  _playlistfile = NULL;

  if (inherit) {
    // inherit settings from another instance
    _loop     = false;
    _order    = inherit->_order == 'l' ? 'a' : inherit->_order;		// inherit playlist doen't make sense
    _blending = inherit->_blending;
    _standing = inherit->_standing;
  } else {
    // nothing to inherit, load defaults
    _loop  = false;
    _order = 'a';
    _blending = 1.0f;
    _standing = 10.0f;
  }

  if (f) {
    // got a config file
    int i = 1;

    cout << "reading config: " << path + "/diashow.conf" << endl;
    while (fgets(buf, 1023, f)) {
      key = val = NULL;
      for (c = buf; c[0]; c++) {
	// cut off comments
	if (c[0] == '"') ignore = not ignore;
	if (c[0] == '#' and not ignore) break;
      }
      ignore = false;
      c[0] = 0;
      // find key start
      c = buf;
      while (c[0] and isblank(c[0])) c++;
      if (not c[0] or c[0] == '\n') goto eol;			// reached end of line
      if (c[0] == '}') {
	// leave group;
	if (timing) {
	  timing = false;
	} else {
	  cerr << "Settings::load: " << path << "/diashow.conf:" << i << 
	    ": unexpected '}' , skipping ..." << endl;
	}
	key = val = NULL;
	goto eol;
      }
      key = c;
      // find key end
      while (c[0] and not isblank(c[0])) c++;
      if (not c[0] or c[0] == '\n') goto eol;			// reached end of line
      c[0] = 0;
      c++;
      // find value start
      while (c[0] and c[0] != '=' and c[0] != '{') c++;
      if (not c[0] or c[0] == '\n') goto eol;			// reached end of line
      if (c[0] == '{') {
	// enter group
	if (not timing and string(key) == "timing") {
	  timing = true;
	} else {
	  cerr << "Settings::load: " << path << "/diashow.conf:" << i << 
	    ": unexpected '{', skipping ..." << endl;
	}
	key = val = NULL;
	goto eol;
      }
      c++;
      while (c[0] and isblank(c[0])) c++;
      if (not c[0] or c[0] == '\n') goto eol;			// reached end of line
      if (c[0] == '"') {
	ignore = true;
	c++;
      }
      val = c;
      // find value end
      while (c[0] and (ignore ? c[0] != '"' : not isblank(c[0]))) c++;
      c[0] = 0;
    eol:
      if (key and not val) {
	// couldn't understand this line
	cerr << "Settings::load: " << path << "/diashow.conf:" << i << 
	  ": syntax error, skipping ..." << endl;
      } else {
	// syntax ok
	if (key) {
	  // got a key-value-pair
	  if (string(key) == "loop") {
	    _loop  = val[0] == 'y';
	  } else if (string(key) == "order") {
	    if (val[0] == 'a' or val[0] == 'l' or val[0] == 'r')
	      _order = val[0];
	    else
	      cerr << "Settings::load: " << path << "/diashow.conf:" << i << 
		": unknown order: " << val << ", skipping ..." << endl;
	  } else if (string(key) == "orderfile") {
	    if (not (_playlistfile = fopen((path + "/" + string(val)).c_str(), "rt"))) {
	      cerr << "Settings::load: " << path << "/diashow.conf:" << i << 
		": can't load playlist file: " << path << "/" << val << 
		", skipping ..." << endl;
	    }
	  } else if (string(key) == "blending") {
	    _blending = atof(val);
	  } else if (string(key) == "standing") {
	    _standing = atof(val);
	  }
	}
      }
      i++;
    }
    cout << "done." << endl;
  } else cout << "no config found for " << path << "/, using inherits/defaults" << endl;
  
  // generating playlist
  if (_order == 'l') return;						// nothing to do for playlist mode
  if (_order == 'r') rnd = fopen("/dev/random", "rb");			// random mode needs randomness ;)
  dir = opendir(path.c_str());
  while ((ent = readdir(dir))) {
    if (ent->d_name[0] != '.' and (ent->d_type == DT_DIR or ent->d_type == DT_REG or ent->d_type == DT_UNKNOWN)) {
      // this entry looks like we want to play it
      if (rnd) {
	// insert at random position
	fread(buf, 1, 1, rnd);
	for (int i = 0; i < (unsigned char)buf[0]; i++)
	  _playlist++;
	_playlist.insertBehind(new string(ent->d_name));
      } else {
	// insert sorted
	_playlist.insert(new string(ent->d_name));
      }
    }
  }
}

string Settings::check(string filename) {
  filename = _path + "/" + filename;
  int tmp = open(filename.c_str(), O_RDONLY | O_DIRECTORY);
  if (tmp >= 0) {
    // got a directory
    close(tmp);
    _sub = new Settings;
    _sub->load(filename, this);
    return _sub->next();
  }
  return filename;
}

string Settings::next() {
  char buf[1024];

  if (_sub) {
    // we are in a subdirectory
    string ret = _sub->next();
    if (ret == "done.") {
      // subdirectory finished
      delete _sub;
      _sub = NULL;
      return next();
    }
    return ret;
  }

  if (_order == 'l') {
    // we've a playlist file
    if (not _playlistfile) return "done.";	// no filename given
    if (fgets(buf, 1023, _playlistfile))	// got a new filename
      return check(string(buf));
    else if (_loop) {
      // reached eof, looping enabled
      rewind(_playlistfile);
      return next();
    } else return "done.";			// no looping, done.
  } else {
    // we've a playlist in RAM
    string *ptr = _playlist.getFirst();
    if (ptr) {
      // yet another filename
      string ret = *ptr;
      _playlist.remove();
      return check(ret);
    } else {
      // no more filenames
      if (_loop) {
	// looping enabled
	load(_path, NULL);
	return next();
      } else return "done.";
    }
    
  }

  return "done.";
}
