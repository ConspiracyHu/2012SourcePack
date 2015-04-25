#include "intro.h"
#include "genesis.h"
#include "precalc.h"
#include "setupdialog.h"
#include "resource.h"
//#include "pipes.h"
//539843 long

//#define savevideo

//#include <stdio.h>
//char *ss = new char[256];
//sprintf(ss,"%d %f %f %f",sizeof(v),v.x,v.y,v.z);
//MessageBox( 0, ss, "HelloWorld", MB_OK );

#ifdef savevideo
#include <stdio.h>

unsigned char bmpheader[56] =
{
	0x42,0x4d,0x38,0x10,0x0e,0,0,0,0,0,0x36,0,0,0,0x28,0,0,0,0x80,0x02,0,0,0xe0,0x01,0,0,1,0,0x18,0,0,0,0,0,0,0,0,0,0x12,0x0b,0,0,0x12,0x0b,0,0,0,0,0,0,0,0,0,0,0,0x0a
};
byte framebuffer[640][480][3];
int timer=0;
FILE *frame;
char fname[40];

#endif

int time,lasttime;
int starttime;
camera perspcam;

void swap()
{
	void *buffer;
	buffer=texturelist;
	texturelist=ptexturelist;
	ptexturelist=(texture*)buffer;

	buffer=materiallist;
	materiallist=pmateriallist;
	pmateriallist=(material*)buffer;

	buffer=eventlist;
	eventlist=peventlist;
	peventlist=(event*)buffer;

	buffer=lastevent;
	lastevent=plastevent;
	plastevent=(event*)buffer;

	buffer=scenelist;
	scenelist=pscenelist;
	pscenelist=(scene*)buffer;
}

void __stdcall prepre(float f) 
{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
	else
	{
		//switchto2d();
		glClear(0x4100);		
		/*glBegin(GL_QUADS);
		glVertex2f(0,510);
		glVertex2f(400*f,510);
		glVertex2f(400*f,511);
		glVertex2f(0,511);
		glEnd();*/
		SwapBuffers(hDC);
	}
}

void __stdcall precalc(float f) 
{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
	else
	{
		glClear(0x4100);
		glLoadIdentity();
		swap();
		displayframe((int)(f*5000.0f));
		swap();
		switchto2d();
		SwapBuffers(hDC);
	}
}

void Init()
{
	inittextureengine();
	int x,y;
	switch (setupcfg.resolution)
	{
	case 0: x=320; y=240; break;
	case 1: x=400; y=300; break;
	case 2: x=512; y=384; break;
	case 3: x=640; y=480; break;
	case 4: x=800; y=600; break;
	case 5: x=1024; y=768; break;
	//case 6: x=1152; y=864; break;
	//case 7: x=1280; y=960; break;
	//case 8: x=1600; y=1200; break;
	}

	Intro_CreateWindow("Conspiracy - A Place Called Universe", x, y, 32, setupcfg.fullscreen==1, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)),setupcfg.alwaysontop==1);
	glClearColor(0,0,0,0);
	glClear(0x4100);
	initintroengine();
	cnsSynth_LoaderCallback=prepre;
	calculatehierarchy[0]=true;
	loadprojectfile(raw_precalc);
	generateprojectlists();
	swap();
	cnsSynth_LoaderCallback=precalc;
	loadprojectfile(raw_genesis);
	generateprojectlists();
#ifndef savevideo
	if (setupcfg.music)
	{
		cnsSynth_LoadMusic();
		cnsSynth_PlayMusic();
	}
#endif
	memset(calculatehierarchy,1,sizeof(calculatehierarchy));
#ifdef savevideo	

#endif
}

void rotatehack(object *o,matrix r, bool objspace)
{
	o->parent=-1;
	if (objspace) m_mult(o->xformmatrix,r,o->xformmatrix);
	else m_mult(r,o->xformmatrix,o->xformmatrix);
	//obj_transform(o,o->xformmatrix);
}

void mainloop()
{
	time=lasttime=0;
	int StartTime=timeGetTime();
	while (!done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
		else if (keys[VK_ESCAPE]) done=true;
		else
		{
#ifndef savevideo
			if (setupcfg.music)
			time=(cnsSynth_GetSync()+20);
			else time=(timeGetTime()-StartTime);
#else
			time=timer;
			timer+=3;
#endif
			
#ifndef savevideo
			//if (lasttime<time)
			{
#endif
				glClear(0x4100);
				glLoadIdentity();

				displayframe(time);
				//0,45,32 es 0,22,63

				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glColor4f(0,linear(45.0f/255.0f,22.0f/255.0f,(float)time/271790.0f),linear(32.0f/255.0f,63.0f/255.0f,(float)time/271790.0f),0);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_LIGHTING);
				glDisable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
				glBegin(GL_LINES);
				glVertex2i(0,75);
				glVertex2i(800,75);
				glVertex2i(0,525);
				glVertex2i(800,525);				
				glEnd();
				glPopAttrib();

				//Sleep(10);

#ifdef savevideo
				glReadPixels(0,0,xres,yres,GL_RGB,GL_UNSIGNED_BYTE,framebuffer);
				sprintf(fname,"frames\\%.5d.bmp",timer/3);
				frame=fopen(fname,"w+b");
				byte a;
				for (int x=0; x<640; x++)
					for (int y=0; y<480; y++)
					{
						a=framebuffer[x][y][1];
						framebuffer[x][y][1]=framebuffer[x][y][2];
						framebuffer[x][y][2]=a;
					}
				fwrite(bmpheader,56,1,frame);
				fwrite(framebuffer,xres*yres,3,frame);

				fclose(frame);

#endif
				SwapBuffers(hDC);

#ifndef savevideo
				lasttime=time;
			}
			//Sleep(10);
#endif


			if (time>=271790) done=true;
			
		}

	}
}

typedef void (APIENTRY * WGLSWAPINTERVALEXT) (int);

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
	hInstance=hInst;
	if (OpenSetupDialog(GetModuleHandle(NULL))) 
	{
		Init();
		WGLSWAPINTERVALEXT wglSwapIntervalEXT = (WGLSWAPINTERVALEXT) wglGetProcAddress("wglSwapIntervalEXT");
		if (wglSwapIntervalEXT) wglSwapIntervalEXT(1); // enable vertical synchronisation
		mainloop();
		//killprojectlists();
		KillGLWindow();
	}
	return 0;
}