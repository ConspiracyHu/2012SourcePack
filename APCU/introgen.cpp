#include "intro.h"
#include "introgen.h"
//#include <windows.h>
//#include <gl\gl.h>
//#include <gl\glu.h>

char boxvertices[] =
	{  -5,-5,-5, 5,-5,-5, 5, 5,-5,-5, 5,-5, 0, 0,-5, 5,-5, 5, 5, 5, 5, 5, 0, 0,
	   -5, 5, 5,-5,-5, 5, 0, 0, 5,-5, 0, 0,	0,-5, 0, 0, 5, 0 };
/*char dodecaedervertices[] =
	{	 35,  00,  93,-35,  00,  93, 57,  57,  57, 00,  93,  35,-57,  57,  57,-57, -57,  57,
		-00, -93,  35, 57, -57,  57, 93,  35,  00, 93, -35,  00, 00, -93, -35, 57, -57, -57,
		 57,  57, -57, 35, -00, -93,-57, -57, -57,-35, -00, -93, 00,  93, -35,-57,  57, -57,
		-93, -35, -00,-93,  35, -00};
char icosaedervertices[] =
{	    0,   0, 117, 105,   0,  52,  32, 100,  52, -85,  61,  52, -85, -61,  52,
	   32,-100,  52,  85,  61, -52, -32, 100, -52,-105,   0, -52, -32,-100, -52,
	   85, -61, -52,   0,   0,-117};*/
char boxpolygons[] =
	{	0,1,4,1,2,4,2,3,4,3,0,4,1,5,7,5,6,7,6,2,7,2,1,7,5,9,10,9,8,10,8,6,10,6,5,10,
		9,0,11,0,3,11,3,8,11,8,9,11,9,5,12,5,1,12,1,0,12,0,9,12,3,2,13,2,6,13,6,8,13,8,3,13 };
/*char dodecaederpolygons[] =
	{	2,0,3,3,0,4,4,0,1,5,1,6,6,1,7,7,1,0,9,7,8,8,7,2,2,7,0,6,7,10,10,7,11,11,7,9,
		13,11,12,12,11,8,8,11,9,10,11,14,14,11,15,15,11,13,17,15,16,16,15,12,12,15,13,
		14,15,18,18,15,19,19,15,17,4,19,3,3,19,16,16,19,17,18,19,5,5,19,1,1,19,4,14,18,10,
		10,18,6,6,18,5,2,3,8,8,3,12,12,3,16 };
char icosaederpolygons[] =
	{	0,2,1,0,3,2,0,4,3,0,5,4,0,1,5,1,2,6,2,3,7,3,4,8,4,5,9,5,1,10,4,9,8,5,10,9,
		1,6,10,2,7,6,3,8,7,6,7,11,7,8,11,8,9,11,9,10,11,10,6,11 };*/
char boxpolytexcoords[] =
	{	0,0,2,0,1,1,2,0,2,2,1,1,2,2,0,2,1,1,0,2,0,0,1,1,0,0,2,0,1,1,2,0,2,2,1,1,2,2,0,2,1,1,0,2,0,0,1,1,0,0,2,0,1,1,2,0,2,2,1,1,2,2,0,2,1,1,0,2,0,0,1,1,
		0,0,2,0,1,1,2,0,2,2,1,1,2,2,0,2,1,1,0,2,0,0,1,1,0,0,2,0,1,1,2,0,2,2,1,1,2,2,0,2,1,1,0,2,0,0,1,1,0,0,2,0,1,1,2,0,2,2,1,1,2,2,0,2,1,1,0,2,0,0,1,1	};

void obj_generatenormals(object *o)
{
	vector3 facetNormal,center,v0,v1;
	int x;
	for (x=0; x<o->vertexnum; x++) o->vertices[x].defaultnormal=v3_make(0,0,0);
	for (x=0; x<o->polygonnum; x++)
	{
		facetNormal=v3_make(0,0,0);
		center=o->vertices[o->polygons[x].v1].base;
		v0 = v3_sub(o->vertices[o->polygons[x].v2].base,center);
		v1 = v3_sub(o->vertices[o->polygons[x].v3].base,center);

		facetNormal=(v3_cross(v1,v0));
		if (o->data.normalsinverted ^ o->polygons[x].normalinverted)
		{
			facetNormal.x*=-1;
			facetNormal.y*=-1;
			facetNormal.z*=-1;
		}
		o->polygons[x].defaultnormal=facetNormal;
	}

	for (x=0; x<o->polygonnum; x++)
	{
		o->vertices[o->polygons[x].v1].defaultnormal=v3_add(o->vertices[o->polygons[x].v1].defaultnormal,o->polygons[x].defaultnormal);
		o->vertices[o->polygons[x].v2].defaultnormal=v3_add(o->vertices[o->polygons[x].v2].defaultnormal,o->polygons[x].defaultnormal);
		o->vertices[o->polygons[x].v3].defaultnormal=v3_add(o->vertices[o->polygons[x].v3].defaultnormal,o->polygons[x].defaultnormal);
	}
	for (x=0; x<o->vertexnum; x++)
	{
		o->vertices[x].defaultnormal=v3_normalize(o->vertices[x].defaultnormal);
	}
}

void obj_counttexturecoordinates(object *o, int xr, int yr, int xoff, int yoff, bool swap, bool xswp, bool yswp)
{
	int x;
	for (x=0; x<o->vertexnum;x++)
	{
		o->vertices[x].t.v=o->vertices[x].dt.v*xr+(float)xoff/255.0f;
		o->vertices[x].t.u=o->vertices[x].dt.u*yr+(float)yoff/255.0f;
	}
	for (x=0; x<o->polygonnum; x++)
	{
		o->polygons[x].t1.v=o->polygons[x].d1.v*(float)xr+(float)xoff/255.0f;
		o->polygons[x].t1.u=o->polygons[x].d1.u*(float)yr+(float)yoff/255.0f;
		o->polygons[x].t2.v=o->polygons[x].d2.v*(float)xr+(float)xoff/255.0f;
		o->polygons[x].t2.u=o->polygons[x].d2.u*(float)yr+(float)yoff/255.0f;
		o->polygons[x].t3.v=o->polygons[x].d3.v*(float)xr+(float)xoff/255.0f;
		o->polygons[x].t3.u=o->polygons[x].d3.u*(float)yr+(float)yoff/255.0f;
	}
	for (x=0; x<o->vertexnum;x++)
	{
		if (xswp) o->vertices[x].t.v=1-o->vertices[x].t.v;
		if (yswp) o->vertices[x].t.u=1-o->vertices[x].t.u;
	}
	for (x=0; x<o->polygonnum;x++)
	{
		if (xswp) o->polygons[x].t1.v=1-o->polygons[x].t1.v;
		if (yswp) o->polygons[x].t1.u=1-o->polygons[x].t1.u;
		if (xswp) o->polygons[x].t2.v=1-o->polygons[x].t2.v;
		if (yswp) o->polygons[x].t2.u=1-o->polygons[x].t2.u;
		if (xswp) o->polygons[x].t3.v=1-o->polygons[x].t3.v;
		if (yswp) o->polygons[x].t3.u=1-o->polygons[x].t3.u;
	}
	if (!swap)
	{
		for (x=0; x<o->vertexnum;x++)
		{
			float b=o->vertices[x].t.u;
			o->vertices[x].t.u=o->vertices[x].t.v;
			o->vertices[x].t.v=b;
		}
		for (x=0; x<o->polygonnum;x++)
		{
			float b;
			b=o->polygons[x].t1.u;
			o->polygons[x].t1.u=o->polygons[x].t1.v;
			o->polygons[x].t1.v=b;
			b=o->polygons[x].t2.u;
			o->polygons[x].t2.u=o->polygons[x].t2.v;
			o->polygons[x].t2.v=b;
			b=o->polygons[x].t3.u;
			o->polygons[x].t3.u=o->polygons[x].t3.v;
			o->polygons[x].t3.v=b;
		}
	}
}

