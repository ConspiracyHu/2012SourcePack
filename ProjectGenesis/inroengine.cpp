#include "introengine.h"

scene *scenelist=NULL;
texture *texturelist=NULL;
material *materiallist=NULL;
event *eventlist=NULL,*lastevent=NULL;
rgba maptexture[256][256];
scrshot shots[20];
bool calculatehierarchy[50];

texture *ptexturelist=NULL;
material *pmateriallist=NULL;
scene *pscenelist=NULL;
event *peventlist=NULL, *plastevent=NULL;

object newobject()
{
	object o;
	memset(&o,0,sizeof(o));
	o.texxscale=1;
	o.texyscale=1;
	o.color[0]=1;
	o.color[1]=1;
	o.color[2]=1;
	o.color[3]=1;
	return o;
}

scene *newscene()
{
	scene *s=new scene;
	memset(s,0,sizeof(scene));
	for (int x=0; x<7; x++)
		s->lights[x].identifier=GL_LIGHT0+x;
	return s;
}

void addvertex(object *o, float x, float y, float z)
{
	if (o->vertexcapacity==o->vertexnum)
	{
		vertex *v=new vertex[(o->vertexnum+10)*2];
		memset(v,0,sizeof(vertex)*(o->vertexnum+10)*2);
		memcpy(v,o->vertices,sizeof(vertex)*o->vertexnum);
		o->vertices=v;
		o->vertexcapacity=(o->vertexnum+10)*2;
	}
	o->vertices[o->vertexnum].d=v3_make(x,y,z);
	o->vertices[o->vertexnum].base=o->vertices[o->vertexnum].d;
	o->vertices[o->vertexnum].position=o->vertices[o->vertexnum].d;
	o->vertexnum++;
}

void addpolygon(object *o, int v1, int v2, int v3, int shading)
{
	if (o->polygoncapacity==o->polygonnum)
	{
		polygon *v=new polygon[(o->polygonnum+10)*2];
		memset(v,0,sizeof(polygon)*(o->polygonnum+10)*2);
		memcpy(v,o->polygons,sizeof(polygon)*o->polygonnum);
		o->polygons=v;
		o->polygoncapacity=(o->polygonnum+10)*2;
	}
	o->polygons[o->polygonnum].t1=o->vertices[v1].t;
	o->polygons[o->polygonnum].t2=o->vertices[v2].t;
	o->polygons[o->polygonnum].t3=o->vertices[v3].t;
	o->polygons[o->polygonnum].d1=o->polygons[o->polygonnum].t1;
	o->polygons[o->polygonnum].d2=o->polygons[o->polygonnum].t2;
	o->polygons[o->polygonnum].d3=o->polygons[o->polygonnum].t3;
	o->polygons[o->polygonnum].v1=v1;
	o->polygons[o->polygonnum].v2=v2;
	o->polygons[o->polygonnum].v3=v3;
	o->polygons[o->polygonnum].color.x=1;
	o->polygons[o->polygonnum].color.y=1;
	o->polygons[o->polygonnum].color.z=1;
	o->polygons[o->polygonnum].color.w=1;
	o->polygons[o->polygonnum++].shading=shading;
}

void addobject(scene &s)
{
	if (s.objectcapacity==s.objectnum)
	{
		object *v=new object[(s.objectnum+10)*2];
		memset(v,0,sizeof(object)*(s.objectnum+10)*2);
		memcpy(v,s.objects,sizeof(object)*s.objectnum);
		s.objects=v;
		s.objectcapacity=(s.objectnum+10)*2;
	}
	s.objects[s.objectnum]=newobject();
	s.objectnum++;
}

object *searchobjectbynumber(object *list, int listsize, int id)
{
	for (int x=0; x<listsize; x++) if (list[x].number==id) return &list[x];
	return NULL;
}

int selsize(selection *scn)
{
	int a=0;
	selection *b=scn;
	while (b!=NULL)
	{
		a++;
		b=b->next;
	}
	return a;
}

material *findmaterial(int num)
{
	material *pt=materiallist;
	while (pt!=NULL) 
	{
		if (pt->number==num) return pt;
		pt=pt->next;
	}
	return NULL;
}

texture *findtexture(int num)
{
	texture *pt=texturelist;
	while (pt!=NULL) 
	{
		if (pt->ID==num) return pt;
		pt=pt->next;
	}
	return NULL;
}

scene *findscene(int num)
{
	scene *pt=scenelist;
	while (pt!=NULL) 
	{
		if (pt->number==num) return pt;
		pt=pt->next;
	}
	return NULL;
}

