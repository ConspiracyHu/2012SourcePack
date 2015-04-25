#include "introengine.h"

camera upcam,leftcam,frontcam,perspcam;
vector3 origo;
char *ss = new char[256];
float *color = new float[4];
GLuint actualtexture=0;
rgba maptexture[256][256];
unsigned int v;

int selnum(selectionlist *scn)
{
	int a=0;
	selectionlist *b=scn;
	while (b!=NULL)
	{
		a++;
		b=b->next;
	}
	return a;
}

int selsize(SELECTION *scn)
{
	int a=0;
	SELECTION *b=scn;
	while (b!=NULL)
	{
		a++;
		b=b->next;
	}
	return a;
}

void init3dengine()
{
	memset(&upcam,0,sizeof(upcam));
	memset(&leftcam,0,sizeof(upcam));
	memset(&frontcam,0,sizeof(upcam));
	memset(&perspcam,0,sizeof(upcam));
	frontcam.up.b.y=-1;
	frontcam.eye.b.z=-10;
	frontcam.fov=45;
	upcam.up.b.z=1;
	upcam.eye.b.y=-10;
	upcam.fov=45;
	leftcam.up.b.y=-1;
	leftcam.eye.b.x=-10;
	leftcam.fov=45;
	perspcam.up.b.y=-1;
	perspcam.eye.b.y=-7;
	perspcam.eye.b.z=-10;
	perspcam.fov=45;
	/*perspcam.up.b.z=1;
	perspcam.eye.b.y=-1;
	perspcam.eye.b.z=0;
	perspcam.fov=45;*/
	origo.b.x=0;
	origo.b.y=0;
	origo.b.z=0;
}

void setcameraview(camera c, float aspectratio)
{
 mode3d=true;
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(c.fov,aspectratio,0.1,200.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(c.eye.b.x,c.eye.b.y,c.eye.b.z,
           c.target.b.x,c.target.b.y,c.target.b.z,
           c.up.b.x,c.up.b.y,c.up.b.z);
}


void addvertex(object *obj, double x, double y, double z)
{
	if (obj->vertexnum==obj->vlistsize)
	{
		vertex *newlist = new vertex [(obj->vlistsize+10)*2];
		memset(newlist,0,(obj->vlistsize+10)*2*sizeof(vertex));
		memcpy(newlist,obj->vertexlist,obj->vlistsize*sizeof(vertex));
		delete obj->vertexlist;
		obj->vertexlist=newlist;
		obj->vlistsize=(obj->vlistsize+10)*2;
	}
	
	vertex buffer;
	memset(&buffer,0,sizeof(vertex));
	buffer.v.b.x=(float)x;
	buffer.v.b.y=(float)y;
	buffer.v.b.z=(float)z;
	buffer.d=buffer.v;
	buffer.base=buffer.v;
	buffer.generated=buffer.v;
	buffer.generatednormal=buffer.normal;
    obj->vertexlist[obj->vertexnum]=buffer;
	obj->vertexnum++;
}

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
		if (addtovertex) addvertexedge(obj->vertexlist[v1],obj->edgenum);
		if (addtovertex) addvertexedge(obj->vertexlist[v2],obj->edgenum);
		obj->edgenum++;
	}

}

void addpolygon(object *obj, int v1, int v2, int v3, int shading)
{
	if (obj->polynum==obj->plistsize)
	{
		polygon *newlist = new polygon [(obj->plistsize+10)*2];
		memset(newlist,0,(obj->plistsize+10)*2*sizeof(polygon));
		memcpy(newlist,obj->polygonlist,obj->plistsize*sizeof(polygon));
		delete obj->polygonlist;
		obj->polygonlist=newlist;
		obj->plistsize=(obj->plistsize+10)*2;
	}
	
	polygon buffer;
	memset(&buffer,0,sizeof(polygon));
	buffer.v1=v1;
	buffer.v2=v2;
	buffer.v3=v3;
	buffer.shading=shading;
	buffer.defshading=shading;
	buffer.t1=obj->vertexlist[v1].t;
	buffer.t2=obj->vertexlist[v2].t;
	buffer.t3=obj->vertexlist[v3].t;
	obj->polygonlist[obj->polynum]=buffer;
	obj->polynum++;
}

object *newobject()
{
	object *buffer = new object;

	memset(buffer,0,sizeof(object));
	buffer->next=NULL;
	buffer->vertexlist=NULL;
	buffer->polygonlist=NULL;
	buffer->polynum=0;
	buffer->vertexnum=0;
	buffer->name=new char[256];
	buffer->objdata.color[0]=255;
	buffer->objdata.color[1]=255;
	buffer->objdata.color[2]=255;
	buffer->objdata.color[3]=255;
	buffer->clones=NULL;
	buffer->lastclone=NULL;
	buffer->cloneparents=NULL;
	buffer->effects=NULL;
	buffer->objdata.textured=false;
	buffer->edgelist=NULL;
	buffer->selected=false;

	return buffer;
}

void obj_transform(object *obj, matrix m)
{
	int x;

	m_xformd(m,origo,obj->center);
	for (x=0;x<obj->vertexnum;x++)
	{
		m_xformd(m,obj->vertexlist[x].d,obj->vertexlist[x].generated);
		m_xform3(m,obj->vertexlist[x].defaultnormal,obj->vertexlist[x].generatednormal);
		obj->vertexlist[x].generatednormal=egyseg(obj->vertexlist[x].generatednormal);
	}
	for (x=0;x<obj->polynum;x++)
	{
		m_xform3(m,obj->polygonlist[x].defaultnormal,obj->polygonlist[x].generatednormal);
		obj->polygonlist[x].generatednormal=egyseg(obj->polygonlist[x].generatednormal);
	}
	memcpy(*obj->xformmatrix,*m,sizeof(matrix));
}

object *searchobjectbynumber(object *vlist, int num)
{
	object *v;
	v=vlist;
	while ((v!=NULL) && (v->objdata.number!=num)) v=v->next;
	return v;
}

int objnum(object *pf)
{
	object *p=pf;
	int aaa=0;
	while (p!=NULL)
	{
		aaa++;
		p=p->next;
	}
	return aaa;
}

void setlastvertextexcoord(object *obj, float y, float x)
{
	obj->vertexlist[obj->vertexnum-1].t.b.x=x;
	obj->vertexlist[obj->vertexnum-1].t.b.y=y;
	obj->vertexlist[obj->vertexnum-1].dt=obj->vertexlist[obj->vertexnum-1].t;
}

void setlastpolytexcoord(object *obj, float y1, float x1, float y2, float x2, float y3, float x3)
{
	obj->polygonlist[obj->polynum-1].t1.b.x=x1;
	obj->polygonlist[obj->polynum-1].t1.b.y=y1;
	obj->polygonlist[obj->polynum-1].d1=obj->polygonlist[obj->polynum-1].t1;
	obj->polygonlist[obj->polynum-1].t2.b.x=x2;
	obj->polygonlist[obj->polynum-1].t2.b.y=y2;
	obj->polygonlist[obj->polynum-1].d2=obj->polygonlist[obj->polynum-1].t2;
	obj->polygonlist[obj->polynum-1].t3.b.x=x3;
	obj->polygonlist[obj->polynum-1].t3.b.y=y3;
	obj->polygonlist[obj->polynum-1].d3=obj->polygonlist[obj->polynum-1].t3;
}

void generatenormals(object *obj)
{
	int x;
  for (x=0; x<obj->vertexnum; x++)
  {
	  obj->vertexlist[x].defaultnormal.a[0]=0;
	  obj->vertexlist[x].defaultnormal.a[1]=0;
	  obj->vertexlist[x].defaultnormal.a[2]=0;
  }
  for (x=0; x<obj->polynum; x++)
  {
	  vector3 facetNormal;
	  memset(&facetNormal,0,sizeof(vector3));
	  vector3 center=obj->vertexlist[obj->polygonlist[x].v1].d,
		      v0=obj->vertexlist[obj->polygonlist[x].v2].d,
			  v1=obj->vertexlist[obj->polygonlist[x].v3].d;
    v0 = vect_kivonas(v0,center);
    v1 = vect_kivonas(v1,center);

	facetNormal=egyseg(cross(v1,v0));
	if (obj->objdata.normalsinverted ^ obj->polygonlist[x].normalinverted) 
	{
		facetNormal.b.x*=-1;
		facetNormal.b.y*=-1;
		facetNormal.b.z*=-1;
	}
	obj->polygonlist[x].defaultnormal=facetNormal;
  }

  for (x=0; x<obj->polynum; x++)
  {
	obj->vertexlist[obj->polygonlist[x].v1].defaultnormal=vect_osszeadas(obj->vertexlist[obj->polygonlist[x].v1].defaultnormal,obj->polygonlist[x].defaultnormal);
	obj->vertexlist[obj->polygonlist[x].v2].defaultnormal=vect_osszeadas(obj->vertexlist[obj->polygonlist[x].v2].defaultnormal,obj->polygonlist[x].defaultnormal);
	obj->vertexlist[obj->polygonlist[x].v3].defaultnormal=vect_osszeadas(obj->vertexlist[obj->polygonlist[x].v3].defaultnormal,obj->polygonlist[x].defaultnormal);
  }
  for (x=0; x<obj->vertexnum; x++)
  {
	  obj->vertexlist[x].defaultnormal=egyseg(obj->vertexlist[x].defaultnormal);
  }
}