float getmappixel(vector2 t, int channel, bool nowrap)
{
	int xp,yp;
	if (!nowrap)
	{
		xp=((int)(t.u*255.0f)) % 256;
		yp=((int)(t.v*255.0f)) % 256;
		if (xp<0) xp+=256;
		if (yp<0) yp+=256;
	}
	else
	{
		xp=((int)(t.u*255.0f));
		yp=((int)(t.v*255.0f));
	}
	int b=0;
	if (xp>=0 && xp<256 && yp>=0 && yp<256)
	{
		switch (channel)
		{
		case 0:{b=maptexture[xp][yp].r;break;}
		case 1:{b=maptexture[xp][yp].g;break;}
		case 2:{b=maptexture[xp][yp].b;break;}
		case 3:{b=(maptexture[xp][yp].r+maptexture[xp][yp].g+maptexture[xp][yp].b)/3;break;}
		}
	}
	return (float)((float)b/255.0f);
}

void setobjectlastvertextexturecoordinates(object *o, float u, float v)
{
	int last=o->vertexnum-1;
	o->vertices[last].t.u=u;
	o->vertices[last].t.v=v;
	o->vertices[last].dt.u=u;
	o->vertices[last].dt.v=v;
}

void setobjectlastpolygontexturecoordinates(object *o, float u1, float v1, float u2, float v2, float u3, float v3)
{
	int last=o->polygonnum-1;
	o->polygons[last].t1.u=u1;
	o->polygons[last].t1.v=v1;
	o->polygons[last].t2.u=u2;
	o->polygons[last].t2.v=v2;
	o->polygons[last].t3.u=u3;
	o->polygons[last].t3.v=v3;
	o->polygons[last].d1=o->polygons[last].t1;
	o->polygons[last].d2=o->polygons[last].t2;
	o->polygons[last].d3=o->polygons[last].t3;
}