camera *findcam(scene *s,int num)
{
	camera *pt=s->cameras;
	while (pt!=NULL) 
	{
		if (pt->number==num) return pt;
		pt=pt->next;
	}
	return NULL;
}


void obj_transform(object *obj, matrix m)
{
	memcpy(*obj->xformmatrix,*m,sizeof(matrix));
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
		{
			obj->glmatrix[x+y*4]=obj->xformmatrix[x][y];
		}
}

float cam[16];

void setlighting(scene *scn)
{
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	int l;
	for (l=0;l<=7;l++)
		if (scn->lights[l].turnedon)
		{
			glEnable(scn->lights[l].identifier);
			glLightfv(scn->lights[l].identifier,GL_AMBIENT,scn->lights[l].ambient);
			glLightfv(scn->lights[l].identifier,GL_DIFFUSE,scn->lights[l].color);
			glLightfv(scn->lights[l].identifier,GL_SPECULAR,scn->lights[l].color);
			glLightfv(scn->lights[l].identifier,GL_POSITION,scn->lights[l].position);
			glLightfv(scn->lights[l].identifier,GL_SPOT_DIRECTION,scn->lights[l].spot_direction);
			glLightfv(scn->lights[l].identifier,GL_SPOT_EXPONENT,&scn->lights[l].spot_exponent);
			glLightfv(scn->lights[l].identifier,GL_SPOT_CUTOFF,&scn->lights[l].spot_cutoff);
			glLightfv(scn->lights[l].identifier,GL_CONSTANT_ATTENUATION,&scn->lights[l].c_att);
			glLightfv(scn->lights[l].identifier,GL_LINEAR_ATTENUATION,&scn->lights[l].l_att);
			glLightfv(scn->lights[l].identifier,GL_QUADRATIC_ATTENUATION,&scn->lights[l].q_att);
		}
		else
		{
			glDisable(scn->lights[l].identifier);
		}
}

void drawpoly(object *o, int poly)
{
	if (o->polygons[poly].shading==aDDict_flatshade) glNormal3f(o->polygons[poly].defaultnormal.x,o->polygons[poly].defaultnormal.y,o->polygons[poly].defaultnormal.z);

	glTexCoord2f(o->polygons[poly].t1.u,o->polygons[poly].t1.v);
	if (o->polygons[poly].shading==aDDict_gouraudshade) glNormal3f(o->vertices[o->polygons[poly].v1].defaultnormal.x,o->vertices[o->polygons[poly].v1].defaultnormal.y,o->vertices[o->polygons[poly].v1].defaultnormal.z);
	glVertex3f(o->vertices[o->polygons[poly].v1].d.x,o->vertices[o->polygons[poly].v1].d.y,o->vertices[o->polygons[poly].v1].d.z);
	glTexCoord2f(o->polygons[poly].t2.u,o->polygons[poly].t2.v);
	if (o->polygons[poly].shading==aDDict_gouraudshade) glNormal3f(o->vertices[o->polygons[poly].v2].defaultnormal.x,o->vertices[o->polygons[poly].v2].defaultnormal.y,o->vertices[o->polygons[poly].v2].defaultnormal.z);
	glVertex3f(o->vertices[o->polygons[poly].v2].d.x,o->vertices[o->polygons[poly].v2].d.y,o->vertices[o->polygons[poly].v2].d.z);
	glTexCoord2f(o->polygons[poly].t3.u,o->polygons[poly].t3.v);
	if (o->polygons[poly].shading==aDDict_gouraudshade) glNormal3f(o->vertices[o->polygons[poly].v3].defaultnormal.x,o->vertices[o->polygons[poly].v3].defaultnormal.y,o->vertices[o->polygons[poly].v3].defaultnormal.z);
	glVertex3f(o->vertices[o->polygons[poly].v3].d.x,o->vertices[o->polygons[poly].v3].d.y,o->vertices[o->polygons[poly].v3].d.z);
}

