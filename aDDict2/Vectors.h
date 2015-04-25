#ifndef __VECTORS__
#define __VECTORS__

#include "memtrack.h"

#include <memory.h>
#include "math.h"

union VECTOR2
{
	float a[2];
	struct 	{ float x,y; };
	struct 	{ float u,v; };
};

union VECTOR3
{
	float a[3];
	struct 	{ float x,y,z; };
};

struct QUATERNION
{
	VECTOR3 v;
	float s;
};

struct RST
{
	VECTOR3 Pos;
	VECTOR3 Rot;
	VECTOR3 Scale;
	QUATERNION Quaternion;
};

typedef float MATRIX[4][4];

VECTOR3 V3_Make(float x, float y, float z);

VECTOR3 V3_Add(VECTOR3 a, VECTOR3 b);
VECTOR3 V3_Sub(VECTOR3 a, VECTOR3 b);
VECTOR3 V3_Mults(VECTOR3 a, float b);
VECTOR3 V3_Cross(VECTOR3 b, VECTOR3 c);
float   V3_Dot(VECTOR3 a, VECTOR3 b);

float V3_Length(VECTOR3 a);
VECTOR3 V3_Normalize(VECTOR3 a);

void AngleAxis2Quaternion(float x, float y, float z, float phi, QUATERNION &q);
QUATERNION Q_Slerp(QUATERNION q1,QUATERNION q2,float t);
QUATERNION Q_Mult(QUATERNION a,QUATERNION b);
QUATERNION Q_Normalize(QUATERNION a);

void M_Identity(MATRIX &m);

void M_XPose(float x, float y, float z, MATRIX &m);
void M_Scale(float x, float y, float z, MATRIX &m);
void M_Rotate(float ax,float ay,float az,float phi,MATRIX &m);
void M_Skew(float ax, float ay, float az, MATRIX &m);
void M_Trapez(float f, MATRIX &m);
void M_Quaternion(QUATERNION q,MATRIX &m);

void M_Mult(MATRIX m1, MATRIX m2, MATRIX &m);

void M_Xformd(MATRIX m, VECTOR3 v, VECTOR3 &d);
void M_Xform3(MATRIX m, VECTOR3 v, VECTOR3 &d);
void M_Invert(MATRIX &mat);
void M_Transpose(MATRIX &mat);

void M_RevXpose(const MATRIX m, VECTOR3 &v);
void M_RevScale(const MATRIX m, VECTOR3 &v);
void M_RevRotate(const MATRIX m, VECTOR3 &v, float &phi);

float LinearInterpolate(float Min, float Max, float Pos);
float BSplineInterpolate(float p1,float p2,float p3,float p4,float t);

float distRayPlane(VECTOR3 vRayOrigin,VECTOR3 vnRayVector,VECTOR3 vnPlaneNormal,float planeD);
void RST2Matrix(RST *a,MATRIX &m, bool kill);

float KillFloat(float a, int b);

#endif