#include "Vectors.h"
#include <windows.h>

VECTOR3 V3_Make(float x, float y, float z)
{
	VECTOR3 Buffer;
	Buffer.x=x;
	Buffer.y=y;
	Buffer.z=z;
	return Buffer;
}

VECTOR3 V3_Add(VECTOR3 a, VECTOR3 b)
{
	VECTOR3 Buffer;
	Buffer.x=a.x+b.x;
	Buffer.y=a.y+b.y;
	Buffer.z=a.z+b.z;
	return Buffer;
}

VECTOR3 V3_Sub(VECTOR3 a, VECTOR3 b)
{
	VECTOR3 Buffer;
	Buffer.x=a.x-b.x;
	Buffer.y=a.y-b.y;
	Buffer.z=a.z-b.z;
	return Buffer;
}

VECTOR3 V3_Mults(VECTOR3 a, float b)
{
	VECTOR3 Buffer;
	Buffer.x=a.x*b;
	Buffer.y=a.y*b;
	Buffer.z=a.z*b;
	return Buffer;
}

VECTOR3 V3_Cross(VECTOR3 b, VECTOR3 c)
{
	VECTOR3 buf;
	buf.x=(b.y * c.z) - (b.z * c.y);
	buf.y=(b.z * c.x) - (b.x * c.z);
	buf.z=(b.x * c.y) - (b.y * c.x);
	return buf;
}

float V3_Dot(VECTOR3 a, VECTOR3 b)
{
 return a.x*b.x+a.y*b.y+a.z*b.z;
}

float V3_Length(VECTOR3 a)
{
	return (float)sqrt((a.x*a.x+a.y*a.y+a.z*a.z));
}

VECTOR3 V3_Normalize(VECTOR3 a)
{
	float l=V3_Length(a);
	return V3_Mults(a,1/l);
}

void M_Mults(MATRIX m, float r, MATRIX &d)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			d[i][j] = m[i][j] * r;
}

void M_Diadic3(VECTOR3 v1, VECTOR3 v2, MATRIX &m)
{
	m[0][0] = v1.x*v2.x; m[0][1] = v1.x*v2.y; m[0][2] = v1.x*v2.z; m[0][3] = 0.0;
	m[1][0] = m[0][1];   m[1][1] = v1.y*v2.y; m[1][2] = v1.y*v2.z; m[1][3] = 0.0;
	m[2][0] = m[0][2];   m[2][1] = m[1][2];   m[2][2] = v1.z*v2.z; m[2][3] = 0.0;
	m[3][0] = 0.0;       m[3][1] = 0.0;       m[3][2] = 0.0;       m[3][3] = 1.0;

}

void M_Cross(VECTOR3 v, MATRIX &m)
{
  m[0][0] = 0.0;  m[0][1] = -v.z; m[0][2] = v.y;  m[0][3] = 0.0;
  m[1][0] = v.z;  m[1][1] = 0.0;  m[1][2] = -v.x; m[1][3] = 0.0;
  m[2][0] = -v.y; m[2][1] = v.x;  m[2][2] = 0.0;  m[2][3] = 0.0;
  m[3][0] = 0.0;  m[3][1] = 0.0;  m[3][2] = 0.0;  m[3][3] = 1.0;
}

void M_Add(MATRIX m1, MATRIX m2, MATRIX &m)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			m[i][j] = m1[i][j] + m2[i][j];
}

void M_Identity(MATRIX &m)
{
	memset(m,0,sizeof(MATRIX));
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			if (x==y) m[x][y]=1;
}

void M_XPose(float x,float y,float z,MATRIX &m)
{
	M_Identity(m);
	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;
}

void M_Scale(float x, float y, float z, MATRIX &m)
{
	M_Identity(m);
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
}

