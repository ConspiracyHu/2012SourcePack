#ifndef __vectors__
#define __vectors__

#include "math.h"
#include <memory.h>

union vector2
{
	float a[2];
	struct { float u,v; };
};

union vector3
{
	float a[3];
	struct { float x,y,z; };
};

union vector4
{
	float a[4];
	struct { float x,y,z,w; };
};

typedef float matrix[4][4];
typedef vector3 vector;

vector3 v3_mults(vector3 v, float r);
vector3 v3_make(float ax, float ay, float az);
vector3 v3_normalize(vector3 v);
vector3 v3_add( vector3 a, vector3 b);
vector3 v3_sub( vector3 a, vector3 b);
vector3 v3_cross(vector3 b, vector3 c);
float   v3_len(vector3 v);
float   v3_dot(vector3 a, vector3 b);

void m_identity(matrix &m);
void m_mults(matrix m, float r, matrix &d);
void m_diadic3(vector3 v1, vector3 v2, matrix &m);
void m_cross(vector3 v, matrix &m);
void m_add(matrix m1, matrix m2, matrix &m);
void m_mult(matrix m1, matrix m2, matrix &m);
void m_xpose(float ax,float ay,float az,matrix &m);
void m_rotate(float ax,float ay,float az,float phi,matrix &m);
void m_scale(float ax, float ay, float az, matrix &m);
void m_xformd(matrix m,vector3 v, vector3 &d);
void m_xform3(matrix m, vector3 v, vector3 &d);
void m_zero(matrix &m);
void m_trans(matrix &m);
void m_invert(matrix m,matrix &dest);
void m_revxpose(const matrix m, vector3 &v);
void m_revscale(const matrix m, vector3 &v);
void m_revrotate(const matrix m, vector3 &v, float &phi);
float m_signedsubdet(matrix m, int i, int j);
float m_det(matrix m);

#endif