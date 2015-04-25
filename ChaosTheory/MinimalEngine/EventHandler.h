#pragma once

#include "WindowHandler.h"
#include "TexGen.h"
#include "3DEngine.h"
#include "IntroEditorEffects.h"

struct EVENT
{
	int StartFrame,EndFrame;
	int EventType;
	int Pass;
	int sx1,sy1,sx2,sy2;
	int ex1,ey1,ex2,ey2;
	void *EffectData;
	bool OnScreenInLastFrame;
	EFFECTCALLBACK Effect;
#ifdef MINIMALTWEAKER
	bool Changed;
#endif
};

#ifdef MINIMALTWEAKER
extern int MaxPass;
#endif

extern float AspectRatio;
extern bool CropAspect;

void glErrorCheck();
void DisplayFrame(int Frame, MATERIAL *MaterialList, SCENE *SceneList, WORLD *WorldList, EVENT *EventList, int EventNum);
