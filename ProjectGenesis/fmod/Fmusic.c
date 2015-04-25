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
#include "system_memory.h"

FMUSIC_MODULE *		FMUSIC_PlayingSong = NULL;
FMUSIC_CHANNEL		FMUSIC_Channel[32];		// channel array for this song
FMUSIC_TIMMEINFO *	FMUSIC_TimeInfo;
FSOUND_SAMPLE		FMUSIC_DummySample;
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
	module->mixer_samplespertick =
		(int)((float)FSOUND_MixRate * (1000.0f / ((float)(module->bpm = bpm) * 2.0f / 5.0f)) / 1000.0f);	
}


//= API FUNCTIONS ==============================================================================

FMUSIC_MODULE	* FMUSIC_LoadSong(
	SAMPLELOADCALLBACK sampleloadcallback,
	NOTELOADCALLBACK noteloadcallback)
{
	FMUSIC_MODULE	*mod;
	mod = FSOUND_Memory_Calloc(sizeof(FMUSIC_MODULE));
	mod->samplecallback = sampleloadcallback;
	mod->notecallback   = noteloadcallback;
	FMUSIC_LoadXM(mod);
	return mod;
}


signed char FMUSIC_FreeSong(){ return 1; }

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
signed char FMUSIC_PlaySong(FMUSIC_MODULE	* mod)
{
	int				count;
	FMUSIC_CHANNEL	*cptr;
	int				totalblocks = FSOUND_BufferSize / FSOUND_BlockSize; 

#ifdef __EXCEPTION_HANDLER__
	if (!mod) 
		return FALSE;
#endif

//	FMUSIC_StopSong(mod);

	mod->globalvolume = mod->defaultglobalvolume;
 	mod->speed = (int)mod->defaultspeed;
	mod->nextorder = -1;
	mod->nextrow = -1;
	mod->row = 0;
	mod->order = 0;
	mod->mixer_samplesleft = 0;
	mod->tick = 0;
	mod->patterndelay = 0;
	mod->time_ms = 0;

//	if (mod->Play)		mod->Play(mod);

	FMUSIC_SetBPM(mod,mod->defaultbpm);

	memset(FMUSIC_Channel, 0, mod->numchannels * sizeof(FMUSIC_CHANNEL));

	for (count=0; count<mod->numchannels; count++)
	{
		cptr = &FMUSIC_Channel[count];
		cptr->cptr = &FSOUND_Channel[count];
	}

	FMUSIC_PlayingSong = mod;

	FMUSIC_TimeInfo = FSOUND_Memory_Calloc(sizeof(FMUSIC_TIMMEINFO) * totalblocks);

	// ========================================================================================================
	// PREPARE THE OUTPUT
	// ========================================================================================================

	{
		WAVEHDR	*wavehdr;
		int	length = FSOUND_BufferSize << 2;

		// CREATE AND START LOOPING WAVEOUT BLOCK
		wavehdr = &FSOUND_MixBlock.wavehdr;

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
#ifdef __EXCEPTION_HANDLER__
	if (!mod) 
		return FALSE;
#endif

#ifdef __EXCEPTION_HANDLER__
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
#endif
	return TRUE;
}

//= INFORMATION FUNCTIONS ======================================================================

int FMUSIC_GetOrder()         {	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].order; }
int FMUSIC_GetRow()           {	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].row; }
unsigned int FMUSIC_GetTime() {	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].ms; }
