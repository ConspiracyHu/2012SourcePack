/******************************************************************************/
/* FMUSIC.C                                                                   */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include "minifmod.h"

#include "music_formatxm.h"
#include "sound.h"
#include "system_file.h"
#include "system_memory.h"

FMUSIC_MODULE *		FMUSIC_PlayingSong = NULL;
FMUSIC_CHANNEL		FMUSIC_Channel[32];		// channel array for this song
FMUSIC_TIMMEINFO *	FMUSIC_TimeInfo;
FSOUND_SAMPLE		FMUSIC_DummySample;
// initialization taken out due to size.. should be ok.
/* =
{
//	0,								// index
	NULL,							// buff

	0,								// length
	0,								// loopstart
	0,								// looplen
	0,								// default volume
	0,								// finetune value from -128 to 127 

	44100,							// default frequency
	128,							// default pan

	8,								// bits
	FSOUND_LOOP_OFF,				// loop mode


	TRUE,							// music owned
	0,								// sample global volume (scalar)
	0,								// relative note 
	8363,							// finetuning adjustment to make for music samples.. relative to 8363hz
	0,								// sample loop start
	0,								// sample loop length
	0,								// vibrato speed 0-64 
	0,								// vibrato depth 0-64 
	0,								// vibrato type 0=sine, 1=rampdown, 2=square, 3=random
	0,								// vibrato rate 0-64 (like sweep?) 
};
*/

FSOUND_CHANNEL			FMUSIC_DummyChannel;
FMUSIC_INSTRUMENT		FMUSIC_DummyInstrument;

//= PRIVATE FUNCTIONS ==============================================================================


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/

void FMUSIC_SetBPM(FMUSIC_MODULE *module, int bpm)
{
	float hz;

	module->bpm = bpm;

	hz = (float)bpm * 2.0f / 5.0f;
	
	// number of samples
	module->mixer_samplespertick = (int)((float)FSOUND_MixRate * (1000.0f / hz) / 1000.0f);	
}


//= API FUNCTIONS ==============================================================================

/*
[API]
[
	[DESCRIPTION]
	To load a module with a given filename.  FMUSIC Supports loading of 
	- .MOD (protracker/fasttracker modules)
	- .S3M (screamtracker 3 modules)
	- .XM  (fasttracker 2 modules)
	- .IT  (impulse tracker modules)

	[PARAMETERS]
	'name'		Filename of module to load.
 
	[RETURN_VALUE]
	On success, a pointer to a FMUSIC_MODULE handle is returned.
	On failure, NULL is returned.

	[REMARKS]
	This function autodetects the format type, regardless of filename.

	[SEE_ALSO]
	FMUSIC_FreeSong
]
*/
FMUSIC_MODULE * FMUSIC_LoadSong(signed char *name, SAMPLELOADCALLBACK sampleloadcallback)
{
	FMUSIC_MODULE		*mod;
	signed char			retcode=FALSE;
	FSOUND_FILE_HANDLE	*fp;

	// create a mod instance
	mod = FSOUND_Memory_Calloc(sizeof(FMUSIC_MODULE));

	fp = FSOUND_File_Open(name, 0, 0);
	if (!fp)
		return NULL;

	mod->samplecallback = sampleloadcallback;

	// try opening all as all formats until correct loader is found
	retcode = FMUSIC_LoadXM(mod, fp);

	FSOUND_File_Close(fp);

	if (!retcode)
	{
		FMUSIC_FreeSong(mod);
		return NULL;
	}

	return mod;
}


/*
[API]
[
	[DESCRIPTION]
	Frees memory allocated for a song and removes it from the FMUSIC system.

	[PARAMETERS]
	'mod'		Pointer to the song to be freed.
 
	[RETURN_VALUE]
	void

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_LoadSong
]
*/
signed char FMUSIC_FreeSong(FMUSIC_MODULE *mod)
{
	int count;

	if (!mod) 
		return FALSE;

	BLOCK_ON_SOFTWAREUPDATE();

	FMUSIC_StopSong(mod);

	// free samples
	if (mod->instrument)
	{
		for (count=0; count<(int)mod->numinsts; count++) 
		{
			int count2;

			FMUSIC_INSTRUMENT	*iptr = &mod->instrument[count];
			for (count2=0; count2<iptr->numsamples; count2++) 
			{
				if (iptr->sample[count2])
				{
					FSOUND_SAMPLE *sptr = iptr->sample[count2];
					FSOUND_Memory_Free(sptr->buff);
					FSOUND_Memory_Free(sptr);
				}
			}


		}
	}

	// free instruments
	if (mod->instrument)
		FSOUND_Memory_Free(mod->instrument);

	// free patterns
	if (mod->pattern)
	{
		for (count=0; count<mod->numpatternsmem; count++)
			if (mod->pattern[count].data) 
				FSOUND_Memory_Free(mod->pattern[count].data);

		if (mod->pattern) 
			FSOUND_Memory_Free(mod->pattern);
	}

	// free song
	FSOUND_Memory_Free(mod);

	return TRUE;
}



