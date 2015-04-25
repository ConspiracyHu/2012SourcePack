#pragma once

#include "memtrack.h"

#include "GUIInterface.h"
#include "Texgen2.h"
#include "EffectTemplates.h"
#include "3dengine.h"
#include "IntroEditor.h"

#pragma pack(1)

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
extern VECTOR3 EyeShake,TargetShake;


void InitIntroEditorEffects();
void __stdcall NoEffect(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Layer2d(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall CalcParticle(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Clear(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall R2Txt(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Feedback(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Feedback2(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Blur(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Display3d(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall ParticleReset(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Contrast(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall EndScroll(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Crystalize(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Gridfiller(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall Cave(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall CamShake(DEFAULTEVENTDATA *DEData, void *EffectData);
void __stdcall MrcEffekt(DEFAULTEVENTDATA *DEData, void *EffectData);

#pragma pack()
