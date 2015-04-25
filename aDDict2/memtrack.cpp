#include "memtrack.h"

int MemUsage;

/*#undef new
void * __cdecl operator new(unsigned int size,const char *file,int line)
{
	//return _malloc_dbg(size,_NORMAL_BLOCK,file,line);
	MemUsage+=size;
	return malloc(size);
}
#define new new(__FILE__,__LINE__)*/