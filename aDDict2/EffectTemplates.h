#pragma once

#include "memtrack.h"

#include "texgen2.h"
#include "GUIInterface.h"
#include "IntroEditor.h"

#define EFFECT_BYTE 0
#define EFFECT_INT 1
#define EFFECT_FLOAT 2
#define EFFECT_RTXT 3
#define EFFECT_VARFUNCTION 4

#define EFFECT_BOOL 5
#define EFFECT_MATSEL 6
#define EFFECT_BLENDSEL 7
#define EFFECT_SCENESEL 8
#define EFFECT_ANIMSEL 9
#define EFFECT_CAMSEL 10
#define EFFECT_CNSTFUNCTION 11
#define EFFECT_VARTEXT 12
#define EFFECT_CONSTTEXT 13

struct EVENT;

typedef void (WINAPI *EFFECTFUNCTIONCALLBACK) (EVENT*);

struct EFFECTPARAMETERTYPE
{
	int Type;
	int iMin,iMax,iDefault;
	float fMin,fMax,fDefault;
	char *Text,*ToolTip;
	int Color;
	GUICALLBACK Function;
};


class EFFECTTEMPLATEDESCRIPTOR
{
	void AddItem();
public:
	int ItemNum;
	int ItemCapacity;
	EFFECTPARAMETERTYPE *ItemList;
	EFFECTTEMPLATEDESCRIPTOR() {ItemNum=0; ItemCapacity=0; ItemList=NULL;}
	~EFFECTTEMPLATEDESCRIPTOR() {delete ItemList;}

	void AddByteBar(unsigned char Min, unsigned char Max, unsigned char Default, int Color, char *Text, char *ToolTip);    // byte
	void AddIntBar(int Min, int Max, int Default, int Color, char *Text, char *ToolTip);    // int
	void AddFloatBar(float Min, float Max, float Default, int Color, char *Text, char *ToolTip);    // float
	void AddRenderTextureSelector();                      //byte
	void AddBooleanButton(int t1, int t2, bool Default, char *Text, char *ToolTip);    // byte
	void AddVariableFunction(int t1, int t2, char *Text, char * ToolTip, GUICALLBACK Function);    // none
	void AddVariableText(int Pos, char *Text);
	void AddConstantText(int Pos, char *Text);

	void AddMaterialSelector();                           //void*
	void AddBlendModeSelector();                          //GLuint*2
	void AddSceneSelector();                              //void*
	void AddAnimSelector();                               //void*
	void AddCameraSelector();                             //void*
	void AddConstantFunction(int t1, int t2, char *Text, char * ToolTip,  GUICALLBACK Function);    // none
};

struct DEFAULTEVENTDATA
{
	float TimePos;
	int x1,y1,x2,y2;
	bool OnScreenInLastFrame;
	bool SecondPass;
	int ActualFrame;
};

typedef void (WINAPI *EFFECTCALLBACK) (DEFAULTEVENTDATA*, void*);

struct EFFECT
{
	EFFECTTEMPLATEDESCRIPTOR *TemplateDescriptor;
	EFFECTCALLBACK EffectProc;
	float r,g,b;
	int DataSize;
	int Type;
	ITEM *ListItem;
};

extern ITEMLIST *EffectList;
extern cINTERFACE *EventParameterInterface;

void RegisterEffects();
void AdjustGUIToEvent(EVENT *Event);

