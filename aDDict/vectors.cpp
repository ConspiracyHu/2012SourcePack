#include "vectors.h"
#define pi 3.141592654f

void v3_mults(vector3 v, float r, vector3 &d)
{
	d.b.x = v.b.x * r;
	d.b.y = v.b.y * r;
	d.b.z = v.b.z * r;
}

float v3_len(vector3 v)
{
	return (float)sqrt(v.b.x*v.b.x + v.b.y*v.b.y + v.b.z*v.b.z);
}

void v3_make(float ax, float ay, float az , vector3 &d)
{
	d.b.x = ax;
	d.b.y = ay;
	d.b.z = az;
}

void v3_normalize(vector3 v, vector3 &d)
{
	float l;
	l = v3_len(v);
	if (l != 0.0) v3_mults(v, 1/l, d);
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
	m[0][0] = v1.b.x*v2.b.x; m[0][1] = v1.b.x*v2.b.y; m[0][2] = v1.b.x*v2.b.z; m[0][3] = 0.0;
	m[1][0] = m[0][1];   m[1][1] = v1.b.y*v2.b.y; m[1][2] = v1.b.y*v2.b.z; m[1][3] = 0.0;
	m[2][0] = m[0][2];   m[2][1] = m[1][2];   m[2][2] = v1.b.z*v2.b.z; m[2][3] = 0.0;
	m[3][0] = 0.0;       m[3][1] = 0.0;       m[3][2] = 0.0;       m[3][3] = 1.0;

}

void m_cross(vector3 v, matrix &m)
{
  m[0][0] = 0.0;  m[0][1] = -v.b.z; m[0][2] = v.b.y;  m[0][3] = 0.0;
  m[1][0] = v.b.z;  m[1][1] = 0.0;  m[1][2] = -v.b.x; m[1][3] = 0.0;
  m[2][0] = -v.b.y; m[2][1] = v.b.x;  m[2][2] = 0.0;  m[2][3] = 0.0;
  m[3][0] = 0.0;  m[3][1] = 0.0;  m[3][2] = 0.0;  m[3][3] = 1.0;
}

void m_add(matrix m1, matrix m2, matrix &m)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			m[i][j] = m1[i][j] + m2[i][j];
}

void m_identity(matrix &m)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			if (i==j) m[i][j]=1; else m[i][j]=0;
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

  if (ax==0 && ay==0 && az==0) {m_identity(m); return;}

  v3_make(ax, ay, az, a);
  v3_normalize(a, a);

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

vector3 vect_osszeadas( vector3 a, vector3 b)
{
	vector3 buf;
	buf.b.x=a.b.x+b.b.x;
	buf.b.y=a.b.y+b.b.y;
	buf.b.z=a.b.z+b.b.z;
	return buf;
}

vector3 vect_kivonas( vector3 a, vector3 b)
{
	vector3 buf;
	buf.b.x=a.b.x-b.b.x;
	buf.b.y=a.b.y-b.b.y;
	buf.b.z=a.b.z-b.b.z;
	return buf;
}

vector3 egyseg( vector3 a)
{
	vector3 buf;
	v3_normalize(a,buf);
	return buf;
}

void m_xformd(matrix m,vector3 v, vector3 &d)
{
 float _x, _y, _z;
 _x = m[0][0] * v.b.x + m[0][1] * v.b.y + m[0][2] * v.b.z + m[0][3];
 _y = m[1][0] * v.b.x + m[1][1] * v.b.y + m[1][2] * v.b.z + m[1][3];
 _z = m[2][0] * v.b.x + m[2][1] * v.b.y + m[2][2] * v.b.z + m[2][3];
 d.b.x = _x;
 d.b.y = _y;
 d.b.z = _z;
}

void m_xform3(matrix m, vector3 v, vector3 &d)
{
 float _x, _y, _z;
 _x = m[0][0] * v.b.x + m[0][1] * v.b.y + m[0][2] * v.b.z;
 _y = m[1][0] * v.b.x + m[1][1] * v.b.y + m[1][2] * v.b.z;
 _z = m[2][0] * v.b.x + m[2][1] * v.b.y + m[2][2] * v.b.z;
 d.b.x = _x;
 d.b.y = _y;
 d.b.z = _z;
}

