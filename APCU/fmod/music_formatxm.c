/******************************************************************************/
/* FORMATXM.C                                                                 */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include <string.h>
#include <math.h>

#include "minifmod.h"
#include "mixer.h"
#include "music.h"
#include "music_formatxm.h"
#include "sound.h"

#include "xmeffects.h"

// Frequency = 8363*2^((6*12*16*4 - Period) / (12*16*4));

#define FMUSIC_XMLINEARPERIOD2HZ(_per) ( (int)(8363.0f*pow(2.0f, ((6.0f*12.0f*16.0f*4.0f - _per) / (float)(12*16*4)))) )

/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#if defined(FMUSIC_XM_PORTATO_ACTIVE) || defined(FMUSIC_XM_PORTATOVOLSLIDE_ACTIVE)
void FMUSIC_XM_Portamento(FMUSIC_CHANNEL *cptr)
{
	// slide pitch down if it needs too.
	if (cptr->freq < cptr->portatarget) 
	{
		cptr->freq += (int)cptr->portaspeed << 2;
		if (cptr->freq > cptr->portatarget) 
			cptr->freq = cptr->portatarget;
	}

	// slide pitch up if it needs too.
	else if (cptr->freq > cptr->portatarget) 
	{
		cptr->freq -= (int)cptr->portaspeed << 2;
		if (cptr->freq < cptr->portatarget) 
			cptr->freq=cptr->portatarget;
	}

	 //
	 //if (glissando[track]) 
	 //{
	 //}
	 //

	 cptr->notectrl |= FMUSIC_FREQ;
}
#endif // FMUSIC_XM_PORTATO_ACTIVE


#if defined(FMUSIC_XM_VIBRATO_ACTIVE) || defined(FMUSIC_XM_VIBRATOVOLSLIDE_ACTIVE)
/*
[API]
[
	[DESCRIPTION]
	to carry out a vibrato at a certain depth and speed            

	[PARAMETERS]
	track - the track number to do the vibrato too           
 
	[RETURN_VALUE]

	[REMARKS]
	AND'ing temp with 31 removes the sign bit giving the abs value

	[SEE_ALSO]
]
*/
void FMUSIC_XM_Vibrato(FMUSIC_CHANNEL *cptr) 
{
	int delta;
	unsigned char temp;

	temp = (cptr->vibpos & 31);

	switch (cptr->wavecontrol & 3)
	{
//		case 0: delta = FMUSIC_SineTable[temp];			// sine
		case 0: delta = (int)(fabs ((sin( (float)(cptr->vibpos) * 2 * 3.141592 / 64.0f )) * 256.0f));
				break;
		case 1: temp <<= 3;									// ramp down
				if (cptr->vibpos < 0) 
					temp=255-temp;
				delta=temp;
				break;
		case 2: 
		case 3: delta = 255;//rand()&255;					// random
				break;
		//default:
				//__assume(0);
	};

	delta *= cptr->vibdepth;
	delta >>=7;
	delta <<=2;   // we use 4*periods so make vibrato 4 times bigger

	if (cptr->vibpos >= 0) 
		cptr->freqdelta = -delta;
	else
		cptr->freqdelta = delta;

	cptr->notectrl |= FMUSIC_FREQ;
}
#endif // defined(FMUSIC_XM_VIBRATO_ACTIVE) || defined(FMUSIC_XM_VIBRATOVOLSLIDE_ACTIVE)





#ifdef FMUSIC_XM_INSTRUMENTVIBRATO_ACTIVE

void FMUSIC_XM_InstrumentVibrato(FMUSIC_CHANNEL *cptr, FMUSIC_INSTRUMENT *iptr) 
{
	int delta;

	switch (iptr->VIBtype)
	{
		case 0: delta = (int)(sin( (float)(cptr->ivibpos) * 2.0f * 3.141592f / 256.0f ) * 64.0f);
				//delta = (int)FMUSIC_FineSineTable[cptr->ivibpos];	// sine
				break;
		case 1: if (cptr->ivibpos < 128) 
					delta=64;						// square
				else 
					delta = -64;								
				break;
		case 2: delta = (128-((cptr->ivibpos+128)%256))>>1;
				break;
		case 3: delta = (128-(((256-cptr->ivibpos)+128)%256))>>1;
				break;
		//default:
				//__assume(0);
	};

	delta *= iptr->VIBdepth;
	if (iptr->VIBsweep) 
		delta = delta * cptr->ivibsweeppos / iptr->VIBsweep;
	delta >>=6;

	cptr->freqdelta += delta;
	
	cptr->ivibsweeppos++;
	if (cptr->ivibsweeppos > iptr->VIBsweep) 
		cptr->ivibsweeppos = iptr->VIBsweep;

	cptr->ivibpos += iptr->VIBrate;
	if (cptr->ivibpos > 255) 
		cptr->ivibpos -= 256;

	cptr->notectrl |= FMUSIC_FREQ;
}
#endif	// FMUSIC_XM_INSTRUMENTVIBRATO_ACTIVE


#ifdef FMUSIC_XM_TREMOLO_ACTIVE
void FMUSIC_XM_Tremolo(FMUSIC_CHANNEL *cptr) 
{
	unsigned char temp;

	temp = (cptr->tremolopos & 31);

	switch((cptr->wavecontrol>>4) & 3)
	{
//		case 0: cptr->voldelta = FMUSIC_SineTable[temp];			// sine
		case 0: cptr->voldelta = (int)(fabs ((sin( (float)(cptr->vibpos) * 2 * 3.141592 / 64.0f )) * 256.0f));
				break;
		case 1: temp <<= 3;										// ramp down
				if (cptr->tremolopos < 0) 
					temp=255-temp;
				cptr->voldelta=temp;
				break;
		case 2: cptr->voldelta = 255;							// square
				break;
//		case 3: cptr->voldelta = FMUSIC_SineTable[temp];		// random (just use sine for now)
		case 3: cptr->voldelta = (int)(fabs ((sin( (float)(cptr->vibpos) * 2 * 3.141592 / 64.0f )) * 256.0f));
				break;
	};

	cptr->voldelta *= cptr->tremolodepth;
	cptr->voldelta >>= 6;

	if (cptr->tremolopos >= 0) 
	{
		if (cptr->volume+cptr->voldelta > 64) 
			cptr->voldelta = 64-cptr->volume;
	}
	else 
	{
		if ((short)(cptr->volume-cptr->voldelta) < 0) 
			cptr->voldelta = cptr->volume;
		cptr->voldelta = -cptr->voldelta;
	}

	cptr->tremolopos += cptr->tremolospeed;
	if (cptr->tremolopos > 31) 
		cptr->tremolopos -=64;

	cptr->notectrl |= FMUSIC_VOLUME;
}
#endif // FMUSIC_XM_TREMOLO_ACTIVE


/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#if defined(FMUSIC_XM_VOLUMEENVELOPE_ACTIVE) || defined(FMUSIC_XM_PANENVELOPE_ACTIVE)

void FMUSIC_XM_ProcessEnvelope(FMUSIC_CHANNEL *cptr, int *pos, int *tick, unsigned char type, int numpoints, unsigned short *points, unsigned char loopend, unsigned char loopstart, unsigned char sustain, int *value, int *valfrac, signed char *envstopped, int *envdelta, unsigned char control)
{
	// Envelope
	if (*pos < numpoints) 
	{
		if (*tick == points[(*pos)<<1])	// if we are at the correct tick for the position
		{
			int currpos, nextpos;
			int currtick, nexttick;
			int curr, next, tickdiff;

			// handle loop
			if ((type & FMUSIC_ENVELOPE_LOOP) && *pos == loopend) 
			{
				*pos  = loopstart;
				*tick = points[(*pos) <<1];
			}

			currpos	= *pos;
			nextpos	= (*pos) + 1;
			currtick = points[currpos<<1];				// get tick at this point
			nexttick = points[nextpos<<1];				// get tick at next point
			curr = points[(currpos<<1)+1] << 16;	// get  at this point << 16
			next = points[(nextpos<<1)+1] << 16;	// get  at next point << 16

			// if it is at the last position, abort the envelope and continue last value
			if (*pos == numpoints - 1) 
			{
				*value = points[(currpos<<1)+1];
				*envstopped = TRUE;
				cptr->notectrl |= control;
				return;
			}
			// sustain
			if ((type & FMUSIC_ENVELOPE_SUSTAIN) && currpos == sustain && !cptr->keyoff)
			{
				*value = points[(currpos<<1)+1];
				cptr->notectrl |= control;
				return;
			}
			// interpolate 2 points to find delta step
			tickdiff = nexttick - currtick;
			if (tickdiff) 
				*envdelta = (next-curr) / tickdiff;
			else
				*envdelta = 0;

			*valfrac = curr;

			(*pos)++;
		}
		else
			*valfrac += *envdelta;				// interpolate
	}

	*value = *valfrac >> 16;
	(*tick)++;

	cptr->notectrl |= control;
}
#endif // (FMUSIC_XM_VOLUMEENVELOPE_ACTIVE) || defined(FMUSIC_XM_PANENVELOPE_ACTIVE)


