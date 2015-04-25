#pragma once

/*#include <malloc.h>

#define _MFC_OVERRIDES_NEW
void * __cdecl operator new(unsigned int,const char *,int);
inline void __cdecl operator delete(void *p, const char *, int)
{ ::operator delete(p); }
#define new new(__FILE__,__LINE__)*/

extern int MemUsage;