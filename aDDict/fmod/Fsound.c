/******************************************************************************/
/* FSOUND.C                                                                   */
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
#include "mixer.h"
#include "mixer_clipcopy.h"
#include "mixer_fpu_ramp.h"
#include "music.h"
#include "sound.h"
#include "system_file.h"
#include "system_memory.h"

#include <windows.h>
#include <mmsystem.h>

//= GLOBALS ================================================================================

FSOUND_CHANNEL		FSOUND_Channel[256];		// channel pool 
int					FSOUND_MixRate;				// mixing rate in hz.
int					FSOUND_BufferSizeMs = 1000;
HWAVEOUT			FSOUND_WaveOutHandle;
FSOUND_SoundBlock	FSOUND_MixBlock;

// mixing info
signed char		*	FSOUND_MixBufferMem;		// mix buffer memory block
signed char		*	FSOUND_MixBuffer;			// mix output buffer (16bit or 32bit)
float				FSOUND_OOMixRate;			// mixing rate in hz.
int					FSOUND_BufferSize;			// size of 1 'latency' ms buffer in bytes
int					FSOUND_BlockSize;			// LATENCY ms worth of samples

// thread control variables
volatile signed char	FSOUND_Software_Exit			= FALSE;		// mixing thread termination flag
volatile signed char	FSOUND_Software_UpdateMutex		= FALSE;
volatile signed char	FSOUND_Software_ThreadFinished	= TRUE;
volatile HANDLE			FSOUND_Software_hThread			= NULL;
volatile int			FSOUND_Software_FillBlock		= 0;
volatile int			FSOUND_Software_RealBlock		= 0;



/*
[API]
[
	[DESCRIPTION]
	Initializes the FMOD Sound System.

	[PARAMETERS]
	'mixrate'		Output rate in hz.  
	'vcmmode'		This is reserved for future expansion.  Set it to 0.
	 
	[RETURN_VALUE]
	On success, TRUE is returned.
	On failure, FALSE is returned.

	[REMARKS]
	
	[SEE_ALSO]
	FSOUND_Init
	FSOUND_Close
]
*/
signed char FSOUND_Init(int mixrate, int vcmmode)
{
	int	count;

	if (!FSOUND_File_OpenCallback || !FSOUND_File_CloseCallback || !FSOUND_File_ReadCallback || !FSOUND_File_SeekCallback || !FSOUND_File_TellCallback)
		return FALSE;

	//=======================================================================================
	// CLOSE IT DOWN FIRST IF IT IS ALREADY OPEN
	//=======================================================================================
	FSOUND_Close();

	//=======================================================================================
	// SET SOME DEFAULTS
	//=======================================================================================
	FSOUND_MixRate			= mixrate;

	{
		WAVEFORMATEX	pcmwf;
		UINT			hr;

		// ========================================================================================================
		// INITIALIZE WAVEOUT
		// ========================================================================================================
		pcmwf.wFormatTag		= WAVE_FORMAT_PCM; 
		pcmwf.nChannels			= 2;
		pcmwf.wBitsPerSample	= 16; 
		pcmwf.nBlockAlign		= pcmwf.nChannels * pcmwf.wBitsPerSample / 8;
		pcmwf.nSamplesPerSec	= mixrate; 
		pcmwf.nAvgBytesPerSec	= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign; 
		pcmwf.cbSize			= 0;

		hr = waveOutOpen(&FSOUND_WaveOutHandle, WAVE_MAPPER, &pcmwf, 0, 0, 0);

		if (hr) 
			return FALSE;

	}

	// ========================================================================================================
	// INITIALIZE SOFTWARE MIXER 
	// ========================================================================================================

	FSOUND_OOMixRate	= 1.0f / (float)mixrate;

	FSOUND_Software_FillBlock=FSOUND_Software_RealBlock=0;

	FSOUND_BlockSize = ((FSOUND_MixRate * FSOUND_LATENCY / 1000) + 3) & 0xFFFFFFFC;	// Number of *samples*
	FSOUND_BufferSize = FSOUND_BlockSize * (FSOUND_BufferSizeMs / FSOUND_LATENCY);	// make it perfectly divisible by granularity
	FSOUND_BufferSize <<= 1;	// double buffer

	mix_volumerampsteps = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
	mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;

	//=======================================================================================
	// ALLOC GLOBAL CHANNEL POOL
	//=======================================================================================
	memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);

	// ========================================================================================================
	// SET UP CHANNELS
	// ========================================================================================================

	for (count=0; count < 256; count++)
	{
		FSOUND_Channel[count].index = count;
		FSOUND_Channel[count].speedhi = 1;
	}

	return TRUE;
}


