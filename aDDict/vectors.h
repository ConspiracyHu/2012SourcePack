#ifndef __vectors__
#define __vectors__

#include <memory.h>
#include <math.h>

typedef float matrix[4][4];

union vector2
{
    float a[2];
	struct {
		float x,y;
	}b;
};

union vector3
{
    float a[3];
	struct {
		float x,y,z;
	}b;
};

union vector4
{
    float a[4];
	struct {
		float x,y,z,w;
	}b;
};


struct quaternion
{
	float w;
	vector3 a;
};

typedef vector3 vector;


void v3_mults(vector3 v, float r, vector3 &d);
float v3_len(vector3 v);


void v3_make(float ax, float ay, float az , vector3 &d);
void v3_normalize(vector3 v, vector3 &d);

void m_mults(matrix m, float r, matrix &d);
void m_diadic3(vector3 v1, vector3 v2, matrix &m);
void m_cross(vector3 v, matrix &m);
void m_add(matrix m1, matrix m2, matrix &m);


void m_identity(matrix &m);
void m_mult(matrix m1, matrix m2, matrix &m);
void m_xpose(float ax,float ay,float az,matrix &m);
void m_rotate(float ax,float ay,float az,float phi,matrix &m);
void m_scale(float ax, float ay, float az, matrix &m);
void m_invert(matrix &mat);
void m_invert(matrix m,matrix &dest);

vector3 vect_osszeadas( vector3 a, vector3 b);
vector3 vect_kivonas( vector3 a, vector3 b);
vector3 egyseg( vector3 a);
void m_xformd(matrix m,vector3 v, vector3 &d);
void m_xform3(matrix m, vector3 v, vector3 &d);

vector3 cross(vector3 b, vector3 c);
vector3 vect_skalarszorzas(vector3 b, float a);
float dotproduct(vector3 a, vector3 b);

void m_revxpose(const matrix m, vector3 &v);
void m_revscale(const matrix m, vector3 &v);
void m_revrotate(const matrix m, vector3 &v, float &phi);

quaternion axisangle2quaternion(vector3 axis, float angle);
void quaternion2axisangle(quaternion q, vector3 &axis, float &angle);
quaternion quaternionmult(quaternion a, quaternion b);

#endif