void M_Rotate(float ax,float ay,float az,float phi,MATRIX &m)
{
  MATRIX m1;
  VECTOR3 a;

  if (ax==0 && ay==0 && az==0) {M_Identity(m); return;}

  a=V3_Make(ax, ay, az);
  a=V3_Normalize(a);

  M_Identity(m);
  M_Mults(m, (float)cos(phi), m);
  M_Diadic3(a, a, m1);
  M_Mults(m1, (float)(1-cos(phi)), m1);
  M_Add(m, m1, m);

  M_Cross(a, m1);
  M_Mults(m1, (float)sin(phi), m1);
  M_Add(m, m1, m);
  m[3][3] = 1.0;

}

/*void M_Skew(float ax, float ay, float az, MATRIX &m)
{
	M_Identity(m);
	m[0][0]=1;   m[1][0]=0; m[2][0]=0;
	m[0][1]=0;  m[1][1]=1;   m[2][1]=0;
	m[0][2]=-ax; m[1][2]=ay;  m[2][2]=1;
}*/

void M_Mult(MATRIX m1, MATRIX m2, MATRIX &m)
{
	int i, j, k;
	float r;
	MATRIX t;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
		{
			r = 0.0;
			for (k=0;k<4;k++)
				r = r + m1[i][k] * m2[k][j];
			t[i][j] = r;
		}
	memcpy(&m,&t,sizeof(t));
}

void M_Xformd(MATRIX m, VECTOR3 v, VECTOR3 &d)
{
 float _x, _y, _z;
 _x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
 _y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
 _z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
 d.x = _x;
 d.y = _y;
 d.z = _z;
}

void M_Xform3(MATRIX m, VECTOR3 v, VECTOR3 &d)
{
 float _x, _y, _z;
 _x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
 _y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
 _z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
 d.x = _x;
 d.y = _y;
 d.z = _z;
}

void M_Invert(MATRIX &mat)
{
	float di,d;
    di= mat[0][0];
    mat[0][0]=d=1.0f/di;
    mat[1][0]*=-d;
    mat[2][0]*=-d;
    mat[3][0]*=-d;
    mat[0][1]*=d;
    mat[0][2]*=d;
    mat[0][3]*=d;
    mat[1][1]+=mat[1][0]*mat[0][1]*di;
    mat[1][2]+=mat[1][0]*mat[0][2]*di;
    mat[1][3]+=mat[1][0]*mat[0][3]*di;
    mat[2][1]+=mat[2][0]*mat[0][1]*di;
    mat[2][2]+=mat[2][0]*mat[0][2]*di;
    mat[2][3]+=mat[2][0]*mat[0][3]*di;
    mat[3][1]+=mat[3][0]*mat[0][1]*di;
    mat[3][2]+=mat[3][0]*mat[0][2]*di;
    mat[3][3]+=mat[3][0]*mat[0][3]*di;
    di=mat[1][1];
    mat[1][1]=d=1.0f/di;
    mat[0][1]*=-d;
    mat[2][1]*=-d;
    mat[3][1]*=-d;
    mat[1][0]*=d;
    mat[1][2]*=d;
    mat[1][3]*=d;
    mat[0][0]+=mat[0][1]*mat[1][0]*di;
    mat[0][2]+=mat[0][1]*mat[1][2]*di;
    mat[0][3]+=mat[0][1]*mat[1][3]*di;
    mat[2][0]+=mat[2][1]*mat[1][0]*di;
    mat[2][2]+=mat[2][1]*mat[1][2]*di;
    mat[2][3]+=mat[2][1]*mat[1][3]*di;
    mat[3][0]+=mat[3][1]*mat[1][0]*di;
    mat[3][2]+=mat[3][1]*mat[1][2]*di;
    mat[3][3]+=mat[3][1]*mat[1][3]*di;
    di=mat[2][2];
    mat[2][2]=d=1.0f/di;
    mat[0][2]*=-d;
    mat[1][2]*=-d;
    mat[3][2]*=-d;
    mat[2][0]*=d;
    mat[2][1]*=d;
    mat[2][3]*=d;
    mat[0][0]+=mat[0][2]*mat[2][0]*di;
    mat[0][1]+=mat[0][2]*mat[2][1]*di;
    mat[0][3]+=mat[0][2]*mat[2][3]*di;
    mat[1][0]+=mat[1][2]*mat[2][0]*di;
    mat[1][1]+=mat[1][2]*mat[2][1]*di;
    mat[1][3]+=mat[1][2]*mat[2][3]*di;
    mat[3][0]+=mat[3][2]*mat[2][0]*di;
    mat[3][1]+=mat[3][2]*mat[2][1]*di;
    mat[3][3]+=mat[3][2]*mat[2][3]*di;
    di=mat[3][3];
    mat[3][3]=d=1.0f/di;
    mat[0][3]*=-d;
    mat[1][3]*=-d;
    mat[2][3]*=-d;
    mat[3][0]*=d;
    mat[3][1]*=d;
    mat[3][2]*=d;
    mat[0][0]+=mat[0][3]*mat[3][0]*di;
    mat[0][1]+=mat[0][3]*mat[3][1]*di;
    mat[0][2]+=mat[0][3]*mat[3][2]*di;
    mat[1][0]+=mat[1][3]*mat[3][0]*di;
    mat[1][1]+=mat[1][3]*mat[3][1]*di;
    mat[1][2]+=mat[1][3]*mat[3][2]*di;
    mat[2][0]+=mat[2][3]*mat[3][0]*di;
    mat[2][1]+=mat[2][3]*mat[3][1]*di;
    mat[2][2]+=mat[2][3]*mat[3][2]*di;
}

