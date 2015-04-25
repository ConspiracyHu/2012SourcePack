#pragma once

#define aDDict_NONE 0
#define aDDict_LINEAR 1
#define aDDict_HERMITE 2
#define aDDict_BEZIER 3
#define aDDict_CATMULL 4
#define aDDict_SINE 5
#define aDDict_SAW 6
#define aDDict_SQUARE 7

struct Key
{
	float Value;
	float Time;
	float an,bn;
};

class SPLINE
{
public:
	int KeyNum;
	int KeyCapacity;
	Key *Keys;

	bool Loop;
	float LoopStart,LoopEnd;
	int Interpolation;

	//void AddKey(float Value, float Time);
	float GetInterpolatedValue(float Time);
	void InitVectors();
	float GetVector(unsigned char sel, int n);
};