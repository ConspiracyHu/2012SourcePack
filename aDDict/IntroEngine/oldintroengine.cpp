#include "introengine.h"

GLuint actualtexture=0;
vector3 origo;
float *color = new float[4];

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
	origo.b.x=0;
	origo.b.y=0;
	origo.b.z=0;
}

void setcameraview(camera c, float aspectratio)
{
 mode3d=true;
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(c.fov,aspectratio,1.0,200.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(c.eye.b.x,c.eye.b.y,c.eye.b.z,
           c.target.b.x,c.target.b.y,c.target.b.z,
           c.up.b.x,c.up.b.y,c.up.b.z);
}


void addvertex(object *obj, double x, double y, double z, double nx, double ny, double nz)
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
	buffer.generated=buffer.v;
	buffer.normal.b.x=(float)nx;
	buffer.normal.b.y=(float)ny;
	buffer.normal.b.z=(float)nz;
	buffer.normal=egyseg(buffer.normal);
	buffer.defaultnormal=buffer.normal;
	buffer.generatednormal=buffer.normal;
    obj->vertexlist[obj->vertexnum]=buffer;
	obj->vertexnum++;
}

void addpolygon(object *obj, int v1, int v2, int v3, float nx, float ny, float nz, int shading)
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
	buffer.defaultnormal.b.x=nx;
	buffer.defaultnormal.b.y=ny;
	buffer.defaultnormal.b.z=nz;
	buffer.defaultnormal=egyseg(buffer.defaultnormal);
	buffer.shading=shading;
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
	buffer->objdata.color[0]=255;
	buffer->objdata.color[1]=255;
	buffer->objdata.color[2]=255;
	buffer->objdata.color[3]=255;
	buffer->objdata.textured=false;

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
}

