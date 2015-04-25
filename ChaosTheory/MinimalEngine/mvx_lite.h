/*********************************************************************

                          #   # #   # #   #
                          ## ## #   #  # # 
                          # # # #   #   # 
                          #   #  # #   # # 
                          #   #   #   #   #

                  REALTIME BUZZ-BASED SYNTHESIZER

        Written by Gargaj/Conspiracy ^ Ümlaüt Design ^ CoolPHat

  "You've successfully proven that you can write a working softsynth, 
   now prove that you can write a good one :)"
                                                  /kb^farbrausch/

**********************************************************************/

#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <dsound.h>
#pragma comment(lib,"dsound.lib")

extern int mvxSystem_SongLength; // in ticks
extern int mvxSystem_SamplesPerTick;

#define stereofloat float

// MIXING ROUTINES
void mvxMixer_Init();
void mvxMixer_Render(stereofloat *,int);
void mvxMixer_DeInit();

// SYSTEM
void mvxSystem_ClipBuffer(float * source, short * dest, int nums);
int mvxSystem_Init(HWND,char*,int);
void mvxSystem_Play();
int mvxSystem_GetSync();
void mvxSystem_Stop();
void mvxSystem_DeInit();
