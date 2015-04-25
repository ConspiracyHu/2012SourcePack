#include "Modellgui.h"

#define newtransformstuff

window3d modellviews[4];
char *s2 = new char[256];
char *sst = new char[256];

selectionlist *sel2;
int lightselected=0;
float colr,colg,colb,colx;
object *obj;
bool clonetransform = false;
int booleanop=0;
int bool1=0,bool2=0;
int o1start=0, o1pos=0, o2start=0, o2pos=0;

int maptransform = 0;
int mapmapping = 0;
int mapxr = 1, mapyr =1;
int mapxoff =0, mapyoff=0;
bool mapxswap=false,mapyswap=false;
int mapmaterial = 0;
int mapchannel = 0;
bool mapswap = false;
bool mapwrap = false;
byte bttrammount=1;
byte linearammount=1;
byte blurammount=1;
int objmenu=0;

vector3 selectcenterbase=origo;
vector3 selectcenter=origo;
GLuint backrawtex;

int pretransform;
bool maptransforming = false;
matrix mapmatrix;
matrix mapbufmatrix;

int scenenum=1, scenepos=0, sceneselected=0;

float CalcXGLScene(float z, float fov, float aspect)
{
 return (float)(fabs( (z*( -tan( radtheta*(fov/2.0) ) *aspect)) )*2.0);
}

float CalcYGLScene(float z, float fov, float aspect)
{
 return (float)(fabs((z*(tan(radtheta*(fov/2.0)))))*2.0);
}

void countselectcenter()
{
	vector v;
	object *o=actualscene->objects;
	while (!((o==NULL) || (o->selected && !o->track && !o->effect))) o=o->next;
	if (o!=NULL && o->vertexnum)
	{
		vector3 mi=o->vertexlist[0].generated,ma=o->vertexlist[0].generated;
		//vector3 mi=origo,ma=origo;
		while (o!=NULL)
		{
			if (o->selected && !o->track && !o->effect)
			{
				for (int x=0;x<o->vertexnum;x++)
				{
					v=o->vertexlist[x].generated;
					mi.b.x=min(mi.b.x,v.b.x);
					mi.b.y=min(mi.b.y,v.b.y);
					mi.b.z=min(mi.b.z,v.b.z);
					ma.b.x=max(ma.b.x,v.b.x);
					ma.b.y=max(ma.b.y,v.b.y);
					ma.b.z=max(ma.b.z,v.b.z);
				}
			}
			o=o->next;
		}
		vector3 aa;
		aa.b.x=(mi.b.x+ma.b.x)/2.0f;
		aa.b.y=(mi.b.y+ma.b.y)/2.0f;
		aa.b.z=(mi.b.z+ma.b.z)/2.0f;
		selectcenterbase=aa;
	}
}

void InitModellerGUI()
{

	memset(modellviews,0,sizeof(modellviews));
 
	int x;

	for (x=0; x<=3; x++)
	{
		modellviews[x].pos=x;
		modellviews[x].grid=true;
		modellviews[x].wireframe=true;
		modellviews[x].a=1;
		modellviews[x].b=1;
		modellviews[x].c=1;
		modellviews[x].xsize=320;
		modellviews[x].ysize=240;
	}

	
	modellviews[0].xpos=12;
	modellviews[0].ypos=33;
	modellviews[0].oxpos=12;
	modellviews[0].oypos=33;
	modellviews[0].cam=upcam;
	modellviews[0].name="TOP VIEW";
	modellviews[0].c1=0;
	modellviews[0].c2=2;
	modellviews[0].c3=1;
	modellviews[0].a=-1;
	modellviews[0].xn="Z";
	modellviews[0].yn="X";
	modellviews[0].transformation=1;
	modellviews[0].centertransform=true;
	modellviews[0].objspacetransform=false;
	modellviews[1].xpos=332;
	modellviews[1].ypos=33;
	modellviews[1].oxpos=332;
	modellviews[1].oypos=33;
	modellviews[1].cam=frontcam;
	modellviews[1].name="FRONT VIEW";
	modellviews[1].c1=0;
	modellviews[1].c2=1;
	modellviews[1].c3=2;
	modellviews[1].a=-1;
	modellviews[1].b=-1;
	modellviews[1].xn="-Y";
	modellviews[1].yn="X";
	modellviews[1].transformation=1;
	modellviews[1].centertransform=true;
	modellviews[1].objspacetransform=false;
	modellviews[2].xpos=12;
	modellviews[2].ypos=273;
	modellviews[2].oxpos=12;
	modellviews[2].oypos=273;
	modellviews[2].cam=leftcam;
	modellviews[2].name="LEFT VIEW";
	modellviews[2].c1=2;
	modellviews[2].c2=1;
	modellviews[2].c3=0;
	modellviews[2].b=-1;
	modellviews[2].xn="-Y";
	modellviews[2].yn="-Z";
	modellviews[2].transformation=1;
	modellviews[2].centertransform=true;
	modellviews[2].objspacetransform=false;
	modellviews[3].xpos=332;
	modellviews[3].ypos=273;
	modellviews[3].oxpos=332;
	modellviews[3].oypos=273;
	modellviews[3].cam=perspcam;
	modellviews[3].name="CAMERA";
	modellviews[3].c1=0;
	modellviews[3].c2=1;
	modellviews[3].a=-1;
	modellviews[3].wireframe=false;
	modellviews[3].camsensitivity=0.5;
	modellviews[3].transformation=1;
	modellviews[3].centertransform=false;
	modellviews[3].objspacetransform=false;
}

SELECTION *createselection(scene &scn)
{
	SELECTION *buffer=NULL,*buf2=NULL;

	object *o=scn.objects;
	while (o!=NULL)
	{
		if (o->selected)
		{
			buf2 = new SELECTION;
			memset(buf2,0,sizeof(SELECTION));
			buf2->next=buffer;
			buffer=buf2;
			buf2->selected=o->objdata.number;
		}
		o=o->next;
	}
	return buffer;
}

void recreateclone(object *o, matrix &m)
{
	SELECTION *selec=o->cloneparents;
	int vxpos=0,plpos=0;
	object *objs;	
	if (o->primitive==dd3d_clone)
	{
		while (selec!=NULL)
		{
			objs=searchobjectbynumber(actualscene->objects,selec->selected);
			int xa;
			for (xa=0;xa<objs->vertexnum;xa++)
			{
				o->vertexlist[vxpos].d=objs->vertexlist[xa].generated;
				o->vertexlist[vxpos].defaultnormal=objs->vertexlist[xa].generatednormal;
				m_xformd(o->xformmatrix,o->vertexlist[vxpos].d,o->vertexlist[vxpos].generated);
				m_xform3(o->xformmatrix,o->vertexlist[vxpos].defaultnormal,o->vertexlist[vxpos].generatednormal);
				vxpos++;
			}
			for (xa=0;xa<objs->polynum;xa++)
			{
				o->polygonlist[plpos].defaultnormal=objs->polygonlist[xa].defaultnormal;
				m_xform3(o->xformmatrix,o->polygonlist[plpos].defaultnormal,o->polygonlist[plpos].generatednormal);
				plpos++;
			}
			selec=selec->next;
		}
	}
}

void recreateclonehierarchy(object *o, matrix &m)
{
	SELECTION *selec=o->clones;
	object *clone;
	while (selec!=NULL)
	{		
		clone=searchobjectbynumber(actualscene->objects,selec->selected);
		recreateclone(clone,m);
		recreateclonehierarchy(clone,m);
		selec=selec->next;
	}
}

int parentsselected(object *o)
{
	SELECTION *sel=o->cloneparents;
	int result=0;
	while (sel!=NULL)
	{
		if (searchobjectbynumber(actualscene->objects,sel->selected)->selected) result++;
		sel=sel->next;
	}
	return result;
}

void mapxform()
{
	if (maptransforming)
	{
		object *o=actualscene->objects;
		while (o!=NULL)
		{
			if (o->selected)
			{
				switch (maptransform)
				{
				case 0:
					{
						vector3 xpse;
						m_revxpose(mapmatrix,xpse);
						matrix s;
						for (int a=0; a<o->vertexnum; a++)
						{
							float p=getmappixel(o->vertexlist[a].t,mapchannel,mapwrap);
							m_xpose(p*xpse.b.x,p*xpse.b.y,p*xpse.b.z,s);
							m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
						}									
						obj_transform(o,o->xformmatrix);
						break;
					}
				case 1:
					{
						vector3 xpse;
						m_revscale(mapmatrix,xpse);
						matrix s;
						for (int a=0; a<o->vertexnum; a++)
						{
							float p=getmappixel(o->vertexlist[a].t,mapchannel,mapwrap);
							m_scale(p*(xpse.b.x-1)+1,p*(xpse.b.y-1)+1,p*(xpse.b.z-1)+1,s);
							m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
						}									
						obj_transform(o,o->xformmatrix);
						break;
					}
				case 2:
					{
						vector3 xpse;
						float phi;
						m_revrotate(mapmatrix,xpse,phi);
						matrix s;
						for (int a=0; a<o->vertexnum; a++)
						{
							float p=getmappixel(o->vertexlist[a].t,mapchannel,mapwrap);
							m_rotate(xpse.b.x,xpse.b.y,xpse.b.z,p*phi,s);
							m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
						}									
						obj_transform(o,o->xformmatrix);
						break;
					}
				}
			}
			o=o->next;
		}
	}
}

