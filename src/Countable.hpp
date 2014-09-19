#ifndef __COUNTABLE_HPP__
#define __COUNTABLE_HPP__

class Countable {
private:
  static int __count;
public:
   Countable() { __count++; }
  ~Countable() { __count--; }
  static int __getCount() { return __count; }
};

#endif
