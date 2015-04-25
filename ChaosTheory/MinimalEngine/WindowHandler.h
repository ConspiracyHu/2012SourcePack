#pragma once

#include <windows.h>
#include "SetupDialog.h"
#include <gl\gl.h>
#include <gl\glu.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

extern HDC			hDC;
extern HGLRC		hRC;
extern HWND			hWnd;
extern HINSTANCE	hInstance;

extern MSG msg;

extern bool	Keys[256];
extern bool	Active;
extern bool Done;

extern int XRes,YRes;

void Intro_CreateWindow(char* Title, DEVMODE m, bool FullScreenFlag, HICON Icon, bool);
void Intro_DestroyWindow(bool);
void SwitchTo2D();

//void WriteDebug(const char *fmt, ...);

#ifdef MINIMALTWEAKER

extern int XRes,YRes,mx,my,lx,ly,rx,ry,mbx,mby,wheel,fwKeys,Wheel;
extern bool LeftShift,LeftCtrl,RightShift,RightCtrl,MidShift,MidCtrl;
extern bool LeftButton,RightButton,MiddleButton;

bool MouseInWindow( float x1,float y1,float x2,float y2);
bool MouseInWindow( int x1,int y1,int x2,int y2);
bool LeftClickInWindow( float x1,float y1,float x2,float y2);
bool LeftClickInWindow( int x1,int y1,int x2,int y2);
bool RightClickInWindow( float x1,float y1,float x2,float y2);
bool RightClickInWindow( int x1,int y1,int x2,int y2);
bool MidClickInWindow( float x1,float y1,float x2,float y2);
bool MidClickInWindow( int x1,int y1,int x2,int y2);

void WaitLeftButtonRelease();
void WaitMiddleButtonRelease();
void WaitRightButtonRelease();
#endif