void obj_createprimitive(object *o, char primitive, int param1, int param2, int param3, int param4)
{
	//o=newobject();
	switch (primitive)
	{
	case (aDDict_box):
		{
			int x;
			for (x=0; x<14; x++) addvertex(o,(float)boxvertices[x*3]/10.0f,(float)boxvertices[x*3+1]/10.0f,(float)boxvertices[x*3+2]/10.0f);
			for (x=0; x<24; x++) 
			{
				addpolygon(o,boxpolygons[x*3],boxpolygons[x*3+1],boxpolygons[x*3+2],aDDict_flatshade);
				setobjectlastpolygontexturecoordinates(o,(float)boxpolytexcoords[x*6]/2.0f,(float)boxpolytexcoords[x*6+1]/2.0f,(float)boxpolytexcoords[x*6+2]/2.0f,(float)boxpolytexcoords[x*6+3]/2.0f,(float)boxpolytexcoords[x*6+4]/2.0f,(float)boxpolytexcoords[x*6+5]/2.0f);
			}
			break;
		}
	case (aDDict_sphere):
		{
			addvertex(o,0,-0.5f,0);
			addvertex(o,0,0.5f,0);
			int xx;
			for (xx=1;xx<=param1-2;xx++)
			{
				float theta=(xx/(param1-1.0f)-0.5f)*180.0f;
				for (int yy=0; yy<param2; yy++)
				{
					float phi=((float)yy/param2)*360.0f;
					addvertex(o,(float)cos(theta*radtheta)*(float)cos(phi*radtheta)/2.0f,
						        (float)sin(theta*radtheta)/2.0f,
						        (float)cos(theta*radtheta)*(float)sin(phi*radtheta)/2.0f);
					if (xx!=0 && xx!=param1-1)
						setobjectlastvertextexturecoordinates(o,(float)xx/(float)param1,(float)yy/(float)param2);
					else				
						if (yy==0) setobjectlastvertextexturecoordinates(o,0.5,0);
				}
			}

			int yy;
			for (yy=0;yy<=param2-1;yy++)
			{
				addpolygon(o,0,(yy+1)%param2+2,(yy)%param2+2,aDDict_gouraudshade);
				setobjectlastpolygontexturecoordinates(o,0,(float)(yy+0.5)/(float)(param2),
														 1/(float)(param1-2),(float)(yy+1)/(float)(param2),
														 1/(float)(param1-2),(float)yy/(float)(param2));
				addpolygon(o,1,(param1-3)*(param2)+(yy)%param2+2,
					           (param1-3)*(param2)+(yy+1)%param2+2,
					            aDDict_gouraudshade);
				setobjectlastpolygontexturecoordinates(o,1,(float)(yy+0.5)/(float)(param2),
														 1-1/(float)(param1-2),(float)(yy)/(float)(param2),
														 1-1/(float)(param1-2),(float)(yy+1)/(float)(param2));
			}

			for (xx=0;xx<=param1-4;xx++)
				for (yy=0;yy<=param2-1;yy++)
				{
					addpolygon(o,xx   *param2+yy    %param2+2,
						         xx   *param2+(yy+1)%param2+2,
						        (xx+1)*param2+(yy+1)%param2+2,
						         aDDict_gouraudshade);
					setobjectlastpolygontexturecoordinates(o,(float)(xx+1)/(float)(param1-2),(float)yy/(float)(param2),
															 (float)(xx+1)/(float)(param1-2),(float)(yy+1)/(float)(param2),
															 (float)((xx+1)+1)/(float)(param1-2),(float)(yy+1)/(float)(param2));
					addpolygon(o,xx   *param2+yy    %param2+2,
						        (xx+1)*param2+(yy+1)%param2+2,
						        (xx+1)*param2+yy    %param2+2,
						         aDDict_gouraudshade);
					setobjectlastpolygontexturecoordinates(o,(float)(xx+1)/(float)(param1-2),(float)yy/(float)(param2),
															 (float)(xx+2)/(float)(param1-2),(float)(yy+1)/(float)(param2),
															 (float)(xx+2)/(float)(param1-2),(float)yy/(float)(param2));
				}
			break;
		}
	/*case (aDDict_dodecaeder):
		{
			for (int x=0; x<20; x++) addvertex(o,(float)dodecaedervertices[x*3]/100.0f,(float)dodecaedervertices[x*3+1]/100.0f,(float)dodecaedervertices[x*3+2]/100.0f);
			for (x=0; x<36; x++) addpolygon(o,dodecaederpolygons[x*3],dodecaederpolygons[x*3+1],dodecaederpolygons[x*3+2],aDDict_flatshade);
			break;
		}*/
	case (aDDict_hasab):
		{
			int xx;
			for (xx=0;xx<=param2;xx++)
				for (int yy=0;yy<param1;yy++)
				{
					addvertex(o,(float)sin(((float)yy/(float)(param1))*360*radtheta)*0.5f,
						        (float)xx/(float)param2-0.5f,
						        (float)cos(((float)yy/(float)(param1))*360*radtheta)*0.5f);
					setobjectlastvertextexturecoordinates(o,(float)yy/(param1),(float)xx/(param2));
				}

			for (xx=0;xx<=param1-1;xx++)
				for (int yy=0;yy<=param2-1;yy++)
				{
					addpolygon(o, yy   *param1+(xx+1)%param1,
						          yy   *param1+  xx  %param1,
						         (yy+1)*param1+(xx+1)%param1, aDDict_gouraudshade);
					setobjectlastpolygontexturecoordinates(o,(float)(xx+1)/(param1),(float)  yy  /(param2),
															 (float)  xx  /(param1),(float)  yy  /(param2),
															 (float)(xx+1)/(param1),(float)(yy+1)/(param2));
					addpolygon(o,(yy+1)*param1+(xx+1)%param1,
						          yy   *param1+  xx  %param1,
						         (yy+1)*param1+  xx  %param1, aDDict_gouraudshade);
					setobjectlastpolygontexturecoordinates(o,(float)(xx+1)/(param1),(float)(yy+1)/(param2),
															 (float)  xx  /(param1),(float)  yy  /(param2),
															 (float)  xx  /(param1),(float)(yy+1)/(param2));
				}
				
			if (param3)
			{
				addvertex(o,0,-0.5,0);
				setobjectlastvertextexturecoordinates(o,0.5,0.5);
				addvertex(o,0,0.5,0);
				setobjectlastvertextexturecoordinates(o,0.5,0.5);

				for (xx=0;xx<=param1-1;xx++)
				{
					addpolygon(o,o->vertexnum-2,xx,(xx+1)%param1,aDDict_flatshade);
					setobjectlastpolygontexturecoordinates(o,o->vertices[o->polygons[o->polygonnum-1].v1].d.x+0.5f,o->vertices[o->polygons[o->polygonnum-1].v1].d.z+0.5f,
															 o->vertices[o->polygons[o->polygonnum-1].v2].d.x+0.5f,o->vertices[o->polygons[o->polygonnum-1].v2].d.z+0.5f,
															 o->vertices[o->polygons[o->polygonnum-1].v3].d.x+0.5f,o->vertices[o->polygons[o->polygonnum-1].v3].d.z+0.5f);
					addpolygon(o,param1*param2+xx,o->vertexnum-1,param1*param2+(xx+1)%param1,aDDict_flatshade);
					setobjectlastpolygontexturecoordinates(o,o->vertices[o->polygons[o->polygonnum-1].v1].d.x+0.5f,o->vertices[o->polygons[o->polygonnum-1].v1].d.z+0.5f,
															 o->vertices[o->polygons[o->polygonnum-1].v2].d.x+0.5f,o->vertices[o->polygons[o->polygonnum-1].v2].d.z+0.5f,
															 o->vertices[o->polygons[o->polygonnum-1].v3].d.x+0.5f,o->vertices[o->polygons[o->polygonnum-1].v3].d.z+0.5f);

				}
			}
			break;
		}
	case (aDDict_cone):
		{
			addvertex(o,0,-0.5f,0);
			setobjectlastvertextexturecoordinates(o,0.5,0.5);
			for (int yy=0;yy<=param1-1;yy++)
			{
				addvertex(o,(float)sin(((float)yy/(float)param1)*360*radtheta)*0.5f,0.5f,
					        (float)cos(((float)yy/(float)param1)*360*radtheta)*0.5f);
				setobjectlastvertextexturecoordinates(o,(float)(sin(((float)yy/(float)(param1))*360*radtheta)*0.5+0.5),(float)(cos(((float)yy/(float)(param1))*360*radtheta)*0.5+0.5));
			}
			
			int xx;
			for (xx=0;xx<=param1-1;xx++)
				addpolygon(o,0, xx   %param1+1,
				               (xx+1)%param1+1,aDDict_gouraudshade);
			
			if (param2)
			{
				if (param1!=3)
				{
					addvertex(o,0,0.5f,0);
					for (xx=0;xx<=param1-1;xx++)
						addpolygon(o,xx   %param1+1,
							          o->vertexnum-1,
							        (xx+1)%param1+1,aDDict_flatshade);
				}
				else addpolygon(o,2,1,3,aDDict_flatshade);

			}
			break;
		}
	/*case (aDDict_icosaeder):
		{
			for (int x=0; x<12; x++) addvertex(o,(float)dodecaedervertices[x*3]/100.0f,(float)dodecaedervertices[x*3+1]/100.0f,(float)dodecaedervertices[x*3+2]/100.0f);
			for (x=0; x<20; x++) addpolygon(o,dodecaederpolygons[x*3],dodecaederpolygons[x*3+1],dodecaederpolygons[x*3+2],aDDict_flatshade);
			break;
		}*/
	case (aDDict_arc):
		{
			for (int xx=-1;xx<=(int)(param1+1);xx++)
				addvertex(o,0.5f*(float)sin(((float)xx/(float)param1)*radtheta*param2),0,
					        0.5f*(float)cos(((float)xx/(float)param1)*radtheta*param2));
			break;
		}
	case (aDDict_loft):
		{
			vector3 up=v3_make(0,-1,0);

			object *v1=searchobjectbynumber((object*)(param3),param4,param1);
			object *v2=searchobjectbynumber((object*)(param3),param4,param2);
			int v1v=v1->vertexnum-2;
			int v2v=v2->vertexnum-2;

			int xx=0;
			if ((v1->vertexnum>1) && (v2->vertexnum>1))
			{
				for (int xa=1;xa<v1->vertexnum-1;xa++)
				{
					m_xformd(v1->xformmatrix,v1->vertices[xa-1].d,v1->vertices[xa-1].position);
					m_xformd(v1->xformmatrix,v1->vertices[xa].d,v1->vertices[xa].position);
					m_xformd(v1->xformmatrix,v1->vertices[xa+1].d,v1->vertices[xa+1].position);
					vector3 a1=v3_normalize(v3_sub(v1->vertices[xa].position,v1->vertices[xa-1].position));
					vector3 a2=v3_normalize(v3_sub(v1->vertices[xa+1].position,v1->vertices[xa].position));
					vector3 dir=v3_normalize(v3_add(a1,a2));

					vector3 nx=v3_cross(up,dir);
					vector3 ny=v3_cross(nx,dir);

					int yy=0;
					for (int ya=1;ya<v2->vertexnum-1;ya++)
					{
						vector3 vv;
						m_xformd(v2->xformmatrix,v2->vertices[ya].d,vv);
						vector3 nv=v3_add(v3_mults(nx,vv.x),
							              v3_mults(ny,vv.z));
						nv=v3_add(nv,v1->vertices[xa].position);

						addvertex(o,nv.x,nv.y,nv.z);
						nv=v3_add(v3_mults(nx,v2->vertices[ya].normal.x),
							v3_mults(ny,v2->vertices[ya].normal.z));

						setobjectlastvertextexturecoordinates(o,(float)xx/(float)(v1v-1),(float)yy/(float)(v2v-1));
						yy++;
					}
					xx++;
				}

				for (xx=0;xx<=v2v-2;xx++)
					for (int yy=0;yy<=v1v-2;yy++)
					{
						addpolygon(o, yy   *v2v+xx+1,
							          yy   *v2v+xx,
							         (yy+1)*v2v+xx+1
									 ,aDDict_gouraudshade);

						addpolygon(o,(yy+1)*v2v+xx+1,
							          yy   *v2v+xx,
							         (yy+1)*v2v+xx
									 ,aDDict_gouraudshade);
					}
			}
			break;
		}
	case (aDDict_line):
		{
			for (int xx=-1;xx<=(int)(param1+1);xx++) 
				addvertex(o,0,0,((float)xx/(float)param1)-0.5f);
			break;
		}
	case (aDDict_grid):
		{
			int yy;
			for (yy=0;yy<=param2;yy++)
				for (int xx=0;xx<=param1;xx++)
				{
					addvertex(o,(float)xx/(float)param1-0.5f,0,(float)yy/(float)param2-0.5f);
					setobjectlastvertextexturecoordinates(o,(float)xx/(float)((int)(param1)),-(float)yy/(float)((int)(param2)));
				}
			for (int xx=0;xx<=(param1-1);xx++)
				for (yy=0;yy<=(param2-1);yy++)
				{
					addpolygon(o,
						 yy   *(param1+1)+xx+1,
						 yy   *(param1+1)+xx,
						(yy+1)*(param1+1)+xx+1,
						aDDict_gouraudshade);
					addpolygon(o,
						(yy+1)*(param1+1)+xx+1,
						 yy   *(param1+1)+xx,
						(yy+1)*(param1+1)+xx,
						aDDict_gouraudshade);
				}
			break;
		}
	case (aDDict_clone):
		{
			selection *parentlist=(selection*)param1;
			object *objectlist=(object*)param2;
			int parentnum=selsize(parentlist);

			for (int x=0;x<parentnum;x++)
			{		 
				object *obj=searchobjectbynumber(objectlist,param3,parentlist->selected);
				if (obj!=NULL)
				{
					int vn=o->vertexnum;
					//int pn=o->polygonnum;
					
					int xa;
					for (xa=0;xa<obj->vertexnum;xa++)
					{
						addvertex(o,0,0,0);
						o->vertices[o->vertexnum-1]=obj->vertices[xa];
						o->vertices[o->vertexnum-1].dt=obj->vertices[xa].t;
						o->vertices[o->vertexnum-1].base=obj->vertices[xa].d;
						m_xformd(obj->xformmatrix,obj->vertices[xa].d,o->vertices[o->vertexnum-1].d);
						m_xform3(obj->xformmatrix,obj->vertices[xa].defaultnormal,o->vertices[o->vertexnum-1].defaultnormal);
						o->vertices[o->vertexnum-1].defaultnormal=v3_normalize(o->vertices[o->vertexnum-1].defaultnormal);
						//o->vertices[o->vertexnum-1].d=obj->vertices[xa].position;
						o->vertices[o->vertexnum-1].base=o->vertices[o->vertexnum-1].d;
						//addvertex(o,obj->vertices[xa].position.x,obj->vertices[xa].position.y,obj->vertices[xa].position.z);
					}

					for (xa=0;xa<obj->polygonnum;xa++)
					{
						addpolygon(o,0,0,0,0);
						o->polygons[o->polygonnum-1]=obj->polygons[xa];
						o->polygons[o->polygonnum-1].v1+=vn;
						o->polygons[o->polygonnum-1].v2+=vn;
						o->polygons[o->polygonnum-1].v3+=vn;
						o->polygons[o->polygonnum-1].d1=obj->polygons[xa].t1;
						o->polygons[o->polygonnum-1].d2=obj->polygons[xa].t2;
						o->polygons[o->polygonnum-1].d3=obj->polygons[xa].t3;
						m_xform3(obj->xformmatrix,obj->polygons[xa].defaultnormal,o->polygons[o->polygonnum-1].defaultnormal);
						o->polygons[o->polygonnum-1].defaultnormal=v3_normalize(o->polygons[o->polygonnum-1].defaultnormal);
						//addpolygon(o,obj->polygons[xa].v1+vn,obj->polygons[xa].v2+vn,obj->polygons[xa].v3+vn,obj->polygons[xa].shading);
					}
					parentlist=parentlist->next;
				}
			}

			break;
		}

	}
 obj_generatenormals(o);
}

