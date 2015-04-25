#ifndef __DDwin__
#define __DDwin__

//#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <gl\gl.h>
#include <gl\glu.h>

extern HDC			hDC;
extern HGLRC		hRC;
extern HWND		hWnd;
extern HINSTANCE	hInstance;

extern int		mx,my,lx,ly,rx,ry,mbx,mby,wheel,fwKeys;
extern bool    leftbutton,rightbutton,middlebutton,lbuttonreleased,rbuttonreleased,mbuttonreleased;

extern bool	keys[256];
extern bool	active;
extern bool	fullscreen;

extern bool    done;
extern int     window_xsize,window_ysize;

struct tkey {
				byte key;
				tkey *next;
			};

extern tkey *keyboardbuffer;
extern tkey *key;
extern MSG msg;
extern bool mode3d;

extern GLuint base,basesmall,base2;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
GLvoid KillGLWindow(GLvoid);
BOOL DD_CreateWindow(char* title, int width, int height, int bits, bool fullscreenflag, HICON icon);
bool mouseinwindow( float x1,float y1,float x2,float y2);
bool mouseinwindow( int x1,int y1,int x2,int y2);
bool leftclickinwindow( float x1,float y1,float x2,float y2);
bool leftclickinwindow( int x1,int y1,int x2,int y2);
bool rightclickinwindow( float x1,float y1,float x2,float y2);
bool rightclickinwindow( int x1,int y1,int x2,int y2);
bool midclickinwindow( float x1,float y1,float x2,float y2);
bool midclickinwindow( int x1,int y1,int x2,int y2);
void waitforleftbuttonrelease();
void waitforrightbuttonrelease();
void switchto2d();
GLvoid BuildFont(GLvoid);
GLvoid BuildFontSmall(GLvoid);
GLvoid BuildFont2(GLvoid);
void glPrint(const char* text, GLuint font);
void glPrint(const char* text, GLuint font, int i);
void glPrintCenter(const char* text, GLuint font, int x1, int y1, int x2, int y2);

#endif