#include "vectors.h"
#define pi 3.141592654f

vector3 v3_mults(vector3 v, float r)
{
	vector3 buf;
	buf.x = v.x * r;
	buf.y = v.y * r;
	buf.z = v.z * r;
	return buf;
}

float v3_len(vector3 v)
{
	return (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vector3 v3_make(float ax, float ay, float az)
{
	vector3 buf;
	buf.x = ax;
	buf.y = ay;
	buf.z = az;
	return buf;
}

vector3 v3_normalize(vector3 v)
{
	float l;
	l = v3_len(v);
	if (l != 0.0) return v3_mults(v, 1/l);
	return v3_make(0,0,0);
}

vector3 v3_add( vector3 a, vector3 b)
{
	vector3 buf;
	buf.x=a.x+b.x;
	buf.y=a.y+b.y;
	buf.z=a.z+b.z;
	return buf;
}

vector3 v3_sub( vector3 a, vector3 b)
{
	vector3 buf;
	buf.x=a.x-b.x;
	buf.y=a.y-b.y;
	buf.z=a.z-b.z;
	return buf;
}

vector3 v3_cross(vector3 b, vector3 c)
{
	vector3 buf;
	buf.x=(b.y * c.z) - (b.z * c.y);
	buf.y=(b.z * c.x) - (b.x * c.z);
	buf.z=(b.x * c.y) - (b.y * c.x);
	return buf;
}

float v3_dot(vector3 a, vector3 b)
{
 return a.x*b.x+a.y*b.y+a.z*b.z;
}


void m_identity(matrix &m)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			if (i==j) m[i][j]=1; else m[i][j]=0;
}

void m_mults(matrix m, float r, matrix &d)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			d[i][j] = m[i][j] * r;
}

void m_diadic3(vector3 v1, vector3 v2, matrix &m)
{
	m[0][0] = v1.x*v2.x; m[0][1] = v1.x*v2.y; m[0][2] = v1.x*v2.z; m[0][3] = 0.0;
	m[1][0] = m[0][1];   m[1][1] = v1.y*v2.y; m[1][2] = v1.y*v2.z; m[1][3] = 0.0;
	m[2][0] = m[0][2];   m[2][1] = m[1][2];   m[2][2] = v1.z*v2.z; m[2][3] = 0.0;
	m[3][0] = 0.0;       m[3][1] = 0.0;       m[3][2] = 0.0;       m[3][3] = 1.0;

}

void m_cross(vector3 v, matrix &m)
{
  m[0][0] = 0.0;  m[0][1] = -v.z; m[0][2] = v.y;  m[0][3] = 0.0;
  m[1][0] = v.z;  m[1][1] = 0.0;  m[1][2] = -v.x; m[1][3] = 0.0;
  m[2][0] = -v.y; m[2][1] = v.x;  m[2][2] = 0.0;  m[2][3] = 0.0;
  m[3][0] = 0.0;  m[3][1] = 0.0;  m[3][2] = 0.0;  m[3][3] = 1.0;
}

void m_add(matrix m1, matrix m2, matrix &m)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			m[i][j] = m1[i][j] + m2[i][j];
}

void m_mult(matrix m1, matrix m2, matrix &m)
{
	int i, j, k;
	float r;
	matrix t;
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

void m_xpose(float ax,float ay,float az,matrix &m)
{
	m_identity(m);
	m[0][3] = ax;
	m[1][3] = ay;
	m[2][3] = az;
}


void m_rotate(float ax,float ay,float az,float phi,matrix &m)
{
  matrix m1;
  vector a;

  a=v3_make(ax, ay, az);
  a=v3_normalize(a);

  m_identity(m);
  m_mults(m, (float)cos(phi), m);
  m_diadic3(a, a, m1);
  m_mults(m1, (float)(1-cos(phi)), m1);
  m_add(m, m1, m);

  m_cross(a, m1);
  m_mults(m1, (float)sin(phi), m1);
  m_add(m, m1, m);
  m[3][3] = 1.0;

}

void m_scale(float ax, float ay, float az, matrix &m)
{
  m_identity(m);
  m[0][0] = ax;
  m[1][1] = ay;
  m[2][2] = az;
}

void m_xformd(matrix m,vector3 v, vector3 &d)
{
 float _x, _y, _z;
 _x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
 _y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
 _z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
 d.x = _x;
 d.y = _y;
 d.z = _z;
}

void m_xform3(matrix m, vector3 v, vector3 &d)
{
 float _x, _y, _z;
 _x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
 _y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
 _z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
 d.x = _x;
 d.y = _y;
 d.z = _z;
}

float m_signedsubdet(matrix m, int i, int j)
{
	int x, y;
	float f, b, v;
	f = 0.0;
  for (x = 0;x<=2;x++)
  {
      v = 1.0;
      for (y = 0;y<=2;y++)
        v = v * m[(y+i+1) & 3][(((x+y) % 3) +j+1) & 3];
      f = f + v;
  }
  b = 0.0;
  for (x = 0;x<=2;x++)
  {
      v = 1.0;
      for (y = 0;y<=2;y++)
        v = v * m[(y+i+1) & 3][(((x-y+6) % 3) +j+1) & 3];
      b = b + v;
  }
  f = f - b;
  if ((i+j)%2 ==1) f = -f;
  return f;
}


float m_det(matrix m)
{
	return m[0][0] * m_signedsubdet(m, 0, 0) +
		m[0][1] * m_signedsubdet(m, 0, 1) +
		m[0][2] * m_signedsubdet(m, 0, 2) +
		m[0][3] * m_signedsubdet(m, 0, 3);
}

void m_zero(matrix &m)
{
	int i, j;
  for (i = 0; i<3; i++)
    for (j = 0; j<=3; j++)
      m[i][j] = 0.0;
}

void m_trans(matrix &m)
{
  int i, j;
  float t;
  for (i = 0;i<=3;i++)
    for (j = 0;j<=i-1;j++)
	{
        t = m[i][j];
        m[i][j] = m[j][i];
        m[j][i] = t;
    }
}

void m_invert(matrix m,matrix &dest)
{
	int i,j;
	float d;

	d = m_det(m);
  if (d == 0.0)
  {
      m_zero(dest);
      return;
  }
  for (i = 0;i<=3;i++)
    for (j = 0;j<=3;j++)
      dest[i][j] = m_signedsubdet(m, i, j);
  m_trans(dest);
  m_mults(dest, 1/d, dest);

}

void m_revxpose(const matrix m, vector3 &v)
{
	v.x=m[0][3];
	v.y=m[1][3];
	v.z=m[2][3];
}

void m_revscale(const matrix m, vector3 &v)
{
	v.x=m[0][0];
	v.y=m[1][1];
	v.z=m[2][2];
}

void m_revrotate(const matrix m, vector3 &v, float &phi)
{
	float c,s;
	v=v3_make(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]);
	s = v3_len(v);
	if (s == 0)
	{
		v=v3_make(0,0,0);
		phi = 0;
		return;
	}
	v=v3_mults(v,1/s);
	c = (m[0][0] + m[1][1] + m[2][2] - 1);
	if (c == 0) phi = pi/2.0f;
	  else
	  {
		  phi = (float)atan(s / c);
		  if (c < 0) phi += pi;
	  }
}