void setlastpolytexcoord(object *obj, float y1, float x1, float y2, float x2, float y3, float x3)
{
	obj->polygonlist[obj->polynum-1].t1.b.x=x1;
	obj->polygonlist[obj->polynum-1].t1.b.y=y1;
	obj->polygonlist[obj->polynum-1].t2.b.x=x2;
	obj->polygonlist[obj->polynum-1].t2.b.y=y2;
	obj->polygonlist[obj->polynum-1].t3.b.x=x3;
	obj->polygonlist[obj->polynum-1].t3.b.y=y3;
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
	 addvertex(obj,-0.5,-0.5,-0.5,0,0,0);
	 addvertex(obj, 0.5,-0.5,-0.5,0,0,0);
	 addvertex(obj, 0.5, 0.5,-0.5,0,0,0);
	 addvertex(obj,-0.5, 0.5,-0.5,0,0,0);
	 addvertex(obj,-0.5,-0.5, 0.5,0,0,0);
	 addvertex(obj, 0.5,-0.5, 0.5,0,0,0);
	 addvertex(obj, 0.5, 0.5, 0.5,0,0,0);
	 addvertex(obj,-0.5, 0.5, 0.5,0,0,0);

	 addpolygon(obj,0,1,2,0,0,-1,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,1,1);
	 addpolygon(obj,0,2,3,0,0,-1,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,1,0,1);
	 addpolygon(obj,5,4,7,0,0,1,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,1,1);
	 addpolygon(obj,5,7,6,0,0,1,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,1,0,1);
	 addpolygon(obj,1,5,6,1,0,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,1,1);
	 addpolygon(obj,1,6,2,1,0,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,1,0,1);
	 addpolygon(obj,4,0,3,-1,0,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,1,1);
	 addpolygon(obj,4,3,7,-1,0,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,1,0,1);
	 addpolygon(obj,4,5,1,0,-1,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,1,1);
	 addpolygon(obj,4,1,0,0,-1,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,1,0,1);
	 addpolygon(obj,3,2,6,0,1,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,0,1,1);
	 addpolygon(obj,3,6,7,0,1,0,dd3d_flatshade);
	 setlastpolytexcoord(obj,0,0,1,1,0,1);
	 break;
				}

 case dd3d_sphere : {
	 
	 for (xx=0;xx<=(int)(param1-1);xx++)
	 {
		 theta=(xx/(param1-1.0)-0.5)*180.0;
		 for (yy=0; yy<=(int)(param2+1); yy++)
		 {
			 phi=((float)yy/(param2))*360;
			 addvertex(obj,cos(theta*radtheta)*cos(phi*radtheta)/2,
				 sin(theta*radtheta)/2,
				 cos(theta*radtheta)*sin(phi*radtheta)/2,
				 0,0,0);
			 v3_normalize(obj->vertexlist[obj->vertexnum-1].d, obj->vertexlist[obj->vertexnum-1].defaultnormal);

			 setlastvertextexcoord(obj,(float)xx/(float)(param1-1),(float)yy/(float)(param2));

		 }
	 }

	 for (xx=0;xx<=(int)(param1-2);xx++)
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,xx*(int)(param2+2)+yy,xx*(int)(param2+2)+yy+1,(xx+1)*(int)(param2+2)+yy+1,0,0,0,dd3d_gouraudshade);
		 obj->polygonlist[obj->polynum-1].defaultnormal=egyseg(vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d,
			 vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d,
			 obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d)));
		 addpolygon(obj,xx*(int)(param2+2)+yy,(xx+1)*(int)(param2+2)+yy+1,(xx+1)*(int)(param2+2)+yy,0,0,0,dd3d_gouraudshade);
		 obj->polygonlist[obj->polynum-1].defaultnormal=egyseg(vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d,
			 vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d,
			 obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d)));
	 }
	 break;
					}
	 
 case dd3d_grid: {
	 for (yy=0;yy<=(int)(param2);yy++)
	 for (xx=0;xx<=(int)(param1);xx++)
	 {
		 addvertex(obj,(float)xx/(float)(param1)-0.5,0,(float)yy/(float)(param2)-0.5,0,-1,0);
		 setlastvertextexcoord(obj,(float)xx/(float)((int)(param1)),-(float)yy/(float)((int)(param2)));
	 }
	 for (xx=0;xx<=(int)(param1-1);xx++)
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,yy*(int)(param1+1)+xx,
			 yy*(int)(param1+1)+xx+1,
			 (yy+1)*(int)(param1+1)+xx+1,0,-1,0,dd3d_gouraudshade);
		 addpolygon(obj,yy*(int)(param1+1)+xx,
			 (yy+1)*(int)(param1+1)+xx+1,
			 (yy+1)*(int)(param1+1)+xx,0,-1,0,dd3d_gouraudshade);
	 }
	 break;
				 }

 case dd3d_hasab : {
	 for (xx=0;xx<=(int)(param2);xx++)
	 for (yy=0;yy<=(int)(param1);yy++)
	 {
		 addvertex(obj,sin(((float)yy/(float)(param1))*360*radtheta)*0.5,
			 (float)xx/(float)(param2)-0.5,
			 cos(((float)yy/(float)(param1))*360*radtheta)*0.5
			 ,0,0,0);
		 obj->vertexlist[obj->vertexnum-1].defaultnormal=obj->vertexlist[obj->vertexnum-1].d;
		 obj->vertexlist[obj->vertexnum-1].defaultnormal.b.y=0;
		 obj->vertexlist[obj->vertexnum-1].defaultnormal=egyseg(obj->vertexlist[obj->vertexnum-1].defaultnormal);
		 setlastvertextexcoord(obj,(float)yy/(param1),(float)xx/(param2));
	 }

	 for (xx=0;xx<=(int)(param1-1);xx++)
	 for (yy=0;yy<=(int)(param2-1);yy++)
	 {
		 addpolygon(obj,yy*(int)(param1+1)+xx,
			 yy*(int)(param1+1)+xx+1,
			 (yy+1)*(int)(param1+1)+xx+1,0,-1,0,dd3d_gouraudshade);
		 addpolygon(obj,yy*(int)(param1+1)+xx,
			 (yy+1)*(int)(param1+1)+xx+1,
			 (yy+1)*(int)(param1+1)+xx,0,-1,0,dd3d_gouraudshade);
	 }
	 
	 if (param3)
	 {
		 for (xx=0;xx<=param1-2;xx++)
		 {
			 addpolygon(obj,0,xx,xx+1,0,-1,0,dd3d_flatshade);
			 addpolygon(obj,(param1+1)*(param2),(param1+1)*(param2)+xx,1+(param1+1)*(param2)+xx,0,1,0,dd3d_flatshade);
		 }
	 }

	 break;

				   }
 case dd3d_cone  : {
	 addvertex(obj,0,-0.5,0,0,-1,0);
	 obj->vertexlist[obj->vertexnum-1].t.b.x=0.5;
	 obj->vertexlist[obj->vertexnum-1].t.b.y=0.5;
	 for (yy=0;yy<=(int)(param1);yy++)
	 {
		 addvertex(obj,sin(((float)yy/(float)(param1))*360*radtheta)*0.5,
			 0.5,
			 cos(((float)yy/(float)(param1))*360*radtheta)*0.5
			 ,0,0,0);
		 obj->vertexlist[obj->vertexnum-1].defaultnormal=obj->vertexlist[obj->vertexnum-1].d;
		 obj->vertexlist[obj->vertexnum-1].defaultnormal.b.y=0;
		 obj->vertexlist[obj->vertexnum-1].defaultnormal=egyseg(obj->vertexlist[obj->vertexnum-1].defaultnormal);
		 setlastvertextexcoord(obj,(float)(sin(((float)yy/(float)(param1))*360*radtheta)*0.5+1),(float)(cos(((float)yy/(float)(param1))*360*radtheta)*0.5+1));
	 }
	 for (xx=0;xx<=(int)(param1)-1;xx++)
	 {
		 addpolygon(obj,0,xx+1,xx+2,0,0,0,dd3d_gouraudshade);
		 obj->polygonlist[obj->polynum-1].defaultnormal=egyseg(vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d,
			 vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d,
			 obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d)));
	 }
	 if (param2)
	 {
		 for (xx=1;xx<=(int)(param1)-2;xx++)
		 {
			 addpolygon(obj,1,xx+1,xx+2,0,0,0,dd3d_gouraudshade);
			 obj->polygonlist[obj->polynum-1].defaultnormal=egyseg(vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].d,
				 vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].d,
				 obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].d)));
		 }
	 }
	 break;
				   }

 case dd3d_arc   : {
	 obj->track=true;
	 for (xx=-1;xx<=(int)(param1+1);xx++)
	 {
		 addvertex(obj,0.5*sin(((float)xx/(float)(param1))*radtheta*param2 ),0,
			 0.5*cos(((float)xx/(float)(param1))*radtheta*param2 ),0,0,0);
		 obj->vertexlist[obj->vertexnum-1].defaultnormal=egyseg(obj->vertexlist[obj->vertexnum-1].d);
	 }
	 break;
				   }

 case dd3d_line  : {
	 obj->track=true;
	 for (xx=-1;xx<=(int)(param1+1);xx++) 
	 {
		 addvertex(obj,0,0,((float)xx/(float)(param1))-0.5,1,0,0);
	 }
	 break;	
				   }

 case dd3d_clone : {
	 SELECTION *sel=(SELECTION*)param1;
	 object *ob=(object*)param2,*objs;
	 int b,a = selsize(sel);
	 int vn,pn;

	 for (b=1;b<=a;b++)
	 {		 
		 objs=searchobjectbynumber(ob,sel->selected);
		 vn=obj->vertexnum;
		 pn=obj->polynum;
		 int xa;
		 for (xa=0;xa<objs->vertexnum;xa++)
		 {
			 addvertex(obj,(double)objs->vertexlist[xa].generated.b.x,(double)objs->vertexlist[xa].generated.b.y,(double)objs->vertexlist[xa].generated.b.z,(double)objs->vertexlist[xa].generatednormal.b.x,(double)objs->vertexlist[xa].generatednormal.b.y,(double)objs->vertexlist[xa].generatednormal.b.z);
			 setlastvertextexcoord(obj,objs->vertexlist[xa].t.b.y,objs->vertexlist[xa].t.b.x);
		 }
		 for (xa=0;xa<objs->polynum;xa++)
		 {
			 addpolygon(obj,objs->polygonlist[xa].v1+vn,objs->polygonlist[xa].v2+vn,objs->polygonlist[xa].v3+vn,objs->polygonlist[xa].generatednormal.b.x,objs->polygonlist[xa].generatednormal.b.y,objs->polygonlist[xa].generatednormal.b.z,objs->polygonlist[xa].shading);
			 setlastpolytexcoord(obj,objs->polygonlist[xa].t1.b.y,objs->polygonlist[xa].t1.b.x,objs->polygonlist[xa].t2.b.y,objs->polygonlist[xa].t2.b.x,objs->polygonlist[xa].t3.b.y,objs->polygonlist[xa].t3.b.x);
			 obj->polygonlist[obj->polynum-1].materialhandle=objs->polygonlist[xa].materialhandle;
		 }
		 sel=sel->next;
	 }

	 break;
				   }

 case dd3d_loft  : {
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
			 for (ya=1;ya<v1->vertexnum-1;ya++)
			 {
				 nv=vect_osszeadas(vect_skalarszorzas(nx,v2->vertexlist[ya].generated.b.x),
					               vect_skalarszorzas(ny,v2->vertexlist[ya].generated.b.z));
				 nv=vect_osszeadas(nv,v1->vertexlist[xa].generated);

				 addvertex(obj,(double)nv.b.x,(double)nv.b.y,(double)nv.b.z,0.0,0.0,0.0);
				 nv=vect_osszeadas(vect_skalarszorzas(nx,v2->vertexlist[ya].generatednormal.b.x),
				         	       vect_skalarszorzas(ny,v2->vertexlist[ya].generatednormal.b.z));

				 obj->vertexlist[obj->vertexnum-1].defaultnormal=egyseg(nv);
				 setlastvertextexcoord(obj,(float)xx/(float)(v1v-1),(float)yy/(float)(v2v-1));
				 yy++;
			 }
			 xx++;
		 }

		 for (xx=0;xx<=v2v-2;xx++)
		 for (yy=0;yy<=v1v-2;yy++)
		 {
			 addpolygon(obj,yy*v2v+xx,
				 yy*v2v+xx+1,
				 (yy+1)*v2v+xx+1,0,0,0,dd3d_gouraudshade);
			 obj->polygonlist[obj->polynum-1].defaultnormal=egyseg(vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].generated,
				 vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].generated,
				 obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].generated)));
			 addpolygon(obj,yy*v2v+xx,
				 (yy+1)*v2v+xx+1,
				 (yy+1)*v2v+xx,0,0,0,dd3d_gouraudshade);
			 obj->polygonlist[obj->polynum-1].defaultnormal=egyseg(vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v1].generated,
				 vect_osszeadas(obj->vertexlist[obj->polygonlist[obj->polynum-1].v2].generated,
				 obj->vertexlist[obj->polygonlist[obj->polynum-1].v3].generated)));
		 }


	 }
	 break;
				   }

 }

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

