/******************************************************************************/
/* SYSTEM_FILE.C                                                              */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minifmod.h"
#include "system_file.h"
#include "system_memory.h"
#include "sound.h"


int				(*FSOUND_File_ReadCallback)(void *buffer, int size) = NULL;
void			(*FSOUND_File_SeekCallback)(int pos, signed char mode) = NULL;
int				(*FSOUND_File_TellCallback)() = NULL;


void FSOUND_File_SetCallbacks(
	int (*ReadCallback)(void *buffer, int size),
	void (*SeekCallback)(int pos, signed char mode),
	int (*TellCallback)())
{
		FSOUND_File_ReadCallback = ReadCallback;
		FSOUND_File_SeekCallback = SeekCallback;
		FSOUND_File_TellCallback = TellCallback;
}


int FSOUND_File_Read(void *buffer, int size) {
	return FSOUND_File_ReadCallback(buffer, size);
}

void FSOUND_File_Seek(int pos, signed char mode) {
	FSOUND_File_SeekCallback(pos, mode);
}

int FSOUND_File_Tell() {
	return FSOUND_File_TellCallback();
}