#ifndef newtransformstuff
void dotransformation2(window3d &viewdata, int x1, int y1, int x2, int y2,float xsize, float ysize)
{

	matrix m1,m2,m3;
	vector3 l;
	object *o;

	if ((xformmenu) && (leftclickinwindow(x1,y1,x2,y2)))
	{
		if (viewdata.wireframe)
		{
			if (!leftclickinwindow(x1,y1,x1+16+16+16,y1+16))
			{
				transform=true;				
				o=actualscene->objects;
				while (o!=NULL)
				{					
					if (o->selected && !o->effect && !o->hidden)
					{
						memcpy(&m1,&o->xformmatrix,sizeof(matrix));

						if (rightbutton) { mx=lx; my=ly; }
						m_identity(m3);

						if (centertransform)
						{
							m_identity(m2);
							m_xpose(-o->center.b.x,-o->center.b.y,-o->center.b.z,m2);
							m_mult(m2,m1,m1);
							memcpy(&m3,&m2,sizeof(matrix));
						}

						m_identity(m2);

						switch (transformation)
						{
						case 0 : { //move
							l.a[viewdata.c1]=(-(mx-lx)*xsize/(x2-x1)*viewdata.a);
							l.a[viewdata.c2]=(-(my-ly)*ysize/(y2-y1)*viewdata.b);
							l.a[viewdata.c3]=0;
							m_xpose(l.b.x,l.b.y,l.b.z,m2);
							sprintf(cursorstring,"(%.3f,%.3f)",l.a[viewdata.c1],l.a[viewdata.c2]);
							break;
								 }
						case 1 : { //rotate
							l.a[viewdata.c1]=0;
							l.a[viewdata.c2]=0;
							l.a[viewdata.c3]=1;
							m_rotate(l.b.x,l.b.y,l.b.z,(float)((mx-lx)*radtheta),m2);
							sprintf(cursorstring,"(%d Degrees)",mx-lx);
							break;
								 }
						case 2 : { //scale
							l.a[viewdata.c1]=((lx-mx)*xsize/(x2-x1)*2-1)*viewdata.a;
							if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-l.a[viewdata.c1];
							l.a[viewdata.c2]=((ly-my)*ysize/(y2-y1)*2-1)*viewdata.b;
							if (viewdata.name=="TOP VIEW") l.a[viewdata.c2]=-l.a[viewdata.c2];
							l.a[viewdata.c3]=1;
							m_scale(l.b.x,l.b.y,l.b.z,m2);
							sprintf(cursorstring,"(%.3fx,%.3fx)",l.a[viewdata.c1],l.a[viewdata.c2]);
							break;
								 }
						case 3 : { //equal scale
							l.a[viewdata.c1]=((lx-mx)*xsize/(x2-x1)*2-1)*viewdata.a;
							if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-l.a[viewdata.c1];
							l.a[viewdata.c2]=l.a[viewdata.c1];
							l.a[viewdata.c3]=1;
							m_scale(l.b.x,l.b.y,l.b.z,m2);
							sprintf(cursorstring,"(%.3fx)",l.a[viewdata.c1]);
							break;
								 }
						case 4 : { //grow
							l.a[viewdata.c1]=((lx-mx)*xsize/(x2-x1)*2-1)*viewdata.a;
							if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-l.a[viewdata.c1];
							l.a[viewdata.c2]=l.a[viewdata.c1];
							l.a[viewdata.c3]=l.a[viewdata.c1];
							m_scale(l.b.x,l.b.y,l.b.z,m2);
							sprintf(cursorstring,"(%.3fx)",l.a[viewdata.c1]);
							break;
								 }
						}

						m_mult(m2,m3,m3);
						m_mult(m2,m1,m1);

						if (centertransform)
						{
							m_identity(m2);
							m_xpose(o->center.b.x,o->center.b.y,o->center.b.z,m2);
							m_mult(m2,m1,m1);
							m_mult(m2,m3,m3);
						}

						if (clonetransform)
						{

							if ((o->primitive==dd3d_clone) && (parentsselected(o)>0) && (parentsselected(o)<selsize(o->cloneparents)))
							{
								m_invert(m3,m2);
								m_mult(m1,m2,m1);
							}

							if ((o->primitive==dd3d_clone) && (parentsselected(o)==selsize(o->cloneparents)))
							{
								m_invert(m3,m2);
								m_mult(m1,m2,m1);
								//memcpy(m1,o->xformmatrix,sizeof(matrix));
							}
						}

						if (!maptransforming)
						obj_transform(o,m1);
						else
						{
							switch (maptransform)
							{
							case 0:
								{
									vector3 xpse;
									m_mult(mapmatrix,m2,mapmatrix);
									m_revxpose(mapmatrix,xpse);
									matrix s;
									for (int a=0; a<o->vertexnum; a++)
									{
										float p=getmappixel(o->vertexlist[a].t,matselected,mapchannel,mapwrap);
										m_xpose(p*xpse.b.x,p*xpse.b.y,p*xpse.b.z,s);
										m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
									}									
									obj_transform(o,o->xformmatrix);
									break;
								}
							case 1:
								{
									vector3 xpse;
									m_mult(mapmatrix,m2,mapmatrix);
									m_revscale(mapmatrix,xpse);
									matrix s;
									for (int a=0; a<o->vertexnum; a++)
									{
										float p=getmappixel(o->vertexlist[a].t,matselected,mapchannel,mapwrap);
										m_scale(p*(xpse.b.x-1)+1,p*(xpse.b.y-1)+1,p*(xpse.b.z-1)+1,s);
										m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
									}									
									obj_transform(o,o->xformmatrix);
									break;
								}
							case 2:
								{
									vector3 xpse;
									float phi;
									m_mult(mapmatrix,m2,mapmatrix);
									m_revrotate(mapmatrix,xpse,phi);
									matrix s;
									for (int a=0; a<o->vertexnum; a++)
									{
										float p=getmappixel(o->vertexlist[a].t,matselected,mapchannel,mapwrap);
										m_rotate(xpse.b.x,xpse.b.y,xpse.b.z,p*phi,s);
										m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
									}									
									obj_transform(o,o->xformmatrix);
									break;
								}
							}
						}
						recreateclonehierarchy(o,m2);

					}
					o=o->next;
				}

			}
    }
  }
}
#else
void dotransformation2(window3d &viewdata, int x1, int y1, int x2, int y2,float xsize, float ysize, int mode)
{
	matrix m1,m2,m3,t;
	m_identity(t);
	m_identity(m1);
	m_identity(m2);
	m_identity(m3);
	vector3 l;
	vector3 d=origo;
	int i=0;
	object *o;
	o=actualscene->objects;
	if (!leftbutton) transform=false;
	while (o!=NULL)
	{
		if (o->selected && !o->effect && !o->hidden)
		{
			d=vect_osszeadas(o->center,d);
			i++;
		}
		o=o->next;
	}
	if (i!=0) d=vect_skalarszorzas(d,1.0f/(float)i);
	countselectcenter();
	d=selectcenterbase;

	if (viewdata.name=="TOP VIEW") d.a[viewdata.c2]*=-1;
	if (viewdata.name=="LEFT VIEW") d.a[viewdata.c1]*=-1;
	int dx,dy;
	dx=(int)((d.a[viewdata.c1]+viewdata.cam.eye.a[viewdata.c1]*viewdata.a)/xsize*(x2-x1));
	dy=(int)((d.a[viewdata.c2]+viewdata.cam.eye.a[viewdata.c2]*viewdata.b)/ysize*(y2-y1));
	if (!transform)
	{
		viewdata.odx=dx;
		viewdata.ody=dy;
		countselectcenter();
		selectcenter=selectcenterbase;
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glScissor(x1,600-y2,x2-x1,y2-y1);
	glEnable(GL_SCISSOR_TEST);
	int xm,ym,ys;
	ys=(y2-y1)/3;
	xm=(int)((x2-x1)/2+x1)+dx;
	ym=(y2-y1)/2+y1+dy;

	if (rightbutton && transform)
	{
		mx=lx;
		my=ly;
	}
	if (i!=0)
	switch (viewdata.transformation)
	{
	case 1: //move
		{
			if (mode==DD_Draw)
			{
				if ((mouseinwindow(xm-10,ym-ys,xm+10,ym-ys/4-1) && !leftbutton) || leftclickinwindow(xm-10,ym-ys,xm+10,ym-ys/4-1))
				glColor4f(0,1,0,1);	else glColor4f(1,1,0,0.6f);
				line(xm,ym,xm,ym-ys);
				line(xm+1,ym-ys,xm+5,ym-ys+4);
				line(xm,ym-ys,xm-5,ym-ys+5);
				
				if ((mouseinwindow((int)(xm+ys/4+1),ym-10,xm+ys,ym+10) && !leftbutton) || leftclickinwindow(xm+ys/4+1,ym-10,xm+ys,ym+10))
				glColor4f(0,1,0,1);	else glColor4f(1,1,0,0.6f);
				line(xm,ym,xm+ys,ym);
				line(xm+ys,ym-1,xm+ys-5,ym-6);
				line(xm+ys,ym,xm+ys-5,ym+5);
				
				glColor4f(1,1,0,1);
				rectangle(xm,ym-ys/4,xm+ys/4,ym);				
				if ((mouseinwindow(xm,ym-ys/4,xm+ys/4,ym) && !leftbutton) || leftclickinwindow(xm,ym-ys/4,xm+ys/4,ym))
				glColor4f(0,1,0,0.5f); else glColor4f(1,1,0,0.3f);				
				quad(xm,ym-ys/4,xm+ys/4,ym);
			}
			if (mode==DD_Check)
			{
				xm=(int)((x2-x1)/2+x1)+viewdata.odx;
				ym=(y2-y1)/2+y1+viewdata.ody;
				if (leftclickinwindow(x1,y1,x2,y2)) // move
				{
					l.a[viewdata.c1]=(-(mx-lx)*xsize/(x2-x1)*viewdata.a);
					l.a[viewdata.c2]=(-(my-ly)*ysize/(y2-y1)*viewdata.b);
					l.a[viewdata.c3]=0;
					if (leftclickinwindow(xm+ys/4+1,ym-10,xm+ys,ym+10)) l.a[viewdata.c2]=0;
					if (leftclickinwindow(xm-10,ym-ys,xm+10,ym-ys/4-1)) l.a[viewdata.c1]=0;
					m_xpose(l.b.x,l.b.y,l.b.z,t);
					
					if (leftclickinwindow(xm+ys/4+1,ym-10,xm+ys,ym+10)) sprintf(cursorstring,"(%.3f)",l.a[viewdata.c1]);
					else
					if (leftclickinwindow(xm-10,ym-ys,xm+10,ym-ys/4-1))	sprintf(cursorstring,"(%.3f)",l.a[viewdata.c2]);
					else
					sprintf(cursorstring,"(%.3f,%.3f)",l.a[viewdata.c1],l.a[viewdata.c2]);
					
				}
			}
			break;
		}
	case 2: //rotate
		{

			if (mode==DD_Draw)
			{
				double f=sqrt((float)(mx-xm)*(mx-xm)+(my-ym)*(my-ym));
				double f2=sqrt((float)(lx-xm)*(lx-xm)+(ly-ym)*(ly-ym));
				if ((f>11 || leftbutton) && ((mouseinwindow(xm-10,ym-ys+10,xm+10,ym+ys-10) && !leftbutton) || leftclickinwindow(xm-10,ym-ys+10,xm+10,ym+ys-10)))
				glColor4f(0,1,0,1);	else glColor4f(1,1,0,0.6f);
				line(xm,ym+ys,xm,ym-ys);
				
				if ((f>11 || leftbutton) && ((mouseinwindow((int)(xm-ys+10),ym-10,xm+ys-10,ym+10) && !leftbutton) || leftclickinwindow((int)(xm-ys+10),ym-10,xm+ys-10,ym+10)))
				glColor4f(0,1,0,1);	else glColor4f(1,1,0,0.6f);
				line(xm-ys,ym,xm+ys,ym);

				if ((f<=ys+10 && f>ys-8 && !leftbutton) || (f2<=ys+10 && f2>ys-8 && leftbutton)) glColor4f(0,1,0,1); else glColor4f(1,1,0,0.6f);
				glEnable(GL_LINE_SMOOTH);
				glBegin(GL_LINE_STRIP);
				for (x=0; x<49; x++) glVertex2d(xm+ys*sin(x*360/48*radtheta),ym+ys*cos(x*360/48*radtheta));
				glEnd();
				glDisable(GL_LINE_SMOOTH);				
			}

			if (mode==DD_Check)
			{
				xm=(int)((x2-x1)/2+x1)+viewdata.odx;
				ym=(y2-y1)/2+y1+viewdata.ody;
				double f=sqrt((float)(mx-xm)*(mx-xm)+(my-ym)*(my-ym));
				double f2=sqrt((float)(lx-xm)*(lx-xm)+(ly-ym)*(ly-ym));
				if (leftclickinwindow(xm-10,ym-ys+10,xm+10,ym+ys-10))
				{
					l.a[viewdata.c1]=1;
					if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-1;
					l.a[viewdata.c2]=0;					
					l.a[viewdata.c3]=0;
					m_rotate(l.b.x,l.b.y,l.b.z,(float)((my-ly)*radtheta),t);
					sprintf(cursorstring,"(%d Degrees)",my-ly);
				}
				else
				if (leftclickinwindow((xm-ys+10),ym-10,xm+ys-10,ym+10))
				{
					l.a[viewdata.c1]=0;					
					l.a[viewdata.c2]=-1;
					if (viewdata.name=="TOP VIEW") l.a[viewdata.c2]=1;
					l.a[viewdata.c3]=0;
					m_rotate(l.b.x,l.b.y,l.b.z,(float)((mx-lx)*radtheta),t);
					sprintf(cursorstring,"(%d Degrees)",mx-lx);
				}
				else
				if (leftbutton)
				{
					l.a[viewdata.c1]=0;
					l.a[viewdata.c2]=0;
					l.a[viewdata.c3]=1;
					m_rotate(l.b.x,l.b.y,l.b.z,(float)((mx-lx)*radtheta),t);
					sprintf(cursorstring,"(%d Degrees)",mx-lx);
				}
			}

			
			break;
		}
	case 3: //scale
		{
			if (mode==DD_Draw)
			{
				if ((mouseinwindow(xm-10,ym-ys,xm+10,ym-ys/4*3-1) && !leftbutton) || leftclickinwindow(xm-10,ym-ys,xm+10,ym-ys/4*3-1))
				glColor4f(0,1,0,1);	else glColor4f(1,1,0,0.6f);
				line(xm,ym,xm,ym-ys);
				line(xm+1,ym-ys,xm+5,ym-ys+4);
				line(xm,ym-ys,xm-5,ym-ys+5);
				
				if ((mouseinwindow((int)(xm+ys/4*3+1),ym-10,xm+ys,ym+10) && !leftbutton) || leftclickinwindow(xm+ys/4*3+1,ym-10,xm+ys,ym+10))
				glColor4f(0,1,0,1);	else glColor4f(1,1,0,0.6f);
				line(xm,ym,xm+ys,ym);
				line(xm+ys,ym-1,xm+ys-5,ym-6);
				line(xm+ys,ym,xm+ys-5,ym+5);
				
				glColor4f(1,1,0,1);
				line(xm,ym,xm,ym-ys/4*3);
				line(xm,ym,xm+ys/4*3,ym);
				line(xm,ym-ys/4,xm+ys/4,ym);
				line(xm,ym-ys/4*2,xm+ys/4*2,ym);
				line(xm,ym-ys/4*3,xm+ys/4*3,ym);

				
				if ((mouseinwindow(xm,ym-ys/4,xm+ys/4,ym) && (mx-xm)-(my-ym)<=ys/4 && !leftbutton) ||
				    (leftclickinwindow(xm,ym-ys/4,xm+ys/4,ym) && (lx-xm)-(ly-ym)<=ys/4 ))
				glColor4f(0,1,0,0.5f);
				else glColor4f(1,1,0,0.3f);
				glBegin(GL_TRIANGLES);
					glVertex2i(xm,ym);
					glVertex2i(xm,ym-ys/4);
					glVertex2i(xm+ys/4,ym);
				glEnd();

				if ((mouseinwindow(xm,ym-ys/4*2,xm+ys/4*2,ym) && ((mx-xm)-(my-ym)>ys/4) && ((mx-xm)-(my-ym)<=ys/4*2) && !leftbutton) ||
					(leftclickinwindow(xm,ym-ys/4*2,xm+ys/4*2,ym) && ((lx-xm)-(ly-ym)>ys/4) && ((lx-xm)-(ly-ym)<=ys/4*2)))
				glColor4f(0,1,0,0.5f);
				else glColor4f(1,1,0,0.3f);
				glBegin(GL_QUADS);
					glVertex2i(xm,ym-ys/4);
					glVertex2i(xm,ym-ys/4*2);
					glVertex2i(xm+ys/4*2,ym);
					glVertex2i(xm+ys/4,ym);
				glEnd();
				
				if ((mouseinwindow(xm,ym-ys/4*3,xm+ys/4*3,ym) && ((mx-xm)-(my-ym)>ys/4*2) && ((mx-xm)-(my-ym)<=ys/4*3) && !leftbutton) ||
					(leftclickinwindow(xm,ym-ys/4*3,xm+ys/4*3,ym) && ((lx-xm)-(ly-ym)>ys/4*2) && ((lx-xm)-(ly-ym)<=ys/4*3)))
				glColor4f(0,1,0,0.5f);
				else glColor4f(1,1,0,0.3f);
				glBegin(GL_QUADS);
					glVertex2i(xm,ym-ys/4*2);
					glVertex2i(xm,ym-ys/4*3);
					glVertex2i(xm+ys/4*3,ym);
					glVertex2i(xm+ys/4*2,ym);
				glEnd();

			}
			if (mode==DD_Check)
			{
				xm=(int)((x2-x1)/2+x1)+viewdata.odx;
				ym=(y2-y1)/2+y1+viewdata.ody;
				if (leftclickinwindow(x1,y1,x2,y2))
				{
					sprintf(cursorstring," ",0);
					if ((lx-xm)-(ly-ym)>ys/4 && (lx-xm)-(ly-ym)<=ys/4*2 && lx>xm && ly<ym)
					{
						l.a[viewdata.c1]=((lx-mx)*xsize/(x2-x1)*2-1)*viewdata.a;
						if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-l.a[viewdata.c1];
						sprintf(cursorstring,"(e-scale %.3fx)",l.a[viewdata.c1]);
						l.a[viewdata.c2]=l.a[viewdata.c1];
						l.a[viewdata.c3]=1;
					}
					else
					if ((lx-xm)-(ly-ym)<=ys/4 && (lx-xm)-(ly-ym)>=0)
					{
						l.a[viewdata.c1]=((lx-mx)*xsize/(x2-x1)*2-1)*viewdata.a;
						if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-l.a[viewdata.c1];
						sprintf(cursorstring,"(grow %.3fx)",l.a[viewdata.c1]);
						l.a[viewdata.c2]=l.a[viewdata.c1];
						l.a[viewdata.c3]=l.a[viewdata.c1];
					}
					else
					{
						l.a[viewdata.c1]=((lx-mx)*xsize/(x2-x1)*2-1)*viewdata.a;
						if (viewdata.name=="LEFT VIEW") l.a[viewdata.c1]=-l.a[viewdata.c1];
						if (leftclickinwindow(xm+ys/4*3+1,ym-10,xm+ys,ym+10))
							sprintf(cursorstring,"(x-scale %.3fx)",l.a[viewdata.c1]);
						if (leftclickinwindow(xm-10,ym-ys,xm+10,ym-ys/4*3-1)) l.a[viewdata.c1]=1;
						

						l.a[viewdata.c2]=((ly-my)*ysize/(y2-y1)*2-1)*viewdata.b;
						if (viewdata.name=="TOP VIEW") l.a[viewdata.c2]=-l.a[viewdata.c2];
						if (leftclickinwindow(xm-10,ym-ys,xm+10,ym-ys/4*3-1))
							sprintf(cursorstring,"(y-scale %.3fx)",l.a[viewdata.c2]);
						if (leftclickinwindow(xm+ys/4*3+1,ym-10,xm+ys,ym+10)) l.a[viewdata.c2]=1;

						l.a[viewdata.c3]=1;
					}

					m_scale(l.b.x,l.b.y,l.b.z,t);
					if (cursorstring[0]==' ')
						sprintf(cursorstring,"(%.3fx,%.3fx)",l.a[viewdata.c1],l.a[viewdata.c2]);
				}
			}
			break;
		}
	}
	if (mode==DD_Check)
	{
		m_xformd(t,selectcenterbase,selectcenter);
		if (leftclickinwindow(x1,y1,x2,y2))
		{
			transform=true;
			o=actualscene->objects;
			if (viewdata.transformation)
			while (o!=NULL)
			{
				if (o->selected && !o->effect && !o->hidden)
				{
					memcpy(&m1,&o->xformmatrix,sizeof(matrix));
					if (viewdata.centertransform)
					{
						m_identity(m2);
						m_xpose(-o->center.b.x,-o->center.b.y,-o->center.b.z,m2);
						m_mult(m2,m1,m1);
						memcpy(&m3,&m2,sizeof(matrix));
					}
					m_mult(t,m3,m3);
					if (!viewdata.objspacetransform)
					m_mult(t,m1,m1);
					else
					m_mult(m1,t,m1);
				

					if (viewdata.centertransform)
					{
						m_identity(m2);
						m_xpose(o->center.b.x,o->center.b.y,o->center.b.z,m2);
						m_mult(m2,m1,m1);
						m_mult(m2,m3,m3);
					}

					if (clonetransform)
					{

						if ((o->primitive==dd3d_clone) && (parentsselected(o)>0) && (parentsselected(o)<selsize(o->cloneparents)))
						{
							m_invert(m3,m2);
							m_mult(m1,m2,m1);
						}

						if ((o->primitive==dd3d_clone) && (parentsselected(o)==selsize(o->cloneparents)))
						{
							m_invert(m3,m2);
							m_mult(m1,m2,m1);
							//memcpy(m1,o->xformmatrix,sizeof(matrix));
						}
					}

					if (!maptransforming) obj_transform(o,m1);
					else
					{
						switch (maptransform)
						{
						case 0:
							{
								vector3 xpse;
								m_mult(mapmatrix,t,mapmatrix);
								m_revxpose(mapmatrix,xpse);
								matrix s;
								for (int a=0; a<o->vertexnum; a++)
								{
									float p=getmappixel(o->vertexlist[a].t,mapchannel,mapwrap);
									m_xpose(p*xpse.b.x,p*xpse.b.y,p*xpse.b.z,s);
									m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
								}									
								obj_transform(o,o->xformmatrix);
								break;
							}
						case 1:
							{
								vector3 xpse;
								m_mult(mapmatrix,t,mapmatrix);
								m_revscale(mapmatrix,xpse);
								matrix s;
								for (int a=0; a<o->vertexnum; a++)
								{
									float p=getmappixel(o->vertexlist[a].t,mapchannel,mapwrap);
									m_scale(p*(xpse.b.x-1)+1,p*(xpse.b.y-1)+1,p*(xpse.b.z-1)+1,s);
									m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
								}									
								obj_transform(o,o->xformmatrix);
								break;
							}
						case 2:
							{
								vector3 xpse;
								float phi;
								m_mult(mapmatrix,t,mapmatrix);
								m_revrotate(mapmatrix,xpse,phi);
								matrix s;
								for (int a=0; a<o->vertexnum; a++)
								{
									float p=getmappixel(o->vertexlist[a].t,mapchannel,mapwrap);
									m_rotate(xpse.b.x,xpse.b.y,xpse.b.z,p*phi,s);
									m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
								}									
								obj_transform(o,o->xformmatrix);
								break;
							}
						}
					}
					recreateclonehierarchy(o,t);
				}

				o=o->next;
			}
		}
	}
	glPopAttrib();
}
#endif

int polycounter(scene *scn)
{
	int pnum=0;
	for (object *x=scn->objects;x!=NULL;x=x->next) pnum+=x->polynum;
	return pnum;	
}

void scene_rendersolidmaptransform(scene &scn)
{
	float *color = new float[4];
	glPushAttrib(GL_ALL_ATTRIB_BITS); //ELOSZOR A TRANSZPARENS OBJEKTEKET ZBUFFER TEST NELKUL, CSAK ZBUFFERBE IRASSAL!
	actualtexture=0;

	glDisable(GL_LIGHTING);
	object *o;
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	o=scn.objects;
	while (o!=NULL)
	{
		int xa;
		float p;
		for (xa=0;xa<o->polynum;xa++)
		{
			p=getmappixel(o->vertexlist[o->polygonlist[xa].v1].t,mapchannel,mapwrap);
			glColor4f(1.0f*p,1.0f*p,0,1);
			glVertex3f(o->vertexlist[o->polygonlist[xa].v1].generated.b.x,o->vertexlist[o->polygonlist[xa].v1].generated.b.y,o->vertexlist[o->polygonlist[xa].v1].generated.b.z);
			
			p=getmappixel(o->vertexlist[o->polygonlist[xa].v2].t,mapchannel,mapwrap);
			glColor4f(1.0f*p,1.0f*p,0,1);
			glVertex3f(o->vertexlist[o->polygonlist[xa].v2].generated.b.x,o->vertexlist[o->polygonlist[xa].v2].generated.b.y,o->vertexlist[o->polygonlist[xa].v2].generated.b.z);
			
			p=getmappixel(o->vertexlist[o->polygonlist[xa].v3].t,mapchannel,mapwrap);
			glColor4f(1.0f*p,1.0f*p,0,1);
			glVertex3f(o->vertexlist[o->polygonlist[xa].v3].generated.b.x,o->vertexlist[o->polygonlist[xa].v3].generated.b.y,o->vertexlist[o->polygonlist[xa].v3].generated.b.z);
		}
		o=o->next;
	}
	glEnd();
	glPopAttrib();/**/
}

void displaywindow(window3d &win, unsigned char mode)
{
	int x1,y1,x2,y2;

	float xsize,ysize,dist;
	camera cam2 = win.cam;

	vector3 l;
	matrix m1,m2;

	x1=win.xpos;
	y1=win.ypos;
	x2=win.xpos+win.xsize;
	y2=win.ypos+win.ysize;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	if (mode==DD_Draw)
	{
		glEnable(GL_DEPTH_TEST);
		glColor4f(buttontextlit);
		rectangle(x1,y1,x2,y2);
		glDepthMask(false);
		glColor4f(0,0,0,0);
		if (actualscene->fog && !win.wireframe)
		glColor4f(actualscene->fogcol[0],actualscene->fogcol[1],actualscene->fogcol[2],0);
		if (!(backdraw && !win.wireframe))
		quad(x1,y1,x2,y2);
		else
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
		
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D,backrawtex);
			glColor4f(1,1,1,1);
			glBegin(GL_QUADS);
				glTexCoord2i(0,0);
				glVertex2i(x1,y1);
				glTexCoord2i(1,0);
				glVertex2i(x2,y1);
				glTexCoord2i(1,1);
				glVertex2i(x2,y2);
				glTexCoord2i(0,1);
				glVertex2i(x1,y2);
			glEnd();
			
			glPopAttrib();
		}
		glDepthMask(true);
	}

	
	setcameraview(win.cam,(float)(4.0/3.0));
	glViewport(x1+1, 600-y2+1, x2-x1-1, y2-y1-1);
	dist=(float)sqrt((win.cam.eye.a[win.c1]-win.cam.target.a[win.c1])*(win.cam.eye.a[win.c1]-win.cam.target.a[win.c1])+
		             (win.cam.eye.a[win.c2]-win.cam.target.a[win.c2])*(win.cam.eye.a[win.c2]-win.cam.target.a[win.c2])+
			         (win.cam.eye.a[win.c3]-win.cam.target.a[win.c3])*(win.cam.eye.a[win.c3]-win.cam.target.a[win.c3]));
	
	xsize=CalcXGLScene(dist,win.cam.fov,(float)(4.0/3.0));
	ysize=CalcYGLScene(dist,win.cam.fov,(float)(4.0/3.0));

	vector3 v1,v2;
	int xx;

	float ai=1,bi=1,ci=1;
	if (win.c3==0) ai=0;
	if (win.c3==1) bi=0;
	if (win.c3==2) ci=0;
	vector3 vv1,vv2;
	vv1.a[0]=(float)(ai*xsize*0.02);
	vv1.a[1]=(float)(bi*xsize*0.02);
	vv1.a[2]=(float)(ci*xsize*0.02);
	vv2=vv1;
	if (ai==0) vv2.a[1]*=-1;
	if (bi==0) vv2.a[0]*=-1;
	if (ci==0) vv2.a[0]*=-1;
	
	if (win.wireframe && !leftclickinwindow(x1,y1,x1+20+19*2,y1+19) && !leftclickinwindow(x2-19*3,y1,x2,y1+19) && !leftclickinwindow(x2-19,y1,x2,y1+19*3) && (mode==DD_Check))
	{
		if ((modellermenu==3) && (leftclickinwindow(x1,y1,x2,y2)) && (actualscene->lights[lightselected].turnedon))//light position
		{
			float v[4];
			for (x=0; x<4; x++)
			 v[x]=actualscene->lights[lightselected].position[x]+actualscene->lights[lightselected].spot_direction[x];
			actualscene->lights[lightselected].position[win.c1]=(float)(((float)(mx-x1)/(float)(x2-x1)-0.5)*xsize*win.a*-1+win.cam.eye.a[win.c1]);
			actualscene->lights[lightselected].position[win.c2]=(float)(((float)(my-y1)/(float)(y2-y1)-0.5)*ysize*win.b*-1+win.cam.eye.a[win.c2]);
			for (x=0; x<4; x++)
			 actualscene->lights[lightselected].spot_direction[x]=v[x]-actualscene->lights[lightselected].position[x];
		}
		if ((modellermenu==3) && (rightclickinwindow(x1,y1,x2,y2)) && (actualscene->lights[lightselected].turnedon) && (actualscene->lights[lightselected].spot_cutoff!=180))//light position
		{
			actualscene->lights[lightselected].spot_direction[win.c1]=(float)(((float)(mx-x1)/(float)(x2-x1)-0.5)*xsize*win.a*-1+win.cam.eye.a[win.c1])-actualscene->lights[lightselected].position[win.c1];
			actualscene->lights[lightselected].spot_direction[win.c2]=(float)(((float)(my-y1)/(float)(y2-y1)-0.5)*ysize*win.b*-1+win.cam.eye.a[win.c2])-actualscene->lights[lightselected].position[win.c2];
		}
	}

	if ((mode==DD_Draw) && (win.wireframe))
	{
		for (x=0;x<=7;x++) if (actualscene->lights[x].turnedon)
		{
			if (actualscene->lights[x].position[3]==0)
			{
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glBegin(GL_LINES);
				if ((x!=lightselected) || (modellermenu!=3)) {glColor4f(0.5,0.0,1.0,1);} else glColor4f(0,1,0,1);
				glVertex3f(0,0,0);
				glVertex3f(actualscene->lights[x].position[0]*ai,
					actualscene->lights[x].position[1]*bi,
					actualscene->lights[x].position[2]*ci);
				glEnd();
				glPopAttrib();
			}
			if (actualscene->lights[x].position[3]!=0) //dotlight || spotlight
			{
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glBegin(GL_LINES);
				if ((x!=lightselected) || (modellermenu!=3)) {glColor4f(0.5,0.0,1.0,1);} else glColor4f(0,1,0,1);
				
				glVertex3f(actualscene->lights[x].position[0]*ai-vv1.a[0],
					actualscene->lights[x].position[1]*bi-vv1.a[1],
					actualscene->lights[x].position[2]*ci-vv1.a[2]);
				glVertex3f(actualscene->lights[x].position[0]*ai+vv1.a[0],
					actualscene->lights[x].position[1]*bi+vv1.a[1],
					actualscene->lights[x].position[2]*ci+vv1.a[2]);
				glVertex3f(actualscene->lights[x].position[0]*ai-vv2.a[0],
					actualscene->lights[x].position[1]*bi-vv2.a[1],
					actualscene->lights[x].position[2]*ci-vv2.a[2]);
				glVertex3f(actualscene->lights[x].position[0]*ai+vv2.a[0],
					actualscene->lights[x].position[1]*bi+vv2.a[1],
					actualscene->lights[x].position[2]*ci+vv2.a[2]);
				glEnd();
				glPopAttrib();
			}
			if (actualscene->lights[x].spot_cutoff!=180) //spotlight
			{
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glBegin(GL_LINES);
				if ((x!=lightselected) || (modellermenu!=3)) {glColor4f(0.5,0.0,1.0,1);} else glColor4f(0,1,0,1);
				glVertex3f(actualscene->lights[x].position[0]*ai,
					actualscene->lights[x].position[1]*bi,
					actualscene->lights[x].position[2]*ci);
				glVertex3f((actualscene->lights[x].position[0]+actualscene->lights[x].spot_direction[0])*ai,
					(actualscene->lights[x].position[1]+actualscene->lights[x].spot_direction[1])*bi,
					(actualscene->lights[x].position[2]+actualscene->lights[x].spot_direction[2])*ci);
				glEnd();
				glPopAttrib();
			}
		}
	}

	if (win.wireframe && (mode==DD_Check))
	{
		if (mouseinwindow(x1,y1,x2,y2) && (!leftbutton) && (!middlebutton)) 
		{
			win.cam.eye.a[win.c3]=win.cam.eye.a[win.c3]+wheel;
			if (win.cam.eye.a[win.c3]>-1) win.cam.eye.a[win.c3]=-1;
		}
		if (midclickinwindow(x1,y1,x2,y2)) win.cam=win.cam2; else win.cam2=win.cam;
		if (midclickinwindow(x1,y1,x2,y2))
		{
			SetCapture(hWnd);
			win.cam.eye.a[win.c1]=win.cam2.eye.a[win.c1]-(mbx-mx)*xsize/(x2-x1)*win.a;
			win.cam.target.a[win.c1]=win.cam2.target.a[win.c1]-(mbx-mx)*xsize/(x2-x1)*win.a;
			win.cam.eye.a[win.c2]=win.cam2.eye.a[win.c2]-(mby-my)*ysize/(y2-y1)*win.b;
			win.cam.target.a[win.c2]=win.cam2.target.a[win.c2]-(mby-my)*ysize/(y2-y1)*win.b;
		}
		if (leftclickinwindow(x1,y1,x2,y2) && modellermenu==0 && modellsubmenu[modellermenu]==1 && !leftclickinwindow(x1,y1,x1+20+19*2,y1+19) && !leftclickinwindow(x2-19*3,y1,x2,y1+19) && !leftclickinwindow(x2-19,y1,x2,y1+19*3))
		{
			modellviews[3].cam.eye.a[win.c1]=win.cam.eye.a[win.c1]-(mx-win.xpos-(float)(x2-x1)/2.0f)*xsize/(x2-x1)*win.a;
			modellviews[3].cam.eye.a[win.c2]=win.cam.eye.a[win.c2]-(my-win.ypos-(float)(y2-y1)/2.0f)*ysize/(y2-y1)*win.b;
			modellviews[3].cam.up=origo;
			modellviews[3].cam.up.b.y=-1;
		}
		if (rightclickinwindow(x1,y1,x2,y2) && modellermenu==0 && modellsubmenu[modellermenu]==1 && !leftclickinwindow(x1,y1,x1+20+19*2,y1+19) && !leftclickinwindow(x2-19*3,y1,x2,y1+19) && !leftclickinwindow(x2-19,y1,x2,y1+19*3))
		{
			modellviews[3].cam.target.a[win.c1]=win.cam.eye.a[win.c1]-(mx-win.xpos-(float)(x2-x1)/2.0f)*xsize/(x2-x1)*win.a;
			modellviews[3].cam.target.a[win.c2]=win.cam.eye.a[win.c2]-(my-win.ypos-(float)(y2-y1)/2.0f)*ysize/(y2-y1)*win.b;
			modellviews[3].cam.up=origo;
			modellviews[3].cam.up.b.y=-1;
		}
	}
	if (!win.wireframe && (mode==DD_Check))
	{
		if (mouseinwindow(x1,y1,x2,y2) && (!leftbutton) && (!middlebutton)) 
		{
			l=egyseg(vect_kivonas(win.cam.target,win.cam.eye));
			win.cam.eye=vect_osszeadas(win.cam.eye,vect_skalarszorzas(l,wheel*win.camsensitivity));
			win.cam.target=vect_osszeadas(win.cam.target,vect_skalarszorzas(l,wheel*win.camsensitivity));
		}
		if (midclickinwindow(x1,y1,x2,y2)) win.cam=win.cam2; else win.cam2=win.cam;
		if (midclickinwindow(x1,y1,x2,y2))
		{
			float a=1;
			if (win.fullscreen) a=0.5;

			m_xpose(win.cam2.eye.b.x,win.cam2.eye.b.y,win.cam2.eye.b.z,m1);

			m_rotate(0,1,0,(float)(-(mbx-mx)*radtheta*a),m2);
			m_xformd(m2,win.cam2.up,win.cam.up);
			m_mult(m1,m2,m2);

			m_xpose(-win.cam2.eye.b.x,-win.cam2.eye.b.y,-win.cam2.eye.b.z,m1);
			m_mult(m2,m1,m2);
			m_xformd(m2,win.cam2.target,win.cam.target);

			l=cross(egyseg(vect_kivonas(win.cam.target,win.cam.eye)),egyseg(win.cam.up));
			m_xpose(win.cam2.eye.b.x,win.cam2.eye.b.y,win.cam2.eye.b.z,m1);
			m_rotate(l.b.x,l.b.y,l.b.z,(float)((mby-my)*radtheta*a),m2);
			m_xformd(m2,win.cam.up,win.cam.up);
			m_mult(m1,m2,m2);

			m_xpose(-win.cam2.eye.b.x,-win.cam2.eye.b.y,-win.cam2.eye.b.z,m1);
			m_mult(m2,m1,m2);
			m_xformd(m2,win.cam.target,win.cam.target);

		}
	}
	
	/*if ((xformmenu) && leftclickinwindow(x1,y1,x2,y2) && (mode==DD_Check))
	{
		if ((win.wireframe) && (!leftclickinwindow(x1,y1,x1+16+16+16,y1+16))) {dotransformation(win,x1,y1,x2,y2,xsize,ysize);}
	}*/

	if (mode==DD_Draw)
	{
		glEnable(GL_DEPTH_TEST);

		glColor4d(0.7,0.7,0.7,1);
		if (win.wireframe)
		{
			if (win.perspective) { scene_renderwireframe(*actualscene,4,win.c3,lofttrack,loftmesh); 
			}
			else
			{ scene_renderwireframe(*actualscene,win.c3,win.c3,lofttrack,loftmesh); }
			if (win.perspective)
			{
				glBegin(GL_LINES);
				glColor4f(1,0,0,1);
				glVertex3f(modellviews[3].cam.eye.b.x,modellviews[3].cam.eye.b.y,modellviews[3].cam.eye.b.z);
				glColor4f(1,1,0,1);
				glVertex3f(modellviews[3].cam.target.b.x,modellviews[3].cam.target.b.y,modellviews[3].cam.target.b.z);
				glEnd();
			}
			else
			{
				int aa[3];
				int x;
				for (x=0;x<=2;x++) if (x!=win.c3) {aa[x]=1;} else {aa[x]=0;}
				glBegin(GL_LINES);
				glColor4f(1,0,0,1);
				glVertex3f(modellviews[3].cam.eye.b.x*aa[0],modellviews[3].cam.eye.b.y*aa[1],modellviews[3].cam.eye.b.z*aa[2]);
				glColor4f(1,1,0,1);
				glVertex3f(modellviews[3].cam.target.b.x*aa[0],modellviews[3].cam.target.b.y*aa[1],modellviews[3].cam.target.b.z*aa[2]);
				glEnd();
			}
		}
		else
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_DEPTH_TEST);
			if (!maptransforming)
			{
				glEnable(GL_FOG);
				scene_rendersolid(*actualscene,win.showhidden);
				if (win.perspective)
				scene_rendernormals(*actualscene);
				glDisable(GL_FOG);
			}
			else
			scene_rendersolidmaptransform(*actualscene);

			glPopAttrib();
		}
	}
	
	if (win.grid && (mode==DD_Draw))
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(false);
		if (win.wireframe)
		{
			glBegin(GL_LINES);
			memset(&v1,0,sizeof(v1));
			for (xx=-((int)xsize+2);xx<=(int)xsize+2;xx++)
			{
				int y = abs(xx+1) % 2;
				v2.b.x=(float)(0.1*(y+1.0));
				v2.b.y=(float)(0.15*(y+1.0));
				v2.b.z=(float)(0.25*(y+1.0));
				v1.a[win.c1]=(float)((float)xx/(float)2.0)+(int)(cam2.eye.a[win.c1]);
				if (v1.a[win.c1]==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.b.x,v2.b.y,v2.b.z,1);
				v1.a[win.c2]=(float)(((float)-ysize/(float)2.0)+(int)(cam2.eye.a[win.c2])-10.0);
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);
				v1.a[win.c2]=(float)(((float)+ysize/(float)2.0)+(int)(cam2.eye.a[win.c2])+10.0);
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);		
			}
			memset(&v1,0,sizeof(v1));
			for (xx=-((int)(ysize+2));xx<=(int)(ysize+2);xx++)
			{
				int y = abs(xx+1) % 2;
				v2.b.x=(float)(0.1*(y+1.0));
				v2.b.y=(float)(0.15*(y+1.0));
				v2.b.z=(float)(0.25*(y+1.0));
				v1.a[win.c1]=-xsize/2+(int)(cam2.eye.a[win.c1])-10;
				v1.a[win.c2]=(float)((float)xx/(float)2.0+(int)(cam2.eye.a[win.c2]));
				if (v1.a[win.c2]==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.b.x,v2.b.y,v2.b.z,1);
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);
				v1.a[win.c1]=xsize/2+(int)(cam2.eye.a[win.c1])+10;
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);
			}
			glEnd();
		}
		else
		{
			if (actualscene->fog)
			{
				glEnable(GL_FOG);
				glFogfv(GL_FOG_MODE,&actualscene->fogmode);
				glFogfv(GL_FOG_COLOR,actualscene->fogcol);
				glFogfv(GL_FOG_START,&actualscene->fogstart);
				glFogfv(GL_FOG_END,&actualscene->fogend);
				glFogfv(GL_FOG_DENSITY,&actualscene->fogdensity);
			}
			glDepthMask(true);
			glEnable(GL_DEPTH_TEST);
			glBegin(GL_LINES);
			memset(&v1,0,sizeof(v1));
			for (xx=-80;xx<=80;xx++)
			{
				int y = abs(xx+1) % 2;
				v2.b.x=(float)(0.1*(y+1.0));
				v2.b.y=(float)(0.15*(y+1.0));
				v2.b.z=(float)(0.25*(y+1.0));
				v1.a[0]=(float)((float)xx/2.0);
				if (xx==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.b.x,v2.b.y,v2.b.z,1);
				v1.a[2]=-40;
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);
				v1.a[2]=40;
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);		
			}
			memset(&v1,0,sizeof(v1));
			for (xx=-80;xx<=80;xx++)
			{
				int y = abs(xx+1) % 2;
				v2.b.x=(float)(0.1*(y+1.0));
				v2.b.y=(float)(0.15*(y+1.0));
				v2.b.z=(float)(0.25*(y+1.0));
				v1.a[2]=(float)((float)xx/2.0);
				if (xx==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.b.x,v2.b.y,v2.b.z,1);
				v1.a[0]=-40;
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);
				v1.a[0]=40;
				glVertex3f(v1.b.x,v1.b.y,v1.b.z);		
			}
			glEnd();
			glDisable(GL_FOG);
		}
		glDepthMask(true);
	}

	if (mode==DD_Draw)
	{
		glEnable(GL_DEPTH_TEST);
		if (!win.wireframe)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_DEPTH_TEST);
			scene_rendertransparent(*actualscene,win.showhidden);
			//scene_renderselect(*actualscene);
			glPopAttrib();
		}
	}

	glEnable(GL_DEPTH_TEST);
	switchto2d();