void scene_renderwireframe(scene &scn)
{

	object *o;

	glBegin(GL_LINES);
	o=scn.objects;
	while (o!=NULL)
	{
		if (!o->track)
		{
			int x;
			for (x=0;x<o->polynum;x++)
			{
				glVertex3f(o->vertexlist[o->polygonlist[x].v1].generated.b.x,o->vertexlist[o->polygonlist[x].v1].generated.b.y,o->vertexlist[o->polygonlist[x].v1].generated.b.z);
				glVertex3f(o->vertexlist[o->polygonlist[x].v2].generated.b.x,o->vertexlist[o->polygonlist[x].v2].generated.b.y,o->vertexlist[o->polygonlist[x].v2].generated.b.z);
				glVertex3f(o->vertexlist[o->polygonlist[x].v1].generated.b.x,o->vertexlist[o->polygonlist[x].v1].generated.b.y,o->vertexlist[o->polygonlist[x].v1].generated.b.z);
				glVertex3f(o->vertexlist[o->polygonlist[x].v3].generated.b.x,o->vertexlist[o->polygonlist[x].v3].generated.b.y,o->vertexlist[o->polygonlist[x].v3].generated.b.z);
				glVertex3f(o->vertexlist[o->polygonlist[x].v3].generated.b.x,o->vertexlist[o->polygonlist[x].v3].generated.b.y,o->vertexlist[o->polygonlist[x].v3].generated.b.z);
				glVertex3f(o->vertexlist[o->polygonlist[x].v2].generated.b.x,o->vertexlist[o->polygonlist[x].v2].generated.b.y,o->vertexlist[o->polygonlist[x].v2].generated.b.z);
			}
		}
		o=o->next;
	}
	glEnd();
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
	buffer=vect_skalarszorzas(o->vertexlist[vxid].generated,3.0f/4.0f);

	switch (o->vertexlist[vxid].vedgenum)
	{
	case 2:	{
		return vect_osszeadas(vect_skalarszorzas(o->vertexlist[vxid].generated,0.5),
			                  vect_skalarszorzas(o->vertexlist[e0].generated,0.5));
			}
	case 3: {
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,5.0f/12.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,-1.0f/12.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,-1.0f/12.0f));
		return buffer;
		break;
			}
	case 4: {
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,3.0f/8.0f));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,-1.0f/8.0f));
		return buffer;
		break;
			}
	case 6: {
		//buffer=vect_skalarszorzas(o->vertexlist[vxid].generated,1.0f/2.0f);
		memset(&buffer,0,sizeof(vector3));
		buffer=vect_skalarszorzas(o->vertexlist[e0].generated,1.0f/2.0f);

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		if (hh)	buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,1.0f/8.0f+2*w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,-1.0f/16.0f-w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,-1.0f/16.0f-w));

		edgevertexposition=(edgevertexposition+1)%o->vertexlist[vxid].vedgenum;
		edgeid=o->vertexlist[vxid].vedgelist[edgevertexposition];
		e0=getedgevertex(o,vxid,edgeid);
		if (hh)	buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,1.0f/8.0f+2*w));

		return buffer;
		break;
			}
	default: {
		for (x=0;x<o->vertexlist[vxid].vedgenum;x++)
		{
			int n=o->vertexlist[vxid].vedgenum;
			float val=(float)((0.25f+cos(2*pi*(float)x/(float)n)+0.5*cos(4*pi*(float)x/(float)n))/(float)n);
			buffer=vect_osszeadas(buffer,vect_skalarszorzas(o->vertexlist[e0].generated,val));
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

void tessellateEdges(object *obj, int* edgeVertMap,int* edgeEdgeMap, float w)
{
	int xi;
	
	for (xi=obj->edgenum-1;xi>=0;xi--)
	{
		vector3 newvertex,newnormal,n1,n2;
		/*v1=obj->vertexlist[obj->edgelist[xi].v1].d;
		v2=obj->vertexlist[obj->edgelist[xi].v2].d;*/
		n1=obj->vertexlist[obj->edgelist[xi].v1].defaultnormal;
		n2=obj->vertexlist[obj->edgelist[xi].v2].defaultnormal;

		newvertex=butterflytessellation(obj,xi,w);
		//newvertex=vect_osszeadas(vect_skalarszorzas(v1,0.5f),vect_skalarszorzas(v2,0.5f));
		newnormal.b.x=(n1.b.x+n2.b.x)/2.0f;
		newnormal.b.y=(n1.b.y+n2.b.y)/2.0f;
		newnormal.b.z=(n1.b.z+n2.b.z)/2.0f;
		
		addvertex(obj,newvertex.b.x,newvertex.b.y,newvertex.b.z,newnormal.b.x,newnormal.b.y,newnormal.b.z);

		edgeVertMap[xi]=obj->vertexnum-1;
		//addedge(obj,obj->edgelist[xi].v1,obj->vertexnum-1,false);
		edgeEdgeMap[xi]=obj->edgenum-1;
		//obj->edgelist[xi].v1=obj->vertexnum-1;
	}
}

void buildNewFaces(object *obj, int* edgeVertMap,int* edgeEdgeMap)
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
		obj->polygonlist[xi].v3=e;
		obj->polygonlist[xi].t3=te;

		addpolygon(obj,d,b,f,obj->polygonlist[xi].defaultnormal.b.x,obj->polygonlist[xi].defaultnormal.b.y,obj->polygonlist[xi].defaultnormal.b.z,obj->polygonlist[xi].shading);
		obj->polygonlist[obj->polynum-1].materialhandle=obj->polygonlist[xi].materialhandle;
		setlastpolytexcoord(obj,td.b.y,td.b.x,tb.b.y,tb.b.x,tf.b.y,tf.b.x);
		addpolygon(obj,e,f,c,obj->polygonlist[xi].defaultnormal.b.x,obj->polygonlist[xi].defaultnormal.b.y,obj->polygonlist[xi].defaultnormal.b.z,obj->polygonlist[xi].shading);
		obj->polygonlist[obj->polynum-1].materialhandle=obj->polygonlist[xi].materialhandle;
		setlastpolytexcoord(obj,te.b.y,te.b.x,tf.b.y,tf.b.x,tc.b.y,tc.b.x);
		addpolygon(obj,e,d,f,obj->polygonlist[xi].defaultnormal.b.x,obj->polygonlist[xi].defaultnormal.b.y,obj->polygonlist[xi].defaultnormal.b.z,obj->polygonlist[xi].shading);
		obj->polygonlist[obj->polynum-1].materialhandle=obj->polygonlist[xi].materialhandle;
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

void addvertexedge(vertex &obj, int v1)
{
	bool add=true;
	int x;
	for (x=0;x<obj.vedgenum;x++) if (obj.vedgelist[x]==v1) add=false;
	if (add) obj.vedgelist[obj.vedgenum++]=v1;
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

void butterflysubdivision(object *obj)
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

	tessellateEdges(obj,edgeVertMap,edgeEdgeMap, 0.0f);
	buildNewFaces(obj,edgeVertMap, edgeEdgeMap);


	delete[] edgeVertMap;
	delete[] edgeEdgeMap;

}