void obj_createprimitive(object *obj, int primitive, int param1, int param2, int param3, int param4, int param5)
{
 int xx,yy,v1v,v2v;
 double theta,phi;
 matrix m;
 vector3 up,a1,a2,dir,nx,ny,nv;
 object *v1,*v2;

 m_identity(m);
 obj->primitive=primitive;
 obj->params[1]=param1;
 obj->params[2]=param2;
 obj->params[3]=param3;
 obj->params[4]=param4;
 obj->params[5]=param5;

 switch (primitive)
 {
 case dd3d_box: {
	 strcpy(obj->name,"Box");
	 
	 addvertex(obj,-0.5,-0.5,-0.5); //0
	 setlastvertextexcoord(obj,0,0);
	 addvertex(obj, 0.5,-0.5,-0.5); //1
	 setlastvertextexcoord(obj,1,0);
	 addvertex(obj, 0.5, 0.5,-0.5); //2
	 setlastvertextexcoord(obj,1,1);
	 addvertex(obj,-0.5, 0.5,-0.5); //3
	 setlastvertextexcoord(obj,0,1);
	 addvertex(obj, 0.0, 0.0,-0.5); //4 center
	 setlastvertextexcoord(obj,0.5,0.5);

	 addvertex(obj, 0.5,-0.5, 0.5); //5
	 setlastvertextexcoord(obj,0,0);
	 addvertex(obj, 0.5, 0.5, 0.5); //6
	 setlastvertextexcoord(obj,0,1);
	 addvertex(obj, 0.5, 0.0, 0.0); //7 center
	 setlastvertextexcoord(obj,0.5,0.5);

	 addvertex(obj,-0.5, 0.5, 0.5); //8
	 setlastvertextexcoord(obj,1,0);
	 addvertex(obj,-0.5,-0.5, 0.5); //9
	 setlastvertextexcoord(obj,1,1);
	 addvertex(obj, 0.0, 0.0, 0.5); //10 center
	 setlastvertextexcoord(obj,0.5,0.5);

	 addvertex(obj,-0.5, 0.0, 0.0); //11 center
	 setlastvertextexcoord(obj,0.5,0.5);
	 addvertex(obj, 0.0,-0.5, 0.0); //12 center
	 setlastvertextexcoord(obj,0.5,0.5);
	 addvertex(obj, 0.0, 0.5, 0.0); //13 center
	 setlastvertextexcoord(obj,0.5,0.5);

	 addpolygon(obj,0,1,4,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,0.5,0.5);
	 addpolygon(obj,1,2,4,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,0,1,1,0.5,0.5);
	 addpolygon(obj,2,3,4,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,1,0,1,0.5,0.5);
	 addpolygon(obj,3,0,4,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,1,0,0,0.5,0.5);

	 addpolygon(obj,1,5,7,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,0.5,0.5);
	 addpolygon(obj,5,6,7,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,0,1,1,0.5,0.5);
	 addpolygon(obj,6,2,7,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,1,0,1,0.5,0.5);
	 addpolygon(obj,2,1,7,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,1,0,0,0.5,0.5);

	 addpolygon(obj,5,9,10,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,0.5,0.5);
	 addpolygon(obj,9,8,10,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,0,1,1,0.5,0.5);
	 addpolygon(obj,8,6,10,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,1,0,1,0.5,0.5);
	 addpolygon(obj,6,5,10,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,1,0,0,0.5,0.5);

	 addpolygon(obj,9,0,11,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,0.5,0.5);
	 addpolygon(obj,0,3,11,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,0,1,1,0.5,0.5);
	 addpolygon(obj,3,8,11,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,1,0,1,0.5,0.5);
	 addpolygon(obj,8,9,11,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,1,0,0,0.5,0.5);

	 addpolygon(obj,9,5,12,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,0.5,0.5);
	 addpolygon(obj,5,1,12,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,0,1,1,0.5,0.5);
	 addpolygon(obj,1,0,12,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,1,0,1,0.5,0.5);
	 addpolygon(obj,0,9,12,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,1,0,0,0.5,0.5);

	 addpolygon(obj,3,2,13,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,0.5,0.5);
	 addpolygon(obj,2,6,13,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,0,1,1,0.5,0.5);
	 addpolygon(obj,6,8,13,dd3d_flatshade);
	 setlastpolytexcoord(obj,1,1,0,1,0.5,0.5);
	 addpolygon(obj,8,3,13,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,1,0,0,0.5,0.5);
	 break;
				}
 case dd3d_icosaeder: {
	 strcpy(obj->name,"Icosa");
	 addvertex(obj,0,0,1.17557);
	 addvertex(obj,1.05146,0,0.525731);
	 addvertex(obj,0.32492,1,0.525731);
	 addvertex(obj,-0.850651,0.618034,0.525731);
	 addvertex(obj,-0.850651,-0.618034,0.525731);
	 addvertex(obj,0.32492,-1,0.525731);
	 addvertex(obj,0.850651,0.618034,-0.525731);
	 addvertex(obj,-0.32492,1,-0.525731);
	 addvertex(obj,-1.05146,0,-0.525731);
	 addvertex(obj,-0.32492,-1,-0.525731);
	 addvertex(obj,0.850651,-0.618034,-0.525731);
	 addvertex(obj,0,0,-1.17557);

	 addpolygon(obj, 0, 2, 1,dd3d_flatshade);
	 addpolygon(obj, 0, 3, 2,dd3d_flatshade);
	 addpolygon(obj, 0, 4, 3,dd3d_flatshade);
	 addpolygon(obj, 0, 5, 4,dd3d_flatshade);
	 addpolygon(obj, 0, 1, 5,dd3d_flatshade);
	 addpolygon(obj, 1, 2, 6,dd3d_flatshade);
	 addpolygon(obj, 2, 3, 7,dd3d_flatshade);
	 addpolygon(obj, 3, 4, 8,dd3d_flatshade);
	 addpolygon(obj, 4, 5, 9,dd3d_flatshade);
	 addpolygon(obj, 5, 1,10,dd3d_flatshade);
	 addpolygon(obj, 4, 9, 8,dd3d_flatshade);
	 addpolygon(obj, 5,10, 9,dd3d_flatshade);
	 addpolygon(obj, 1, 6,10,dd3d_flatshade);
	 addpolygon(obj, 2, 7, 6,dd3d_flatshade);
	 addpolygon(obj, 3, 8, 7,dd3d_flatshade);
	 addpolygon(obj, 6, 7,11,dd3d_flatshade);
	 addpolygon(obj, 7, 8,11,dd3d_flatshade);
	 addpolygon(obj, 8, 9,11,dd3d_flatshade);
	 addpolygon(obj, 9,10,11,dd3d_flatshade);
	 addpolygon(obj,10, 6,11,dd3d_flatshade);

	 break;
				}

 case dd3d_dodecaeder: {
	 strcpy(obj->name,"Dodeca");
	 addvertex(obj,0.3568	,0.0000	,0.9342);
	 addvertex(obj,-0.3568	,0.0000	,0.9342);
	 addvertex(obj,0.5774	,0.5774	,0.5774);
	 addvertex(obj,0.0000	,0.9342	,0.3568);
	 addvertex(obj,-0.5774	,0.5774	,0.5774);
	 addvertex(obj,-0.5774	,-0.5774,	0.5774);
	 addvertex(obj,-0.0000	,-0.9342,	0.3568);
	 addvertex(obj,0.5774	,-0.5774,	0.5774);
	 addvertex(obj,0.9342	,0.3568	,0.0000);
	 addvertex(obj,0.9342	,-0.3568,	0.0000);
	 addvertex(obj,0.0000	,-0.9342,	-0.3568);
	 addvertex(obj,0.5774	,-0.5774,	-0.5774);
	 addvertex(obj,0.5774	,0.5774	,-0.5774);
	 addvertex(obj,0.3568	,-0.0000,	-0.9342);
	 addvertex(obj,-0.5774	,-0.5774,	-0.5774);
	 addvertex(obj,-0.3568	,-0.0000,	-0.9342);
	 addvertex(obj,0.0000	,0.9342	,-0.3568);
	 addvertex(obj,-0.5774	,0.5774	,-0.5774);
	 addvertex(obj,-0.9342	,-0.3568,	-0.0000);
	 addvertex(obj,-0.9342	,0.3568	,-0.0000);

	 addpolygon(obj, 2, 0, 3,dd3d_flatshade);
	 addpolygon(obj, 3, 0, 4,dd3d_flatshade);
	 addpolygon(obj, 4, 0, 1,dd3d_flatshade);
	 addpolygon(obj, 5, 1, 6,dd3d_flatshade);
	 addpolygon(obj, 6, 1, 7,dd3d_flatshade);
	 addpolygon(obj, 7, 1, 0,dd3d_flatshade);
	 addpolygon(obj, 9, 7, 8,dd3d_flatshade);
	 addpolygon(obj, 8, 7, 2,dd3d_flatshade);
	 addpolygon(obj, 2, 7, 0,dd3d_flatshade);
	 addpolygon(obj, 6, 7,10,dd3d_flatshade);
	 addpolygon(obj,10, 7,11,dd3d_flatshade);
	 addpolygon(obj,11, 7, 9,dd3d_flatshade);
	 addpolygon(obj,13,11,12,dd3d_flatshade);
	 addpolygon(obj,12,11, 8,dd3d_flatshade);
	 addpolygon(obj, 8,11, 9,dd3d_flatshade);
	 addpolygon(obj,10,11,14,dd3d_flatshade);
	 addpolygon(obj,14,11,15,dd3d_flatshade);
	 addpolygon(obj,15,11,13,dd3d_flatshade);
	 addpolygon(obj,17,15,16,dd3d_flatshade);
	 addpolygon(obj,16,15,12,dd3d_flatshade);
	 addpolygon(obj,12,15,13,dd3d_flatshade);
	 addpolygon(obj,14,15,18,dd3d_flatshade);
	 addpolygon(obj,18,15,19,dd3d_flatshade);
	 addpolygon(obj,19,15,17,dd3d_flatshade);
	 addpolygon(obj, 4,19, 3,dd3d_flatshade);
	 addpolygon(obj, 3,19,16,dd3d_flatshade);
	 addpolygon(obj,16,19,17,dd3d_flatshade);
	 addpolygon(obj,18,19, 5,dd3d_flatshade);
	 addpolygon(obj, 5,19, 1,dd3d_flatshade);
	 addpolygon(obj, 1,19, 4,dd3d_flatshade);
	 addpolygon(obj,14,18,10,dd3d_flatshade);
	 addpolygon(obj,10,18, 6,dd3d_flatshade);
	 addpolygon(obj, 6,18, 5,dd3d_flatshade);
	 addpolygon(obj, 2, 3, 8,dd3d_flatshade);
	 addpolygon(obj, 8, 3,12,dd3d_flatshade);
	 addpolygon(obj,12, 3,16,dd3d_flatshade);

	 break;
				}


 case dd3d_sphere : {
	 
	 strcpy(obj->name,"Sphere");

	 addvertex(obj,0,-0.5,0);
	 addvertex(obj,0,0.5,0);
	 for (xx=1;xx<=(int)(param1-2);xx++)
	 {
		 theta=(xx/(param1-1.0)-0.5)*180.0;
		 for (yy=0; yy<(int)(param2); yy++)
		 {
			 phi=((float)yy/(param2))*360;
			 addvertex(obj,cos(theta*radtheta)*cos(phi*radtheta)/2,
				 sin(theta*radtheta)/2,
				 cos(theta*radtheta)*sin(phi*radtheta)/2);
			 if (xx!=0 && xx!=param1-1)
			 setlastvertextexcoord(obj,(float)xx/(float)param1,(float)yy/(float)param2);
			 else
			 {
				 if (yy==0) setlastvertextexcoord(obj,0.5,0);
			 }
		 }
	 }

	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,0,
			            (yy+1)%param2+2,
						(yy)%param2+2,
						dd3d_gouraudshade);
		 setlastpolytexcoord(obj,0,(float)(yy+0.5)/(float)(param2),
			                   1/(float)(param1-2),(float)(yy+1)/(float)(param2),
							   1/(float)(param1-2),(float)yy/(float)(param2));
	 }
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,1,
			            (param1-3)*(param2)+(yy)%param2+2,
						(param1-3)*(param2)+(yy+1)%param2+2,
						dd3d_gouraudshade);
		 setlastpolytexcoord(obj,1,(float)(yy+0.5)/(float)(param2),
			                   1-1/(float)(param1-2),(float)(yy)/(float)(param2),
							   1-1/(float)(param1-2),(float)(yy+1)/(float)(param2));
	 }
	 for (xx=0;xx<=(int)(param1-4);xx++)
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,xx*(int)(param2)+yy%param2+2,
			            xx*(int)(param2)+(yy+1)%param2+2,
						(xx+1)*(int)(param2)+(yy+1)%param2+2,
						dd3d_gouraudshade);
		 setlastpolytexcoord(obj,(float)(xx+1)/(float)(param1-2),(float)yy/(float)(param2),
			                   (float)(xx+1)/(float)(param1-2),(float)(yy+1)/(float)(param2),
							   (float)((xx+1)+1)/(float)(param1-2),(float)(yy+1)/(float)(param2));
		 addpolygon(obj,xx*(int)(param2)+yy%param2+2,
			        (xx+1)*(int)(param2)+(yy+1)%param2+2,
					(xx+1)*(int)(param2)+yy%param2+2,
					dd3d_gouraudshade);
		 setlastpolytexcoord(obj,(float)(xx+1)/(float)(param1-2),(float)yy/(float)(param2),
			                     (float)(xx+2)/(float)(param1-2),(float)(yy+1)/(float)(param2),
								 (float)(xx+2)/(float)(param1-2),(float)yy/(float)(param2));
	 }
	 break;
					}
	 
 case dd3d_grid: {
	 strcpy(obj->name,"Grid");
	 for (yy=0;yy<=(int)(param2);yy++)
	 for (xx=0;xx<=(int)(param1);xx++)
	 {
		 addvertex(obj,(float)xx/(float)(param1)-0.5,0,(float)yy/(float)(param2)-0.5);
		 setlastvertextexcoord(obj,(float)xx/(float)((int)(param1)),-(float)yy/(float)((int)(param2)));
	 }
	 for (xx=0;xx<=(int)(param1-1);xx++)
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,
			 yy*(int)(param1+1)+xx+1,yy*(int)(param1+1)+xx,
			 (yy+1)*(int)(param1+1)+xx+1,dd3d_gouraudshade);
		 addpolygon(obj,
			 (yy+1)*(int)(param1+1)+xx+1,yy*(int)(param1+1)+xx,
			 (yy+1)*(int)(param1+1)+xx,dd3d_gouraudshade);
	 }
	 break;
				 }

 case dd3d_hasab : {
	 strcpy(obj->name,"Tube");
	 for (xx=0;xx<=(int)(param2);xx++)
	 for (yy=0;yy<(int)(param1);yy++)
	 {
		 addvertex(obj,sin(((float)yy/(float)(param1))*360*radtheta)*0.5,
			                (float)xx/(float)(param2)-0.5,
			 cos(((float)yy/(float)(param1))*360*radtheta)*0.5);		 
		 setlastvertextexcoord(obj,(float)yy/(param1),(float)xx/(param2));
	 }
	 addvertex(obj,0,-0.5,0);
	 setlastvertextexcoord(obj,0.5,0.5);
	 addvertex(obj,0,0.5,0);
	 setlastvertextexcoord(obj,0.5,0.5);

	 for (xx=0;xx<=(int)(param1-1);xx++)
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,
			   yy  *(int)(param1)+(xx+1)%param1,
			   yy  *(int)(param1)+  xx  %param1,
			 (yy+1)*(int)(param1)+(xx+1)%param1, dd3d_gouraudshade);
		 setlastpolytexcoord(obj,(float)(xx+1)/(param1),(float)  yy  /(param2),
			                     (float)  xx  /(param1),(float)  yy  /(param2),
								 (float)(xx+1)/(param1),(float)(yy+1)/(param2));
		 addpolygon(obj,
			 (yy+1)*(int)(param1)+(xx+1)%param1,
			   yy  *(int)(param1)+  xx  %param1,
			 (yy+1)*(int)(param1)+  xx  %param1, dd3d_gouraudshade);
		 setlastpolytexcoord(obj,(float)(xx+1)/(param1),(float)(yy+1)/(param2),
			                     (float)  xx  /(param1),(float)  yy  /(param2),
								 (float)  xx  /(param1),(float)(yy+1)/(param2));
	 }
	 
	 if (param3)
	 {
		 for (xx=0;xx<=param1-1;xx++)
		 {
			 addpolygon(obj,obj->vertexnum-2,xx,(xx+1)%param1,dd3d_flatshade);
			 setlastpolytexcoord(obj,0.5,0.5,
				                     (float)(xx)/(param1+1),0,
				                     (float)(xx+1)/(param1+1),0);
			 addpolygon(obj,(param1)*(param2)+xx,obj->vertexnum-1,(param1)*(param2)+(xx+1)%param1,dd3d_flatshade);
			 setlastpolytexcoord(obj,(float)(xx)/(param1+1),1,
									 0.5,0.5,
				                     (float)(xx+1)/(param1+1),1);
		 }
	 }

	 break;

				   }
 case dd3d_cone  : {
	 strcpy(obj->name,"Cone");
	 addvertex(obj,0,-0.5,0);
	 setlastvertextexcoord(obj,0.5,0.5);
	 for (yy=0;yy<=(int)(param1-1);yy++)
	 {
		 addvertex(obj,sin(((float)yy/(float)(param1))*360*radtheta)*0.5,
			 0.5,
			 cos(((float)yy/(float)(param1))*360*radtheta)*0.5);
		 float xp,yp;
		 xp=(float)(sin(((float)yy/(float)(param1))*360*radtheta)*0.5+0.5);
		 yp=(float)(cos(((float)yy/(float)(param1))*360*radtheta)*0.5+0.5);
		 setlastvertextexcoord(obj,xp,yp);
	 }
	 for (xx=0;xx<=(int)(param1-1);xx++)
	 {
		 addpolygon(obj,0,(xx)%(param1)+1,(xx+1)%(param1)+1,dd3d_gouraudshade);
	 }
	 if (param2)
	 {
		 if (param1!=3)
		 {
			 addvertex(obj,0,0.5,0);
			 for (xx=0;xx<=(int)(param1-1);xx++)
			 {
				 addpolygon(obj,(xx)%(param1)+1,
					 obj->vertexnum-1,
					 (xx+1)%(param1)+1,dd3d_flatshade);
			 }
		 }
		 else
		 {
			 addpolygon(obj,2,1,3,dd3d_flatshade);
		 }
	 }
	 break;
				   }

 case dd3d_arc   : {
	 strcpy(obj->name,"Arc");
	 obj->track=true;
	 for (xx=-1;xx<=(int)(param1+1);xx++)
	 {
		 addvertex(obj,0.5*sin(((float)xx/(float)(param1))*radtheta*param2 ),0,
			 0.5*cos(((float)xx/(float)(param1))*radtheta*param2 ));
	 }

	 break;
				   }

 case dd3d_line  : {
	 strcpy(obj->name,"Line");
	 obj->track=true;
	 for (xx=-1;xx<=(int)(param1+1);xx++) 
	 {
		 addvertex(obj,0,0,((float)xx/(float)(param1))-0.5);
	 }
	 break;	
				   }

 case dd3d_clone : {
	 strcpy(obj->name,"Merge");
	 SELECTION *sel=(SELECTION*)param1;
	 object *ob=(object*)param2,*objs;
	 int b,a = selsize(sel);
	 int vn,pn;
	 SELECTION *selec2=sel;
	 SELECTION *newsel=NULL;
	 object *sobj=NULL;
	 obj->cloneparents=sel;
	 obj->objdata.textured=true;
	 for (b=1;b<=a;b++)
	 {
		 newsel=new SELECTION[1];
		 newsel->selected=obj->objdata.number;
		 newsel->next=NULL;
		 sobj=searchobjectbynumber(ob,selec2->selected);
		 if (sobj->clones==NULL)						
		 {
			 sobj->clones=newsel;
			 sobj->lastclone=newsel;
		 }
		 else
		 {
			 sobj->lastclone->next=newsel;
			 sobj->lastclone=newsel;
		 }
		 selec2=selec2->next;
	 }

	 for (b=1;b<=a;b++)
	 {		 
		 objs=searchobjectbynumber(ob,sel->selected);
		 vn=obj->vertexnum;
		 pn=obj->polynum;
		 int xa;
		 for (xa=0;xa<objs->vertexnum;xa++)
		 {
			 addvertex(obj,(double)objs->vertexlist[xa].generated.b.x,(double)objs->vertexlist[xa].generated.b.y,(double)objs->vertexlist[xa].generated.b.z);
			 setlastvertextexcoord(obj,objs->vertexlist[xa].t.b.y,objs->vertexlist[xa].t.b.x);
		 }
		 for (xa=0;xa<objs->polynum;xa++)
		 {
			 addpolygon(obj,objs->polygonlist[xa].v1+vn,objs->polygonlist[xa].v2+vn,objs->polygonlist[xa].v3+vn,objs->polygonlist[xa].shading);
			 setlastpolytexcoord(obj,objs->polygonlist[xa].t1.b.y,objs->polygonlist[xa].t1.b.x,objs->polygonlist[xa].t2.b.y,objs->polygonlist[xa].t2.b.x,objs->polygonlist[xa].t3.b.y,objs->polygonlist[xa].t3.b.x);
			 obj->polygonlist[obj->polynum-1].materialhandle=objs->polygonlist[xa].materialhandle;
			 obj->polygonlist[obj->polynum-1].envmaphandle=objs->polygonlist[xa].envmaphandle;
		 }
		 sel=sel->next;
	 }

	 break;
				   }

 case dd3d_loft  : {
	 strcpy(obj->name,"Loft");
	 up.b.x=0;
	 up.b.y=-1;
	 up.b.z=0;

	 v1=searchobjectbynumber((object*)(param3),param1);
	 v2=searchobjectbynumber((object*)(param3),param2);
	 v1v=v1->vertexnum-2;
	 v2v=v2->vertexnum-2;
	 xx=0;
	 if ((v1->vertexnum>1) && (v2->vertexnum>1))
	 {
		 int xa,ya;

		 for (xa=1;xa<v1->vertexnum-1;xa++)
		 {
			 a1=egyseg(vect_kivonas(v1->vertexlist[xa].generated,v1->vertexlist[xa-1].generated));
			 a2=egyseg(vect_kivonas(v1->vertexlist[xa+1].generated,v1->vertexlist[xa].generated));
			 dir=egyseg(vect_osszeadas(a1,a2));

			 nx=cross(up,dir);
			 ny=cross(nx,dir);

			 yy=0;
			 for (ya=1;ya<v2->vertexnum-1;ya++)
			 {
				 nv=vect_osszeadas(vect_skalarszorzas(nx,v2->vertexlist[ya].generated.b.x),
					               vect_skalarszorzas(ny,v2->vertexlist[ya].generated.b.z));
				 nv=vect_osszeadas(nv,v1->vertexlist[xa].generated);

				 addvertex(obj,(double)nv.b.x,(double)nv.b.y,(double)nv.b.z);
				 nv=vect_osszeadas(vect_skalarszorzas(nx,v2->vertexlist[ya].generatednormal.b.x),
				         	       vect_skalarszorzas(ny,v2->vertexlist[ya].generatednormal.b.z));

				 setlastvertextexcoord(obj,(float)xx/(float)(v1v-1),(float)yy/(float)(v2v-1));
				 yy++;
			 }
			 xx++;
		 }

		 for (xx=0;xx<=v2v-2;xx++)
		 for (yy=0;yy<=v1v-2;yy++)
		 {
			 addpolygon(obj,
				 yy*v2v+xx+1,yy*v2v+xx,
				 (yy+1)*v2v+xx+1,dd3d_gouraudshade);
			 addpolygon(obj,
				 (yy+1)*v2v+xx+1,yy*v2v+xx,
				 (yy+1)*v2v+xx,dd3d_gouraudshade);
		 }


	 }
	 break;
				   }

 }

 int x;
 for (x=0; x<obj->polynum; x++)
 {
	 obj->polygonlist[x].d1=obj->polygonlist[x].t1;
	 obj->polygonlist[x].d2=obj->polygonlist[x].t2;
	 obj->polygonlist[x].d3=obj->polygonlist[x].t3;
 }
 for (x=0; x<obj->vertexnum; x++)
 {
	 obj->vertexlist[x].dt=obj->vertexlist[x].t;
	 obj->vertexlist[x].base=obj->vertexlist[x].d;
 }
 generatenormals(obj);
}