/*
[API]
[
	[DESCRIPTION]
	Starts a song playing.

	[PARAMETERS]
	'mod'		Pointer to the song to be played.
 
	[RETURN_VALUE]
	TRUE		song succeeded playing
	FALSE		song failed playing

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_StopSong
]
*/
signed char FMUSIC_PlaySong(FMUSIC_MODULE *mod, int startorder)
{
	int				count;
	FMUSIC_CHANNEL	*cptr;
	int				totalblocks = FSOUND_BufferSize / FSOUND_BlockSize; 

	if (!mod) 
		return FALSE;

//	if (FMUSIC_PlayingSong)	FMUSIC_StopSong(mod);

	mod->globalvolume = mod->defaultglobalvolume;
 	mod->speed = (int)mod->defaultspeed;
	mod->row = 0;
	mod->order = startorder;
	mod->nextorder = -1;
	mod->nextrow = -1;
	mod->mixer_samplesleft = 0;
	mod->tick = 0;
	mod->patterndelay = 0;
	mod->time_ms = 0;

	if (mod->Play)
		mod->Play(mod);

	FMUSIC_SetBPM(mod, mod->defaultbpm);

	memset(FMUSIC_Channel, 0, mod->numchannels * sizeof(FMUSIC_CHANNEL));

	for (count=0; count<mod->numchannels; count++)
	{
		cptr = &FMUSIC_Channel[count];
		cptr->cptr = &FSOUND_Channel[count];

		// music system channel initialization
/*
		// removed for size .. should be ok ..

		cptr->volume		= 0;
		cptr->voldelta		= 0;
		cptr->inst			= 0;
		cptr->note			= 0;
		cptr->period		= 0;
		cptr->portaupdown	= 0;
		cptr->volslide		= 0;
		cptr->panslide		= 0;
		cptr->portatarget	= 0;
		cptr->portaspeed	= 0;
		cptr->portareached  = FALSE;
		cptr->sampleoffset	= 0;
		cptr->retrigx		= 0;
		cptr->retrigy		= 0;
		cptr->retrigcount	= 0;
		cptr->vibpos		= 0;
		cptr->vibspeed		= 0;
		cptr->vibdepth		= 0;
		cptr->tremolopos	= 0;
		cptr->tremolospeed	= 0;
		cptr->tremolodepth	= 0;
		cptr->panbrellopos	= 0;
		cptr->panbrellospeed = 0;
		cptr->panbrellodepth = 0;
		cptr->patlooprow	= 0;
		cptr->patloopno		= 0;
		cptr->wavecontrol	= 0;
		cptr->wavecontrolvib = 0;
		cptr->wavecontroltrem = 0;
		cptr->wavecontrolpan = 0;
		cptr->tremorpos		= 0;
		cptr->arpeggio		= 0;
		cptr->channelvsl	= 0;
		cptr->recenteffect  = 0;
		cptr->highoffset	= 0;
		cptr->volcolumn_volslide = 0;
		cptr->fineportaup	= 0;
		cptr->fineportadown = 0;
*/
	}

	FMUSIC_PlayingSong = mod;

	FMUSIC_TimeInfo = FSOUND_Memory_Calloc(sizeof(FMUSIC_TIMMEINFO) * totalblocks);

	// ========================================================================================================
	// PREPARE THE OUTPUT
	// ========================================================================================================

	{
		WAVEHDR	*wavehdr;
		int	length = 0;

		// CREATE AND START LOOPING WAVEOUT BLOCK
		wavehdr = &FSOUND_MixBlock.wavehdr;

		length = FSOUND_BufferSize;
		length <<= 2;	// 16bits

		FSOUND_MixBlock.data = FSOUND_Memory_Calloc(length);
		
		wavehdr->dwFlags			= WHDR_BEGINLOOP | WHDR_ENDLOOP;
		wavehdr->lpData				= (LPSTR)FSOUND_MixBlock.data;
		wavehdr->dwBufferLength		= length;
		wavehdr->dwBytesRecorded	= 0;
		wavehdr->dwUser				= 0;
		wavehdr->dwLoops			= -1;
		waveOutPrepareHeader(FSOUND_WaveOutHandle, wavehdr, sizeof(WAVEHDR));

	}

	// ========================================================================================================
	// ALLOCATE MIXBUFFER
	// ========================================================================================================
	FSOUND_MixBufferMem = (signed char *)FSOUND_Memory_Calloc((FSOUND_BufferSize << 3) + 256);
	FSOUND_MixBuffer = (signed char *)(((unsigned int)FSOUND_MixBufferMem + 15) & 0xFFFFFFF0);

	// ========================================================================================================
	// PREFILL THE MIXER BUFFER 
	// ========================================================================================================

	FSOUND_Software_FillBlock = 0;

	do 
	{
		FSOUND_Software_Fill();
	} while (FSOUND_Software_FillBlock);

	// ========================================================================================================
	// START THE OUTPUT
	// ========================================================================================================

	waveOutWrite(FSOUND_WaveOutHandle, &FSOUND_MixBlock.wavehdr, sizeof(WAVEHDR));

	{
		DWORD	FSOUND_dwThreadId;

		// ========================================================================================================
		// CREATE THREADS / TIMERS (last)
		// ========================================================================================================
		FSOUND_Software_Exit = FALSE;

		FSOUND_Software_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FSOUND_Software_DoubleBufferThread, 0,0, &FSOUND_dwThreadId);

		SetThreadPriority(FSOUND_Software_hThread, THREAD_PRIORITY_TIME_CRITICAL);	// THREAD_PRIORITY_HIGHEST);
	}
	return TRUE;
}


