#include "intro.h"
#define fullscr false

int SceneNum = 2;

scene *CNSlogo=NULL,*copytest=NULL;
tTexture *texturelist=NULL,*lasttexture=NULL;
material *materiallist=NULL,*lastmaterial=NULL;
matrix m;

camera cam;

void Init()
{
	memset(&cam,0,sizeof(cam));
	cam.up.b.z=1;
	cam.eye.b.y=-16;
	cam.eye.b.z=0;
	cam.fov=45;
	inittextureengine();
	InitPrecalc();
	//LoadScene(raw_copytest,&copytest,&texturelist,&lasttexture,&materiallist,&lastmaterial,SceneNum,PrecalcAnim);
	LoadScene(raw_alienprobe,&CNSlogo,&texturelist,&lasttexture,&materiallist,&lastmaterial,SceneNum,PrecalcAnim);
	status=0;
	while (status<=1)
	{
	 PrecalcRotate(status);
	 status+=0.005f;
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	m_rotate(0.9f,1.0f,0.7f,radtheta,m);
}

void MainLoop()
{
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
			glClear(0x4100);
			glLoadIdentity();
			m_xform3(m,cam.eye,cam.eye);
			m_xform3(m,cam.up,cam.up);
			setcameraview(cam,4.0f/3.0f);
			scene_render(*CNSlogo);
			//scene_render(*copytest);
			SwapBuffers(hDC);
			Sleep(10);
		}

	}
}


INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
	Intro_CreateWindow("a.D.D.i.c.t. Engine Test", 800, 600, 32, fullscr, LoadIcon(hInstance, MAKEINTRESOURCE(101)));
	Init();
	MainLoop();
	KillGLWindow();
	return 0;
}