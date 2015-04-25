#include "DDwindow.h"

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

int		mx,my,lx,ly,rx,ry,mbx,mby,wheel,fwKeys;
bool    leftbutton,rightbutton,middlebutton,lbuttonreleased,rbuttonreleased,mbuttonreleased = false;

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;

bool    done = false;
int     window_xsize,window_ysize = 0;
bool	mode3d = true;
GLuint	base,basesmall,base2;

tkey *keyboardbuffer=NULL;
tkey *key=NULL;
MSG msg;


GLvoid KillGLWindow(GLvoid)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

}

BOOL DD_CreateWindow(char* title, int width, int height, int bits, bool fullscreenflag, HICON icon)
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
	//	ShowCursor(FALSE);
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
			//PostQuitMessage(0);
			done = true;
			return 0;					
		}

		case WM_KEYDOWN:				
		{
			/*char *ss = new char[256];
			sprintf(ss,"%d",wParam);
			MessageBox( 0, ss, "HelloWorld", MB_OK );*/
			keys[wParam] = true;
			return 0;					
		}

		case WM_KEYUP:					
		{
			keys[wParam] = false;
			return 0;
		}

		case WM_CHAR: 
		{
			key = new tkey;
			key->next=keyboardbuffer;
			key->key=wParam;
			keyboardbuffer=key;
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			POINT ap;
			GetCursorPos(&ap);
			ScreenToClient(hWnd,&ap);
			mx=ap.x;
			my=ap.y;
			return 0;
		}

        case WM_LBUTTONDOWN:
        {
            leftbutton=true;
			POINT ap;
			GetCursorPos(&ap);
			ScreenToClient(hWnd,&ap);
            lx=ap.x;
            ly=ap.y;
			return 0;
        }
        
		case WM_LBUTTONUP:
        {
            leftbutton=false;
            lbuttonreleased=true;
			return 0;
        }
        
		case WM_RBUTTONDOWN:
        {
            rightbutton=true;
			POINT ap;
			GetCursorPos(&ap);
			ScreenToClient(hWnd,&ap);
            rx=ap.x;
            ry=ap.y;
			return 0;
        }
        
		case WM_RBUTTONUP:
        {
            rightbutton=false;
            rbuttonreleased=true;
			return 0;
        }
        
		case WM_MBUTTONDOWN:
        {
            middlebutton=true;
			POINT ap;
			GetCursorPos(&ap);
			ScreenToClient(hWnd,&ap);
            mbx=ap.x;
            mby=ap.y;
			return 0;
        }
        
		case WM_MBUTTONUP:
        {
            middlebutton=false;
            mbuttonreleased=true;
			return 0;	
        }

        case 0x020A:
        {
            //fwKeys = GET_KEYSTATE_WPARAM(wParam);
            //wheel  = GET_WHEEL_DELTA_WPARAM(wParam);
			wheel  = (int)((int)(wParam) / 7854320);
			return 0;
        }
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

bool mouseinwindow( float x1,float y1,float x2,float y2)
{
 return ((mx>=x1) && (mx<=x2) && (my>=y1) && (my<=y2));
}

bool mouseinwindow( int x1,int y1,int x2,int y2)
{
 return ((mx>=x1) && (mx<=x2) && (my>=y1) && (my<=y2));
}

bool leftclickinwindow( float x1,float y1,float x2,float y2)
{
 return ((lx>=x1) && (lx<=x2) && (ly>=y1) && (ly<=y2) && leftbutton);
}

bool leftclickinwindow( int x1,int y1,int x2,int y2)
{
 return ((lx>=x1) && (lx<=x2) && (ly>=y1) && (ly<=y2) && leftbutton);
}

bool rightclickinwindow( float x1,float y1,float x2,float y2)
{
 return ((rx>=x1) && (rx<=x2) && (ry>=y1) && (ry<=y2) && rightbutton);
}

bool rightclickinwindow( int x1,int y1,int x2,int y2)
{
 return ((rx>=x1) && (rx<=x2) && (ry>=y1) && (ry<=y2) && rightbutton);
}

bool midclickinwindow( float x1,float y1,float x2,float y2)
{
 return ((mbx>=x1) && (mbx<=x2) && (mby>=y1) && (mby<=y2) && middlebutton);
}

bool midclickinwindow( int x1,int y1,int x2,int y2)
{
 return ((mbx>=x1) && (mbx<=x2) && (mby>=y1) && (mby<=y2) && middlebutton);
}

void waitforleftbuttonrelease()
{
	MSG msg;
	while (leftbutton)
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) done=true;
			else
			{ 
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
}

void waitforrightbuttonrelease()
{
	MSG msg;
	while (rightbutton)
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) done=true;
			else
			{ 
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
}


GLvoid BuildFont(GLvoid)
{
	HFONT	font;
	HFONT	oldfont;

	base = glGenLists(96);

	font = CreateFont(	-15,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Courier New");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

GLvoid BuildFont2(GLvoid)
{
	HFONT	font;
	HFONT	oldfont;

	base2 = glGenLists(96);

	font = CreateFont(	12,							// Height Of Font
						6,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_NORMAL,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base2);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}


GLvoid KillFont(GLvoid)
{
	glDeleteLists(base, 96);
}

GLvoid BuildFontSmall(GLvoid)
{
	HFONT	font;
	HFONT	oldfont;

	basesmall = glGenLists(96);

	font = CreateFont(	-10,							// Height Of Font
						5,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Courier New");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, basesmall);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

GLvoid KillFontSmall(GLvoid)
{
	glDeleteLists(basesmall, 96);
}


void glPrint(const char* text, GLuint font)
{
	glPushAttrib(GL_LIST_BIT);
	glListBase(font - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

void glPrint(const char* text, GLuint font, int i)
{
	glPushAttrib(GL_LIST_BIT);
	glListBase(font - 32);
	glCallLists(min((int)strlen(text),i), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

void glPrintCenter(const char* text, GLuint font, int x1, int y1, int x2, int y2)
{
	int xw,yw;
	if (font==base2) xw=7;
	if (font==base2) yw=12;
	int xs=xw*(strlen(text)),ys=yw;
	int xc=x1+(x2-x1)/2, yc=y1+(y2-y1);
	
	if (font==base2)
	glRasterPos2i( xc-xs/2,yc-ys/2);
	glPushAttrib(GL_LIST_BIT);
	glListBase(font - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

void switchto2d()
{
	glLoadIdentity();
	mode3d=false;
	glViewport(0, 0, window_xsize, window_ysize);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0.0, window_xsize, window_ysize,0.0);
}

