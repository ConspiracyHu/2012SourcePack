#include "cloth.h"

struct clothpoint
{
	vector3 position;
	vector3 speed;
	vector3 force;
	vector3 normal;
};

clothpoint cloth[15][10];

vector3 Gravity;
vector3 wind;
int xrez=14,yrez=9,mod=8;

void initcloth()
{
	Gravity.x=-0.0007f;
	Gravity.y=-0.0008f;
	Gravity.z=0.0001f;
	wind.x=-0.00033f;
	wind.y=-0.0001f;
	wind.z=-0.00015f;

	for (int x=0; x<xrez; x++)
		for (int y=0; y<yrez; y++)
		{
			cloth[x][y].position.x=(float)x;
			cloth[x][y].position.y=(float)y;
			cloth[x][y].position.z=0;
			cloth[x][y].speed.x=0;
			cloth[x][y].speed.y=0;
			cloth[x][y].speed.z=0;
		}
}

void checkspring(int x1,int y1,int x2,int y2,float defaultdistance)
{
	if (x1>=0 && x1<xrez && y1>=0 && y1<yrez &&
		x2>=0 && x2<xrez && y2>=0 && y2<yrez)
	{
		float Ks=0.324f;
		float Kd=0.04f;

		vector3 p1 = cloth[x1][y1].position;
		vector3 p2 = cloth[x2][y2].position;
		vector3 deltaP=v3_sub(p1,p2);
		float dist=v3_len(deltaP);

		float Hterm = (dist - defaultdistance) * Ks;

		vector3 deltaV = v3_sub(cloth[x1][y1].speed,cloth[x2][y2].speed);
		float Dterm = (v3_dot(deltaV,deltaP) * Kd) / dist;

		vector3 springForce = v3_normalize(deltaP);

		float multiplier=-(Hterm+Dterm);

		if (fabs(multiplier)>=0.0001)
		{
			springForce = v3_mults(springForce,multiplier);
			cloth[x1][y1].force=v3_add(cloth[x1][y1].force,springForce);
			cloth[x2][y2].force=v3_sub(cloth[x2][y2].force,springForce);
		}
	}
}

void countnormal(int x1, int y1, int x2, int y2, int x3, int y3)
{
	vector3 normal=v3_cross(v3_sub(cloth[x1][y1].position,cloth[x2][y2].position),v3_sub(cloth[x1][y1].position,cloth[x3][y3].position));
	vector3 force=v3_mults(v3_normalize(normal),v3_dot(normal,wind));
	cloth[x1][y1].force=v3_add(cloth[x1][y1].force,force);
	cloth[x2][y2].force=v3_add(cloth[x2][y2].force,force);
	cloth[x3][y3].force=v3_add(cloth[x3][y3].force,force);
	cloth[x1][y1].normal=v3_add(cloth[x1][y1].normal,normal);
	cloth[x2][y2].normal=v3_add(cloth[x2][y2].normal,normal);
	cloth[x3][y3].normal=v3_add(cloth[x3][y3].normal,normal);
}

void calculatecloth()
{
	int x;
	for (x=0; x<xrez; x++)
		for (int y=0; y<yrez; y++)
		{
			memset(&cloth[x][y].normal,0,sizeof(vector3));
			if (!(x==xrez-1 && y%mod==0))
			{
				cloth[x][y].force=Gravity;
				cloth[x][y].force=v3_add(cloth[x][y].force,v3_mults(cloth[x][y].speed,-0.002f));
			}
		}
	
	for (x=0; x<xrez-1; x++)
		for (int y=0; y<yrez-1; y++)
		{
			countnormal(x,y,x+1,y+1,x+1,y);
			countnormal(x,y,x,y+1,x+1,y+1);
		}

	for (x=0; x<xrez; x++)
		for (int y=0; y<yrez; y++)
			cloth[x][y].normal=v3_normalize(cloth[x][y].normal);

	for (x=0; x<xrez; x++)
		for (int y=0; y<yrez; y++)
		{
			checkspring(x,y,x+1,y,1);
			checkspring(x,y,x,y+1,1);
			if (x%2==0) checkspring(x,y,x+2,y,2);
			if (y%2==0) checkspring(x,y,x,y+2,2);
			checkspring(x,y,x+1,y+1,(float)sqrt(2.0f));
			checkspring(x,y+1,x+1,y,(float)sqrt(2.0f));
		}
	for (x=0; x<xrez; x++)
		for (int y=0; y<yrez; y++)
		{
			if (!(x==xrez-1 && y%mod==0))
			cloth[x][y].speed=v3_add(cloth[x][y].speed,cloth[x][y].force);
			else
				memset(&cloth[x][y].speed,0,sizeof(vector3));
			cloth[x][y].position=v3_add(cloth[x][y].position,cloth[x][y].speed);
		}
	
}

void drawcloth()
{
	float color[4];
	glBegin(GL_QUADS);
	for (int x=0; x<xrez-1; x++)
		for (int y=0; y<yrez-1; y++)
		{
			if (y<=yrez/3) 
			{
				color[0]=0;	color[1]=0.6f;	color[2]=0;	color[3]=0;
			}
			if (y>=yrez/3 && y<=yrez*2/3) 
			{
				color[0]=1;	color[1]=1;	color[2]=1;	color[3]=0;
			}
			if (y>=yrez*2/3) 
			{
				color[0]=0.8f;	color[1]=0;	color[2]=0;	color[3]=0;
			}
			glMaterialfv(GL_FRONT,GL_DIFFUSE,color);

			glNormal3f(cloth[x  ][y  ].normal.x,-cloth[x  ][y  ].normal.y,cloth[x  ][y  ].normal.z);
			glVertex3d(-(cloth[x  ][y  ].position.x-xrez+1)/4,-(cloth[x  ][y  ].position.y)/4+1.8,(cloth[x  ][y  ].position.z)/4);
			
			glNormal3f(cloth[x+1][y  ].normal.x,-cloth[x+1][y  ].normal.y,cloth[x+1][y  ].normal.z);
			glVertex3d(-(cloth[x+1][y  ].position.x-xrez+1)/4,-(cloth[x+1][y  ].position.y)/4+1.8,(cloth[x+1][y  ].position.z)/4);

			glNormal3f(cloth[x+1][y+1].normal.x,-cloth[x+1][y+1].normal.y,cloth[x+1][y+1].normal.z);
			glVertex3d(-(cloth[x+1][y+1].position.x-xrez+1)/4,-(cloth[x+1][y+1].position.y)/4+1.8,(cloth[x+1][y+1].position.z)/4);
		
			glNormal3f(cloth[x  ][y+1].normal.x,-cloth[x  ][y+1].normal.y,cloth[x  ][y+1].normal.z);
			glVertex3d(-(cloth[x  ][y+1].position.x-xrez+1)/4,-(cloth[x  ][y+1].position.y)/4+1.8,(cloth[x  ][y+1].position.z)/4);
		
		}
	glEnd();
}
