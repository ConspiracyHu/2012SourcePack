/************************************************************************************************************/
//
//
//                                     Project Genesis  Final Version
//
//											  full source code
//
//										 (c)  2002-2003 Conspiracy
//
//											All  rights reserved.
//
//
//	             Most of the code herein was written by BoyC, except where stated otherwise. 
//	                (most notably: synth code by Gargaj, gridfiller effect code by mrc!)
//
//
//	    The sound playback system is a heavily modified minifmod version, modifications done by Gargaj
//
//
/************************************************************************************************************/

#include "intro.h"
#include "genesis.h"
#include "pgprecalc.h"
#include "setupdialog.h"
#include "resource.h"

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
		glClear(0x4100);		
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

unsigned short originalramp[256][3],adjustedramp[256][3];


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
	}

	Intro_CreateWindow("Conspiracy - Project Genesis", x, y, 32, setupcfg.fullscreen==1, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)),setupcfg.alwaysontop==1);
	glClearColor(0,0,0,0);
	glClear(0x4100);
	initintroengine();
	initcloth();
	cnsSynth_LoaderCallback=prepre;
	calculatehierarchy[0]=true;
	loadprojectfile(raw_pgprecalc);
	generateprojectlists();
	swap();
	cnsSynth_LoaderCallback=precalc;
	loadprojectfile(raw_genesis);
	generateprojectlists();
	if (setupcfg.music)
	{
		cnsSynth_LoadMusic();
		cnsSynth_PlayMusic();
	}
	calculatehierarchy[4]=true;
	calculatehierarchy[5]=true;
	calculatehierarchy[6]=true;
	calculatehierarchy[7]=true;
	calculatehierarchy[13]=true;
	calculatehierarchy[25]=true;
	calculatehierarchy[26]=true;
	calculatehierarchy[36]=true;
	calculatehierarchy[31]=true;
	calculatehierarchy[34]=true;
	calculatehierarchy[39]=true;
	calculatehierarchy[42]=true;
	calculatehierarchy[44]=true;
	calculatehierarchy[45]=true;
}

void rotatehack(object *o,matrix r, bool objspace)
{
	o->parent=-1;
	if (objspace) m_mult(o->xformmatrix,r,o->xformmatrix);
	else m_mult(r,o->xformmatrix,o->xformmatrix);
}

void specevents(int time, float ii)
{
	// hack to rotate the gear thing

	if (time>=399500 && time<=416480)
	{
		matrix fogaskerek,fogaskereki,egeszforgas,egeszforgasi,gyuruforgas;
		m_identity(fogaskerek);
		m_identity(fogaskereki);
		m_identity(egeszforgas);
		m_identity(egeszforgasi);
		m_identity(gyuruforgas);
		m_rotate(0,1,0,(float)((float)time*pi/600.0f*ii),fogaskerek);
		m_rotate(0,1,0,(float)((float)-time*pi/600.0f*ii),fogaskereki);
		m_rotate(0,1,0,(float)((float)time*pi/1800.0f*ii),egeszforgas);
		m_rotate(0,1,0,(float)((float)-time*pi/1800.0f*ii),egeszforgasi);
		m_rotate(0,1,0,(float)((float)time*pi/700.0f*ii),gyuruforgas);

		scene *kapalogep=findscene(17);
		calculatehierarchy[17]=true;

		object *dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,42);
		rotatehack(dummy1,gyuruforgas,false);
		dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,43);
		rotatehack(dummy1,gyuruforgas,false);

		dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,38);
		rotatehack(dummy1,egeszforgas,false);

		int i;
		for (i=2; i<=24; i++)
		{
			object *dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,i);
			if (i!=24)
			rotatehack(dummy1,fogaskerek,false);
			else
			rotatehack(dummy1,fogaskereki,true);
			
			rotatehack(dummy1,egeszforgas,false);
		}
		for (i=56; i<=57; i++)
		{
			object *dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,i);
			rotatehack(dummy1,fogaskereki,true);

			rotatehack(dummy1,egeszforgas,false);
		}

		for (i=32; i<=37; i++)
		{
			object *dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,i);
			rotatehack(dummy1,egeszforgasi,false);
		}	
		
		dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,58);
		rotatehack(dummy1,fogaskerek,true);
		rotatehack(dummy1,egeszforgasi,false);
		for (i=59; i<=61; i++)
		{
			object *dummy1=searchobjectbynumber(kapalogep->objects,kapalogep->objectnum,i);
			rotatehack(dummy1,fogaskereki,true);
			rotatehack(dummy1,egeszforgasi,false);
		}/**/
	}
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
			if (setupcfg.music)
			time=(cnsSynth_GetSync()+57);
			else time=(timeGetTime()-StartTime);
			{
				glClear(0x4100);
				glLoadIdentity();

				specevents(time,1.0f);
				displayframe(time);
				specevents(time,-1.0f);

				SwapBuffers(hDC);

				lasttime=time;
			}
			
			if (time>=512000) done=true;
			
		}

	}
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
	hInstance=hInst;
	if (OpenSetupDialog(GetModuleHandle(NULL))) 
	{
		Init();
		mainloop();
		KillGLWindow();
	}
	return 0;
}