scene  *newscene()
{
	scene *buffer = new scene;
	memset(buffer,0,sizeof(scene));
	buffer->next=NULL;
	buffer->objects=NULL;
	buffer->lastobject=NULL;
	buffer->floatbytes=3;
	int l;
	for (l=0;l<=7;l++)
	{
		buffer->lights[l].c_att=1;
		buffer->lights[l].identifier=GL_LIGHT0+l;
		buffer->lights[l].spot_cutoff=180;
	}
	buffer->lights[0].turnedon=true;
	buffer->lights[0].position[0]=-1;
	buffer->lights[0].position[1]=-1;
	buffer->lights[0].position[2]=-1;
	buffer->lights[0].color[0]=1;
	buffer->lights[0].color[1]=1;
	buffer->lights[0].color[2]=1;
	buffer->lights[1].turnedon=true;
	buffer->lights[1].position[0]=1;
	buffer->lights[1].position[1]=1;
	buffer->lights[1].position[2]=1;
	buffer->lights[1].color[0]=0.3f;
	buffer->lights[1].color[1]=0.3f;
	buffer->lights[1].color[2]=0.3f;

	return buffer;
}

void scene_addobject(scene *scn)
{
	object *obj = newobject();

	if (scn->lastobject==NULL)
	{
		scn->objects=obj;
		scn->lastobject=obj;
		obj->objdata.number=0;
	}
	else
	{
		obj->objdata.number=scn->lastobject->objdata.number+1;
		scn->lastobject->next=obj;
		scn->lastobject=obj;
	}
}

void scene_renderwireframe(scene &scn,byte a,byte b,int tr,int me)
{

	object *o;
	int aa[3];
	int x;
	int y=0;

	for (x=0;x<=2;x++) if (x!=a) {aa[x]=1;} else {aa[x]=0;}
	glBegin(GL_LINES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (!o->hidden)
		{
			if (!o->track)
			{
				//#ifdef addict
				if (o->selected) glColor4d(0.8,0.8,1,1); else glColor4d(0.5,0.5,0.5,1);
				//#endif
				for (x=0;x<o->polynum;x++)
				{
					if (o->polygonlist[x].generatednormal.a[b]<=0)
					{ 
						glVertex3f(o->vertexlist[o->polygonlist[x].v1].generated.b.x*aa[0],o->vertexlist[o->polygonlist[x].v1].generated.b.y*aa[1],o->vertexlist[o->polygonlist[x].v1].generated.b.z*aa[2]);
						glVertex3f(o->vertexlist[o->polygonlist[x].v2].generated.b.x*aa[0],o->vertexlist[o->polygonlist[x].v2].generated.b.y*aa[1],o->vertexlist[o->polygonlist[x].v2].generated.b.z*aa[2]);
						glVertex3f(o->vertexlist[o->polygonlist[x].v1].generated.b.x*aa[0],o->vertexlist[o->polygonlist[x].v1].generated.b.y*aa[1],o->vertexlist[o->polygonlist[x].v1].generated.b.z*aa[2]);
						glVertex3f(o->vertexlist[o->polygonlist[x].v3].generated.b.x*aa[0],o->vertexlist[o->polygonlist[x].v3].generated.b.y*aa[1],o->vertexlist[o->polygonlist[x].v3].generated.b.z*aa[2]);
						glVertex3f(o->vertexlist[o->polygonlist[x].v3].generated.b.x*aa[0],o->vertexlist[o->polygonlist[x].v3].generated.b.y*aa[1],o->vertexlist[o->polygonlist[x].v3].generated.b.z*aa[2]);
						glVertex3f(o->vertexlist[o->polygonlist[x].v2].generated.b.x*aa[0],o->vertexlist[o->polygonlist[x].v2].generated.b.y*aa[1],o->vertexlist[o->polygonlist[x].v2].generated.b.z*aa[2]);
					}
				}
			}
			//#ifdef addict
			else
			{
				if (o->selected) {glColor4d(0.8,0.8,1,1);} else {glColor4d(0.5,0.5,0.5,1);}
				if (y==tr) glColor4f(1,0.5,0.5,1);
				if (y==me) glColor4f(0.5,1,0.5,1);
				if ((y==tr) && (tr==me)) glColor4f(1,1,1,1);
				glEnd();
				glBegin(GL_LINE_STRIP);
				for (x=1;x<o->vertexnum-1;x++)
					glVertex3f(o->vertexlist[x].generated.b.x*aa[0],o->vertexlist[x].generated.b.y*aa[1],o->vertexlist[x].generated.b.z*aa[2]);
				glEnd();
				glBegin(GL_LINES);
				y++;
			}
			//#endif
		}
		o=o->next;
	}
	glEnd();
}

