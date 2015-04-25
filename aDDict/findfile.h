#ifndef __findfile__
#define __findfile__

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>

struct tfilelist {
				WIN32_FIND_DATA filedata;
				tfilelist *next;
			};

tfilelist *findfile(const char *path);
int filenum(tfilelist *list);

#endif