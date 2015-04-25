#include "introeditor.h"

char str[256];
int itimelinestart=0;
int itimelinezoom=1;
int itimelinelength=60000;
int startpass=0,startp=0;
int maxpassnum=0;
int passlist[5];
event *eventlist=NULL;
event *selectedevent=NULL;
int passcounter=0;
bool framedisiplay=false;
bool neweventadd=false;
int neweventstart=0,neweventend=0,eventpass=0;
int icurrentframe=0;
bool iplaying=false;
int iplaystart=0;
bool moving=false;
int oldstart,oldend;
bool justcreated=false;
bool musicmenu=false;
int musselected=0, musbarpos=0;
bool playstarted=false;
int pausestart=0;
int pausesize=0;
bool leftdrag=false, rightdrag=false;
bool syncloaded=false;
int synclength=0;
int patternstarts[256];
int syncbarpos=0,syncselected=0;
int startpattern=0;

bool rulers=false;
bool rulermenu=false;
int xruler1=96,xruler2=384,yruler1=128,yruler2=512;
bool x1move=false,y1move=false,x2move=false,y2move=false;
int x1old=0,y1old=0,x2old=0,y2old=0;

scrshot shots[20];

int cntcamanims(cameraanimation *scenelist)
{
	int x=0;
	cameraanimation *s=scenelist;
	while (s!=NULL)
	{
		x++;
		s=s->next;
	}
	return x;
}

int cntanimanims(sceneobjanim *scenelist)
{
	int x=0;
	sceneobjanim *s=scenelist;
	while (s!=NULL)
	{
		x++;
		s=s->next;
	}
	return x;
}

void addevent(int pass, int start, int end)
{
	bool found=false;
	int cnt=0;
	for (event *e=eventlist; e; e=e->next) { if (e->pass==pass) found=true; cnt++;}
	if (!found) passcounter++;
	event *newevent=new event;
	memset(newevent,0,sizeof(event));
	sprintf(newevent->name,"#%d",cnt+1);
	newevent->pass=pass;
	newevent->startframe=max(0,start);
	newevent->endframe=min(itimelinelength,end);
	newevent->next=eventlist;
	newevent->blendfunc1=GL_ONE;
	newevent->blendfunc2=GL_ONE;
	newevent->startrectx=0.5;
	newevent->startrecty=0.5;
	newevent->startrectxrez=1;
	newevent->startrectyrez=1;
	newevent->startrectx1=0;
	newevent->startrecty1=0;
	newevent->startrectx2=639;
	newevent->startrecty2=479;
	newevent->endrectx=0.5;
	newevent->endrecty=0.5;
	newevent->endrectxrez=1;
	newevent->endrectyrez=1;
	newevent->endrectx1=0;
	newevent->endrecty1=0;
	newevent->endrectx2=639;
	newevent->endrecty2=479;
	newevent->camerastart=0;
	newevent->cameraend=1;
	newevent->animstart=0;
	newevent->animend=1;
	eventlist=newevent;	
}

#define GRIDXSIZE 64
#define GRIDYSIZE 48
float sqrr(float n) { return n*n; }
double offset[GRIDXSIZE+1][GRIDYSIZE+1][2];	// offset

void InitEffect(float u1, float v1, float u2, float v2) {
	int i,j;
	double o,p;
	double reli, relj;

	for (i=0;i<=GRIDXSIZE;i++) {
		for (j=0;j<=GRIDYSIZE;j++) {

			reli=(float)i/GRIDXSIZE;
			relj=(float)j/GRIDYSIZE;
			o=(rand()-16384)/163840.0;
			p=(u2-u1)*reli+u1;
			if ((o+p)<u1) { o=p-u1; }
			if ((o+p)>u2) { o=p-u2; }
			offset[i][j][0]=o;

			o=(rand()-16384)/163840.0;
			p=(v2-v1)*relj+v1;
			if ((o+p)<v1) { o=p-v1; }
			if ((o+p)>v2) { o=p-v2; }
			offset[i][j][1]=o;

			//offset[i][j][1]=0.0;//rand()-16384)/163840.0;
		}
	}
}

void initintroeditor()
{
	InitEffect( 0, 1.0f-480.0f/1024.0f, 640.0f/1024.0f, 1);
	byte *n=new byte[256*256*4];
	memset(n,0,256*256*4);
	for (int x=0; x<20; x++)
	{
		glGenTextures(1,&shots[x].texture);
		glBindTexture(GL_TEXTURE_2D,shots[x].texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,4,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,n);
	}
	/*addevent(0,500,10000);
	addevent(10,800,3000);
	addevent(10,8000,13000);
	addevent(27,1000,17000);
	addevent(32,8700,15000);
	addevent(32,3000,7000);
	addevent(16,300,900);
	addevent(16,1500,7000);
	addevent(23,700,9365);*/
}

void drawgrid(float grid[GRIDXSIZE+1][GRIDYSIZE+1][2], float x1, float y1, float x2, float y2)
{
				for (int i=0;i<GRIDXSIZE;i++) {
					for (int j=0;j<GRIDYSIZE;j++) {
						float _x1=(x2-x1)*((float)i/GRIDXSIZE)+x1;
						float _x2=(x2-x1)*((float)(i+1)/GRIDXSIZE)+x1;
						float _y1=(y2-y1)*((float)j/GRIDYSIZE)+y1;
						float _y2=(y2-y1)*((float)(j+1)/GRIDYSIZE)+y1;
						glBegin(GL_QUADS);
							glTexCoord2f(grid[i  ][j  ][0],grid[i  ][j  ][1]);
							glVertex2f(_x1,_y1);
							glTexCoord2f(grid[i+1][j  ][0],grid[i+1][j  ][1]);
							glVertex2f(_x2,_y1);
							glTexCoord2f(grid[i+1][j+1][0],grid[i+1][j+1][1]);
							glVertex2f(_x2,_y2);
							glTexCoord2f(grid[i  ][j+1][0],grid[i  ][j+1][1]);
							glVertex2f(_x1,_y2);
						glEnd();
					}
				}
}

void MakeEffect(int fxtype, float time, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2,
				int i1, int i2, int i3, int i4, float f1, float f2, float f3, float f4) {

	int i,j;			// looper
	float reli,relj;		// normalized loop values
	float reli2,relj2;
	float angle;
	float u,v;
	float _x1,_y1,_x2,_y2,_z;	// temp screen coords
	float deltau=(u2-u1)/GRIDXSIZE;
	float deltav=(v2-v1)/GRIDYSIZE;
	float deltax=(x2-x1)/GRIDXSIZE;
	float deltay=(y2-y1)/GRIDYSIZE;

	float f;				// temp shit

	float grid[GRIDXSIZE+1][GRIDYSIZE+1][2];	// da mighty grid :)

	if (time<0) { time=0; }
	if (time>1.0) { time=1.0; }
	u1+=0.002;
	u2-=0.002;
	v1-=0.002;
	v2+=0.002;

	switch (fxtype) {
	case 0: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						u=(u2-u1)*reli+u1 + (sin(time*2+reli*20+relj*15)*0.05 + cos(time*2+reli*10+relj*25)*0.05) * sqrr(sin(time*pi*0.5));
						if (u<u1) { u=u1; }
						if (u>u2) { u=u2; }
						v=(v2-v1)*relj+v1 + (sin(time*2+reli*18+relj*25)*0.05 + sin(time*2+reli*28+relj*15)*0.05) * sqrr(sin(time*pi*0.5));
						if (v>v1) { v=v1; }
						if (v<v2) { v=v2; }
						grid[i][j][0]=u;
						grid[i][j][1]=v;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);


				break;
			}

	case 1: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						f=(float)sqrt(sqrr(reli-0.5)+sqrr(relj-0.5))*2.0;
						f=pow(f,time*3.5f);
						f*=sqrr(sin(time*pi));
						grid[i][j][0]=((u2-u1)*reli+u1-0.5*(u1+u2))*(1-f)+0.5*(u1+u2);
						grid[i][j][1]=((v2-v1)*relj+v1-0.5*(v1+v2))*(1-f)+0.5*(v1+v2);
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 2: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						f=sqrt(sqrr(reli-0.5)+sqrr(relj-0.5))*2;
						if (f>0.001) {
						f=pow(f,1/(4*sqrr(sin(time*pi))+1)-1);
						//f=1;
						//f=(1-f)*sqr(sin(time*pi))+f*(1-sqr(sin(time*pi)));
						} else { f=0; }
						grid[i][j][0]=((u2-u1)*reli+u1-0.5*(u1+u2))*(f)+0.5*(u1+u2);
						grid[i][j][1]=((v2-v1)*relj+v1-0.5*(v1+v2))*(f)+0.5*(v1+v2);
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 3: {
				
				time *=3.6;
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						u=(u2-u1)*reli+u1 + sqrr(sin(time*pi/180*50))*offset[i][j][0];
						if (u<u1) { u=u1; }
						if (u>u2) { u=u2; }
						v=(v2-v1)*relj+v1 + sqrr(sin(time*pi/180*50))*offset[i][j][1];
						if (v>v1) { v=v1; }
						if (v<v2) { v=v2; }
						grid[i][j][0]=u;
						grid[i][j][1]=v;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 4: {
				time *=3.6;
				
				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						grid[i][j][0]=(u2-u1)*reli+u1 + sqrr(sin(time*pi/180*50))*offset[i][j][0];
						grid[i][j][1]=(v2-v1)*relj+v1 + sqrr(sin(time*pi/180*50))*offset[i][j][1];
					}
				}

				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {
						_x1=(x2-x1)*((float)i/GRIDXSIZE)+x1;
						_x2=(x2-x1)*((float)(i+1)/GRIDXSIZE)+x1;
						_y1=(y2-y1)*((float)j/GRIDYSIZE)+y1;
						_y2=(y2-y1)*((float)(j+1)/GRIDYSIZE)+y1;
						glBegin(GL_QUADS);
							glTexCoord2f(grid[i][j][0],grid[i][j][1]);
							glVertex2f(_x1,_y1);
							glTexCoord2f(grid[i][j][0]+deltau,grid[i][j][1]);
							glVertex2f(_x2,_y1);
							glTexCoord2f(grid[i][j][0]+deltau,grid[i][j][1]+deltav);
							glVertex2f(_x2,_y2);
							glTexCoord2f(grid[i][j][0],grid[i][j][1]+deltav);
							glVertex2f(_x1,_y2);
						glEnd();
					}
				}

				break;
			}
	case 5: {
				time *=3.6;
				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						grid[i][j][0]=(u2-u1)*reli+u1;
						grid[i][j][1]=(v2-v1)*relj+v1;
					}
				}

				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {
						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();
						_x1=(x2-x1)*((float)(i+0.5)/GRIDXSIZE)+x1 + sqrr(sin(time*pi/180*50))*offset[i][j][0]*20;
						_y1=(y2-y1)*((float)(j+0.5)/GRIDYSIZE)+y1 + sqrr(sin(time*pi/180*50))*offset[i][j][1]*20;
						_z=sqrr(sin(time*pi/180*50))*offset[i][j][1]*10+sqrr(sin(time*pi/180*50))*offset[i][j][0]*10;
						glRotatef(time*180/3.6,0,1,0);
						glTranslatef(_x1,_y1,_z);
						glRotatef(time*400,0,0,1);
						glBegin(GL_QUADS);
							glTexCoord2f(grid[i][j][0],grid[i][j][1]);
							glVertex2f(-0.5*deltax,-0.5*deltay);
							glTexCoord2f(grid[i][j][0]+deltau,grid[i][j][1]);
							glVertex2f( 0.5*deltax,-0.5*deltay);
							glTexCoord2f(grid[i][j][0]+deltau,grid[i][j][1]+deltav);
							glVertex2f( 0.5*deltax, 0.5*deltay);
							glTexCoord2f(grid[i][j][0],grid[i][j][1]+deltav);
							glVertex2f(-0.5*deltax, 0.5*deltay);
						glEnd();
					}
				}

				break;
			}
	case 6: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						reli2=reli-0.5;
						relj2=relj-0.5;
						f=sqrt(sqrr(reli2)+sqrr(relj2));

						if (f>0.01) {
							angle=atan(relj2/reli2);
						} else {
							angle=0;
						}
						if (reli2<0) { angle+=pi; }
						if (f<0.5) {
							if (i1) {
								angle+=25*sqrr(0.5-f)*sqrr(sin(time*pi*0.5));
							} else {
								angle-=25*sqrr(0.5-f)*sqrr(sin(time*pi*0.5));
							}
						}

						reli=f*cos(angle)+0.5;
						relj=f*sin(angle)+0.5;

						grid[i][j][0]=(u2-u1)*reli+u1;
						grid[i][j][1]=(v2-v1)*relj+v1;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 7: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						reli2=reli-0.5;
						relj2=relj-0.5;
						f=sqrt(sqrr(reli2)+sqrr(relj2));

						if (f>0.01) {
							angle=atan(relj2/reli2);
						} else {
							angle=0;
						}
						if (reli2<0) { angle-=pi; }

						f-=sqrr(sin(time*pi/180*400+f*10))*0.2 * sqrr(sin(time*pi/180*180));
						

						reli=f*cos(angle)+0.5;
						relj=f*sin(angle)+0.5;

						grid[i][j][0]=(u2-u1)*reli+u1;
						grid[i][j][1]=(v2-v1)*relj+v1;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 8: {
				
						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();
						glRotatef(time*180,0,1,0);
						glBegin(GL_QUADS);
							glTexCoord2f(u1,v1);
							glVertex3f(x1,y1,0);
							glTexCoord2f(u2,v1);
							glVertex3f(x2,y1,0);
							glTexCoord2f(u2,v2);
							glVertex3f(x2,y2,0);
							glTexCoord2f(u1,v2);
							glVertex3f(x1,y2,0);
						glEnd();

				break;
			}
	}

}