/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#ifdef FMUSIC_XM_VOLUMEBYTE_ACTIVE

void FMUSIC_XM_ProcessVolumeByte(FMUSIC_CHANNEL *cptr, unsigned char volume)
{
	if (volume >= 0x10 && volume <= 0x50) 
	{
		cptr->volume = volume-0x10;
		cptr->notectrl |= FMUSIC_VOLUME;
	}
	else
	{
		switch (volume >> 4) 
		{
			case 0x6 : 
			{
				cptr->volume -= (volume & 0xF);
				if (cptr->volume < 0) 
					cptr->volume = 0;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
			case 0x7 : 
			{
				cptr->volume += (volume & 0xF);
				if (cptr->volume > 0x40) 
					cptr->volume = 0x40;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
			case 0x8 :
			{
				cptr->volume -= (volume & 0xF);
				if (cptr->volume < 0) 
					cptr->volume = 0;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
			case 0x9 :
			{
				cptr->volume += (volume & 0xF);
				if (cptr->volume > 0x40) 
					cptr->volume = 0x40;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
			case 0xa :
			{
				cptr->vibspeed = (volume & 0xF);
				break;
			}
			case 0xb :
			{
				cptr->vibdepth = (volume & 0xF);
				break;
			}
			case 0xc :
			{
				cptr->pan = (volume & 0xF) << 4;
				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
			case 0xd :
			{
				cptr->pan -= (volume & 0xF);
				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
			case 0xe :
			{
				cptr->pan += (volume & 0xF);
				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
			case 0xf :
			{
				if (volume & 0xF) 
					cptr->portaspeed = (volume & 0xF) << 4;
				cptr->portatarget = cptr->period;
				cptr->notectrl &= ~FMUSIC_TRIGGER;
				break;
			}
		//default:
				//__assume(0);
		};
	}
}
#endif // #define FMUSIC_XM_VOLUMEBYTE_ACTIVE

/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#ifdef FMUSIC_XM_AMIGAPERIODS_ACTIVE
int FMUSIC_XM_GetAmigaPeriod(int note, int finetune)
{
	int period;

	period = (int)(pow(2.0f, (float)(132-note) / 12.0f) * 13.375f);

	// interpolate for finer tuning
	if (finetune < 0 && note)
	{
		int diff = period - (int)(pow(2.0f, (float)(132-(note-1)) / 12.0f) * 13.375f);
		diff *= abs(finetune);
		diff /= 128;
		period -= diff;
	}
	else
	{
		int diff = (int)(pow(2.0f, (float)(132-(note+1)) / 12.0f) * 13.375f) - period;
		diff *= finetune;
		diff /= 128;
		period += diff;
	}

	return period;
}
#endif // FMUSIC_XM_AMIGAPERIODS_ACTIVE


/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#ifdef FMUSIC_XM_TREMOR_ACTIVE
void FMUSIC_XM_Tremor(FMUSIC_CHANNEL *cptr)
{
	if (cptr->tremorpos >= cptr->tremoron) 
		cptr->voldelta = -cptr->volume;
	cptr->tremorpos++;
	if (cptr->tremorpos >= (cptr->tremoron + cptr->tremoroff)) 
		cptr->tremorpos = 0;
	cptr->notectrl |= FMUSIC_VOLUME;
}
#endif

/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void FMUSIC_XM_UpdateFlags(FMUSIC_CHANNEL *cptr, FSOUND_SAMPLE *sptr, FMUSIC_MODULE *mod)
{
	FSOUND_CHANNEL *ccptr = cptr->cptr;
	int channel;
    
    if (ccptr)
        channel = ccptr->index;
	
	if (!(cptr->freq + cptr->freqdelta)) 
		cptr->notectrl &= ~FMUSIC_FREQ;	// divide by 0 check

	if (cptr->notectrl & FMUSIC_TRIGGER)		
	{
		//==========================================================================================
		// ALLOCATE A CHANNEL
		//==========================================================================================
		ccptr = &FSOUND_Channel[channel];

        // this swaps between channels to avoid sounds cutting each other off and causing a click
        if (ccptr->sptr != NULL)
        {
            if (channel < 32)
                channel += 32;
            else
                channel -= 32;

            memcpy(&FSOUND_Channel[channel], ccptr, sizeof(FSOUND_CHANNEL));
            FSOUND_Channel[channel].index = channel; // oops dont want its index

            // this should cause the old channel to ramp out nicely.
		    ccptr->volume = ccptr->actualvolume = 0;
		    ccptr->leftvolume  = 0;
		    ccptr->rightvolume = 0;

            ccptr = &FSOUND_Channel[channel];
            cptr->cptr = ccptr;
        }

		ccptr->sptr = sptr;
			
		//==========================================================================================
		// START THE SOUND!
		//==========================================================================================
		if (ccptr->sampleoffset >= sptr->loopstart + sptr->looplen)
			ccptr->sampleoffset = 0;

		ccptr->mixpos    = ccptr->sampleoffset;
		ccptr->mixposlo  = 0;
		ccptr->speeddir  = FSOUND_MIXDIR_FORWARDS;
		ccptr->sampleoffset = 0;	// reset it (in case other samples come in and get corrupted etc)

		// volume ramping
		ccptr->ramp_leftvolume	= 0;
		ccptr->ramp_rightvolume	= 0;
		ccptr->ramp_count		= 0;
	}

	if (cptr->notectrl & FMUSIC_VOLUME)	
	{
		float finalvol;
		int volume;
		
		finalvol = (float)cptr->envvol;				//  6 bits (   64)
		finalvol *= (cptr->volume+cptr->voldelta);	//  6 bits (   64)
		finalvol *= cptr->fadeoutvol;				// 16 bits (65536)
		finalvol *= mod->globalvolume;				//  6 bits (   64)
														// ==============
														// 42 bits
	
		// Any half arsed compiler will convert this into 1 constant at compile time.
		finalvol *= (255.0f / (64.0f * 64.0f * 65536.0f * 64.0f)) * 0.5f;
		volume = (int)finalvol;

        if (ccptr)
        {
    		ccptr->volume = ccptr->actualvolume = volume;
	    	ccptr->leftvolume  = (volume * ccptr->actualpan) / 255;
		    ccptr->rightvolume = (volume * (255L-ccptr->actualpan)) / 255;
        }

//		FSOUND_Software_SetVolume(&FSOUND_Channel[channel], (int)finalvol);
	}
	if (cptr->notectrl & FMUSIC_PAN)
	{
		int pan = cptr->pan+( (cptr->envpan-32) * ( (128-abs(cptr->pan-128))/32 ) );

		if (pan < 0)
			pan = 0;
		if (pan > 255)
			pan = 255;

        if (ccptr)
        {
		    ccptr->pan = ccptr->actualpan = pan;
		    ccptr->leftvolume  = (ccptr->actualvolume * pan) / 255;
		    ccptr->rightvolume = (ccptr->actualvolume * (255L-pan)) / 255;
        }
			
//		FSOUND_Software_SetPan(&FSOUND_Channel[channel], finalpan);
	}
	if (cptr->notectrl & FMUSIC_FREQ)		
	{
		int freq;

		if (mod->flags & FMUSIC_XMFLAGS_LINEARFREQUENCY)
			freq = FMUSIC_XMLINEARPERIOD2HZ(cptr->freq+cptr->freqdelta);
		else
			freq = FMUSIC_PERIOD2HZ(cptr->freq+cptr->freqdelta);

		ccptr->frequency = freq;

		if (freq < 100) 
			freq = 100;

		__asm
		{
			push	eax
			push	ebx
			push	ecx
			push	edx

			// work out low fractional part of the speed using a 64bit divide
			mov		ecx, ccptr
			mov		ebx, FSOUND_MixRate
			mov		eax, freq
			xor		edx, edx
			div		ebx							// get quotient and remainder
			mov		[ecx].speedhi, eax
			div		ebx							// now divide remainder by rate to get lower 32bit fraction
			mov		[ecx].speedlo, eax

			pop		edx
			pop		ecx
			pop		ebx
			pop		eax
		}
	}
	if (cptr->notectrl & FMUSIC_STOP)		
	{
//		FSOUND_StopSound(channel);

		ccptr->mixposlo = 0;
		ccptr->mixpos = 0;
//		ccptr->sptr = NULL;
		ccptr->sampleoffset = 0;	// if this channel gets stolen it will be safe
	}
}


int volume_reset_flag=0;

void FMUSIC_XM_Resetcptr(FMUSIC_CHANNEL *cptr, FSOUND_SAMPLE	*sptr)
{
	if (volume_reset_flag)
		cptr->volume		= (int)sptr->defvol;

	volume_reset_flag=0;

	cptr->pan			= sptr->defpan;
	cptr->envvol		= 64;
	cptr->envvolpos		= 0;
	cptr->envvoltick	= 0;
	cptr->envvoldelta	= 0;

	cptr->envpan		= 32;
	cptr->envpanpos		= 0;
	cptr->envpantick	= 0;
	cptr->envpandelta	= 0;

	cptr->keyoff		= FALSE;
	cptr->fadeoutvol	= 65536;
	cptr->envvolstopped = FALSE;
	cptr->envpanstopped = FALSE;
	cptr->ivibsweeppos = 0;
	cptr->ivibpos = 0;

	// retrigger tremolo and vibrato waveforms
	if ((cptr->wavecontrol & 0xF) < 4) 
		cptr->vibpos=0;
	if ((cptr->wavecontrol >> 4) < 4) 
		cptr->tremolopos=0;

	cptr->tremorpos	= 0;								// retrigger tremor count

	cptr->notectrl |= FMUSIC_VOLUME;
	cptr->notectrl |= FMUSIC_PAN;
}



/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void FMUSIC_UpdateXMNote(FMUSIC_MODULE *mod) 
{
	signed char	breakflag=FALSE;
	signed char	jumpflag=FALSE;
	FMUSIC_NOTE	*current;
	int		count;

	mod->nextorder = -1;
	mod->nextrow = -1;

	// Point our note pointer to the correct pattern buffer, and to the
	// correct offset in this buffer indicated by row and number of channels
	current = mod->pattern[mod->orderlist[mod->order]].data + (mod->row*mod->numchannels);

	if (!current) 
		return;

	// Loop through each channel in the row until we have finished
	for (count=0; count<mod->numchannels; count++,current++)
	{
		FMUSIC_CHANNEL			*cptr  = NULL;
		FMUSIC_INSTRUMENT		*iptr  = NULL;
		FSOUND_SAMPLE			*sptr  = NULL;
		unsigned char			paramx, paramy;
		int						oldvolume, oldfreq, oldpan;
		signed char				porta = FALSE;

		cptr = &FMUSIC_Channel[count];
/*

		if (current->effect) cptr->effectparam = current->eparam;

		//cptr->effect_flag=!(current->effect && current->eparam);
		if ( current->effect && current->eparam) cptr->effect_flag=1;
		if (!current->effect) cptr->effect_flag=0;
		
		if (cptr->effect_flag) current->eparam = cptr->effectparam;
*/

		
		paramx = current->eparam >> 4;			// get effect param x
		paramy = current->eparam & 0xF;			// get effect param y


//			**** FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
//		if (LinkedListIsRootNode(cptr, &cptr->vchannelhead)) 
//			cptr = &FMUSIC_DummyVirtualChannel; // no channels allocated yet
//			**** FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1


		porta = (current->effect == FMUSIC_XM_PORTATO || current->effect == FMUSIC_XM_PORTATOVOLSLIDE);

		// first store note and instrument number if there was one
		if (current->number && !porta)							//  bugfix 3.20 (&& !porta)
			cptr->inst = current->number-1;						// remember the Instrument #

		if (current->note && current->note != FMUSIC_KEYOFF && !porta) //  bugfix 3.20 (&& !porta)
			cptr->note = current->note-1;						// remember the note

		if (cptr->inst >= (int)mod->numinsts)
		{
			iptr = &FMUSIC_DummyInstrument;
			sptr = &FMUSIC_DummySample;
			sptr->buff = NULL;
		}
		else
		{
			// set up some instrument and sample pointers
			iptr = &mod->instrument[cptr->inst];
			if (iptr->keymap[cptr->note] >= 16) 
				sptr = &FMUSIC_DummySample;
			else 
				sptr = iptr->sample[iptr->keymap[cptr->note]];

			if (!porta)
				cptr->sptr = sptr;
		}

		oldvolume = cptr->volume;
		oldfreq   = cptr->freq;
		oldpan    = cptr->pan;

		// if there is no more tremolo, set volume to volume + last tremolo delta
		if (cptr->recenteffect == FMUSIC_XM_TREMOLO && current->effect != FMUSIC_XM_TREMOLO)
			cptr->volume += cptr->voldelta;		
		cptr->recenteffect  = current->effect;

		cptr->voldelta = 0;
		cptr->notectrl	= 0;

		//= PROCESS NOTE ===============================================================================
		if (current->note && current->note != FMUSIC_KEYOFF) 
		{
			// get note according to relative note
			cptr->realnote = current->note + sptr->relative - 1;

			// get period according to realnote and finetune
			if (mod->flags & FMUSIC_XMFLAGS_LINEARFREQUENCY) 
			{
				cptr->period = (10*12*16*4) - (cptr->realnote*16*4) - (sptr->finetune / 2);
			}
			else 
			{
#ifdef FMUSIC_XM_AMIGAPERIODS_ACTIVE
				cptr->period = FMUSIC_XM_GetAmigaPeriod(cptr->realnote, sptr->finetune);
#endif
			}
			
			// frequency only changes if there are no portamento effects
			if (!porta)
				cptr->freq = cptr->period;

			cptr->notectrl = FMUSIC_TRIGGER;
		}

		cptr->freqdelta	= 0;
		cptr->notectrl		|= FMUSIC_FREQ;
		cptr->notectrl		|= FMUSIC_VOLUME;


		//= PROCESS INSTRUMENT NUMBER ==================================================================
//		if (current->number)

		if (current->note && current->note != FMUSIC_KEYOFF) {
      volume_reset_flag = current->number;
			FMUSIC_XM_Resetcptr(cptr,sptr);
		}
		//= PROCESS VOLUME BYTE ========================================================================
#ifdef FMUSIC_XM_VOLUMEBYTE_ACTIVE
		if (current->volume) 
			FMUSIC_XM_ProcessVolumeByte(cptr, current->volume);
#endif

		//= PROCESS KEY OFF ============================================================================
		if (current->note == FMUSIC_KEYOFF || current->effect == FMUSIC_XM_KEYOFF) 
			cptr->keyoff = TRUE;

		//= PROCESS ENVELOPES ==========================================================================
#ifdef FMUSIC_XM_VOLUMEENVELOPE_ACTIVE
		if (iptr->VOLtype & FMUSIC_ENVELOPE_ON)
		{
			if (!cptr->envvolstopped) 
				FMUSIC_XM_ProcessEnvelope(cptr, &cptr->envvolpos, &cptr->envvoltick, iptr->VOLtype, iptr->VOLnumpoints, &iptr->VOLPoints[0], iptr->VOLLoopEnd, iptr->VOLLoopStart, iptr->VOLsustain, &cptr->envvol, &cptr->envvolfrac, &cptr->envvolstopped, &cptr->envvoldelta, FMUSIC_VOLUME);
		}
		else 
#endif
			if (cptr->keyoff) 
				cptr->envvol = 0;

#ifdef FMUSIC_XM_PANENVELOPE_ACTIVE
		if (iptr->PANtype & FMUSIC_ENVELOPE_ON && !cptr->envpanstopped) 
			FMUSIC_XM_ProcessEnvelope(cptr, &cptr->envpanpos, &cptr->envpantick, iptr->PANtype, iptr->PANnumpoints, &iptr->PANPoints[0], iptr->PANLoopEnd, iptr->PANLoopStart, iptr->PANsustain, &cptr->envpan, &cptr->envpanfrac, &cptr->envpanstopped, &cptr->envpandelta, FMUSIC_PAN);
#endif

		//= PROCESS VOLUME FADEOUT =====================================================================
		if (cptr->keyoff) 
		{
			cptr->fadeoutvol -= iptr->VOLfade;
			if (cptr->fadeoutvol < 0) 
				cptr->fadeoutvol = 0;
			cptr->notectrl |= FMUSIC_VOLUME;
		}


		//= PROCESS TICK 0 EFFECTS =====================================================================
#if  1
		switch (current->effect) 
		{
			// not processed on tick 0
#ifdef FMUSIC_XM_ARPEGGIO_ACTIVE
			case FMUSIC_XM_ARPEGGIO : 
			{
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTAUP_ACTIVE
			case FMUSIC_XM_PORTAUP :
			{
				if (current->eparam) 
					cptr->portaup = current->eparam;
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTADOWN_ACTIVE
			case FMUSIC_XM_PORTADOWN :
			{
				if (current->eparam) 
					cptr->portadown = current->eparam;
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTATO_ACTIVE
			case FMUSIC_XM_PORTATO :
			{
				if (current->eparam) 
					cptr->portaspeed = current->eparam;
				cptr->portatarget = cptr->period;
				cptr->notectrl &= ~FMUSIC_TRIGGER;
				cptr->notectrl &= ~FMUSIC_FREQ;
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTATOVOLSLIDE_ACTIVE
			case FMUSIC_XM_PORTATOVOLSLIDE : 
			{
				cptr->portatarget = cptr->period;
				if (current->eparam) 
					cptr->volslide = current->eparam;
				cptr->notectrl &= ~FMUSIC_TRIGGER;
				cptr->notectrl &= ~FMUSIC_FREQ;
				break;
			}
#endif
#ifdef FMUSIC_XM_VIBRATO_ACTIVE
			case FMUSIC_XM_VIBRATO :
			{
				if (paramx) 
					cptr->vibspeed = paramx;
				if (paramy) 
					cptr->vibdepth = paramy;
				FMUSIC_XM_Vibrato(cptr);
				break;
			}
#endif
#ifdef FMUSIC_XM_VIBRATOVOLSLIDE_ACTIVE
			case FMUSIC_XM_VIBRATOVOLSLIDE : 
			{
				if (current->eparam) 
					cptr->volslide = current->eparam;
				FMUSIC_XM_Vibrato(cptr);
				break;								// not processed on tick 0
			}
#endif
#ifdef FMUSIC_XM_TREMOLO_ACTIVE
			case FMUSIC_XM_TREMOLO : 
			{
				if (paramx) 
					cptr->tremolospeed = paramx;
				if (paramy) 
					cptr->tremolodepth = paramy;
				break;
			}
#endif
#ifdef FMUSIC_XM_SETPANPOSITION_ACTIVE
			case FMUSIC_XM_SETPANPOSITION : 
			{
				cptr->pan = current->eparam;
				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
#endif
#ifdef FMUSIC_XM_SETSAMPLEOFFSET_ACTIVE
			case FMUSIC_XM_SETSAMPLEOFFSET : 
			{
				unsigned int offset;

				if (current->eparam) 
					cptr->sampleoffset = current->eparam;

				if (!cptr->cptr) break;

				offset = (int)(cptr->sampleoffset) << 8;

				if (offset >= sptr->loopstart + sptr->looplen) 
				{
					cptr->notectrl &= ~FMUSIC_TRIGGER;
					cptr->notectrl |= FMUSIC_STOP;
				}
				else 
					cptr->cptr->sampleoffset = offset;
				break;
			}
#endif
#ifdef FMUSIC_XM_VOLUMESLIDE_ACTIVE
			case FMUSIC_XM_VOLUMESLIDE : 
			{
				if (current->eparam) 
					cptr->volslide  = current->eparam;
				break;				
			}
#endif
#ifdef FMUSIC_XM_PATTERNJUMP_ACTIVE
			case FMUSIC_XM_PATTERNJUMP : // --- 00 B00 : --- 00 D63 , should put us at ord=0, row=63
			{
				mod->nextorder = current->eparam;
				mod->nextrow = 0;
				if (mod->nextorder >= (int)mod->numorders) 
					mod->nextorder=0;
				jumpflag = 1;
				break;
			}
#endif
#ifdef FMUSIC_XM_SETVOLUME_ACTIVE
			case FMUSIC_XM_SETVOLUME : 
			{
				cptr->volume = current->eparam;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
#endif
#ifdef FMUSIC_XM_PATTERNBREAK_ACTIVE
			case FMUSIC_XM_PATTERNBREAK : 
			{
				mod->nextrow = (paramx*10) + paramy;
				if (mod->nextrow > 63) 
					mod->nextrow = 0;
				if (!breakflag && !jumpflag) 
					mod->nextorder = mod->order+1;
				if (mod->nextorder >= (int)mod->numorders) 
					mod->nextorder=0;
				break;
			}
#endif
			case FMUSIC_XM_SPECIAL : 
			{
				switch (paramx) 
				{
					// not processed on tick 0 / unsupported
					case FMUSIC_XM_RETRIG :
					case FMUSIC_XM_NOTECUT :
					case FMUSIC_XM_SETFILTER :
					case FMUSIC_XM_FUNKREPEAT :
					case FMUSIC_XM_SETGLISSANDO : 
					{
						break;
					}
#ifdef FMUSIC_XM_FINEPORTAUP_ACTIVE
					case FMUSIC_XM_FINEPORTAUP : 
					{
						if (paramy) 
							cptr->fineportaup = paramy;
						cptr->freq -= (cptr->fineportaup << 2);
						break;
					}
#endif
#ifdef FMUSIC_XM_FINEPORTADOWN_ACTIVE
					case FMUSIC_XM_FINEPORTADOWN :
					{
						if (paramy) 
							cptr->fineportadown = paramy;
						cptr->freq += (cptr->fineportadown << 2);
						break;
					}
#endif
#ifdef FMUSIC_XM_SETVIBRATOWAVE_ACTIVE
					case FMUSIC_XM_SETVIBRATOWAVE : 
					{
						cptr->wavecontrol &= 0xF0;
						cptr->wavecontrol |= paramy;
						break;
					}
#endif
#ifdef FMUSIC_XM_SETFINETUNE_ACTIVE
					case FMUSIC_XM_SETFINETUNE :
					{
						sptr->finetune = paramy;
						break;
					}
#endif
#ifdef FMUSIC_XM_PATTERNLOOP_ACTIVE
					case FMUSIC_XM_PATTERNLOOP :
					{
						if (paramy == 0) 
							cptr->patlooprow = mod->row;
						else 
						{
							if (!cptr->patloopno) 
								cptr->patloopno = paramy;
							else cptr->patloopno--;
							if (cptr->patloopno) 
								mod->row = cptr->patlooprow-1;
						}
						break;
					}
#endif
#ifdef FMUSIC_XM_SETTREMOLOWAVE_ACTIVE
					case FMUSIC_XM_SETTREMOLOWAVE : 
					{
						cptr->wavecontrol &= 0xF;
						cptr->wavecontrol |= (paramy<<4);
						break;
					}
#endif
#ifdef FMUSIC_XM_SETPANPOSITION16_ACTIVE
					case FMUSIC_XM_SETPANPOSITION16 :
					{
						cptr->pan = paramy<<4;
						cptr->notectrl |= FMUSIC_PAN;
						break;
					}
#endif
#ifdef FMUSIC_XM_FINEVOLUMESLIDEUP_ACTIVE
					case FMUSIC_XM_FINEVOLUMESLIDEUP :
					{
						if (paramy) 
							cptr->finevslup = paramy;

						cptr->volume += cptr->finevslup;
						
						if (cptr->volume > 64) 
							cptr->volume=64;

						cptr->notectrl |= FMUSIC_VOLUME;
						break;
					}
#endif
#ifdef FMUSIC_XM_FINEVOLUMESLIDEDOWN_ACTIVE
					case FMUSIC_XM_FINEVOLUMESLIDEDOWN :
					{
						if (paramy) cptr->finevslup = paramy;

						cptr->volume -= cptr->finevslup;
						
						if (cptr->volume < 0) 
							cptr->volume =0;

						cptr->notectrl |= FMUSIC_VOLUME;
						break;
					}
#endif
#ifdef FMUSIC_XM_NOTEDELAY_ACTIVE
					case FMUSIC_XM_NOTEDELAY : 
					{
						cptr->volume = oldvolume;
						cptr->freq   = oldfreq;
						cptr->pan    = oldpan;
						cptr->notectrl &= ~FMUSIC_FREQ;
						cptr->notectrl &= ~FMUSIC_VOLUME;
						cptr->notectrl &= ~FMUSIC_PAN;
						cptr->notectrl &= ~FMUSIC_TRIGGER;
						break;
					}
#endif
#ifdef FMUSIC_XM_PATTERNDELAY_ACTIVE
					case FMUSIC_XM_PATTERNDELAY : 
					{
						mod->patterndelay = paramy;
						mod->patterndelay *= mod->speed;
						break;
					}
#endif
				};
				break;
			}
#ifdef FMUSIC_XM_SETSPEED_ACTIVE
			case FMUSIC_XM_SETSPEED : 
			{
				if (current->eparam < 0x20)
					mod->speed = current->eparam;
				else
					FMUSIC_SetBPM(mod, current->eparam); 
				break;
			}
#endif
#ifdef FMUSIC_XM_SETGLOBALVOLUME_ACTIVE
			case FMUSIC_XM_SETGLOBALVOLUME : 
			{
				mod->globalvolume = current->eparam;
				if (mod->globalvolume > 64) 
					mod->globalvolume=64;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
#endif
#ifdef FMUSIC_XM_GLOBALVOLSLIDE_ACTIVE
			case FMUSIC_XM_GLOBALVOLSLIDE :
			{
				if (current->eparam) 
					mod->globalvsl = current->eparam;
				break;
			}
#endif
#ifdef FMUSIC_XM_SETENVELOPEPOS_ACTIVE
			case FMUSIC_XM_SETENVELOPEPOS :
			{
				int currpos, nextpos;
				int currtick, nexttick;
				int currvol, nextvol, tickdiff;

				if (!(iptr->VOLtype & FMUSIC_ENVELOPE_ON)) 
					break;

				currpos = 0;
				
				// search and reinterpolate new envelope position				
				while (current->eparam > iptr->VOLPoints[(currpos+1)<<1] && currpos < iptr->VOLnumpoints) currpos++;

				cptr->envvolpos = currpos;

				// if it is at the last position, abort the envelope and continue last volume
				if (cptr->envvolpos >= iptr->VOLnumpoints - 1) 
				{
					cptr->envvol = iptr->VOLPoints[((iptr->VOLnumpoints-1)<<1)+1];
					cptr->envvolstopped = TRUE;
					break;
				}
				
				cptr->envvolstopped = FALSE;
				cptr->envvoltick = current->eparam;

				nextpos	= cptr->envvolpos + 1;

				currtick = iptr->VOLPoints[currpos<<1];				// get tick at this point
				nexttick = iptr->VOLPoints[nextpos<<1];				// get tick at next point

				currvol = iptr->VOLPoints[(currpos<<1)+1] << 16;	// get VOL at this point << 16
				nextvol	= iptr->VOLPoints[(nextpos<<1)+1] << 16;	// get VOL at next point << 16

				// interpolate 2 points to find delta step
				tickdiff = nexttick - currtick;
				if (tickdiff) cptr->envvoldelta = (nextvol-currvol) / tickdiff;
				else		  cptr->envvoldelta = 0;

				tickdiff = cptr->envvoltick - currtick;

				cptr->envvolfrac  = currvol + (cptr->envvoldelta * tickdiff);
				cptr->envvol = cptr->envvolfrac >> 16;
				cptr->envvolpos++;
				break;
			}
#endif
#ifdef FMUSIC_XM_PANSLIDE_ACTIVE
			case FMUSIC_XM_PANSLIDE :
			{
				if (current->eparam) 
				{
					cptr->panslide = current->eparam;
					cptr->notectrl |= FMUSIC_PAN;
				}
				break;
			}
#endif
#ifdef FMUSIC_XM_MULTIRETRIG_ACTIVE
			case FMUSIC_XM_MULTIRETRIG:
			{
				if (current->eparam) 
				{
					cptr->retrigx = paramx;
					cptr->retrigy = paramy;
				}
				break;
			}
#endif
#ifdef FMUSIC_XM_TREMOR_ACTIVE
			case FMUSIC_XM_TREMOR :
			{
				if (current->eparam) 
				{
					cptr->tremoron = (paramx+1);
					cptr->tremoroff = (paramy+1);
				}
				FMUSIC_XM_Tremor(cptr);
				break;
			}
#endif
#ifdef FMUSIC_XM_EXTRAFINEPORTA_ACTIVE
			case FMUSIC_XM_EXTRAFINEPORTA :
			{
				
				if (paramx == 1) 
				{
					if (paramy) 
						cptr->xtraportaup = paramy;
					cptr->freq -= cptr->xtraportaup;
				}
				else if (paramx == 2) 
				{
					if (paramy) 
						cptr->xtraportadown = paramy;
					cptr->freq += cptr->xtraportadown;
				}
				break;
			}
#endif
//			default:
//				__assume(0);
		};
#endif
		//= INSTRUMENT VIBRATO ============================================================================
#ifdef FMUSIC_XM_INSTRUMENTVIBRATO_ACTIVE
		FMUSIC_XM_InstrumentVibrato(cptr, iptr);	// this gets added to previous freqdeltas
#endif	
		FMUSIC_XM_UpdateFlags(cptr,sptr,mod);
	}
 }           


/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void FMUSIC_UpdateXMEffects(FMUSIC_MODULE *mod) 
{
	FMUSIC_NOTE	*current;
	int		count;

	current = mod->pattern[mod->orderlist[mod->order]].data + (mod->row*mod->numchannels);

//	if (!current) return;

	for (count=0; count<mod->numchannels; count++,current++)
	{
		FMUSIC_CHANNEL			*cptr  = NULL;
		FMUSIC_INSTRUMENT		*iptr  = NULL;
		FSOUND_SAMPLE			*sptr  = NULL;
		unsigned char			effect, paramx, paramy;
		
		cptr = &FMUSIC_Channel[count];

//			**** FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
//		cptr = LinkedListNextNode(&cptr->vchannelhead);
//
//		if (LinkedListIsRootNode(cptr, &cptr->vchannelhead)) 
//			cptr = &FMUSIC_DummyVirtualChannel; // no channels allocated yet
//			**** FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

/*
		if (cptr->inst >= (int)mod->numinsts)
		{
			iptr = &FMUSIC_DummyInstrument;
			sptr = &FMUSIC_DummySample;
			sptr->buff = NULL;
		}
		else
*/
		{
			iptr = &mod->instrument[cptr->inst];
			if (iptr->keymap[cptr->note] >= 16) 
				sptr = &FMUSIC_DummySample;
			else 
				sptr = iptr->sample[iptr->keymap[cptr->note]];

			if (!sptr)
				sptr = &FMUSIC_DummySample;
		}

		effect = current->effect;			// grab the effect number
		paramx = current->eparam >> 4;		// grab the effect parameter x
		paramy = current->eparam & 0xF;		// grab the effect parameter y

		cptr->voldelta	= 0;				// this is for tremolo / tremor etc
		cptr->freqdelta = 0;				// this is for vibrato / arpeggio etc
		cptr->notectrl	= 0;

		//= PROCESS ENVELOPES ==========================================================================
#ifdef FMUSIC_XM_VOLUMEENVELOPE_ACTIVE
		if (iptr->VOLtype & FMUSIC_ENVELOPE_ON && !cptr->envvolstopped) 
			FMUSIC_XM_ProcessEnvelope(cptr, &cptr->envvolpos, &cptr->envvoltick, iptr->VOLtype, iptr->VOLnumpoints, &iptr->VOLPoints[0], iptr->VOLLoopEnd, iptr->VOLLoopStart, iptr->VOLsustain, &cptr->envvol, &cptr->envvolfrac, &cptr->envvolstopped, &cptr->envvoldelta, FMUSIC_VOLUME);
#endif
#ifdef FMUSIC_XM_PANENVELOPE_ACTIVE
		if (iptr->PANtype & FMUSIC_ENVELOPE_ON && !cptr->envpanstopped) 
			FMUSIC_XM_ProcessEnvelope(cptr, &cptr->envpanpos, &cptr->envpantick, iptr->PANtype, iptr->PANnumpoints, &iptr->PANPoints[0], iptr->PANLoopEnd, iptr->PANLoopStart, iptr->PANsustain, &cptr->envpan, &cptr->envpanfrac, &cptr->envpanstopped, &cptr->envpandelta, FMUSIC_PAN);
#endif

		//= PROCESS VOLUME FADEOUT =====================================================================
		if (cptr->keyoff) 
		{
			cptr->fadeoutvol -= iptr->VOLfade;
			if (cptr->fadeoutvol < 0) 
				cptr->fadeoutvol = 0;
			cptr->notectrl |= FMUSIC_VOLUME;
		}

		#ifdef FMUSIC_XM_VOLUMEBYTE_ACTIVE
		switch (current->volume >> 4) 
		{
//			case 0x0: 
//			case 0x1: 
//			case 0x2: 
//			case 0x3: 
//			case 0x4: 
//			case 0x5: 
//				break;
			case 0x6: 
			{
				cptr->volume -= (current->volume & 0xF);
				if (cptr->volume < 0) 
					cptr->volume = 0;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
			case 0x7 :
			{
				cptr->volume += (current->volume & 0xF);
				if (cptr->volume > 0x40) 
					cptr->volume = 0x40;
				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
#ifdef FMUSIC_XM_VIBRATO_ACTIVE
			case 0xb :
			{
				cptr->vibdepth = (current->volume & 0xF);

				FMUSIC_XM_Vibrato(cptr);

				cptr->vibpos += cptr->vibspeed;
				if (cptr->vibpos > 31) 
					cptr->vibpos -= 64;
				break;
			}
#endif
			case 0xd :
			{
				cptr->pan -= (current->volume & 0xF);
				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
			case 0xe :
			{
				cptr->pan += (current->volume & 0xF);
				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
#ifdef FMUSIC_XM_PORTATO_ACTIVE
			case 0xf :
			{
				FMUSIC_XM_Portamento(cptr);
				break;
			}
#endif
//			default:
//				__assume(0);
		};
		#endif


		switch(effect) 
		{
#ifdef FMUSIC_XM_ARPEGGIO_ACTIVE
			case FMUSIC_XM_ARPEGGIO : 
			{
				if (current->eparam > 0)
				{
					switch (mod->tick % 3) 
					{
						case 1: 
							if (mod->flags & FMUSIC_XMFLAGS_LINEARFREQUENCY) 
								cptr->freqdelta = paramx << 6;
#ifdef FMUSIC_XM_AMIGAPERIODS_ACTIVE
							else 
								cptr->freqdelta = FMUSIC_XM_GetAmigaPeriod(cptr->realnote+paramx, sptr->finetune) -
												  FMUSIC_XM_GetAmigaPeriod(cptr->realnote, sptr->finetune);
#endif
							break;
						case 2: 
							if (mod->flags & FMUSIC_XMFLAGS_LINEARFREQUENCY) 
								cptr->freqdelta = paramy << 6;
#ifdef FMUSIC_XM_AMIGAPERIODS_ACTIVE
							else 
								cptr->freqdelta = FMUSIC_XM_GetAmigaPeriod(cptr->realnote+paramy, sptr->finetune) -
												  FMUSIC_XM_GetAmigaPeriod(cptr->realnote, sptr->finetune);
#endif
							break;
						//default:
								//__assume(0);
					};
					cptr->notectrl |= FMUSIC_FREQ;
				}
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTAUP_ACTIVE
			case FMUSIC_XM_PORTAUP : 
			{
				cptr->freqdelta = 0;

				cptr->freq -= cptr->portaup << 2; // subtract freq
				if (cptr->freq < 56) 
					cptr->freq=56;  // stop at B#8
				cptr->notectrl |= FMUSIC_FREQ;
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTADOWN_ACTIVE
			case FMUSIC_XM_PORTADOWN : 
			{
				cptr->freqdelta = 0;

				cptr->freq += cptr->portadown << 2; // subtract freq
				cptr->notectrl |= FMUSIC_FREQ;
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTATO_ACTIVE
			case FMUSIC_XM_PORTATO : 
			{
				cptr->freqdelta = 0;

				FMUSIC_XM_Portamento(cptr);
				break;
			}
#endif
#ifdef FMUSIC_XM_VIBRATO_ACTIVE
			case FMUSIC_XM_VIBRATO :
			{
				FMUSIC_XM_Vibrato(cptr);
				cptr->vibpos += cptr->vibspeed;
				if (cptr->vibpos > 31) 
					cptr->vibpos -= 64;
				break;
			}
#endif
#ifdef FMUSIC_XM_PORTATOVOLSLIDE_ACTIVE
			case FMUSIC_XM_PORTATOVOLSLIDE : 
			{
				cptr->freqdelta = 0;

				FMUSIC_XM_Portamento(cptr);

				paramx = cptr->volslide >> 4;    // grab the effect parameter x
				paramy = cptr->volslide & 0xF;   // grab the effect parameter y

				// slide up takes precedence over down
				if (paramx) 
				{
					cptr->volume += paramx;
					if (cptr->volume > 64) 
						cptr->volume = 64;
				}
				else if (paramy) 
				{
					cptr->volume -= paramy;
					if (cptr->volume < 0)
						cptr->volume = 0;
				}

				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
#endif
#ifdef FMUSIC_XM_VIBRATOVOLSLIDE_ACTIVE
			case FMUSIC_XM_VIBRATOVOLSLIDE : 
			{
				FMUSIC_XM_Vibrato(cptr);
				cptr->vibpos += cptr->vibspeed;
				if (cptr->vibpos > 31)
					cptr->vibpos -= 64;

				paramx = cptr->volslide >> 4;    // grab the effect parameter x
				paramy = cptr->volslide & 0xF;   // grab the effect parameter y

				// slide up takes precedence over down
				if (paramx) 
				{
					cptr->volume += paramx;
					if (cptr->volume > 64)
						cptr->volume = 64;
				}
				else if (paramy) 
				{
					cptr->volume -= paramy;
					if (cptr->volume < 0)
						cptr->volume = 0;
				}

				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
#endif
#ifdef FMUSIC_XM_TREMOLO_ACTIVE
			case FMUSIC_XM_TREMOLO :
			{
				FMUSIC_XM_Tremolo(cptr);
				break;
			}
#endif
#ifdef FMUSIC_XM_VOLUMESLIDE_ACTIVE
			case FMUSIC_XM_VOLUMESLIDE : 
			{
				paramx = cptr->volslide >> 4;    // grab the effect parameter x
				paramy = cptr->volslide & 0xF;   // grab the effect parameter y
				
				// slide up takes precedence over down
				if (paramx) 
				{
					cptr->volume += paramx;
					if (cptr->volume > 64) 
						cptr->volume = 64;
				}
				else if (paramy) 
				{
					cptr->volume -= paramy;
					if (cptr->volume < 0)  
						cptr->volume = 0;
				}

				cptr->notectrl |= FMUSIC_VOLUME;
				break;
			}
#endif
			// extended PT effects
			case FMUSIC_XM_SPECIAL: 
			{
				switch (paramx) 
				{
#ifdef FMUSIC_XM_NOTECUT_ACTIVE
					case FMUSIC_XM_NOTECUT: 
					{
						if (mod->tick==paramy) 
						{
							cptr->volume = 0;
							cptr->notectrl |= FMUSIC_VOLUME;
						}
						break;
					}
#endif
#ifdef FMUSIC_XM_RETRIG_ACTIVE
					case FMUSIC_XM_RETRIG : 
					{
						if (!paramy) 
							break; // divide by 0 bugfix
						if (!(mod->tick % paramy)) 
						{
							cptr->notectrl |= FMUSIC_TRIGGER;
							cptr->notectrl |= FMUSIC_VOLUME;
							cptr->notectrl |= FMUSIC_FREQ;
						}
						break;
					}
#endif
#ifdef FMUSIC_XM_NOTEDELAY_ACTIVE
					case FMUSIC_XM_NOTEDELAY : 
					{
						if (mod->tick == paramy) 
						{
							//= PROCESS INSTRUMENT NUMBER ==================================================================
							FMUSIC_XM_Resetcptr(cptr,sptr);

							cptr->freq = cptr->period;
							cptr->notectrl |= FMUSIC_FREQ;
#ifdef FMUSIC_XM_VOLUMEBYTE_ACTIVE
							if (current->volume) 
								FMUSIC_XM_ProcessVolumeByte(cptr, current->volume);
#endif
							cptr->notectrl |= FMUSIC_TRIGGER;
						}
						else
						{
							cptr->notectrl &= ~FMUSIC_VOLUME;
							cptr->notectrl &= ~FMUSIC_FREQ;
							cptr->notectrl &= ~FMUSIC_PAN;
							cptr->notectrl &= ~FMUSIC_TRIGGER;
						}
						break;
					}
#endif
					//default:
							//__assume(0);
				};
				break;
			}
#ifdef FMUSIC_XM_MULTIRETRIG_ACTIVE
			case FMUSIC_XM_MULTIRETRIG : 
			{
				if (!cptr->retrigy) break; // divide by 0 bugfix
				
				if (!(mod->tick % cptr->retrigy)) 
				{
					if (cptr->retrigx) 
					{
						switch (cptr->retrigx) 
						{
							case 1: cptr->volume--;
									break;
							case 2: cptr->volume -= 2;
									break;
							case 3: cptr->volume -= 4;
									break;
							case 4: cptr->volume -= 8;
									break;
							case 5: cptr->volume -= 16;
									break;
							case 6: cptr->volume = cptr->volume * 2 / 3;
									break;
							case 7: cptr->volume >>= 1;
									break;
							case 8: // ?
									break;
							case 9: cptr->volume++;
									break;
							case 0xA: cptr->volume += 2;
									break;
							case 0xB: cptr->volume += 4;
									break;
							case 0xC: cptr->volume += 8;
									break;
							case 0xD: cptr->volume += 16;
									break;
							case 0xE: cptr->volume = cptr->volume * 3 / 2;
									break;
							case 0xF: cptr->volume <<= 1;
									break;
						//default:
								//__assume(0);
						};
						if (cptr->volume > 64) cptr->volume = 64;
						if (cptr->volume < 0)  cptr->volume = 0;
						cptr->notectrl |= FMUSIC_VOLUME;
					}
					cptr->notectrl |= FMUSIC_TRIGGER;
				}
				break;
			}
#endif
#ifdef FMUSIC_XM_GLOBALVOLSLIDE_ACTIVE
			case FMUSIC_XM_GLOBALVOLSLIDE : 
			{
				paramx = mod->globalvsl >> 4;    // grab the effect parameter x
				paramy = mod->globalvsl & 0xF;   // grab the effect parameter y
				
				// slide up takes precedence over down
				if (paramx) 
				{
					mod->globalvolume += paramx;
					if (mod->globalvolume > 64) 
						mod->globalvolume = 64;
				}
				else if (paramy) 
				{
					mod->globalvolume -= paramy;
					if (mod->globalvolume < 0)  
						mod->globalvolume = 0;
				}
				break;
			}
#endif
#ifdef FMUSIC_XM_PANSLIDE_ACTIVE
			case FMUSIC_XM_PANSLIDE :
			{
				paramx = cptr->panslide >> 4;    // grab the effect parameter x
				paramy = cptr->panslide & 0xF;   // grab the effect parameter y
				
				// slide right takes precedence over left
				if (paramx) 
				{
					cptr->pan += paramx;
					if (cptr->pan > 255) 
						cptr->pan = 255;
				}
				else if (paramy) 
				{
					cptr->pan -= paramy;
					if (cptr->pan < 0)  
						cptr->pan = 0;
				}

				cptr->notectrl |= FMUSIC_PAN;
				break;
			}
#endif
#ifdef FMUSIC_XM_TREMOR_ACTIVE
			case FMUSIC_XM_TREMOR :
			{
				FMUSIC_XM_Tremor(cptr);
				break;
			}
#endif
//			default:
//				__assume(0);
		};

		//= INSTRUMENT VIBRATO ============================================================================
#ifdef FMUSIC_XM_INSTRUMENTVIBRATO_ACTIVE
		FMUSIC_XM_InstrumentVibrato(cptr, iptr);	// this gets added to previous freqdeltas
#endif

		FMUSIC_XM_UpdateFlags(cptr, sptr,mod);
	}
}



/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void FMUSIC_UpdateXM(FMUSIC_MODULE *mod)
{
	if (mod->tick == 0)									// new note
	{
		// process any rows commands to set the next order/row
		if (mod->nextorder >= 0) mod->order = mod->nextorder;
		if (mod->nextrow >= 0)   mod->row = mod->nextrow;

		FMUSIC_UpdateXMNote(mod);					// Update and play the note

		// if there were no row commands
		if (mod->nextrow == -1)
		{
			mod->nextrow = mod->row+1;
			if (mod->nextrow >= mod->pattern[mod->orderlist[mod->order]].rows)	// if end of pattern 
			{ 
				mod->nextorder = mod->order+1;			// so increment the order
				if (mod->nextorder >= (int)mod->numorders) 
					mod->nextorder = (int)mod->restart;
				mod->nextrow = 0;						// start at top of pattn
			}
		}
	}
	else 
		FMUSIC_UpdateXMEffects(mod);					// Else update the inbetween row effects


	mod->tick++;
	if (mod->tick >= mod->speed + mod->patterndelay)
	{
		mod->patterndelay = 0;
		mod->tick = 0;
	}
}


/*
[API]
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
signed char FMUSIC_LoadXM(FMUSIC_MODULE *mod)
{
//	unsigned short	filenumpatterns;//=0;
	int				count;
//	unsigned int	mainHDRsize;

	//= VERIFY ==================================================================================

//	FSOUND_File_Seek(0, SEEK_SET);

#ifdef __EXCEPTION_HANDLER__
	signed char		str[256];
	char			*ComparisonStr = "Extended Module: "; 
	FSOUND_File_Read(str, 17);

	for (count=0; count < 17; count++)
		if (str[count] != ComparisonStr[count])
			return FALSE;
#endif

	// set a few default values for this format
	mod->Update			= &FMUSIC_UpdateXM;
	mod->defaultglobalvolume = 64;

//	FSOUND_File_Seek(21, SEEK_CUR);      		// read in module name.

	// Skip tracker name and version number
	FSOUND_File_Read(&mod->numorders, 6+2+256+8);

	// seek to patterndata
	FSOUND_File_Seek(/*60L+*/0x150, SEEK_SET);

/*
	mod->numpatterns = 0;
	for (count=0; count<(int)mod->numorders; count++)
	{
		if (mod->orderlist[count] >= mod->numpatterns) 
			mod->numpatterns = mod->orderlist[count]+1;
	}
*/
	// alloc pattern array (whatever is bigger.. filenumpatterns or mod->numpatterns)
//	mod->numpatternsmem = (mod->numpatterns > filenumpatterns ? mod->numpatterns : filenumpatterns);
	mod->numpatternsmem = mod->numpatterns;// = filenumpatterns;
	mod->pattern = (FMUSIC_PATTERN *)FSOUND_Memory_Calloc(mod->numpatternsmem * sizeof(FMUSIC_PATTERN));	//FIXME:MEMLEAK

	// unpack and read patterns
	for (count=0; count < mod->numpatterns; count++) 
	{
		FMUSIC_PATTERN *pptr;
//		unsigned short	patternsize, rows;
//		unsigned int	patternHDRsize;
		unsigned char tempchar = 0;

		pptr = &mod->pattern[count];

/*
		FSOUND_File_Read(&patternHDRsize, 4);
		FSOUND_File_Read(&tempchar, 1);				
		FSOUND_File_Read(&rows, 2);				// length of pattern
		FSOUND_File_Read(&patternsize, 2);		// packed pattern size
*/
		//pptr->rows = rows;
		pptr->rows = 0x40;

		// allocate memory for pattern buffer
		pptr->data = (FMUSIC_NOTE *)FSOUND_Memory_Calloc(mod->numchannels * pptr->rows * sizeof(FMUSIC_NOTE));	//FIXME:MEMLEAK

		{
			int x,y;
			for (x=0; x < pptr->rows; x++) 
				for (y=0; y < mod->numchannels; y++) {
					FMUSIC_NOTE *nptr;
					nptr = &(pptr->data[x*mod->numchannels+y]);

					*nptr=*(FMUSIC_NOTE*)mod->notecallback(count,x,y);
					if (nptr->note == 97)			nptr->note = FMUSIC_KEYOFF;
					if (nptr->number > 0x80)	nptr->number = 0;
				}
		}
		
	}

	// alloc instrument array
	mod->instrument = (FMUSIC_INSTRUMENT *)FSOUND_Memory_Calloc((int)(mod->numinsts) * sizeof(FMUSIC_INSTRUMENT));

	// load instrument information
	for (count=0; count<(int)mod->numinsts; count++) 
	{
		unsigned int		count2;
		FMUSIC_INSTRUMENT	*iptr;
		unsigned int		instHDRsize;
		unsigned short		numsamples;
		int				firstsampleoffset;

		// point a pointer to that particular instrument
		iptr = &mod->instrument[count];

		firstsampleoffset = FSOUND_File_Tell();
		FSOUND_File_Read(&instHDRsize, 4);				// instrument size
		firstsampleoffset += instHDRsize;

		FSOUND_File_Seek(23, SEEK_CUR); 				// instrument name
		FSOUND_File_Read(&numsamples, 2);				// number of samples in this instrument

/*
		if (numsamples >16)
			return FALSE;
*/
		iptr->numsamples = numsamples;

		if (numsamples > 0) 
		{
			unsigned int sampHDRsize;
			unsigned char tempchar = 0;

			FSOUND_File_Read(&sampHDRsize, 4);			// sampleheader size
			FSOUND_File_Read(iptr->keymap, 96);			// sample numbers
			FSOUND_File_Read(iptr->VOLPoints, 48);		// Volume Envelope (24 words)
			FSOUND_File_Read(iptr->PANPoints, 48);		// Panning Envelope (24 words)

#if 1	// WARNING! DONT CHANGE THE HEADER AROUND BECAUSE OF THIS HACK.
			FSOUND_File_Read(&iptr->VOLnumpoints, 16);
#else
			iptr->VOLnumpoints	= FSOUND_File_GetChar(fp);
			iptr->PANnumpoints	= FSOUND_File_GetChar(fp);
			iptr->VOLsustain	= FSOUND_File_GetChar(fp);
			iptr->VOLLoopStart	= FSOUND_File_GetChar(fp);
			iptr->VOLLoopEnd	= FSOUND_File_GetChar(fp);
			iptr->PANsustain	= FSOUND_File_GetChar(fp);
			iptr->PANLoopStart	= FSOUND_File_GetChar(fp);
			iptr->PANLoopEnd	= FSOUND_File_GetChar(fp);
			iptr->VOLtype		= FSOUND_File_GetChar(fp);
			iptr->PANtype		= FSOUND_File_GetChar(fp);
			iptr->VIBtype		= FSOUND_File_GetChar(fp);
			iptr->VIBsweep		= FSOUND_File_GetChar(fp);
			iptr->VIBdepth		= FSOUND_File_GetChar(fp);
			iptr->VIBrate		= FSOUND_File_GetChar(fp);
			FSOUND_File_Read(&iptr->VOLfade, 2);
#endif

			iptr->VOLfade *= 2;		// i DONT KNOW why i needed this.. it just made it work

			if (iptr->VOLnumpoints < 2) iptr->VOLtype = FMUSIC_ENVELOPE_OFF;
			if (iptr->PANnumpoints < 2) iptr->PANtype = FMUSIC_ENVELOPE_OFF;

			// seek to first sample
			FSOUND_File_Seek(firstsampleoffset, SEEK_SET);
			for (count2=0; count2<numsamples; count2++) 
			{
				FSOUND_SAMPLE	*sptr;
				unsigned char	dat;

				iptr->sample[count2] = (FSOUND_SAMPLE *)FSOUND_Memory_Calloc(sizeof(FSOUND_SAMPLE));
				sptr = iptr->sample[count2];

#if 1	// WARNING! DONT CHANGE THE HEADER AROUND BECAUSE OF THIS HACK.
				FSOUND_File_Read(&sptr->length, 14);
#else
				FSOUND_File_Read(&sptr->length, 4);
				FSOUND_File_Read(&sptr->loopstart, 4);
				FSOUND_File_Read(&sptr->looplen, 4);
				sptr->defvol = FSOUND_File_GetChar(fp);
				sptr->finetune = FSOUND_File_GetChar(fp);			// finetune -128 to +127
#endif

				// type of sample
				sptr->loopmode = FSOUND_LOOP_OFF;
				sptr->bits = 8;

				FSOUND_File_Read(&dat, 1);
				if (dat & 1) 
				{
					sptr->loopmode &= ~(FSOUND_LOOP_OFF);
					sptr->loopmode |= FSOUND_LOOP_NORMAL;
				}
				if (dat & 2) 
				{
					sptr->loopmode &= ~(FSOUND_LOOP_OFF | FSOUND_LOOP_NORMAL);
					sptr->loopmode |= FSOUND_LOOP_BIDI;		// bidirectional
				}
#ifdef __16BIT_SAMPLE_SUPPORT__
				if (dat & 16)	sptr->bits = 16;		
#endif

				if (sptr->loopmode & FSOUND_LOOP_OFF) 
				{
					sptr->loopstart = 0;
					sptr->looplen = sptr->length;
				}

#ifdef __16BIT_SAMPLE_SUPPORT__
				if (sptr->bits == 16)
				{
					sptr->length >>= 1;
					sptr->loopstart >>= 1;
					sptr->looplen >>= 1;
				}
#endif

				if (!sptr->looplen) 
				{
					sptr->loopstart = 0;
					sptr->looplen = sptr->length;
					sptr->loopmode = FSOUND_LOOP_OFF;
				}

				FSOUND_File_Read(&tempchar, 1);	sptr->defpan   = (int)tempchar;
				FSOUND_File_Read(&tempchar, 1);	sptr->relative = (int)tempchar;

				FSOUND_File_Seek(23, SEEK_CUR);

				sptr->buff = (unsigned char *)FSOUND_Memory_Calloc((sptr->length<<1)+16);

			}

			// Load sample data
			for (count2=0; count2<numsamples; count2++) 
			{
				FSOUND_SAMPLE	*sptr = iptr->sample[count2];
				unsigned int	samplelenbytes = sptr->length * sptr->bits / 8;
				int				count3;
				char			*buff = NULL;

//				if (sptr->length)
				{

					buff = FSOUND_Memory_Calloc((sptr->length * 2)+16);

					mod->samplecallback(buff, samplelenbytes, sptr->bits, count, count2);

					{
						signed short *wptr;

						// promote to 16bits
						wptr = (signed short *)sptr->buff;

						for (count3=0; count3 < (int)sptr->length; count3++)
							*wptr++ = (signed short)(buff[count3]) << 8;

						sptr->bits = 16;

						FSOUND_Memory_Free(buff);
					}
#ifdef __16BIT_SAMPLE_SUPPORT__
					else
					{
						FSOUND_Memory_Free(sptr->buff);
						sptr->buff = buff;
					}
#endif
//					mode = FSOUND_DELTA | FSOUND_SIGNED | FSOUND_16BITS;

					{
						signed short *buff = (signed short *)sptr->buff;
					
						// BUGFIX 1.3 - removed click for end of non looping sample (also size optimized a bit)
						if (sptr->loopmode == FSOUND_LOOP_BIDI)
						{
							buff[sptr->loopstart+sptr->looplen] = buff[sptr->loopstart+sptr->looplen-1];// fix it
						}
						else if (sptr->loopmode == FSOUND_LOOP_NORMAL)
						{
							buff[sptr->loopstart+sptr->looplen] = buff[sptr->loopstart];// fix it
						}
					}
				}
			}
		}
	}


	return TRUE;
}
