#pragma once
#include "vectors.h"
#include "3dEngine.h"

class SCENE;

struct PARTICLE
{
	VECTOR3 Position,LastPos,DisplayPos,DisplayPos2;
	VECTOR3 Speed;
	int TailCnt,TailCnt2;
	int Age,StartAge;
	bool Aging;
	bool Active;
	unsigned char Color1[5],Color2[5];
	VECTOR3 *Tail;
	float Rotation,RotChaos;
	float Size;
};

struct PARTICLEVECTOR
{
	//GL_T2F_C4UB_V3F
	float u,v;
	RGBA col;
	VECTOR3 Pos;
};

struct EMITTERDATA
{
	int DefaultAge,AgeChaos;
	float ParticlesPerFrame;
	float ParticleNumBuffer;
	float d;
	VECTOR3 n;

	PARTICLE *Particles;
	int MaxParticles;

	int LastFrameChecked;	

	bool Head,Tail,ObjectHead;
	unsigned char TailLength,TailLength2;
	SCENE *ObjectHeadScene;
	int ObjectHeadSceneID;
	GLuint HeadMaterial;
	float Color1[5], Color2[5];
	float Param1, Param2;
	float Size;

	float CamFade,Rotspeed,RotChaos;
	float Speed,SpeedChaos,DirChaos;
	VECTOR3 Dir;

	bool RandRotate,FixedUp;
};

struct PARTICLESORTINFO
{
	float Distance;
	int ParticleID;
};