void obj_renderenvmap(object *obj)
{
	if (!obj->track)
	{
	glEnd();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBindTexture(GL_TEXTURE_2D,obj->polygonlist[0].envmaphandle);
	/*TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);*/

	glEnable(GL_BLEND);
	//glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(false);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S); 
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	object *o=obj;	
	color[0]=(float)(o->objdata.color[0]/255.0);
	color[1]=(float)(o->objdata.color[1]/255.0);
	color[2]=(float)(o->objdata.color[2]/255.0);
	color[3]=(float)(o->objdata.color[3]/255.0);
	glColor4fv(color);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,color);

	glBegin(GL_TRIANGLES);
	int xa;
	for (xa=0;xa<o->polynum;xa++)
	{
		if (o->polygonlist[xa].shading==dd3d_flatshade) glNormal3f(o->polygonlist[xa].generatednormal.b.x,o->polygonlist[xa].generatednormal.b.y,o->polygonlist[xa].generatednormal.b.z);

		if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.z);
		glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
		if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.z);
		glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
		if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.z);
		glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
	}
	glEnd();
	/*glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);*/
	glPopAttrib();
	glBegin(GL_TRIANGLES);
	}
}

void obj_rendersolidwire(object *o)
{
	glEnd();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	//glLineWidth(5.0);
	
	glBegin(GL_LINES);
	int xa;
	for (xa=0;xa<o->polynum;xa++)
	{
		glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
		glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);

		glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
		glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);

		glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
		glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
	}
	glEnd();
	glPopAttrib();
	glBegin(GL_TRIANGLES);
}

void scene_renderselect(scene &scn)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); //ELOSZOR A TRANSZPARENS OBJEKTEKET ZBUFFER TEST NELKUL, CSAK ZBUFFERBE IRASSAL!
	actualtexture=0;
	glColor4f(1,1,1,1);
	object *o;
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (o->selected) obj_renderenvmap(o);
		o=o->next;
	}
	glEnd();
	glPopAttrib();/**/
}

void scene_rendersolid(scene &scn, bool renderhidden)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); //ELOSZOR A TRANSZPARENS OBJEKTEKET ZBUFFER TEST NELKUL, CSAK ZBUFFERBE IRASSAL!
	actualtexture=0;
	glColor4f(1,1,1,1);
	/*glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);*/
	
	int l;
	for (l=0;l<=7;l++)
		if (scn.lights[l].turnedon)
		{
			glEnable(scn.lights[l].identifier);
			glLightfv(scn.lights[l].identifier,GL_AMBIENT,scn.lights[l].ambient);
			glLightfv(scn.lights[l].identifier,GL_DIFFUSE,scn.lights[l].color);
			glLightfv(scn.lights[l].identifier,GL_SPECULAR,scn.lights[l].color);
			glLightfv(scn.lights[l].identifier,GL_POSITION,scn.lights[l].position);
			glLightfv(scn.lights[l].identifier,GL_SPOT_DIRECTION,scn.lights[l].spot_direction);
			glLightfv(scn.lights[l].identifier,GL_SPOT_EXPONENT,&scn.lights[l].spot_exponent);
			glLightfv(scn.lights[l].identifier,GL_SPOT_CUTOFF,&scn.lights[l].spot_cutoff);
			glLightfv(scn.lights[l].identifier,GL_CONSTANT_ATTENUATION,&scn.lights[l].c_att);
			glLightfv(scn.lights[l].identifier,GL_LINEAR_ATTENUATION,&scn.lights[l].l_att);
			glLightfv(scn.lights[l].identifier,GL_QUADRATIC_ATTENUATION,&scn.lights[l].q_att);
		}
		else
		{
			glDisable(scn.lights[l].identifier);
		}
	
	object *o;
	glDisable(GL_BLEND);
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (o->objdata.color[3]==255)
		{
			if (!o->hidden)
			{
				color[0]=(float)(o->objdata.color[0]/255.0);
				color[1]=(float)(o->objdata.color[1]/255.0);
				color[2]=(float)(o->objdata.color[2]/255.0);
				color[3]=(float)(o->objdata.color[3]/255.0);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
				int xa;
				for (xa=0;xa<o->polynum;xa++)
				{
					if (o->objdata.textured)
					{
						if (o->polygonlist[xa].materialhandle!=actualtexture)
						{
							glEnd();
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,o->polygonlist[xa].materialhandle);
							actualtexture=o->polygonlist[xa].materialhandle;
							glBegin(GL_TRIANGLES);
						}
					}
					if ((!o->objdata.textured) || (o->polygonlist[xa].materialhandle==0))
					{
						glEnd();
						glDisable(GL_TEXTURE_2D);
						glBegin(GL_TRIANGLES);
						actualtexture=0;
					}
					if (o->polygonlist[xa].shading==dd3d_flatshade) glNormal3f(o->polygonlist[xa].generatednormal.b.x,o->polygonlist[xa].generatednormal.b.y,o->polygonlist[xa].generatednormal.b.z);

					if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.z);
					if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t1.b.x,o->polygonlist[xa].t1.b.y);
					glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
					if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.z);
					if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t2.b.x,o->polygonlist[xa].t2.b.y);
					glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
					if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.z);
					if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t3.b.x,o->polygonlist[xa].t3.b.y);
					glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
				}
				if (o->objdata.envmapped) obj_renderenvmap(o);
			}
			else if (renderhidden)
			{
				obj_rendersolidwire(o);
			}
		}
		o=o->next;
	}
	glEnd();
	glPopAttrib();/**/
}

void scene_rendernormals(scene &scn)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); 
	object *o;
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (!o->hidden)
		{
			int xa;	
			glColor4f(1,0,0,1);
			for (xa=0;xa<o->vertexnum;xa++)
			{
				glVertex3f(o->vertexlist[xa].generated.b.x,o->vertexlist[xa].generated.b.y,o->vertexlist[xa].generated.b.z);
				glVertex3f(o->vertexlist[xa].generated.b.x+o->vertexlist[xa].generatednormal.b.x*0.1f,
					       o->vertexlist[xa].generated.b.y+o->vertexlist[xa].generatednormal.b.y*0.1f,
						   o->vertexlist[xa].generated.b.z+o->vertexlist[xa].generatednormal.b.z*0.1f);
			}

			glColor4f(0,1,0,1);
			for (xa=0;xa<o->polynum;xa++)
			{
				vector3 middle;
				middle=vect_skalarszorzas(vect_osszeadas(vect_osszeadas(o->vertexlist[o->polygonlist[xa].v1].generated,o->vertexlist[o->polygonlist[xa].v2].generated),o->vertexlist[o->polygonlist[xa].v3].generated),1.0f/3.0f);
				glVertex3f(middle.b.x,middle.b.y,middle.b.z);
				glVertex3f(middle.b.x+o->polygonlist[xa].generatednormal.b.x*0.1f,
					       middle.b.y+o->polygonlist[xa].generatednormal.b.y*0.1f,
						   middle.b.z+o->polygonlist[xa].generatednormal.b.z*0.1f);
			}
		}
		o=o->next;
	}
	glEnd();
	glPopAttrib();
}

void scene_rendertransparent(scene &scn, bool renderhidden)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); //ELOSZOR A TRANSZPARENS OBJEKTEKET ZBUFFER TEST NELKUL, CSAK ZBUFFERBE IRASSAL!
	glColor4f(1,1,1,1);
	actualtexture=0;
	
	int l;
	for (l=0;l<=7;l++)
		if (scn.lights[l].turnedon)
		{
			glEnable(scn.lights[l].identifier);
			glLightfv(scn.lights[l].identifier,GL_AMBIENT,scn.lights[l].ambient);
			glLightfv(scn.lights[l].identifier,GL_DIFFUSE,scn.lights[l].color);
			glLightfv(scn.lights[l].identifier,GL_SPECULAR,scn.lights[l].color);
			glLightfv(scn.lights[l].identifier,GL_POSITION,scn.lights[l].position);
			glLightfv(scn.lights[l].identifier,GL_SPOT_DIRECTION,scn.lights[l].spot_direction);
			glLightfv(scn.lights[l].identifier,GL_SPOT_EXPONENT,&scn.lights[l].spot_exponent);
			glLightfv(scn.lights[l].identifier,GL_SPOT_CUTOFF,&scn.lights[l].spot_cutoff);
			glLightfv(scn.lights[l].identifier,GL_CONSTANT_ATTENUATION,&scn.lights[l].c_att);
			glLightfv(scn.lights[l].identifier,GL_LINEAR_ATTENUATION,&scn.lights[l].l_att);
			glLightfv(scn.lights[l].identifier,GL_QUADRATIC_ATTENUATION,&scn.lights[l].q_att);
		}
		else
		{
			glDisable(scn.lights[l].identifier);
		}
	
	object *o;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (o->objdata.color[3]<255)
		{
			if (!o->hidden)
			{
				color[0]=(float)(o->objdata.color[0]/255.0);
				color[1]=(float)(o->objdata.color[1]/255.0);
				color[2]=(float)(o->objdata.color[2]/255.0);
				color[3]=(float)(o->objdata.color[3]/255.0);
							glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
							int xa;
							for (xa=0;xa<o->polynum;xa++)
							{
								if (o->objdata.textured)
								{
									if (o->polygonlist[xa].materialhandle!=actualtexture)
									{
										glEnd();
										glEnable(GL_TEXTURE_2D);
										glBindTexture(GL_TEXTURE_2D,o->polygonlist[xa].materialhandle);
										actualtexture=o->polygonlist[xa].materialhandle;
										glBegin(GL_TRIANGLES);
									}
								}
								if ((!o->objdata.textured) || (o->polygonlist[xa].materialhandle==0))
								{
									glEnd();
									glDisable(GL_TEXTURE_2D);
									glBegin(GL_TRIANGLES);
								}
								if (o->polygonlist[xa].shading==dd3d_flatshade) glNormal3f(o->polygonlist[xa].generatednormal.b.x,o->polygonlist[xa].generatednormal.b.y,o->polygonlist[xa].generatednormal.b.z);

								if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.z);
								if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t1.b.x,o->polygonlist[xa].t1.b.y);
								glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
								if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.z);
								if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t2.b.x,o->polygonlist[xa].t2.b.y);
								glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
								if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.z);
								if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t3.b.x,o->polygonlist[xa].t3.b.y);
								glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
							}
							if (o->objdata.envmapped) obj_renderenvmap(o);
			}
			else if (renderhidden)
			{
				obj_rendersolidwire(o);
			}
		}
		o=o->next;
	}
	glEnd();
	glPopAttrib();/**/
}

int getedgevertex(object *o, int badvertex, int edgeid)
{
	int b;
	b=-1;
	if (o->edgelist[edgeid].v1==badvertex) b=o->edgelist[edgeid].v2; else b=o->edgelist[edgeid].v1;
	return b;
}

vector3 getbadvector(object *o, int vxid, int edgeid, float w, bool hh)
{
	int e0=getedgevertex(o,vxid,edgeid);

	vector3 buffer;
	memset(&buffer,0,sizeof(vector3));
	int edgevertexposition=-1; //position of the edge in the vertex's edge array
	int x;
	for (x=0;x<o->vertexlist[vxid].vedgenum;x++) if (o->vertexlist[vxid].vedgelist[x]==edgeid) edgevertexposition=x;
	buffer=vect_skalarszorzas(o->vertexlist[vxid].d,3.0f/4.0f);

	switch (o->vertexlist[vxid].vedgenum)
	{
	case 2:	{
		return vect_osszeadas(vect_skalarszorzas(o->vertexlist[vxid].d,0.5),
			                  vect_skalarszorzas(o->vertexlist[e0].d,0.5));
			}
	case 3: {
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,5.0f/12.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,-1.0f/12.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,-1.0f/12.0f));
		return buffer;
		break;
			}
	case 4: {
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,3.0f/8.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,-1.0f/8.0f));
		return buffer;
		break;
			}
	case 6: {
		//buffer=vect_skalarszorzas(o->vertexlist[vxid].d,1.0f/2.0f);
		memset(&buffer,0,sizeof(vector3));
		buffer=vect_skalarszorzas(o->vertexlist[e0].d,1.0f/2.0f);

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		if (hh)	buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,1.0f/8.0f+2*w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,-1.0f/16.0f-w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,-1.0f/16.0f-w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		if (hh)	buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,1.0f/8.0f+2*w));

		return buffer;
		break;
			}
	default: {
		for (x=0;x<o->vertexlist[vxid].vedgenum;x++)
		{
			int n=o->vertexlist[vxid].vedgenum;
			float val=(float)((0.25f+cos(2*pi*(float)x/(float)n)+0.5*cos(4*pi*(float)x/(float)n))/(float)n);
			buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].d,val));
			edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
			edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
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
	a1=o->vertexlist[e_.v1];
	a2=o->vertexlist[e_.v2];

	if (a1.vedgenum==6 && a2.vedgenum==6)
	{
		a=getbadvector(o,e_.v1,e,w,false);
		b=getbadvector(o,e_.v2,e,w,true);
		return vect_osszeadas(a,b);
	}
	else
	{
		if (a1.vedgenum!=6) a=getbadvector(o,e_.v1,e,w,false);
		if (a2.vedgenum!=6) b=getbadvector(o,e_.v2,e,w,false);
		if (a1.vedgenum==6) return b;
		if (a2.vedgenum==6) return a;
		if (a1.vedgenum!=6 && a2.vedgenum!=6) return vect_osszeadas(vect_skalarszorzas(a,0.5f),vect_skalarszorzas(b,0.5f));
	}

	return buffer;
}

