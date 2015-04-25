#pragma once

#include "memtrack.h"

#include "aDDict2.h"
#include "GUIInterface.h"
#include "Texgen2.h"
#include "EffectTemplates.h"
#include "mvx.h"

struct EFFECT;

struct EVENT
{
	char Name[30];
	int Num;
	int StartFrame,EndFrame;
	int EventType;
	int Pass;
	int iParameters[10];
	float fParameters[10];
	int sx1,sy1,sx2,sy2;
	int ex1,ey1,ex2,ey2;
	EVENT *Next;
	EFFECT *Effect;
	void *EffectData;
	bool OnScreenInLastFrame;
	bool Selected;
	int GrabFrame;
	int GrabPass;
	int SF,EF,P;
};

struct RENDERTEXTURESLOT
{
	GLuint TexImage;
	float x1,y1,x2,y2;
};

extern RENDERTEXTURESLOT RenderTextures[6];

void __stdcall GUI_TimelinerLeftClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_TimelinerMidClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_TimelinerRightClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_TimelinerWheel(cINTERFACE* Interface, int ButtonId);

void DisplayTimeliner(GUIITEM *I);
void DisplayViewPort(GUIITEM *I);
void BuildIntroEditorGUI(cINTERFACE *IntroEditor);
void ResetWorldParticles();

extern EVENT *SelectedEvent,*EventList;

float Linear(float a, float b, float t);
extern GUIITEM *MainDisplay;
void SetOrthoMode();

extern cINTERFACE *EventParameterInterface;
extern bool CamInfo;

void ArrangeEventList();
void __stdcall GUI_ViewPortLeftClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_ViewPortMidClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_ViewPortWheel(cINTERFACE* Interface, int ButtonId);