void stop() {
	if (!FMUSIC_PlayingSong) return;
	FMUSIC_StopSong(mod);
	FSOUND_Close();
}

void play(int ord) {
	if (FMUSIC_PlayingSong) stop();
	FSOUND_Init(44100, 0);
	FMUSIC_PlaySong(mod,ord);
}

void timeline(int mode)
{
	if (!leftbutton) justcreated=false;
	if (button(1,511,11,519,"",0,0,base,framedisiplay,mode)==DDgui_LeftClick && mode!=DD_AfterCheck) {framedisiplay=!framedisiplay; waitleftbutton=true;}
	if (mode==DD_Check && mouseinwindow(12,528-8,652,593-7)) itimelinezoom=max(1,itimelinezoom+wheel);
	hscroller(12,585,652,600,15,15,itimelinelength,itimelinelength/itimelinezoom,itimelinestart,mode);
	scroller(652,510,668,600,15,15,passcounter+1,5,startp,mode);
	if (mode==DD_Draw)
	{
		glColor4f(buttontextlit);
		rectangle(12,528-8,652,593-7);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDepthMask(false);
		for (int x=0; x<11; x++)
		{
			glColor4f(buttontextlit);
			if (!framedisiplay)
			{
				sprintf(str,"%d:%.2f",(int)((((float)itimelinestart+(float)itimelinelength/(float)itimelinezoom/10.0*(float)x)/100.0))/60,
					(int)((((float)itimelinestart+(float)itimelinelength/(float)itimelinezoom/10.0*(float)x)))%6000/100.0);
				glRasterPos2d(13+60.5*x,521-4);
				glPrint(str,basesmall);
			}
			else
			{
				sprintf(str,"%.5d",itimelinestart+(int)((float)itimelinelength/(float)itimelinezoom/10.0*(float)x));
				glRasterPos2d(13+61.4*x,521-4);
				glPrint(str,basesmall);
			}
			glColor4f(50.0f/300.0f,65.0f/300.0f,89.0f/300.0f,0);
			line(12+64*x,521,12+64*x,585);
		}
		glPopAttrib();
		glColor4f(0,0,0,0);
		for (x=1; x<5; x++)
		{
			line(12,520+x*13,652,520+x*13);
		}
	}
	startpass=-1;
	for (y=0; y<=startp; y++)
	{
		int buf=255;
		for (event *e=eventlist; e; e=e->next)
		{
			if (e->pass>startpass && e->pass<buf) buf=e->pass;
		}
		startpass=buf;
	}
	for (y=0; y<5; y++)
	{
		passlist[y]=99;
		for (event *e=eventlist; e; e=e->next)
		{
			if (y!=0)
			{
				if (e->pass<passlist[y] && e->pass>passlist[y-1])
					passlist[y]=e->pass;
			}
			else
			{
				if (e->pass<passlist[y] && e->pass>=startpass) passlist[y]=e->pass;
			}
		}
	}
	if (mode==DD_Draw)
	{
		glColor4f(buttontextlit);
		for (int x=0; x<5; x++)
		{
			sprintf(str,"%.2d",passlist[x]);
			glRasterPos2i(1,533+x*13-4);
			glPrint(str,basesmall);
		}
	}
	for (event *e=eventlist; e; e=e->next)
	{
		for (int x=0; x<5; x++)
		{
			if (passlist[x]==e->pass)
			{
				if (e->startframe<=itimelinestart+itimelinelength/itimelinezoom && e->endframe>=itimelinestart)
				{
					int p1=(int)((float)(e->startframe-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0);
					int p2=(int)((float)(e->endframe-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0);
					if (e==selectedevent)
					{
						if (p1<=640 && p1>=0)
						{
							if (mouseinwindow(max(12,p1-3+12),511,min(652,p1+3+12),520) || leftdrag)
							{
								glColor4f(0,1,0,0);
								line(p1+12,511,p1+12,520);
								glColor4f(0,0.8f,0,0);
								line(p1+11,511,p1+11,520);
								line(p1+13,511,p1+13,520);
							}
							else
							{
								glColor4f(buttontextlit);
								line(p1+12,511,p1+12,520);
							}						
							if (leftclickinwindow(max(12,p1-3+12),511,min(652,p1+3+12),520))
							{
								oldstart=e->startframe;
								oldend=e->endframe;
								leftdrag=true;
							}
						}
						if (!leftbutton)
						{
							leftdrag=false;
							rightdrag=false;
						}
						if (p2<=640 && p2>=0)
						{
							if (mouseinwindow(max(12,p2-3+12),511,min(652,p2+3+12),520) || rightdrag)
							{
								glColor4f(0,1,0,0);
								line(p2+12,511,p2+12,520);
								glColor4f(0,0.8f,0,0);
								line(p2+11,511,p2+11,520);
								line(p2+13,511,p2+13,520);
							}
							else
							{
								glColor4f(buttontextlit);
								line(p2+12,511,p2+12,520);
							}
							if (leftclickinwindow(max(12,p2-3+12),511,min(652,p2+3+12),520))
							{
								oldstart=e->startframe;
								oldend=e->endframe;
								rightdrag=true;
							}
						}
						if ((leftdrag && rightdrag)) {rightdrag=false;}
						{
							int minstart=-1,maxend=itimelinelength+1;
							for (event *a=eventlist;a;a=a->next)
							{
								if (a->pass==selectedevent->pass && a->endframe<oldstart && a->endframe>minstart) minstart=a->endframe;
								if (a->pass==selectedevent->pass && a->startframe>oldend && a->startframe<maxend) maxend=a->startframe;
							}
							minstart++;
							maxend--;
							if (leftdrag)
							{
								e->startframe=min(e->endframe-1,max(minstart,(int)((float)((float)((mx)-12)/(float)itimelinezoom)/640.0*(float)itimelinelength)+itimelinestart));
								p1=(int)((float)(e->startframe-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0);
								sprintf(cursorstring,"Frame %d",e->startframe);
							}
							if (rightdrag)
							{
								e->endframe=max(e->startframe+1,min(maxend,(int)((float)((float)((mx)-12)/(float)itimelinezoom)/640.0*(float)itimelinelength)+itimelinestart));
								p2=(int)((float)(e->endframe-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0);
								sprintf(cursorstring,"Frame %d",e->endframe);
							}
						}
					}
					int pos1=max(p1,0);
					int pos2=min(p2,640);
					if (!justcreated)
					{
						if (!moving && leftclickinwindow(pos1+13,521+x*13,pos2+11,520+x*13+13) && e==selectedevent)
						{
							moving=true;
							oldstart=e->startframe;
							oldend=e->endframe;
						}
						if (moving && e==selectedevent)
						{
							int newstart,newend;
							int minstart=-1,maxend=itimelinelength+1;
							for (event *a=eventlist;a;a=a->next)
							{
								if (a->pass==selectedevent->pass && a->endframe<oldstart && a->endframe>minstart) minstart=a->endframe;
								if (a->pass==selectedevent->pass && a->startframe>oldend && a->startframe<maxend) maxend=a->startframe;
							}
							minstart++; maxend--;
							newstart=(int)(oldstart+(float)(mx-lx)/640.0f*(float)itimelinelength/(float)itimelinezoom);
							newend=(int)(oldend+(float)(mx-lx)/640.0f*(float)itimelinelength/(float)itimelinezoom);
							if (newstart<minstart) {newstart=minstart; newend=minstart+oldend-oldstart;}
							if (newend>maxend) {newend=maxend; newstart=maxend-oldend+oldstart;}

							pos1=max((int)((float)(newstart-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0),0);
							pos2=min((int)((float)(newend-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0),640);
							if (!leftbutton)
							{
								e->startframe=newstart;
								e->endframe=newend;
								moving=false;
							}
						}
					}

					sprintf(str,"%s",e->name);
					if (e->eventtype==layer2d)
					sprintf(str,"%s:%s",e->name,"2D");
					if (e->eventtype==layer3d)
					sprintf(str,"%s:%s",e->name,"3D");
					if (e->eventtype==cleargl)
					sprintf(str,"%s:%s",e->name,"clr");
					if (e->eventtype==rendertotext)
					sprintf(str,"%s:%s",e->name,"r2txt");
					if (e->eventtype==feedback)
					sprintf(str,"%s:%s",e->name,"feedback");
					if (e->eventtype==grideffect)
					{
						switch (e->effect)
						{
							case 0:
								sprintf(str,"%s:%s",e->name,"sine");
								break;
							case 1:
								sprintf(str,"%s:%s",e->name,"glass");
								break;
							case 2:
								sprintf(str,"%s:%s",e->name,"fovzoom");
								break;
							case 3:
								sprintf(str,"%s:%s",e->name,"ripple");
								break;
							case 4:
								sprintf(str,"%s:%s",e->name,"disintegrate");
								break;
							case 5:
								sprintf(str,"%s:%s",e->name,"blow");
								break;
							case 6:
								sprintf(str,"%s:%s",e->name,"twirl");
								break;
							case 7:
								sprintf(str,"%s:%s",e->name,"wave");
								break;
							case 8:
								sprintf(str,"%s:%s",e->name,"rotate");
								break;
						}
					}

					if (button(pos1+13,521+x*13,pos2+11,520+x*13+13,str,3,9,base2,selectedevent==e,mode)==DDgui_LeftClick && !moving) { selectedevent=e; musicmenu=false; rulermenu=false; justcreated=true;}
				}
			}
		}
	}
	if (rightclickinwindow(12,528-8,652,593-7))
	{
		neweventadd=true;
		int pos1=(int)((float)((float)(rx-12)/(float)itimelinezoom)/640.0*(float)itimelinelength)+itimelinestart;
		int pos2=(int)((float)((float)(mx-12)/(float)itimelinezoom)/640.0*(float)itimelinelength)+itimelinestart;
		sprintf(cursorstring,"Frame: %d",min(max(0,pos2),itimelinelength));
		eventpass=passlist[(ry-520)/13];
		if (eventpass==99)
		{
			int eventmax=-1;
			for (event *e=eventlist; e; e=e->next)
			{
				if (e->pass>eventmax) eventmax=e->pass;
			}
			eventpass=eventmax+1;
		}
		neweventstart=min(pos1,pos2);
		neweventend=max(pos1,pos2);
		if (mode==DD_Draw)
		{
			int p=(ry-520)/13;
			line(rx,520+p*13,rx,520+(p+1)*13);
			if (mx>12 && mx<652)
			line(mx,520+p*13,mx,520+(p+1)*13);
		}
	}
	else
	{
		if (neweventadd)
		{
			event *e;
			for (e=eventlist; e; e=e->next)
			{
				if (e->pass==eventpass && e->startframe>=neweventstart && e->endframe<=neweventend) neweventadd=false;
				if (e->pass==eventpass && e->startframe<=neweventstart && e->endframe>=neweventend) neweventadd=false;
			}

			if (neweventadd)
			{
				for (e=eventlist; e; e=e->next)
				{
					if (e->pass==eventpass && neweventstart<=e->endframe && neweventstart>=e->startframe) neweventstart=e->endframe+1;
					if (e->pass==eventpass && neweventend>=e->startframe && neweventend<=e->endframe) neweventend=e->startframe-1;
				}
				neweventadd=false;
				if (neweventstart!=neweventend)
				addevent(eventpass,neweventstart,neweventend);
				justcreated=true;
			}
		}
	}
	if (mode==DD_Draw)
	{
		glColor4f(1,1,1,1);
		if (icurrentframe>=itimelinestart && icurrentframe<=itimelinestart+(float)itimelinelength/(float)itimelinezoom)
		{
			int pos1=(int)((float)(icurrentframe-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0);
			line(pos1+12,511,pos1+12,585);
		}
		glColor4f(0.0,1.0,1.0,1.0);
		if (patternstarts[startpattern]>=itimelinestart && patternstarts[startpattern]<=itimelinestart+(float)itimelinelength/(float)itimelinezoom)
		{
			int pos1=(int)((float)(patternstarts[startpattern]-itimelinestart)/(float)itimelinelength*(float)itimelinezoom*640.0);
			line(pos1+12,511,pos1+12,585);
		}
		glColor4f(buttontextlit);
	}
	if (rightclickinwindow(12,511,652,519))
	{
		icurrentframe=(int)((float)((float)(mx-12)/(float)itimelinezoom)/640.0*(float)itimelinelength)+itimelinestart;
		icurrentframe=min(max(0,icurrentframe),itimelinelength);
	}

	if (midclickinwindow(12,511,652,579))
	{
		int icc=(int)((float)((float)(mbx-12)/(float)itimelinezoom)/640.0*(float)itimelinelength)+itimelinestart;
		//if (syncloaded)
		{
			for (int x=0; x<synclength; x++)
			{
				if (patternstarts[x]>=icc) x=synclength;
				else
				startpattern=x;
			}
		}
		//icurrentframe=min(max(0,icurrentframe),itimelinelength);
	}

}

void sortevents()
{
	if (eventlist!=NULL)
	{
		int cnt=0;
		event *e;
		for (e=eventlist; e; e=e->next) {cnt++;}
		event **nlist = new event*[cnt];
		e=eventlist;
		for (int x=0; x<cnt; x++)
		{
			nlist[x]=e;
			e=e->next;
		}
		for (x=0; x<cnt; x++)
		{
			int smallest=nlist[x]->pass;
			int smallestindex=x;
			for (int y=x; y<cnt; y++) 
				if (nlist[y]->pass<smallest) 
				{
					smallest=nlist[y]->pass;
					smallestindex=y;
				}
				event *buf=nlist[smallestindex];
				nlist[smallestindex]=nlist[x];
				nlist[x]=buf;
		}
		eventlist=nlist[0];
		for (x=0; x<cnt; x++)
		{
			if (x+1<cnt)
				nlist[x]->next=nlist[x+1];
			else
				nlist[x]->next=NULL;
		}
		delete nlist;
	}
}

float linear(float a, float b, float t)
{
	return a+(b-a)*t;
}

camera lastcam;

void drawevent(event *e, float t)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	float x1,y1,x2,y2;

	x1=(float)floor(linear((float)e->startrectx1,(float)e->endrectx1,t));
	y1=(float)floor(linear((float)e->startrecty1,(float)e->endrecty1,t));
	x2=(float)floor(linear((float)e->startrectx2,(float)e->endrectx2,t))+1;
	y2=(float)floor(linear((float)e->startrecty2,(float)e->endrecty2,t))+1;

	if (e->eventtype==layer2d)
	{
		glDepthMask(false);
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(13,90,640,480);
		glColor4f(linear(e->startcol[0],e->endcol[0],t),linear(e->startcol[1],e->endcol[1],t),linear(e->startcol[2],e->endcol[2],t),linear(e->startcol[3],e->endcol[3],t));
		glEnable(GL_BLEND);
		glBlendFunc(e->blendfunc1,e->blendfunc2);
		if (e->textured) 
		{
			glEnable(GL_TEXTURE_2D); 
			glBindTexture(GL_TEXTURE_2D,e->mattexture);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(x1,y1);
			glTexCoord2f(0,1);
			glVertex2f(x2,y1);
			glTexCoord2f(1,1);
			glVertex2f(x2,y2);
			glTexCoord2f(1,0);
			glVertex2f(x1,y2);
			glEnd();
		}
		else 
		{
			glDisable(GL_TEXTURE_2D);
			quad(x1,y1,x2,y2);
		}
	}
	if (e->eventtype==layer3d)
	{
		float animtimer=linear(e->animstart,e->animend,t);
		scene *pf = e->iscene; //findscene(scenelist,e->sceneid);
		cameraanimation *cam=e->icam;//findcamanim(pf->cameraanims,e->cameraid);
		sceneobjanim *objanim=e->ianim;
		getscenestate(pf,animtimer*(float)objanim->length,objanim->number);
		calculateobjhierarchy(pf,-1,NULL);
		camera c;
		float f=(linear(e->camerastart,e->cameraend,t)*(float)cam->length);
		if (cam->keyframenum!=0)
		getcamerastate(cam, c, f);
		else c=lastcam;
		setcameraview(c,(float)(x2-x1)/(float)(y2-y1));
		lastcam=c;
		glViewport((int)(13+x1),(int)(570-y2),(int)(x2-x1),(int)(y2-y1));
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		scene_rendersolid(*pf,false);
		scene_rendertransparent(*pf,false);
		switchto2d();
	}
	if (e->eventtype==cleargl)
	{
		//glClear((0x100 && selectedevent->clearzbuffer) || (0x4000 && selectedevent->clearscreen));
		if (e->clearzbuffer && !e->clearscreen) glClear(0x100);
		if (!e->clearzbuffer && e->clearscreen) //glClear(0x4000);
		{
			glColor4f(0,0,0,0);
			glDisable(GL_DEPTH_TEST);
			quad(12,30,653,511);
		}
		if (e->clearzbuffer && e->clearscreen) //glClear(0x4000);
		{
			glColor4f(0,0,0,0);
			glDisable(GL_DEPTH_TEST);
			quad(12,30,653,511);
			glClear(0x100);
		}
		//if (!e->clearzbuffer && e->clearscreen) glClear(0x4000);
		//if (e->clearzbuffer && e->clearscreen) glClear(0x4100);
	}
	if (e->eventtype==rendertotext)
	{
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(13,90,640,480);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,shots[e->texture].texture);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,(int)(13+x1),(int)(570-y2), 1024, 1024,0);
		shots[e->texture].x1=0;
		shots[e->texture].y1=(y2-y1)/1024.0f;
		shots[e->texture].x2=(x2-x1)/1024.0f;
		shots[e->texture].y2=0;
	}
	if (e->eventtype==feedback)
	{
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(13,90,640,480);
		glColor4f(linear(e->startcol[0],e->endcol[0],t),linear(e->startcol[1],e->endcol[1],t),linear(e->startcol[2],e->endcol[2],t),linear(e->startcol[3],e->endcol[3],t));
		glEnable(GL_BLEND);
		glBlendFunc(e->blendfunc1,e->blendfunc2);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_TEXTURE_2D); 
		glBindTexture(GL_TEXTURE_2D,shots[e->texture].texture);
		glBegin(GL_QUADS);

		float xc,yc,xs,ys;
		xc=(shots[e->texture].x1+shots[e->texture].x2)/2.0f;
		yc=(shots[e->texture].y1+shots[e->texture].y2)/2.0f;
		xs=(shots[e->texture].x2-shots[e->texture].x1)/2.0f;
		ys=(shots[e->texture].y2-shots[e->texture].y1)/2.0f;

		for (x=0; x<e->param1; x++)
		{
			float zoom=1.0f+(float)x/e->param1/e->param4;
			glColor4f(linear(e->startcol[0],e->endcol[0],t)*(float)(20-x)/20.0f,linear(e->startcol[1],e->endcol[1],t)*(float)(20-x)/20.0f,linear(e->startcol[2],e->endcol[2],t)*(float)(20-x)/20.0f,linear(e->startcol[3],e->endcol[3],t)*(float)(20-x)/20.0f);
			glTexCoord2f(xc-xs/zoom,yc-ys/zoom);
			glVertex2f(x1,y1);
			glTexCoord2f(xc+xs/zoom,yc-ys/zoom);
			glVertex2f(x2,y1);
			glTexCoord2f(xc+xs/zoom,yc+ys/zoom);
			glVertex2f(x2,y2);
			glTexCoord2f(xc-xs/zoom,yc+ys/zoom);
			glVertex2f(x1,y2);
		}

		glEnd();

	}
	if (e->eventtype==grideffect)
	{
		glDepthMask(false);
		float time=linear(e->animstart,e->animend,t);
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(13,90,640,480);
		glColor4f(linear(e->startcol[0],e->endcol[0],t),linear(e->startcol[1],e->endcol[1],t),linear(e->startcol[2],e->endcol[2],t),linear(e->startcol[3],e->endcol[3],t));
		glEnable(GL_BLEND);
		glBlendFunc(e->blendfunc1,e->blendfunc2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,shots[e->texture].texture);
		MakeEffect(e->effect,time,x1,y1,x2,y2, shots[e->texture].x1, shots[e->texture].y1, shots[e->texture].x2, shots[e->texture].y2,
				0, 0, 0, 0, 0, 0, 0, 0);
	}
	glPopAttrib();
}

void displaycurrentframe()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	sortevents();
	for (event *e=eventlist; e; e=e->next)
	{
		if (icurrentframe>=e->startframe && icurrentframe<=e->endframe) drawevent(e,(float)(icurrentframe-e->startframe)/(float)(e->endframe-e->startframe));
	}
	glPopAttrib();
	switchto2d();
}

long fsize(FILE *str) {
	FILE *stream=str;
	long length, curpos;
	curpos=ftell(stream);
	fseek(stream, 0, SEEK_END);
	length=ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

void introeditor(int mode)
{
	if (mouseinwindow(13,31,652,510))
	{
		sprintf(cursorstring,"(%d:%d)",mx-13,my-31);
	}
	if (mode==DD_Draw)
	{
		glColor4f(0,0,0,0);
		glDepthMask(false);
		quad(12,30,652,510);
		glDepthMask(true);
	}
	if (mode==DD_Draw) 
	{
		displaycurrentframe();
		passcounter=0;
		int lastpass=-1;
		for (event *e=eventlist;e;e=e->next)
		{
			if (e->pass!=lastpass) passcounter++;
			lastpass=e->pass;
		}
	}
	if (mode==DD_Draw)
	{
		glDepthMask(false);
		glColor4f(buttontextlit);
		rectangle(12,30,653,511);
		glDepthMask(true);
	}
	if (button(726,585,759,597,"rulers",3,8,base2,rulermenu,mode) ==DDgui_LeftClick && mode!=DD_AfterCheck)
	{
		rulermenu=!rulermenu;
		musicmenu=false;
		//selectedevent=NULL;
		waitleftbutton=true;
	}
	if (rulermenu)
	{
		if (selectedevent && mode==DD_Draw)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glLoadIdentity();
			gluOrtho2D (-0.1, 639.1, 479.1,-0.1);
			glViewport(13,90,640,480);
			glColor4f(0,1,0,1);
			glDisable(GL_DEPTH_TEST);
			specrectangle(selectedevent->startrectx1,selectedevent->startrecty1,
				selectedevent->startrectx2,selectedevent->startrecty2,timeGetTime());
			glColor4f(1,1,0,1);
			specrectangle(selectedevent->endrectx1,selectedevent->endrecty1,
				selectedevent->endrectx2,selectedevent->endrecty2,timeGetTime()+1000);
			switchto2d();
			glPopAttrib();
			glColor4f(buttontextlit);
		}
		if (selectedevent && button(661,470,795,489,"Set Rulers To Boundaries",3,13,base2,false,mode) == DDgui_LeftClick)
		{
			yruler1=selectedevent->startrectx1;
			yruler2=selectedevent->startrectx2;
			xruler1=selectedevent->startrecty1;
			xruler2=selectedevent->startrecty2;
			rulers=true;
		}
		if (button(660,70,794,90,"RULERS",45,13,base2,rulers,mode)==DDgui_LeftClick && mode!=DD_AfterCheck)
		{
			rulers=!rulers;
			waitleftbutton=true;
		}

		if (button(660+15,490,680+15,510,"4:3",3,13,base2,false,mode) == DDgui_LeftClick)
		{
			yruler1=0;
			xruler1=0;
			yruler2=639;
			xruler2=479;
		}
		if (button(681+15,490,711+15,510,"16:9",5,13,base2,false,mode) == DDgui_LeftClick)
		{
			yruler1=0;
			xruler1=59;
			yruler2=639;
			xruler2=419;
		}
		if (button(712+15,490,732+15,510,"2:1",4,13,base2,false,mode) == DDgui_LeftClick)
		{
			yruler1=0;
			xruler1=79;
			yruler2=639;
			xruler2=399;
		}
		if (button(733+15,490,768+15,510,"2.35:1",4,13,base2,false,mode) == DDgui_LeftClick)
		{
			yruler1=0;
			xruler1=103;
			yruler2=639;
			xruler2=375;
		}

		if (!middlebutton) 
		{
			x1move=x2move=y1move=y2move=false;
		}

		if (x1move) xruler1=max(0,min(479,(my)-31));
		if (x2move) xruler2=max(0,min(479,(my)-31));
		if (y1move) yruler1=max(0,min(639,(mx)-13));
		if (y2move) yruler2=max(0,min(639,(mx)-13));
		
		if (midclickinwindow(13,xruler1+21,653,xruler1+41) && rulers && !x1move)
		{
			x1move=true;
			x1old=xruler1;
		}
	
		if (midclickinwindow(13,xruler2+21,653,xruler2+41) && rulers && !x2move)
		{
			x2move=true;
			x2old=xruler2;
		}
		
		if (midclickinwindow(yruler1+3,31,yruler1+23,510) && rulers && !y1move)
		{
			y1move=true;
			y1old=yruler1;
		}
		
		if (midclickinwindow(yruler2+3,31,yruler2+23,510) && rulers && !y2move)
		{
			y2move=true;
			y2old=yruler2;
		}
	}

	if (button(760,585,797,597,"music",5,8,base2,musicmenu,mode))
	{
		musicmenu=true;
		rulermenu=false;
		selectedevent=NULL;
		xmlist=findfile(musicmask);
		synclist=findfile(syncmask);
	}
	if (musicmenu)
	{
		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			rectangle(661,120,775,264);
		}	  
		if (mode==DD_Check)
			if (leftclickinwindow(661,121,774,263) && mouseinwindow(661,121,774,260)) musselected=min(musbarpos+(my-121) / 10,filenum(xmlist)-1);
		if ((mode ==DD_Check) && mouseinwindow(661,121,774,263)) musbarpos-=wheel*4;
		scroller(775,120,790,264,15,15,filenum(xmlist),14,musbarpos,mode);
		if (mode==DD_Draw)
		{
			pf = xmlist;
			for (x=1;x<=musbarpos;x++) pf=pf->next;
			for (x=0;x<=13;x++)
			{
				if (pf!=NULL)
				{
					if (musbarpos+x==musselected) glColor4f(col4); else glColor4f(buttontextlit);
					glRasterPos2i(665,130+x*10);
					strcpy(st,pf->filedata.cFileName);
					glPrint(st,base2,18);
					pf=pf->next;
				}
			}

			glColor4f(buttontextlit);
			sprintf(s,"%d MUSICS.",filenum(xmlist));
			glRasterPos2i(683,277);
			glPrint(s,base2);

		}
		if ((button(686,283,771,299,texbutton1,0,96.0/256.0,85.0/256.0,112.0/256.0,false,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck))
		{
			pf = xmlist;
			for (x=1;x<=musselected;x++) pf=pf->next;
			sprintf(s,"%s%s",musicdir,pf->filedata.cFileName);
			//loadaDDictscene(actualscene,NULL,s,musobjectsave,musselectionsave,mustexturesave,muslightsave,muscamerasave,musenvironmentsave);
			int fsize;
			FILE *f=fopen(s,"rb");
			fseek(f, 0, SEEK_END); fsize = ftell(f);
			fseek(f,0,SEEK_SET);
			music_data=new char[fsize];
			fread(music_data,1,fsize,f);
			musicloaded=true;
			music_pos=0;
			music_len=fsize;
			mod=FMUSIC_LoadSong("",0);
			play(startpattern);
			stop();
			waitleftbutton=true;
		}

		if (mode==DD_Draw)
		{
			glColor4f(buttontextlit);
			rectangle(661,120+200,775,264+200);
		}	  
		if (mode==DD_Check)
			if (leftclickinwindow(661,121+200,774,263+200) && mouseinwindow(661,121+200,774,260+200)) syncselected=min(syncbarpos+(my-121-200) / 10,filenum(synclist)-1);
		if ((mode ==DD_Check) && mouseinwindow(661,121+200,774,263+200)) syncbarpos-=wheel*4;
		scroller(775,120+200,790,264+200,15,15,filenum(synclist),14,syncbarpos,mode);
		if (mode==DD_Draw)
		{
			pf = synclist;
			for (x=1;x<=syncbarpos;x++) pf=pf->next;
			for (x=0;x<=13;x++)
			{
				if (pf!=NULL)
				{
					if (syncbarpos+x==syncselected) glColor4f(col4); else glColor4f(buttontextlit);
					glRasterPos2i(665,130+x*10+200);
					strcpy(st,pf->filedata.cFileName);
					glPrint(st,base2,18);
					pf=pf->next;
				}
			}

			glColor4f(buttontextlit);
			sprintf(s,"%d SYNCS.",filenum(synclist));
			glRasterPos2i(683,277+200);
			glPrint(s,base2);

		}
		if ((button(686,283+200,771,299+200,texbutton1,0,96.0/256.0,85.0/256.0,112.0/256.0,false,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck))
		{
			pf = synclist;
			for (x=1;x<=syncselected;x++) pf=pf->next;
			sprintf(s,"%s%s",syncdir,pf->filedata.cFileName);
			FILE *f=fopen(s,"rb");
			int data;
			int x=0;
			while (fscanf(f,"%d",&data)>0)
			{				
				patternstarts[x]=data/10;
				x++;
			}
			synclength=x;
			waitleftbutton=true;
		}
	}

	if (icurrentframe>=itimelinelength)
	{
		icurrentframe=0;
		if (musicloaded && iplaying)
		{
			FSOUND_SetPause(false);
			if (FMUSIC_PlayingSong)
			{					
				//FMUSIC_StopSong(mod);
				//FSOUND_Close();
				stop();
				mod=FMUSIC_LoadSong("",0);
				FSOUND_Init(44100, 0);
			}
			//FMUSIC_PlaySong(mod,0);
		}
		iplaying=false;
		playstarted=false;
	}
	timeline(mode);
	if (button(282+382,560,310+382,583,texbutton2,171.0/256.0,19.0/256.0,199.0/256.0,42.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{ 
		icurrentframe=0; 
		playstarted=false;
	}
	if (button(310+382,560,330+382,583,texbutton2,171.0/256.0,42.0/256.0,191.0/256.0,65.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		startpattern--;
		//icurrentframe--; 
		//if (icurrentframe<0) icurrentframe=0; 
		if (startpattern<0) startpattern=0; 
		waitleftbutton=true;
	}
	if (button(330+382,560,350+382,583,texbutton2,171.0/256.0,88.0/256.0,191.0/256.0,111.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		// (iplaying)
		{
			if (musicloaded)
			{
				FSOUND_SetPause(false);
				if (FMUSIC_PlayingSong)
				{					
					//FMUSIC_StopSong(mod);
					//FSOUND_Close();
					stop();
					FSOUND_Init(44100, 0);
				}
			}
			playstarted=false;
			iplaying=false;
			if (musicloaded)
			iplaystart=FMUSIC_GetTime(mod);
			else
			iplaystart=timeGetTime();
			icurrentframe=0;
			waitleftbutton=true;
		}
	}
	if (!iplaying)
	{
		if (button(350+382,560,370+382,583,texbutton2,171.0/256.0,111.0/256.0,191.0/256.0,134.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		{
			if (musicloaded)
			{
				if (!playstarted) 
				{
					iplaystart=(icurrentframe%itimelinelength)*10;
					//FMUSIC_PlaySong(mod,0);
					play(startpattern);
				}
				else
				{
					FSOUND_SetPause(false);
				}
				playstarted=true;
				//pausesize+=FMUSIC_GetTime(mod)-pausestart;
			}
			iplaying=true;
			if (musicloaded)
			{

			}
			else
			iplaystart=timeGetTime()-(icurrentframe%itimelinelength)*10;
			waitleftbutton=true;
		}
	}
	else
	{
		if (musicloaded)
			icurrentframe=(FMUSIC_GetTime(mod)+iplaystart)/10+patternstarts[startpattern];
		else
		icurrentframe=(timeGetTime()-iplaystart)/10;
		if (button(350+382,560,370+382,583,texbutton2,171.0/256.0,203.0/256.0,191.0/256.0,226.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		{
			if (musicloaded)
			{
				//iplaystart=(icurrentframe%itimelinelength)*10;
				FSOUND_SetPause(true);
			}
			iplaying=false;
			if (musicloaded) {}
			//iplaystart=0;//FMUSIC_GetTime(mod);
			else
			iplaystart=timeGetTime();
			waitleftbutton=true;
		}
	}
	if (button(370+382,560,390+382,583,texbutton2,171.0/256.0,157.0/256.0,191.0/256.0,180.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		//icurrentframe++; 
		//if (icurrentframe>itimelinelength) icurrentframe=itimelinelength; 
		startpattern++;
		if (startpattern>synclength) startpattern=synclength; 
		waitleftbutton=true;
	}
	if (button(390+382,560,418+382,583,texbutton2,171.0/256.0,180.0/256.0,199.0/256.0,203.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
	{
		icurrentframe=itimelinelength;
		getcamerastate(actualcameraanim,actualcamera,(float)icurrentframe);
	}

	if (rulers)	
	if (mode==DD_Draw)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glLoadIdentity();
		gluOrtho2D (-0.1, 639.1, 479.1,-0.1);
		glViewport(13,90,640,480);
		glColor4f(1,0,0,1);
		glDisable(GL_DEPTH_TEST);
		specline(0,xruler1,640,xruler1,timeGetTime());
		if (mouseinwindow(13,xruler1+21,653,xruler1+41))
		{
			specline(0,xruler1,640,xruler1,timeGetTime()+1000);
		}
	
		specline(0,xruler2,640,xruler2,timeGetTime());
		if (mouseinwindow(13,xruler2+21,653,xruler2+41))
		{
			specline(0,xruler2,640,xruler2,timeGetTime()+1000);
		}
		
		specline(yruler1,0,yruler1,480,timeGetTime());
		if (mouseinwindow(yruler1+3,31,yruler1+23,510))
		{
			specline(yruler1,0,yruler1,480,timeGetTime()+1000);
		}
		
		specline(yruler2,0,yruler2,480,timeGetTime());
		if (mouseinwindow(yruler2+3,31,yruler2+23,510))
		{
			specline(yruler2,0,yruler2,480,timeGetTime()+1000);
		}

		switchto2d();
		glPopAttrib();
	}

	if (mode==DD_Check && mouseinwindow(13,31,652,510) && rulers)
	{
		if (mouseinwindow(13,21+xruler1,653,41+xruler1)) my=31+xruler1;
		if (mouseinwindow(13,21+xruler2,653,41+xruler2)) my=31+xruler2;
		if (mouseinwindow(3+yruler1,31,23+yruler1,510)) mx=13+yruler1;
		if (mouseinwindow(3+yruler2,31,23+yruler2,510)) mx=13+yruler2;

		if (leftclickinwindow(13,21+xruler1,653,41+xruler1)) ly=31+xruler1;
		if (leftclickinwindow(13,21+xruler2,653,41+xruler2)) ly=31+xruler2;
		if (leftclickinwindow(3+yruler1,31,23+yruler1,510)) lx=13+yruler1;
		if (leftclickinwindow(3+yruler2,31,23+yruler2,510)) lx=13+yruler2;
	
		if (rightclickinwindow(13,21+xruler1,653,41+xruler1)) ry=31+xruler1;
		if (rightclickinwindow(13,21+xruler2,653,41+xruler2)) ry=31+xruler2;
		if (rightclickinwindow(3+yruler1,31,23+yruler1,510)) rx=13+yruler1;
		if (rightclickinwindow(3+yruler2,31,23+yruler2,510)) rx=13+yruler2;
	
	}

	if (selectedevent!=NULL)
	{
		if (button(340+329,520-4+66,397+329,536-4+66,texbutton2,0,0,57.0/256.0,16.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check) 
		{
			bool copydone=false;
			bool havespace=true;
			int x=selectedevent->pass+1;
			event *e;
			while (!copydone)
			{
				havespace=true;
				for (e=eventlist;e;e=e->next)
				if (e->pass==x)
				{
					if ((e->startframe>=selectedevent->startframe && e->startframe<=selectedevent->endframe) ||
					    (e->endframe>=selectedevent->startframe && e->endframe<=selectedevent->endframe) || 
						(e->startframe<=selectedevent->startframe && e->endframe>=selectedevent->endframe)) havespace=false;
				}
				if (havespace)
				{
					copydone=true;
					addevent(x,selectedevent->startframe,selectedevent->endframe);

					eventlist->eventtype=selectedevent->eventtype;
					eventlist->startrectx=selectedevent->startrectx;
					eventlist->startrecty=selectedevent->startrecty;
					eventlist->startrectx1=selectedevent->startrectx1;
					eventlist->startrecty1=selectedevent->startrecty1;
					eventlist->startrectx2=selectedevent->startrectx2;
					eventlist->startrecty2=selectedevent->startrecty2;
					eventlist->endrectx1=selectedevent->endrectx1;
					eventlist->endrecty1=selectedevent->endrecty1;
					eventlist->endrectx2=selectedevent->endrectx2;
					eventlist->endrecty2=selectedevent->endrecty2;
					eventlist->endrectx=selectedevent->endrectx;
					eventlist->endrecty=selectedevent->endrecty;
					eventlist->startrectxrez=selectedevent->startrectxrez;
					eventlist->startrectyrez=selectedevent->startrectyrez;
					eventlist->endrectxrez=selectedevent->endrectxrez;
					eventlist->endrectyrez=selectedevent->endrectyrez;
					eventlist->blendfunc1=selectedevent->blendfunc1;
					eventlist->blendfunc2=selectedevent->blendfunc2;
					eventlist->startcol[0]=selectedevent->startcol[0];
					eventlist->startcol[1]=selectedevent->startcol[1];
					eventlist->startcol[2]=selectedevent->startcol[2];
					eventlist->startcol[3]=selectedevent->startcol[3];
					eventlist->endcol[0]=selectedevent->endcol[0];
					eventlist->endcol[1]=selectedevent->endcol[1];
					eventlist->endcol[2]=selectedevent->endcol[2];
					eventlist->endcol[3]=selectedevent->endcol[3];
					eventlist->textured=selectedevent->textured;
					eventlist->texxtile=selectedevent->texxtile;
					eventlist->texytile=selectedevent->texytile;
					eventlist->sceneid=selectedevent->sceneid;
					eventlist->cameraid=selectedevent->cameraid;
					eventlist->animid=selectedevent->animid;
					eventlist->texture=selectedevent->texture;
					eventlist->clearscreen=selectedevent->clearscreen;
					eventlist->clearzbuffer=selectedevent->clearzbuffer;
					eventlist->menu1=selectedevent->menu1;
					eventlist->menu2=selectedevent->menu2;
					eventlist->icamselected=selectedevent->icamselected;
					eventlist->icampos=selectedevent->icampos;
					eventlist->isceneselected=selectedevent->isceneselected;
					eventlist->iscenepos=selectedevent->iscenepos;
					eventlist->ianimselected=selectedevent->ianimselected;
					eventlist->ianimpos=selectedevent->ianimpos;
					eventlist->iscene=selectedevent->iscene;
					eventlist->icam=selectedevent->icam;
					eventlist->ianim=selectedevent->ianim;
					eventlist->matselected=selectedevent->matselected;
					eventlist->matstart=selectedevent->matstart;
					eventlist->mattexture=selectedevent->mattexture;

					eventlist->effect=selectedevent->effect;
					eventlist->param1=selectedevent->param1;
					eventlist->param2=selectedevent->param2;
					eventlist->param3=selectedevent->param3;
					eventlist->param4=selectedevent->param4;
					eventlist->param5=selectedevent->param5;
					eventlist->param6=selectedevent->param6;

					waitleftbutton=true;
				}
				x++;
			}
		}
		if (button(340+403,552-8,397+403,568-8,texbutton2,0,32.0/256.0,57.0/256.0,48.0/256.0,false,mode) == DDgui_LeftClick && mode==DD_Check)
		{
			event *b=NULL;
			for (event *e=eventlist; e && e!=selectedevent; e=e->next) b=e;
			if (b==NULL) { if (eventlist) eventlist=eventlist->next;} else
			b->next=selectedevent->next;
			selectedevent=NULL;
		}
		else
		{
			if (button(670,544,690,560,"UP",4,11,base2,false,mode) && mode!=DD_AfterCheck)
			{
				bool go=true;
				for (event *e=eventlist;e;e=e->next)
				{
					if (e->pass==selectedevent->pass-1 && 
						!( ((e->endframe<selectedevent->startframe) && (e->startframe<selectedevent->startframe)) 
						|| ((e->endframe>selectedevent->endframe) && (e->startframe>selectedevent->endframe)))) go=false;
				}
				if (go)
				selectedevent->pass=max(0,selectedevent->pass-1);
				waitleftbutton=true;
			}
			if (button(691,544,740,560,"DOWN",7,11,base2,false,mode) && mode!=DD_AfterCheck)
			{
				bool go=true;
				for (event *e=eventlist;e;e=e->next)
				{
					if (e->pass==selectedevent->pass+1 && 
						!( ((e->endframe<selectedevent->startframe) && (e->startframe<selectedevent->startframe)) 
						|| ((e->endframe>selectedevent->endframe) && (e->startframe>selectedevent->endframe)))) go=false;
				}
				if (go)
				selectedevent->pass=min(99,selectedevent->pass+1);
				waitleftbutton=true;
			}
			if (mode==DD_Draw)
			{
				glRasterPos2i(660,40);
				glPrint(selectedevent->name,base2);
				glRasterPos2i(660,50);
				sprintf(str,"Start: %d End: %d",selectedevent->startframe,selectedevent->endframe);
				glPrint(str,base2);
			}
			if (!rulermenu && button(660,60,700,80,"2D",14,13,base2,selectedevent->eventtype==layer2d || selectedevent->eventtype>cleargl ,mode) == DDgui_LeftClick) selectedevent->eventtype=layer2d;
			if (!rulermenu && button(701,60,741,80,"3D",14,13,base2,selectedevent->eventtype==layer3d,mode) == DDgui_LeftClick) selectedevent->eventtype=layer3d;
			if (!rulermenu && button(742,60,782,80,"CLR",10,13,base2,selectedevent->eventtype==cleargl,mode) == DDgui_LeftClick) selectedevent->eventtype=cleargl;
			if (selectedevent->eventtype==cleargl  && !rulermenu)
			{
				if (button(660,90,701,110,"ZBUF",6,13,base2,selectedevent->clearzbuffer,mode) == DDgui_LeftClick && mode!=DD_AfterCheck && !rulermenu) { selectedevent->clearzbuffer=!selectedevent->clearzbuffer; waitleftbutton=true;}
				if (button(702,90,741,110,"SCR",9,13,base2,selectedevent->clearscreen,mode) == DDgui_LeftClick && mode!=DD_AfterCheck && !rulermenu) { selectedevent->clearscreen=!selectedevent->clearscreen;  waitleftbutton=true;}
			}
			if (!rulermenu && (selectedevent->eventtype==layer2d || selectedevent->eventtype>cleargl))
			{
				if (button(660,90,701,110,"STAT",6,13,base2,selectedevent->menu1==0,mode && !rulermenu) == DDgui_LeftClick) selectedevent->menu1=0;
				if (button(702,90,741,110,"VAR",9,13,base2,selectedevent->menu1==1,mode && !rulermenu) == DDgui_LeftClick) selectedevent->menu1=1;
				if (aCid && button(742,90,782,110,"FX",13,13,base2,selectedevent->menu1==2,mode && !rulermenu) == DDgui_LeftClick) selectedevent->menu1=2;
				switch (selectedevent->menu1)
				{
				case 0:
					{
						if (mode==DD_Draw)
						{
							glPushAttrib(GL_ALL_ATTRIB_BITS);
							glColor4f(1,1,1,1);
							glDisable(GL_DEPTH_TEST);
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,mattexture);
							glBegin(GL_QUADS);
							glTexCoord2f(0,0);
							glVertex2i(715,465);
							glTexCoord2f(0,1);
							glVertex2i(790,465);
							glTexCoord2f(1,1);
							glVertex2i(790,539);
							glTexCoord2f(1,0);
							glVertex2i(715,539);
							glEnd();
							glDisable(GL_TEXTURE_2D);
							glColor4f(buttontextlit);
							rectangle(715,465,790,540);
							rectangle(661,387,774,460);
							glDisable(GL_DEPTH_TEST);
							glColor4f(buttontextlit);
							glPopAttrib();
						}
						scroller(775,257+130,790,330+130,15,15,materialnum(materiallist),7,selectedevent->matstart,mode);
						if (mode==DD_Draw)
						{
							material *pt=materiallist;
							for (x=1;x<=selectedevent->matstart;x++) pt=pt->next;
							for (x=selectedevent->matstart;x<=selectedevent->matstart+6;x++)
							{
								if (pt!=NULL)
								{
									if (x==selectedevent->matselected) glColor4f(col4); else glColor4f(buttontextlit);
									glRasterPos2i(663,267+130+(x-selectedevent->matstart)*10);
									sprintf(s,"%3d:%s",x,pt->name);
									glPrint(s,base2,12);
									pt=pt->next;
								}
							}
						}
						if (mode==DD_Check)
						{
							if (mouseinwindow(661,427-170+130,774,496-170+130) && leftclickinwindow(661,427-170+130,774,496-160+130) && (materiallist!=NULL))
							{
								selectedevent->matselected=min(materialnum(materiallist)-1,(my-427+170-130)/10+selectedevent->matstart);
								material *pt=materiallist;
								for (x=1;x<=selectedevent->matselected;x++) pt=pt->next;
								mattexture=pt->handle;
							}
							if (mouseinwindow(661,427-170+130,774,496-170+130)) selectedevent->matstart-=wheel;
						}
						material *pt=materiallist;
						for (x=1;x<=selectedevent->matselected;x++) pt=pt->next;
						if (pt!=NULL) mattexture=pt->handle;
						if (materiallist!=NULL)
						{
							if (button(665,506-42,710,522-42,texbutton2,0,112.0/256.0,45.0/256.0,128.0/256.0,false,mode) == DDgui_LeftClick)
							{
								selectedevent->textured=true;
								selectedevent->mattexture=pt->handle;
								selectedevent->texture=pt->number;
							}
							if (button(665,506-26,710,522-26,texbutton2,0,128.0/256.0,45.0/256.0,144.0/256.0,false,mode) == DDgui_LeftClick) selectedevent->textured=false;
						}

						glColor4f(buttontextlit);
						glRasterPos2i(660,134);
						glPrint("Layer Multiplier:",base2);
						glColor4f(1,1,1,1);
						bool i=selectedevent->blendfunc1==GL_ZERO;
						RadioButton(655,140,i,"Zero",mode); if (i) selectedevent->blendfunc1=GL_ZERO;
						i=selectedevent->blendfunc1==GL_ONE;
						RadioButton(655,153,i,"One",mode); if (i) selectedevent->blendfunc1=GL_ONE;
						i=selectedevent->blendfunc1==GL_DST_COLOR;
						RadioButton(655,166,i,"Background Color",mode); if (i) selectedevent->blendfunc1=GL_DST_COLOR;
						i=selectedevent->blendfunc1==GL_ONE_MINUS_DST_COLOR;
						RadioButton(655,179,i,"One minus Bckgrnd Color",mode); if (i) selectedevent->blendfunc1=GL_ONE_MINUS_DST_COLOR;
						i=selectedevent->blendfunc1==GL_DST_ALPHA;
						RadioButton(655,192,i,"Background Alpha",mode); if (i) selectedevent->blendfunc1=GL_DST_ALPHA;
						i=selectedevent->blendfunc1==GL_ONE_MINUS_DST_ALPHA;
						RadioButton(655,205,i,"One minus Bckgrnd Alpha",mode); if (i) selectedevent->blendfunc1=GL_ONE_MINUS_DST_ALPHA;
						i=selectedevent->blendfunc1==GL_SRC_ALPHA;
						RadioButton(655,218,i,"Layer Alpha",mode); if (i) selectedevent->blendfunc1=GL_SRC_ALPHA;
						i=selectedevent->blendfunc1==GL_ONE_MINUS_SRC_ALPHA;
						RadioButton(655,231,i,"One minus Layer Alpha",mode); if (i) selectedevent->blendfunc1=GL_ONE_MINUS_SRC_ALPHA;
						i=selectedevent->blendfunc1==GL_SRC_ALPHA_SATURATE;
						RadioButton(655,244,i,"Layer Alpha Saturate",mode); if (i) selectedevent->blendfunc1=GL_SRC_ALPHA_SATURATE;
						glColor4f(buttontextlit);
						glRasterPos2i(660,134+135);
						glPrint("Background Multiplier:",base2);
						glColor4f(1,1,1,1);
						i=selectedevent->blendfunc2==GL_ZERO;
						RadioButton(655,140+135,i,"Zero",mode); if (i) selectedevent->blendfunc2=GL_ZERO;
						i=selectedevent->blendfunc2==GL_ONE;
						RadioButton(655,153+135,i,"One",mode); if (i) selectedevent->blendfunc2=GL_ONE;
						i=selectedevent->blendfunc2==GL_SRC_COLOR;
						RadioButton(655,166+135,i,"Layer Color",mode); if (i) selectedevent->blendfunc2=GL_SRC_COLOR;
						i=selectedevent->blendfunc2==GL_ONE_MINUS_SRC_COLOR;
						RadioButton(655,179+135,i,"One minus Layer Color",mode); if (i) selectedevent->blendfunc2=GL_ONE_MINUS_SRC_COLOR;
						i=selectedevent->blendfunc2==GL_DST_ALPHA;
						RadioButton(655,192+135,i,"Background Alpha",mode); if (i) selectedevent->blendfunc2=GL_DST_ALPHA;
						i=selectedevent->blendfunc2==GL_ONE_MINUS_DST_ALPHA;
						RadioButton(655,205+135,i,"One minus Bckgrnd Alpha",mode); if (i) selectedevent->blendfunc2=GL_ONE_MINUS_DST_ALPHA;
						i=selectedevent->blendfunc2==GL_SRC_ALPHA;
						RadioButton(655,218+135,i,"Layer Alpha",mode); if (i) selectedevent->blendfunc2=GL_SRC_ALPHA;
						i=selectedevent->blendfunc2==GL_ONE_MINUS_SRC_ALPHA;
						RadioButton(655,231+135,i,"One minus Layer Alpha",mode); if (i) selectedevent->blendfunc2=GL_ONE_MINUS_SRC_ALPHA;
						break;
					}
				case 1:
					{
						if (button(661,470,795,489,"Set Rulers To Boundaries",3,13,base2,false,mode) == DDgui_LeftClick)
						{
							yruler1=selectedevent->startrectx1;
							yruler2=selectedevent->startrectx2;
							xruler1=selectedevent->startrecty1;
							xruler2=selectedevent->startrecty2;
							rulers=true;
						}
						if (button(660+15,490,680+15,510,"4:3",3,13,base2,false,mode) == DDgui_LeftClick)
						{
							selectedevent->startrectx=0.5;
							selectedevent->startrecty=0.5;
							selectedevent->startrectxrez=1;
							selectedevent->startrectyrez=1;
							selectedevent->startrectx1=0;
							selectedevent->startrecty1=0;
							selectedevent->startrectx2=639;
							selectedevent->startrecty2=479;
							selectedevent->endrectx=0.5;
							selectedevent->endrecty=0.5;
							selectedevent->endrectxrez=1;
							selectedevent->endrectyrez=1;
							selectedevent->endrectx1=0;
							selectedevent->endrecty1=0;
							selectedevent->endrectx2=639;
							selectedevent->endrecty2=479;
						}
						if (button(681+15,490,711+15,510,"16:9",5,13,base2,false,mode) == DDgui_LeftClick)
						{
							selectedevent->startrectx=0.5;
							selectedevent->startrecty=0.5;
							selectedevent->startrectxrez=1;
							selectedevent->startrectyrez=9.0/16.0*4.0/3.0;
							selectedevent->startrectx1=0;
							selectedevent->startrecty1=59;
							selectedevent->startrectx2=639;
							selectedevent->startrecty2=419;
							selectedevent->endrectx=0.5;
							selectedevent->endrecty=0.5;
							selectedevent->endrectxrez=1;
							selectedevent->endrectyrez=9.0/16.0*4.0/3.0;
							selectedevent->endrectx1=0;
							selectedevent->endrecty1=59;
							selectedevent->endrectx2=639;
							selectedevent->endrecty2=419;
						}
						if (button(712+15,490,732+15,510,"2:1",4,13,base2,false,mode) == DDgui_LeftClick)
						{
							selectedevent->startrectx=0.5;
							selectedevent->startrecty=0.5;
							selectedevent->startrectxrez=1;
							selectedevent->startrectyrez=0.5f*4.0f/3.0f;
							selectedevent->startrectx1=0;
							selectedevent->startrecty1=79;
							selectedevent->startrectx2=639;
							selectedevent->startrecty2=399;
							selectedevent->endrectx=0.5;
							selectedevent->endrecty=0.5;
							selectedevent->endrectxrez=1;
							selectedevent->endrectyrez=0.5f*4.0f/3.0f;
							selectedevent->endrectx1=0;
							selectedevent->endrecty1=79;
							selectedevent->endrectx2=639;
							selectedevent->endrecty2=399;
						}
						if (button(733+15,490,768+15,510,"2.35:1",4,13,base2,false,mode) == DDgui_LeftClick)
						{
							selectedevent->startrectx=0.5;
							selectedevent->startrecty=0.5;
							selectedevent->startrectxrez=1;
							selectedevent->startrectyrez=4.0f/3.0f/2.35f;
							selectedevent->startrectx1=0;
							selectedevent->startrecty1=103;
							selectedevent->startrectx2=639;
							selectedevent->startrecty2=375;
							selectedevent->endrectx=0.5;
							selectedevent->endrecty=0.5;
							selectedevent->endrectxrez=1;
							selectedevent->endrectyrez=4.0f/3.0f/2.35f;
							selectedevent->endrectx1=0;
							selectedevent->endrecty1=103;
							selectedevent->endrectx2=639;
							selectedevent->endrecty2=375;
						}
						if (mode==DD_Draw)
						{
							glPushAttrib(GL_ALL_ATTRIB_BITS);
							glLoadIdentity();
							gluOrtho2D (-0.1, 639.1, 479.1,-0.1);
							glViewport(13,90,640,480);
							glColor4f(0,1,0,1);
							glDisable(GL_DEPTH_TEST);
							specrectangle(selectedevent->startrectx1,selectedevent->startrecty1,
										  selectedevent->startrectx2,selectedevent->startrecty2,timeGetTime());
							glColor4f(1,1,0,1);
							specrectangle(selectedevent->endrectx1,selectedevent->endrecty1,
										  selectedevent->endrectx2,selectedevent->endrecty2,timeGetTime()+1000);
							switchto2d();
							glPopAttrib();
							glColor4f(buttontextlit);
							glRasterPos2i(660,134);
							glPrint("START COLOR",base2);
							glRasterPos2i(660,234);
							glPrint("END COLOR",base2);
						}
						sprintf(str,"%d",(int)(selectedevent->startcol[0]*255));
						ValueBar(660,140,1,0.0f,1.0f,1.0f/255.0f,&selectedevent->startcol[0],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->startcol[1]*255));
						ValueBar(660,160,2,0.0f,1.0f,1.0f/255.0f,&selectedevent->startcol[1],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->startcol[2]*255));
						ValueBar(660,180,3,0.0f,1.0f,1.0f/255.0f,&selectedevent->startcol[2],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->startcol[3]*255));
						ValueBar(660,200,0,0.0f,1.0f,1.0f/255.0f,&selectedevent->startcol[3],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->endcol[0]*255));
						ValueBar(660,240,1,0.0f,1.0f,1.0f/255.0f,&selectedevent->endcol[0],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->endcol[1]*255));
						ValueBar(660,260,2,0.0f,1.0f,1.0f/255.0f,&selectedevent->endcol[1],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->endcol[2]*255));
						ValueBar(660,280,3,0.0f,1.0f,1.0f/255.0f,&selectedevent->endcol[2],str,52,13,base2,mode);
						sprintf(str,"%d",(int)(selectedevent->endcol[3]*255));
						ValueBar(660,300,0,0.0f,1.0f,1.0f/255.0f,&selectedevent->endcol[3],str,52,13,base2,mode);
						if (mode==DD_Check)
						{
							if (leftclickinwindow(13,31,652,510))
							{
								float xr,yr,xp,yp;
								xr=(float)fabs((float)mx-lx)/639.0f;
								yr=(float)fabs((float)my-ly)/479.0f;
								xp=((float)mx+(float)lx-26.0f)/2.0f/639.0f;
								yp=((float)my+(float)ly-62.0f)/2.0f/479.0f;
								selectedevent->startrectx=xp;
								selectedevent->startrecty=yp;
								selectedevent->startrectxrez=xr;
								selectedevent->startrectyrez=yr;
								selectedevent->startrectx1=min(lx-13,mx-13);
								selectedevent->startrecty1=min(ly-31,my-31);
								selectedevent->startrectx2=max(lx-13,mx-13);
								selectedevent->startrecty2=max(ly-31,my-31);
							}
							if (rightclickinwindow(13,31,652,510))
							{
								float xr,yr,xp,yp;
								xr=(float)fabs((float)mx-rx)/639.0f;
								yr=(float)fabs((float)my-ry)/479.0f;
								xp=((float)mx+(float)rx-26.0f)/2.0f/639.0f;
								yp=((float)my+(float)ry-62.0f)/2.0f/479.0f;
								selectedevent->endrectx=xp;
								selectedevent->endrecty=yp;
								selectedevent->endrectxrez=xr;
								selectedevent->endrectyrez=yr;
								selectedevent->endrectx1=min(rx-13,mx-13);
								selectedevent->endrecty1=min(ry-31,my-31);
								selectedevent->endrectx2=max(rx-13,mx-13);
								selectedevent->endrecty2=max(ry-31,my-31);
							}
						}
						break;
					}
				case 2:
					{
						if (button(660,120,701,140,"2Dlayer",2,13,base2,selectedevent->eventtype==layer2d,mode) == DDgui_LeftClick)	selectedevent->eventtype=layer2d;
						if (button(660,141,783,161,"Grideffect",36,13,base2,selectedevent->eventtype==grideffect,mode) == DDgui_LeftClick) selectedevent->eventtype=grideffect;
						if (button(702,120,741,140,"R2TXT",3,13,base2,selectedevent->eventtype==rendertotext,mode) == DDgui_LeftClick) selectedevent->eventtype=rendertotext;
						if (button(742,120,783,140,"FeedB",5,13,base2,selectedevent->eventtype==feedback,mode) == DDgui_LeftClick) selectedevent->eventtype=feedback;

						if (selectedevent->eventtype==grideffect)
						{
							sprintf(str,"RenderTextureSlot %d",selectedevent->texture);
							ValueBar(660,166,0,0,19,&selectedevent->texture,str,12,13,base2,mode);
							if (mode==DD_Draw)
							{
								glPushAttrib(GL_ALL_ATTRIB_BITS);
								glEnable(GL_TEXTURE_2D);
								glColor4f(1,1,1,1);

								glBindTexture(GL_TEXTURE_2D,shots[selectedevent->texture].texture);
								glBegin(GL_QUADS);
								glTexCoord2f(shots[selectedevent->texture].x1,shots[selectedevent->texture].y1);
								glVertex2f(660,189);
								glTexCoord2f(shots[selectedevent->texture].x2,shots[selectedevent->texture].y1);
								glVertex2f(788,189);
								glTexCoord2f(shots[selectedevent->texture].x2,shots[selectedevent->texture].y2);
								glVertex2f(788,317);
								glTexCoord2f(shots[selectedevent->texture].x1,shots[selectedevent->texture].y2);
								glVertex2f(660,317);
								glEnd();

								glPopAttrib();
							}
							glColor4f(1,1,1,1);
							bool i=selectedevent->effect==0;
							RadioButton(660,320,i,"SINESLIME",mode); if (i) selectedevent->effect=0;
							i=selectedevent->effect==1;
							RadioButton(660,335,i,"GLASSBOTTOM",mode); if (i) selectedevent->effect=1;
							i=selectedevent->effect==2;
							RadioButton(660,350,i,"FOVZOOM",mode); if (i) selectedevent->effect=2;
							i=selectedevent->effect==3;
							RadioButton(660,365,i,"RIPPLE",mode); if (i) selectedevent->effect=3;
							i=selectedevent->effect==4;
							RadioButton(660,380,i,"DISINTEGRATE",mode); if (i) selectedevent->effect=4;
							//i=selectedevent->effect==5;
							//RadioButton(660,395,i,"BLOW",mode); if (i) selectedevent->effect=5;
							i=selectedevent->effect==6;
							RadioButton(660,395,i,"TWIRL",mode); if (i) selectedevent->effect=6;
							i=selectedevent->effect==7;
							RadioButton(660,410,i,"WAVE",mode); if (i) selectedevent->effect=7;
							//i=selectedevent->effect==8;
							//RadioButton(660,440,i,"ROTATE",mode); if (i) selectedevent->effect=8;
							sprintf(str,"Start: %.3f",selectedevent->animstart);
							ValueBar(660,460,0,0.0f,1.0f,0.001f,&selectedevent->animstart,str,34,13,base2,mode);
							sprintf(str,"End: %.3f",selectedevent->animend);
							ValueBar(660,480,0,0.0f,1.0f,0.001f,&selectedevent->animend,str,37,13,base2,mode);
						}

						
						if (selectedevent->eventtype==rendertotext)
						{
							sprintf(str,"RenderTextureSlot %d",selectedevent->texture);
							ValueBar(660,166,0,0,19,&selectedevent->texture,str,12,13,base2,mode);										
							
							if (mode==DD_Draw)
							{
								glPushAttrib(GL_ALL_ATTRIB_BITS);
								glEnable(GL_TEXTURE_2D);
								glColor4f(1,1,1,1);

								glBindTexture(GL_TEXTURE_2D,shots[selectedevent->texture].texture);
								glBegin(GL_QUADS);
								glTexCoord2f(shots[selectedevent->texture].x1,shots[selectedevent->texture].y1);
								glVertex2f(660,189);
								glTexCoord2f(shots[selectedevent->texture].x2,shots[selectedevent->texture].y1);
								glVertex2f(788,189);
								glTexCoord2f(shots[selectedevent->texture].x2,shots[selectedevent->texture].y2);
								glVertex2f(788,317);
								glTexCoord2f(shots[selectedevent->texture].x1,shots[selectedevent->texture].y2);
								glVertex2f(660,317);
								glEnd();

								glPopAttrib();
							}
						}

						if (selectedevent->eventtype==feedback)
						{
							sprintf(str,"RenderTextureSlot %d",selectedevent->texture);
							ValueBar(660,166,0,0,19,&selectedevent->texture,str,12,13,base2,mode);
							
							if (mode==DD_Draw)
							{
								glPushAttrib(GL_ALL_ATTRIB_BITS);
								glEnable(GL_TEXTURE_2D);
								glColor4f(1,1,1,1);

								glBindTexture(GL_TEXTURE_2D,shots[selectedevent->texture].texture);
								glBegin(GL_QUADS);
								glTexCoord2f(shots[selectedevent->texture].x1,shots[selectedevent->texture].y1);
								glVertex2f(660,189);
								glTexCoord2f(shots[selectedevent->texture].x2,shots[selectedevent->texture].y1);
								glVertex2f(788,189);
								glTexCoord2f(shots[selectedevent->texture].x2,shots[selectedevent->texture].y2);
								glVertex2f(788,317);
								glTexCoord2f(shots[selectedevent->texture].x1,shots[selectedevent->texture].y2);
								glVertex2f(660,317);
								glEnd();
								glPopAttrib();
							}

							sprintf(str,"Layernum: %d",selectedevent->param1);
							ValueBar(660,370,0,1,255,&selectedevent->param1,str,23,13,base2,mode);
							sprintf(str,"Density: %.3f",selectedevent->param4);
							ValueBar(660,390,0,0.0f,10.0f,0.01f,&selectedevent->param4,str,23,13,base2,mode);

						}
						
						break;
					}
			}

			}
			if (selectedevent->eventtype==layer3d  && !rulermenu)
			{
				if (button(661,470,795,489,"Set Rulers To Boundaries",3,13,base2,false,mode) == DDgui_LeftClick)
				{
					yruler1=selectedevent->startrectx1;
					yruler2=selectedevent->startrectx2;
					xruler1=selectedevent->startrecty1;
					xruler2=selectedevent->startrecty2;
					rulers=true;
				}
				if (button(660+15,490,680+15,510,"4:3",3,13,base2,false,mode) == DDgui_LeftClick)
				{
					selectedevent->startrectx=0.5;
					selectedevent->startrecty=0.5;
					selectedevent->startrectxrez=1;
					selectedevent->startrectyrez=1;
					selectedevent->startrectx1=0;
					selectedevent->startrecty1=0;
					selectedevent->startrectx2=639;
					selectedevent->startrecty2=479;
					selectedevent->endrectx=0.5;
					selectedevent->endrecty=0.5;
					selectedevent->endrectxrez=1;
					selectedevent->endrectyrez=1;
					selectedevent->endrectx1=0;
					selectedevent->endrecty1=0;
					selectedevent->endrectx2=639;
					selectedevent->endrecty2=479;
				}
				if (button(681+15,490,711+15,510,"16:9",5,13,base2,false,mode) == DDgui_LeftClick)
				{
					selectedevent->startrectx=0.5;
					selectedevent->startrecty=0.5;
					selectedevent->startrectxrez=1;
					selectedevent->startrectyrez=9.0/16.0*4.0/3.0;
					selectedevent->startrectx1=0;
					selectedevent->startrecty1=59;
					selectedevent->startrectx2=639;
					selectedevent->startrecty2=419;
					selectedevent->endrectx=0.5;
					selectedevent->endrecty=0.5;
					selectedevent->endrectxrez=1;
					selectedevent->endrectyrez=9.0/16.0*4.0/3.0;
					selectedevent->endrectx1=0;
					selectedevent->endrecty1=59;
					selectedevent->endrectx2=639;
					selectedevent->endrecty2=419;
				}
				if (button(712+15,490,732+15,510,"2:1",4,13,base2,false,mode) == DDgui_LeftClick)
				{
					selectedevent->startrectx=0.5;
					selectedevent->startrecty=0.5;
					selectedevent->startrectxrez=1;
					selectedevent->startrectyrez=0.5f*4.0f/3.0f;
					selectedevent->startrectx1=0;
					selectedevent->startrecty1=79;
					selectedevent->startrectx2=639;
					selectedevent->startrecty2=399;
					selectedevent->endrectx=0.5;
					selectedevent->endrecty=0.5;
					selectedevent->endrectxrez=1;
					selectedevent->endrectyrez=0.5f*4.0f/3.0f;
					selectedevent->endrectx1=0;
					selectedevent->endrecty1=79;
					selectedevent->endrectx2=639;
					selectedevent->endrecty2=399;
				}
				if (button(733+15,490,768+15,510,"2.35:1",4,13,base2,false,mode) == DDgui_LeftClick)
				{
					selectedevent->startrectx=0.5;
					selectedevent->startrecty=0.5;
					selectedevent->startrectxrez=1;
					selectedevent->startrectyrez=4.0f/3.0f/2.35f;
					selectedevent->startrectx1=0;
					selectedevent->startrecty1=103;
					selectedevent->startrectx2=639;
					selectedevent->startrecty2=375;
					selectedevent->endrectx=0.5;
					selectedevent->endrecty=0.5;
					selectedevent->endrectxrez=1;
					selectedevent->endrectyrez=4.0f/3.0f/2.35f;
					selectedevent->endrectx1=0;
					selectedevent->endrecty1=103;
					selectedevent->endrectx2=639;
					selectedevent->endrecty2=375;
				}
				if (mode==DD_Draw)
				{
					glPushAttrib(GL_ALL_ATTRIB_BITS);
					glLoadIdentity();
					gluOrtho2D (-0.1, 639.1, 479.1,-0.1);
					glViewport(13,90,640,480);
					glColor4f(0,1,0,1);
					glDisable(GL_DEPTH_TEST);
					specrectangle(selectedevent->startrectx1,selectedevent->startrecty1,
								  selectedevent->startrectx2,selectedevent->startrecty2,timeGetTime());
					glColor4f(1,1,0,1);
					specrectangle(selectedevent->endrectx1,selectedevent->endrecty1,
								  selectedevent->endrectx2,selectedevent->endrecty2,timeGetTime()+1000);
					switchto2d();
					glPopAttrib();
					if (selectedevent->menu1==0)
					{
						glColor4f(buttontextlit);
						glRasterPos2i(662,93);
						glPrint("SELECT SCENE:",base2);
						rectangle(661,100,775,204);
						glColor4f(buttontextlit);
						glRasterPos2i(662,214);
						glPrint("SELECT CAMERA:",base2);
						rectangle(661,220,775,324);
						glRasterPos2i(662,335);
						glPrint("SELECT ANIM:",base2);
						rectangle(661,340,775,444);
					}
				}
				if (mode==DD_Check)
				{
					if (leftclickinwindow(12,30,652,509))
					{
						float xr,yr,xp,yp;
						xr=(float)fabs((float)mx-lx)/639.0f;
						yr=(float)fabs((float)my-ly)/479.0f;
						xp=((float)mx+(float)lx-26.0f)/2.0f/639.0f;
						yp=((float)my+(float)ly-62.0f)/2.0f/479.0f;
						selectedevent->startrectx=xp;
						selectedevent->startrecty=yp;
						selectedevent->startrectxrez=xr;
						selectedevent->startrectyrez=yr;
						selectedevent->startrectx1=min(lx-13,mx-13);
						selectedevent->startrecty1=min(ly-31,my-31);
						selectedevent->startrectx2=max(lx-13,mx-13);
						selectedevent->startrecty2=max(ly-31,my-31);
					}
					if (rightclickinwindow(12,30,652,509))
					{
						float xr,yr,xp,yp;
						xr=(float)fabs((float)mx-rx)/639.0f;
						yr=(float)fabs((float)my-ry)/479.0f;
						xp=((float)mx+(float)rx-26.0f)/2.0f/639.0f;
						yp=((float)my+(float)ry-62.0f)/2.0f/479.0f;
						selectedevent->endrectx=xp;
						selectedevent->endrecty=yp;
						selectedevent->endrectxrez=xr;
						selectedevent->endrectyrez=yr;
						selectedevent->endrectx1=min(rx-13,mx-13);
						selectedevent->endrecty1=min(ry-31,my-31);
						selectedevent->endrectx2=max(rx-13,mx-13);
						selectedevent->endrecty2=max(ry-31,my-31);
					}
				}
				if (button(670-4,450,731-4,469,"Select",14,13,base2,selectedevent->menu1==0,mode)==DDgui_LeftClick) selectedevent->menu1=0;
				if (button(732-4,450,793-4,469,"Timer",14,13,base2,selectedevent->menu1==1,mode)==DDgui_LeftClick) selectedevent->menu1=1;
				if (selectedevent->menu1==0)
				{
					//////////////////////////// SCENE ///////////////////////
					if (mode==DD_Check)
						if (leftclickinwindow(661,101,774,203) && mouseinwindow(661,101,774,200)) 
							selectedevent->isceneselected=min(selectedevent->iscenepos+(my-101) / 10,cntscenes()-1);
						if ((mode ==DD_Check) && mouseinwindow(661,101,774,203)) selectedevent->iscenepos-=wheel*4;
						scroller(775,100,790,204,15,15,cntscenes(),10,selectedevent->iscenepos,mode);
						if (mode==DD_Draw)
						{
							scene *pf = scenelist;
							for (x=1;x<=selectedevent->iscenepos;x++) pf=pf->next;
							for (x=0;x<10;x++)
							{
								if (pf!=NULL)
								{
									if (selectedevent->iscenepos+x==selectedevent->isceneselected) glColor4f(col4); else glColor4f(buttontextlit);
									glRasterPos2i(665,110+x*10);
									strcpy(str,pf->name);
									glPrint(str,base2,18);
									pf=pf->next;
								}
							}
						}
						scene *pf = scenelist;
						for (x=1;x<=selectedevent->isceneselected;x++) pf=pf->next;
						selectedevent->sceneid=pf->number;
						selectedevent->iscene=pf;

						//////////////////////////// CAMERA ///////////////////////

						if (mode==DD_Check)
							if (leftclickinwindow(661,221,774,323) && mouseinwindow(661,221,774,320)) selectedevent->icamselected=min(selectedevent->icampos+(my-221) / 10,cntcamanims(selectedevent->iscene->cameraanims)-1);
							if ((mode ==DD_Check) && mouseinwindow(661,221,774,323)) selectedevent->icampos-=wheel*4;
							scroller(775,220,790,324,15,15,cntcamanims(selectedevent->iscene->cameraanims),10,selectedevent->icampos,mode);
							if (mode==DD_Draw)
							{
								cameraanimation *pf = selectedevent->iscene->cameraanims;
								for (x=1;x<=selectedevent->icampos;x++) pf=pf->next;
								for (x=0;x<10;x++)
								{
									if (pf!=NULL)
									{
										if (selectedevent->icampos+x==selectedevent->icamselected) glColor4f(col4); else glColor4f(buttontextlit);
										glRasterPos2i(665,230+x*10);
										strcpy(str,pf->name);
										glPrint(str,base2,18);
										pf=pf->next;
									}
								}
							}
							cameraanimation *pc = selectedevent->iscene->cameraanims;
							for (x=1;x<=selectedevent->icamselected;x++) if (pc) pc=pc->next;
							if (pc) selectedevent->cameraid=pc->number;
							if (pc) selectedevent->icam=pc;
							else
							{
								selectedevent->cameraid=0;
								selectedevent->icam=pf->cameraanims;
								selectedevent->icamselected=0;
							}

							//////////////////////////// ANIM ///////////////////////

							if (mode==DD_Check)
								if (leftclickinwindow(661,221+120,774,323+120) && mouseinwindow(661,221+120,774,320+120)) selectedevent->ianimselected=min(selectedevent->ianimpos+(my-221-120) / 10,cntanimanims(selectedevent->iscene->anims)-1);
								if ((mode ==DD_Check) && mouseinwindow(661,221+120,774,323+120)) selectedevent->ianimpos-=wheel*4;
								scroller(775,220+120,790,324+120,15,15,cntanimanims(selectedevent->iscene->anims),10,selectedevent->ianimpos,mode);
								if (mode==DD_Draw)
								{
									sceneobjanim *pf = selectedevent->iscene->anims;
									for (x=1;x<=selectedevent->ianimpos;x++) pf=pf->next;
									for (x=0;x<10;x++)
									{
										if (pf!=NULL)
										{
											if (selectedevent->ianimpos+x==selectedevent->ianimselected) glColor4f(col4); else glColor4f(buttontextlit);
											glRasterPos2i(665,230+x*10+120);
											strcpy(str,pf->name);
											glPrint(str,base2,18);
											pf=pf->next;
										}
									}
								}
								sceneobjanim *ppc = selectedevent->iscene->anims;
								for (x=1;x<=selectedevent->ianimselected;x++) ppc=ppc->next;
								selectedevent->animid=ppc->number;
								selectedevent->ianim=ppc;			
				}
				if (selectedevent->menu1==1)
				{
					if (selectedevent->icam!=NULL)
					{
						int i=(int)(selectedevent->camerastart*(float)selectedevent->icam->length);
						sprintf(str,"Cam Startframe %d",i);
						ValueBar(660,140,0,0,selectedevent->icam->length,&i,str,12,13,base2,mode);
						selectedevent->camerastart=(float)i/(float)selectedevent->icam->length;
						
						i=(int)(selectedevent->cameraend*(float)selectedevent->icam->length);
						sprintf(str,"Cam Endframe %d",i);
						ValueBar(660,160,0,0,selectedevent->icam->length,&i,str,14,13,base2,mode);
						selectedevent->cameraend=(float)i/(float)selectedevent->icam->length;
					}

					if (selectedevent->ianim!=NULL)
					{
						int i=(int)(selectedevent->animstart*(float)selectedevent->ianim->length);
						sprintf(str,"Anim Startframe %d",i);
						ValueBar(660,190,0,0,selectedevent->ianim->length,&i,str,12,13,base2,mode);
						selectedevent->animstart=(float)i/(float)selectedevent->ianim->length;
						
						i=(int)(selectedevent->animend*(float)selectedevent->ianim->length);
						sprintf(str,"Anim Endframe %d",i);
						ValueBar(660,210,0,0,selectedevent->ianim->length,&i,str,13,13,base2,mode);
						selectedevent->animend=(float)i/(float)selectedevent->ianim->length;
					}
				}
			}
		}
	}
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor4f(0,0,0,1);
	glDisable(GL_DEPTH_TEST);
	quad(14,499,239,509);
	glColor4f(buttontextlit);
	sprintf(str,"Frame %.5d Pattern %.3d Xpos %.3d Ypos %.3d",icurrentframe,startpattern,mx-13,my-31);
	glRasterPos2i(14,508);
	glPrint(str,base2);
	glPopAttrib();
}