void tessellateEdges(object *obj, int* edgeVertMap,int* edgeEdgeMap, float w, bool linear)
{
	int xi;
	
	for (xi=obj->edgenum-1;xi>=0;xi--)
	{
		vector3 newvertex,newnormal,v1,v2,n1,n2;
		v1=obj->vertexlist[obj->edgelist[xi].v1].d;
		v2=obj->vertexlist[obj->edgelist[xi].v2].d;
		n1=obj->vertexlist[obj->edgelist[xi].v1].defaultnormal;
		n2=obj->vertexlist[obj->edgelist[xi].v2].defaultnormal;

		if (linear)
			newvertex=vect_osszeadas(vect_skalarszorzas(v1,0.5f),vect_skalarszorzas(v2,0.5f));
		else
			newvertex=butterflytessellation(obj,xi,w);
		newnormal.b.x=(n1.b.x+n2.b.x)/2.0f;
		newnormal.b.y=(n1.b.y+n2.b.y)/2.0f;
		newnormal.b.z=(n1.b.z+n2.b.z)/2.0f;
		
		addvertex(obj,newvertex.b.x,newvertex.b.y,newvertex.b.z);
		setlastvertextexcoord(obj,(obj->vertexlist[obj->edgelist[xi].v1].t.b.y+
			                      obj->vertexlist[obj->edgelist[xi].v2].t.b.y)/2,
								  (obj->vertexlist[obj->edgelist[xi].v1].t.b.x+
			                      obj->vertexlist[obj->edgelist[xi].v2].t.b.x)/2);

		edgeVertMap[xi]=obj->vertexnum-1;
		edgeEdgeMap[xi]=obj->edgenum-1;
	}
}

void buildNewFaces(object *obj, int* edgeVertMap,int* edgeEdgeMap, bool linear)
{
	int xi;
	
	for (xi=obj->polynum-1;xi>=0;xi--)
	{
		int a,b,c,d,e,f;
		vector2 ta,tb,tc,td,te,tf;
		a=obj->polygonlist[xi].v1;
		b=obj->polygonlist[xi].v2;
		c=obj->polygonlist[xi].v3;
		d=edgeVertMap[obj->polygonlist[xi].pedgelist[0]];
		e=edgeVertMap[obj->polygonlist[xi].pedgelist[1]];
		f=edgeVertMap[obj->polygonlist[xi].pedgelist[2]];
		ta=obj->polygonlist[xi].t1;
		tb=obj->polygonlist[xi].t2;
		tc=obj->polygonlist[xi].t3;
		td.b.x=(ta.b.x+tb.b.x)/2.0f;
		td.b.y=(ta.b.y+tb.b.y)/2.0f;
		te.b.x=(ta.b.x+tc.b.x)/2.0f;
		te.b.y=(ta.b.y+tc.b.y)/2.0f;
		tf.b.x=(tb.b.x+tc.b.x)/2.0f;
		tf.b.y=(tb.b.y+tc.b.y)/2.0f;
		obj->polygonlist[xi].v2=d;
		obj->polygonlist[xi].t2=td;
		obj->polygonlist[xi].d2=obj->polygonlist[xi].t2;
		obj->polygonlist[xi].v3=e;
		obj->polygonlist[xi].t3=te;
		obj->polygonlist[xi].d3=obj->polygonlist[xi].t3;
		if (!linear) obj->polygonlist[xi].shading=dd3d_gouraudshade;

		addpolygon(obj,d,b,f,obj->polygonlist[xi].shading);
		obj->polygonlist[obj->polynum-1].materialhandle=obj->polygonlist[xi].materialhandle;
		if (!linear) obj->polygonlist[obj->polynum-1].shading=dd3d_gouraudshade;
		setlastpolytexcoord(obj,td.b.y,td.b.x,tb.b.y,tb.b.x,tf.b.y,tf.b.x);
		addpolygon(obj,e,f,c,obj->polygonlist[xi].shading);
		obj->polygonlist[obj->polynum-1].materialhandle=obj->polygonlist[xi].materialhandle;
		if (!linear) obj->polygonlist[obj->polynum-1].shading=dd3d_gouraudshade;
		setlastpolytexcoord(obj,te.b.y,te.b.x,tf.b.y,tf.b.x,tc.b.y,tc.b.x);
		addpolygon(obj,e,d,f,obj->polygonlist[xi].shading);
		obj->polygonlist[obj->polynum-1].materialhandle=obj->polygonlist[xi].materialhandle;
		if (!linear) obj->polygonlist[obj->polynum-1].shading=dd3d_gouraudshade;
		setlastpolytexcoord(obj,te.b.y,te.b.x,td.b.y,td.b.x,tf.b.y,tf.b.x);
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

void sortvertexedges(object *obj)
{
	int x,z;
	int y;
	for (x=0; x<obj->vertexnum; x++)
	{
		vertex buffer=obj->vertexlist[x];
		if (buffer.vedgenum>2)
		{
		vector3 vertvector;
		vector3 normal = obj->vertexlist[x].defaultnormal;
		vector3 vx = obj->vertexlist[x].d;
		float *szogek = new float[buffer.vedgenum];
		memset(szogek,0,sizeof(float)*buffer.vedgenum);

		int e=getedgevertex(obj,x,buffer.vedgelist[0]);
		vector3 refvector=obj->vertexlist[e].d;
		refvector = vect_kivonas(refvector,vx);

		vector3 projvec=egyseg(normal);
		projvec=vect_skalarszorzas(projvec,dotproduct(refvector,normal));
		refvector = egyseg(vect_kivonas(refvector,projvec));

		for (y=1;y<buffer.vedgenum;y++)
		{
			e=getedgevertex(obj,x,buffer.vedgelist[y]);
			vertvector = vect_kivonas(obj->vertexlist[e].d,vx);
			projvec = normal;
			projvec=vect_skalarszorzas(projvec,dotproduct(vertvector,normal));
			vertvector = egyseg(vect_kivonas(vertvector,projvec));

			float cosTheta = dotproduct(vertvector,refvector);
			if (cosTheta < -1.0f) cosTheta = -1.0f;
			if (cosTheta > 1.0f) cosTheta = 1.0f;
			float angle = (float)acos(cosTheta);
			angle *= (float)(180.0f / pi);

			vector3 crossProd=cross(vertvector,refvector);
			if (dotproduct(crossProd,normal) < 0)
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
			b1=obj->vertexlist[x].vedgelist[smallest];
			obj->vertexlist[x].vedgelist[smallest]=obj->vertexlist[x].vedgelist[z];
			obj->vertexlist[x].vedgelist[z]=b1;
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
			obj->vertexlist[xa].vedgenum=0;
		}
		for (xa=0; xa<obj->polynum; xa++)
		{
			addedge(obj,obj->polygonlist[xa].v1,obj->polygonlist[xa].v2,true);
			obj->polygonlist[xa].pedgelist[0]=searchedge(obj,obj->polygonlist[xa].v1,obj->polygonlist[xa].v2);
			addedge(obj,obj->polygonlist[xa].v1,obj->polygonlist[xa].v3,true);
			obj->polygonlist[xa].pedgelist[1]=searchedge(obj,obj->polygonlist[xa].v1,obj->polygonlist[xa].v3);
			addedge(obj,obj->polygonlist[xa].v2,obj->polygonlist[xa].v3,true);
			obj->polygonlist[xa].pedgelist[2]=searchedge(obj,obj->polygonlist[xa].v2,obj->polygonlist[xa].v3);
		}
		sortvertexedges(obj);
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

	generatenormals(obj);
	obj_transform(obj,obj->xformmatrix);
}

void meshblur(object *obj)
{
	generateedges(obj);
	vertex *newvlist = new vertex [obj->vertexnum];
	memcpy(newvlist,obj->vertexlist,sizeof(vertex)*obj->vertexnum);
	int x;
	for (x=0;x<obj->vertexnum;x++)
	{
		vector3 buffer;
		float dw=2;
		buffer=vect_skalarszorzas(obj->vertexlist[x].d,2);
		for (int y=0;y<newvlist[x].vedgenum;y++)
		{
			float w;
			int vx=getedgevertex(obj, x, newvlist[x].vedgelist[y]);
			/*vector3 v=vect_kivonas(obj->vertexlist[x].d,obj->vertexlist[vx].d);
			w=sqrt(v.b.x*v.b.x+v.b.y*v.b.y+v.b.z*v.b.z);
			w*=w;
			w=1-w;
			if (w<0) w=0;*/
			w=1;
			dw+=w;
			buffer=vect_skalarszorzas(vect_osszeadas(buffer,obj->vertexlist[vx].d),w);
		}

		buffer=vect_skalarszorzas(buffer,1.0f/dw);
		newvlist[x].d=buffer;
		newvlist[x].base=buffer;
	}
	for (x=0; x<obj->polynum; x++)
	{
		obj->polygonlist[x].shading=dd3d_gouraudshade;
	}
	memcpy(obj->vertexlist,newvlist,sizeof(vertex)*obj->vertexnum);
	generatenormals(obj);
	obj_transform(obj,obj->xformmatrix);
}

void obj_counttexturecoordinates(object *o, int mapping, int xr, int yr, int xoff, int yoff, bool swap, bool xswp, bool yswp)
{
	float xmin,xmax,ymin,ymax,zmin,zmax;
	xmin=xmax=ymin=ymax=zmin=zmax=0.0f;
	int x;
	for (x=0; x<o->vertexnum; x++)
	{
		xmin=min(xmin,o->vertexlist[x].d.b.x);
		xmax=max(xmax,o->vertexlist[x].d.b.x);
		ymin=min(ymin,o->vertexlist[x].d.b.y);
		ymax=max(ymax,o->vertexlist[x].d.b.y);
		zmin=min(zmin,o->vertexlist[x].d.b.z);
		zmax=max(zmax,o->vertexlist[x].d.b.z);
	}
	for (x=0; x<o->polynum;x++)
	{
		o->polygonlist[x].t1=o->polygonlist[x].d1;
		o->polygonlist[x].t2=o->polygonlist[x].d2;
		o->polygonlist[x].t3=o->polygonlist[x].d3;
	}
	for (x=0; x<o->vertexnum;x++)
	{
		o->vertexlist[x].t=o->vertexlist[x].dt;
	}
	switch (mapping)
	{
	case dd3d_defaultmap:
		{
			for (x=0; x<o->vertexnum;x++)
			{
				o->vertexlist[x].t.b.x=o->vertexlist[x].t.b.x*xr+(float)xoff/255.0f;
				o->vertexlist[x].t.b.y=o->vertexlist[x].t.b.y*yr+(float)yoff/255.0f;
			}
			for (x=0; x<o->polynum; x++)
			{
				o->polygonlist[x].t1.b.x=o->polygonlist[x].t1.b.x*xr+(float)xoff/255.0f;
				o->polygonlist[x].t1.b.y=o->polygonlist[x].t1.b.y*yr+(float)yoff/255.0f;
				o->polygonlist[x].t2.b.x=o->polygonlist[x].t2.b.x*xr+(float)xoff/255.0f;
				o->polygonlist[x].t2.b.y=o->polygonlist[x].t2.b.y*yr+(float)yoff/255.0f;
				o->polygonlist[x].t3.b.x=o->polygonlist[x].t3.b.x*xr+(float)xoff/255.0f;
				o->polygonlist[x].t3.b.y=o->polygonlist[x].t3.b.y*yr+(float)yoff/255.0f;
			}
			break;
		}
	}
	for (x=0; x<o->vertexnum;x++)
	{
		if (xswp) o->vertexlist[x].t.b.x=1-o->vertexlist[x].t.b.x;
		if (yswp) o->vertexlist[x].t.b.y=1-o->vertexlist[x].t.b.y;
	}
	for (x=0; x<o->polynum;x++)
	{
		if (xswp) o->polygonlist[x].t1.b.x=1-o->polygonlist[x].t1.b.x;
		if (yswp) o->polygonlist[x].t1.b.y=1-o->polygonlist[x].t1.b.y;
		if (xswp) o->polygonlist[x].t2.b.x=1-o->polygonlist[x].t2.b.x;
		if (yswp) o->polygonlist[x].t2.b.y=1-o->polygonlist[x].t2.b.y;
		if (xswp) o->polygonlist[x].t3.b.x=1-o->polygonlist[x].t3.b.x;
		if (yswp) o->polygonlist[x].t3.b.y=1-o->polygonlist[x].t3.b.y;
	}
	if (swap)
	{
		for (x=0; x<o->vertexnum;x++)
		{
			float b=o->vertexlist[x].t.b.x;
			o->vertexlist[x].t.b.x=o->vertexlist[x].t.b.y;
			o->vertexlist[x].t.b.y=b;
		}
		for (x=0; x<o->polynum;x++)
		{
			float b;
			b=o->polygonlist[x].t1.b.x;
			o->polygonlist[x].t1.b.x=o->polygonlist[x].t1.b.y;
			o->polygonlist[x].t1.b.y=b;
			b=o->polygonlist[x].t2.b.x;
			o->polygonlist[x].t2.b.x=o->polygonlist[x].t2.b.y;
			o->polygonlist[x].t2.b.y=b;
			b=o->polygonlist[x].t3.b.x;
			o->polygonlist[x].t3.b.x=o->polygonlist[x].t3.b.y;
			o->polygonlist[x].t3.b.y=b;
		}
	}
}

#define CROSS(dest,v1,v2)                      \
              dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
              dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
              dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; dest[1]=v1[1]-v2[1]; dest[2]=v1[2]-v2[2]; 

#define SET(dest,src) dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; 

#define ADD(dest,v1,v2) dest[0]=v1[0]+v2[0]; dest[1]=v1[1]+v2[1]; dest[2]=v1[2]+v2[2]; 

#define MULT(dest,v,factor) dest[0]=factor*v[0]; dest[1]=factor*v[1]; dest[2]=factor*v[2];

void isect2(float VTX0[3],float VTX1[3],float VTX2[3],float VV0,float VV1,float VV2,
	    float D0,float D1,float D2,float *isect0,float *isect1,float isectpoint0[3],float isectpoint1[3]) 
{
  float tmp=D0/(D0-D1);          
  float diff[3];
  *isect0=VV0+(VV1-VV0)*tmp;         
  SUB(diff,VTX1,VTX0);              
  MULT(diff,diff,tmp);               
  ADD(isectpoint0,diff,VTX0);        
  tmp=D0/(D0-D2);                    
  *isect1=VV0+(VV2-VV0)*tmp;          
  SUB(diff,VTX2,VTX0);                   
  MULT(diff,diff,tmp);                 
  ADD(isectpoint1,VTX0,diff);          
}

int compute_intervals_isectline(float VERT0[3],float VERT1[3],float VERT2[3],
				       float VV0,float VV1,float VV2,float D0,float D1,float D2,
				       float D0D1,float D0D2,float *isect0,float *isect1,
				       float isectpoint0[3],float isectpoint1[3])
{
  if(D0D1>0.0f)                                        
  {                                                    
    /* here we know that D0D2<=0.0 */                  
    /* that is D0, D1 are on the same side, D2 on the other or on the plane */
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);
  } 
  else if(D0D2>0.0f)                                   
    {                                                   
    /* here we know that d0d1<=0.0 */             
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1);
  }                                                  
  else if(D1*D2>0.0f || D0!=0.0f)   
  {                                   
    /* here we know that d0d1<=0.0 or that D0!=0.0 */
    isect2(VERT0,VERT1,VERT2,VV0,VV1,VV2,D0,D1,D2,isect0,isect1,isectpoint0,isectpoint1);   
  }                                                  
  else if(D1!=0.0f)                                  
  {                                               
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1); 
  }                                         
  else if(D2!=0.0f)                                  
  {                                                   
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);     
  }                                                 
  else                                               
  {                                                   
    /* triangles are coplanar */    
    return 1;
  }
  return 0;
}

