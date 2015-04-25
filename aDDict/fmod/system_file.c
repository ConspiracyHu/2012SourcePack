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


unsigned int	(*FSOUND_File_OpenCallback)(char *name) = NULL;
void			(*FSOUND_File_CloseCallback)(unsigned int handle) = NULL;
int				(*FSOUND_File_ReadCallback)(void *buffer, int size, unsigned int handle) = NULL;
void			(*FSOUND_File_SeekCallback)(unsigned int handle, int pos, signed char mode) = NULL;
int				(*FSOUND_File_TellCallback)(unsigned int handle) = NULL;


/*
[API]
[
	[DESCRIPTION]
	Specify user callbacks for FMOD's internal file manipulation.
	If ANY of these parameters are NULL, then FMOD will switch back to its own file routines.
	You can replace this with memory routines (ie name can be cast to a memory address for example, then open sets up
	a handle based on this information), or alternate file routines, ie a WAD file reader.

	[PARAMETERS]

	'OpenCallback'		
	Callback for opening a file.  
	Parameters:
	-----------
	"name"	This is the filename.  You may treat this how you like.  
	Instructions:
	-------------
	You MUST open the file and return a handle for future file function calls.  Cast the handle to an unsigned int when returning it, then in later functions, cast it back to your own handle type.
	Return 0 signify an open error.  This is very important.
	It is a good idea if you are using memory based file routines, to store the size of the file here, as it is needed to support SEEK_END in the seek function, described below.

	'CloseCallback'
	Callback for closing a file.
	Parameters:
	-----------
	"handle"	This is the handle you returned from the open callback to use for your own file routines.
	Instructions:
	-------------
	Close your file handle and do any cleanup here.

	'ReadCallback'
	Callback for opening a file.  
	Parameters:
	-----------
	"buffer"	You must read and copy your file data into this pointer.
	"length"    You must read this many bytes from your file data.
	"handle"	This is the handle you returned from the open callback to use for your own file routines.
	Instructions:
	-------------
	You must read "length" number of bytes into the "buffer" provided, then if you need to, increment your file pointer.
	IMPORTANT: you MUST return the number of bytes that were *successfully* read here.  Normally this is just the same as "length", but if you are at the end of the file, you will probably only read successfully the number of bytes up to the end of the file (if you tried to read more than that).

	'SeekCallback'
	Callback for seeking within a file.  There are 3 modes of seeking, as described below.
	Parameters:
	-----------
	"handle"	This is the handle you returned from the open callback to use for your own file routines.
	"pos"		This is the position or offset to seek by depending on the mode.
	"mode"		This is the seek command.  It uses and is compatible with SEEK_SET, SEEK_CUR and SEEK_END from stdio.h, so use them.
	Instructions:
	-------------
	You must reset your file pointer based on the commands given above.
	SEEK_END must reposition your file pointer at the END of the file, plus any negative offset.  To do this you must know the size of the file, it is suggested you find and store this in the open function.  Remember that a SEEK_END position value of -1 is the last byte.

	'TellCallback'
	Callback for returning the offset from the base of the file in BYTES.
	Parameters:
	-----------
	"handle"	This is the handle you returned from the open callback to use for your own file routines.
	Instructions:
	-------------
	You must return the offset from the base of the file, using this routine.

 	[RETURN_VALUE]
	void

	[REMARKS]
	Memory loader FMOD functions are not affected, such as FMUSIC_LoadSongMemory etc.
	WARNING : This function is dangerous in the wrong hands.  You must return the right values, and each
	command must work properly, or FMOD will not function, or it may even crash if you give it invalid data.
	You must support SEEK_SET, SEEK_CUR and SEEK_END properly, or FMOD will not work properly.  See standard I/O help files on how these work under fseek().
	Read the documentation in REMARKS and do exactly what it says.  See the 'simple' example for how it is
	used properly.
]
*/
void FSOUND_File_SetCallbacks(unsigned int (*OpenCallback)(char *name), void	(*CloseCallback)(unsigned int handle),	int (*ReadCallback)(void *buffer, int size, unsigned int handle), void (*SeekCallback)(unsigned int handle, int pos, signed char mode), int (*TellCallback)(unsigned int handle))
{
	if (!OpenCallback || !CloseCallback || !ReadCallback || !SeekCallback || !TellCallback)
	{
		FSOUND_File_OpenCallback = NULL;
		FSOUND_File_CloseCallback = NULL;
		FSOUND_File_ReadCallback = NULL;
		FSOUND_File_SeekCallback = NULL;
		FSOUND_File_TellCallback = NULL;
	}
	else
	{
		FSOUND_File_OpenCallback = OpenCallback;
		FSOUND_File_CloseCallback = CloseCallback;
		FSOUND_File_ReadCallback = ReadCallback;
		FSOUND_File_SeekCallback = SeekCallback;
		FSOUND_File_TellCallback = TellCallback;
	}
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FSOUND_FILE_HANDLE *FSOUND_File_Open(void *data, signed char type, int length)
{
	FSOUND_FILE_HANDLE *handle;

	handle = (FSOUND_FILE_HANDLE *)FSOUND_Memory_Alloc(sizeof(FSOUND_FILE_HANDLE));
	handle->type = type;
	handle->length = length;
	handle->userhandle = FSOUND_File_OpenCallback(data);

	if (!handle->userhandle)
	{
		FSOUND_Memory_Free(handle);
		return NULL;
	}

	return handle;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void FSOUND_File_Close(FSOUND_FILE_HANDLE *handle)
{
	if (!handle) 
		return;

	FSOUND_File_CloseCallback(handle->userhandle);
	FSOUND_Memory_Free(handle);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
int FSOUND_File_Read(void *buffer, int size, FSOUND_FILE_HANDLE *handle)
{
	if (!handle) 
		return 0;

	return FSOUND_File_ReadCallback(buffer, size, handle->userhandle);
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void FSOUND_File_Seek(FSOUND_FILE_HANDLE *handle, int pos, signed char mode)
{
	if (!handle) 
		return;

	FSOUND_File_SeekCallback(handle->userhandle, pos, mode);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
int FSOUND_File_Tell(FSOUND_FILE_HANDLE *handle)
{
	if (!handle) 
		return 0;

	return FSOUND_File_TellCallback(handle->userhandle);
}