/*void plane_imp_triangle_int_add_3d ( float x1, float y1, float z1, 
  float x2, float y2, float z2, float dist1, float dist2, 
  int *num_int, float x[], float y[], float z[] )
{
  float alpha;

  if ( fabs(dist1) <= 0.00001 ) {
    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;
  }
  else if ( fabs(dist2) <= 0.00001 ) {
    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;
  }
  else if ( dist1 * dist2 < 0.0 ) {
    alpha = dist2 / ( dist2 - dist1 );
    x[*num_int] = alpha * x1 + ( 1.0f - alpha ) * x2;
    y[*num_int] = alpha * y1 + ( 1.0f - alpha ) * y2;
    z[*num_int] = alpha * z1 + ( 1.0f - alpha ) * z2;
    *num_int = *num_int + 1;
  }

  return;
}

void plane_imp_triangle_int_3d ( float x1, float y1, float z1, float x2, 
  float y2, float z2, float x3, float y3, float z3, float a, float b, 
  float c, float d, int *num_int, float x[], float y[], float z[], float sides[], bool &split )

{
  split=false;
  float dist1;
  float dist2;
  float dist3;

  *num_int = 0;
  dist1 = a * x1 + b * y1 + c * z1 + d;
  dist2 = a * x2 + b * y2 + c * z2 + d;
  dist3 = a * x3 + b * y3 + c * z3 + d;
  if ( fabs(dist1) <= 0.00001 ) {

    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;

  }

  if ( fabs(dist2) <= 0.00001 ) {

    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;

  }

  if ( fabs(dist3) <= 0.00001 ) {

    x[*num_int] = x3;
    y[*num_int] = y3;
    z[*num_int] = z3;
    *num_int = *num_int + 1;

  }
  if ( *num_int >= 2 ) {
    return;
  }
  if ( *num_int == 1 ) {

    if ( fabs(dist1) <= 0.00001 ) {

      plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
        dist2, dist3, num_int, x, y, z );

    }
    else if ( fabs(dist2) <= 0.00001 ) {

      plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
        dist1, dist3, num_int, x, y, z );

    }
    else if ( fabs(dist3) <= 0.00001 ) {

      plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
        dist1, dist2, num_int, x, y, z );

    }

    return;

  }
  split=true;
  if ( dist1 * dist2 < 0.0 && dist1 * dist3 < 0.0 ) {

    sides[0]=1; //ab
    plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
      dist1, dist2, num_int, x, y, z );

	sides[1]=3; //ac
    plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
      dist1, dist3, num_int, x, y, z );

  }
  else if ( dist2 * dist1 < 0.0 && dist2 * dist3 < 0.0 ) {

	sides[0]=1; //ab
    plane_imp_triangle_int_add_3d ( x2, y2, z2, x1, y1, z1, 
      dist2, dist1, num_int, x, y, z );

	sides[1]=2; //bc
    plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
      dist2, dist3, num_int, x, y, z );

  }
  else if ( dist3 * dist1 < 0.0 && dist3 * dist2 < 0.0 ) {

	sides[0]=3; //ac
    plane_imp_triangle_int_add_3d ( x3, y3, z3, x1, y1, z1, 
      dist3, dist1, num_int, x, y, z );

	sides[1]=2; //bc
    plane_imp_triangle_int_add_3d ( x3, y3, z3, x2, y2, z2, 
      dist3, dist2, num_int, x, y, z );

  }

  return;
}

int checkplanetri(vector3 normal, vector3 planevert, vector3 a, vector3 b, vector3 c)
{
	float planedist=-normal.x*planevert.x-normal.y*planevert.y-normal.z*planevert.z;
	float x[3],y[3],z[3],sides[3];
	int num_int=0;
	bool split;
	plane_imp_triangle_int_3d ( a.x, a.y, a.z, 
		                        b.x, b.y, b.z, 
								c.x, c.y, c.z,  
								normal.x, normal.y, normal.z, planedist,
								&num_int, x, y, z, sides, split);
	if (num_int==3) return tri_onplane;
	if (num_int==2 && split) return tri_split;
	float ta;
	vector3 s;
	s=v3_add(v3_add(a,b),c);
	s.x/=3.0f;
	s.y/=3.0f;
	s.z/=3.0f;
	ta=normal.x*s.x+normal.y*s.y+normal.z*s.z+planedist;
	if (ta<0) return tri_back;
	return tri_front;
}

float vectorvsplanecheck(vector3 normal, vector3 planevert, vector3 dot)
{
	vector3 v=v3_sub(dot,planevert);
	return normal.x*v.x+normal.y*v.y+normal.z*v.z;
}

float gett(vector3 a, vector3 b, vector3 p)
{
	float d1=v3_len(v3_sub(b,a));
	float d2=v3_len(v3_sub(p,a));
	return d2/d1;
}

vector2 gettexc(vector2 a, vector2 b, float t)
{
	vector2 ret;
	ret.u=(b.u-a.u)*t+a.u;
	ret.v=(b.v-a.v)*t+a.v;
	return ret;
}

void cutplanepoly(vector3 normal, vector3 planevert, cpolygon p, cpolygon *&frontlist, cpolygon *&backlist)
{
	cpolygon *buffer;
	float planedist=-normal.x*planevert.x-normal.y*planevert.y-normal.z*planevert.z;
	float xx[3],yy[3],zz[3],sides[3];
	int num_int;
	bool split;
	plane_imp_triangle_int_3d ( p.a.x, p.a.y, p.a.z, 
		                        p.b.x, p.b.y, p.b.z, 
								p.c.x, p.c.y, p.c.z,  
								normal.x, normal.y, normal.z, planedist,
								&num_int, xx, yy, zz, sides, split);

	vector3 i1,i2;
	if (num_int==2)
	{
		i1.x=xx[0]; i1.y=yy[0]; i1.z=zz[0];
		i2.x=xx[1]; i2.y=yy[1]; i2.z=zz[1];
		vector2 i1t,i2t;

		if (sides[0]==1 && sides[1]==2) //ab,bc metszet
		{
			i1t=gettexc(p.data.t1,p.data.t2,gett(p.a,p.b,i1));
			i2t=gettexc(p.data.t2,p.data.t3,gett(p.b,p.c,i2));
			//i1t=i0;
			//i2t=i0;

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i1;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t2=i1t;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i2;
			buffer->c=p.c;
			buffer->data=p.data;
			buffer->data.t2=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			
			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=p.b;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			return;
		}
		else
		if (sides[0]==1 && sides[1]==3) //ab,ac metszet
		{
			i1t=gettexc(p.data.t1,p.data.t2,gett(p.a,p.b,i1));
			i2t=gettexc(p.data.t1,p.data.t3,gett(p.a,p.c,i2));

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i1;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t2=i1t;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}

			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=p.c;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			buffer->data.t2=p.data.t3;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			
			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=p.b;
			buffer->c=p.c;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			return;
		}
		else
		if (sides[0]==3 && sides[1]==2) //bc,ac metszet
		{
			i2t=gettexc(p.data.t2,p.data.t3,gett(p.b,p.c,i2));
			i1t=gettexc(p.data.t1,p.data.t3,gett(p.a,p.c,i1));

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i2;
			buffer->c=i1;
			buffer->data=p.data;
			buffer->data.t2=i2t;
			buffer->data.t3=i1t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}

			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=i2;
			buffer->c=p.c;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			buffer->data.t2=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			
			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=p.b;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			return;
		}
	}
}

bspnode *generatebsptree(cpolygon *inlist)
{
	bspnode *node=new bspnode;
	node->front=NULL;
	node->back=NULL;
	node->polylist=NULL;
	cpolygon *frontlist=NULL;
	cpolygon *backlist=NULL;

	node->planenormal=v3_normalize(v3_cross(v3_sub(inlist->b,inlist->a),
								   v3_sub(inlist->c,inlist->a)));
	node->planevertex=inlist->a;

	for (cpolygon *x=inlist; x; x=x->next)
	{
		switch (checkplanetri(node->planenormal,node->planevertex,x->a,x->b,x->c))
		{
		case tri_onplane:
			{
				//MessageBox( 0, "onplane", "HelloWorld", MB_OK );
				cpolygon *buffer=new cpolygon;
				*buffer=*x;
				buffer->next=node->polylist;
				node->polylist=buffer;

				//addpolytolist(&node->polylist,*x);
				break;
			}
		case tri_front:
			{
				//MessageBox( 0, "front", "HelloWorld", MB_OK );
				cpolygon *buffer=new cpolygon;
				*buffer=*x;
				buffer->next=frontlist;
				frontlist=buffer;

				//addpolytolist(&frontlist,*x);
				break;
			}
		case tri_back:
			{
				//MessageBox( 0, "back", "HelloWorld", MB_OK );
				cpolygon *buffer=new cpolygon;
				*buffer=*x;
				buffer->next=backlist;
				backlist=buffer;

				//addpolytolist(&backlist,*x);
				break;
			}
		case tri_split:
			{
				//MessageBox( 0, "split", "HelloWorld", MB_OK );
				cutplanepoly(node->planenormal,node->planevertex,*x,frontlist,backlist);
				break;
			}
		}
	}

	if (frontlist) node->front=generatebsptree(frontlist);
	if (backlist) node->back=generatebsptree(backlist);

	return node;
}

cpolygon *csgclippolygon(bspnode *node, cpolygon *poly, int side, bool &cut)
{
	int test=checkplanetri(node->planenormal,node->planevertex,poly->a,poly->b,poly->c);

	switch (test)
	{
	case tri_front:
		{
			if (node->front) return csgclippolygon(node->front,poly,side,cut);
			if (side==tri_front) {cut=false; return poly;}
			cut=true;
			return NULL;			
			break;
		}
	case tri_back:
		{
			if (node->back) return csgclippolygon(node->back,poly,side,cut);
			if (side==tri_back) {cut=false; return poly;}
			cut=true;
			return NULL;
			break;
		}
	case tri_split:
		{
			cpolygon *pFront=NULL, *pBack=NULL, *pRetFront=NULL, *pRetBack=NULL;
			cutplanepoly(node->planenormal, node->planevertex, *poly, pFront, pBack);
			bool pcut=false;
			bool ppcut;

			if (node->front) //front polykat lenyomni a front agon
			{
				for (cpolygon *x=pFront; x; x=x->next)
				{
					cpolygon *lowerpolys=csgclippolygon(node->front,x,side,ppcut);
					if (ppcut) pcut=true;
					for (cpolygon *y=lowerpolys; y; y=y->next)
					{
						cpolygon *buffer=new cpolygon;
						*buffer=*y;
						buffer->next=pRetFront;
						pRetFront=buffer;
					}
				}
			}
			else 
			{
				if (side==tri_front) pRetFront=pFront;
				else pcut=true;
			}

			if (node->back) //Back polykat lenyomni a Back agon
			{
				for (cpolygon *x=pBack; x; x=x->next)
				{
					cpolygon *lowerpolys=csgclippolygon(node->back,x,side,ppcut);
					if (ppcut) pcut=true;
					for (cpolygon *y=lowerpolys; y; y=y->next)
					{
						cpolygon *buffer=new cpolygon;
						*buffer=*y;
						buffer->next=pRetBack;
						pRetBack=buffer;
					}
				}
			}
			else 
			{
				if (side==tri_back) pRetBack=pBack;
				else pcut=true;
			}

			if (!pcut)
			{
				cut=false;
				return poly;
			}
			cut=true;

			cpolygon *rlist=NULL;
			for (cpolygon *c=pRetFront; c; c=c->next)
			{
				cpolygon *buffer=new cpolygon;
				*buffer=*c;
				buffer->next=rlist;
				rlist=buffer;
			}
			for (c=pRetBack; c; c=c->next)
			{
				cpolygon *buffer=new cpolygon;
				*buffer=*c;
				buffer->next=rlist;
				rlist=buffer;
			}
			return rlist;

			break;
		}
	}

	return poly;
}

cpolygon *getpolylist(object *o)
{
	cpolygon *list=NULL;
	for (int x=0; x<o->polygonnum; x++)
	{
		cpolygon *buffer=new cpolygon;
		buffer->a=o->vertices[o->polygons[x].v1].d;
		buffer->b=o->vertices[o->polygons[x].v2].d;
		buffer->c=o->vertices[o->polygons[x].v3].d;
		buffer->data=o->polygons[x];
		buffer->next=list;
		list=buffer;
	}
	return list;
}

void booleanpolymover(bspnode *bsptree, cpolygon *polylist, object *o, int function)
{
	bool cu;
	cpolygon c;
	for (cpolygon *a=polylist; a; a=a->next)
	{
		c=*a;
		c.next=NULL;
		cpolygon *r=csgclippolygon(bsptree,&c,function,cu);
		for (cpolygon *retlist=r; retlist; retlist=retlist->next)
		{
			addvertex(o,retlist->a.x,retlist->a.y,retlist->a.z);
			addvertex(o,retlist->b.x,retlist->b.y,retlist->b.z);
			addvertex(o,retlist->c.x,retlist->c.y,retlist->c.z);
			addpolygon(o,0,0,0,0);
			o->polygons[o->polygonnum-1]=retlist->data;
			o->polygons[o->polygonnum-1].d1=o->polygons[o->polygonnum-1].t1;
			o->polygons[o->polygonnum-1].d2=o->polygons[o->polygonnum-1].t2;
			o->polygons[o->polygonnum-1].d3=o->polygons[o->polygonnum-1].t3;
			if (function==tri_back)
			{
				o->polygons[o->polygonnum-1].v1=o->vertexnum-3;
				o->polygons[o->polygonnum-1].v2=o->vertexnum-2;
				o->polygons[o->polygonnum-1].v3=o->vertexnum-1;
			}
			else
			{
				o->polygons[o->polygonnum-1].v1=o->vertexnum-2;
				o->polygons[o->polygonnum-1].v2=o->vertexnum-3;
				o->polygons[o->polygonnum-1].v3=o->vertexnum-1;
			}
		}
	}
}

void obj_boolean(object *o1, object *o2, int function)
{
	matrix r,m;
	memcpy(r,o1->xformmatrix,sizeof(matrix));
	m_invert(r,m);

	for (int z=0; z<o2->vertexnum; z++)
	{
		o2->vertices[z].base=o2->vertices[z].d;
		m_xformd(m,o2->vertices[z].position,o2->vertices[z].d);
	}

	cpolygon *p1list=getpolylist(o1);
	cpolygon *p2list=getpolylist(o2);

	bspnode *bsp1=generatebsptree(p1list);
	bspnode *bsp2=generatebsptree(p2list);

	int o1func=tri_back, o2func=tri_back;
	switch (function)
	{
	case 1:
		{ 
			o2func=tri_front;
			break;
		}
	case 2:
		{ 
			o1func=tri_front;
			o2func=tri_front;
			break;
		}
	}

	o1->polygonnum=0;
	o1->vertexnum=0;

	booleanpolymover(bsp2,p1list,o1,o1func);
	booleanpolymover(bsp1,p2list,o1,o2func);

	obj_generatenormals(o1);
	//obj_transform(o1,o1->xformmatrix);

	for (z=0; z<o2->vertexnum; z++)	o2->vertices[z].d=o2->vertices[z].base;
	//obj_transform(o2,o2->xformmatrix);
}*/

