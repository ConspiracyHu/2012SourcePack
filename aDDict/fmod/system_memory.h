/******************************************************************************/
/* SYSTEM_MEMORY.H                                                            */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#ifndef _SYSTEM_MEMORY_H_
#define _SYSTEM_MEMORY_H_

// include your system's header here
#include <stdlib.h>

// redefine here
#define FSOUND_Memory_Free(_ptr)			free(_ptr)
#define FSOUND_Memory_Alloc(_len)			calloc(_len, 1)
#define FSOUND_Memory_Calloc(_len)			calloc(_len, 1)
#define FSOUND_Memory_Realloc(_ptr, _len)	realloc(_ptr, _len)

#endif
