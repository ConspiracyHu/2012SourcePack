#include "precalc.h"
#include "CNSprecalc.h"

scene *CNSprecalc=NULL;
camera pcam;

void WINAPI PrecalcAnim(float endvalue)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_LIGHTING);
	float realstatus=status/endvalue;
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
	else if (keys[VK_ESCAPE]) done=true;
	else
	{
		realstatus=status/endvalue;
		matrix m;
		object *o=CNSprecalc->objects;
		glClear(0x4100);
		glLoadIdentity();
		int x;
		for (x=0; x<=7; x++)
		{
			int y;
			for (y=0;y<o->vertexnum;y++) o->vertexlist[y].v=o->vertexlist[y].generated;
			o=o->next;
		}
		o=CNSprecalc->objects;
		for (x=0; x<=7; x++)
		{	
			m_identity(m);
			if ((realstatus>=(float)x/8.0f) && (realstatus<(float)(x+1)/8.0f)) m_xpose((float)(sin((float)x/8.0f*radtheta*360)*((float)x/8.0f-realstatus+1.0f/8.0f)*100.0f),0,(float)(cos((float)x/8.0f*radtheta*360)*((float)x/8.0f-realstatus+1.0f/8.0f)*100.0f),m);
			if (realstatus<=(float)(x)/8.0f) memset(m,0,sizeof(m));
			int y;
			for (y=0;y<o->vertexnum;y++) m_xformd(m,o->vertexlist[y].generated,o->vertexlist[y].generated);
			o=o->next;
		}

		glLoadIdentity();
		setcameraview(pcam,4.0f/3.0f);
		scene_render(*CNSprecalc);

		o=CNSprecalc->objects;
		for (x=0; x<=7; x++)
		{
			int y;
			for (y=0;y<o->vertexnum;y++) o->vertexlist[y].generated=o->vertexlist[y].v;
			o=o->next;
		}

		SwapBuffers(hDC);
	}
	glPopAttrib();

}

void PrecalcRotate(float f)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	matrix m,m1;
	object *o=CNSprecalc->objects;
	float phi;
	m_xpose(0,0,-1.002f,m1);
	if (f<=0.5) phi=(float)(f*f*810*radtheta); else phi=(float)(25.12-(1-f)*(1-f)*810*radtheta);
	m_rotate(0,1,0,phi,m);
	m_mult(m,m1,m);
	m_xpose(0,0,1.002f,m1);
	m_mult(m1,m,m);
	int x;
	for (x=0; x<=7; x++)
	{
		int y;
		for (y=0;y<o->vertexnum;y++) m_xformd(m,o->vertexlist[y].generated,o->vertexlist[y].generated);
		o=o->next;
	}
	glClear(0x4100);
	glLoadIdentity();
	setcameraview(pcam,4.0f/3.0f);
	glDisable(GL_TEXTURE_2D);
	scene_render(*CNSprecalc);
	SwapBuffers(hDC);

	o=CNSprecalc->objects;
	for (x=0; x<=7; x++)
	{
		int y;
		for (y=0;y<o->vertexnum;y++) o->vertexlist[y].generated=o->vertexlist[y].v;
		o=o->next;
	}
	glPopAttrib();
}

void WINAPI animdummy(float f)
{
}


void InitPrecalc()
{
	memset(&pcam,0,sizeof(pcam));
	pcam.up.b.z=1;
	pcam.eye.b.y=-16;
	pcam.eye.b.z=0;
	pcam.fov=45;
	tTexture *texturelist=NULL,*lasttexture=NULL;
	material *materiallist=NULL,*lastmaterial=NULL;
	LoadScene(raw_CNSprecalc,&CNSprecalc,&texturelist,&lasttexture,&materiallist,&lastmaterial,2,animdummy);status=0;
}