void addvertexedge(vertex &obj, int v1)
{
	bool add=true;	
	for (int x=0;x<obj.vedgenum;x++) if (obj.vedgelist[x]==v1) add=false;
	if (add) 
	{
		if (obj.vedgecapacity==obj.vedgenum)
		{
			int *buffer = new int[(obj.vedgecapacity+10)*2];
			memset(buffer,0,sizeof(int)*(obj.vedgecapacity+10)*2);
			memcpy(buffer,obj.vedgelist,sizeof(int)*obj.vedgenum);
			int *b2 = obj.vedgelist;
			obj.vedgecapacity=(obj.vedgecapacity+10)*2;
			obj.vedgelist=buffer;
			delete[] b2;
		}
		obj.vedgelist[obj.vedgenum++]=v1;
	}
}

void addedge(object *obj, int v1, int v2, bool addtovertex)
{
    //char *ss = new char[256];
	//sprintf(s,"%d",texgenmenu);
	int xa;
	bool add=true;
	for (xa=0;xa<obj->edgenum;xa++)
	{
		if ((obj->edgelist[xa].v1==v1 && obj->edgelist[xa].v2==v2) ||
		    (obj->edgelist[xa].v2==v1 && obj->edgelist[xa].v1==v2)) add=false;
	}
	if (add)
	{
		if (obj->edgenum==obj->elistsize)
		{
			edge *newlist = new edge [(obj->elistsize+10)*2];
			memset(newlist,0,(obj->elistsize+10)*2*sizeof(edge));
			memcpy(newlist,obj->edgelist,obj->elistsize*sizeof(edge));
			delete obj->edgelist;
			obj->edgelist=newlist;
			obj->elistsize=(obj->elistsize+10)*2;
		}
		obj->edgelist[obj->edgenum].v1=v1;
		obj->edgelist[obj->edgenum].v2=v2;
		if (addtovertex) addvertexedge(obj->vertices[v1],obj->edgenum);
		if (addtovertex) addvertexedge(obj->vertices[v2],obj->edgenum);
		obj->edgenum++;
	}

}