/*
[API]
[
	[DESCRIPTION]
	Stops a song from playing.

	[PARAMETERS]
	'mod'		Pointer to the song to be stopped.
 
	[RETURN_VALUE]
	void

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_PlaySong
]
*/
signed char FMUSIC_StopSong(FMUSIC_MODULE *mod)
{
	if (!mod) 
		return FALSE;

	// Kill the thread
	FSOUND_Software_Exit = TRUE;

	// wait until callback has settled down and exited
	BLOCK_ON_SOFTWAREUPDATE();

	if (FSOUND_Software_hThread) 
	{
		while (!FSOUND_Software_ThreadFinished) 
        {
			Sleep(50);
        }
		FSOUND_Software_hThread = NULL;
	}

	// remove the output mixbuffer
	if (FSOUND_MixBufferMem)
    {
		FSOUND_Memory_Free(FSOUND_MixBufferMem);
    }

    if (FSOUND_MixBlock.wavehdr.lpData)
    {
    	waveOutUnprepareHeader(FSOUND_WaveOutHandle, &FSOUND_MixBlock.wavehdr, sizeof(WAVEHDR));
	    FSOUND_MixBlock.wavehdr.dwFlags &= ~WHDR_PREPARED;
    }

	FMUSIC_PlayingSong = NULL;

	if (FMUSIC_TimeInfo)
    {
		FSOUND_Memory_Free(FMUSIC_TimeInfo);
    }

	return TRUE;
}

//= INFORMATION FUNCTIONS ======================================================================

/*
[API]
[
	[DESCRIPTION]
	Returns the song's current order number

	[PARAMETERS]
	'mod'		Pointer to the song to retrieve current order number from.
 
	[RETURN_VALUE]
	The song's current order number.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_GetPattern
]
*/
int FMUSIC_GetOrder(FMUSIC_MODULE *mod)
{
	if (!mod) 
		return 0;

	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].order;
}


/*
[API]
[
	[DESCRIPTION]
	Returns the song's current row number.

	[PARAMETERS]
 	'mod'		Pointer to the song to retrieve current row from.

	[RETURN_VALUE]
	On success, the song's current row number is returned.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
]
*/
int FMUSIC_GetRow(FMUSIC_MODULE *mod)
{
	if (!mod) 
		return 0;

	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].row;
}


/*
[API]
[
	[DESCRIPTION]
	Returns the time in milliseconds since the song was started.  This is useful for
	synchronizing purposes becuase it will be exactly the same every time, and it is 
	reliably retriggered upon starting the song.  Trying to synchronize using other 
	windows timers can lead to varying results, and inexact performance.  This fixes that
	problem by actually using the number of samples sent to the soundcard as a reference.

	[PARAMETERS]
	'mod'		Pointer to the song to get time from.
 
	[RETURN_VALUE]
	On success, the time played in milliseconds is returned.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
]
*/
unsigned int FMUSIC_GetTime(FMUSIC_MODULE *mod)
{
	if (!mod) 
		return 0;

	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].ms;
}

void FSOUND_SetPause(unsigned int pause) {
	if (pause) waveOutPause(FSOUND_WaveOutHandle);
	else waveOutRestart(FSOUND_WaveOutHandle);
	FMUSIC_IsPaused=pause;
}
