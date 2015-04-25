#pragma once

#include "Texgen.h"
#include "3dengine.h"
#include "WindowHandler.h"
extern MATERIAL *MaterialList;

#pragma pack(1)

struct DEFAULTEVENTDATA
{
	float TimePos;
	int x1,y1,x2,y2;
	int StartFrame;
	bool OnScreenInLastFrame;
	//bool SecondPass;
	int CurrentFrame;
	float Aspect;
};

struct RENDERTEXTURESLOT
{
	GLuint TexImage;
	float x1,y1,x2,y2;
};

struct LAYER2DTEMPLATE
{
	RGBA StartCol;
	RGBA EndCol;
	GLuint SrcBlend,DstBlend;
	bool Textured;
	MATERIAL *Texture;
};

struct PARTICLETEMPLATE
{
	WORLD *World;
	int   CamID;
	int   AnimID;
	float AnimStart,AnimEnd;
};

struct CLEARTEMPLATE
{
	RGBA Col;
	bool Screen,Zbuffer;
};

struct R2TXTTEMPLATE
{
	char RenderTexture;
};

struct FEEDBACKTEMPLATE
{
	RGBA StartCol;
	RGBA EndCol;
	GLuint SrcBlend,DstBlend;
	char RenderTexture;
	unsigned char LayerNum;
	unsigned char LayerZoom;
};

struct BLURFXTEMPLATE
{
	RGBA StartCol;
	RGBA EndCol;
	GLuint SrcBlend,DstBlend;
	char RenderTexture;
	unsigned char LayerNum;
	unsigned char LayerZoom;
};

struct CONTRASTFXTEMPLATE
{
	RGBA StartCol;
	RGBA EndCol;
	GLuint SrcBlend,DstBlend;
	char RenderTexture;
	unsigned char MulAmount;
	unsigned char AddAmount;
};

struct DISPLAY3DTEMPLATE
{
	WORLD *World;
	int   CamID;
	int   AnimID;
	float CamStart,CamEnd;
	float AnimStart,AnimEnd;
	bool DontSaveCam;
};

struct PARTICLERESETTEMPLATE
{
	WORLD *World;
};

struct CRYSTALIZEFXTEMPLATE
{
	int CrystalNum;
	unsigned char RandSeed;
	unsigned char Detail;
	float sCrystalSize,eCrystalSize;
	float AnimChaos;
	unsigned char aStart,aEnd;
	unsigned char AlternateAnim;

	GLuint SrcBlend,DstBlend;

	char RenderTexture;
};

struct GRIDFILLERTEMPLATE
{
	RGBA StartCol;
	RGBA EndCol;
	GLuint SrcBlend,DstBlend;

	float Repeat;
	float XSin[4];
	float YSin[4];
	float ZSin[4];
	float WSin[4];

	bool Shade;
	bool Textured;

	MATERIAL *Texture;
};

struct CAVETEMPLATE
{
	unsigned char RandSeed;
	float Fov;
	unsigned char r,g,b;

	unsigned char Data[9];

	bool Solid,WireFrame,Lighting;

	unsigned char fps,skip;

	bool Textured;
	MATERIAL *Texture;
};

struct SHAKETEMPLATE
{
	float Eye,Target,Start,End;
	unsigned char Freq;
};

struct MRCTEMPLATE
{
	RGBA StartCol;
	RGBA EndCol;
	GLuint SrcBlend,DstBlend;

	unsigned char EffectID;
	float StartTime,EndTime;
	unsigned char RenderTexture;
};

extern int time; // cave time


#pragma pack()

typedef void (WINAPI *EFFECTCALLBACK) (DEFAULTEVENTDATA*, void*);

void InitIntroEditorEffects();
#ifdef INCLUDE_EVENT_NOEFFECT
void __stdcall NoEffect(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_2DLAYER
void __stdcall Layer2d(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_PARTICLECALC
void __stdcall CalcParticle(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_CLEAR
void __stdcall Clear(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif;

#ifdef INCLUDE_EVENT_RENDERTOTEXTURE
void __stdcall R2Txt(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_FEEDBACK
void __stdcall Feedback(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_BLUR
void __stdcall Blur(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_3DSCENE
void __stdcall Display3d(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_PARTICLERESET
void __stdcall ParticleReset(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_CONTRAST
void __stdcall Contrast(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_FUNKENDSCROLL
#include "writer.h"
extern CWriter *EndScrollText;
void __stdcall EndScroll(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_CRYSTALIZE
void __stdcall Crystalize(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_GRIDFILLER
void InitGrid();
void __stdcall Gridfiller(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

extern int LastTime;

#ifdef INCLUDE_EVENT_CAVE
void InitTunnels(int Rands, float a, float b, float c,float d,float e, float f, float g, float h, float radius);
void CalculateCave(bool,bool,int);
void __stdcall Cave(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_CAMSHAKE
void __stdcall CamShake(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_MRCEFFECT
void __stdcall MrcEffekt(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

#ifdef INCLUDE_EVENT_FEEDBACK2
void __stdcall Feedback2(DEFAULTEVENTDATA *DEData, void *EffectData);
#endif

extern VECTOR3 EyeShake,TargetShake;