void M_Transpose(MATRIX &mat)
{
	float b;
	for (int x=0; x<4; x++)
		for (int y=x; y<4; y++)
		{
			b=mat[x][y];
			mat[x][y]=mat[y][x];
			mat[y][x]=b;
		}
}

/*void M_RevXpose(const MATRIX m, VECTOR3 &v)
{
	v.x=m[0][3];
	v.y=m[1][3];
	v.z=m[2][3];
}

void M_RevScale(const MATRIX m, VECTOR3 &v)
{
	v.x=m[0][0];
	v.y=m[1][1];
	v.z=m[2][2];
}

void M_RevRotate(const MATRIX m, VECTOR3 &v, float &phi)
{
	float c,s;
	v=V3_Make(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]);
	s = V3_Length(v);
	if (s == 0)
	{
		v=V3_Make(0,0,0);
		phi = 0;
		return;
	}
	v=V3_Mults(v,1/s);
	c = (m[0][0] + m[1][1] + m[2][2] - 1);
	if (c == 0) phi = 3.1415f/2.0f;
	  else
	  {
		  phi = (float)atan(s / c);
		  if (c < 0) phi += 3.1415f;
	  }
}*/

void M_Quaternion(QUATERNION q,MATRIX &m)
{
	M_Identity(m);
	m[0][0]=1-2*q.v.y*q.v.y-2*q.v.z*q.v.z;
	m[0][1]=  2*q.v.x*q.v.y+2*  q.s*q.v.z;
	m[0][2]=  2*q.v.x*q.v.z-2*  q.s*q.v.y;

	m[1][0]=  2*q.v.x*q.v.y-2*  q.s*q.v.z;
	m[1][1]=1-2*q.v.x*q.v.x-2*q.v.z*q.v.z;
	m[1][2]=  2*q.v.y*q.v.z+2*  q.s*q.v.x;

	m[2][0]=  2*q.v.x*q.v.z+2*  q.s*q.v.y;
	m[2][1]=  2*q.v.y*q.v.z-2*  q.s*q.v.x;
	m[2][2]=1-2*q.v.x*q.v.x-2*q.v.y*q.v.y;
}


float LinearInterpolate(float Min, float Max, float Pos)
{
	return Min+(Max-Min)*Pos;
}


