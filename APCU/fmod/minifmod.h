/******************************************************************************/
/* MINIFMOD.H                                                                 */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

//==========================================================================================
// MINIFMOD Main header file. Copyright (c), FireLight Multimedia 2000.
// Based on FMOD, copyright (c), FireLight Multimedia 2000.
//==========================================================================================

#ifndef _MINIFMOD_H_
#define _MINIFMOD_H_

//===============================================================================================
//= DEFINITIONS
//===============================================================================================

//#define __EXCEPTION_HANDLER__
//#define __16BIT_SAMPLE_SUPPORT__

// fmod defined types
typedef struct FMUSIC_MODULE	FMUSIC_MODULE;

//===============================================================================================
//= FUNCTION PROTOTYPES
//===============================================================================================

//#include "music.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==================================
// Initialization / Global functions.
// ==================================
typedef void * (*NOTELOADCALLBACK)(int pattern, int row, int channel);
typedef void (*SAMPLELOADCALLBACK)(void *buff, int lenbytes, int numbits, int instno, int sampno);
//typedef void (*FMUSIC_CALLBACK)(FMUSIC_MODULE *mod, unsigned char param);

// this must be called before FSOUND_Init!
/*
void FSOUND_File_SetCallbacks(int  (*ReadCallback)(void *buffer, int size),
                              void (*SeekCallback)(int pos, signed char mode),
                              int	 (*TellCallback)()
															);
*/
int	 (*FSOUND_File_Read)(void *buffer, int size);
void (*FSOUND_File_Seek)(int pos, signed char mode);
int	 (*FSOUND_File_Tell)();

signed char		FSOUND_Init(int mixrate);
void			FSOUND_Close();

// =============================================================================================
// FMUSIC API
// =============================================================================================

// Song management / playback functions.
// =====================================
//FMUSIC_MODULE *mod;

FMUSIC_MODULE	* FMUSIC_LoadSong(SAMPLELOADCALLBACK sampleloadcallback, NOTELOADCALLBACK noteloadcallback);
signed char	FMUSIC_FreeSong();
signed char	FMUSIC_PlaySong(FMUSIC_MODULE	* mod);
signed char	FMUSIC_StopSong();

// Runtime song information.
// =========================
int				FMUSIC_GetOrder();
int				FMUSIC_GetRow();
unsigned int	FMUSIC_GetTime();
  
#ifdef __cplusplus
}
#endif

#endif
