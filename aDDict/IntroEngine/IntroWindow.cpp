#include "IntroWindow.h"

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;

bool    done = false;
int     window_xsize=0,window_ysize = 0;
bool	mode3d = true;

GLuint	base,basesmall;
MSG msg;

GLvoid KillGLWindow(GLvoid)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

}

BOOL Intro_CreateWindow(char* title, int width, int height, int bits, bool fullscreenflag, HICON icon)
{
	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)height;

	fullscreen=fullscreenflag;
	window_xsize=width;
	window_ysize=height;

	hInstance			= GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= icon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";

	RegisterClass(&wc);
	
	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= bits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
	}

	if (fullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
	    dwExStyle=WS_EX_APPWINDOW + WS_EX_WINDOWEDGE;
        dwStyle=WS_OVERLAPPED+ WS_CAPTION+ WS_SYSMENU+WS_MINIMIZEBOX;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!fullscreenflag)

	       hWnd=CreateWindowEx(	dwExStyle,
								"OpenGL",
								title,
								dwStyle |
								WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN,
								112, 64,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,
								NULL,
								hInstance,
								NULL);
	else
	       hWnd=CreateWindowEx(	dwExStyle,
								"OpenGL",
								title,
								dwStyle |
								WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN,
								0, 0,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,
								NULL,
								hInstance,
								NULL);

	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	
	hDC=GetDC(hWnd);
	PixelFormat=ChoosePixelFormat(hDC,&pfd);
	SetPixelFormat(hDC,PixelFormat,&pfd);
	hRC=wglCreateContext(hDC);
	wglMakeCurrent(hDC,hRC);
	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	glEnable(GL_DEPTH_TEST);

	return TRUE;									
}

LRESULT CALLBACK WndProc(	HWND	hWnd,
							UINT	uMsg,
							WPARAM	wParam,
							LPARAM	lParam)
{
	switch (uMsg)						
	{
		case WM_ACTIVATE:				
		{
			active=(!HIWORD(wParam));
			return 0;					
		}

		case WM_SYSCOMMAND:				
		{
			switch (wParam)				
			{
				case SC_SCREENSAVE:		
				case SC_MONITORPOWER:	
				return 0;				
			}
			break;						
		}

		case WM_CLOSE:					
		{
			done = true;
			return 0;					
		}

		case WM_KEYDOWN:				
		{
			keys[wParam] = true;
			return 0;					
		}

		case WM_KEYUP:					
		{
			keys[wParam] = true;
			return 0;
		}

	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