/*float distRayPlane(VECTOR3 vRayOrigin,
                   VECTOR3 vnRayVector,
                   VECTOR3 vnPlaneNormal,
                   float planeD)
{
    float cosAlpha;
    float deltaD;


    cosAlpha = V3_Dot( 
        vnRayVector ,  vnPlaneNormal );

    // parallel to the plane (alpha=90)
    if (cosAlpha==0) return -1.0f;


    deltaD = planeD - 
        V3_Dot(vRayOrigin,vnPlaneNormal);
    
    return (deltaD/cosAlpha);
}*/

void AngleAxis2Quaternion(float x, float y, float z, float phi, QUATERNION &q)
{
  q.s = (float)cos(phi/2.0f);
  q.v.x = x * (float)sin(phi/2.0f);
  q.v.y = y * (float)sin(phi/2.0f);
  q.v.z = z * (float)sin(phi/2.0f);
}

QUATERNION Q_Add(QUATERNION a,QUATERNION b)
{
	QUATERNION c=a;
	c.s=a.s+b.s;
	c.v.x=a.v.x+b.v.x;
	c.v.y=a.v.y+b.v.y;
	c.v.z=a.v.z+b.v.z;
	return c;
}

QUATERNION Q_Mults(QUATERNION a,float b)
{
	QUATERNION c=a;
	c.s=a.s*b;
	c.v.x=a.v.x*b;
	c.v.y=a.v.y*b;
	c.v.z=a.v.z*b;
	return c;
}

QUATERNION Q_Mult(QUATERNION a,QUATERNION b)
{
	QUATERNION q;
	q.s  =a.s*b.s   - a.v.x*b.v.x - a.v.y*b.v.y - a.v.z*b.v.z;
	q.v.x=a.s*b.v.x + a.v.x*b.s   + a.v.y*b.v.z - a.v.z*b.v.y;
	q.v.y=a.s*b.v.y + a.v.y*b.s   + a.v.z*b.v.x - a.v.x*b.v.z;
	q.v.z=a.s*b.v.z + a.v.z*b.s   + a.v.x*b.v.y - a.v.y*b.v.x;
	return q;

}

QUATERNION Q_Normalize(QUATERNION q) 
{
	float d=(float)sqrt(q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z + q.s*q.s);
	QUATERNION r=Q_Mults(q,1.0f/d);
	return r;
}

float Q_DotProduct(QUATERNION q1,QUATERNION q2) 
{
	return (q1.s  *q2.s)+
	       (q1.v.x*q2.v.x)+
	       (q1.v.y*q2.v.y)+
	       (q1.v.z*q2.v.z);

}

QUATERNION Q_Slerp(QUATERNION q1,QUATERNION q2,float t) 
{
	float a=Q_DotProduct(q1,q2);

	a=(float)acos(a);

	float c1,c2;
	if (a<1) 
	{
		c1=1-t;
		c2=t;
	}
	else 
	{
		c1=(float)(sin((1-t)*a)/sin(a)); 
		c2=(float)(sin(   t *a)/sin(a));
	}

	return Q_Normalize(Q_Add(Q_Mults(q1,c1),Q_Mults(q2,c2)));
}

void RST2Matrix(RST *a,MATRIX &m)
{
	MATRIX b;
	M_Scale(a->Scale.x,a->Scale.y,a->Scale.z,m);
	M_Quaternion(a->Quaternion,b);
	M_Mult(b,m,m);
	M_XPose(a->Pos.x,a->Pos.y,a->Pos.z,b);
	M_Mult(b,m,m);
}

//GeCCo_Rand rnd;

float Random()
{
	return (rand()/float(RAND_MAX)-0.5f);
}

float BSplineInterpolate(float c0, float c1, float c2, float c3, float u)
{
	float u2 = u*u, u3 = u*u*u;
	float b0 = u3/6.0f;
	float b1 = (1+3*u+3*u2-3*u3)/6.0f;
	float b2 = (4-6*u2+3*u3)/6.0f;
	float b3 = (1-3*u+3*u2-u3)/6.0f;
	return(c0*b3+c1*b2+c2*b1+c3*b0);
}

