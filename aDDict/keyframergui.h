#ifndef __keyframergui__
#define __keyframergui__

#include "addict.h"
#include "modellgui.h"

extern objectanimation *actualanim;
extern cameraanimation *actualcameraanim;
extern camera actualcamera;
extern int passcounter;

objectanimation *findobjanim(object *o, int animnum);
void countselectcenter();
void initkeyframergui();
void keyframergui(int mode);

#endif