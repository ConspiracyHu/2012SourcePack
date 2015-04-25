#include "Splines.h"
#include "Vectors.h"
#include "includelist.h"

float hermite(float p1,float p2,float r1,float r2,float t) {
  return p1*( 2.0f*(t*t*t)-3.0f*(t*t)+1.0f)+
         r1*(     (t*t*t)-2.0f*(t*t)+t  )+
         p2*(-2.0f*(t*t*t)+3.0f*(t*t)    )+
         r2*(     (t*t*t)-    (t*t)    );
}

/*float bezier(float p1,float p2,float r1,float r2,float t) {
	float t3=t*t*t;
	float t2=t*t;
	
	float cx = 3.0f * (r1 - p1);
	float bx = 3.0f * (r2 - r1) - cx;
	float ax = p2 - p1 - cx - bx;

	return ax*t3 + bx*t2 + cx*t + p1;
}*/

void  SPLINE::InitVectors()
{
	int i;
	if (KeyNum<=1) return;
	for (i=0; i<KeyNum; i++) 
	{
		Keys[i].an=GetVector(0,i);
		Keys[i].bn=GetVector(1,i);
	}
}

#define TENS 0.0f
#define LOOP 0
#define BIAS 0.0f
#define CONT 0.0f

float SPLINE::GetVector(unsigned char sel, int n) 
{

	Key   kn_1, kn, kn1;
	float pn_1, pn, pn1;

	kn = Keys[n];
	pn = kn.Value;

	if (sel == 2) return pn;

	if (n == 0) 
	{
		//first key
		kn1 = Keys[1];
		pn1 = kn1.Value;

		if (KeyNum == 2) 
		{
			//2 Keys
			return (float)((pn1 - pn)*(1.0 - TENS));
		};
		if (!LOOP) 
		{
			//first key, no loop
			return (float)(((pn1 - pn)*1.5 - GetVector(0,1)*0.5)*(1.0 - TENS));
		} 
		else 
		{
			//first key, loop
			kn_1 = Keys[KeyNum-2];
		};
	}
	
	else 
	if (n == KeyNum-1) 
	{
		//last key
		kn_1 = Keys[n-1];
		pn_1 = kn_1.Value;

		if (KeyNum == 2) 
		{
			//2 Keys
			return (float)((pn - pn_1)*(1.0 - TENS));
		};
		if (!LOOP) 
		{
			//last key, no loop
			return (float)(((pn - pn_1)*1.5 - GetVector(1,n-1)*0.5)*(1.0 - TENS));
		} 
		else 
		{
			//last key, loop
			kn1 = Keys[1];
		};
	} 
	else 
	{
		//middle Keys
		kn_1= Keys[n-1];
		kn1 = Keys[n+1];
	};
	pn_1= kn_1.Value;
	pn1 = kn1 .Value;

	float f = (sel == 0) ? 0.5f : -0.5f;

	float g1 = (pn  - pn_1)*(1.0f + BIAS);
	float g2 = (pn1 - pn  )*(1.0f - BIAS);

	return (g1 + (g2-g1)*(0.5f + f*CONT))*(1.0f - TENS);
}

float catmull(float v0,float v1,float v2,float v3,float t) {
   //float xx = t / distance;
   float P = (v3 - v2) - (v0 - v1);
   float Q = (v0 - v1) - P;
   float R =  v2 - v0;
   float r=((P*t*t*t) + (Q*t*t) + (R*t) + v1);
   return r;
}

float catmull2(float v0,float v1,float v2,float v3,float t) {
	float P= -v0 + 3*v1 - 3*v2 + v3;
	float Q=2*v0 - 5*v1 + 4*v2 - v3;
	float R= -v0 + v2;
	float S= 2*v1;
	float r=((P*t*t*t) + (Q*t*t) + (R*t) + S)*0.5f;
	return r;
}

