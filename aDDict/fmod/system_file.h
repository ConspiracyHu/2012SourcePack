/******************************************************************************/
/* SYSTEM_FILE.H                                                              */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#ifndef _SYSTEM_FILE_H_
#define _SYSTEM_FILE_H_

typedef struct tag_FSOUND_FILE_HANDLE 
{
	signed char		type;
	void			*fp;
	signed char		*mem;
	int				basepos;
	unsigned int	userhandle;
	int				length;
} FSOUND_FILE_HANDLE;

FSOUND_FILE_HANDLE *FSOUND_File_Open(void *data, signed char type, int length);
void				FSOUND_File_Close(FSOUND_FILE_HANDLE *handle);
int					FSOUND_File_Read(void *buffer, int size, FSOUND_FILE_HANDLE *handle);
void				FSOUND_File_Seek(FSOUND_FILE_HANDLE *handle, int pos, signed char mode);
int					FSOUND_File_Tell(FSOUND_FILE_HANDLE *handle);


extern unsigned int	(*FSOUND_File_OpenCallback)(char *name);
extern void			(*FSOUND_File_CloseCallback)(unsigned int handle);
extern int			(*FSOUND_File_ReadCallback)(void *buffer, int size, unsigned int handle);
extern void			(*FSOUND_File_SeekCallback)(unsigned int handle, int pos, signed char mode);
extern int			(*FSOUND_File_TellCallback)(unsigned int handle);

#endif