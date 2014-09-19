#ifndef __MEMWATCH_HPP__
#define __MEMWATCH_HPP__

#include <new>

using namespace std;

void* operator new(size_t);
void* operator new[](size_t);
void  operator delete(void*);
void  operator delete[](void*);

void* operator new(size_t, const nothrow_t& tag);
void* operator new[](size_t, const nothrow_t& tag);
void  operator delete(void*, const nothrow_t& tag);
void  operator delete[](void*, const nothrow_t& tag);

long __usedMem();

#endif
