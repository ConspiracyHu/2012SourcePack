#ifndef __IntroInput__
#define __IntroInput__

#include "IntroEngine.h"

extern float status,oldstatus;

typedef void (WINAPI *LOADER_CALLBACK)(float f);

void LoadScene(unsigned char *data, scene **scn, tTexture **texlist, tTexture **lasttex, material **matlist, material **lastmat, int scnnum, LOADER_CALLBACK StatusBar);

#endif