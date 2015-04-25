#include "keyframergui.h"
#include "track.h"

char stri[256];

window3d keyframeviews[4];
int keyframermenu=1;

int timelinestart=0;
int timelinezoom=1;
int currentframe=0;
bool playing=false;
int playstart;
bool keyframethere=false;

int camselected=0,campos=0;

objectanimation *actualanim;
sceneobjanim *actualanimation;
int actualanimnum=0,actualanimlength=0;
cameraanimation *actualcameraanim;
camera actualcamera;
bool camchanged = false;
bool objchanged = false;
int hierarchystart,hierarchyselected;
int animstart=0,animselected=0;

void getscenestate()
{
	for (object *o=actualscene->objects;o;o=o->next)
	{
		if (!o->effect && !o->hidden)
		{
			o->changed=false;
			getobjectstate(o->actualanim,*o,(float)currentframe);
		}
	}
	objchanged=false;
}

void initkeyframergui()
{
	memset(keyframeviews,0,sizeof(keyframeviews));
 
	int x;

	for (x=0; x<=3; x++)
	{
		keyframeviews[x].pos=x;
		keyframeviews[x].grid=true;
		keyframeviews[x].wireframe=true;
		keyframeviews[x].a=1;
		keyframeviews[x].b=1;
		keyframeviews[x].c=1;
		keyframeviews[x].xsize=320;
		keyframeviews[x].ysize=240;
	}

	
	keyframeviews[0].xpos=148;
	keyframeviews[0].ypos=33;
	keyframeviews[0].oxpos=148;
	keyframeviews[0].oypos=33;
	keyframeviews[0].cam=upcam;
	keyframeviews[0].name="TOP VIEW";
	keyframeviews[0].c1=0;
	keyframeviews[0].c2=2;
	keyframeviews[0].c3=1;
	keyframeviews[0].a=-1;
	keyframeviews[0].xn="Z";
	keyframeviews[0].yn="X";
	keyframeviews[0].transformation=1;
	keyframeviews[0].centertransform=true;
	keyframeviews[0].objspacetransform=false;
	keyframeviews[1].xpos=468;
	keyframeviews[1].ypos=33;
	keyframeviews[1].oxpos=468;
	keyframeviews[1].oypos=33;
	keyframeviews[1].cam=frontcam;
	keyframeviews[1].name="FRONT VIEW";
	keyframeviews[1].c1=0;
	keyframeviews[1].c2=1;
	keyframeviews[1].c3=2;
	keyframeviews[1].a=-1;
	keyframeviews[1].b=-1;
	keyframeviews[1].xn="-Y";
	keyframeviews[1].yn="X";
	keyframeviews[1].transformation=1;
	keyframeviews[1].centertransform=true;
	keyframeviews[1].objspacetransform=false;
	keyframeviews[2].xpos=148;
	keyframeviews[2].ypos=273;
	keyframeviews[2].oxpos=148;
	keyframeviews[2].oypos=273;
	keyframeviews[2].cam=leftcam;
	keyframeviews[2].name="LEFT VIEW";
	keyframeviews[2].c1=2;
	keyframeviews[2].c2=1;
	keyframeviews[2].c3=0;
	keyframeviews[2].b=-1;
	keyframeviews[2].xn="-Y";
	keyframeviews[2].yn="-Z";
	keyframeviews[2].transformation=1;
	keyframeviews[2].centertransform=true;
	keyframeviews[2].objspacetransform=false;
	keyframeviews[3].xpos=468;
	keyframeviews[3].ypos=273;
	keyframeviews[3].oxpos=468;
	keyframeviews[3].oypos=273;
	keyframeviews[3].cam=perspcam;
	keyframeviews[3].name="CAMERA";
	keyframeviews[3].c1=0;
	keyframeviews[3].c2=1;
	keyframeviews[3].a=-1;
	keyframeviews[3].wireframe=false;
	keyframeviews[3].camsensitivity=0.5;
	keyframeviews[3].transformation=1;
	keyframeviews[3].centertransform=false;
	keyframeviews[3].objspacetransform=false;

	actualcamera=perspcam;
	actualcameraanim=scenelist->cameraanims;
	actualanimation=scenelist->anims;

}

vector3 iselectcenterbase=origo;
vector3 iselectcenter=origo;

void countiselectcenter()
{
	vector v;
	object *o=actualscene->objects;
	while (!((o==NULL) || (o->selected && !o->track && !o->effect))) o=o->next;
	if (o!=NULL)
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
		iselectcenterbase=aa;
	}
}

objkeyframedata dotransformation2keyframer(window3d &viewdata, int x1, int y1, int x2, int y2,float xsize, float ysize, int mode)
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
	objkeyframedata ok,ook;
	memset(&ok,0,sizeof(ok));
	ok.stretch.b.x=1;
	ok.stretch.b.y=1;
	ok.stretch.b.z=1;
	ook=ok;
	if (keyframermenu!=1) return ok;
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
	countiselectcenter();
	d=iselectcenterbase;

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
		iselectcenter=iselectcenterbase;
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
					//m_xpose(l.b.x,l.b.y,l.b.z,t);
					ok.position=l;
					
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
					//m_rotate(l.b.x,l.b.y,l.b.z,(float)((my-ly)*radtheta),t);
					ok.rotaxis=l;
					ok.rotangle=my-ly;
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
					ok.rotaxis=l;
					ok.rotangle=mx-lx;
					sprintf(cursorstring,"(%d Degrees)",mx-lx);
				}
				else
				if (leftbutton)
				{
					l.a[viewdata.c1]=0;
					l.a[viewdata.c2]=0;
					l.a[viewdata.c3]=1;
					m_rotate(l.b.x,l.b.y,l.b.z,(float)((mx-lx)*radtheta),t);
					ok.rotaxis=l;
					ok.rotangle=mx-lx;
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

					//m_scale(l.b.x,l.b.y,l.b.z,t);
					ok.stretch=l;
					if (cursorstring[0]==' ')
						sprintf(cursorstring,"(%.3fx,%.3fx)",l.a[viewdata.c1],l.a[viewdata.c2]);
				}
			}
			break;
		}
	}
	glPopAttrib();
	if (leftclickinwindow(x1,y1,x2,y2)) transform=true; else return ook;
	return ok;
}