vector3 cross(vector3 b, vector3 c)
{
	vector3 buf;
	buf.b.x=(b.b.y * c.b.z) - (b.b.z * c.b.y);
	buf.b.y=(b.b.z * c.b.x) - (b.b.x * c.b.z);
	buf.b.z=(b.b.x * c.b.y) - (b.b.y * c.b.x);
	return buf;
}

vector3 vect_skalarszorzas(vector3 b, float a)
{
 vector3 c;
 c.b.x=b.b.x*a;
 c.b.y=b.b.y*a;
 c.b.z=b.b.z*a;
 return c;
}

float dotproduct(vector3 a, vector3 b)
{
 return a.b.x*b.b.x+a.b.y*b.b.y+a.b.z*b.b.z;
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

void m_invert(matrix &mat)
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

void m_revxpose(const matrix m, vector3 &v)
{
	v.b.x=m[0][3];
	v.b.y=m[1][3];
	v.b.z=m[2][3];
}

void m_revscale(const matrix m, vector3 &v)
{
	v.b.x=m[0][0];
	v.b.y=m[1][1];
	v.b.z=m[2][2];
}

/*procedure m_revrotate(const m : matrix; var v : vector; var phi : real);
var c, s : real;
begin
  v3_make(m[2, 1] - m[1, 2], m[0, 2] - m[2, 0], m[1, 0] - m[0, 1], v);
  s := v3_len(v);
  if s = 0 then
    begin
      v3_make(0, 0, 0, v);
      phi := 0;
      exit;
    end;
  v3_muls(v, 1/s, v);
  s := s * 0.5;
  c := (m[0, 0] - sqr(v.x)) / (1 - sqr(v.x));
  if c = 0 then phi := pi/2
  else
    begin
      phi := arctan(s / c);
      if c < 0 then phi := phi + pi;
    end;
end;

/**/

void m_revrotate(const matrix m, vector3 &v, float &phi)
{
	float c,s;
	v3_make(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1], v);
	s = v3_len(v);
	if (s == 0)
	{
		v3_make(0,0,0,v);
		phi = 0;
		return;
	}
	v=vect_skalarszorzas(v,1/s);
	c = (m[0][0] + m[1][1] + m[2][2] - 1);
	if (c == 0) phi = pi/2.0f;
	  else
	  {
		  phi = (float)atan(s / c);
		  if (c < 0) phi += pi;
	  }
}

quaternion axisangle2quaternion(vector3 axis, float angle)
{
	quaternion r;
	r.w     = (float) cos(angle/2.0);
    r.a.b.x = (float) ( axis.b.x * sin(angle/2.0) );
    r.a.b.y = (float) ( axis.b.y * sin(angle/2.0) );
    r.a.b.z = (float) ( axis.b.z * sin(angle/2.0) );
	if (axis.b.x==0 && axis.b.y==0 && axis.b.z==0)
	{
		r.w=1;
	}
	return r;
}

void quaternion2axisangle(quaternion q, vector3 &axis, float &angle)
{
	if (q.a.b.x == 0 && q.a.b.y == 0 && q.a.b.z == 0)
	{
		axis.b.x=1;
		axis.b.y=0;
		axis.b.z=0;
		angle=0;
		return;
	}
	angle=2.0f*(float)acos(q.w);
	float scale=q.a.b.x*q.a.b.x+q.a.b.y*q.a.b.y+q.a.b.z*q.a.b.z;
	axis.b.x=q.a.b.x/scale;
	axis.b.y=q.a.b.y/scale;
	axis.b.z=q.a.b.z/scale;
}

quaternion quaternionmult(quaternion a, quaternion b)
{
	quaternion r;
	r.w     = a.w*b.w - a.a.b.x*b.a.b.x - a.a.b.y*b.a.b.y - a.a.b.z*b.a.b.z;
	r.a.b.x = a.w*b.a.b.x + a.a.b.x*b.w + a.a.b.y*b.a.b.z - a.a.b.z*b.a.b.y;
	r.a.b.y = a.w*b.a.b.y + a.a.b.y*b.w + a.a.b.z*b.a.b.x - a.a.b.x*b.a.b.z;
	r.a.b.z = a.w*b.a.b.z + a.a.b.z*b.w + a.a.b.x*b.a.b.y - a.a.b.y*b.a.b.x;
	return r;
}