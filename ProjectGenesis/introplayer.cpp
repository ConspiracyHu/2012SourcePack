#include "introplayer.h"

#define GRIDXSIZE 64
#define GRIDYSIZE 48
float sqrr(float n) { return n*n; }
double offset[GRIDXSIZE+1][GRIDYSIZE+1][2];	// offset
int clothtimer=422130;


void InitEffect(float u1, float v1, float u2, float v2) 
{
	// Code done by mrc!

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

		}
	}
}

void drawgrid(float grid[GRIDXSIZE+1][GRIDYSIZE+1][2], float x1, float y1, float x2, float y2)
{
	// Code done by mrc!

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

	// Code done by mrc!

	int i,j;			// looper
	float reli,relj;		// normalized loop values
	float reli2,relj2;
	float angle;
	float u,v;
	float _x1,_y1,_x2,_y2;	// temp screen coords
	float deltau=(u2-u1)/GRIDXSIZE;
	float deltav=(v2-v1)/GRIDYSIZE;
	float deltax=(x2-x1)/GRIDXSIZE;
	float deltay=(y2-y1)/GRIDYSIZE;

	float f;				// temp shit

	float grid[GRIDXSIZE+1][GRIDYSIZE+1][2];	// da mighty grid :)

	if (time<0) { time=0; }
	if (time>1.0) { time=1.0; }
	u1+=0.002f;
	u2-=0.002f;
	v1-=0.002f;
	v2+=0.002f;

	switch (fxtype) {
	case 0: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						u=(float)( (u2-u1)*reli+u1 + (sin(time*2.0f+reli*20.0f+relj*15.0f)*0.05f + cos(time*2.0f+reli*10+relj*25.0f)*0.05f) * sqrr((float)sin(time*(float)pi*0.5f)));
						if (u<u1) { u=u1; }
						if (u>u2) { u=u2; }
						v=(float)((v2-v1)*relj+v1 + (sin(time*2+reli*18+relj*25)*0.05f + sin(time*2+reli*28+relj*15)*0.05) * sqrr((float)sin(time*pi*0.5)));
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
						f=(float)(sqrt((float)sqrr(reli-0.5f)+(float)sqrr(relj-0.5f))*2.0f);
						f=(float)pow(f,time*3.5f);
						f*=(float)sqrr((float)sin(time*(float)pi));
						grid[i][j][0]=(float)(((u2-u1)*reli+u1-0.5*(u1+u2))*(1-f)+0.5*(u1+u2));
						grid[i][j][1]=(float)(((v2-v1)*relj+v1-0.5*(v1+v2))*(1-f)+0.5*(v1+v2));
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
						f=(float)(sqrt((float)sqrr(reli-0.5f)+(float)sqrr(relj-0.5f))*2);
						if (f>0.001) {
						f=(float)pow(f,1/(4*sqrr((float)sin(time*(float)pi))+1)-1);
						} else { f=0; }
						grid[i][j][0]=(float)(((u2-u1)*reli+u1-0.5*(u1+u2))*(f)+0.5*(u1+u2));
						grid[i][j][1]=(float)(((v2-v1)*relj+v1-0.5*(v1+v2))*(f)+0.5*(v1+v2));
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 3: {
				
				time *=3.6f;
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						u=(float)((u2-u1)*reli+u1 + sqrr((float)sin(time*(float)pi/180*50))*offset[i][j][0]);
						if (u<u1) { u=u1; }
						if (u>u2) { u=u2; }
						v=(float)((v2-v1)*relj+v1 + sqrr((float)sin(time*(float)pi/180*50))*offset[i][j][1]);
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
				time *=3.6f;
				
				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						grid[i][j][0]=(float)((u2-u1)*reli+u1 + sqrr((float)sin(time*(float)pi/180*50))*offset[i][j][0]);
						grid[i][j][1]=(float)((v2-v1)*relj+v1 + sqrr((float)sin(time*(float)pi/180*50))*offset[i][j][1]);
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
	case 6: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						reli2=reli-0.5f;
						relj2=relj-0.5f;
						f=(float)sqrt(sqrr(reli2)+sqrr(relj2));

						if (f>0.01) {
							angle=(float)atan(relj2/reli2);
						} else {
							angle=0;
						}
						if (reli2<0) { angle+=(float)pi; }
						if (f<0.5) {
							if (i1) {
								angle+=25*sqrr(0.5f-f)*sqrr((float)sin(time*(float)pi*0.5f));
							} else {
								angle-=25*sqrr(0.5f-f)*sqrr((float)sin(time*(float)pi*0.5f));
							}
						}

						reli=(float)(f*cos(angle)+0.5f);
						relj=(float)(f*sin(angle)+0.5f);

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
						reli2=reli-0.5f;
						relj2=relj-0.5f;
						f=(float)sqrt(sqrr(reli2)+sqrr(relj2));

						if (f>0.01) {
							angle=(float)atan(relj2/reli2);
						} else {
							angle=0;
						}
						if (reli2<0) { angle-=(float)pi; }

						f-=(float)(sqrr((float)sin(time*(float)pi/180*400+f*10))*0.2f * sqrr((float)sin(time*pi/180*180)));
						

						reli=(float)(f*cos(angle)+0.5);
						relj=(float)(f*sin(angle)+0.5);

						grid[i][j][0]=(u2-u1)*reli+u1;
						grid[i][j][1]=(v2-v1)*relj+v1;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	}

}

void quad(float x1, float y1, float x2,float y2)
{
	glBegin(GL_QUADS);
	 glVertex2f(x1,y1);
	 glVertex2f(x2,y1);
	 glVertex2f(x2,y2);
	 glVertex2f(x1,y2);
	glEnd();
}

float linear(float a, float b, float t)
{
	return a+(b-a)*t;
}

objanim *findobjanim(objanim *olist, int id)
{
	for (objanim *ol=olist;ol;ol=ol->next) if (ol->number==id) return ol;
	return NULL;
}

void getscenestate(scene *pf,float animtimer,int ianim)
{
	for (int o=0; o<pf->objectnum; o++)
	if (pf->objects[o].data.primitive!=9 && 
		pf->objects[o].data.primitive!=11 &&
		pf->objects[o].data.primitive<100)
	{
		m_identity(pf->objects[o].currentmatrix);
		memset(&pf->objects[o].orient,0,sizeof(orientation));
		pf->objects[o].orient.stretch.x=1;
		pf->objects[o].orient.stretch.y=1;
		pf->objects[o].orient.stretch.z=1;
		objanim *oa=findobjanim(pf->objects[o].anims,ianim);

		if (oa->posx && oa->posx->numkey)
		{
			pf->objects[o].orient.position.x=oa->posx->GetKey(animtimer);
			pf->objects[o].orient.position.y=oa->posy->GetKey(animtimer);
			pf->objects[o].orient.position.z=oa->posz->GetKey(animtimer);

			pf->objects[o].orient.rotaxis.x=oa->rotx->GetKey(animtimer);
			pf->objects[o].orient.rotaxis.y=oa->roty->GetKey(animtimer);
			pf->objects[o].orient.rotaxis.z=oa->rotz->GetKey(animtimer);
			pf->objects[o].orient.rotangle=oa->rota->GetKey(animtimer);
			if (pf->objects[o].orient.rotaxis.x==0 &&
				pf->objects[o].orient.rotaxis.y==0 &&
				pf->objects[o].orient.rotaxis.z==0)
			{
				pf->objects[o].orient.rotaxis.x=1;
				pf->objects[o].orient.rotangle=0;
			}

			pf->objects[o].orient.stretch.x=oa->strx->GetKey(animtimer);
			pf->objects[o].orient.stretch.y=oa->stry->GetKey(animtimer);
			pf->objects[o].orient.stretch.z=oa->strz->GetKey(animtimer);

			pf->objects[o].color[0]=oa->colr->GetKey(animtimer);
			pf->objects[o].color[1]=oa->colg->GetKey(animtimer);
			pf->objects[o].color[2]=oa->colb->GetKey(animtimer);
			pf->objects[o].color[3]=oa->cola->GetKey(animtimer);

			object *obj=&(pf->objects[o]);

			matrix m,a,b;
			m_identity(a); m_identity(b);
			m_scale(obj->orient.stretch.x,obj->orient.stretch.y,obj->orient.stretch.z,b);
			m_mult(a,b,m);

			m_identity(a); m_identity(b);
			m_rotate(obj->orient.rotaxis.x,obj->orient.rotaxis.y,obj->orient.rotaxis.z,obj->orient.rotangle*(float)radtheta,b);
			m_mult(a,b,a);
			m_mult(m,a,m);

			m_identity(a); m_identity(b);
			m_xpose(obj->orient.position.x,obj->orient.position.y,obj->orient.position.z,b);
			m_mult(a,b,a);
			m_mult(m,a,obj->currentmatrix);
		}

	}
}

void calculateobjhierarchy(scene *actualscene,int parentid, object *parent)
{
	for (int on=0;on<actualscene->objectnum;on++)
	{
		object *o=&(actualscene->objects[on]);
		if (o->data.primitive<100 && o->parent==parentid)
		{
			matrix relative,parentmatrix;

			memcpy(relative,o->xformmatrix,sizeof(matrix));
			m_identity(parentmatrix);
			
			if (parentid!=-1)
			{
				memcpy(parentmatrix,parent->xformmatrix,sizeof(matrix));

				matrix i;
				m_invert(parentmatrix,i);
				memcpy(parentmatrix,i,sizeof(matrix));

				m_mult(relative,parentmatrix,relative); 

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
				m_invert(o->buffermatrix,i);
				m_mult(parent->currentmatrix,o->buffermatrix,m);
				m_mult(m,o->currentmatrix,m);
				m_mult(i,m,o->currentmatrix);
			}

			memcpy(o->xformmatrix,o->buffermatrix,sizeof(matrix));
			calculateobjhierarchy(actualscene,o->number,o);
		}
	}
}

camera *lastcam;

void drawevent(event *e, float t)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	float x1,y1,x2,y2;

	x1=(float)floor(linear((float)e->startrectx1,(float)e->endrectx1,t));
	y1=(float)floor(linear((float)e->startrecty1,(float)e->endrecty1,t));
	if (xres==320) y1+=0.5; if (xres==1024) y1+=1;
	x2=(float)floor(linear((float)e->startrectx2,(float)e->endrectx2,t))+1;
	y2=(float)floor(linear((float)e->startrecty2,(float)e->endrecty2,t))+1;

	if (e->eventtype==layer2d)
	{
		glDepthMask(false);
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(0,0,xres,yres);
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
		float animtimer=linear((float)e->animstart,(float)e->animend,t);
		scene *pf = e->iscene;
		camera *cam=e->icam;
		
		if (calculatehierarchy[pf->number])
		{
			getscenestate(pf,animtimer,e->ianim);		
			calculateobjhierarchy(pf,-1,NULL);
		}
		
		float f=linear((float)e->camstart,(float)e->camend,t);

		if (cam->eyex)
		{
			cam->eye.x=cam->eyex->GetKey(f);
			cam->eye.y=cam->eyey->GetKey(f);
			cam->eye.z=cam->eyez->GetKey(f);
			cam->target.x=cam->trgx->GetKey(f);
			cam->target.y=cam->trgy->GetKey(f);
			cam->target.z=cam->trgz->GetKey(f);
			cam->fov=cam->fovt->GetKey(f);
			cam->roll=cam->rollt->GetKey(f);
		}

		glLoadIdentity();
		if (!cam->eyex || (cam->eyex && cam->eyex->numkey==0)) cam=lastcam;
		if (lastcam) setcameraview(*cam,(float)(x2-x1)/(float)(y2-y1));
		lastcam=cam;
		glViewport((int)(x1*(float)xres/640.0),(int)(yres-y2*(float)yres/480.0),(int)((x2-x1)*(float)xres/640.0),(int)((y2-y1)*(float)yres/480.0));
		glEnable(GL_BLEND);
		glEnable(GL_LIGHTING);
		renderscene(pf);
	}
	if (e->eventtype==cleargl)
	{
		if (e->clearzbuffer && !e->clearscreen) glClear(0x100);
		if (!e->clearzbuffer && e->clearscreen) glClear(0x4000);
		if (e->clearzbuffer && e->clearscreen) glClear(0x4100);
	}
	if (e->eventtype==rendertotext)
	{
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(0,0,xres,yres);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,shots[e->texture].texture);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,(int)(x1*(float)xres/640.0),(int)(yres-y2*(float)yres/480.0), 1024, 1024,0);
		shots[e->texture].x1=0;
		shots[e->texture].y1=(y2-y1)*(float)xres/640.0f/1024.0f;
		shots[e->texture].x2=(x2-x1)*(float)yres/480.0f/1024.0f;
		shots[e->texture].y2=0;
	}
	if (e->eventtype==feedback)
	{
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(0,0,xres,yres);
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

		for (int x=0; x<e->param1; x++)
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
		float time=linear(e->gridstart,e->gridend,t);
		glLoadIdentity();
		gluOrtho2D (0, 640, 480,0);
		glViewport(0,0,xres,yres);
		glColor4f(linear(e->startcol[0],e->endcol[0],t),linear(e->startcol[1],e->endcol[1],t),linear(e->startcol[2],e->endcol[2],t),linear(e->startcol[3],e->endcol[3],t));
		glEnable(GL_BLEND);
		glBlendFunc(e->blendfunc1,e->blendfunc2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,shots[e->texture].texture);
		MakeEffect(e->effect,time,x1,y1,x2,y2, shots[e->texture].x1, shots[e->texture].y1, shots[e->texture].x2, shots[e->texture].y2,
				0, 0, 0, 0, 0, 0, 0, 0);
	}
	switchto2d();
	glPopAttrib();
}

