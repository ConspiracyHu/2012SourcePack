/******************************************************************************/
/* MIXER_CLIPCOPY.C                                                           */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include "sound.h"

static unsigned long mix_temp1		= 0;
static const float	 mix_3times2pow51 = 3.0f * (float)(1L<<25) * (float)(1L<<26);

/*
[API]
[
	[DESCRIPTION]
	Size optimized version of the commented out clipper below

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
	This uses an fadd trick and does 2 float to ints in about 8 cycles.. which is what just 1 
	fistp would take normally.. note this is meant for p5 machines, as ppro has a 1cycle 
	fistp which is faster.

	[SEE_ALSO]
]
*/
void FSOUND_MixerClipCopy_Float32(void *dest, void *src, long len)
{
	int count;
	short *destptr = (short *)dest;
	float *srcptr = (float *)src;

	//if (len <=0 || !dest || !src) return;

	for (count=0; count<len<<1; count++)
	{
		int val;
		__asm
		{
			mov eax, srcptr
			fld [eax]
			add srcptr, 4
			fistp val
		}
		*destptr++ = (val < -32768 ? -32768 : val > 32767 ? 32767 : val);
	}
		
}
