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

// this is smaller but slower.
#define USER_SMALLER_CLIPCOPY

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
#ifdef USER_SMALLER_CLIPCOPY
void FSOUND_MixerClipCopy_Float32(void *dest, void *src, long len)
{
	int count;
	short *destptr = (short *)dest;
	float *srcptr = (float *)src;

	if (len <=0 || !dest || !src) 
		return;

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
#else

void FSOUND_MixerClipCopy_Float32(void *dest, void *src, long len)
{
	__int64 lefti,righti;

	if (len <=0 || !dest || !src) 
		return;

	__asm
	{
		fld		mix_3times2pow51		// st0:magic

		// if there wasnt any channels mixed (all their sptr's were null) then just clear, dont clip
		jne		ClipAndCopySetup
		xor		eax, eax

		mov		edi, dest
		mov		ecx, len				// number of dwords in 16bit stereo output stream
		rep		stosd
		jmp		MixerFPU_Stereo16_Finished

		//= Clip and copy 32bit mixbuffer to 16bit output for both pointers
	ClipAndCopySetup:
		mov		esi, src				// point esi to float 32bit mix buffer
		mov		edi, dest				// point edi to 16bit output stream
		mov		ecx, len				
		mov		ebx, [edi]				// preread destination into cache

	ClipAndCopyLoop:
		fld		dword ptr [esi]			// st0:left  st1:magic
		fld		dword ptr [esi+4]		// st0:right st1:left  st2:magic
		fadd	st(0), st(2)			// st0:right+magic st1:left  st2:magic
		fxch							// st0:left st1:right+magic st2:magic
		fadd	st(0), st(2)			// st0:left+magic st1:right+magic st2:magic
		fxch							// st0:right+magic st1:left+magic st2:magic
		// 1st fadd
		//
		fstp	qword ptr righti		// st0:left+magic st1:magic
		fstp	qword ptr lefti			// st0:magic

		mov		ebx, dword ptr lefti
		mov		eax, dword ptr righti

		cmp     eax, 32767
		jg		clampmax2
		cmp     eax, -32768
		jl		clampmin2
		cmp     ebx, 32767
		jg		clampmax3
		cmp     ebx, -32768
		jl		clampmin3

		shl		eax, 16
		and		ebx, 00000FFFFh
		add		esi, 8
		or		eax, ebx
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop
		jmp		MixerFPU_Stereo16_Finished

	clampmax2:
		mov		eax, 07fff0000h
		nop
		cmp     ebx, 32767
		jg		clampmax4
		cmp     ebx, -32768
		jl		clampmin4

		and		ebx, 00000FFFFh
		add		esi, 8
		or		eax, ebx
		nop
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop
		jmp		MixerFPU_Stereo16_Finished

	clampmin2:
		mov		eax, 080000000h
		nop
		cmp     ebx, 32767
		jg		clampmax4
		cmp     ebx, -32768
		jl		clampmin4

		and		ebx, 00000FFFFh
		add		esi, 8
		or		eax, ebx
		nop
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop
		jmp		MixerFPU_Stereo16_Finished

	clampmax3:
		shl		eax, 16
		mov		ebx, 000007FFFh
		or		eax, ebx
		add		esi, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop
		jmp		MixerFPU_Stereo16_Finished

	clampmin3:
		shl		eax, 16
		mov		ebx, 000008000h
		or		eax, ebx
		add		esi, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop
		jmp		MixerFPU_Stereo16_Finished

	clampmax4:
		mov		ebx, 000007fffh
		add		esi, 8
		or		eax, ebx
		nop
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop
		jmp		MixerFPU_Stereo16_Finished

	clampmin4:
		mov		ebx, 000008000h
		add		esi, 8
		or		eax, ebx
		nop
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		ClipAndCopyLoop

	MixerFPU_Stereo16_Finished:	
		fstp	mix_temp1

	}
}
#endif

