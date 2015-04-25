#ifndef __windowhandler__
#define __windowhandler

#include "memtrack.h"

#include <stdio.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#define DEMOSAVE_LeftButton 0
#define DEMOSAVE_RightButton 1
#define DEMOSAVE_MiddleButton 2
#define DEMOSAVE_Wheel 3
#define DEMOSAVE_Key 4

extern FILE *DebugText;

extern HDC			hDC;
extern HGLRC		hRC;
extern HWND		    hWnd;
extern HINSTANCE	hInstance;

extern bool	Keys[256];
extern bool	Active;

extern bool Done,WaitLeftButton,WaitMiddleButton,WaitRightButton;

extern GLuint	base,basesmall;
extern MSG msg;
extern int Wheel;

extern int XRes,YRes,mx,my,lx,ly,rx,ry,mbx,mby,wheel,fwKeys,Wheel;
extern bool LeftShift,LeftCtrl,RightShift,RightCtrl,MidShift,MidCtrl;
extern char *CommandLine;
extern bool CommandLineEnter;
extern bool CommandLineReading;
extern bool CommandLineCommand;
extern bool LeftButton,RightButton,MiddleButton;
extern bool	Transforming,Cammoving;
extern bool	TransformingWhenRightClicked,TransformingWhenMidClicked,CammovingWhenRightClicked;

extern HCURSOR Crosshair,Arrow,Horizontal,Vertical,HV1,HV2,Hand;
extern HCURSOR CurrentCursor;
extern bool DebugStart;
extern bool DemoPlayback;
extern int DemoSaveStartTime;
extern FILE *DemoSave;

GLvoid KillGLWindow(GLvoid);
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL Intro_CreateWindow(char* title, int width, int height, int bits, bool fullscreenflag, HICON icon);

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

void SwitchTo2D();

void WriteDebug(const char *fmt, ...);

void InitDemoSave();
void DeinitDemoSave();
void DeinitDemoPlayback();
void InitDemoPlayback();
void PlayBackFunction();

#endif