#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return 1;                      \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return 1;                      \
    }                                                 \
  }                                

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  /* test edge U0,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}

#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  float a,b,c,d0,d1,d2;                     \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b=-(U1[i0]-U0[i0]);                       \
  c=-a*U0[i0]-b*U0[i1];                     \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b=-(U2[i0]-U1[i0]);                       \
  c=-a*U1[i0]-b*U1[i1];                     \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b=-(U0[i0]-U2[i0]);                       \
  c=-a*U2[i0]-b*U2[i1];                     \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return 1;                 \
  }                                         \
}

int coplanar_tri_tri(float N[3],float V0[3],float V1[3],float V2[3],
                     float U0[3],float U1[3],float U2[3])
{
   float A[3];
   short i0,i1;
   /* first project onto an axis-aligned plane, that maximizes the area */
   /* of the triangles, compute indices: i0,i1. */
   A[0]=(float)(fabs(N[0]));
   A[1]=(float)(fabs(N[1]));
   A[2]=(float)(fabs(N[2]));
   if(A[0]>A[1])
   {
      if(A[0]>A[2])  
      {
          i0=1;      /* A[0] is greatest */
          i1=2;
      }
      else
      {
          i0=0;      /* A[2] is greatest */
          i1=1;
      }
   }
   else   /* A[0]<=A[1] */
   {
      if(A[2]>A[1])
      {
          i0=0;      /* A[2] is greatest */
          i1=1;                                           
      }
      else
      {
          i0=0;      /* A[1] is greatest */
          i1=2;
      }
    }               
                
    /* test all edges of triangle 1 against the edges of triangle 2 */
    EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2);
    EDGE_AGAINST_TRI_EDGES(V1,V2,U0,U1,U2);
    EDGE_AGAINST_TRI_EDGES(V2,V0,U0,U1,U2);
                
    /* finally, test if tri1 is totally contained in tri2 or vice versa */
    POINT_IN_TRI(V0,U0,U1,U2);
    POINT_IN_TRI(U0,V0,V1,V2);

    return 0;
}

#define SORT2(a,b,smallest)       \
             if(a>b)       \
             {             \
               float c;    \
               c=a;        \
               a=b;        \
               b=c;        \
               smallest=1; \
             }             \
             else smallest=0;

