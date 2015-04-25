#include "intro.h"
#include "introgen.h"

char boxvertices[] =
	{  -5,-5,-5, 5,-5,-5, 5, 5,-5,-5, 5,-5, 0, 0,-5, 5,-5, 5, 5, 5, 5, 5, 0, 0,
	   -5, 5, 5,-5,-5, 5, 0, 0, 5,-5, 0, 0,	0,-5, 0, 0, 5, 0 };
char boxpolygons[] =
	{	0,1,4,1,2,4,2,3,4,3,0,4,1,5,7,5,6,7,6,2,7,2,1,7,5,9,10,9,8,10,8,6,10,6,5,10,
		9,0,11,0,3,11,3,8,11,8,9,11,9,5,12,5,1,12,1,0,12,0,9,12,3,2,13,2,6,13,6,8,13,8,3,13 };
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
		center=o->vertices[o->polygons[x].v1].d;
		v0 = v3_sub(o->vertices[o->polygons[x].v2].d,center);
		v1 = v3_sub(o->vertices[o->polygons[x].v3].d,center);

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
					
					int xa;
					for (xa=0;xa<obj->vertexnum;xa++)
					{
						addvertex(o,0,0,0);
						o->vertices[o->vertexnum-1]=obj->vertices[xa];
						o->vertices[o->vertexnum-1].dt=obj->vertices[xa].t;
						o->vertices[o->vertexnum-1].base=obj->vertices[xa].d;
						m_xformd(obj->xformmatrix,obj->vertices[xa].d,o->vertices[o->vertexnum-1].d);
						m_xform3(obj->xformmatrix,obj->vertices[xa].defaultnormal,o->vertices[o->vertexnum-1].defaultnormal);
						o->vertices[o->vertexnum-1].base=o->vertices[o->vertexnum-1].d;
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
					}
					parentlist=parentlist->next;
				}
			}

			break;
		}

	}
 obj_generatenormals(o);
}
