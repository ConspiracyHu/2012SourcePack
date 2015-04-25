#include "engine.h"

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
 gluPerspective(c.fov,aspectratio,0.1,500.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 //glRotatef(1,1,1,(float)c.roll*360.0f/255.0f);
 vector3 dir=egyseg(vect_kivonas(c.target,c.eye));
 matrix m;
 m_rotate(dir.b.x,dir.b.y,dir.b.z,(float)c.roll*360.0f/255.0f*(float)radtheta,m);
 vector3 u;
 m_xformd(m,c.up,u);
 gluLookAt(c.eye.b.x,c.eye.b.y,c.eye.b.z,
           c.target.b.x,c.target.b.y,c.target.b.z,
           u.b.x,u.b.y,u.b.z);
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

bool checkedges(int a, int b, int x, int y, int z)
{
	return ((a==x) || (a==y) || (a==z)) && ((b==x) || (b==y) || (b==z)) && (a!=b);
}

bool checkfaces(object *o1, int a, int b)
{
	float szog=(float)acos(dotproduct(egyseg(o1->polygonlist[a].defaultnormal),
		                              egyseg(o1->polygonlist[b].defaultnormal)));
	return fabs(szog)>=pi/4.0;
}

int searchpolybyside(object *o,int a, int b, int cpoly)
{
	int buf=-1;
	for (int x=0; x<o->polynum; x++)
	{
		polygon p=o->polygonlist[x];
		if ((p.v1==a || p.v2==a || p.v3==a) && (p.v1==b || p.v2==b || p.v3==b) && x!=cpoly && a!=b) buf=x;
	}
	return buf;
}

void smoothgroups(object *o1)
{
	for (int x=0; x<o1->polynum; x++)
		/*for (int y=0; y<o1->polynum; y++)
		{
			if (x!=y)
			{
				if (checkedges(o1->polygonlist[x].v1,
					           o1->polygonlist[x].v2,
							   o1->polygonlist[y].v1,
							   o1->polygonlist[y].v2,
							   o1->polygonlist[y].v3) && checkfaces(o1,x,y))
				{
					
					addvertex(o1,o1->vertexlist[o1->polygonlist[x].v1].d.b.x,
						         o1->vertexlist[o1->polygonlist[x].v1].d.b.y,
								 o1->vertexlist[o1->polygonlist[x].v1].d.b.z);
					addvertex(o1,o1->vertexlist[o1->polygonlist[x].v2].d.b.x,
						         o1->vertexlist[o1->polygonlist[x].v2].d.b.y,
								 o1->vertexlist[o1->polygonlist[x].v2].d.b.z);
					o1->polygonlist[x].v1=o1->vertexnum-2;
					o1->polygonlist[x].v2=o1->vertexnum-1;
				}
				if (checkedges(o1->polygonlist[x].v2,
					           o1->polygonlist[x].v3,
							   o1->polygonlist[y].v1,
							   o1->polygonlist[y].v2,
							   o1->polygonlist[y].v3) && checkfaces(o1,x,y))
				{
					
					addvertex(o1,o1->vertexlist[o1->polygonlist[x].v2].d.b.x,
						         o1->vertexlist[o1->polygonlist[x].v2].d.b.y,
								 o1->vertexlist[o1->polygonlist[x].v2].d.b.z);
					addvertex(o1,o1->vertexlist[o1->polygonlist[x].v3].d.b.x,
						         o1->vertexlist[o1->polygonlist[x].v3].d.b.y,
								 o1->vertexlist[o1->polygonlist[x].v3].d.b.z);
					o1->polygonlist[x].v2=o1->vertexnum-2;
					o1->polygonlist[x].v3=o1->vertexnum-1;
				}
				if (checkedges(o1->polygonlist[x].v1,
					           o1->polygonlist[x].v3,
							   o1->polygonlist[y].v1,
							   o1->polygonlist[y].v2,
							   o1->polygonlist[y].v3) && checkfaces(o1,x,y))
				{
					
					addvertex(o1,o1->vertexlist[o1->polygonlist[x].v1].d.b.x,
						         o1->vertexlist[o1->polygonlist[x].v1].d.b.y,
								 o1->vertexlist[o1->polygonlist[x].v1].d.b.z);
					addvertex(o1,o1->vertexlist[o1->polygonlist[x].v3].d.b.x,
						         o1->vertexlist[o1->polygonlist[x].v3].d.b.y,
								 o1->vertexlist[o1->polygonlist[x].v3].d.b.z);
					o1->polygonlist[x].v1=o1->vertexnum-2;
					o1->polygonlist[x].v3=o1->vertexnum-1;
				}
			}
		}*/
	for (int x=0; x<o1->polynum; x++)
	{
		int p1,p2,p3;
		p1=searchpolybyside(o1,o1->polygonlist[x].v1,o1->polygonlist[x].v2,x);
		p2=searchpolybyside(o1,o1->polygonlist[x].v1,o1->polygonlist[x].v3,x);
		p3=searchpolybyside(o1,o1->polygonlist[x].v2,o1->polygonlist[x].v3,x);
	}

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
	buffer->color[0]=255;
	buffer->color[1]=255;
	buffer->color[2]=255;
	buffer->color[3]=255;
	buffer->clones=NULL;
	buffer->lastclone=NULL;
	buffer->cloneparents=NULL;
	buffer->effects=NULL;
	buffer->objdata.textured=false;
	buffer->edgelist=NULL;
	buffer->selected=false;
	buffer->objdata.texx=1;
	buffer->objdata.texy=1;
	buffer->anims=new objectanimation;
	buffer->anims->next=NULL;
	buffer->anims->number=0;
	buffer->anims->length=1000;
	buffer->anims->keyframecapacity=10;
	buffer->anims->keyframenum=0;
	buffer->anims->keyframes= new keyframe[10];
	buffer->lastanim=buffer->anims;
	buffer->currentorientation.stretch.b.x=1;
	buffer->currentorientation.stretch.b.y=1;
	buffer->currentorientation.stretch.b.z=1;
	buffer->transform.stretch.b.x=1;
	buffer->transform.stretch.b.y=1;
	buffer->transform.stretch.b.z=1;
	buffer->actualanim=buffer->anims;
	buffer->parent=-1;
	allocateobjecttracks(buffer->anims);

	return buffer;
}

void obj_transform(object *obj, matrix m)
{
	int x;

	matrix Rot_Mat;
	vector3 s1,s2,s3;
	v3_make(m[0][0],m[1][0],m[2][0],s1);
	v3_make(m[0][1],m[1][1],m[2][1],s2);
	v3_make(m[0][2],m[1][2],m[2][2],s3);
	s1=egyseg(s1);
	s2=egyseg(s2);
	s3=egyseg(s3);
	m_identity(Rot_Mat);
	Rot_Mat[0][0]=s1.b.x; Rot_Mat[1][0]=s1.b.y; Rot_Mat[2][0]=s1.b.z;
	Rot_Mat[0][1]=s2.b.x; Rot_Mat[1][1]=s2.b.y; Rot_Mat[2][1]=s2.b.z;
	Rot_Mat[0][2]=s3.b.x; Rot_Mat[1][2]=s3.b.y; Rot_Mat[2][2]=s3.b.z;
	memcpy(Rot_Mat,m,sizeof(matrix));
	m_invert(Rot_Mat);

	matrix rm;
	for (x=0;x<4;x++)
		for (int y=0;y<4;y++)
		{
			rm[x][y]=Rot_Mat[y][x];
		}

	m_xformd(m,obj->basecenter,obj->center);
	for (x=0;x<obj->vertexnum;x++)
	{
		m_xformd(m,obj->vertexlist[x].d,obj->vertexlist[x].generated);
		m_xform3(rm,obj->vertexlist[x].defaultnormal,obj->vertexlist[x].generatednormal);
		obj->vertexlist[x].generatednormal=egyseg(obj->vertexlist[x].generatednormal);
		//matrix
	}
	for (x=0;x<obj->polynum;x++)
	{
		m_xform3(rm,obj->polygonlist[x].defaultnormal,obj->polygonlist[x].generatednormal);
		obj->polygonlist[x].generatednormal=egyseg(obj->polygonlist[x].generatednormal);
	}
	memcpy(*obj->xformmatrix,*m,sizeof(matrix));
	glEnable(GL_NORMALIZE);
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
  for (x=0; x<obj->vertexnum; x++) obj->vertexlist[x].defaultnormal=origo;
  for (x=0; x<obj->polynum; x++)
  {
	  vector3 facetNormal;
	  memset(&facetNormal,0,sizeof(vector3));
	  vector3 center=obj->vertexlist[obj->polygonlist[x].v1].d,
				  v0=obj->vertexlist[obj->polygonlist[x].v2].d,
				  v1=obj->vertexlist[obj->polygonlist[x].v3].d;
    v0 = vect_kivonas(v0,center);
    v1 = vect_kivonas(v1,center);

	facetNormal=(cross(v1,v0));
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
	  /*vector3 v1,v2;
	  v1=vect_kivonas(obj->vertexlist[obj->polygonlist[x].v1].d,
		              obj->vertexlist[obj->polygonlist[x].v2].d);
	  v2=vect_kivonas(obj->vertexlist[obj->polygonlist[x].v1].d,
		              obj->vertexlist[obj->polygonlist[x].v3].d);
	  obj->vertexlist[obj->polygonlist[x].v1].defaultnormal=vect_osszeadas(obj->vertexlist[obj->polygonlist[x].v1].defaultnormal,vect_skalarszorzas(obj->polygonlist[x].defaultnormal,fabs(dotproduct(v1,v2))));
	  v1=vect_kivonas(obj->vertexlist[obj->polygonlist[x].v2].d,obj->vertexlist[obj->polygonlist[x].v1].d);
	  v2=vect_kivonas(obj->vertexlist[obj->polygonlist[x].v2].d,obj->vertexlist[obj->polygonlist[x].v3].d);
	  obj->vertexlist[obj->polygonlist[x].v2].defaultnormal=vect_osszeadas(obj->vertexlist[obj->polygonlist[x].v2].defaultnormal,vect_skalarszorzas(obj->polygonlist[x].defaultnormal,fabs(dotproduct(v1,v2))));
	  v1=vect_kivonas(obj->vertexlist[obj->polygonlist[x].v3].d,obj->vertexlist[obj->polygonlist[x].v1].d);
	  v2=vect_kivonas(obj->vertexlist[obj->polygonlist[x].v3].d,obj->vertexlist[obj->polygonlist[x].v2].d);
	  obj->vertexlist[obj->polygonlist[x].v3].defaultnormal=vect_osszeadas(obj->vertexlist[obj->polygonlist[x].v3].defaultnormal,vect_skalarszorzas(obj->polygonlist[x].defaultnormal,fabs(dotproduct(v1,v2))));*/
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

	 addvertex(obj,0,0,1.17);
	 addvertex(obj,1.05,0,0.52);
	 addvertex(obj,0.32,1,0.52);
	 addvertex(obj,-0.85,0.61,0.52);
	 addvertex(obj,-0.85,-0.61,0.52);
	 addvertex(obj,0.32,-1,0.52);
	 addvertex(obj,0.85,0.61,-0.52);
	 addvertex(obj,-0.32,1,-0.52);
	 addvertex(obj,-1.05,0,-0.52);
	 addvertex(obj,-0.32,-1,-0.52);
	 addvertex(obj,0.85,-0.61,-0.52);
	 addvertex(obj,0,0,-1.17);

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
	 addvertex(obj, 0.35	, 0.00	, 0.93);
	 addvertex(obj,-0.35	, 0.00	, 0.93);
	 addvertex(obj, 0.57	, 0.57	, 0.57);
	 addvertex(obj, 0.00	, 0.93	, 0.35);
	 addvertex(obj,-0.57	, 0.57	, 0.57);
	 addvertex(obj,-0.57	,-0.57  , 0.57);
	 addvertex(obj,-0.00	,-0.93  , 0.35);
	 addvertex(obj, 0.57	,-0.57  , 0.57);
	 addvertex(obj, 0.93	, 0.35	, 0.00);
	 addvertex(obj, 0.93	,-0.35  , 0.00);
	 addvertex(obj, 0.00	,-0.93  ,-0.35);
	 addvertex(obj, 0.57	,-0.57  ,-0.57);
	 addvertex(obj, 0.57	, 0.57	,-0.57);
	 addvertex(obj, 0.35	,-0.00  ,-0.93);
	 addvertex(obj,-0.57	,-0.57  ,-0.57);
	 addvertex(obj,-0.35	,-0.00  ,-0.93);
	 addvertex(obj, 0.00	, 0.93	,-0.35);
	 addvertex(obj,-0.57	, 0.57	,-0.57);
	 addvertex(obj,-0.93	,-0.35  ,-0.00);
	 addvertex(obj,-0.93	, 0.35	,-0.00);

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
			 setlastpolytexcoord(obj,obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d.b.x+0.5f,obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d.b.z+0.5f,
				                     obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d.b.x+0.5f,obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d.b.z+0.5f,
				                     obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d.b.x+0.5f,obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d.b.z+0.5f);
			 addpolygon(obj,(param1)*(param2)+xx,obj->vertexnum-1,(param1)*(param2)+(xx+1)%param1,dd3d_flatshade);
			 setlastpolytexcoord(obj,obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d.b.x+0.5f,obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d.b.z+0.5f,
				                     obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d.b.x+0.5f,obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d.b.z+0.5f,
				                     obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d.b.x+0.5f,obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d.b.z+0.5f);
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
			 //addvertex(obj,(double)objs->vertexlist[xa].generated.b.x,(double)objs->vertexlist[xa].generated.b.y,(double)objs->vertexlist[xa].generated.b.z);
			 addvertex(obj,0,0,0);
			 obj->vertexlist[obj->vertexnum-1]=objs->vertexlist[xa];
			 obj->vertexlist[obj->vertexnum-1].base=obj->vertexlist[obj->vertexnum-1].generated;
			 obj->vertexlist[obj->vertexnum-1].d=obj->vertexlist[obj->vertexnum-1].generated;
			 obj->vertexlist[obj->vertexnum-1].defaultnormal=obj->vertexlist[obj->vertexnum-1].generatednormal;
		 }
		 for (xa=0;xa<objs->polynum;xa++)
		 {
			 //addpolygon(obj,objs->polygonlist[xa].v1+vn,objs->polygonlist[xa].v2+vn,objs->polygonlist[xa].v3+vn,objs->polygonlist[xa].shading);
			 addpolygon(obj,0,0,0,0);
			 obj->polygonlist[obj->polynum-1]=objs->polygonlist[xa];
			 obj->polygonlist[obj->polynum-1].v1+=vn;
			 obj->polygonlist[obj->polynum-1].v2+=vn;
			 obj->polygonlist[obj->polynum-1].v3+=vn;
			 obj->polygonlist[obj->polynum-1].d1=obj->polygonlist[obj->polynum-1].t1;
			 obj->polygonlist[obj->polynum-1].d2=obj->polygonlist[obj->polynum-1].t2;
			 obj->polygonlist[obj->polynum-1].d3=obj->polygonlist[obj->polynum-1].t3;

			 /*setlastpolytexcoord(obj,objs->polygonlist[xa].t1.b.y,objs->polygonlist[xa].t1.b.x,objs->polygonlist[xa].t2.b.y,objs->polygonlist[xa].t2.b.x,objs->polygonlist[xa].t3.b.y,objs->polygonlist[xa].t3.b.x);
			 obj->polygonlist[obj->polynum-1].materialhandle=objs->polygonlist[xa].materialhandle;
			 obj->polygonlist[obj->polynum-1].envmaphandle=objs->polygonlist[xa].envmaphandle;*/
		 }
		 sel=sel->next;
	 }

	 vector3 aa;
	 aa.b.x=0;
	 aa.b.y=0;
	 aa.b.z=0;
	 for (int x=0; x<obj->vertexnum; x++)
	 {
		 aa=vect_osszeadas(aa,obj->vertexlist[x].base);
	 }
	 aa=vect_skalarszorzas(aa,1.0f/(float)obj->vertexnum);
	 obj->basecenter=aa;


	 break;
				   }

 case dd3d_loft  : {
	 strcpy(obj->name,"Loft");
	 up.b.x=0;
	 up.b.y=-1;
	 up.b.z=0;

	 v1=searchobjectbynumber((object*)(param3),param1);
	 v2=searchobjectbynumber((object*)(param3),param2);
	 if (v1 && v2)
	 {
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
 //smoothgroups(obj);
 generatenormals(obj);
}

void clearscene(scene *s)
{
	scene *sc=s->next;
	char a[40];
	int num=s->number;
	memcpy(a,s->name,40);
	memset(s,0,sizeof(scene));
	s->number=num;
	s->next=sc;
	s->objects=NULL;
	s->lastobject=NULL;
	s->floatbytes=3;
	int l;
	for (l=0;l<=7;l++)
	{
		s->lights[l].c_att=1;
		s->lights[l].identifier=GL_LIGHT0+l;
		s->lights[l].spot_cutoff=180;
	}
	s->name=new char[40];
	memcpy(s->name,a,40);
	s->lights[0].turnedon=true;
	s->lights[0].position[0]=-1;
	s->lights[0].position[1]=-1;
	s->lights[0].position[2]=-1;
	s->lights[0].color[0]=1;
	s->lights[0].color[1]=1;
	s->lights[0].color[2]=1;
	s->lights[1].turnedon=true;
	s->lights[1].position[0]=1;
	s->lights[1].position[1]=1;
	s->lights[1].position[2]=1;
	s->lights[1].color[0]=0.3f;
	s->lights[1].color[1]=0.3f;
	s->lights[1].color[2]=0.3f;
	s->fogdensity=1.0f;
	s->fogend=1.0f;
	s->fogmode=GL_EXP;

	s->cameraanims=new cameraanimation;
	sprintf(s->cameraanims->name,"Default");
	s->cameraanims->next=NULL;
	s->cameraanims->length=1000;
	s->cameraanims->number=0;
	s->cameraanims->keyframecapacity=10;
	s->cameraanims->keyframenum=0;
	s->cameraanims->keyframes= new keyframe[10];
	s->lastcamanim=s->cameraanims;
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
	buffer->name=new char[40];
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
	buffer->fogdensity=1.0f;
	buffer->fogend=1.0f;
	buffer->fogmode=GL_EXP;

	buffer->cameraanims=new cameraanimation;
	sprintf(buffer->cameraanims->name,"Default");
	buffer->cameraanims->next=NULL;
	buffer->cameraanims->length=1000;
	buffer->cameraanims->number=0;
	buffer->cameraanims->keyframecapacity=10;
	buffer->cameraanims->keyframenum=0;
	buffer->cameraanims->keyframes= new keyframe[10];
	buffer->lastcamanim=buffer->cameraanims;
	for (int x=0; x<255; x++) buffer->animlengths[x]=1000;

	buffer->anims=new sceneobjanim;
	memset(buffer->anims,0,sizeof(sceneobjanim));
	sprintf(buffer->anims->name,"Default");
	buffer->anims->length=1000;
	buffer->anims->next=NULL;
	buffer->lastanim=buffer->anims;
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

	for (sceneobjanim *anim=scn->anims->next;anim;anim=anim->next)
	{
		objectanimation *buffer=new objectanimation;
		buffer->next=NULL;
		buffer->number=anim->number;
		buffer->keyframecapacity=10;
		buffer->keyframenum=0;
		buffer->keyframes= new keyframe[10];
		obj->lastanim->next=buffer;
		obj->lastanim=buffer;
	}

}

void scene_addanim(scene *scn)
{
	sceneobjanim *anim = new sceneobjanim;
	memset(anim,0,sizeof(sceneobjanim));
	anim->number=scn->lastanim->number+1;
	scn->lastanim->next=anim;
	scn->lastanim=anim;
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
					//if (o->polygonlist[x].generatednormal.a[b]<=0)
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
	if (!obj->track && obj->polynum)
	{
	glEnd();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBindTexture(GL_TEXTURE_2D,obj->polygonlist[0].envmaphandle);

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
	color[0]=(o->color[0]/255.0f);
	color[1]=(o->color[1]/255.0f);
	color[2]=(o->color[2]/255.0f);
	color[3]=(o->color[3]/255.0f);
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
	if (scn.fog)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_MODE,&scn.fogmode);
		glFogfv(GL_FOG_COLOR,scn.fogcol);
		glFogfv(GL_FOG_START,&scn.fogstart);
		glFogfv(GL_FOG_END,&scn.fogend);
		glFogfv(GL_FOG_DENSITY,&scn.fogdensity);
	}
	else
	{ glDisable(GL_FOG); }
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
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		if ((int)(o->color[3])==255)
		{
			if (!o->hidden)
			{
				color[0]=(o->color[0]/255.0f);
				color[1]=(o->color[1]/255.0f);
				color[2]=(o->color[2]/255.0f);
				color[3]=(o->color[3]/255.0f);
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
	if (scn.fog)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_MODE,&scn.fogmode);
		glFogfv(GL_FOG_COLOR,scn.fogcol);
		glFogfv(GL_FOG_START,&scn.fogstart);
		glFogfv(GL_FOG_END,&scn.fogend);
		glFogfv(GL_FOG_DENSITY,&scn.fogdensity);
	}
	else
	{ glDisable(GL_FOG); }
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
	if (scn.fog)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_MODE,&scn.fogmode);
		glFogfv(GL_FOG_COLOR,scn.fogcol);
		glFogfv(GL_FOG_START,&scn.fogstart);
		glFogfv(GL_FOG_END,&scn.fogend);
		glFogfv(GL_FOG_DENSITY,&scn.fogdensity);
	}
	else
	{ glDisable(GL_FOG); }
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
		if ((int)(o->color[3])<255)
		{
			if (!o->hidden)
			{
				color[0]=o->color[0]/255.0f;
				color[1]=o->color[1]/255.0f;
				color[2]=o->color[2]/255.0f;
				color[3]=o->color[3]/255.0f;
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
		//sortvertexedges(obj);
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
	int x;
	for (x=0; x<o->vertexnum;x++)
	{
		o->vertexlist[x].t.b.x=o->vertexlist[x].dt.b.x*xr+(float)xoff/255.0f;
		o->vertexlist[x].t.b.y=o->vertexlist[x].dt.b.y*yr+(float)yoff/255.0f;
	}
	for (x=0; x<o->polynum; x++)
	{
		o->polygonlist[x].t1.b.x=o->polygonlist[x].d1.b.x*(float)xr+(float)xoff/255.0f;
		o->polygonlist[x].t1.b.y=o->polygonlist[x].d1.b.y*(float)yr+(float)yoff/255.0f;
		o->polygonlist[x].t2.b.x=o->polygonlist[x].d2.b.x*(float)xr+(float)xoff/255.0f;
		o->polygonlist[x].t2.b.y=o->polygonlist[x].d2.b.y*(float)yr+(float)yoff/255.0f;
		o->polygonlist[x].t3.b.x=o->polygonlist[x].d3.b.x*(float)xr+(float)xoff/255.0f;
		o->polygonlist[x].t3.b.y=o->polygonlist[x].d3.b.y*(float)yr+(float)yoff/255.0f;
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

scene *findscene(scene *list, int id)
{
	for (scene *e=list; e; e=e->next) if (e->number==id) return e;
	return NULL;
}

cameraanimation *findcamanim(cameraanimation *list, int id)
{
	for (cameraanimation *e=list; e; e=e->next) if (e->number==id) return e;
	return NULL;
}


//////////// keyframer

void addkeyframe(objectanimation *anim)
{
	if (anim->keyframecapacity==anim->keyframenum)
	{
		keyframe *buffer = new keyframe[(anim->keyframecapacity+10)*2];
		memset(buffer,0,sizeof(keyframe)*(anim->keyframecapacity+10)*2);
		memcpy(buffer,anim->keyframes,sizeof(keyframe)*(anim->keyframecapacity));
		anim->keyframecapacity=(anim->keyframecapacity+10)*2;
		anim->keyframes=buffer;
	}
	anim->keyframenum++;
}

void addkeyframe(cameraanimation *anim)
{
	if (anim->keyframecapacity==anim->keyframenum)
	{
		keyframe *buffer = new keyframe[(anim->keyframecapacity+10)*2];
		memset(buffer,0,sizeof(keyframe)*(anim->keyframecapacity+10)*2);
		memcpy(buffer,anim->keyframes,sizeof(keyframe)*(anim->keyframecapacity));
		anim->keyframecapacity=(anim->keyframecapacity+10)*2;
		anim->keyframes=buffer;
	}
	anim->keyframenum++;
}

/*float hermite(float p1,float p2,float r1,float r2,float t) {
  return (float)(p1*( 2.0*(t*t*t)-3.0*(t*t)+1.0)+
				 r1*(     (t*t*t)-2.0*(t*t)+t  )+
				 p2*(-2.0*(t*t*t)+3.0*(t*t)    )+
				 r2*(     (t*t*t)-    (t*t)    ));
}*/

void sortkeyframes(objectanimation *anim)
{
	int smallest,smallestindex;
	keyframe buffer;
	for (int x=0; x<anim->keyframenum; x++)
	{
		smallest=anim->keyframes[x].frame;
		smallestindex=x;
		for (int y=x; y<anim->keyframenum; y++)
		{
			if (anim->keyframes[y].frame<smallest)
			{
				smallest=anim->keyframes[y].frame;
				smallestindex=y;
			}
		}
		buffer=anim->keyframes[x];
		anim->keyframes[x]=anim->keyframes[smallestindex];
		anim->keyframes[smallestindex]=buffer;
	}
}

void sortkeyframes(cameraanimation *anim)
{
	int smallest,smallestindex;
	keyframe buffer;
	for (int x=0; x<anim->keyframenum; x++)
	{
		smallest=anim->keyframes[x].frame;
		smallestindex=x;
		for (int y=x; y<anim->keyframenum; y++)
		{
			if (anim->keyframes[y].frame<smallest)
			{
				smallest=anim->keyframes[y].frame;
				smallestindex=y;
			}
		}
		buffer=anim->keyframes[x];
		anim->keyframes[x]=anim->keyframes[smallestindex];
		anim->keyframes[smallestindex]=buffer;
	}
}

void calculatecameraanim(cameraanimation *camanim)
{
	sortkeyframes(camanim);
	for (int x=0; x<camanim->keyframenum; x++)
	{
		camerakeyframe *c=(camerakeyframe*)camanim->keyframes[x].data;
		camanim->eyex.keys[x].data=c->eye.b.x;
		camanim->eyex.keys[x].frame=camanim->keyframes[x].frame;
		camanim->eyey.keys[x].data=c->eye.b.y;
		camanim->eyey.keys[x].frame=camanim->keyframes[x].frame;
		camanim->eyez.keys[x].data=c->eye.b.z;
		camanim->eyez.keys[x].frame=camanim->keyframes[x].frame;
		camanim->targetx.keys[x].data=c->target.b.x;
		camanim->targetx.keys[x].frame=camanim->keyframes[x].frame;
		camanim->targety.keys[x].data=c->target.b.y;
		camanim->targety.keys[x].frame=camanim->keyframes[x].frame;
		camanim->targetz.keys[x].data=c->target.b.z;
		camanim->targetz.keys[x].frame=camanim->keyframes[x].frame;
		camanim->rolltrack.keys[x].data=c->roll;
		camanim->rolltrack.keys[x].frame=camanim->keyframes[x].frame;
		camanim->fovtrack.keys[x].data=c->fov;
		camanim->fovtrack.keys[x].frame=camanim->keyframes[x].frame;
	}
	camanim->eyex.InitVectors();
	camanim->eyey.InitVectors();
	camanim->eyez.InitVectors();
	camanim->targetx.InitVectors();
	camanim->targety.InitVectors();
	camanim->targetz.InitVectors();
	camanim->fovtrack.InitVectors();
	camanim->rolltrack.InitVectors();
}

void calculateobjectanim(objectanimation *objanim)
{
	sortkeyframes(objanim);
	for (int x=0; x<objanim->keyframenum; x++)
	{
		objkeyframedata *c=(objkeyframedata*)objanim->keyframes[x].data;
		objanim->posx.keys[x].data=c->position.b.x;
		objanim->posx.keys[x].frame=objanim->keyframes[x].frame;
		objanim->posy.keys[x].data=c->position.b.y;
		objanim->posy.keys[x].frame=objanim->keyframes[x].frame;
		objanim->posz.keys[x].data=c->position.b.z;
		objanim->posz.keys[x].frame=objanim->keyframes[x].frame;

		objanim->strx.keys[x].data=c->stretch.b.x;
		objanim->strx.keys[x].frame=objanim->keyframes[x].frame;
		objanim->stry.keys[x].data=c->stretch.b.y;
		objanim->stry.keys[x].frame=objanim->keyframes[x].frame;
		objanim->strz.keys[x].data=c->stretch.b.z;
		objanim->strz.keys[x].frame=objanim->keyframes[x].frame;

		objanim->rotx.keys[x].data=c->rotaxis.b.x;
		objanim->rotx.keys[x].frame=objanim->keyframes[x].frame;
		objanim->roty.keys[x].data=c->rotaxis.b.y;
		objanim->roty.keys[x].frame=objanim->keyframes[x].frame;
		objanim->rotz.keys[x].data=c->rotaxis.b.z;
		objanim->rotz.keys[x].frame=objanim->keyframes[x].frame;
		objanim->rotphi.keys[x].data=c->rotangle;
		objanim->rotphi.keys[x].frame=objanim->keyframes[x].frame;

		objanim->colr.keys[x].data=c->color[0];
		objanim->colr.keys[x].frame=objanim->keyframes[x].frame;
		objanim->colg.keys[x].data=c->color[1];
		objanim->colg.keys[x].frame=objanim->keyframes[x].frame;
		objanim->colb.keys[x].data=c->color[2];
		objanim->colb.keys[x].frame=objanim->keyframes[x].frame;
		objanim->cola.keys[x].data=c->color[3];
		objanim->cola.keys[x].frame=objanim->keyframes[x].frame;
	
	}
	objanim->posx.InitVectors();
	objanim->posy.InitVectors();
	objanim->posz.InitVectors();
	objanim->strx.InitVectors();
	objanim->stry.InitVectors();
	objanim->strz.InitVectors();
	objanim->rotx.InitVectors();
	objanim->roty.InitVectors();
	objanim->rotz.InitVectors();
	objanim->rotphi.InitVectors();
	objanim->colr.InitVectors();
	objanim->colg.InitVectors();
	objanim->colb.InitVectors();
	objanim->cola.InitVectors();
}


void getcamerastate(cameraanimation *camanim, camera &c, float frame)
{
	if (camanim->keyframenum!=0)
	{
		c.up=origo;
		c.up.b.y=-1;
		c.eye.b.x=camanim->eyex.GetKey(frame);
		c.eye.b.y=camanim->eyey.GetKey(frame);
		c.eye.b.z=camanim->eyez.GetKey(frame);
		c.target.b.x=camanim->targetx.GetKey(frame);
		c.target.b.y=camanim->targety.GetKey(frame);
		c.target.b.z=camanim->targetz.GetKey(frame);
		c.fov=camanim->fovtrack.GetKey(frame);
		c.roll=camanim->rolltrack.GetKey(frame);
	}
}

void getobjectstate(objectanimation *objanim, object &c, float frame)
{
	if (objanim->keyframenum!=0)
	{		
		c.currentorientation.position.b.x=objanim->posx.GetKey(frame);
		c.currentorientation.position.b.y=objanim->posy.GetKey(frame);
		c.currentorientation.position.b.z=objanim->posz.GetKey(frame);

		c.currentorientation.rotaxis.b.x=objanim->rotx.GetKey(frame);
		c.currentorientation.rotaxis.b.y=objanim->roty.GetKey(frame);
		c.currentorientation.rotaxis.b.z=objanim->rotz.GetKey(frame);
		c.currentorientation.rotangle=(short)objanim->rotphi.GetKey(frame);

		c.currentorientation.stretch.b.x=objanim->strx.GetKey(frame);
		c.currentorientation.stretch.b.y=objanim->stry.GetKey(frame);
		c.currentorientation.stretch.b.z=objanim->strz.GetKey(frame);

		c.color[0]=objanim->colr.GetKey(frame);
		c.color[1]=objanim->colg.GetKey(frame);
		c.color[2]=objanim->colb.GetKey(frame);
		c.color[3]=objanim->cola.GetKey(frame);

		
		c.keyframercol[0]=c.color[0];
		c.keyframercol[1]=c.color[1];
		c.keyframercol[2]=c.color[2];
		c.keyframercol[3]=c.color[3];
	}
	matrix p,r,s;
	m_identity(p);
	m_identity(r);
	m_identity(s);
	m_xpose(c.currentorientation.position.b.x,c.currentorientation.position.b.y,c.currentorientation.position.b.z,p);
	m_scale(c.currentorientation.stretch.b.x,c.currentorientation.stretch.b.y,c.currentorientation.stretch.b.z,s);
	m_rotate(c.currentorientation.rotaxis.b.x,c.currentorientation.rotaxis.b.y,c.currentorientation.rotaxis.b.z,c.currentorientation.rotangle,r);
	m_mult(p,r,c.currentmatrix);
	m_mult(c.currentmatrix,s,c.currentmatrix);
}


void allocatecameratracks(cameraanimation *canim)
{
	//delete [] canim->eyex.keys; 
	canim->eyex.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->eyey.keys; 
	canim->eyey.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->eyez.keys; 
	canim->eyez.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->targetx.keys; 
	canim->targetx.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->targety.keys; 
	canim->targety.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->targetz.keys; 
	canim->targetz.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->fovtrack.keys; 
	canim->fovtrack.keys=new KEY[canim->keyframenum+1];
	//delete [] canim->rolltrack.keys; 
	canim->rolltrack.keys=new KEY[canim->keyframenum+1];
	canim->eyex.numkey=canim->keyframenum;
	canim->eyey.numkey=canim->keyframenum;
	canim->eyez.numkey=canim->keyframenum;
	canim->targetx.numkey=canim->keyframenum;
	canim->targety.numkey=canim->keyframenum;
	canim->targetz.numkey=canim->keyframenum;
	canim->fovtrack.numkey=canim->keyframenum;
	canim->rolltrack.numkey=canim->keyframenum;
}

void allocateobjecttracks(objectanimation *canim)
{
	canim->posx.keys=new KEY[canim->keyframenum+1];
	canim->posy.keys=new KEY[canim->keyframenum+1];
	canim->posz.keys=new KEY[canim->keyframenum+1];
	canim->strx.keys=new KEY[canim->keyframenum+1];
	canim->stry.keys=new KEY[canim->keyframenum+1];
	canim->strz.keys=new KEY[canim->keyframenum+1];
	canim->rotx.keys=new KEY[canim->keyframenum+1];
	canim->roty.keys=new KEY[canim->keyframenum+1];
	canim->rotz.keys=new KEY[canim->keyframenum+1];
	canim->rotphi.keys=new KEY[canim->keyframenum+1];
	canim->colr.keys=new KEY[canim->keyframenum+1];
	canim->colg.keys=new KEY[canim->keyframenum+1];
	canim->colb.keys=new KEY[canim->keyframenum+1];
	canim->cola.keys=new KEY[canim->keyframenum+1];

	canim->posx.numkey=canim->keyframenum;
	canim->posy.numkey=canim->keyframenum;
	canim->posz.numkey=canim->keyframenum;
	canim->strx.numkey=canim->keyframenum;
	canim->stry.numkey=canim->keyframenum;
	canim->strz.numkey=canim->keyframenum;
	canim->rotx.numkey=canim->keyframenum;
	canim->roty.numkey=canim->keyframenum;
	canim->rotz.numkey=canim->keyframenum;
	canim->rotphi.numkey=canim->keyframenum;
	canim->colr.numkey=canim->keyframenum;
	canim->colg.numkey=canim->keyframenum;
	canim->colb.numkey=canim->keyframenum;
	canim->cola.numkey=canim->keyframenum;
}

objectanimation *findobjanim(object *o, int animnum)
{
	objectanimation *oa=o->anims;
	while (oa)
	{
		if (oa->number==animnum) return oa;
		oa=oa->next;
	}
	return NULL;
}

void getscenestate(scene *actualscene, float currentframe, int animid)
{
	for (object *o=actualscene->objects;o;o=o->next)
	{
		if (!o->effect && !o->hidden)
		{
			o->changed=false;
			objectanimation *oa=findobjanim(o,animid);
			if (oa)
			getobjectstate(oa,*o,currentframe);
		}
	}
}

void calculateobjhierarchy(scene *actualscene,int parentid, object *parent)
{
	for (object *o=actualscene->objects;o;o=o->next)
		if (!o->effect && !o->hidden && o->parent==parentid)
		{
			matrix relative,parentmatrix;

			memcpy(relative,o->xformmatrix,sizeof(matrix));
			m_identity(parentmatrix);
			
			if (parentid!=-1)
			{
				memcpy(parentmatrix,parent->xformmatrix,sizeof(matrix));

				m_invert(parentmatrix);
				matrix i;
				memcpy(i,parentmatrix,sizeof(matrix));

				m_mult(relative,parentmatrix,relative); 
				//relative-ban az xformmatrix-ok relativ matrixa

				m_mult(i,relative,relative);
				m_mult(parent->currentmatrix,relative,relative);
				m_mult(parent->xformmatrix,relative,relative);

				memcpy(parentmatrix,parent->xformmatrix,sizeof(matrix));
			}
			
			memcpy(o->buffermatrix,o->xformmatrix,sizeof(matrix));

			matrix m;
			m_mult(relative,parentmatrix,m);
			m_mult(m,o->currentmatrix,m);

			obj_transform(o,m);
			if (parentid!=-1)
			{
				matrix i;
				memcpy(i,o->buffermatrix,sizeof(matrix));
				m_invert(i);
				m_mult(parent->currentmatrix,o->buffermatrix,m);
				m_mult(m,o->currentmatrix,m);
				m_mult(i,m,o->currentmatrix);
			}

			memcpy(o->xformmatrix,o->buffermatrix,sizeof(matrix));
			calculateobjhierarchy(actualscene,o->objdata.number,o);
		}
}


#include "boolean.cpp"