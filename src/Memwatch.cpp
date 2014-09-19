#include <iostream>

#include <malloc.h>

#include "Memwatch.hpp"

long __allocatedMemory;

void* n(size_t count) throw (bad_alloc) {
  void* tmp = malloc(count);
  if (tmp) {
    __allocatedMemory += malloc_usable_size(tmp);
    return tmp;
  } else throw "no free mem!";
  return NULL;
}

void d(void* ptr) throw () {
  __allocatedMemory -= malloc_usable_size(ptr);
  free(ptr);
}

void* operator new(size_t count) throw (bad_alloc) {
  return n(count);
}

void* operator new[](size_t count) throw (bad_alloc) {
  return n(count);
}

void  operator delete(void* ptr) throw () {
  return d(ptr);
}

void  operator delete[](void* ptr) throw () {
  return d(ptr);
}

void* operator new(size_t count, const nothrow_t& tag) {
  return n(count);
}

void* operator new[](size_t count, const nothrow_t& tag) {
  return n(count);
}

void  operator delete(void* ptr, const nothrow_t& tag) {
  return d(ptr);
}
void  operator delete[](void* ptr, const nothrow_t& tag) {
  return d(ptr);
}

long __usedMem() {
  return __allocatedMemory;
}
