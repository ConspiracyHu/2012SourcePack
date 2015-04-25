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

void FSOUND_File_Open();
void FSOUND_File_Close();
int	 FSOUND_File_Read(void *buffer, int size);
void FSOUND_File_Seek(int pos, signed char mode);
int	 FSOUND_File_Tell();


/*
extern unsigned int	(*FSOUND_File_OpenCallback)();
extern void			(*FSOUND_File_CloseCallback)();
*/
extern int			(*FSOUND_File_ReadCallback)(void *buffer, int size);
extern void			(*FSOUND_File_SeekCallback)(int pos, signed char mode);
extern int			(*FSOUND_File_TellCallback)();

#endif