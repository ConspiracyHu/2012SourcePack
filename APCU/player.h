#include <windows.h>

#ifndef __PLAYER_H__
#define __PLAYER_H__
typedef void (WINAPI *ZAJCALLBACK) (float);

extern ZAJCALLBACK cnsSynth_LoaderCallback;

int cnsSynth_LoadMusic();
void cnsSynth_PlayMusic();
int cnsSynth_GetSync();

#endif