int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3],
				                     float U0[3],float U1[3],float U2[3],int *coplanar,
				                     float isectpt1[3],float isectpt2[3])
{
  float E1[3],E2[3];
  float N1[3],N2[3],d1,d2;
  float du0,du1,du2,dv0,dv1,dv2;
  float D[3];
  float isect1[2], isect2[2];
  float isectpointA1[3],isectpointA2[3];
  float isectpointB1[3],isectpointB2[3];
  float du0du1,du0du2,dv0dv1,dv0dv2;
  short index;
  float vp0,vp1,vp2;
  float up0,up1,up2;
  float b,c,max;
  int smallest1,smallest2;
  
  /* compute plane equation of triangle(V0,V1,V2) */
  SUB(E1,V1,V0);
  SUB(E2,V2,V0);
  CROSS(N1,E1,E2);
  d1=-DOT(N1,V0);
  /* plane equation 1: N1.X+d1=0 */

  /* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane*/
  du0=DOT(N1,U0)+d1;
  du1=DOT(N1,U1)+d1;
  du2=DOT(N1,U2)+d1;

  /* coplanarity robustness check */
#if USE_EPSILON_TEST==TRUE
  if(fabs(du0)<EPSILON) du0=0.0;
  if(fabs(du1)<EPSILON) du1=0.0;
  if(fabs(du2)<EPSILON) du2=0.0;
#endif
  du0du1=du0*du1;
  du0du2=du0*du2;

  if(du0du1>0.0f && du0du2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute plane of triangle (U0,U1,U2) */
  SUB(E1,U1,U0);
  SUB(E2,U2,U0);
  CROSS(N2,E1,E2);
  d2=-DOT(N2,U0);
  /* plane equation 2: N2.X+d2=0 */

  /* put V0,V1,V2 into plane equation 2 */
  dv0=DOT(N2,V0)+d2;
  dv1=DOT(N2,V1)+d2;
  dv2=DOT(N2,V2)+d2;

#if USE_EPSILON_TEST==TRUE
  if(fabs(dv0)<EPSILON) dv0=0.0;
  if(fabs(dv1)<EPSILON) dv1=0.0;
  if(fabs(dv2)<EPSILON) dv2=0.0;
#endif

  dv0dv1=dv0*dv1;
  dv0dv2=dv0*dv2;
        
  if(dv0dv1>0.0f && dv0dv2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute direction of intersection line */
  CROSS(D,N1,N2);

  /* compute and index to the largest component of D */
  max=(float)(fabs(D[0]));
  index=0;
  b=(float)(fabs(D[1]));
  c=(float)(fabs(D[2]));
  if(b>max) max=b,index=1;
  if(c>max) max=c,index=2;

  /* this is the simplified projection onto L*/
  vp0=V0[index];
  vp1=V1[index];
  vp2=V2[index];
  
  up0=U0[index];
  up1=U1[index];
  up2=U2[index];

  /* compute interval for triangle 1 */
  *coplanar=compute_intervals_isectline(V0,V1,V2,vp0,vp1,vp2,dv0,dv1,dv2,
				       dv0dv1,dv0dv2,&isect1[0],&isect1[1],isectpointA1,isectpointA2);
  if(*coplanar) return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);     


  /* compute interval for triangle 2 */
  compute_intervals_isectline(U0,U1,U2,up0,up1,up2,du0,du1,du2,
			      du0du1,du0du2,&isect2[0],&isect2[1],isectpointB1,isectpointB2);

  SORT2(isect1[0],isect1[1],smallest1);
  SORT2(isect2[0],isect2[1],smallest2);

  if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;

  /* at this point, we know that the triangles intersect */

  if(isect2[0]<isect1[0])
  {
    if(smallest1==0) { SET(isectpt1,isectpointA1); }
    else { SET(isectpt1,isectpointA2); }

    if(isect2[1]<isect1[1])
    {
      if(smallest2==0) { SET(isectpt2,isectpointB2); }
      else { SET(isectpt2,isectpointB1); }
    }
    else
    {
      if(smallest1==0) { SET(isectpt2,isectpointA2); }
      else { SET(isectpt2,isectpointA1); }
    }
  }
  else
  {
    if(smallest2==0) { SET(isectpt1,isectpointB1); }
    else { SET(isectpt1,isectpointB2); }

    if(isect2[1]>isect1[1])
    {
      if(smallest1==0) { SET(isectpt2,isectpointA2); }
      else { SET(isectpt2,isectpointA1); }      
    }
    else
    {
      if(smallest2==0) { SET(isectpt2,isectpointB2); }
      else { SET(isectpt2,isectpointB1); } 
    }
  }
  return 1;
}


struct pathedge{
	vector3 a,b;
	bool inpath;
	bool circleend;
	bool circlestart;
	bool pathend;
	bool pathstart;
	int sidepathentry;
	int sidepathentryend;
	bool beenhere;
	float mu;
};

struct pathedgelist{ 
	int pathedgeroom;
	int pathedgenum;
	int pathnum;
	pathedge *list;
};

void addpathedge(pathedgelist &list, pathedge elem)
{
	if (list.pathedgeroom==list.pathedgenum)
	{
		pathedge *buffer = new pathedge[(list.pathedgeroom+10)*2];
		memset(buffer,0,sizeof(pathedge)*(list.pathedgeroom+10)*2);
		memcpy(buffer,list.list,sizeof(pathedge)*list.pathedgeroom);
		pathedge *b=list.list;
		list.list=buffer;
		delete[] b;
		list.pathedgeroom=(list.pathedgeroom+10)*2;
	}
	list.list[list.pathedgenum]=elem;
	list.pathedgenum++;	
}

void swapedges(pathedge *list, int a, int b)
{
	pathedge buf;
	buf=list[a];
	list[a]=list[b];
	list[b]=buf;
}

int searchedgestartvertex(pathedgelist list, vector3 a, int match)
{
	int result=-1;
	float eps=0.00001f;

	for (int x=0; x<list.pathedgenum; x++)
	if (x!=match && fabs(list.list[x].a.b.x-a.b.x)<=eps && 
		              fabs(list.list[x].a.b.y-a.b.y)<=eps && 
									fabs(list.list[x].a.b.z-a.b.z)<=eps) return x;
	return result;
}

int searchedgeendvertex(pathedgelist list, vector3 a, int match)
{
	int result=-1;
	float eps=0.00001f;

	for (int x=0; x<list.pathedgenum; x++)
	if (x!=match && fabs(list.list[x].b.b.x-a.b.x)<=eps && 
									fabs(list.list[x].b.b.y-a.b.y)<=eps && 
									fabs(list.list[x].b.b.z-a.b.z)<=eps) return x;
	return result;
}

int searchnext(pathedgelist list, int curr)
{
	int result=-1;
	int nexta=searchedgestartvertex(list,list.list[curr].b,curr);
	int nextb=searchedgeendvertex(list,list.list[curr].b,curr);
	if (nexta!=-1 && nextb==-1) result=nexta;
	if (nexta==-1 && nextb!=-1) 
	{
		vector3 b=list.list[nextb].a;
		list.list[nextb].a=list.list[nextb].b;
		list.list[nextb].b=b;
		result=nextb;
	}
	return result;
}

int searchprev(pathedgelist list, int curr)
{
	int result=-1;
	int nexta=searchedgestartvertex(list,list.list[curr].a,curr);
	int nextb=searchedgeendvertex(list,list.list[curr].a,curr);
	if (nexta!=-1 && nextb==-1)
	{
		vector3 b=list.list[nexta].a;
		list.list[nexta].a=list.list[nexta].b;
		list.list[nexta].b=b;
		result=nexta;
	}
	if (nexta==-1 && nextb!=-1) result=nextb;
	return result;
}


void arrangepaths(pathedgelist &list)
{
	char *ss = new char[256];

	for (int x=0; x<list.pathedgenum; x++)
		if (!list.list[x].inpath)
		{
			int prevnext=x;
			int prev=searchprev(list,x);
			while (!(prev==x || prev==-1)) { prevnext=prev; prev=searchprev(list,prev);}
			if (prev==-1)
			{//nem kort talaltunk
				swapedges(list.list,x,prevnext); //megkerestuk a szakasz elso elemet, es az aktualissal lecsereltuk
				list.list[x].inpath=true;
				list.list[x].pathstart=true;
				int next=searchnext(list,x);
				int y=x+1;
				while (next!=-1)
				{
					swapedges(list.list,y,next);
					list.list[y].inpath=true;
					next=searchnext(list,y++);
				}
				list.list[y-1].pathend=true;
			}
			else
			{//kort talaltunk
				list.list[x].circlestart=true;
				list.list[x].inpath=true;
				int next=searchnext(list,x);
				int y=x+1;
				while (next!=x)
				{
					swapedges(list.list,y,next);
					list.list[y].inpath=true;
					next=searchnext(list,y++);
				}
				list.list[y-1].circleend=true;
			}
		}
}

typedef void (__stdcall *GluTessCallbackType)();
void __stdcall EdgeFlagDataCB(GLboolean flag,void* lpContext){}

int polyindexcntr=0;
int polyindices[3];
object *objtoadd=NULL;
int polyshade=dd3d_flatshade;
int basepoly=0;
polygon poly;

int searchpoly(object *o1, int a, int b, int c)
{
	int v1,v2,v3;

	for (int x=0; x<o1->polynum; x++)
	{
		v1=o1->polygonlist[x].v1;
		v2=o1->polygonlist[x].v1;
		v3=o1->polygonlist[x].v1;
		if ((a==v1 && b==v2 && c==v3) || 
		    (a==v1 && b==v3 && c==v2) || 
			(a==v2 && b==v1 && c==v3) || 
			(a==v2 && b==v3 && c==v1) || 
			(a==v3 && b==v1 && c==v2) || 
			(a==v3 && b==v2 && c==v1)) return x;
	}

	return -1;
}

void __stdcall VertexCB(int vertex_data, object * polydata)
{
	polyindices[polyindexcntr++]=vertex_data;

	if (polyindexcntr==3)
	{
		polyindexcntr=0;
		if (searchpoly(objtoadd,polyindices[0],polyindices[1],polyindices[2])==-1) addpolygon(objtoadd,polyindices[0],polyindices[1],polyindices[2],polyshade);
		objtoadd->polygonlist[objtoadd->polynum-1]=poly;
		objtoadd->polygonlist[objtoadd->polynum-1].v1=polyindices[0];
		objtoadd->polygonlist[objtoadd->polynum-1].v2=polyindices[1];
		objtoadd->polygonlist[objtoadd->polynum-1].v3=polyindices[2];
	}
}

void __stdcall BeginCB(GLenum type)
{
	polyindexcntr=0;
}

void __stdcall EndCB(GLenum type)
{
/*	objtoadd->polygonlist[basepoly]=objtoadd->polygonlist[objtoadd->polynum-1];
	objtoadd->polynum--;/**/
}


int searchvertexindex(object *o1, vector3 v)
{
	int i=-1;
	float eps=0.00001f;
	for (int x=0; x<o1->vertexnum; x++)
	{
		if (fabs(o1->vertexlist[x].d.b.x-v.b.x)<eps &&
			fabs(o1->vertexlist[x].d.b.y-v.b.y)<eps &&
			fabs(o1->vertexlist[x].d.b.z-v.b.z)<eps) return x;
	}
	return i;
}

bool online(vector3 a, vector3 b, vector3 p, float &mu)
{
	mu=-1;
	float eps=0.000001f;
	bool result=false;
	vector3 ab=egyseg(vect_kivonas(b,a));
	vector3 ap=egyseg(vect_kivonas(p,a));
	float dp=(float)(fabs(dotproduct(ab,ap)));
	if (dp>=1.0f-eps) result=true;
	if (!result) return false;

	float mx=0,my=0,mz=0;
	vector3 n=vect_kivonas(b,a);
	vector3 pma=vect_kivonas(p,a);

	if (fabs(n.b.x)>=eps) mx=pma.b.x/n.b.x;
	if (fabs(n.b.y)>=eps) my=pma.b.y/n.b.y;
	if (fabs(n.b.z)>=eps) mz=pma.b.z/n.b.z;

	     if (fabs(n.b.x)>=eps) mu=mx;
	else if (fabs(n.b.y)>=eps) mu=my;
	else if (fabs(n.b.z)>=eps) mu=mz;

	return result;
}

void cutpolysides(pathedgelist &polypaths, pathedgelist &polyedgelist, vector3 a, vector3 b, vector3 c)
{ 
	float mu;
	pathedge elem;
	memset(&elem,0,sizeof(pathedge));
	elem.a=a; elem.sidepathentry=-1;
	addpathedge(polyedgelist,elem);
	elem.a=b; elem.mu=1;
	addpathedge(polyedgelist,elem);
	elem.a=c; elem.mu=2;
	addpathedge(polyedgelist,elem);
	int x;
	for (x=0; x<polypaths.pathedgenum; x++)
	{
		vector3 p;
		if (polypaths.list[x].pathstart)
		{
			p=polypaths.list[x].a;
			elem.a=p;
			
			     if (online(a,b,p,mu)) elem.mu=mu;
			else if (online(b,c,p,mu)) elem.mu=mu+1.0f;
			else if (online(c,a,p,mu)) elem.mu=mu+2.0f;

			elem.sidepathentry=x;
			addpathedge(polyedgelist,elem);
		}
		if (polypaths.list[x].pathend)
		{
			p=polypaths.list[x].b;
			elem.a=p;
			
			     if (online(a,b,p,mu)) elem.mu=mu;
			else if (online(b,c,p,mu)) elem.mu=mu+1.0f;
			else if (online(c,a,p,mu)) elem.mu=mu+2.0f;

			elem.sidepathentry=x;
			addpathedge(polyedgelist,elem);
		}
	}
	float smallest;
	int smallestindex;
	for (x=0; x<polyedgelist.pathedgenum; x++)
	{
		smallest=10;
		smallestindex=-1;
		for (int y=x; y<polyedgelist.pathedgenum; y++)
			if (polyedgelist.list[y].mu<smallest)
			{
				smallest=polyedgelist.list[y].mu;
				smallestindex=y;
			}
		swapedges(polyedgelist.list,x,smallestindex);	
		if (polyedgelist.list[x].sidepathentry!=-1)
		{
			if (polypaths.list[polyedgelist.list[x].sidepathentry].pathstart)
			polypaths.list[polyedgelist.list[x].sidepathentry].sidepathentry=x;
			if (polypaths.list[polyedgelist.list[x].sidepathentry].pathend)
			polypaths.list[polyedgelist.list[x].sidepathentry].sidepathentryend=x;
		}
	}
}

void addtessvertex(object *o, GLUtesselator *tess, vector3 vert)
{
	double vx[3];
	vx[0]=vert.b.x;
	vx[1]=vert.b.y;
	vx[2]=vert.b.z;
	int v=searchvertexindex(objtoadd,vert);
	if (v==-1) {addvertex(objtoadd,vert.b.x,vert.b.y,vert.b.z); v=objtoadd->vertexnum-1;}
	gluTessVertex(tess,reinterpret_cast<double*>(vx),reinterpret_cast<void*>(v));
}

int gothrupath(object *o, GLUtesselator *tess,pathedgelist &polypaths,pathedgelist &polyedgelist, int bee)
{
	int result=-1;
	int start=polyedgelist.list[bee].sidepathentry;
	
	if (polypaths.list[start].pathstart && !polypaths.list[start].pathend)
	{
		int z=start;
		while (!polypaths.list[z].pathend) addtessvertex(o,tess,polypaths.list[z++].b);
		result=polypaths.list[z].sidepathentryend;
	}
	if (polypaths.list[start].pathend && !polypaths.list[start].pathstart)
	{
		int z=start;
		while (!polypaths.list[z].pathstart) addtessvertex(o,tess,polypaths.list[z--].a);
		result=polypaths.list[z].sidepathentry;
	}

	if (polypaths.list[start].pathend && polypaths.list[start].pathstart)
	{
		int a=searchedgestartvertex(polyedgelist, polypaths.list[start].a,-1);
		int b=searchedgestartvertex(polyedgelist, polypaths.list[start].b,-1);

		if (bee==a) result=b;
		if (bee==b) result=a;
	}

	return result;
}

void goaround(object *o,GLUtesselator *tess,pathedgelist &polyedgelist,pathedgelist &polypaths,int start)
{
	
	addtessvertex(o,tess,polyedgelist.list[start].a);
	polyedgelist.list[start].beenhere=true;

	int z=start+1;
	while (z!=start)
	{		
		polyedgelist.list[z].beenhere=true;
		addtessvertex(o,tess,polyedgelist.list[z].a);

		if (polyedgelist.list[z].sidepathentry!=-1) //ha nem sarkon vagyunk eppen
		{ 
			z=gothrupath(o,tess,polypaths,polyedgelist,z);
			polyedgelist.list[z].beenhere=true;
			addtessvertex(o,tess,polyedgelist.list[z].a);
		}

		if (z!=start) z=(z+1)%polyedgelist.pathedgenum; //ha sarkon vagyunk akkor siman tovabbmegyunk
	}
	
}

void cutobject(object *o1,object *o1add,pathedgelist *obj1paths,pathedgelist *obj1abc, int o1pnum, int strt)
{
	objtoadd=o1add;
	for (int x=0; x<o1pnum;x++)
	{
		GLUtesselator* tess = gluNewTess();
		gluTessCallback(tess,GLU_TESS_BEGIN,reinterpret_cast<GluTessCallbackType>(BeginCB));
		//gluTessCallback(tess,GLU_TESS_END,reinterpret_cast<GluTessCallbackType>(EndCB));
		gluTessCallback(tess,GLU_TESS_EDGE_FLAG_DATA,reinterpret_cast<GluTessCallbackType>(EdgeFlagDataCB));
		gluTessCallback(tess,GLU_TESS_VERTEX_DATA,reinterpret_cast<GluTessCallbackType>(VertexCB));

		basepoly=x;
		poly=o1->polygonlist[x];

		gluTessBeginPolygon(tess,NULL);

		int start=strt;
		if (strt==1)
		{
			if (obj1abc[x].list[start].sidepathentry==-1) start=2;
			if (obj1abc[x].list[start].sidepathentry==-1) start=3;
		}

		//a kulso elek a metszo utakkal
		int y;
		for (y=start; y<obj1abc[x].pathedgenum; y+=2)
		{
			if (!obj1abc[x].list[y].beenhere && (obj1abc[x].list[y].sidepathentry!=-1 || y==start))
			{
				gluTessBeginContour(tess);
				goaround(o1,tess,obj1abc[x],obj1paths[x],y);
				gluTessEndContour(tess);
			}
		}

		//most jonnek a korok
		for (y=0; y<obj1paths[x].pathedgenum; y++)
		{
			if (obj1paths[x].list[y].circlestart)
			{
				gluTessBeginContour(tess);
				while (!obj1paths[x].list[y].circleend)
				{
					addtessvertex(o1,tess,obj1paths[x].list[y].a);
					y++;
				}
				addtessvertex(o1,tess,obj1paths[x].list[y].a);
				gluTessEndContour(tess);
			}
		}

		gluTessEndPolygon(tess);

		gluDeleteTess(tess);
	}

}

int rayIntersectsTriangle(vector3 p, vector3 d, 
			vector3 v0, vector3 v1, vector3 v2, float &t) 
{

	vector3 e1,e2,h,s,q;
	float a,f,u,v;
	
	e1=vect_kivonas(v1,v0);
	e2=vect_kivonas(v2,v0);
	h=cross(d,e2);
	a = dotproduct(e1,h);
	
	if (a > -0.00001 && a < 0.00001) return(false);
	
	f = 1/a;
	s=vect_kivonas(p,v0);
	u = f * (dotproduct(s,h));
	
	if (u < 0.0 || u > 1.0)	return(false);
	
	q=cross(s,e1);
	v = f * dotproduct(d,q);
	if (v < 0.0 || u + v > 1.0)	return(false);

    t = dotproduct(e2, q) * f;

	return(true);
}

void checkpolys(object *dummy, object *o)
{
	generatenormals(dummy);
	float t;

	for (int x=0; x<dummy->polynum; x++)
	{
		vector3 polycenter,raydir;
		polycenter= vect_osszeadas(
					vect_osszeadas(dummy->vertexlist[dummy->polygonlist[x].v1].d,
					               dummy->vertexlist[dummy->polygonlist[x].v2].d),
								   dummy->vertexlist[dummy->polygonlist[x].v3].d);
		polycenter.b.x/=3;
		polycenter.b.y/=3;
		polycenter.b.z/=3;
		raydir=egyseg(dummy->polygonlist[x].defaultnormal);
		int isectcounter=0;
	
		for (int y=0; y<o->polynum; y++)
		{
			if (rayIntersectsTriangle(polycenter,raydir,
				o->vertexlist[o->polygonlist[y].v1].d,
				o->vertexlist[o->polygonlist[y].v2].d,
				o->vertexlist[o->polygonlist[y].v3].d,t))
			{
				//vector3 ap=vect_kivonas(o->vertexlist[o->polygonlist[y].v3].d,polycenter);
				if (t>=0) isectcounter++;
			}
		}

		dummy->polygonlist[x].insideotherobject=isectcounter % 2;
	}

}

void movetooutput(object *output, object *in, int shared, int invertnormals)
{
	for (int x=0; x<in->polynum; x++)
	{
		if (( shared &&  in->polygonlist[x].insideotherobject) ||
		    (!shared && !in->polygonlist[x].insideotherobject))
		{
			int a,b,c;
			vector3 av=in->vertexlist[in->polygonlist[x].v1].d,
				    bv=in->vertexlist[in->polygonlist[x].v2].d,
					cv=in->vertexlist[in->polygonlist[x].v3].d;
			a=searchvertexindex(output,av);
			b=searchvertexindex(output,bv);
			c=searchvertexindex(output,cv);
			if (a==-1) {addvertex(output,av.b.x,av.b.y,av.b.z); a=output->vertexnum-1;}
			if (b==-1) {addvertex(output,bv.b.x,bv.b.y,bv.b.z); b=output->vertexnum-1;}
			if (c==-1) {addvertex(output,cv.b.x,cv.b.y,cv.b.z); c=output->vertexnum-1;}
			addpolygon(output,a,b,c,in->polygonlist[x].shading);
			output->polygonlist[output->polynum-1]=in->polygonlist[x];
			output->polygonlist[output->polynum-1].v1=a;
			output->polygonlist[output->polynum-1].v2=b;
			output->polygonlist[output->polynum-1].v3=c;
			if (invertnormals) output->polygonlist[output->polynum-1].normalinverted=!output->polygonlist[output->polynum-1].normalinverted;
		}
	}
}

void obj_boolean(object *o1, object *o2, int function)
{
	object *dummy1=newobject(),*dummy2=newobject();
	object *output=newobject();

	matrix m;
	memcpy(m,o1->xformmatrix,sizeof(matrix));
	m_invert(m);

	int z;
	for (z=0; z<o2->vertexnum; z++)
	{
		o2->vertexlist[z].base=o2->vertexlist[z].d;
		m_xformd(m,o2->vertexlist[z].generated,o2->vertexlist[z].d);
	}

	int o1pnum=o1->polynum, 
		o2pnum=o2->polynum;
	pathedgelist *obj1paths = new pathedgelist[o1->polynum],
		         *obj2paths = new pathedgelist[o2->polynum];
	memset(obj1paths,0,sizeof(pathedgelist)*o1->polynum);
	memset(obj2paths,0,sizeof(pathedgelist)*o2->polynum);
	
	pathedgelist *obj1abc = new pathedgelist[o1->polynum],
		         *obj2abc = new pathedgelist[o2->polynum];
	memset(obj1abc,0,sizeof(pathedgelist)*o1->polynum);
	memset(obj2abc,0,sizeof(pathedgelist)*o2->polynum);
	//ide mennek majd a polykhoz tartozo vagasgorbek meg a polygonok szelein levo szakaszok
	
	int coplanar=1;
	int x;
	for (x=0; x<o1pnum;x++)
		for (int y=0; y<o2pnum;y++)
		{
			coplanar=1;
			pathedge section;
			memset(&section,0,sizeof(pathedge));
			if (tri_tri_intersect_with_isectline(o1->vertexlist[o1->polygonlist[x].v1].d.a,o1->vertexlist[o1->polygonlist[x].v2].d.a,o1->vertexlist[o1->polygonlist[x].v3].d.a,
				                                 o2->vertexlist[o2->polygonlist[y].v1].d.a,o2->vertexlist[o2->polygonlist[y].v2].d.a,o2->vertexlist[o2->polygonlist[y].v3].d.a,
				&coplanar,
				section.a.a,section.b.a))
			{
				if (!coplanar)
				{
					//section-ban o1 x.polyjanak es o2 y.polyjanak metszet szakasza,
					//a ket poly erintkezik
					float eps=0.00001f;
					if (!(fabs(section.a.b.x-section.b.b.x)<=eps &&
						  fabs(section.a.b.y-section.b.b.y)<=eps &&
						  fabs(section.a.b.z-section.b.b.z)<=eps))
					{
						addpathedge(obj1paths[x],section);
						addpathedge(obj2paths[y],section);
					}
				}
			}
		}

	obj_transform(o2,o2->xformmatrix);

	for (x=0; x<o1pnum;x++) arrangepaths(obj1paths[x]);
	for (x=0; x<o2pnum;x++) arrangepaths(obj2paths[x]);

	//polyoldalak vagasanak szamitasa
	for (x=0; x<o1pnum;x++)	cutpolysides(obj1paths[x],obj1abc[x],o1->vertexlist[o1->polygonlist[x].v1].d,o1->vertexlist[o1->polygonlist[x].v2].d,o1->vertexlist[o1->polygonlist[x].v3].d);
	for (x=0; x<o2pnum;x++)	cutpolysides(obj2paths[x],obj2abc[x],o2->vertexlist[o2->polygonlist[x].v1].d,o2->vertexlist[o2->polygonlist[x].v2].d,o2->vertexlist[o2->polygonlist[x].v3].d);

	//polyvagas kovetkezik GLU RULEZ, legalabb valamit ne en csinaljak ;-)
	cutobject(o1,dummy1,obj1paths,obj1abc,o1pnum,0);
	cutobject(o2,dummy2,obj2paths,obj2abc,o2pnum,0);
	for (x=0; x<o1pnum;x++)	for (int y=0; y<obj1abc[x].pathedgenum;y++)	obj1abc[x].list[y].beenhere=false;
	for (x=0; x<o2pnum;x++)	for (int y=0; y<obj2abc[x].pathedgenum;y++)	obj2abc[x].list[y].beenhere=false;
	cutobject(o1,dummy1,obj1paths,obj1abc,o1pnum,1);
	cutobject(o2,dummy2,obj2paths,obj2abc,o2pnum,1);/**/

	//polyvagas vege, lecc boolean

	checkpolys(dummy1,o2);
	checkpolys(dummy2,o1);

	switch (function)
	{
	case 0:  //add
		{ 
			movetooutput(output,dummy1,false,false);
			movetooutput(output,dummy2,false,false);
			break;
		}
	case 1:  //subtract
		{ 
			movetooutput(output,dummy1,false,false);
			movetooutput(output,dummy2,true,true);
			break;
		}
	case 2:  //intersect
		{ 
			movetooutput(output,dummy1,true,false);
			movetooutput(output,dummy2,true,false);
			break;
		}
	}

	//eeeeeeeeees keeeeeeeeeeeeeeeeeeeesz :))))))))

	delete[] o1->vertexlist;
	delete[] o1->polygonlist;

	o1->vlistsize=output->vlistsize;
	o1->vertexnum=output->vertexnum;
	o1->plistsize=output->plistsize;
	o1->polynum=output->polynum;
	o1->vertexlist=output->vertexlist;
	o1->polygonlist=output->polygonlist;
	generatenormals(o1);
	obj_transform(o1,o1->xformmatrix);

	for (z=0; z<o2->vertexnum; z++)	o2->vertexlist[z].d=o2->vertexlist[z].base;
	obj_transform(o2,o2->xformmatrix);
}

float getmappixel(vector2 t, int channel, bool nowrap)
{
	int xp,yp;
	if (!nowrap)
	{
		xp=((int)(t.b.y*255.0f)) % 256;
		yp=((int)(t.b.x*255.0f)) % 256;
		if (xp<0) xp+=256;
		if (yp<0) yp+=256;
	}
	else
	{
		xp=((int)(t.b.y*255.0f));
		yp=((int)(t.b.x*255.0f));
	}
	int b=0;
	if (xp>=0 && xp<256 && yp>=0 && yp<256)
	{
		switch (channel)
		{
		case 0:{b=maptexture[xp][yp].a.r;break;}
		case 1:{b=maptexture[xp][yp].a.g;break;}
		case 2:{b=maptexture[xp][yp].a.b;break;}
		case 3:{b=(maptexture[xp][yp].a.r+maptexture[xp][yp].a.g+maptexture[xp][yp].a.b)/3;break;}
		}
	}
	return (float)((float)b/255.0f);
}

void scene_render(scene &scn)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	actualtexture=0;
	
	int l;
	for (l=0;l<=7;l++)
		if (scn.lights[l].turnedon)
		{
			glEnable(scn.lights[l].identifier);
			glLightfv(scn.lights[l].identifier,GL_AMBIENT,scn.lights[l].ambient);
			glLightfv(scn.lights[l].identifier,GL_DIFFUSE,scn.lights[l].color);
			glLightfv(scn.lights[l].identifier,GL_SPECULAR,scn.lights[l].color);
			glLightfv(scn.lights[l].identifier,GL_POSITION,scn.lights[l].position);
			glLightfv(scn.lights[l].identifier,GL_SPOT_DIRECTION,scn.lights[l].spot_direction);
			glLightfv(scn.lights[l].identifier,GL_SPOT_EXPONENT,&scn.lights[l].spot_exponent);
			glLightfv(scn.lights[l].identifier,GL_SPOT_CUTOFF,&scn.lights[l].spot_cutoff);
			glLightfv(scn.lights[l].identifier,GL_CONSTANT_ATTENUATION,&scn.lights[l].c_att);
			glLightfv(scn.lights[l].identifier,GL_LINEAR_ATTENUATION,&scn.lights[l].l_att);
			glLightfv(scn.lights[l].identifier,GL_QUADRATIC_ATTENUATION,&scn.lights[l].q_att);
		}
		else
		{
			glDisable(scn.lights[l].identifier);
		}
	
	object *o;
	glDisable(GL_BLEND);
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (o->objdata.color[3]==255)
		{
			color[0]=(float)(o->objdata.color[0]/255.0);
			color[1]=(float)(o->objdata.color[1]/255.0);
			color[2]=(float)(o->objdata.color[2]/255.0);
			color[3]=(float)(o->objdata.color[3]/255.0);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
			int xa;
			for (xa=0;xa<o->polynum;xa++)
			{
				if (o->objdata.textured)
				{
					if (o->polygonlist[xa].materialhandle!=actualtexture)
					{
						glEnd();
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D,o->polygonlist[xa].materialhandle);
						actualtexture=o->polygonlist[xa].materialhandle;
						glBegin(GL_TRIANGLES);
					}
				}
				if ((!o->objdata.textured) || (o->polygonlist[xa].materialhandle==0))
				{
					glEnd();
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_TRIANGLES);
					actualtexture=0;
				}
				if (o->polygonlist[xa].shading==dd3d_flatshade) glNormal3f(o->polygonlist[xa].generatednormal.b.x,o->polygonlist[xa].generatednormal.b.y,o->polygonlist[xa].generatednormal.b.z);

				if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.z);
				if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t1.b.x,o->polygonlist[xa].t1.b.y);
				glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
				if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.z);
				if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t2.b.x,o->polygonlist[xa].t2.b.y);
				glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
				if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.z);
				if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t3.b.x,o->polygonlist[xa].t3.b.y);
				glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
			}
		}
		o=o->next;
	}
	glEnd();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (o->objdata.color[3]<255)
		{
			color[0]=(float)(o->objdata.color[0]/255.0);
			color[1]=(float)(o->objdata.color[1]/255.0);
			color[2]=(float)(o->objdata.color[2]/255.0);
			color[3]=(float)(o->objdata.color[3]/255.0);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
			int xa;
			for (xa=0;xa<o->polynum;xa++)
			{
				if (o->objdata.textured)
				{
					if (o->polygonlist[xa].materialhandle!=actualtexture)
					{
						glEnd();
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D,o->polygonlist[xa].materialhandle);
						actualtexture=o->polygonlist[xa].materialhandle;
						glBegin(GL_TRIANGLES);
					}
				}
				if ((!o->objdata.textured) || (o->polygonlist[xa].materialhandle==0))
				{
					glEnd();
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_TRIANGLES);
				}
				if (o->polygonlist[xa].shading==dd3d_flatshade) glNormal3f(o->polygonlist[xa].generatednormal.b.x,o->polygonlist[xa].generatednormal.b.y,o->polygonlist[xa].generatednormal.b.z);

				if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v1].generatednormal.b.z);
				if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t1.b.x,o->polygonlist[xa].t1.b.y);
				glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
				if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v2].generatednormal.b.z);
				if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t2.b.x,o->polygonlist[xa].t2.b.y);
				glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
				if (o->polygonlist[xa].shading==dd3d_gouraudshade) glNormal3f(o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.x,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.y,o->vertexlist[o->polygonlist[xa].v3].generatednormal.b.z);
				if (o->objdata.textured) glTexCoord2f(o->polygonlist[xa].t3.b.x,o->polygonlist[xa].t3.b.y);
				glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
			}
		}
		o=o->next;
	}
	glEnd();
	glPopAttrib();
}