float SPLINE::GetInterpolatedValue(float Time)
{
	int x=0;
	float amp=1;

	if (Interpolation == aDDict_SINE || Interpolation == aDDict_SAW || Interpolation == aDDict_SQUARE)
	{
		if (Time<Keys[0].Time) amp=LinearInterpolate(0,Keys[0].Value,Time/Keys[0].Time);
		if (Time>Keys[1].Time) amp=LinearInterpolate(Keys[1].Value,0,(Time-Keys[1].Time)/(1-Keys[1].Time));
		if (Time>=Keys[0].Time && Time<=Keys[1].Time) amp=LinearInterpolate(Keys[0].Value,Keys[1].Value,(Time-Keys[0].Time)/(Keys[1].Time-Keys[0].Time));
	}
	else
	{
		while (Keys[x].Time<Time && x<KeyNum) x++;
		x--;
		if (x==-1) 
		{
			x=0;
			return Keys[x].Value;
		}
		if (x==KeyNum-1) return Keys[x].Value;
	}

	float t; //a ket kulcs kozotti pozicio
	t=(Time-Keys[x].Time)/(Keys[x+1].Time-Keys[x].Time);

	switch (Interpolation)
	{
	case aDDict_LINEAR:
		return LinearInterpolate((float)Keys[x].Value,(float)Keys[x+1].Value,t);
	case aDDict_HERMITE:
		return hermite(Keys[x].Value,Keys[x+1].Value,Keys[x].an  ,Keys[x+1].bn,  t );
	case aDDict_CATMULL:
		int a,b,c,d;
		a=x-1;
		if (x-1<0) a=0;
		b=x; c=x+1;
		d=x+2;
		if (x+1>=KeyNum-1) d=KeyNum-1;
		return catmull2(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
		break;
	case aDDict_BEZIER:
		{
			int a,b,c,d;
			a=x-1;
			if (x-1<0) a=0;
			b=x; c=x+1;
			d=x+2;
			if (x+1>=KeyNum-1) d=KeyNum-1;
			/*a=(x-1+KeyNum) % KeyNum;
			b=(x+KeyNum)   % KeyNum;
			c=(x+1+KeyNum) % KeyNum;
			d=(x+2+KeyNum) % KeyNum;*/
			return BSplineInterpolate(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
			//return catmull2(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
		}
	/*case aDDict_BEZIER:
		return bezier(Keys[x].Value,Keys[x].Value,Keys[x+1].Value,Keys[x+1].Value,t);*/
	case aDDict_SINE:
		return (float)(amp*sin(Time/Keys[2].Time));
		break;
	case aDDict_SAW:
		float va;
		float t;
		t=Time/Keys[2].Time/3.1415f;
		t=t-(float)floor(t);

		if (t<0.5f) va=4*t-1; else va=1-4*(t-0.5f);

 		return amp*va;
		break;
	case aDDict_SQUARE:
		float v=(float)sin(Time/Keys[2].Time);
		v=(float)(v/fabs(v));
		return amp*v;
		break;
	}

	return Keys[x].Value;
}

void GetVector(unsigned char sel, unsigned int n, SPLINE *s, SPLINE *x, SPLINE *y, SPLINE *z, float *a, float *b, float *c, float *d)
{
	Key      kn_1[4], kn[4], kn1[4];
	QUATERNION qn_1, qn, qn1;

	kn[0]=s->Keys[n];
	kn[1]=x->Keys[n];
	kn[2]=y->Keys[n];
	kn[3]=z->Keys[n];
	qn.s=s->Keys[n].Value;
	qn.v.x=x->Keys[n].Value;
	qn.v.y=y->Keys[n].Value;
	qn.v.z=z->Keys[n].Value;

	//  if (sel == 2) return qn;

	if (!n) 
	{

		kn1[0]=s->Keys[1];
		kn1[1]=x->Keys[1];
		kn1[2]=y->Keys[1];
		kn1[3]=z->Keys[1];
		qn1.s=s->Keys[1].Value;
		qn1.v.x=x->Keys[1].Value;
		qn1.v.y=y->Keys[1].Value;
		qn1.v.z=z->Keys[1].Value;

		QUATERNION q=Q_Slerp(qn,qn1,1/3.0f);
		*a=q.s;
		*b=q.v.x;
		*c=q.v.y;
		*d=q.v.z;
		return;
		
	} 
	else if ((signed)n == s->KeyNum-1) 
	{
		kn_1[0]=s->Keys[n-1];
		kn_1[1]=x->Keys[n-1];
		kn_1[2]=y->Keys[n-1];
		kn_1[3]=z->Keys[n-1];
		qn_1.s=s->Keys[n-1].Value;
		qn_1.v.x=x->Keys[n-1].Value;
		qn_1.v.y=y->Keys[n-1].Value;
		qn_1.v.z=z->Keys[n-1].Value;

		QUATERNION q=Q_Slerp(qn,qn_1,1/3.0f);
		*a=q.s;
		*b=q.v.x;
		*c=q.v.y;
		*d=q.v.z;
		return;
	}
	else {
		//middle Keys
		kn_1[0]=s->Keys[n-1];
		kn_1[1]=x->Keys[n-1];
		kn_1[2]=y->Keys[n-1];
		kn_1[3]=z->Keys[n-1];
		kn1[0] =s->Keys[n+1];
		kn1[1] =x->Keys[n+1];
		kn1[2] =y->Keys[n+1];
		kn1[3] =z->Keys[n+1];
	};
	qn_1.s= kn_1[0].Value;
	qn_1.v.x= kn_1[1].Value;
	qn_1.v.y= kn_1[2].Value;
	qn_1.v.z= kn_1[3].Value;
	qn1.s = kn1[0] .Value;
	qn1.v.x= kn1[1] .Value;
	qn1.v.y= kn1[2] .Value;
	qn1.v.z= kn1[3] .Value;

	float f = !sel ? 1.0f : -1.0f;

	QUATERNION g1 = Q_Slerp(qn,qn_1,-1/3.0f);
	QUATERNION g2 = Q_Slerp(qn,qn1,  1/3.0f);
	//  udQuatKeyGetQuat:=udQuatSlerp(qn, udQuatSlerp(g1,g2,0.5+f*0.5*kn.c) ,f*(kn.t-1));

	//  return (g1 + (g2-g1)*(0.5 + f*CONT))*(1.0 - TENS);
	QUATERNION q=Q_Slerp(qn,Q_Slerp(g1,g2,0.5), f*(-1));
	*a=q.s;
	*b=q.v.x;
	*c=q.v.y;
	*d=q.v.z;
}

QUATERNION quat_interp(Key k1[4], Key k2[4], float t) 
{
	QUATERNION k1v,k1b,k2a,k2v;
	k1v.s=k1[0].Value;
	k1v.v.x=k1[1].Value;
	k1v.v.y=k1[2].Value;
	k1v.v.z=k1[3].Value;
	k1v=Q_Normalize(k1v);
	k1b.s=k1[0].bn;
	k1b.v.x=k1[1].bn;
	k1b.v.y=k1[2].bn;
	k1b.v.z=k1[3].bn;
	k1b=Q_Normalize(k1b);

	k2v.s=k2[0].Value;
	k2v.v.x=k2[1].Value;
	k2v.v.y=k2[2].Value;
	k2v.v.z=k2[3].Value;
	k2v=Q_Normalize(k2v);
	k2a.s=k2[0].an;
	k2a.v.x=k2[1].an;
	k2a.v.y=k2[2].an;
	k2a.v.z=k2[3].an;
	k2a=Q_Normalize(k2a);

	QUATERNION q0=Q_Slerp(k1v, k1b  , t);
	QUATERNION q1=Q_Slerp(k1b  , k2a  , t);
	QUATERNION q2=Q_Slerp(k2a  , k2v, t);
	
	q0=Q_Slerp(q0,q1,t);
	q1=Q_Slerp(q1,q2,t);
	
	return Q_Normalize(Q_Slerp(q0,q1,t));
}

void Spline_QuaternionGetVectors(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z)
{
	if (s->KeyNum==1) return;
	for (int i=0; i<s->KeyNum; i++) 
	{
		GetVector(0,i,s,x,y,z,&s->Keys[i].an,&x->Keys[i].an,&y->Keys[i].an,&z->Keys[i].an);
		GetVector(1,i,s,x,y,z,&s->Keys[i].bn,&x->Keys[i].bn,&y->Keys[i].bn,&z->Keys[i].bn);
	}
}

void Spline_Slerp(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z, float *a, float *b, float *c, float *d, float Time)
{
	int i=0;
	while (s->Keys[i].Time<Time && i<s->KeyNum) i++;
	i--;
	if (i==-1) 
	{
		i=0;
		//return Keys[i].Value;
		*a=s->Keys[i].Value;
		*b=x->Keys[i].Value;
		*c=y->Keys[i].Value;
		*d=z->Keys[i].Value;
		return;
	}
	if (i==s->KeyNum-1) 
	{
		*a=s->Keys[i].Value;
		*b=x->Keys[i].Value;
		*c=y->Keys[i].Value;
		*d=z->Keys[i].Value;
		return;
	}

	float t; //a ket kulcs kozotti pozicio
	t=(Time-s->Keys[i].Time)/(s->Keys[i+1].Time-s->Keys[i].Time);
	Key q1[4],q2[4];
	q1[0]=s->Keys[i];
	q1[1]=x->Keys[i];
	q1[2]=y->Keys[i];
	q1[3]=z->Keys[i];
	q2[0]=s->Keys[i+1];
	q2[1]=x->Keys[i+1];
	q2[2]=y->Keys[i+1];
	q2[3]=z->Keys[i+1];
	QUATERNION q3;
	q3=quat_interp(q1,q2,t);
	*a=q3.s;
	*b=q3.v.x;
	*c=q3.v.y;
	*d=q3.v.z;
}