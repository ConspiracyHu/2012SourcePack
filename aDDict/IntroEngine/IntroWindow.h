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
extern bool	fullscreen;

extern bool    done;
extern int     window_xsize,window_ysize;
extern bool	mode3d;

extern GLuint	base,basesmall;
extern MSG msg;

GLvoid KillGLWindow(GLvoid);
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL Intro_CreateWindow(char* title, int width, int height, int bits, bool fullscreenflag, HICON icon);

#endif