/******************************************************************************/
/* MIXER_FPU_RAMP.H                                                           */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#ifndef _MIXER_FPU_RAMP_H
#define _MIXER_FPU_RAMP_H

#define FSOUND_VOLUMERAMP_STEPS	128		// at 44.1khz

void FSOUND_Mixer_FPU_Ramp(void *mixptr, int len, signed char returnaddress);

extern unsigned int mix_volumerampsteps;
extern float		mix_1overvolumerampsteps;

#endif