int searchedge(object *obj, int v1, int v2)
{
	int r=-1,xi;
	for (xi=0;xi<obj->edgenum;xi++)
	{
		if (((obj->edgelist[xi].v1==v1) && (obj->edgelist[xi].v2==v2)) || 
		   ((obj->edgelist[xi].v2==v1) && (obj->edgelist[xi].v1==v2))) r=xi;
	}
	return r;
}

int getedgevertex(object *o, int badvertex, int edgeid)
{
	int b;
	b=-1;
	if (o->edgelist[edgeid].v1==badvertex) b=o->edgelist[edgeid].v2; else b=o->edgelist[edgeid].v1;
	return b;
}

void sortvertexedges(object *obj)
{
	int x,z;
	int y;
	for (x=0; x<obj->vertexnum; x++)
	{
		vertex buffer=obj->vertices[x];
		if (buffer.vedgenum>2)
		{
		vector3 vertvector;
		vector3 normal = obj->vertices[x].defaultnormal;
		vector3 vx = obj->vertices[x].d;
		float *szogek = new float[buffer.vedgenum];
		memset(szogek,0,sizeof(float)*buffer.vedgenum);

		int e=getedgevertex(obj,x,buffer.vedgelist[0]);
		vector3 refvector=obj->vertices[e].d;
		refvector = v3_sub(refvector,vx);

		vector3 projvec=v3_normalize(normal);
		projvec=v3_mults(projvec,v3_dot(refvector,normal));
		refvector = v3_normalize(v3_sub(refvector,projvec));

		for (y=1;y<buffer.vedgenum;y++)
		{
			e=getedgevertex(obj,x,buffer.vedgelist[y]);
			vertvector = v3_sub(obj->vertices[e].d,vx);
			projvec = normal;
			projvec=v3_mults(projvec,v3_dot(vertvector,normal));
			vertvector = v3_normalize(v3_sub(vertvector,projvec));

			float cosTheta = v3_dot(vertvector,refvector);
			if (cosTheta < -1.0f) cosTheta = -1.0f;
			if (cosTheta > 1.0f) cosTheta = 1.0f;
			float angle = (float)acos(cosTheta);
			angle *= (float)(180.0f / pi);

			vector3 crossProd=v3_cross(vertvector,refvector);
			if (v3_dot(crossProd,normal) < 0)
			{
				angle = 360 - angle;
			}

			szogek[y]=angle;

		}
		int smallest; float smallestvalue;
		int b1; float b2;
		char *ss = new char[256];
		for (z=0;z<buffer.vedgenum;z++)
		{
			smallest=z;
			smallestvalue=szogek[z];
			for (y=z;y<buffer.vedgenum;y++)
			{
				if (szogek[y]<=smallestvalue)
				{
					smallest=y;
					smallestvalue=szogek[y];
				}
			}
			b2=szogek[smallest];
			szogek[smallest]=szogek[z];
			szogek[z]=b2;
			b1=obj->vertices[x].vedgelist[smallest];
			obj->vertices[x].vedgelist[smallest]=obj->vertices[x].vedgelist[z];
			obj->vertices[x].vedgelist[z]=b1;
		}

		delete[] szogek;
		}
	}
}