void docloth(int time, event *e, float t)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	float x1,y1,x2,y2;
	x1=(float)floor(linear((float)e->startrectx1,(float)e->endrectx1,t));
	y1=(float)floor(linear((float)e->startrecty1,(float)e->endrecty1,t));
	x2=(float)floor(linear((float)e->startrectx2,(float)e->endrectx2,t))+1;
	y2=(float)floor(linear((float)e->startrecty2,(float)e->endrecty2,t))+1;
	setcameraview(*lastcam,(float)(x2-x1)/(float)(y2-y1));
	glViewport((int)(x1*(float)xres/640.0),(int)(yres-y2*(float)yres/480.0),(int)((x2-x1)*(float)xres/640.0),(int)((y2-y1)*(float)yres/480.0));
	for (;clothtimer<time;)
	{
		calculatecloth();
		clothtimer+=10;
	}
	glEnable(GL_FOG);
	scene *s=findscene(1);
	glFogfv(GL_FOG_COLOR,s->fogcol);
	glFogf(GL_FOG_DENSITY,s->fogdensity);
	setlighting(s);
	drawcloth();
	switchto2d();
	glPopAttrib();
}

void displayframe(int icurrentframe)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	for (event *e=eventlist; e; e=e->next)
	{
		if (icurrentframe>=e->startframe && icurrentframe<=e->endframe) 
		{
			drawevent(e,(float)(icurrentframe-e->startframe)/(float)(e->endframe-e->startframe));
			if ((e->startframe==422130 && e->endframe==451449) ||
				(e->startframe==451450 && e->endframe==473539) ||
				(e->startframe==473540 && e->endframe==509729))
				docloth(icurrentframe,e,(float)(icurrentframe-e->startframe)/(float)(e->endframe-e->startframe));
		}
	}
	glPopAttrib();
}