void displaykeyframewindow(window3d &win, unsigned char mode)
{
	memset(&win.or,0,sizeof(win.or));
	win.or.stretch.b.x=1;
	win.or.stretch.b.y=1;
	win.or.stretch.b.z=1;
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
		quad(x1,y1,x2,y2);
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
			actualscene->lights[lightselected].position[win.c1]=(float)(((float)(mx-x1)/(float)(x2-x1)-0.5)*xsize*win.a*-1+win.cam.eye.a[win.c1]);
			actualscene->lights[lightselected].position[win.c2]=(float)(((float)(my-y1)/(float)(y2-y1)-0.5)*ysize*win.b*-1+win.cam.eye.a[win.c2]);
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
		if (leftclickinwindow(x1,y1,x2,y2) && keyframermenu==2 && modellsubmenu[keyframermenu]==0 && !leftclickinwindow(x1,y1,x1+20+19*2,y1+19) && !leftclickinwindow(x2-19*3,y1,x2,y1+19) && !leftclickinwindow(x2-19,y1,x2,y1+19*3))
		{
			actualcamera.eye.a[win.c1]=win.cam.eye.a[win.c1]-(mx-win.xpos-(float)(x2-x1)/2.0f)*xsize/(x2-x1)*win.a;
			actualcamera.eye.a[win.c2]=win.cam.eye.a[win.c2]-(my-win.ypos-(float)(y2-y1)/2.0f)*ysize/(y2-y1)*win.b;
			actualcamera.up=origo;
			actualcamera.up.b.y=-1;
			camchanged=true;
		}
		if (rightclickinwindow(x1,y1,x2,y2) && keyframermenu==2 && modellsubmenu[keyframermenu]==0 && !leftclickinwindow(x1,y1,x1+20+19*2,y1+19) && !leftclickinwindow(x2-19*3,y1,x2,y1+19) && !leftclickinwindow(x2-19,y1,x2,y1+19*3))
		{
			actualcamera.target.a[win.c1]=win.cam.eye.a[win.c1]-(mx-win.xpos-(float)(x2-x1)/2.0f)*xsize/(x2-x1)*win.a;
			actualcamera.target.a[win.c2]=win.cam.eye.a[win.c2]-(my-win.ypos-(float)(y2-y1)/2.0f)*ysize/(y2-y1)*win.b;
			actualcamera.up=origo;
			actualcamera.up.b.y=-1;
			camchanged=true;
		}
	}
	if (!win.wireframe && (mode==DD_Check))
	{
		if (mouseinwindow(x1,y1,x2,y2) && (!leftbutton) && (!middlebutton)) 
		{
			l=egyseg(vect_kivonas(win.cam.target,win.cam.eye));
			win.cam.eye=vect_osszeadas(win.cam.eye,vect_skalarszorzas(l,wheel*win.camsensitivity));
			win.cam.target=vect_osszeadas(win.cam.target,vect_skalarszorzas(l,wheel*win.camsensitivity));
			actualcamera=win.cam;
			if (wheel!=0)
			camchanged=true;
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
			actualcamera=win.cam;

			camchanged=true;

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
			if (win.perspective) { scene_renderwireframe(*actualscene,4,win.c3,lofttrack,loftmesh); }
			else
			{ scene_renderwireframe(*actualscene,win.c3,win.c3,lofttrack,loftmesh); }
			/*glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);*/
			if (win.perspective)
			{
				glBegin(GL_LINES);
				glColor4f(1,0,0,1);
				glVertex3f(actualcamera.eye.b.x,actualcamera.eye.b.y,actualcamera.eye.b.z);
				glColor4f(1,1,0,1);
				glVertex3f(actualcamera.target.b.x,actualcamera.target.b.y,actualcamera.target.b.z);
				glEnd();
			}
			else
			{
				int aa[3];
				int x;
				for (x=0;x<=2;x++) if (x!=win.c3) {aa[x]=1;} else {aa[x]=0;}
				glBegin(GL_LINES);
				glColor4f(1,0,0,1);
				glVertex3f(actualcamera.eye.b.x*aa[0],actualcamera.eye.b.y*aa[1],actualcamera.eye.b.z*aa[2]);
				glColor4f(1,1,0,1);
				glVertex3f(actualcamera.target.b.x*aa[0],actualcamera.target.b.y*aa[1],actualcamera.target.b.z*aa[2]);
				glEnd();
			}
		}
		else
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FOG);
			scene_rendersolid(*actualscene,win.showhidden);
			if (win.perspective)
			scene_rendernormals(*actualscene);
			glDisable(GL_FOG);

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
	//if ((win.wireframe) && (!leftclickinwindow(x1,y1,x1+16+16+16,y1+16) || mode==DD_Draw)) {dotransformation2(win,x1,y1,x2,y2,xsize,ysize,mode);}
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

	if (win.wireframe)
	{
		if ((button(x1+1+19*2,y1,x1+20+19*2,y1+19,cubetex, 57.0/256.0, 18.0/256.0, 76.0/256.0, 37.0/256.0, win.perspective,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.perspective=!win.perspective; waitleftbutton=true;}

		if ((button(x2-19*3,y1,x2-19*2,y1+19,cubetex, 57.0/256.0, 94.0/256.0, 76.0/256.0, 113.0/256.0, win.transformation==1,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (win.transformation==1) win.transformation=0; else win.transformation=1; waitleftbutton=true;}
		if ((button(x2-19*2,y1,x2-19*1,y1+19,cubetex, 57.0/256.0, 113.0/256.0, 76.0/256.0, 132.0/256.0, win.transformation==2,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (win.transformation==2) win.transformation=0; else win.transformation=2; waitleftbutton=true;}
		if ((button(x2-19*1,y1,x2-19*0,y1+19,cubetex, 57.0/256.0, 132.0/256.0, 76.0/256.0, 151.0/256.0, win.transformation==3,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { if (win.transformation==3) win.transformation=0; else win.transformation=3; waitleftbutton=true;}
		if ((button(x2-19*1,y1+19,x2-19*0,y1+38,cubetex, 57.0/256.0, 151.0/256.0, 76.0/256.0, 170.0/256.0, win.centertransform,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.centertransform=!win.centertransform; waitleftbutton=true;}
		if ((button(x2-19*1,y1+38,x2-19*0,y1+57,cubetex, 57.0/256.0, 170.0/256.0, 76.0/256.0, 189.0/256.0, win.objspacetransform,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.objspacetransform=!win.objspacetransform; waitleftbutton=true;}

	}
	else
	{
		/*if (mode==DD_Draw)
		{
			glDisable(GL_DEPTH_TEST);
			glColor4d(col2);
			quad(x1+16+17+16+16,y1+1,x1+16+17+100+16+16,y1+16);
			glEnable(GL_DEPTH_TEST);
		}*/
		if ((button(x1+1+19*2,y1,x1+20+19*2,y1+19,cubetex, 57.0/256.0, 56.0/256.0, 76.0/256.0, 75.0/256.0, win.perspective,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.perspective=!win.perspective; waitleftbutton=true;}
		if ((button(x1+1+19*3,y1,x1+20+19*3,y1+19,cubetex, 57.0/256.0, 37.0/256.0, 76.0/256.0, 56.0/256.0, win.showhidden,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck)) { win.showhidden=!win.showhidden; waitleftbutton=true;}

		ValueBar(x1+20+19*3,y1,0,0.02f,1.5f,0.01f,&win.camsensitivity,"",0,0,base2,mode);
	}

	
	switchto2d();
	if (win.wireframe) {win.or=dotransformation2keyframer(win,x1,y1,x2,y2,xsize,ysize,mode);}
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

bool dragging=false;
int selected=-1;

void drawanimtimeline(int mode)
{
	actualanimlength=actualanimation->length;
	if (!leftbutton && dragging) dragging=false;
	if (!leftbutton || !dragging) selected=-1;
	int highlighted=-1;
	hscroller(148,542,788,557,15,15,actualanimlength+1,actualanimlength/timelinezoom,timelinestart,mode);
	if (mode==DD_Check)
	{
		if (leftclickinwindow(148,513,788,525))
		{
			currentframe=max(0,min(actualanimlength,(int)((float)(mx-148)/640.0f*(float)actualanimlength/(float)timelinezoom+timelinestart)));
			getcamerastate(actualcameraanim, actualcamera, (float)currentframe);
			getscenestate();
		}
		if (mouseinwindow(148,513,788,525))
		{
			currentframe=max(0,min(actualanimlength,currentframe+wheel));
			if (wheel!=0)
			{
				getcamerastate(actualcameraanim, actualcamera, (float)currentframe);
				getscenestate();
			}
		}
		if (mouseinwindow(148,541,788,556))	timelinestart=max(0,min(actualcameraanim->length*(timelinezoom-1)/timelinezoom,timelinestart+wheel));
		if (mouseinwindow(148,526,788,541))	timelinezoom=min(max(1,timelinezoom+wheel),(int)((float)actualcameraanim->length/40.0f));
	}
	if (mode==DD_Draw)
	{
		glColor4f(buttontextlit);
		rectangle(148,526,788,541);

		for (int x=0; x<=10; x++)
		{
			glRasterPos2i(x*62+149,523);
			sprintf(stri,"%.4d",timelinestart+x*actualanimlength/timelinezoom/10);
			glPrint(stri,basesmall);
		}
		if (currentframe>=timelinestart && currentframe<=timelinestart+actualanimlength/timelinezoom)
		{			
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_DEPTH_TEST);
			glColor4f(backgroundcol);
			int pos=148+(int)((float)(currentframe-timelinestart)/(float)((float)actualanimlength/(float)timelinezoom)*640.0f);
			quad(pos-10,513,pos+11,525);
			glColor4f(1,1,0,1);

			glBegin(GL_TRIANGLES);
				glVertex2i(pos,525);
				glVertex2i(pos-4,521);
				glVertex2i(pos+5,521);
			glEnd();

			glColor4f(1,1,1,1);
			glRasterPos2i(pos-9,520);
			sprintf(stri,"%.4d",currentframe);
			glPrint(stri,basesmall);
			glPopAttrib();
		}

		for (object *o=actualscene->objects;o;o=o->next)
		if (o->selected && !o->effect && !o->hidden)
		{
			actualanim=o->actualanim;
			for (x=0; x<actualanim->keyframenum; x++)
			{
				if (actualanim->keyframes[x].frame>=timelinestart && actualanim->keyframes[x].frame<=timelinestart+actualanimlength/timelinezoom)
				{
					int pos=(int)(148.0+(float)(actualanim->keyframes[x].frame-timelinestart)/(float)((float)actualanimlength/(float)timelinezoom)*640.0f);
					if (mouseinwindow(pos-5,526,pos+5,541))
					{
						o->highlightedkf=x;
					}
				}
				if (leftbutton && o->kfdragging) o->highlightedkf=o->selectedkf;
				if (actualanim->keyframes[x].frame>=timelinestart && actualanim->keyframes[x].frame<=timelinestart+actualanimlength/timelinezoom)
				{
					int pos=(int)(148.0+(float)(actualanim->keyframes[x].frame-timelinestart)/(float)((float)actualanimlength/(float)timelinezoom)*640.0f);
					if (o->highlightedkf==x) glColor4f(0,1,0,1); else glColor4f(buttontextlit);
					line(pos,527,pos,541);
					if (o->highlightedkf==x)
					{
						glColor4f(0,0.9f,0,1);
						if (pos!=148)
							line(pos-1,527,pos-1,541);
						if (pos!=788)
							line(pos+1,527,pos+1,541);
					}
				}
			}
		}
	}


	for (object *o=actualscene->objects;o;o=o->next)
	{
		if (!leftbutton && o->kfdragging) 
		{
			calculateobjectanim(o->actualanim);
			getobjectstate(o->actualanim, *o, (float)currentframe);
			o->kfdragging=false;
		}
		if (!leftbutton || !o->kfdragging) o->selectedkf=-1;
		o->highlightedkf=-1;
	}
	
	if (mode==DD_Check)
	{
		for (object *o=actualscene->objects;o;o=o->next)
		{

			for (x=0; x<o->actualanim->keyframenum; x++)
			{
				if (o->actualanim->keyframes[x].frame>=timelinestart && o->actualanim->keyframes[x].frame<=timelinestart+o->actualanim->length/timelinezoom)
				{
					int pos=(int)(148.0+(float)(o->actualanim->keyframes[x].frame-timelinestart)/(float)((float)o->actualanim->length/(float)timelinezoom)*640.0f);
					if (mouseinwindow(pos-5,526,pos+5,541))
					{
						o->highlightedkf=x;
					}
				}
				if (leftclickinwindow(148,526,788,541) && !o->kfdragging && o->highlightedkf!=-1 && o->selected)
				{
					o->kfdragging=true;
					o->selectedkf=o->highlightedkf;
				}
				if (o->kfdragging)
				{
					o->actualanim->keyframes[o->selectedkf].frame=max(0,min(o->actualanim->length,(int)((float)(mx-148)/640.0f*(float)o->actualanim->length/(float)timelinezoom+timelinestart)));
					sprintf(cursorstring,"(frame %d)",o->actualanim->keyframes[o->selectedkf].frame);
				}
			}
		}

	}
}


void drawcameratimeline(int mode)
{
	if (!leftbutton && dragging) 
	{
		calculatecameraanim(actualcameraanim);
		getcamerastate(actualcameraanim, actualcamera, (float)currentframe);
		keyframeviews[3].cam=actualcamera;
		dragging=false;
	}
	if (!leftbutton || !dragging) selected=-1;
	int highlighted=-1;
	hscroller(148,542,788,557,15,15,actualcameraanim->length+1,actualcameraanim->length/timelinezoom,timelinestart,mode);
	if (mode==DD_Draw)
	{
		glColor4f(buttontextlit);
		rectangle(148,526,788,541);

		for (int x=0; x<=10; x++)
		{
			glRasterPos2i(x*62+149,523);
			sprintf(stri,"%.4d",timelinestart+x*actualcameraanim->length/timelinezoom/10);
			glPrint(stri,basesmall);
		}

		for (x=0; x<actualcameraanim->keyframenum; x++)
		{
			if (actualcameraanim->keyframes[x].frame>=timelinestart && actualcameraanim->keyframes[x].frame<=timelinestart+actualcameraanim->length/timelinezoom)
			{
				int pos=(int)(148.0+(float)(actualcameraanim->keyframes[x].frame-timelinestart)/(float)((float)actualcameraanim->length/(float)timelinezoom)*640.0f);
				if (mouseinwindow(pos-5,526,pos+5,541))
				{
					highlighted=x;
				}
			}
			if (leftbutton && dragging) highlighted=selected;
			if (actualcameraanim->keyframes[x].frame>=timelinestart && actualcameraanim->keyframes[x].frame<=timelinestart+actualcameraanim->length/timelinezoom)
			{
				int pos=(int)(148.0+(float)(actualcameraanim->keyframes[x].frame-timelinestart)/(float)((float)actualcameraanim->length/(float)timelinezoom)*640.0f);
				if (highlighted==x) glColor4f(0,1,0,1); else glColor4f(buttontextlit);
				line(pos,527,pos,541);
				if (highlighted==x)
				{
					glColor4f(0,0.9f,0,1);
					if (pos!=148)
					line(pos-1,527,pos-1,541);
					if (pos!=788)
					line(pos+1,527,pos+1,541);
				}
			}
		}

		if (currentframe>=timelinestart && currentframe<=timelinestart+actualcameraanim->length/timelinezoom)
		{			
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_DEPTH_TEST);
			glColor4f(backgroundcol);
			int pos=(int)(148.0+(float)(currentframe-timelinestart)/(float)((float)actualcameraanim->length/(float)timelinezoom)*640.0f);
			quad(pos-10,513,pos+11,525);
			glColor4f(1,1,0,1);

			glBegin(GL_TRIANGLES);
				glVertex2i(pos,525);
				glVertex2i(pos-4,521);
				glVertex2i(pos+5,521);
			glEnd();

			glColor4f(1,1,1,1);
			glRasterPos2i(pos-9,520);
			sprintf(stri,"%.4d",currentframe);
			glPrint(stri,basesmall);
			glPopAttrib();
		}
	}
	if (mode==DD_Check)
	{
		for (x=0; x<actualcameraanim->keyframenum; x++)
		{
			if (actualcameraanim->keyframes[x].frame>=timelinestart && actualcameraanim->keyframes[x].frame<=timelinestart+actualcameraanim->length/timelinezoom)
			{
				int pos=(int)(148.0+(float)(actualcameraanim->keyframes[x].frame-timelinestart)/(float)((float)actualcameraanim->length/(float)timelinezoom)*640.0f);
				if (mouseinwindow(pos-5,526,pos+5,541))
				{
					highlighted=x;
				}
			}
			if (leftclickinwindow(148,526,788,541) && !dragging && highlighted!=-1)
			{
				dragging=true;
				selected=highlighted;
			}
			if (dragging)
			{
				actualcameraanim->keyframes[selected].frame=max(0,min(actualcameraanim->length,(int)((float)(mx-148)/640.0f*(float)actualcameraanim->length/(float)timelinezoom+timelinestart)));
				sprintf(cursorstring,"(frame %d)",actualcameraanim->keyframes[selected].frame);
			}
		}
		if (leftclickinwindow(148,513,788,525))
		{
			currentframe=max(0,min(actualcameraanim->length,(int)((float)(mx-148)/640.0f*(float)actualcameraanim->length/(float)timelinezoom+timelinestart)));
			getcamerastate(actualcameraanim, actualcamera, (float)currentframe);
			getscenestate();
			camchanged=false;
		}
		if (mouseinwindow(148,513,788,525))
		{
			currentframe=max(0,min(actualcameraanim->length,currentframe+wheel));
			if (wheel!=0)
			{
				getcamerastate(actualcameraanim, actualcamera, (float)currentframe);
				getscenestate();
				camchanged=false;
			}
		}
		if (mouseinwindow(148,541,788,556))
		{
			timelinestart=max(0,min(actualcameraanim->length*(timelinezoom-1)/timelinezoom,timelinestart+wheel));
		}
		if (mouseinwindow(148,526,788,541))
		{
			timelinezoom=min(max(1,timelinezoom+wheel),(int)((float)actualcameraanim->length/40.0f));
		}
	}
}

int cntcams(cameraanimation *c)
{
	int x=0;
	cameraanimation *s=c;
	while (s!=NULL)
	{
		x++;
		s=s->next;
	}
	return x;
}

int cntanims()
{
	int x=0;
	sceneobjanim *s=actualscene->anims;
	while (s!=NULL)
	{
		x++;
		s=s->next;
	}
	return x;
}


void calculateobjhierarchy(int parentid, object *parent)
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
			calculateobjhierarchy(o->objdata.number,o);
		}
}

void keyframergui(int mode)
{
	for (o=actualscene->objects;o;o=o->next)
	{
		for (int x=0; x<4; x++)
		{
			o->objdata.color[x]=(int)(o->keyframercol[x]);
			o->color[x]=o->keyframercol[x];
		}
	}
	keyframeviews[3].cam=actualcamera;
	if (!keyframeviews[0].fullscreen && !keyframeviews[1].fullscreen && !keyframeviews[2].fullscreen && !keyframeviews[3].fullscreen)
	{
		int x;
		for (x=0;x<=3;x++) displaykeyframewindow(keyframeviews[x],mode);
	}
	else
	{
		if (keyframeviews[0].fullscreen) displaykeyframewindow(keyframeviews[0],mode); else
		if (keyframeviews[1].fullscreen) displaykeyframewindow(keyframeviews[1],mode); else
		if (keyframeviews[2].fullscreen) displaykeyframewindow(keyframeviews[2],mode); else
		if (keyframeviews[3].fullscreen) displaykeyframewindow(keyframeviews[3],mode);
	}
	objkeyframedata a;
	if (mode==DD_Check)
	{
		a=keyframeviews[0].or;
		a.position.b.x+=keyframeviews[1].or.position.b.x;
		a.position.b.y+=keyframeviews[1].or.position.b.y;
		a.position.b.z+=keyframeviews[1].or.position.b.z;
		a.position.b.x+=keyframeviews[2].or.position.b.x;
		a.position.b.y+=keyframeviews[2].or.position.b.y;
		a.position.b.z+=keyframeviews[2].or.position.b.z;
		a.stretch.b.x*=keyframeviews[1].or.stretch.b.x;
		a.stretch.b.y*=keyframeviews[1].or.stretch.b.y;
		a.stretch.b.z*=keyframeviews[1].or.stretch.b.z;
		a.stretch.b.x*=keyframeviews[2].or.stretch.b.x;
		a.stretch.b.y*=keyframeviews[2].or.stretch.b.y;
		a.stretch.b.z*=keyframeviews[2].or.stretch.b.z;
		matrix r1,r2,r3;
		m_rotate(a.rotaxis.b.x,a.rotaxis.b.y,a.rotaxis.b.z,a.rotangle*(float)radtheta,r1);
		m_rotate(keyframeviews[1].or.rotaxis.b.x,keyframeviews[1].or.rotaxis.b.y,keyframeviews[1].or.rotaxis.b.z,keyframeviews[1].or.rotangle*(float)radtheta,r2);
		m_rotate(keyframeviews[2].or.rotaxis.b.x,keyframeviews[2].or.rotaxis.b.y,keyframeviews[2].or.rotaxis.b.z,keyframeviews[2].or.rotangle*(float)radtheta,r3);
		m_mult(r1,r2,r1);
		m_mult(r1,r3,r1);
		float rotangle;
		m_revrotate(r1,a.rotaxis,rotangle);
		a.rotangle=(short)(rotangle/radtheta);
		//a.rotangle=a.rotangle+keyframeviews[1].or.rotangle+keyframeviews[2].or.rotangle;
		for (object *o=actualscene->objects;o;o=o->next)
		{
			if (o->selected && !o->effect && !o->hidden)
			{
				if (transform)
				{
					o->changed=true;
					o->transform=a;
				}
				else
				{
					o->currentorientation.position.b.x+=o->transform.position.b.x;
					o->currentorientation.position.b.y+=o->transform.position.b.y;
					o->currentorientation.position.b.z+=o->transform.position.b.z;
					o->currentorientation.stretch.b.x*=o->transform.stretch.b.x;
					o->currentorientation.stretch.b.y*=o->transform.stretch.b.y;
					o->currentorientation.stretch.b.z*=o->transform.stretch.b.z;
					matrix mcurr,mtrans;
					m_rotate(o->currentorientation.rotaxis.b.x,o->currentorientation.rotaxis.b.y,o->currentorientation.rotaxis.b.z,o->currentorientation.rotangle*(float)radtheta,mcurr);
					m_rotate(o->transform.rotaxis.b.x,o->transform.rotaxis.b.y,o->transform.rotaxis.b.z,o->transform.rotangle*(float)radtheta,mtrans);
					m_mult(mcurr,mtrans,mcurr);
					float arotangle;
					m_revrotate(mcurr,o->currentorientation.rotaxis,arotangle);
					o->currentorientation.rotangle=(short)(arotangle/radtheta);
					memset(&o->transform,0,sizeof(o->transform));
					o->transform.stretch.b.x=1;
					o->transform.stretch.b.y=1;
					o->transform.stretch.b.z=1;
				}
			}

			matrix m,a,b;
			m_identity(a); m_identity(b);
			m_scale(o->currentorientation.stretch.b.x,o->currentorientation.stretch.b.y,o->currentorientation.stretch.b.z,a);
			m_scale(o->transform.stretch.b.x,o->transform.stretch.b.y,o->transform.stretch.b.z,b);
			m_mult(a,b,m);

			m_identity(a); m_identity(b);
			m_rotate(o->currentorientation.rotaxis.b.x,o->currentorientation.rotaxis.b.y,o->currentorientation.rotaxis.b.z,o->currentorientation.rotangle*(float)radtheta,a);
			m_rotate(o->transform.rotaxis.b.x,o->transform.rotaxis.b.y,o->transform.rotaxis.b.z,o->transform.rotangle*(float)radtheta,b);
			m_mult(a,b,a);
			m_mult(m,a,m);

			m_identity(a); m_identity(b);
			m_xpose(o->currentorientation.position.b.x,o->currentorientation.position.b.y,o->currentorientation.position.b.z,a);
			m_xpose(o->transform.position.b.x,o->transform.position.b.y,o->transform.position.b.z,b);
			m_mult(a,b,a);
			m_mult(m,a,o->currentmatrix);

			/*memcpy(o->buffermatrix,o->xformmatrix,sizeof(matrix));
			m_mult(o->xformmatrix,m,m);
			obj_transform(o,m);
			memcpy(o->xformmatrix,o->buffermatrix,sizeof(matrix));*/
			
		}
	}
	//calculateobjpositions(-1,NULL);
	calculateobjhierarchy(-1,NULL);
	switchto2d();
	if (keyframermenu==1) drawanimtimeline(mode);
	if (keyframermenu==2) drawcameratimeline(mode);
	if (button(262,560,290,583,texbutton2,171.0/256.0,19.0/256.0,199.0/256.0,42.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{ 
		currentframe=0; 
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		camchanged=false;
	}
	if (keyframermenu==2 && button(290,560,310,583,texbutton2,171.0/256.0,65.0/256.0,191.0/256.0,88.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		for (x=0;x<=actualcameraanim->keyframenum && actualcameraanim->keyframes[x].frame<currentframe;x++);
		if (currentframe>actualcameraanim->keyframes[actualcameraanim->keyframenum-1].frame) currentframe=actualcameraanim->keyframes[actualcameraanim->keyframenum-1].frame;
		else
		{
			currentframe=actualcameraanim->keyframes[max(0,x-1)].frame;
			if (x==0) currentframe=0;
		}
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		camchanged=false;
		waitleftbutton=true;
	}
	if (button(310,560,330,583,texbutton2,171.0/256.0,42.0/256.0,191.0/256.0,65.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		currentframe--; 
		if (currentframe<0) currentframe=0; 
		waitleftbutton=true;
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		camchanged=false;
	}
	if (button(330,560,350,583,texbutton2,171.0/256.0,88.0/256.0,191.0/256.0,111.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		if (playing)
		{
			playing=false;
			playstart=timeGetTime();
			currentframe=0;
			waitleftbutton=true;
			getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
			getscenestate();
			camchanged=false;
		}
	}
	if (!playing)
	{
		if (button(350,560,370,583,texbutton2,171.0/256.0,111.0/256.0,191.0/256.0,134.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		{
			playing=true;
			if (keyframermenu==2)
			playstart=timeGetTime()-(currentframe%actualcameraanim->length)*10;
			if (keyframermenu==1)
			playstart=timeGetTime()-(currentframe%actualanimation->length)*10;
			waitleftbutton=true;
		}
	}
	else
	{
		if (button(350,560,370,583,texbutton2,171.0/256.0,203.0/256.0,191.0/256.0,226.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		{
			playing=false;
			playstart=timeGetTime();
			waitleftbutton=true;
		}
	}
	if (button(370,560,390,583,texbutton2,171.0/256.0,157.0/256.0,191.0/256.0,180.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		currentframe++; 
		if (keyframermenu==2 && currentframe>actualcameraanim->length) currentframe=actualcameraanim->length; 
		if (keyframermenu==1 && currentframe>actualanimation->length) currentframe=actualanimation->length; 
		waitleftbutton=true;
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		camchanged=false;
	}
	if (keyframermenu==2 && button(390,560,410,583,texbutton2,171.0/256.0,134.0/256.0,191.0/256.0,157.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
	{
		for (x=0;x<=actualcameraanim->keyframenum && actualcameraanim->keyframes[x].frame<=currentframe;x++);
		//if (currentframe<actualcameraanim->keyframes[0].frame) currentframe=actualcameraanim->keyframes[actualcameraanim->keyframenum-1].frame;
		//else
		{
			currentframe=actualcameraanim->keyframes[min(actualcameraanim->keyframenum-1,x)].frame;
			if (x>=actualcameraanim->keyframenum) currentframe=actualcameraanim->length;
		}
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		camchanged=false;
		waitleftbutton=true;
	}
	if (button(410,560,438,583,texbutton2,171.0/256.0,180.0/256.0,199.0/256.0,203.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		if (keyframermenu==2)
		currentframe=actualcameraanim->length;
		if (keyframermenu==1)
		currentframe=actualanimation->length;
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		camchanged=false;
	}
	if (playing)
	{
		currentframe=(timeGetTime()-playstart)/10;
		if (currentframe>=actualcameraanim->length)
		{
			currentframe=actualcameraanim->length;
			playing=false;
		}
		getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		getscenestate();
		keyframeviews[3].cam=actualcamera;
	}
	keyframethere=false;
	for (x=0; x<actualcameraanim->keyframenum; x++)
	{
		if (actualcameraanim->keyframes[x].frame==currentframe) keyframethere=true;
	}
	objchanged=false;
	for (object *o=actualscene->objects;o;o=o->next) if (o->changed) objchanged=true;

	if (objchanged && keyframermenu==1)
	{
		if (button(148,560,177,589,texbutton3,0,80.0/256.0,29.0/256.0,109.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		for (object *o=actualscene->objects;o;o=o->next) if (o->changed && o->selected && !o->effect && !o->hidden)
		{
			keyframethere=false;
			for (x=0; x<o->actualanim->keyframenum; x++)
			if (o->actualanim->keyframes[x].frame==currentframe) keyframethere=true;
			if (!keyframethere)
			addkeyframe(o->actualanim);

			allocateobjecttracks(o->actualanim);
			objkeyframedata *c;
			if (!keyframethere)
				c=new objkeyframedata;
			else
			{
				for (x=0; x<o->actualanim->keyframenum; x++) if (currentframe==o->actualanim->keyframes[x].frame) c=(objkeyframedata*)o->actualanim->keyframes[x].data;
			}
			memset(c,0,sizeof(objkeyframedata));
			c->position=o->currentorientation.position;
			c->rotaxis=o->currentorientation.rotaxis;
			c->stretch=o->currentorientation.stretch;
			c->rotangle=o->currentorientation.rotangle;
			c->color[0]=o->keyframercol[0];
			c->color[1]=o->keyframercol[1];
			c->color[2]=o->keyframercol[2];
			c->color[3]=o->keyframercol[3];
			c->header.keyframetype=dd3d_objkeyframetype;
			if (!keyframethere)
			{
				o->actualanim->keyframes[o->actualanim->keyframenum-1].frame=currentframe;
				o->actualanim->keyframes[o->actualanim->keyframenum-1].data=c;
			}
			
			calculateobjectanim(o->actualanim);
			objchanged=false;
			o->changed=false;
		}
	}


	if (camchanged && keyframermenu==2)
	{
		if (button(148,560,177,589,texbutton3,0,80.0/256.0,29.0/256.0,109.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		{
			if (!keyframethere)
			addkeyframe(actualcameraanim);

			if (camchanged)
			{
				allocatecameratracks(actualcameraanim);
				camerakeyframe *c;
				if (!keyframethere)
				c=new camerakeyframe;
				else
				{
					for (x=0; x<actualcameraanim->keyframenum; x++) if (currentframe==actualcameraanim->keyframes[x].frame) c=(camerakeyframe*)actualcameraanim->keyframes[x].data;
				}
				memset(c,0,sizeof(camerakeyframe));
				c->eye=actualcamera.eye;
				c->target=actualcamera.target;
				c->fov=actualcamera.fov;
				c->roll=actualcamera.roll;
				c->header.keyframetype=dd3d_camerakeyframetype;
				if (!keyframethere)
				{
					actualcameraanim->keyframes[actualcameraanim->keyframenum-1].frame=currentframe;
					actualcameraanim->keyframes[actualcameraanim->keyframenum-1].data=c;
				}
				
			}

			calculatecameraanim(actualcameraanim);

			camchanged=false;
		}
	}
	if (keyframethere)
	if (keyframermenu==2)
	{
		if (button(177,560,206,589,texbutton3,0,109.0/256.0,29.0/256.0,138.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
		{
			y=0;
			for (x=0; x<actualcameraanim->keyframenum; x++)
			{
				if (actualcameraanim->keyframes[x].frame==currentframe) 
				{
					actualcameraanim->keyframes[x].frame=actualcameraanim->length*10;
					y++;
				}
			}
			sortkeyframes(actualcameraanim);
			actualcameraanim->keyframenum-=y;
			calculatecameraanim(actualcameraanim);
		}
	}

	if (keyframermenu==2)
	{
		if (actualcameraanim->keyframenum==0) sprintf(s,"NO KEYFRAMES");
		if (actualcameraanim->keyframenum==1) sprintf(s,"%d KEYFRAME",actualcameraanim->keyframenum);
		if (actualcameraanim->keyframenum>1) sprintf(s,"%d KEYFRAMES",actualcameraanim->keyframenum);
	}
	glRasterPos2i(320,590);
	glPrint(s,basesmall);

	if (button(27,31,46,50,cubetex,0,0,19.0/256.0,19.0/256.0,keyframermenu==0,mode) == DDgui_LeftClick) keyframermenu=0;
	if (button(47,31,66,50,cubetex,0,19.0/256.0,19.0/256.0,38.0/256.0,keyframermenu==1,mode) == DDgui_LeftClick) keyframermenu=1;
	if (button(67,31,86,50,texbutton2,171.0/256.0,0.0,190.0/256.0,19.0/256.0,keyframermenu==2,mode) == DDgui_LeftClick) keyframermenu=2;
	if (button(87,31,106,50,cubetex,0,57.0/256.0,19.0/256.0,76.0/256.0,keyframermenu==3,mode) == DDgui_LeftClick) keyframermenu=3;
	if (button(107,31,126,50,cubetex,0,114.0/256.0,19.0/256.0,133.0/256.0,keyframermenu==7,mode) == DDgui_LeftClick) keyframermenu=7;

	//if ()
	if (keyframermenu==2 && button(733,560,790,576,texbutton3,0,171.0/256.0,57.0/256.0,187.0/256.0,false,mode) == DDgui_LeftClick)
	{
		char *ss=readline("Enter New Max Frame Number",230,0,base,generatedtexture.texts[generatedtexture.commandnum].text);
		int newnum=0;
		if (sscanf(ss,"%d",&newnum))
		{
			if (newnum>0)
			{
				actualcameraanim->length=newnum;
			}
		}
	}
	if (keyframermenu==1 && button(733,560,790,576,texbutton3,0,171.0/256.0,57.0/256.0,187.0/256.0,false,mode) == DDgui_LeftClick)
	{
		char *ss=readline("Enter New Max Frame Number",230,0,base,generatedtexture.texts[generatedtexture.commandnum].text);
		int newnum=0;
		if (sscanf(ss,"%d",&newnum))
		{
			if (newnum>0)
			{
				for (object *o=actualscene->objects;o;o=o->next)
				{
					o->actualanim->length=newnum;
				}
				actualanimation->length=newnum;
			}
		}
	}

	switch (keyframermenu)
	{
	case 0: {
		break;
			}
	case 1: {
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		o=actualscene->objects;
		objnumber=objnum(o);

		if (button(703-650,115-30+165,748-650,131-30+165,mainmenutex,377.0/512.0,32.0/128.0,422.0/512.0,48.0/128.0,false,mode) == DDgui_LeftClick)
		{
			char *ss=readline("Enter Anim Name",270,0,base,"");
			if (strcmp(ss,""))
			{
				scene_addanim(actualscene);
				sprintf(actualscene->lastanim->name,"%s",ss);
				actualscene->lastanim->length=1000;
				for (object *o=actualscene->objects;o;o=o->next)
				{
					objectanimation *n=new objectanimation;
					memset(n,0,sizeof(objectanimation));
					n->number=actualscene->lastanim->number;
					n->keyframecapacity=10;
					n->keyframenum=0;
					n->length=1000;
					n->keyframes= new keyframe[10];
					o->lastanim->next=n;
					o->lastanim=n;
				}
			}
		}
		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			rectangle(661-650,120+150,775-650,264+150);
		}	  
		animselected=min(animselected,cntanims()-1);
		if (mode==DD_Check)
		if (leftclickinwindow(661-650,121+150,774-650,263+150) && mouseinwindow(661-650,121+150,774-650,260+150)) animselected=min(animstart+(my-121-150) / 10,cntanims()-1);
		if ((mode ==DD_Check) && mouseinwindow(661-650,121+150,774-650,263+150)) animstart-=wheel*4;
		scroller(775-650,120+150,790-650,264+150,15,15,cntanims(),14,animstart,mode);
		if (mode==DD_Draw)
		{
			sceneobjanim *pf = actualscene->anims;
			for (x=1;x<=animstart;x++) pf=pf->next;
			for (x=0;x<=13;x++)
			{
				if (pf!=NULL)
				{
					if (animstart+x==animselected) glColor4f(col4); else glColor4f(buttontextlit);
					glRasterPos2i(665-650,130+x*10+150);
					//strcpy(s,pf->name);
					sprintf(s,"%s (%d)",pf->name,pf->number);
					glPrint(s,base2,18);
					pf=pf->next;
				}
			}

			glColor4f(buttontextlit);
			sprintf(s,"%d ANIM(S).",cntanims());
			glRasterPos2i(683-650,277+150);
			glPrint(s,base2);

		}

		sceneobjanim *pf = actualscene->anims;
		for (x=1;x<=animselected;x++) pf=pf->next;
		if (actualanimation!=pf)
		{
			actualanimation=pf;
			//getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
			for (object *o=actualscene->objects;o;o=o->next)
			{
				o->actualanim=findobjanim(o,pf->number);
				getobjectstate(o->actualanim,*o,(float)currentframe);
			}
		}

		/*cameraanimation *pf = actualscene->cameraanims;
		for (x=1;x<=animselected;x++) pf=pf->next;
		if (actualcameraanim!=pf)
		{
			actualcameraanim=pf;
			getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
		}*/

		
		float colors[4];
		memset(colors,0,sizeof(float)*4);
		for (object *o=actualscene->objects;o;o=o->next)
		{
			if (o->selected)
			{
				for (int x=0; x<4; x++)	colors[x]=o->keyframercol[x];
				break;
			}
		}

		ValueBar(12,170,1,0.0f,255.0f,1.0f/255.0f,&colors[0],"",0,0,base2,mode);
		ValueBar(12,190,2,0.0f,255.0f,1.0f/255.0f,&colors[1],"",0,0,base2,mode);
		ValueBar(12,210,3,0.0f,255.0f,1.0f/255.0f,&colors[2],"",0,0,base2,mode);
		ValueBar(12,230,0,0.0f,255.0f,1.0f/255.0f,&colors[3],"",0,0,base2,mode);

		if (leftclickinwindow(12,170,143,250))
		{
			for (object *o=actualscene->objects;o;o=o->next)
			{
				if (o->selected)
				{
					for (int x=0; x<4; x++)	o->keyframercol[x]=colors[x];
				}
				o->changed=true;
			}
		}

		o=actualscene->objects;
		if (mode==DD_Check)
		{
			if (mouseinwindow(21,520,131,590)) selectstart-=wheel*2;
			if (leftclickinwindow(21,520,131,590) && mouseinwindow(21,520,131,594) && (objnumber!=0))
			{
				for (x=1;x<=min((int)(selectstart+(my-521) / 10),objnumber);x++) o=o->next;
				if (o!=NULL)
				{
					if (!o->effect)
						o->selected=!o->selected;
				}
				waitleftbutton=true;
			}

			if (rightclickinwindow(21,520,131,590) && mouseinwindow(21,520,131,594) && (objnumber!=0))
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
			if (button(1,518,20,537,cubetex,171.0/256.0,152.0/256.0,190.0/256.0,171.0/256.0,false,mode) == DDgui_LeftClick) { o=actualscene->objects; while (o!=NULL)  { if (!o->effect) o->selected=true; o=o->next; } }
			if (button(1,537,20,556,cubetex,171.0/256.0,171.0/256.0,190.0/256.0,190.0/256.0,false,mode) == DDgui_LeftClick) deselectall();
			if (button(1,556,20,575,cubetex,171.0/256.0,190.0/256.0,190.0/256.0,209.0/256.0,false,mode) == DDgui_LeftClick) { o=actualscene->objects; while (o!=NULL)  { if (!o->effect) o->selected=!o->selected; o=o->next; } waitleftbutton=true; }
		}
		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			rectangle( 21,520,131,594);
		}	  			
		o=actualscene->objects;
		scroller(131,520,146,594,15,15,objnumber,7,selectstart,mode);
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
					glRasterPos2i(22,530+x*10);
					sprintf(s,"%4d:%s (%d) (%d)",o->objdata.number,o->name,o->parent,o->actualanim->keyframenum);
					glPrint(s,base2);
					o=o->next;		
				}
			}
		}

		o=actualscene->objects;
		objnumber=objnum(o);

		if (mode==DD_Check)
		{
			if (mouseinwindow(21,120-50,131,190-50)) hierarchystart-=wheel*2;
			if (leftclickinwindow(21,120-50,131,190-50) && mouseinwindow(21,120-50,131,190-50) && (objnumber!=0))
			{
				hierarchyselected=min((int)(hierarchystart+(my-(121-50)) / 10),objnumber);
			}
		}
		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			rectangle( 21,120-50,131,194-50);
		}	  			
		o=actualscene->objects;
		scroller(131,120-50,146,194-50,15,15,objnumber,7,hierarchystart,mode);
		for (x=1;x<=hierarchystart;x++) o=o->next;
		if (mode==DD_Draw)
		{
			for (x=0;x<=6;x++)
			{
				if (o!=NULL)
				{
					if (x+hierarchystart==hierarchyselected) glColor4f(col4); else glColor4f(buttontextlit);
					glRasterPos2i(22,130+x*10-50);
					sprintf(s,"%4d:%s",o->objdata.number,o->name);
					glPrint(s,base2);
					o=o->next;		
				}
			}
		}

		if (actualscene->objects!=NULL)
		if (button(21,150,131,165,"Set Parent",26,10,base2,false,mode) == DDgui_LeftClick)
		{
			o=actualscene->objects;
			for (x=0;x<hierarchyselected;x++) o=o->next;
			if (o!=NULL)
			{
				for (object *l=actualscene->objects;l;l=l->next)
					if (l->selected && !l->effect && !l->hidden)
				{
					if (l->objdata.number!=o->objdata.number) 
					{
						l->parent=o->objdata.number;
					}
				}
			}
		}
		if (button(21,166,131,181,"No Parent",26,10,base2,false,mode) == DDgui_LeftClick)
		{
			o=actualscene->objects;
			for (x=0;x<hierarchyselected;x++) o=o->next;
			if (o!=NULL)
			{
				for (object *l=actualscene->objects;l;l=l->next)
					if (l->selected && !l->effect && !l->hidden)
				{
					//if (l->objdata.number!=o->objdata.number) 
					{
						l->parent=-1;
					}
				}
			}
		}

		
		glPopAttrib();
		break;
		}
	case 2: {
		sprintf(stri,"ROLL: %.3f",actualcamera.roll*360.0/255.0);
		ValueBar(10,280,0,-255.0f,255.0f,255.0f/36000.0f,&actualcamera.roll,stri,32,13,base2,mode);
		sprintf(stri,"FOV: %.3f",actualcamera.fov);
		ValueBar(10,300,0,1.0f,179.0f,1.0f,&actualcamera.fov,stri,34,13,base2,mode);
		if (leftclickinwindow(10,280,141,320)) camchanged=true;
		if (button(703-654,115-50,760-654,130-50,texbutton3,0,187.0/256.0,57.0/256.0,202.0/256.0,false,mode) == DDgui_LeftClick)
		{
			actualcamera=modellviews[3].cam;
			keyframeviews[3].cam=modellviews[3].cam;
			camchanged=true;
		}
		if (button(703-650,115-30,748-650,131-30,mainmenutex,377.0/512.0,32.0/128.0,422.0/512.0,48.0/128.0,false,mode) == DDgui_LeftClick)
		{
			cameraanimation *nscene = new cameraanimation;
			memset(nscene,0,sizeof(cameraanimation));
			nscene->next=NULL;
			nscene->length=1000;
			nscene->number=0;
			nscene->keyframecapacity=10;
			nscene->keyframenum=0;
			nscene->keyframes= new keyframe[10];
			nscene->number=actualscene->lastcamanim->number+1;
			actualscene->lastcamanim->next=nscene;
			actualscene->lastcamanim=nscene;
			char *ss=readline("Enter Camera Name",270,0,base,"");
			sprintf(nscene->name,"%s",ss);
		}
		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			rectangle(661-650,120,775-650,264);
		}	  
		camselected=min(camselected,cntcams(actualscene->cameraanims)-1);
		if (mode==DD_Check)
		if (leftclickinwindow(661-650,121,774-650,263) && mouseinwindow(661-650,121,774-650,260)) camselected=min(campos+(my-121) / 10,cntcams(actualscene->cameraanims)-1);
		if ((mode ==DD_Check) && mouseinwindow(661-650,121,774-650,263)) campos-=wheel*4;
		scroller(775-650,120,790-650,264,15,15,cntcams(actualscene->cameraanims),14,campos,mode);
		if (mode==DD_Draw)
		{
			cameraanimation *pf = actualscene->cameraanims;
			for (x=1;x<=campos;x++) pf=pf->next;
			for (x=0;x<=13;x++)
			{
				if (pf!=NULL)
				{
					if (campos+x==camselected) glColor4f(col4); else glColor4f(buttontextlit);
					glRasterPos2i(665-650,130+x*10);
					strcpy(s,pf->name);
					glPrint(s,base2,18);
					pf=pf->next;
				}
			}

			glColor4f(buttontextlit);
			sprintf(s,"%d CAMERA(S).",cntcams(actualscene->cameraanims));
			glRasterPos2i(683-650,277);
			glPrint(s,base2);

		}
		cameraanimation *pf = actualscene->cameraanims;
		for (x=1;x<=camselected;x++) pf=pf->next;
		if (actualcameraanim!=pf)
		{
			actualcameraanim=pf;
			getcamerastate(actualcameraanim,actualcamera,(float)currentframe);
			//modellviews[3].cam=actualcam->editview;
			//modellviews[3].cam2=actualcam->editview;
		}
		break;
			}
	}
	for (o=actualscene->objects;o;o=o->next)
	{
		for (x=0; x<4; x++)
		{
			o->objdata.color[x]=(int)o->colbuf[x];
			o->color[x]=o->colbuf[x];
		}
	}
}