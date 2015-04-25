#ifndef __IntroWindow__
#define __IntroWindow__

#include <windows.h>
#include <winuser.h>
#include <gl\gl.h>
#include <gl\glu.h>

extern HDC			hDC;
extern HGLRC		hRC;
extern HWND		    hWnd;
extern HINSTANCE	hInstance;

extern bool	keys[256];
extern bool	active;

extern bool    done;
extern bool	mode3d;

extern GLuint	base,basesmall;
extern MSG msg;

extern int xres,yres;

GLvoid KillGLWindow(GLvoid);
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL Intro_CreateWindow(char* title, int width, int height, int bits, bool fullscreenflag, HICON icon, bool aontop);
void switchto2d();

#endif