void generateedges(object *obj)
{
	int xa;
	//if (obj->edgenum==0)
	{
		obj->edgenum=0;
		for (xa=0; xa<obj->vertexnum; xa++)
		{
			obj->vertices[xa].vedgenum=0;
		}
		for (xa=0; xa<obj->polygonnum; xa++)
		{
			addedge(obj,obj->polygons[xa].v1,obj->polygons[xa].v2,true);
			obj->polygons[xa].pedgelist[0]=searchedge(obj,obj->polygons[xa].v1,obj->polygons[xa].v2);
			addedge(obj,obj->polygons[xa].v1,obj->polygons[xa].v3,true);
			obj->polygons[xa].pedgelist[1]=searchedge(obj,obj->polygons[xa].v1,obj->polygons[xa].v3);
			addedge(obj,obj->polygons[xa].v2,obj->polygons[xa].v3,true);
			obj->polygons[xa].pedgelist[2]=searchedge(obj,obj->polygons[xa].v2,obj->polygons[xa].v3);
		}
		sortvertexedges(obj);
	}
}

vector3 getbadvector(object *o, int vxid, int edgeid, float w, bool hh)
{
	int e0=getedgevertex(o,vxid,edgeid);

	vector3 buffer;
	memset(&buffer,0,sizeof(vector3));
	int edgevertexposition=-1; //position of the edge in the vertex's edge array
	int x;
	for (x=0;x<o->vertices[vxid].vedgenum;x++) if (o->vertices[vxid].vedgelist[x]==edgeid) edgevertexposition=x;
	buffer=v3_mults(o->vertices[vxid].d,3.0f/4.0f);

	switch (o->vertices[vxid].vedgenum)
	{
	case 2:	{
		return v3_add(v3_mults(o->vertices[vxid].d,0.5),
			                  v3_mults(o->vertices[e0].d,0.5));
			}
	case 3: {
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,5.0f/12.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,-1.0f/12.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,-1.0f/12.0f));
		return buffer;
		break;
			}
	case 4: {
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,3.0f/8.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,-1.0f/8.0f));
		return buffer;
		break;
			}
	case 6: {
		//buffer=v3_mults(o->vertices[vxid].d,1.0f/2.0f);
		memset(&buffer,0,sizeof(vector3));
		buffer=v3_mults(o->vertices[e0].d,1.0f/2.0f);

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		if (hh)	buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,1.0f/8.0f+2*w));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,-1.0f/16.0f-w));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,w));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,-1.0f/16.0f-w));

		edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
		edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		if (hh)	buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,1.0f/8.0f+2*w));

		return buffer;
		break;
			}
	default: {
		for (x=0;x<o->vertices[vxid].vedgenum;x++)
		{
			int n=o->vertices[vxid].vedgenum;
			float val=(float)((0.25f+cos(2*pi*(float)x/(float)n)+0.5*cos(4*pi*(float)x/(float)n))/(float)n);
			buffer=v3_add(buffer,v3_mults(o->vertices[e0].d,val));
			edgevertexposition=(edgevertexposition+1)%o->vertices[vxid].vedgenum;
			edgeid=o->vertices[vxid].vedgelist[edgevertexposition];
			e0=getedgevertex(o,vxid,edgeid);
		}

		return buffer;
		break;
			}
	}

	return buffer;
}

vector3 butterflytessellation(object *o, int e, float w)
{
	vector3 buffer;
	memset(&buffer,0,sizeof(vector3));
	vector3 a,b;

	edge e_=o->edgelist[e];
	vertex a1,a2;
	a1=o->vertices[e_.v1];
	a2=o->vertices[e_.v2];

	if (a1.vedgenum==6 && a2.vedgenum==6)
	{
		a=getbadvector(o,e_.v1,e,w,false);
		b=getbadvector(o,e_.v2,e,w,true);
		return v3_add(a,b);
	}
	else
	{
		if (a1.vedgenum!=6) a=getbadvector(o,e_.v1,e,w,false);
		if (a2.vedgenum!=6) b=getbadvector(o,e_.v2,e,w,false);
		if (a1.vedgenum==6) return b;
		if (a2.vedgenum==6) return a;
		if (a1.vedgenum!=6 && a2.vedgenum!=6) return v3_add(v3_mults(a,0.5f),v3_mults(b,0.5f));
	}

	return buffer;
}

void setlastvertextexcoord(object *obj, float y, float x)
{
	obj->vertices[obj->vertexnum-1].t.u=x;
	obj->vertices[obj->vertexnum-1].t.v=y;
	obj->vertices[obj->vertexnum-1].dt=obj->vertices[obj->vertexnum-1].t;
}