/*
[API]
[
	[DESCRIPTION]
	Shuts down the WHOLE FMOD Sound System.

	[PARAMETERS]
 
	[RETURN_VALUE]
	void

	[REMARKS]
	This also closes down the sample management system, freeing all MANAGED samples loaded (unless they	were allocated with the FSOUND_UNMANAGED flag).
	All songs (mods) are stopped (but does not free them).
	All streams are stopped and freed.
	CD Tracks are stopped.

	[SEE_ALSO]
	FSOUND_Init
]
*/
void FSOUND_Close()
{
	// ========================================================================================================
	// SHUT DOWN OUTPUT DRIVER 
	// ========================================================================================================
	FSOUND_SetPause(0);
	waveOutReset(FSOUND_WaveOutHandle);
	waveOutClose(FSOUND_WaveOutHandle);
}




/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
void FSOUND_Software_Fill()
{
	void  *	mixbuffer; 
	int		mixpos		= FSOUND_Software_FillBlock * FSOUND_BlockSize;
	int		totalblocks = FSOUND_BufferSize / FSOUND_BlockSize; 


	mixbuffer = (char *)FSOUND_MixBuffer + (mixpos << 3);

	//==============================================================================
	// MIXBUFFER CLEAR
	//==============================================================================
	memset(mixbuffer, 0, FSOUND_BlockSize << 3);

	//==============================================================================
	// UPDATE MUSIC
	//==============================================================================

	{
		int MixedSoFar = 0;
		int MixedLeft = FMUSIC_PlayingSong->mixer_samplesleft;
		int SamplesToMix;
		signed char *MixPtr;

		// keep resetting the mix pointer to the beginning of this portion of the ring buffer
		MixPtr = mixbuffer;

		while (MixedSoFar < FSOUND_BlockSize)
		{
			if (!MixedLeft)
			{
				FMUSIC_PlayingSong->Update(FMUSIC_PlayingSong);		// update new mod tick
				SamplesToMix = FMUSIC_PlayingSong->mixer_samplespertick;
				MixedLeft = SamplesToMix;
			}
			else SamplesToMix = MixedLeft;

			if (MixedSoFar + SamplesToMix > FSOUND_BlockSize) 
				SamplesToMix = FSOUND_BlockSize - MixedSoFar;

			FSOUND_Mixer_FPU_Ramp(MixPtr, SamplesToMix, FALSE); 

			MixedSoFar	+= SamplesToMix;
			MixPtr		+= (SamplesToMix << 3);
			MixedLeft	-= SamplesToMix;

			FMUSIC_PlayingSong->time_ms += (int)(((float)SamplesToMix * FSOUND_OOMixRate) * 1000);
		}

		FMUSIC_TimeInfo[FSOUND_Software_FillBlock].ms = FMUSIC_PlayingSong->time_ms;
		FMUSIC_TimeInfo[FSOUND_Software_FillBlock].row = FMUSIC_PlayingSong->row;
		FMUSIC_TimeInfo[FSOUND_Software_FillBlock].order = FMUSIC_PlayingSong->order;
		
		FMUSIC_PlayingSong->mixer_samplesleft = MixedLeft;
	}


	// ====================================================================================
	// CLIP AND COPY BLOCK TO OUTPUT BUFFER
	// ====================================================================================
	{
		void  			*ptr1, *ptr2; 
		unsigned int	len1, len2;

		ptr1 = FSOUND_MixBlock.data + (mixpos << 2);
		ptr2 = NULL;
		len1 = FSOUND_BlockSize << 2;
		len2 = 0;

		FSOUND_MixerClipCopy_Float32(ptr1, mixbuffer, len1>>2); 
	}

	FSOUND_Software_FillBlock++;
	if (FSOUND_Software_FillBlock >= totalblocks)
		FSOUND_Software_FillBlock = 0;
}


/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
DWORD FSOUND_Software_DoubleBufferThread(LPDWORD lpdwParam)
{
	int totalblocks; 

	FSOUND_Software_ThreadFinished = FALSE;

	totalblocks = FSOUND_BufferSize / FSOUND_BlockSize; 

	while (!FSOUND_Software_Exit)
	{
		int		cursorpos,cursorblock,prevblock;
		MMTIME	mmt;

		while (FMUSIC_IsPaused && !FSOUND_Software_Exit) { Sleep(10); }

		mmt.wType = TIME_BYTES;
		waveOutGetPosition(FSOUND_WaveOutHandle, &mmt, sizeof(MMTIME));
		mmt.u.cb >>= 2;
		cursorpos = mmt.u.cb;

		cursorpos %= FSOUND_BufferSize;
		cursorblock = cursorpos / FSOUND_BlockSize;

		prevblock = cursorblock - 1;
		if (prevblock < 0)
			prevblock = totalblocks - 1;

		while (FSOUND_Software_FillBlock != cursorblock)
		{
			FSOUND_Software_UpdateMutex = TRUE;

			FSOUND_Software_Fill();
	
			FSOUND_Software_RealBlock++;
			if (FSOUND_Software_RealBlock >= totalblocks)
				FSOUND_Software_RealBlock = 0;

			FSOUND_Software_UpdateMutex = FALSE;
		}

		Sleep(5);
	};

	FSOUND_Software_ThreadFinished = TRUE;

	return 0;
}