#ifdef newtransformstuff
	if ((win.wireframe) && (!leftclickinwindow(x1,y1,x1+16+16+16,y1+16) || mode==DD_Draw)) {dotransformation2(win,x1,y1,x2,y2,xsize,ysize,mode);}
#else
	if ((win.wireframe) && mode==DD_Check && (!leftclickinwindow(x1,y1,x1+16+16+16,y1+16) || mode==DD_Draw)) {dotransformation2(win,x1,y1,x2,y2,xsize,ysize);}
#endif
	if ((button(x1+1,y1,x1+20,y1+19,cubetex, 57.0/256.0, 75.0/256.0, 76.0/256.0, 94.0/256.0, win.fullscreen,mode)== DDgui_LeftClick) && (mode!=DD_AfterCheck)) 
	
	{
		win.fullscreen=!win.fullscreen; 
		if (win.fullscreen)
		{
			if (mainmenu==2)
			win.xpos=12;
			if (mainmenu==3)
			win.xpos=148;
			win.ypos=33;
			win.xsize=640;
			win.ysize=480;
		}
		else
		{
			win.xpos=win.oxpos;
			win.ypos=win.oypos;
			win.xsize=320;
			win.ysize=240;
		}
		waitleftbutton=true; }
	if ((button(x1+20,y1,x1+20+19,y1+19,cubetex, 57.0/256.0, 0, 76.0/256.0, 18.0/256.0, win.grid,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.grid=! win.grid; waitleftbutton=true; }

#ifdef newtransformstuff
	if (win.wireframe)
#endif
	{
		if (win.wireframe)
		if ((button(x1+1+19*2,y1,x1+20+19*2,y1+19,cubetex, 57.0/256.0, 18.0/256.0, 76.0/256.0, 37.0/256.0, win.perspective,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.perspective=!win.perspective; waitleftbutton=true;}

#ifdef newtransformstuff
		if (!maptransforming || (maptransforming && maptransform==0)) if ((button(x2-19*3,y1,x2-19*2,y1+19,cubetex, 57.0/256.0, 94.0/256.0, 76.0/256.0, 113.0/256.0, win.transformation==1,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (win.transformation==1) win.transformation=0; else win.transformation=1; waitleftbutton=true;}
		if (!maptransforming || (maptransforming && maptransform==2)) if ((button(x2-19*2,y1,x2-19*1,y1+19,cubetex, 57.0/256.0, 113.0/256.0, 76.0/256.0, 132.0/256.0, win.transformation==2,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (win.transformation==2) win.transformation=0; else win.transformation=2; waitleftbutton=true;}
		if (!maptransforming || (maptransforming && maptransform==1)) if ((button(x2-19*1,y1,x2-19*0,y1+19,cubetex, 57.0/256.0, 132.0/256.0, 76.0/256.0, 151.0/256.0, win.transformation==3,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (win.transformation==3) win.transformation=0; else win.transformation=3; waitleftbutton=true;}
		if (!maptransforming) if ((button(x2-19*1,y1+19,x2-19*0,y1+38,cubetex, 57.0/256.0, 151.0/256.0, 76.0/256.0, 170.0/256.0, win.centertransform,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.centertransform=!win.centertransform; waitleftbutton=true;}
		if (!maptransforming) if ((button(x2-19*1,y1+38,x2-19*0,y1+57,cubetex, 57.0/256.0, 170.0/256.0, 76.0/256.0, 189.0/256.0, win.objspacetransform,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.objspacetransform=!win.objspacetransform; waitleftbutton=true;}
#else
		if (!maptransforming || (maptransforming && maptransform==0)) if ((button(x2-19*3,y1,x2-19*2,y1+19,cubetex, 57.0/256.0, 94.0/256.0, 76.0/256.0, 113.0/256.0, transformation==0,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (transformation==0) transformation=9; else transformation=0; waitleftbutton=true;}
		if (!maptransforming || (maptransforming && maptransform==2)) if ((button(x2-19*2,y1,x2-19*1,y1+19,cubetex, 57.0/256.0, 113.0/256.0, 76.0/256.0, 132.0/256.0, transformation==1,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (transformation==1) transformation=9; else transformation=1; waitleftbutton=true;}
		if (!maptransforming || (maptransforming && maptransform==1)) if ((button(x2-19*1,y1+19,x2-19*0,y1+38,cubetex, 114.0/256.0, 170.0/256.0, 133.0/256.0, 189.0/256.0, transformation==4,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (transformation==4) transformation=9; else transformation=4; waitleftbutton=true;}
		
		if (!maptransforming || (maptransforming && maptransform==1)) if ((button(x2-19*3,y1+19,x2-19*2,y1+38,cubetex, 114.0/256.0, 151.0/256.0, 133.0/256.0, 170.0/256.0, transformation==2,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (transformation==2) transformation=9; else transformation=2; waitleftbutton=true;}
		if (!maptransforming || (maptransforming && maptransform==1)) if ((button(x2-19*2,y1+19,x2-19*1,y1+38,cubetex, 57.0/256.0, 132.0/256.0, 76.0/256.0, 151.0/256.0, transformation==3,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (transformation==3) transformation=9; else transformation=3; waitleftbutton=true;}
		
		if (!maptransforming) if ((button(x2-19*1,y1,x2-19*0,y1+19,cubetex, 57.0/256.0, 151.0/256.0, 76.0/256.0, 170.0/256.0, centertransform,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { centertransform=!centertransform; waitleftbutton=true;}
		//if (!maptransforming) if ((button(x2-19*1,y1+38,x2-19*0,y1+57,cubetex, 57.0/256.0, 170.0/256.0, 76.0/256.0, 189.0/256.0, win.objspacetransform,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.objspacetransform=!win.objspacetransform; waitleftbutton=true;}
#endif

	}
#ifdef newtransformstuff
	else
#endif
	{
		/*if (mode==DD_Draw)
		{
			glDisable(GL_DEPTH_TEST);
			glColor4d(col2);
			quad(x1+16+17+16+16,y1+1,x1+16+17+100+16+16,y1+16);
			glEnable(GL_DEPTH_TEST);
		}*/
		if (!win.wireframe)
		{
			if ((button(x1+1+19*2,y1,x1+20+19*2,y1+19,cubetex, 57.0/256.0, 56.0/256.0, 76.0/256.0, 75.0/256.0, win.perspective,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.perspective=!win.perspective; waitleftbutton=true;}
			if ((button(x1+1+19*3,y1,x1+20+19*3,y1+19,cubetex, 57.0/256.0, 37.0/256.0, 76.0/256.0, 56.0/256.0, win.showhidden,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.showhidden=!win.showhidden; waitleftbutton=true;}

			ValueBar(x1+20+19*3,y1,0,0.02f,1.5f,0.01f,&win.camsensitivity,"",0,0,base2,mode);
		}
	}

	
	switchto2d();
	//if (win.wireframe) {dotransformation2(win,x1,y1,x2,y2,xsize,ysize,mode);}
	if (mode==DD_Draw)
	{
		glDisable(GL_DEPTH_TEST);
		glColor4f(col4);
		if (win.wireframe)
		{
			glColor4f(buttontextlit);
			line(x1+5,y2-50,x1+5,y2-5);
			line(x1+5,y2-5,x1+50,y2-5);
			glRasterPos2i(x1+8,y2-42);
			glPrint(win.xn,basesmall);
			glRasterPos2i(x1+42,y2-8);
			glPrint(win.yn,basesmall);

			glColor4f(1,1,1,1);
			glRasterPos2i(x1+16+16+4+16+9,y1+13);
		}
		else
			glRasterPos2i(x1+16+16+16+16+8+140,y1+14);
	
		if (win.wireframe)
		glPrint(win.name,base2);
		else
		{
			glPrint(win.name,base2);

			char s[256];
			memset(s,0,256);
			sprintf(s,"%d",polycounter(actualscene));

			glColor4d(buttontextlit);
			glRasterPos2i(x1+2,y2-3);
			glPrint(s,basesmall);
		}
	}
	glPopAttrib();


}

void deselectall()
{
	object *o=actualscene->objects;
	while (o!=NULL)
	{
		o->selected=false;
		o=o->next;
	}
}

int counttracks(object *obj)
{
 object *o=obj;
 int a=0;

 while (o!=NULL)
 {
   if (o->track) a++;
   o=o->next;
 }

 return a;
}

int countprim(int p)
{
 object *o=actualscene->objects;
 int n=0;

 while (o!=NULL)
 {
   if (o->primitive==p) n++;
   o=o->next;
 }
 return n+1;
}

bool checkdeletable(scene scn, object *o)
{	
	if (o!=NULL)
	{
		if (o->effect) return true;
		bool deletable=true;
		SELECTION *s=o->clones;
		while (s!=NULL)
		{
			if (!checkdeletable(scn,searchobjectbynumber(scn.objects,s->selected))) deletable=false;
			s=s->next;
		}
		s=o->effects;
		while (s!=NULL)
		{
			if (!checkdeletable(scn,searchobjectbynumber(scn.objects,s->selected))) deletable=false;
			s=s->next;
		}
		return o->selected && deletable && !o->hidden;
	}
	else
	return false;
}

void delentryfromselection(object *o, int id)
{
	if (o!=NULL)
	{
		SELECTION *s=o->clones,*s2=NULL;
		while (s!=NULL)
		{
			if (s->selected==id)
			{
				if (s2==NULL) 
				{
					o->clones=s->next;
					s2=o->clones;
				}
				else s2->next=s->next;
				if (s->next==NULL)
				{
					o->lastclone=s2;
				}
			}
			s2=s;
			s=s->next;
		}
	}
}

void delentryfromeffectselection(object *o, int id)
{
	if (o!=NULL)
	{
		SELECTION *s=o->effects,*s2=NULL;
		while (s!=NULL)
		{
			if (s->selected==id)
			{
				if (s2==NULL) 
				{
					o->effects=s->next;
					s2=o->effects;
				}
				else s2->next=s->next;
				/*if (s->next==NULL)
				{
					o->lastclone=s2;
				}*/
			}
			s2=s;
			s=s->next;
		}
	}
}


void delentryfromparentselection(object *o, int id)
{
	if (o!=NULL)
	{
		SELECTION *s=o->cloneparents,*s2=NULL;
		while (s!=NULL)
		{
			if (s->selected==id)
			{
				if (s2==NULL)
				{
					o->cloneparents=s->next;
					s2=o->cloneparents;
				}
				else s2->next=s->next;
				/*if (s->next==NULL)
				{
					o->lastclone=s2;
				}*/
			}
			s2=s;
			s=s->next;
		}
	}
}


void deleteobject(scene &scn,int objid)
{
	object *o=scn.objects;
	object *o2=NULL;
	while (o!=NULL)
	{
		if (o->objdata.number==objid)
		{
			if (checkdeletable(scn,o))
			{

				SELECTION *clonelist=o->clones;
				while (clonelist!=NULL)
				{
					deleteobject(scn,clonelist->selected);
					clonelist=clonelist->next;
				}
				o->clones=NULL;

				SELECTION *parentlist=o->cloneparents;
				while (parentlist!=NULL)
				{
					object *o2=searchobjectbynumber(scn.objects,parentlist->selected);
					delentryfromselection(o2,objid);
					delentryfromeffectselection(o2,objid);
					parentlist=parentlist->next;
				}

				clonelist=o->effects;
				while (clonelist!=NULL)
				{
					object *o2=searchobjectbynumber(scn.objects,clonelist->selected);
					delentryfromparentselection(o2,objid);
					if (o2 && o2->cloneparents==NULL) deleteobject(scn,o2->objdata.number);
					clonelist=clonelist->next;
				}

				if (o2!=NULL) o2->next=o->next;
				else
				{
					scn.objects=o->next;
					o2=scn.objects;
				}
				if (o->next==NULL) scn.lastobject=o2;
			}
		}

		o2=o;
		o=o->next;
	}
}

void delete_selected()
{
	SELECTION *s2=createselection(*actualscene);
	while (s2!=NULL)
	{
		deleteobject(*actualscene,s2->selected);
		s2=s2->next;
	}
}

void drawcreatemenu(unsigned char mode)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	o=actualscene->objects;
	objnumber=objnum(o);

	if (mode==DD_Check)
	{
		if (mouseinwindow(31,520,141,590)) selectstart-=wheel*2;
		if (leftclickinwindow(31,520,141,590) && mouseinwindow(31,520,141,594) && (objnumber!=0))
		{
			for (x=1;x<=min((int)(selectstart+(my-521) / 10),objnumber);x++) o=o->next;
			if (o!=NULL)
			{
				if (!o->effect)
					o->selected=!o->selected;
			}
			waitleftbutton=true;
		}

		if (rightclickinwindow(31,520,141,590) && mouseinwindow(31,520,141,594) && (objnumber!=0))
		{
			for (x=1;x<=min((int)(selectstart+(ry-521) / 10),objnumber);x++) o=o->next;
			if (o!=NULL)
			{
				if (!o->effect)
				{
					deselectall();
					o->selected=!o->selected;
				}
			}
			waitleftbutton=true;
		}
	}


	if (mode!=DD_AfterCheck)
	{
		if (button(11,518,30,537,cubetex,171.0/256.0,152.0/256.0,190.0/256.0,171.0/256.0,false,mode) == DDgui_LeftClick) { o=actualscene->objects; while (o!=NULL)  { if (!o->effect) o->selected=true; o=o->next; } }
		if (button(11,537,30,556,cubetex,171.0/256.0,171.0/256.0,190.0/256.0,190.0/256.0,false,mode) == DDgui_LeftClick) deselectall();
		if (button(11,556,30,575,cubetex,171.0/256.0,190.0/256.0,190.0/256.0,209.0/256.0,false,mode) == DDgui_LeftClick) { o=actualscene->objects; while (o!=NULL)  { if (!o->effect) o->selected=!o->selected; o=o->next; } waitleftbutton=true; }
		
		if (button(11,575,30,594,cubetex,171.0/256.0,209.0/256.0,190.0/256.0,228.0/256.0,clonetransform,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) { clonetransform=!clonetransform; waitleftbutton=true; }

		//rectangle(201,520,311,594);
		//if (button(182,520,200,538,"S",5,13,base2,false,mode) == DDgui_LeftClick) 
		if (button(181,518,200,537,cubetex,0,152.0/256.0,19.0/256.0,171.0/256.0,false,mode) == DDgui_LeftClick) 
		{
			char *ss=readline("Enter Selection Name",260,0,base2,"");
			if (ss[0]!=13)
			{
				selectionlist *buffer = new selectionlist[1];
				memset(buffer,0,sizeof(selectionlist));
				buffer->selection=createselection(*actualscene);
				buffer->next=NULL;
				if (actualscene->lastselection!=NULL)
				{
					actualscene->lastselection->next=buffer;
					actualscene->lastselection=buffer;
				}
				else
				{
					actualscene->lastselection=buffer;
					actualscene->selections=buffer;
				}
				buffer->name=new char[80];
				memset(buffer->name,0,80);			
				sprintf(buffer->name,"%s",ss);
			}
		}
		
		if (button(181,537,200,556,cubetex,57.0/256.0,190.0/256.0,76.0/256.0,209.0/256.0,false,mode) == DDgui_LeftClick) 
		{
			if (actualscene->selections!=NULL)
			{
				sel2 = actualscene->selections;
				for (x=1;x<=selselected;x++) sel2=sel2->next;
				SELECTION *buff = sel2->selection;
				while (buff!=NULL)
				{
					searchobjectbynumber(actualscene->objects,buff->selected)->selected=true;
					buff=buff->next;
				}
			}
		}
		if (button(181,556,200,575,cubetex,114.0/256.0,190.0/256.0,133.0/256.0,209.0/256.0,false,mode) == DDgui_LeftClick) 
		{
			if (actualscene->selections!=NULL)
			{
				sel2 = actualscene->selections;
				for (x=1;x<=selselected-1;x++) sel2=sel2->next;
				if (selnum(actualscene->selections)==1) {actualscene->selections=NULL; actualscene->lastselection=NULL;}
				else
					if (selselected==0) {actualscene->selections=actualscene->selections->next;}
					else
					{
						sel2->next=sel2->next->next;
						selselected--;
					}
					waitleftbutton=true;
			}
		}

		//if (button(340,520,405,540," COPY",6,13,base2,false,mode) == DDgui_LeftClick) 
		if (button(340,520-4,397,536-4,texbutton2,0,0,57.0/256.0,16.0/256.0,false,mode) == DDgui_LeftClick) 
		{
			object *o=actualscene->objects;
			int a = objnum(actualscene->objects);
			int b,c;

			for (b=1;b<=a;b++)
			{
				if (o->selected)
				{
					scene_addobject(actualscene);
					c=actualscene->lastobject->objdata.number;
					actualscene->lastobject->objdata=o->objdata;
					actualscene->lastobject->objdata.number=c;
					memcpy(actualscene->lastobject->params,o->params,sizeof(int)*6);
					actualscene->lastobject->primitive=o->primitive;
					obj_createprimitive(actualscene->lastobject,o->primitive,o->params[1],o->params[2],o->params[3],o->params[4],o->params[5]);
					obj_transform(actualscene->lastobject,o->xformmatrix);
					sprintf(st,"%s%d",actualscene->lastobject->name,countprim(o->primitive)-1);
					strcpy(actualscene->lastobject->name,st);
					actualscene->lastobject->selected=true;
					o->selected=false;

					int xa;
					for (xa=0;xa<o->polynum;xa++)
						actualscene->lastobject->polygonlist[xa].materialhandle=o->polygonlist[xa].materialhandle;
				}
				o=o->next;
			}
			waitleftbutton=true;
		}
		if (button(340,536-4,397,552-4,texbutton2,0,16.0/256.0,57.0/256.0,32.0/256.0,false,mode) == DDgui_LeftClick) 
		{
			SELECTION *selec=createselection(*actualscene);
			if (selec!=NULL)
			{
				matrix m;
				m_identity(m);
				scene_addobject(actualscene);
				
				obj_createprimitive(actualscene->lastobject,dd3d_clone,(int)selec,(int)(actualscene->objects),0,0,0);
				sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_clone)-1);
				memcpy(actualscene->lastobject->name,st,256);

				deselectall();
				actualscene->lastobject->selected=true;
				obj_transform(actualscene->lastobject,m);
			}
			waitleftbutton=true;
			//transform=false;
		}
		if (button(340,552-4,397,568-4,texbutton2,0,32.0/256.0,57.0/256.0,48.0/256.0,false,mode) == DDgui_LeftClick) delete_selected();
		if (button(340,568-4,397,584-4,texbutton3,0,138.0/256.0,57.0/256.0,154.0/256.0,false,mode) == DDgui_LeftClick) { o=actualscene->objects; while (o!=NULL)  { if (o->selected) o->hidden=true; o=o->next; } }
		if (button(340,584-4,397,600-4,texbutton3,0,154.0/256.0,57.0/256.0,170.0/256.0,false,mode) == DDgui_LeftClick) { o=actualscene->objects; while (o!=NULL)  { if (o->selected) o->hidden=false; o=o->next; } }
			}

	if (mode==DD_Draw)
	{
		glColor4f(buttontextlit);
		rectangle( 31,520,141,594);
		rectangle(201,520,311,594);
	}	  			

	if ((mode==DD_Check) && leftclickinwindow(201,520,311,589) && mouseinwindow(201,520,311,589)) selselected=min(selbarpos+(my-520) / 10,selnum(actualscene->selections)-1);
	if ((mode ==DD_Check) && mouseinwindow(201,520,311,589)) selbarpos-=wheel*2;
	scroller(311,520,326,594,15,15,selnum(actualscene->selections),7,selbarpos,mode);
	if (mode==DD_Draw)
	{
		sel2 = actualscene->selections;
		for (x=1;x<=selbarpos;x++) sel2=sel2->next;
		for (x=0;x<=6;x++)
		{
			if (sel2!=NULL)
			{
				if (selbarpos+x==selselected) glColor4f(col4); else glColor4f(buttontextlit);
				glRasterPos2i(205,530+x*10);
				glPrint(sel2->name,base2,12);
				sel2=sel2->next;
			}
		}
	}

	o=actualscene->objects;
	scroller(141,520,156,594,15,15,objnumber,7,selectstart,mode);
	for (x=1;x<=selectstart;x++) o=o->next;
	if (mode==DD_Draw)
	{
		for (x=0;x<=6;x++)
		{
			if (o!=NULL)
			{
				if (o->selected) glColor4f(col4); else glColor4f(buttontextlit);
				if (o->hidden) glColor4f(0,0.6f,0,1); if ((o->hidden) && (o->selected)) glColor4f(0.2f,1,0.2f,1);
				if (o->effect) glColor4f(1,1,0,1);
				glRasterPos2i(32,530+x*10);
				sprintf(s,"%4d:%s",o->objdata.number,o->name);
				glPrint(s,base2);
				o=o->next;		
			}
		}
	}

	glPopAttrib();

}

int materialnum(material *t)
{
	int x=0;
	material *tt=t;
	while (tt!=NULL)
	{
		x++;
		tt=tt->next;
	}
	return x;
}

int cntscenes()
{
	int x=0;
	scene *s=scenelist;
	while (s!=NULL)
	{
		x++;
		s=s->next;
	}
	return x;
}

void ModellerGUI(unsigned char mode)
{
	actualscene->editview=modellviews[3].cam;
	if (mode==DD_Check)
	{
		transform=false;
		if ((xformmenu) && (leftclickinwindow(12,33,652,513))) transform=true;
		if (transform)
		{
			o=actualscene->objects;
			while (o!=NULL)
			{
				memcpy(o->xformmatrix,o->buffermatrix,sizeof(matrix));
				o->bugfixed=false;
				o=o->next;
			}
			if (maptransforming)
			memcpy(mapmatrix,mapbufmatrix,sizeof(matrix));
		}
		else
		{
			o=actualscene->objects;
			while (o!=NULL)
			{
				memcpy(o->buffermatrix,o->xformmatrix,sizeof(matrix));
				o->bugfixed=false;
				o=o->next;
			}
			if (maptransforming)
			memcpy(mapbufmatrix,mapmatrix,sizeof(matrix));
		}
	}

	if (!maptransforming)
	{
		if (button(658,31,677,50,cubetex,0,0,19.0/256.0,19.0/256.0,modellermenu==0,mode) == DDgui_LeftClick) modellermenu=0;
		if (button(658,51,677,70,cubetex,0,19.0/256.0,19.0/256.0,38.0/256.0,modellermenu==1,mode) == DDgui_LeftClick) modellermenu=1;
		if (button(678,71,697,90,cubetex,0,38.0/256.0,19.0/256.0,57.0/256.0,modellermenu==2,mode) == DDgui_LeftClick) modellermenu=2;
		if (button(658,71,677,90,cubetex,0,57.0/256.0,19.0/256.0,76.0/256.0,modellermenu==3,mode) == DDgui_LeftClick) { modellermenu=3; modellviews[0].transformation=0; modellviews[1].transformation=0; modellviews[2].transformation=0; modellviews[3].transformation=0;}
		
		if (button(658,91,677,110,cubetex,0,76.0/256.0,19.0/256.0,95.0/256.0,modellermenu==5,mode) == DDgui_LeftClick) modellermenu=5;
		if (button(678,31,697,50,cubetex,0,95.0/256.0,19.0/256.0,114.0/256.0,modellermenu==6,mode) == DDgui_LeftClick) modellermenu=6;
		if (button(678,91,697,110,cubetex,0,114.0/256.0,19.0/256.0,133.0/256.0,modellermenu==7,mode) == DDgui_LeftClick) modellermenu=7;

		if (button(678,51,697,70,cubetex,0,133.0/256.0,19.0/256.0,152.0/256.0,modellermenu==4,mode) == DDgui_LeftClick) modellermenu=4;

	}

	if (!modellviews[0].fullscreen && !modellviews[1].fullscreen && !modellviews[2].fullscreen && !modellviews[3].fullscreen)
	{
		int x;
		for (x=0;x<=3;x++) displaywindow(modellviews[x],mode);
	}
	else
	{
		if (modellviews[0].fullscreen) displaywindow(modellviews[0],mode); else
		if (modellviews[1].fullscreen) displaywindow(modellviews[1],mode); else
		if (modellviews[2].fullscreen) displaywindow(modellviews[2],mode); else
		if (modellviews[3].fullscreen) displaywindow(modellviews[3],mode);
	}
	switchto2d();
	if (createmenu) drawcreatemenu(mode);

	switch (modellermenu)
	{	
	case 0: {
		//if (button(706,31,791,50,"Environm.",2,12,base2,modellsubmenu[modellermenu]==0,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=0;
		if (button(706,31,791,47,texbutton1,0,64.0/256.0,85.0/256.0,80.0/256.0,modellsubmenu[modellermenu]==1,mode) == DDgui_LeftClick) { modellsubmenu[modellermenu]=1; modellviews[0].transformation=0; modellviews[1].transformation=0; modellviews[2].transformation=0; modellviews[3].transformation=0; }
		if (button(706,47,791,63,texbutton1,0,80.0/256.0,85.0/256.0,96.0/256.0,modellsubmenu[modellermenu]==0,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=0;
		xformmenu=true;
		switch (modellsubmenu[modellermenu])
		{
		case 0: //environment menu
			{
				if ((button(686,114,771,130,texbutton1,0,128.0/256.0,85.0/256.0,144.0/256.0,actualscene->fog,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { actualscene->fog=!actualscene->fog; waitleftbutton=true;};
				
				if (mode==DD_Draw)
				{
					glColor4f(buttontextlit);
					glRasterPos2i(704,153);
					glPrint("COLOR:",base2);
					glRasterPos2i(666,236);
					glPrint("SETTINGS:",base2);
				}
				sprintf(s2,"%d",(int)(actualscene->fogcol[0]*255.0));
				ValueBar(661,160,1,0.0f,1.0f,1.0f/256.0f,&(actualscene->fogcol[0]),s2,52,13, base2, mode);
				sprintf(s2,"%d",(int)(actualscene->fogcol[1]*255.0));
				ValueBar(661,179,2,0.0f,1.0f,1.0f/256.0f,&(actualscene->fogcol[1]),s2,52,13, base2, mode);
				sprintf(s2,"%d",(int)(actualscene->fogcol[2]*255.0));
				ValueBar(661,198,3,0.0f,1.0f,1.0f/256.0f,&(actualscene->fogcol[2]),s2,52,13, base2, mode);
				
				sprintf(s2,"DENSITY: %.3f",actualscene->fogdensity);
				ValueBar(661,242,0,0.0f,0.7f,0.001f,&(actualscene->fogdensity),s2,25,13, base2, mode);
				sprintf(s2,"START: %d",(int)actualscene->fogstart);
				ValueBar(661,261,0,0.0f,255.0f,1.0f/256.0f,&(actualscene->fogstart),s2,34,13, base2, mode);
				sprintf(s2,"END: %d",(int)actualscene->fogend);
				ValueBar(661,280,0,0.0f,255.0f,1.0f/256.0f,&(actualscene->fogend),s2,42,13, base2, mode);

				break;
			}
		case 1:
			{
				ValueBar(661,160,0,0.0f,255.0f,1.0f,&modellviews[3].cam.roll,"ROLL",52,13,base2,mode);
				ValueBar(661,180,0,1.0f,179.0f,1.0f,&modellviews[3].cam.fov,"FOV",54,13,base2,mode);
				break;
			}
		}
		break;
			}
	case 1: { //object menu
		xformmenu=true;

		matrix m;
		object *o1,*o2;

		if ((button(698-20,126,717-20,145,cubetex,114.0/256.0,0,133.0/256.0,19.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
			
		{
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_box,0,0,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_box)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			m_identity(m);
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}

		if ((button(717,126,736,145,cubetex,114.0/256.0,76.0/256.0,133.0/256.0,95.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_icosaeder,0,0,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_icosaeder)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			m_identity(m);
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}
		
		if ((button(736+20,126,755+20,145,cubetex,114.0/256.0,95.0/256.0,133.0/256.0,114.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)  
		{
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_dodecaeder,0,0,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_dodecaeder)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			m_identity(m);
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}

		/*if ((button(741,31,760,50,cubetex,114.0/256.0,19.0/256.0,133.0/256.0,38.0/256.0,objmenu==1,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=1;
		if ((button(761,31,780,50,cubetex,57.0/256.0,0.0/256.0,76.0/256.0,19.0/256.0,objmenu==2,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=2;
		if ((button(741,51,760,70,cubetex,114.0/256.0,38.0/256.0,133.0/256.0,57.0/256.0,objmenu==3,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=3;

		if ((button(721,51,740,70,cubetex,114.0/256.0,57.0/256.0,133.0/256.0,76.0/256.0,objmenu==4,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=4;
		if ((button(721,71,740,90,cubetex,114.0/256.0,133.0/256.0,133.0/256.0,152.0/256.0,objmenu==5,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=5;
		if ((button(741,71,760,90,cubetex,114.0/256.0,114.0/256.0,133.0/256.0,133.0/256.0,objmenu==6,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=6;

		if ((button(722,91,779,107,mainmenutex,153.0/512.0,92.0/128.0,210.0/512.0,107.0/128.0,objmenu==7,mode) == DDgui_LeftClick) && mode==DD_Check) objmenu=7;*/

		if ((button(717,120+43,736,139+43,cubetex,114.0/256.0,19.0/256.0,133.0/256.0,38.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			m_identity(m);
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_sphere,spherex,spherey,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_sphere)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}

		sprintf(st,"SPHERE-X: %d",spherex);
		ValueBar(662,140+43,0,3,255,&spherex,st,21,14,base2,mode);
		sprintf(st,"SPHERE-Y: %d",spherey);
		ValueBar(662,160+43,0,3,255,&spherey,st,21,14,base2,mode);

		if ((button(697,185+43,716,204+43,cubetex,114.0/256.0,57.0/256.0,133.0/256.0,76.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			m_identity(m);
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_hasab,tubex,tubey,(int)tubetop,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_hasab)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}					
		if ((button(737,185+43,756,204+43,cubetex,171.0/256.0,228.0/256.0,190.0/256.0,247.0/256.0,tubetop,mode) && mode!=DD_AfterCheck)) {tubetop=!tubetop; waitleftbutton=true;}
		sprintf(st,"TUBE-X: %d",tubex);
		ValueBar(662,205+43,0,3,255,&tubex,st,21,14,base2,mode);
		sprintf(st,"TUBE-Y: %d",tubey);
		ValueBar(662,225+43,0,1,255,&tubey,st,21,14,base2,mode);

		if ((button(697,250+43,716,269+43,cubetex,114.0/256.0,38.0/256.0,133.0/256.0,57.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			m_identity(m);
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_cone,conex,(int)conetop,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_cone)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}
		if ((button(737,250+43,756,269+43,cubetex,171.0/256.0,228.0/256.0,190.0/256.0,247.0/256.0,conetop,mode) && mode!=DD_AfterCheck)) {conetop=!conetop; waitleftbutton=true;}
		sprintf(st,"CONE-X: %d",conex);
		ValueBar(662,270+43,0,3,255,&conex,st,21,14,base2,mode);

		if ((button(717,295+43,736,314+43,cubetex,57.0/256.0,0.0/256.0,76.0/256.0,19.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			m_identity(m);
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_grid,gridx,gridy,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_grid)-1);
			memcpy(actualscene->lastobject->name,st,256);
			actualscene->lastobject->selected=true;
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}
		sprintf(st,"GRID-X: %d",gridx);
		ValueBar(662,315+43,0,1,255,&gridx,st,21,14,base2,mode);
		sprintf(st,"GRID-Y: %d",gridy);
		ValueBar(662,335+43,0,1,255,&gridy,st,21,14,base2,mode);

		if ((button(717,365+43,736,384+43,cubetex,114.0/256.0,133.0/256.0,133.0/256.0,152.0/256.0,false,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			m_identity(m);
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_arc,arcx,arcy,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_arc)-1);
			strcpy(actualscene->lastobject->name,st);
			actualscene->lastobject->selected=true;
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}
		sprintf(st,"ARC-X: %d",arcx);
		ValueBar(662,385+43,0,1,255,&arcx,st,21,14,base2,mode);
		sprintf(st,"ARC-Y: %d",arcy);
		ValueBar(662,405+43,0,0,360,&arcy,st,21,14,base2,mode);

		if ((button(717,430+43,736,449+43,cubetex,114.0/256.0,114.0/256.0,133.0/256.0,133.0/256.0,objmenu==6,mode) == DDgui_LeftClick) && mode==DD_Check)
		{
			m_identity(m);
			deselectall();
			scene_addobject(actualscene);
			obj_createprimitive(actualscene->lastobject,dd3d_line,linex,0,0,0,0);
			sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_line)-1);
			strcpy(actualscene->lastobject->name,st);
			actualscene->lastobject->selected=true;
			obj_transform(actualscene->lastobject,m);
			waitleftbutton=true;
		}
		sprintf(st,"LINE-X: %d",linex);
		ValueBar(662,450+43,0,1,255,&linex,st,21,14,base2,mode);


		glColor4f(buttontextlit);
		rectangle(661,533,776,593);
		tracknumber=counttracks(actualscene->objects);
		scroller(776,533,791,593,15,15,tracknumber,6,loftstart,mode);
		if (leftclickinwindow(661,533,776,592) && mouseinwindow(661,533,776,592) && (loftstart+(my-533) / 10<tracknumber)) loftmesh=loftstart+(my-533) / 10;

		o=actualscene->objects;
		loftcounter=0;
		while ((o!=NULL) && (!o->track)) o=o->next;
		while ((o!=NULL) && (loftcounter!=loftstart))
		{
			if (o->track)  loftcounter++;
			o=o->next;
		}
		while ((o!=NULL) && (!o->track)) o=o->next;
		if (mode==DD_Draw)
		{
			for (x=0;x<=5;x++)
			{
				if ((o!=NULL) && o->track)
				{
					sprintf(st,"%3d:%s",o->objdata.number,o->name);
					glColor4f(buttontextlit);
					if (loftcounter==loftmesh) glColor4f(1,1,1,1);
					glRasterPos2i(663,541+10*x);
					glPrint(st,base2);
					o=o->next;
					loftcounter++;
				}
				while ((o!=NULL) && (!o->track)) o=o->next;
			}
		}


		rectangle(526,533,641,593);
		scroller(641,533,656,593,15,15,tracknumber,6,trackstart,mode);
		if (leftclickinwindow(526,532,641,592) && mouseinwindow(526,533,641,592) && (trackstart+(my-533) / 10<tracknumber)) lofttrack=trackstart+(my-533) / 10;

		o=actualscene->objects;
		loftcounter=0;
		while ((o!=NULL) && (!o->track)) o=o->next;
		while ((o!=NULL) && (loftcounter!=trackstart))
		{
			if (o->track)  loftcounter++;
			o=o->next;
		}
		while ((o!=NULL) && (!o->track)) o=o->next;
		if (mode==DD_Draw)
		{
			for (x=0;x<=5;x++)
			{
				if ((o!=NULL) && o->track)
				{
					sprintf(st,"%3d:%s",o->objdata.number,o->name);
					glColor4f(buttontextlit);
					if (loftcounter==lofttrack) glColor4f(1,1,1,1);
					glRasterPos2i(528,541+10*x);
					glPrint(st,base2);
					o=o->next;
					loftcounter++;
				}
				while ((o!=NULL) && (!o->track)) o=o->next;
			}
		}

		glColor4f(1,0.5,0.5,1);
		glRasterPos2i(526,530);
		glPrint("TRACK",base2);

		glColor4f(0.5,1,0.5,1);
		glRasterPos2i(758,530);
		glPrint("MESH",base2);
		glColor4f(1,1,1,1);

		if (tracknumber>=1)
		{						
			if ((button(630,516,687,532,mainmenutex,153.0/512.0,92.0/128.0,210.0/512.0,108.0/128.0,false,mode) == DDgui_LeftClick) && (mode==DD_Check))
			{
				waitleftbutton=true;
				o1=actualscene->objects;
				int xx=0;
				while ((o1!=NULL) && (!o1->track)) o1=o1->next;
				while ((o1!=NULL) && (xx!=lofttrack))
				{
					if (o1->track)  xx++;
					o1=o1->next;
				}
				while ((o1!=NULL) && (!o1->track)) o1=o1->next;

				o2=actualscene->objects;
				xx=0;
				while ((o2!=NULL) && (!o2->track)) o2=o2->next;
				while ((o2!=NULL) && (xx!=loftmesh))
				{
					if (o2->track)  xx++;
					o2=o2->next;
				}
				while ((o2!=NULL) && (!o2->track)) o2=o2->next;
				m_identity(m);
				deselectall();
				scene_addobject(actualscene);
				obj_createprimitive(actualscene->lastobject,dd3d_loft,o1->objdata.number,o2->objdata.number,(int)(actualscene->objects),0,0);
				sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_loft)-1);
				memcpy(actualscene->lastobject->name,st,256);

				actualscene->lastobject->selected=true;
				obj_transform(actualscene->lastobject,m);
			}
		}

		/*switch (objmenu)
		{
			case 7: //loft
				{
					tracknumber=counttracks(actualscene->objects);
					scroller(495,520,510,583,15,15,tracknumber,5,loftstart,mode);
					glColor4f(col);
					rectangle(395,520,494,583);

					if (leftclickinwindow(395,520,494,579) && (loftstart+(ly-520) / 12<tracknumber)) lofttrack=loftstart+(ly-520) / 12;
					if (rightclickinwindow(395,520,494,579) && (loftstart+(ly-520) / 12<tracknumber)) loftmesh=loftstart+(ry-520) / 12;
					o=actualscene->objects;
					loftcounter=0;
					while ((o!=NULL) && (!o->track)) o=o->next;
					while ((o!=NULL) && (loftcounter!=loftstart))
					{
						if (o->track)  loftcounter++;
						o=o->next;
					}
					while ((o!=NULL) && (!o->track)) o=o->next;
					for (x=0;x<=4;x++)
					{
						if ((o!=NULL) && o->track)
						{
							sprintf(st,"%3d:%s",o->objdata.number,o->name);
							glColor4f(col);
							if (loftcounter==lofttrack) glColor4f(1,0.5,0.5,1);
							if (loftcounter==loftmesh) glColor4f(0.5,1,0.5,1);
							if ((loftcounter==lofttrack) && (lofttrack==loftmesh)) glColor4f(1,1,1,1);
							glRasterPos2i(400,531+12*x);
							glPrint(st,base2);
							o=o->next;
							loftcounter++;
						}
						while ((o!=NULL) && (!o->track)) o=o->next;
					}

					glColor4f(1,0.5,0.5,1);
					glRasterPos2i(187+139+1,536+11);
					glPrint("L:Track",base2);
					glColor4f(0.5,1,0.5,1);
					glRasterPos2i(187+139+6,536+11+15);
					glPrint("R:Mesh",base2);
					glColor4f(1,1,1,1);
					glRasterPos2i(187+139+15,536+11+30);
					glPrint("Both",base2);

					if (tracknumber>=1)
					{
						if ((button(187+139,520,391,535,"Loft",16,11,base2,false,mode) == DDgui_LeftClick) && (mode==DD_Check))
						{
							waitleftbutton=true;
							o1=actualscene->objects;
							int xx=0;
							while ((o1!=NULL) && (!o1->track)) o1=o1->next;
							while ((o1!=NULL) && (xx!=lofttrack))
							{
								if (o1->track)  xx++;
								o1=o1->next;
							}
							while ((o1!=NULL) && (!o1->track)) o1=o1->next;

							o2=actualscene->objects;
							xx=0;
							while ((o2!=NULL) && (!o2->track)) o2=o2->next;
							while ((o2!=NULL) && (xx!=loftmesh))
							{
								if (o2->track)  xx++;
								o2=o2->next;
							}
							while ((o2!=NULL) && (!o2->track)) o2=o2->next;
							m_identity(m);
							deselectall();
							scene_addobject(actualscene);
							obj_createprimitive(actualscene->lastobject,dd3d_loft,o1->objdata.number,o2->objdata.number,(int)(actualscene->objects),0,0);
							sprintf(st,"%s%d",actualscene->lastobject->name,countprim(dd3d_loft)-1);
							memcpy(actualscene->lastobject->name,st,256);

							actualscene->lastobject->selected=true;
							obj_transform(actualscene->lastobject,m);
						}
					}
					break;
				}
		}*/

		
		break;
			} // object menu switch end
	case 2: { // tracks menu
		xformmenu=true;
		break;
			} // tracks menu switch end
	case 3: { //lights menu
		xformmenu=false;
		for (x=0;x<4;x++)
		{
			if (button(714+x*17,31,714+(x+1)*17,48,cubetex,171.0/256.0,(double)x*17/256.0,188.0/256.0,(double)(x+1)*17/256.0,x==lightselected,mode) == DDgui_LeftClick) lightselected=x;
		}
		for (x=4;x<8;x++)
		{
			if (button(714+(x-4)*17,31+17,714+(x-3)*17,48+17,cubetex,171.0/256.0,(double)x*17/256.0,188.0/256.0,(double)(x+1)*17/256.0,x==lightselected,mode) == DDgui_LeftClick) lightselected=x;
		}

		if ((button(706,67,791,83,texbutton1,0,112.0/256.0,85.0/256.0,128.0/256.0,actualscene->lights[lightselected].turnedon,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { actualscene->lights[lightselected].turnedon=!actualscene->lights[lightselected].turnedon; waitleftbutton=true;};
		
		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			glRasterPos2i(704,183);
			glPrint("COLOR:",base2);
			glRasterPos2i(666,266);
			glPrint("AMBIENT COLOR:",base2);
		}
		sprintf(s2,"%d",(int)(actualscene->lights[lightselected].color[0]*255.0));
		ValueBar(661,190,1,0.0f,1.0f,1.0f/256.0f,&(actualscene->lights[lightselected].color[0]),s2,60,13, base2, mode);
		sprintf(s2,"%d",(int)(actualscene->lights[lightselected].color[1]*255.0));
		ValueBar(661,209,2,0.0f,1.0f,1.0f/256.0f,&(actualscene->lights[lightselected].color[1]),s2,60,13, base2, mode);
		sprintf(s2,"%d",(int)(actualscene->lights[lightselected].color[2]*255.0));
		ValueBar(661,228,3,0.0f,1.0f,1.0f/256.0f,&(actualscene->lights[lightselected].color[2]),s2,60,13, base2, mode);
		
		sprintf(s2,"%d",(int)(actualscene->lights[lightselected].ambient[0]*255.0));
		ValueBar(661,272,1,0.0f,1.0f,1.0f/256.0f,&(actualscene->lights[lightselected].ambient[0]),s2,60,13, base2, mode);
		sprintf(s2,"%d",(int)(actualscene->lights[lightselected].ambient[1]*255.0));
		ValueBar(661,291,2,0.0f,1.0f,1.0f/256.0f,&(actualscene->lights[lightselected].ambient[1]),s2,60,13, base2, mode);
		sprintf(s2,"%d",(int)(actualscene->lights[lightselected].ambient[2]*255.0));
		ValueBar(661,310,3,0.0f,1.0f,1.0f/256.0f,&(actualscene->lights[lightselected].ambient[2]),s2,60,13, base2, mode);

		if (button(697,120,754,136,texbutton2,0,96.0/256.0,57.0/256.0,112.0/256.0,actualscene->lights[lightselected].position[3]==0,mode) ==  DDgui_LeftClick) {actualscene->lights[lightselected].position[3]=0; actualscene->lights[lightselected].spot_cutoff=180;}
		if (button(697,136,754,152,texbutton2,0,64.0/256.0,57.0/256.0,80.0/256.0,actualscene->lights[lightselected].position[3]!=0 && actualscene->lights[lightselected].spot_cutoff==180,mode) ==  DDgui_LeftClick) {actualscene->lights[lightselected].position[3]=1; actualscene->lights[lightselected].spot_cutoff=180;}
		if (button(697,152,754,168,texbutton2,0,80.0/256.0,57.0/256.0,96.0/256.0,actualscene->lights[lightselected].spot_cutoff!=180,mode) ==  DDgui_LeftClick) {actualscene->lights[lightselected].position[3]=1; actualscene->lights[lightselected].spot_cutoff=90;}

		if (actualscene->lights[lightselected].position[3]!=0)
		{ //dotlight
			if (mode==DD_Draw)
			{
				glColor4f(buttontextlit);
				glRasterPos2i(676,374);
				glPrint("ATTENUATION:",base2);			         
			}

			ValueBar(661,380,0,0.0f,1.0f,1.0/256.0,&(actualscene->lights[lightselected].c_att),"CONSTANT",31,13,base2,mode);
			ValueBar(661,400,0,0.0f,1.0f,1.0/256.0,&(actualscene->lights[lightselected].l_att),"LINEAR",39,13,base2,mode);
			ValueBar(661,420,0,0.0f,1.0f,1.0/256.0,&(actualscene->lights[lightselected].q_att),"QUADRATIC",26,13,base2,mode);

			if (actualscene->lights[lightselected].spot_cutoff!=180)
			{
				ValueBar(661,475,0,0.0f,90.0f,0.9f,&(actualscene->lights[lightselected].spot_cutoff),"CUTOFF",39,13,base2,mode);
				ValueBar(661,496,0,0.0f,10.0f,0.1f,&(actualscene->lights[lightselected].spot_exponent),"EXPONENT",31,13,base2,mode);
			}

		}

		break;
			} //lights menu switch end
	case 4: { //material menu
			xformmenu=false;
			bool noselected=false;
			//if (mode==DD_Check)
			{
				obj=actualscene->objects;
				while (!((obj==NULL) || (obj->selected)))
				{
					obj=obj->next;
				}
				if (obj!=NULL)
				{
					colr=obj->objdata.color[0];
					colg=obj->objdata.color[1];
					colb=obj->objdata.color[2];
					colx=obj->objdata.color[3];
					mapxr=obj->objdata.texx;
					mapyr=obj->objdata.texy;
					mapswap=obj->objdata.swaptexturexy;
					mapxswap=obj->objdata.inverttexx;
					mapyswap=obj->objdata.inverttexy;
					mapxoff=obj->objdata.texox;
					mapyoff=obj->objdata.texoy;
				}
				else noselected=true;
			}

			/*ValueBar(661,200,790,220,0.0f,255.0f,&colr,"RED",52,13,base2,mode);
			ValueBar(661,221,790,241,0.0f,255.0f,&colg,"GREEN",44,13,base2,mode);
			ValueBar(661,242,790,262,0.0f,255.0f,&colb,"BLUE",48,13,base2,mode);
			ValueBar(661,263,790,283,0.0f,255.0f,&colx,"TRANSPARENCY",12,13,base2,mode);*/
			
			sprintf(s2,"%d",(int)colr);
			ValueBar(661,340,1,0.0f,255.0f,1.0f,&colr,s2,52,13,base2,mode);
			sprintf(s2,"%d",(int)colg);
			ValueBar(661,359,2,0.0f,255.0f,1.0f,&colg,s2,52,13,base2,mode);
			sprintf(s2,"%d",(int)colb);
			ValueBar(661,378,3,0.0f,255.0f,1.0f,&colb,s2,52,13,base2,mode);
			sprintf(s2,"%d",(int)colx);
			ValueBar(661,397,0,0.0f,255.0f,1.0f,&colx,s2,52,13,base2,mode);

			sprintf(s2,"X SCALE: %d",mapyr);
			ValueBar(661,470,0,0,255,&mapyr,s2,28,13,base2,mode);
			sprintf(s2,"Y SCALE: %d",mapxr);
			ValueBar(661,490,0,0,255,&mapxr,s2,28,13,base2,mode);

			if (mode==DD_Draw)
			{
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glColor4f(1,1,1,1);
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,mattexture);
				glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2i(662,121);
				glTexCoord2f(0,1);
				glVertex2i(790,121);
				glTexCoord2f(1,1);
				glVertex2i(790,249);
				glTexCoord2f(1,0);
				glVertex2i(662,249);
				glEnd();
				glDisable(GL_TEXTURE_2D);
				glColor4f(buttontextlit);
				rectangle(661,120,790,249);				
				rectangle(661,257,774,330);
				glDisable(GL_DEPTH_TEST);
				glColor4f(buttontextlit);
				if (mapxoff!=0) line(662,121+mapxoff/2,789,121+mapxoff/2);
				if (mapyoff!=0) line(662+mapyoff/2,121,662+mapyoff/2,248);
				glPopAttrib();
			}

			scroller(775,257,790,330,15,15,materialnum(materiallist),7,matstart,mode);
			if (mode==DD_Draw)
			{
				material *pt=materiallist;
				for (x=1;x<=matstart;x++) pt=pt->next;
				for (x=matstart;x<=matstart+6;x++)
				{
					if (pt!=NULL)
					{
						if (x==matselected) glColor4f(col4); else glColor4f(buttontextlit);
						glRasterPos2i(663,267+(x-matstart)*10);
						sprintf(s,"%3d:%s (%d|%d)",x,pt->name,pt->number,pt->texture);
						glPrint(s,base2,22);
						pt=pt->next;
					}
				}
			}
			if (mode==DD_Check)
			{
				if (mouseinwindow(661,427-170,774,496-170) && leftclickinwindow(661,427-170,774,496-160) && (materiallist!=NULL))
				{
					matselected=min(materialnum(materiallist)-1,(my-427+170)/10+matstart);
					material *pt=materiallist;
					for (x=1;x<=matselected;x++) pt=pt->next;
					mattexture=pt->handle;
				}
				if (mouseinwindow(661,427-170,774,496-170)) matstart-=wheel;
			}

			if (button(706,450+61,751,466+61,texbutton3,0,48.0/256.0,45.0/256.0,64.0/256.0,mapswap,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) 
			{
				waitleftbutton=true;
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected) 
					{
						o->objdata.swaptexturexy=!o->objdata.swaptexturexy;
						obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);
					}
					o=o->next;
				}
			}
			if (button(661,450+61,706,466+61,texbutton3,0,32.0/256.0,45.0/256.0,48.0/256.0,mapyswap,mode) == DDgui_LeftClick && mode!=DD_AfterCheck)
			{
				waitleftbutton=true;
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected) 
					{
						o->objdata.inverttexy=!o->objdata.inverttexy;
						obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);
					}
					o=o->next;
				}
			}
			if (button(751,450+61,796,466+61,texbutton3,0,64.0/256.0,45.0/256.0,80.0/256.0,mapxswap,mode) == DDgui_LeftClick && mode!=DD_AfterCheck)
			{
				waitleftbutton=true;
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected) 
					{
						o->objdata.inverttexx=!o->objdata.inverttexx;
						obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);
					}
					o=o->next;
				}
			}
			
			if ((mode==DD_Check) && leftclickinwindow(661,340,790,283+140) || mouseinwindow(661,340,790,283+140))
			{
				obj=actualscene->objects;
				while (obj!=NULL)
				{
					if (obj->selected)
					{
						obj->objdata.color[0]=(unsigned char)(colr);
						obj->objdata.color[1]=(unsigned char)(colg);
						obj->objdata.color[2]=(unsigned char)(colb);
						obj->objdata.color[3]=(unsigned char)(colx);
						obj->color[0]=(unsigned char)(colr);
						obj->color[1]=(unsigned char)(colg);
						obj->color[2]=(unsigned char)(colb);
						obj->color[3]=(unsigned char)(colx);
					}
					obj=obj->next;
				}
			}
			if ((mode==DD_Check) && (leftclickinwindow(661,470,790,510)))
			{
				obj=actualscene->objects;
				while (obj!=NULL)
				{
					if (obj->selected)
					{
						obj->objdata.texx=mapxr;
						obj->objdata.texy=mapyr;
						obj_counttexturecoordinates(obj,0,obj->objdata.texx,obj->objdata.texy,obj->objdata.texox,obj->objdata.texoy,obj->objdata.swaptexturexy,obj->objdata.inverttexx,obj->objdata.inverttexy);
					}
					obj=obj->next;
				}
			}

			if (materiallist!=NULL)
			{
				//if (button(661,506,704,526,"SET",9,13,base2,false,mode) == DDgui_LeftClick) 
				if (button(661,506-85,706,522-85,texbutton2,0,112.0/256.0,45.0/256.0,128.0/256.0,false,mode) == DDgui_LeftClick)
				{
					object *o=actualscene->objects;
					material *pt=materiallist;
					for (x=1;x<=matselected;x++) pt=pt->next;
					while (o!=NULL)
					{
						if (o->selected)
						{
							o->objdata.textured=true;
							o->objdata.material1=pt->number;
							int xa;
							for (xa=0;xa<o->polynum;xa++)
							{
								o->polygonlist[xa].materialhandle=mattexture;
							}
						}
						o=o->next;
					}
				};
				if (button(661,522-85,706,538-85,texbutton2,0,160.0/256.0,45.0/256.0,176.0/256.0,false,mode) == DDgui_LeftClick) 
				{
					object *o=actualscene->objects;
					material *pt=materiallist;
					for (x=1;x<=matselected;x++) pt=pt->next;
					while (o!=NULL)
					{
						if (o->selected)
						{
							o->objdata.envmapped=true;
							o->objdata.material2=pt->number;
							int xa;
							for (xa=0;xa<o->polynum;xa++)
							{
								o->polygonlist[xa].envmaphandle=mattexture;
							}
						}
						o=o->next;
					}
				};
				if (button(706,506-85,751,522-85,texbutton2,0,128.0/256.0,45.0/256.0,144.0/256.0,false,mode) == DDgui_LeftClick) {
					object *o=actualscene->objects;
					material *pt=materiallist;
					for (x=1;x<=matselected;x++) pt=pt->next;
					while (o!=NULL)
					{
						if (o->selected)
						{
							o->objdata.textured=false;
							o->objdata.material1=0;
							int xa;
							for (xa=0;xa<o->polynum;xa++)
							{
								o->polygonlist[xa].materialhandle=0;
							}
						}
						o=o->next;
					}
				};
				if (button(706,522-85,751,538-85,texbutton2,0,176.0/256.0,45.0/256.0,192.0/256.0,false,mode) == DDgui_LeftClick) {
					object *o=actualscene->objects;
					material *pt=materiallist;
					for (x=1;x<=matselected;x++) pt=pt->next;
					while (o!=NULL)
					{
						if (o->selected)
						{
							o->objdata.envmapped=false;
							o->objdata.material2=0;
							int xa;
							for (xa=0;xa<o->polynum;xa++)
							{
								o->polygonlist[xa].envmaphandle=0;
							}
						}
						o=o->next;
					}
				};
				if (leftclickinwindow(662,121,790,249) && mouseinwindow(662,121,790,249))
				{
					mapxoff=(my-121)*2;
					mapyoff=(mx-662)*2;
					object *o=actualscene->objects;
					while (o!=NULL)
					{
						o->objdata.texox=mapxoff;
						o->objdata.texoy=mapyoff;
						if (o->selected) obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);
						o=o->next;
					}
				}
                
				if (button(751,506-85,796,522-85,texbutton2,0,144.0/256.0,45.0/256.0,160.0/256.0,false,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) 
				{
					if (matselected==0)
					{
						materiallist=materiallist->next;
						if (materiallist==NULL) lastmaterial=NULL;
					}
					else
					{
						material *m=materiallist;
						for (int x=0; x<matselected-1; x++)
						{
							m=m->next;
						}
						m->next=m->next->next;
						if (m->next==NULL) 
						{
							matselected--;
							lastmaterial=NULL;
						}
					}
					waitleftbutton=true;
				};
			}
			
			if (button(751,522-85,796,538-85,texbutton2,0,192.0/256.0,45.0/256.0,208.0/256.0,false,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) {
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected)
					{
						o->objdata.normalsinverted=!o->objdata.normalsinverted;
						generatenormals(o);
						obj_transform(o,o->xformmatrix);
					}
					o=o->next;
				}
				waitleftbutton=true;
			};
			if (button(661,538-85,706,554-85,texbutton2,0,208.0/256.0,45.0/256.0,224.0/256.0,false,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) {
				
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected) 
					{						
						o->objdata.shading=dd3d_default;
						for (int x=0; x<o->polynum; x++) o->polygonlist[x].shading=o->polygonlist[x].defshading;
					}
					o=o->next;
				}
				waitleftbutton=true;
			};
			if (button(706,538-85,751,554-85,texbutton2,0,224.0/256.0,45.0/256.0,240.0/256.0,false,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) {
				
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected) 
					{						
						o->objdata.shading=dd3d_flatshade;
						for (int x=0; x<o->polynum; x++) o->polygonlist[x].shading=dd3d_flatshade;
					}
					o=o->next;
				}
				waitleftbutton=true;
			};
			if (button(751,538-85,796,554-85,texbutton2,0,240.0/256.0,45.0/256.0,256.0/256.0,false,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) {
				
				object *o=actualscene->objects;
				while (o!=NULL)
				{
					if (o->selected) 
					{						
						o->objdata.shading=dd3d_gouraudshade;
						for (int x=0; x<o->polynum; x++) o->polygonlist[x].shading=dd3d_gouraudshade;
					}
					o=o->next;
				}
				waitleftbutton=true;
			};

		xformmenu=true;
		break;
			} // xfrom menu switch end
	case 5: { // effects menu
		
		if (button(706,39,791,55,texbutton1,0,16.0/256.0,85.0/256.0,32.0/256.0,modellsubmenu[modellermenu]==0,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=0;
		if (button(706,55,791,71,texbutton1,0,0,85.0/256.0,16.0/256.0,modellsubmenu[modellermenu]==1,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=1;
		if (button(706,71,791,87,texbutton1,0,32.0/256.0,85.0/256.0,48.0/256.0,modellsubmenu[modellermenu]==2,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=2;
		if (button(706,87,791,103,texbutton1,0,48.0/256.0,85.0/256.0,64.0/256.0,modellsubmenu[modellermenu]==3,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=3;


		switch (modellsubmenu[modellermenu])
		{
		case 0:
			{
				ValueBar(661,153,0,1,5,&bttrammount," AMOUNT",25,13,base2,mode);
				if ((button(725,134,786,150,mainmenutex,329.0/512.0,0,390.0/512.0,16.0/128.0,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
				{
					SELECTION *sel = createselection(*actualscene);
					if (sel!=NULL)
					{
						scene_addobject(actualscene);
						for (object *x=actualscene->objects;x!=NULL;x=x->next)
						{
							if (x->selected && !x->effect && !x->track)
							{
								for (y=0; y<bttrammount; y++) butterflysubdivision(x,false);
								SELECTION *newsel = new SELECTION;
								newsel->selected=actualscene->lastobject->objdata.number;
								newsel->next=x->effects;
								x->effects=newsel;
							}
						}
						strcpy(actualscene->lastobject->name,"Butterfly");
						actualscene->lastobject->params[1]=bttrammount;
						actualscene->lastobject->params[2]=int(sel);
						actualscene->lastobject->cloneparents=sel;
						actualscene->lastobject->effect=true;
						actualscene->lastobject->primitive=dd3d_butterfly;						
					}
					waitleftbutton=true;
				}
				if ((button(664,134,725,150,mainmenutex,329.0/512.0,16.0/128.0,390.0/512.0,32.0/128.0,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
				{
					SELECTION *sel = createselection(*actualscene);
					if (sel!=NULL)
					{
						scene_addobject(actualscene);
						for (object *x=actualscene->objects;x!=NULL;x=x->next)
						{
							if (x->selected && !x->effect && !x->track)
							{
								for (y=0; y<bttrammount; y++)	butterflysubdivision(x,true);
								SELECTION *newsel = new SELECTION;
								newsel->selected=actualscene->lastobject->objdata.number;
								newsel->next=x->effects;
								x->effects=newsel;
							}
						}
						strcpy(actualscene->lastobject->name,"Linear");
						actualscene->lastobject->params[1]=bttrammount;
						actualscene->lastobject->params[2]=int(sel);
						actualscene->lastobject->cloneparents=sel;
						actualscene->lastobject->effect=true;
						actualscene->lastobject->primitive=dd3d_linearsubdivision;
					}
					waitleftbutton=true;
				}
			break;
			}
		case 1:
			{
				if (!maptransforming || maptransform==0)
				if ((button(717,340,736,359,cubetex,57.0/256.0,95.0/256.0,76.0/256.0,114.0/256.0,maptransform==0,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {maptransform=0;}
				if (!maptransforming)
				if ((button(698,340,717,359,cubetex,57.0/256.0,133.0/256.0,76.0/256.0,152.0/256.0,maptransform==1 && !maptransforming,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {maptransform=1;}
				/*if (maptransforming && maptransform==1)
				{
					if ((button(661,231,705,251,"SCLE",4,13,base2,transformation==2,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {transformation=2;}
					if ((button(706,231,748,251,"ESCL",4,13,base2,transformation==3,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {transformation=3;}
					if ((button(749,231,790,251,"GROW",3,13,base2,transformation==4,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {transformation=4;}
				}*/
				if (!maptransforming || maptransform==2)
				if ((button(736,340,755,359,cubetex,57.0/256.0,114.0/256.0,76.0/256.0,133.0/256.0,maptransform==2,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {maptransform=2;}
				if ((button(698,359,717,378,mainmenutex,263.0/512.0,32.0/128.0,282.0/512.0,51.0/128.0,mapchannel==0,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {mapchannel=0;mapxform();}
				if ((button(717,359,736,378,mainmenutex,263.0/512.0,51.0/128.0,282.0/512.0,70.0/128.0,mapchannel==1,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {mapchannel=1;mapxform();}
				if ((button(736,359,755,378,mainmenutex,263.0/512.0,70.0/128.0,282.0/512.0,89.0/128.0,mapchannel==2,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {mapchannel=2;mapxform();}
				if ((button(698,378,755,394,texbutton2,0,48.0/256.0,57.0/256.0,64.0/256.0,mapchannel==3,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) {mapchannel=3;mapxform();}
				if (mode==DD_Draw)
				{
					glPushAttrib(GL_ALL_ATTRIB_BITS);
					switch (mapchannel)
					{
						case 0 : {glColor4f(1,0,0,1); break;}//red
						case 1 : {glColor4f(0,1,0,1); break;}//green
						case 2 : {glColor4f(0,0,1,1); break;}//blue
						case 3 : {glColor4f(1,1,1,1); break;}//all		
					}
					glDisable(GL_DEPTH_TEST);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,mattexture);
					glBegin(GL_QUADS);
					glTexCoord2f(0,0);
					glVertex2i(662,121);
					glTexCoord2f(0,1);
					glVertex2i(790,121);
					glTexCoord2f(1,1);
					glVertex2i(790,249);
					glTexCoord2f(1,0);
					glVertex2i(662,249);
					glEnd();
					glDisable(GL_TEXTURE_2D);
					glColor4f(buttontextlit);
					rectangle(661,120,790,249);
					rectangle(661,257,774,330);
					glPopAttrib();
				}

				scroller(775,257,790,330,15,15,materialnum(materiallist),7,matstart,mode);
				if (mode==DD_Draw)
				{
					material *pt=materiallist;
					for (x=1;x<=matstart;x++) pt=pt->next;
					for (x=matstart;x<=matstart+6;x++)
					{
						if (pt!=NULL)
						{
							if (x==matselected) glColor4f(col4); else glColor4f(buttontextlit);
							glRasterPos2i(663,267+(x-matstart)*10);
							sprintf(s,"%3d:%s",x,pt->name);
							glPrint(s,base2,12);
							pt=pt->next;
						}
					}
				}
				if (mode==DD_Check)
				{
					if (mouseinwindow(661,257,774,326) && leftclickinwindow(661,257,774,326) && (materiallist!=NULL))
					{
						matselected=min(materialnum(materiallist)-1,(my-257)/10+matstart);
						material *pt=materiallist;
						for (x=1;x<=matselected;x++) pt=pt->next;
						mattexture=pt->handle;
						tTexture *txlist=texturelist;
						for (x=0;x<pt->texture;x++) txlist=txlist->next;
						memcpy(maptexture,txlist->layers[pt->layer],256*256*4);
						mapxform();
					}
					if (mouseinwindow(661,257,774,326)) matstart-=wheel;
				}

				if (maptransforming)
				{
					glPushAttrib(GL_ALL_ATTRIB_BITS);
					glDisable(GL_DEPTH_TEST);
					glColor4f(buttontextlit);
					if (mapxoff!=0) line(662,121+mapxoff/2,789,121+mapxoff/2);
					if (mapyoff!=0) line(662+mapyoff/2,121,662+mapyoff/2,248);
					glPopAttrib();
					ValueBar(661,410,0,0,255,&mapxr,"X SCALE",28,13,base2,mode);
					ValueBar(661,430,0,0,255,&mapyr,"Y SCALE",28,13,base2,mode);
					if (leftclickinwindow(661,410,800,450))
					{
						object *o=actualscene->objects;
						while (o!=NULL)
						{
							if (o->selected) obj_counttexturecoordinates(o, mapmapping, mapxr, mapyr, mapxoff, mapyoff, mapswap, mapxswap, mapyswap);
							o=o->next;
						}
						mapxform();
					}
					if (leftclickinwindow(662,121,790,249) && mouseinwindow(662,121,790,249))
					{
						mapxoff=(my-121)*2;
						mapyoff=(mx-662)*2;
						object *o=actualscene->objects;
						while (o!=NULL)
						{
							if (o->selected) obj_counttexturecoordinates(o, mapmapping, mapxr, mapyr, mapxoff, mapyoff, mapswap, mapxswap, mapyswap);
							o=o->next;
						}
						mapxform();
					}
					if (button(706,450,751,466,texbutton3,0,48.0/256.0,45.0/256.0,64.0/256.0,mapswap,mode) == DDgui_LeftClick && mode!=DD_AfterCheck) 
					{
						mapswap=!mapswap;
						waitleftbutton=true;
						object *o=actualscene->objects;
						while (o!=NULL)
						{
							if (o->selected) obj_counttexturecoordinates(o, mapmapping, mapxr, mapyr, mapxoff, mapyoff, mapswap, mapxswap, mapyswap);
							o=o->next;
						}
						mapxform();
					}
					if (button(661,450,706,466,texbutton3,0,32.0/256.0,45.0/256.0,48.0/256.0,mapyswap,mode) == DDgui_LeftClick && mode!=DD_AfterCheck)
					{
						mapyswap=!mapyswap;
						waitleftbutton=true;
						object *o=actualscene->objects;
						while (o!=NULL)
						{
							if (o->selected) obj_counttexturecoordinates(o, mapmapping, mapxr, mapyr, mapxoff, mapyoff, mapswap, mapxswap, mapyswap);
							o=o->next;
						}
						mapxform();
					}
					if (button(751,450,796,466,texbutton3,0,64.0/256.0,45.0/256.0,80.0/256.0,mapxswap,mode) == DDgui_LeftClick && mode!=DD_AfterCheck)
					{
						mapxswap=!mapxswap;
						waitleftbutton=true;
						object *o=actualscene->objects;
						while (o!=NULL)
						{
							if (o->selected) obj_counttexturecoordinates(o, mapmapping, mapxr, mapyr, mapxoff, mapyoff, mapswap, mapxswap, mapyswap);
							o=o->next;
						}
						mapxform();
					}
				}

				if (materiallist!=NULL && !maptransforming)
				{
					if ((button(685,394,770,410,texbutton1,0,0,85.0/256.0,16.0/256.0,maptransforming,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck) 
					{
						if (!maptransforming)
						{
							pretransform=transformation;
							switch (maptransform)
							{
							case 0: {transformation=0; break;}
							case 1: {transformation=2; break;}
							case 2: {transformation=1; break;}
							}
							modellviews[0].transformation=transformation+1;
							modellviews[1].transformation=transformation+1;
							modellviews[2].transformation=transformation+1;
							m_identity(mapmatrix);
							m_identity(mapbufmatrix);
							material *pt=materiallist;
							for (x=1;x<=matselected;x++) pt=pt->next;
							mattexture=pt->handle;
							tTexture *txlist=texturelist;
							for (x=0;x<pt->texture;x++) txlist=txlist->next;
							memcpy(maptexture,txlist->layers[pt->layer],256*256*4);
						}
						mapxr=1;
						mapyr=1;
						maptransforming=!maptransforming;
						waitleftbutton=true;
					}
				}
				else if (materiallist!=NULL && maptransforming)
				{
					if ((button(669,394,726,410,texbutton3,0,0,57.0/256.0,16.0/256.0,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
					{
						SELECTION *sel = createselection(*actualscene);
						if (sel!=NULL)
						{
							scene_addobject(actualscene);
							object *o=actualscene->objects;
							while (o!=NULL)
							{
								if (o->selected)
								{
									for (x=0; x<o->vertexnum; x++)
									{
										o->vertexlist[x].base=o->vertexlist[x].d;
										o->vertexlist[x].t=o->vertexlist[x].dt;
									}								
									for (x=0; x<o->polynum; x++)
									{
										o->polygonlist[x].t1=o->polygonlist[x].d1;
										o->polygonlist[x].t2=o->polygonlist[x].d2;
										o->polygonlist[x].t3=o->polygonlist[x].d3;
									}
									generatenormals(o);
									obj_transform(o,o->xformmatrix);
									
									SELECTION *newsel = new SELECTION;
									newsel->selected=actualscene->lastobject->objdata.number;
									newsel->next=o->effects;
									o->effects=newsel;

								}
								o=o->next;							
							}
							strcpy(actualscene->lastobject->name,"Map Xform");
							actualscene->lastobject->params[1]=1;
							actualscene->lastobject->params[2]=int(sel);
							actualscene->lastobject->cloneparents=sel;
							actualscene->lastobject->effect=true;
							actualscene->lastobject->primitive=dd3d_map;
							material *pt=materiallist;
							for (x=1;x<=matselected;x++) pt=pt->next;
							actualscene->lastobject->objdata.material1=pt->number;
							actualscene->lastobject->objdata.chnnl1=mapchannel;
							actualscene->lastobject->objdata.inverttexx=mapxswap;
							actualscene->lastobject->objdata.inverttexy=mapyswap;
							actualscene->lastobject->objdata.mapping=mapmapping;
							actualscene->lastobject->objdata.swaptexturexy=mapswap;
							actualscene->lastobject->objdata.texox=mapxoff;
							actualscene->lastobject->objdata.texoy=mapyoff;
							actualscene->lastobject->objdata.texx=mapxr;
							actualscene->lastobject->objdata.texy=mapyr;
							actualscene->lastobject->objdata.material2=maptransform;
							memcpy(actualscene->lastobject->xformmatrix,mapmatrix,sizeof(matrix));

						}
						transformation=pretransform;
						maptransforming=!maptransforming;
						
						waitleftbutton=true;
					}
					if ((button(726,394,783,410,texbutton3,0,16.0/256.0,57.0/256.0,32.0/256.0,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
					{
						object *o=actualscene->objects;
						while (o!=NULL)
						{
							if (o->selected)
							{
								for (x=0; x<o->vertexnum; x++)
								{
									o->vertexlist[x].d=o->vertexlist[x].base;
									o->vertexlist[x].t=o->vertexlist[x].dt;
								}
								for (x=0; x<o->polynum; x++)
								{
									o->polygonlist[x].t1=o->polygonlist[x].d1;
									o->polygonlist[x].t2=o->polygonlist[x].d2;
									o->polygonlist[x].t3=o->polygonlist[x].d3;
								}
								generatenormals(o);
								obj_transform(o,o->xformmatrix);
							}
							o=o->next;							
						}
						transformation=pretransform;
						maptransforming=!maptransforming;
						waitleftbutton=true;
					}

				}
				break;
			}
		case 2:
			{
				ValueBar(661,153,0,1,5,&blurammount," AMOUNT",25,13,base2,mode);
				if ((button(684,134,769,150,texbutton1,0,32.0/256.0,85.0/256.0,48.0/256.0,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
				{
					SELECTION *sel = createselection(*actualscene);
					if (sel!=NULL)
					{
						scene_addobject(actualscene);
						for (object *x=actualscene->objects;x!=NULL;x=x->next)
						{
							if (x->selected && !x->effect && !x->track)
							{
								for (y=0; y<blurammount; y++) meshblur(x);
								SELECTION *newsel = new SELECTION;
								newsel->selected=actualscene->lastobject->objdata.number;
								newsel->next=x->effects;
								x->effects=newsel;
							}
						}
						strcpy(actualscene->lastobject->name,"Blur");
						actualscene->lastobject->params[1]=blurammount;
						actualscene->lastobject->params[2]=int(sel);
						actualscene->lastobject->cloneparents=sel;
						actualscene->lastobject->effect=true;
						actualscene->lastobject->primitive=dd3d_blur;
					}
					waitleftbutton=true;
				}
			break;
			}
		case 3:
			{
				if (mode==DD_Draw)
				{
					glColor4f(buttontextlit);
					rectangle(661,135,774,210);
					rectangle(661,225,774,300);
					glColor4f(col4);
					glRasterPos2i(661,130);
					glPrint("SELECT BASE OBJ",base2);
					glRasterPos2i(661,220);
					glPrint("SELECT NEW OBJ",base2);
				}

				scroller(775,135,790,210,15,15,objnum(actualscene->objects),7,o1start,mode);
				scroller(775,225,790,300,15,15,objnum(actualscene->objects),7,o2start,mode);
				
				if ((mode==DD_Check) && leftclickinwindow(661,135,775,204) && mouseinwindow(661,135,775,204)) bool1=min(o1start+(my-135) / 10,objnum(actualscene->objects)-1);
				if (mode==DD_Draw)
				{
					object *o = actualscene->objects;
					for (x=0;x<o1start;x++)o=o->next;
					for (x=0;x<=6;x++)
					{
						if (o!=NULL)
						{
							if (o1start+x==bool1) glColor4f(col4); else glColor4f(buttontextlit);
							glRasterPos2i(663,145+x*10);
							glPrint(o->name,base2,12);
							o=o->next;
						}
					}
				}

				if ((mode==DD_Check) && leftclickinwindow(661,215,775,294) && mouseinwindow(661,215,775,294)) bool2=min(o2start+(my-225) / 10,objnum(actualscene->objects)-1);
				if (mode==DD_Draw)
				{
					object *o = actualscene->objects;
					for (x=0;x<o2start;x++)o=o->next;
					for (x=0;x<=6;x++)
					{
						if (o!=NULL)
						{
							if (o2start+x==bool2) glColor4f(col4); else glColor4f(buttontextlit);
							glRasterPos2i(663,235+x*10);
							glPrint(o->name,base2,12);
							o=o->next;
						}
					}
				}

				if (button(698,315,717,334,mainmenutex,320.0/512.0,32.0/128.0,339.0/512.0,51.0/128.0,booleanop==0,mode) == DDgui_LeftClick) booleanop=0;
				if (button(717,315,736,334,mainmenutex,320.0/512.0,51.0/128.0,339.0/512.0,70.0/128.0,booleanop==1,mode) == DDgui_LeftClick) booleanop=1;
				if (button(736,315,755,334,mainmenutex,320.0/512.0,70.0/128.0,339.0/512.0,89.0/128.0,booleanop==2,mode) == DDgui_LeftClick) booleanop=2;
				object *a,*b;
				a=actualscene->objects;
				b=actualscene->objects;
				bool1=min(bool1,objnum(actualscene->objects));
				bool2=min(bool2,objnum(actualscene->objects));
				for (x=0;x<bool1;x++) a=a->next;
				for (x=0;x<bool2;x++) b=b->next;

				if (bool1!=bool2 && a!=NULL && b!=NULL && !a->effect && !b->effect && !a->track && !b->track)
				if ((button(685,336,770,352,texbutton1,0,48.0/256.0,85.0/256.0,64.0/256.0,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
				{
					scene_addobject(actualscene);

					SELECTION *newsel = new SELECTION;
					newsel->selected=actualscene->lastobject->objdata.number;
					newsel->next=a->effects;
					a->effects=newsel;

					newsel = new SELECTION;
					newsel->selected=a->objdata.number;
					newsel->next=b->effects;
					b->effects=newsel;

					newsel = new SELECTION;
					newsel->selected=a->objdata.number;
					newsel->next=actualscene->lastobject->cloneparents;
					actualscene->lastobject->cloneparents=newsel;

					newsel = new SELECTION;
					newsel->selected=b->objdata.number;
					newsel->next=a->cloneparents;
					a->cloneparents=newsel;

					obj_boolean(a,b,booleanop);
					strcpy(actualscene->lastobject->name,"Bool");
					actualscene->lastobject->params[1]=booleanop;
					actualscene->lastobject->params[2]=a->objdata.number;
					actualscene->lastobject->params[3]=b->objdata.number;
					actualscene->lastobject->effect=true;
					actualscene->lastobject->primitive=dd3d_boolean;
					b->hidden=true;
					memcpy(actualscene->lastobject->xformmatrix,a->xformmatrix,sizeof(matrix));
					m_invert(actualscene->lastobject->xformmatrix);
					m_mult(actualscene->lastobject->xformmatrix,b->xformmatrix,actualscene->lastobject->xformmatrix);
					waitleftbutton=true;
				}
			break;
			}
		}

		xformmenu=true;
		break;
			} // effects menu switch end
	case 6: { // edit menu
		xformmenu=true;
		break;
			} // edit menu switch end
	case 7: { //file menu
		xformmenu=true;
	
		if (button(703,45,748,61,mainmenutex,377.0/512.0,32.0/128.0,422.0/512.0,48.0/128.0,modellsubmenu[modellermenu]==0,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=0;
		if (button(748,45,793,61,mainmenutex,377.0/512.0,48.0/128.0,422.0/512.0,64.0/128.0,modellsubmenu[modellermenu]==1,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=1;
		if (button(703,61,748,77,mainmenutex,377.0/512.0,64.0/128.0,422.0/512.0,80.0/128.0,modellsubmenu[modellermenu]==2,mode) == DDgui_LeftClick) {modellsubmenu[modellermenu]=2; scnlist=findfile(scenemask); }
		if (button(748,61,793,77,mainmenutex,377.0/512.0,80.0/128.0,422.0/512.0,96.0/128.0,modellsubmenu[modellermenu]==3,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=3;
		if (button(703,77,748,93,mainmenutex,377.0/512.0,96.0/128.0,422.0/512.0,112.0/128.0,false,mode) == DDgui_LeftClick) clearscene(actualscene);
		if (button(748,77,793,93,mainmenutex,377.0/512.0,112.0/128.0,422.0/512.0,128.0/128.0,modellsubmenu[modellermenu]==5,mode) == DDgui_LeftClick) modellsubmenu[modellermenu]=5;

		switch (modellsubmenu[modellermenu])
		{
		case 0: {
			if (button(703,115,748,131,mainmenutex,377.0/512.0,32.0/128.0,422.0/512.0,48.0/128.0,false,mode) == DDgui_LeftClick)
			{
				scene *nscene = newscene();
				nscene->editview=perspcam;
				nscene->number=lastscene->number+1;
				lastscene->next=nscene;
				lastscene=nscene;
				char *ss=readline("Enter Scene Name",270,0,base,"");
				sprintf(nscene->name,"%s",ss);
			}
			break;
				}
		case 2: { // import scene
			if (mode==DD_Draw)
			{
				glColor4f(buttontextlit);
				rectangle(661,120,775,264);
			}	  
			if (mode==DD_Check)
			if (leftclickinwindow(661,121,774,263) && mouseinwindow(661,121,774,260)) scnselected=min(scnbarpos+(my-121) / 10,filenum(scnlist)-1);
			if ((mode ==DD_Check) && mouseinwindow(661,121,774,263)) scnbarpos-=wheel*4;
			scroller(775,120,790,264,15,15,filenum(scnlist),14,scnbarpos,mode);
			if (mode==DD_Draw)
			{
				pf = scnlist;
				for (x=1;x<=scnbarpos;x++) pf=pf->next;
				for (x=0;x<=13;x++)
				{
					if (pf!=NULL)
					{
						if (scnbarpos+x==scnselected) glColor4f(col4); else glColor4f(buttontextlit);
						glRasterPos2i(665,130+x*10);
						strcpy(st,pf->filedata.cFileName);
						glPrint(st,base2,18);
						pf=pf->next;
					}
				}

				glColor4f(buttontextlit);
				sprintf(s,"%d SCENES.",filenum(scnlist));
				glRasterPos2i(683,277);
				glPrint(s,base2);

			}
			glColor4f(1,1,1,1);
			RadioButton(681,301,scnobjectsave,"OBJECTS",mode);
			RadioButton(681,315,scnselectionsave,"SELECTIONS",mode);
			RadioButton(681,329,scntexturesave,"TEXTURES",mode);
			RadioButton(681,343,scnlightsave,"LIGHTS",mode);
			RadioButton(681,357,scncamerasave,"CAMERAS",mode);
			RadioButton(681,371,scnenvironmentsave,"ENVIRONMENT",mode);
			if ((button(686,283,771,299,texbutton1,0,96.0/256.0,85.0/256.0,112.0/256.0,false,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck))
			{
				int ID=actualscene->number;
				pf = scnlist;
				for (x=1;x<=scnselected;x++) pf=pf->next;
				sprintf(s,"%s%s",scenedir,pf->filedata.cFileName);
				//loadaDDictscene(actualscene,NULL,s,scnobjectsave,scnselectionsave,scntexturesave,scnlightsave,scncamerasave,scnenvironmentsave);
				loadandscene(*actualscene,s);
				tTexture *tex=texturelist;
				while ((tex!=NULL) && (tex->number!=selectedtexture)) tex=tex->next;
				memcpy(generatedtexture.commands,tex->commands,sizeof(generatedtexture.commands));
				memcpy(generatedtexture.texts,tex->texts,sizeof(generatedtexture.texts));
				generatedtexture.commandnum=tex->commandnum;
				memcpy(generatedtexture.layers,tex->layers,sizeof(generatedtexture.layers));
				for (y=0;y<=3;y++)
				{
					glBindTexture(GL_TEXTURE_2D, texlayers[y]);
					glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
				}				
				
				if (materiallist!=NULL)
				{
					matselected=0;
					material *mat=materiallist;
					for (x=1;x<=matselected;x++) mat=mat->next;
					mattexture=mat->handle;
				}			

				actualscene=findscene(scenelist,ID);

				waitleftbutton=true;
			}
			break;
				}
		case 3: { // export scene
			if (button(685,121,770,137,texbutton1,0,144.0/256.0,85.0/256.0,160.0/256.0,false,mode) == DDgui_LeftClick) 
			{
				char *ss=readline("Enter Filename (.scn not needed)",210,0,base2,"");
				if (ss!="")
				{
					sprintf(s,"%s%s.nsc",scenedir,ss);
					saveaDDictscene(*actualscene,NULL,s);
				}
			}
			/*if (button(685,237,770,253,texbutton1,0,160.0/256.0,85.0/256.0,176.0/256.0,false,mode) == DDgui_LeftClick) 
			{
				char *ss=readline("Enter Filename (.scn not needed)",210,0,base2,"");
				if (ss!="")
				{
					sprintf(s,"%s%s.min",minimaldir,ss);
					SaveMinimalScene(*actualscene,NULL,s);
				}
			}*/
			break;
				}
		case 5: { // select scene
			if (mode==DD_Draw)
			{
				glColor4f(buttontextlit);
				rectangle(661,120,775,264);
			}	  
			if (mode==DD_Check)
			if (leftclickinwindow(661,121,774,263) && mouseinwindow(661,121,774,260)) sceneselected=min(scenepos+(my-121) / 10,cntscenes()-1);
			if ((mode ==DD_Check) && mouseinwindow(661,121,774,263)) scenepos-=wheel*4;
			scroller(775,120,790,264,15,15,cntscenes(),14,scenepos,mode);
			if (mode==DD_Draw)
			{
				scene *pf = scenelist;
				for (x=1;x<=scenepos;x++) pf=pf->next;
				for (x=0;x<=13;x++)
				{
					if (pf!=NULL)
					{
						if (scenepos+x==sceneselected) glColor4f(col4); else glColor4f(buttontextlit);
						glRasterPos2i(665,130+x*10);
						//strcpy(sst,pf->name);
						sprintf(sst,"%s (%d)",pf->name,pf->number);
						glPrint(sst,base2,18);
						pf=pf->next;
					}
				}

				glColor4f(buttontextlit);
				sprintf(s,"%d SCENES.",cntscenes());
				glRasterPos2i(683,277);
				glPrint(s,base2);

			}
			scene *pf = scenelist;
			for (x=1;x<=sceneselected;x++) pf=pf->next;
			if (actualscene!=pf)
			{
				actualscene=pf;
				modellviews[3].cam=actualscene->editview;
				modellviews[3].cam2=actualscene->editview;
			}

			break;
				}

		}


	
		break;
			}//filemenu switch end

	
	}//modellermenu switch end

	//modellviews[3].cam.fov=65;
	for (object *o=actualscene->objects;o;o=o->next)
	{
		for (int x=0; x<4; x++)
		{
			o->colbuf[x]=o->objdata.color[x];
			o->keyframercol[x]=o->objdata.color[x];
		}
	}

}