void tessellateEdges(object *obj, int* edgeVertMap,int* edgeEdgeMap, float w, bool linear)
{
	int xi;
	
	for (xi=obj->edgenum-1;xi>=0;xi--)
	{
		vector3 newvertex,newnormal,v1,v2,n1,n2;
		v1=obj->vertices[obj->edgelist[xi].v1].d;
		v2=obj->vertices[obj->edgelist[xi].v2].d;
		n1=obj->vertices[obj->edgelist[xi].v1].defaultnormal;
		n2=obj->vertices[obj->edgelist[xi].v2].defaultnormal;

		if (linear)
			newvertex=v3_add(v3_mults(v1,0.5f),v3_mults(v2,0.5f));
		else
			newvertex=butterflytessellation(obj,xi,w);
		newnormal.x=(n1.x+n2.x)/2.0f;
		newnormal.y=(n1.y+n2.y)/2.0f;
		newnormal.z=(n1.z+n2.z)/2.0f;
		
		addvertex(obj,newvertex.x,newvertex.y,newvertex.z);
		setlastvertextexcoord(obj,(obj->vertices[obj->edgelist[xi].v1].t.v+
			                      obj->vertices[obj->edgelist[xi].v2].t.v)/2,
								  (obj->vertices[obj->edgelist[xi].v1].t.u+
			                      obj->vertices[obj->edgelist[xi].v2].t.u)/2);

		edgeVertMap[xi]=obj->vertexnum-1;
		edgeEdgeMap[xi]=obj->edgenum-1;
	}
}

void setlastpolytexcoord(object *obj, float y1, float x1, float y2, float x2, float y3, float x3)
{
	obj->polygons[obj->polygonnum-1].t1.u=x1;
	obj->polygons[obj->polygonnum-1].t1.v=y1;
	obj->polygons[obj->polygonnum-1].d1=obj->polygons[obj->polygonnum-1].t1;
	obj->polygons[obj->polygonnum-1].t2.u=x2;
	obj->polygons[obj->polygonnum-1].t2.v=y2;
	obj->polygons[obj->polygonnum-1].d2=obj->polygons[obj->polygonnum-1].t2;
	obj->polygons[obj->polygonnum-1].t3.u=x3;
	obj->polygons[obj->polygonnum-1].t3.v=y3;
	obj->polygons[obj->polygonnum-1].d3=obj->polygons[obj->polygonnum-1].t3;
}

void buildNewFaces(object *obj, int* edgeVertMap,int* edgeEdgeMap, bool linear)
{
	int xi;
	
	for (xi=obj->polygonnum-1;xi>=0;xi--)
	{
		int a,b,c,d,e,f;
		vector2 ta,tb,tc,td,te,tf;
		a=obj->polygons[xi].v1;
		b=obj->polygons[xi].v2;
		c=obj->polygons[xi].v3;
		d=edgeVertMap[obj->polygons[xi].pedgelist[0]];
		e=edgeVertMap[obj->polygons[xi].pedgelist[1]];
		f=edgeVertMap[obj->polygons[xi].pedgelist[2]];
		ta=obj->polygons[xi].t1;
		tb=obj->polygons[xi].t2;
		tc=obj->polygons[xi].t3;
		td.u=(ta.u+tb.u)/2.0f;
		td.v=(ta.v+tb.v)/2.0f;
		te.u=(ta.u+tc.u)/2.0f;
		te.v=(ta.v+tc.v)/2.0f;
		tf.u=(tb.u+tc.u)/2.0f;
		tf.v=(tb.v+tc.v)/2.0f;
		obj->polygons[xi].v2=d;
		obj->polygons[xi].t2=td;
		obj->polygons[xi].d2=obj->polygons[xi].t2;
		obj->polygons[xi].v3=e;
		obj->polygons[xi].t3=te;
		obj->polygons[xi].d3=obj->polygons[xi].t3;
		if (!linear) obj->polygons[xi].shading=aDDict_gouraudshade;

		addpolygon(obj,d,b,f,obj->polygons[xi].shading);
		obj->polygons[obj->polygonnum-1].texturehandle=obj->polygons[xi].texturehandle;
		obj->polygons[obj->polygonnum-1].envmaphandle=obj->polygons[xi].envmaphandle;
		if (!linear) obj->polygons[obj->polygonnum-1].shading=aDDict_gouraudshade;
		setlastpolytexcoord(obj,td.v,td.u,tb.v,tb.u,tf.v,tf.u);
		obj->polygons[obj->polygonnum-1].color=obj->polygons[xi].color;
		
		addpolygon(obj,e,f,c,obj->polygons[xi].shading);
		obj->polygons[obj->polygonnum-1].texturehandle=obj->polygons[xi].texturehandle;
		obj->polygons[obj->polygonnum-1].envmaphandle=obj->polygons[xi].envmaphandle;
		if (!linear) obj->polygons[obj->polygonnum-1].shading=aDDict_gouraudshade;
		setlastpolytexcoord(obj,te.v,te.u,tf.v,tf.u,tc.v,tc.u);
		obj->polygons[obj->polygonnum-1].color=obj->polygons[xi].color;
		
		addpolygon(obj,e,d,f,obj->polygons[xi].shading);
		obj->polygons[obj->polygonnum-1].texturehandle=obj->polygons[xi].texturehandle;
		obj->polygons[obj->polygonnum-1].envmaphandle=obj->polygons[xi].envmaphandle;
		if (!linear) obj->polygons[obj->polygonnum-1].shading=aDDict_gouraudshade;
		setlastpolytexcoord(obj,te.v,te.u,td.v,td.u,tf.v,tf.u);
		obj->polygons[obj->polygonnum-1].color=obj->polygons[xi].color;
	}

}

void butterflysubdivision(object *obj, bool linear)
{
	generateedges(obj);
	int xa;
	
	int* edgeVertMap = new int[obj->edgenum];
	for (xa=0; xa<obj->edgenum; xa++)
        {
            edgeVertMap[xa] = -1;
        }
	int* edgeEdgeMap = new int[obj->edgenum];
	for (xa=0; xa<obj->edgenum; xa++)
	{
		edgeEdgeMap[xa] = -1;
	}

	tessellateEdges(obj,edgeVertMap,edgeEdgeMap, 0.0f, linear);
	buildNewFaces(obj,edgeVertMap, edgeEdgeMap, linear);

	delete[] edgeVertMap;
	delete[] edgeEdgeMap;

	obj_generatenormals(obj);
	obj_transform(obj,obj->xformmatrix);
}

void meshblur(object *obj)
{
	generateedges(obj);
	vertex *newvlist = new vertex [obj->vertexnum];
	memcpy(newvlist,obj->vertices,sizeof(vertex)*obj->vertexnum);
	int x;
	for (x=0;x<obj->vertexnum;x++)
	{
		vector3 buffer;
		float dw=2;
		buffer=v3_mults(obj->vertices[x].d,2);
		for (int y=0;y<newvlist[x].vedgenum;y++)
		{
			float w;
			int vx=getedgevertex(obj, x, newvlist[x].vedgelist[y]);
			/*vector3 v=v3_sub(obj->vertices[x].d,obj->vertices[vx].d);
			w=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
			w*=w;
			w=1-w;
			if (w<0) w=0;*/
			w=1;
			dw+=w;
			buffer=v3_mults(v3_add(buffer,obj->vertices[vx].d),w);
		}

		buffer=v3_mults(buffer,1.0f/dw);
		newvlist[x].d=buffer;
		newvlist[x].base=buffer;
	}
	for (x=0; x<obj->polygonnum; x++)
	{
		obj->polygons[x].shading=aDDict_gouraudshade;
	}
	memcpy(obj->vertices,newvlist,sizeof(vertex)*obj->vertexnum);
	obj_generatenormals(obj);
	obj_transform(obj,obj->xformmatrix);
}
