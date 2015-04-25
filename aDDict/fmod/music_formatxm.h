/******************************************************************************/
/* FORMATXM.H                                                                 */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#ifndef _FMUSIC_FORMATXM
#define _FMUSIC_FORMATXM

#include "music.h"
#include "system_file.h"


enum FMUSIC_XMCOMMANDS
{
	FMUSIC_XM_ARPEGGIO,
	FMUSIC_XM_PORTAUP,
	FMUSIC_XM_PORTADOWN,
	FMUSIC_XM_PORTATO,
	FMUSIC_XM_VIBRATO,
	FMUSIC_XM_PORTATOVOLSLIDE,
	FMUSIC_XM_VIBRATOVOLSLIDE,
	FMUSIC_XM_TREMOLO,
	FMUSIC_XM_SETPANPOSITION,
	FMUSIC_XM_SETSAMPLEOFFSET,
	FMUSIC_XM_VOLUMESLIDE,
	FMUSIC_XM_PATTERNJUMP,
	FMUSIC_XM_SETVOLUME,
	FMUSIC_XM_PATTERNBREAK,
	FMUSIC_XM_SPECIAL,
	FMUSIC_XM_SETSPEED,
	FMUSIC_XM_SETGLOBALVOLUME,
	FMUSIC_XM_GLOBALVOLSLIDE,
	FMUSIC_XM_I,
	FMUSIC_XM_J,
	FMUSIC_XM_KEYOFF,
	FMUSIC_XM_SETENVELOPEPOS,
	FMUSIC_XM_M,
	FMUSIC_XM_N,
	FMUSIC_XM_O,
	FMUSIC_XM_PANSLIDE,
	FMUSIC_XM_Q,
	FMUSIC_XM_MULTIRETRIG,
	FMUSIC_XM_S,
	FMUSIC_XM_TREMOR,
	FMUSIC_XM_U,
	FMUSIC_XM_V,
	FMUSIC_XM_W,
	FMUSIC_XM_EXTRAFINEPORTA,
	FMUSIC_XM_Y,
	FMUSIC_XM_Z,
};


enum FMUSIC_XMCOMMANDSSPECIAL
{
	FMUSIC_XM_SETFILTER,
	FMUSIC_XM_FINEPORTAUP,
	FMUSIC_XM_FINEPORTADOWN,
	FMUSIC_XM_SETGLISSANDO,
	FMUSIC_XM_SETVIBRATOWAVE,
	FMUSIC_XM_SETFINETUNE,
	FMUSIC_XM_PATTERNLOOP,
	FMUSIC_XM_SETTREMOLOWAVE,
	FMUSIC_XM_SETPANPOSITION16,
	FMUSIC_XM_RETRIG,
	FMUSIC_XM_FINEVOLUMESLIDEUP,
	FMUSIC_XM_FINEVOLUMESLIDEDOWN,
	FMUSIC_XM_NOTECUT,
	FMUSIC_XM_NOTEDELAY,
	FMUSIC_XM_PATTERNDELAY,
	FMUSIC_XM_FUNKREPEAT,
};

#define FMUSIC_XMFLAGS_LINEARFREQUENCY		1

typedef struct
{
    unsigned long	instSize;               // instrument size
    signed char		instName[22];           // instrument filename
    unsigned char	instType;               // instrument type (now 0)
    unsigned short  numSamples;             // number of samples in instrument
} FMUSIC_XM_INSTHEADER;

typedef struct
{
    unsigned long	headerSize;             // sample header size
    unsigned char	noteSmpNums[96];        // sample numbers for notes
    unsigned short  volEnvelope[2*12];      // volume envelope points
    unsigned short  panEnvelope[2*12];      // panning envelope points
    unsigned char   numVolPoints;           // number of volume envelope points
    unsigned char   numPanPoints;           // number of panning env. points
    unsigned char   volSustain;             // volume sustain point
    unsigned char   volLoopStart;           // volume loop start point
    unsigned char   volLoopEnd;             // volume loop end point
    unsigned char   panSustain;             // panning sustain point
    unsigned char   panLoopStart;           // panning loop start point
    unsigned char   panLoopEnd;             // panning loop end point
    unsigned char   volEnvFlags;            // volume envelope flags
    unsigned char   panEnvFlags;            // panning envelope flags

    unsigned char   vibType;                // vibrato type
    unsigned char   vibSweep;               // vibrato sweep
    unsigned char   vibDepth;               // vibrato depth
    unsigned char   vibRate;                // vibrato rate
    unsigned short  volFadeout;             // volume fadeout
    unsigned short  reserved;
} FMUSIC_XM_INSTSAMPLEHEADER;


signed char FMUSIC_LoadXM(FMUSIC_MODULE *mod, FSOUND_FILE_HANDLE *fp);

#endif