void renderscene(scene *Scene)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	GLuint lasthandle=256;
	bool textureon=false;
	if (Scene->fog)
	{
		glFogf(GL_FOG_DENSITY,Scene->fogdensity);
		glFogfv(GL_FOG_COLOR,Scene->fogcol);
		glEnable(GL_FOG);
	}
	setlighting(Scene);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glColor4f(1,1,1,1);
	int x;
	for (x=0; x<Scene->objectnum; x++)
	{
		object o=Scene->objects[x];
		glLoadMatrixf(cam);
		glMultMatrixf(o.glmatrix);
		if (o.polygonnum>=1)
		{
			glMaterialfv(GL_FRONT,GL_DIFFUSE,o.color);
			glCallList(o.solidlist);
		}
	}
	for (x=0; x<Scene->objectnum; x++)
	{
		object o=Scene->objects[x];
		glLoadMatrixf(cam);
		glMultMatrixf(o.glmatrix);
		if (o.polygonnum>=1)
		{
			glMaterialfv(GL_FRONT,GL_DIFFUSE,o.color);
			glCallList(o.translist);
		}
	}

	for (x=0; x<Scene->objectnum; x++)
	{
		object o=Scene->objects[x];
		glLoadMatrixf(cam);
		glMultMatrixf(o.glmatrix);
		if (o.polygonnum>=1)
		{
			glMaterialfv(GL_FRONT,GL_DIFFUSE,o.color);
			glCallList(o.envlist);
		}
	}

	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setcameraview(camera c, float aspectratio)
{
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(c.fov,aspectratio,0.1,500.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 vector3 dir=v3_normalize(v3_sub(c.target,c.eye));
 matrix m;
 m_rotate(dir.x,dir.y,dir.z,(float)c.roll*360.0f/255.0f*(float)radtheta,m);
 vector3 u;
 m_xformd(m,c.up,u);
 gluLookAt(c.eye.x,c.eye.y,c.eye.z,
           c.target.x,c.target.y,c.target.z,
           u.x,u.y,u.z);
 glGetFloatv(GL_MODELVIEW_MATRIX,cam);
}

void initintroengine()
{
	byte *n=new byte[256*256*4];
	memset(n,0,256*256*4);
	for (int x=0; x<20; x++)
	{
		glGenTextures(1,&shots[x].texture);
		glBindTexture(GL_TEXTURE_2D,shots[x].texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,n);
	}
	InitEffect( 0, 1.0f-480.0f/1024.0f, 640.0f/1024.0f, 1);
}

void generateprojectlists()
{
	for (scene *s=scenelist;s;s=s->next)
	{
		for (int i=0;i<s->objectnum;i++)
		{
			GLuint lasthandle=256;
			bool textureon=false;
			s->objects[i].solidlist=glGenLists(1);
			s->objects[i].translist=glGenLists(1);
			s->objects[i].envlist=glGenLists(1);
			object o=s->objects[i];

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glNewList(s->objects[i].solidlist,GL_COMPILE);

			glColor4f(1,1,1,1);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glBegin(GL_TRIANGLES);
			int y;
			for (y=0; y<o.polygonnum; y++)
				if (o.polygons[y].color.w==1.0)
			{
				if (o.data.textured && o.polygons[y].texturehandle!=lasthandle)
				{
					textureon=true;
					lasthandle=o.polygons[y].texturehandle;
					glEnd();
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,lasthandle);
					glBegin(GL_TRIANGLES);
				}
				if (textureon && !o.polygons[y].texturehandle)
				{
					glEnd();
					textureon=false;
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_TRIANGLES);
				}
				
					drawpoly(&o,y);
			}
			glEnd();
			glEndList();
			glNewList(s->objects[i].translist,GL_COMPILE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glDepthMask(false);
			glColor4f(1,1,1,1);
			lasthandle=256;
			glBegin(GL_TRIANGLES);
			for (y=0; y<o.polygonnum; y++)
				if (o.polygons[y].color.w!=1.0)
			{
				
				if (o.data.textured && o.polygons[y].texturehandle!=lasthandle)
				{
					textureon=true;
					lasthandle=o.polygons[y].texturehandle;
					glEnd();
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,lasthandle);
					glBegin(GL_TRIANGLES);
				}
				if (textureon && !o.polygons[y].texturehandle)
				{
					glEnd();
					textureon=false;
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_TRIANGLES);
				}				
					drawpoly(&o,y);
			}
			glEnd();
			glEndList();
			glNewList(s->objects[i].envlist,GL_COMPILE);
			lasthandle=256;
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_TEXTURE_GEN_S); 
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glColor4f(1,1,1,1);
			glBegin(GL_TRIANGLES);
			if (o.data.material2set)
			{
				for (y=0; y<o.polygonnum; y++)
				{
					if (o.data.material2set && o.polygons[y].envmaphandle!=lasthandle)
					{
						lasthandle=o.polygons[y].envmaphandle;
						glEnd();
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D,lasthandle);
						glBegin(GL_TRIANGLES);
					}
					drawpoly(&o,y);
				}
			}
			glEnd();
			glEndList();
			glPopAttrib();			
		}
	}
}
