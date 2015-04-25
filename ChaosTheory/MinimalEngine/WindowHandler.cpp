#include "windowhandler.h"
//#include "windows.h"

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

MSG msg;

bool	Keys[256];
bool	Active = true;
bool    Done = false;

int XRes,YRes;

#ifdef MINIMALTWEAKER
int mx,my,lx,ly,rx,ry,mbx,mby,wheel,fwKeys,Wheel;
bool LeftShift,LeftCtrl,RightShift,RightCtrl,MidShift,MidCtrl;
bool LeftButton,RightButton,MiddleButton,LeftButtonReleased,RightButtonReleased,MiddleButtonReleased = false;
#endif


void Intro_DestroyWindow(bool fullscreen)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ChangeDisplaySettings(NULL,0);
	}
	ShowCursor(TRUE);
	DestroyWindow(hWnd);
}

/*void WriteDebug(const char *fmt, ...) 
{
	char text[512];
	va_list		ap;

	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	FILE * f;
	if ( (f=fopen("debug.txt","at")) ) 
	{
		fprintf(f,text);
		fclose(f);
	}
}*/

void CopyScreenToBitmap(LPRECT lpRect, unsigned char *out)
{
	HDC         hScrDC, hMemDC;         // screen DC and memory DC     
	//	HBITMAP     hBitmap; //, 
	//	HBITMAP     hBitmap;
	//	HBITMAP     hOldBitmap;    // handles to deice-dependent bitmaps     
	int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab     
	int         nWidth, nHeight;        // DIB width and height     
	int         xScrn, yScrn;           // screen resolution      

	HGDIOBJ     hOldBitmap , hBitmap;

	// check for an empty rectangle 
	if (IsRectEmpty(lpRect))       
		return;      
	// create a DC for the screen and create     
	// a memory DC compatible to screen DC          

	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);     
	hMemDC = CreateCompatibleDC(hScrDC);      // get points of rectangle to grab  

	nX = lpRect->left;     
	nY = lpRect->top;     
	nX2 = lpRect->right;     
	nY2 = lpRect->bottom;      // get screen resolution      

	xScrn = GetDeviceCaps(hScrDC, HORZRES);     
	yScrn = GetDeviceCaps(hScrDC, VERTRES);      

	//make sure bitmap rectangle is visible      

	nX=max(0,nX);
	nY=max(0,nY);
	nX2=min(nX2,xScrn);
	nY2=min(nY2,yScrn);

	nWidth = nX2 - nX;     
	nHeight = nY2 - nY;      

	// create a bitmap compatible with the screen DC     

	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      

	// select new bitmap into memory DC     

	hOldBitmap =   SelectObject (hMemDC, hBitmap);      

	// bitblt screen DC to memory DC     

	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);     

	// select old bitmap back into memory DC and get handle to     
	// bitmap of the screen          

	hBitmap = SelectObject(hMemDC, hOldBitmap);      

	// clean up      

	BITMAPINFO bmi;    
	bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth=XRes;
	bmi.bmiHeader.biHeight=YRes;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=32;
	bmi.bmiHeader.biCompression=BI_RGB;
	int i=GetDIBits(hMemDC,(HBITMAP)hBitmap,0,YRes,out,&bmi,DIB_RGB_COLORS);
	for (int y=0; y<YRes/2; y++)
		for (int x=0; x<XRes; x++)
		{
			int p1=(y*XRes+x)*4;
			int p2=((YRes-y-1)*XRes+x)*4;
			unsigned char c;
			c=out[p1];
			out[p1]=out[p2+2];
			out[p2+2]=c;

			c=out[p1+1];
			out[p1+1]=out[p2+1];
			out[p2+1]=c;

			c=out[p1+2];
			out[p1+2]=out[p2];
			out[p2]=c;
		}

	DeleteDC(hScrDC);     
	DeleteDC(hMemDC);      

	// return handle to the bitmap      

	//return (HBITMAP)hBitmap; 

}

//#include "GeCCo_FindFile.h"
//#include "png.h"
//#pragma comment(lib,"zlib.lib")
//#pragma comment(lib,"libpng.lib")

void SavePNGFile(unsigned char *Image, int XRes, int YRes, bool Alpha, char *file_name)
{
}


void ScreenSave()
{
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
			Active=(!HIWORD(wParam));
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
			Done = true;
			return 0;					
		}

		case WM_KEYDOWN:				
		{
			Keys[wParam] = true;
			return 0;					
		}

		case WM_KEYUP:					
		{
			Keys[wParam] = false;
			if (wParam==VK_SNAPSHOT)
			{
				ScreenSave();
				return DefWindowProcW(hWnd,uMsg,wParam,lParam);
			}
			return 0;
		}
#ifdef MINIMALTWEAKER
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
            LeftButton=true;
			POINT ap;
			GetCursorPos(&ap);
			LeftShift=(wParam & MK_SHIFT)==0;
			LeftCtrl=(wParam & MK_CONTROL)==0;
			ScreenToClient(hWnd,&ap);
            lx=ap.x;
            ly=ap.y;
			return 0;
        }
        
		case WM_LBUTTONUP:
        {
            LeftButton=false;
            LeftButtonReleased=true;
			return 0;
        }
        
		case WM_RBUTTONDOWN:
        {
            RightButton=true;
			POINT ap;
			GetCursorPos(&ap);
			RightShift=(wParam & MK_SHIFT)==0;
			RightCtrl=(wParam & MK_CONTROL)==0;
			ScreenToClient(hWnd,&ap);
            rx=ap.x;
            ry=ap.y;
			return 0;
        }
        
		case WM_RBUTTONUP:
        {
            RightButton=false;
            RightButtonReleased=true;
			return 0;
        }
        
		case WM_MBUTTONDOWN:
        {
            MiddleButton=true;
			POINT ap;
			GetCursorPos(&ap);
			MidShift=(wParam & MK_SHIFT)==0;
			MidCtrl=(wParam & MK_CONTROL)==0;
			ScreenToClient(hWnd,&ap);
            mbx=ap.x;
            mby=ap.y;
			//DemoPlayback=false;
			return 0;
        }
        
		case WM_MBUTTONUP:
        {
            MiddleButton=false;
            MiddleButtonReleased=true;
			return 0;	
        }
#endif

	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void Intro_CreateWindow(char* Title, DEVMODE mode, bool FullScreenFlag, HICON Icon, bool AonTop)
{
	int Width=mode.dmPelsWidth;
	int Height=mode.dmPelsHeight;
	int Bits=mode.dmBitsPerPel;
	XRes=Width;
	YRes=Height;
	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)Width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)Height;

	hInstance			= GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= Icon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";

	RegisterClass(&wc);
	//WriteDebug("Class Registered.\n");
	
	if (FullScreenFlag)
	{
		/*DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= Width;
		dmScreenSettings.dmPelsHeight	= Height;
		dmScreenSettings.dmBitsPerPel	= Bits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;*/

		ChangeDisplaySettings(&mode,CDS_FULLSCREEN);
		//WriteDebug("Screen Mode Set.\n");
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
		ShowCursor(FALSE);
		//WriteDebug("Cursor off.\n");
	}
	else
	{
	    dwExStyle=WS_EX_APPWINDOW + WS_EX_WINDOWEDGE;
        dwStyle=WS_OVERLAPPED+ WS_CAPTION+ WS_SYSMENU+WS_MINIMIZEBOX;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
	//WriteDebug("Window rectangle Adjusted.\n");

    hWnd=CreateWindowEx(	dwExStyle,
								"OpenGL",
								Title,
								dwStyle |
								WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN,
								CW_USEDEFAULT, CW_USEDEFAULT,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,
								NULL,
								hInstance,
								NULL);
	//WriteDebug("Window Created.\n");

	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		Bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	
	hDC=GetDC(hWnd);
	//WriteDebug("Got DC.\n");
	PixelFormat=ChoosePixelFormat(hDC,&pfd);
	//WriteDebug("Chosepixelformat done.\n");
	SetPixelFormat(hDC,PixelFormat,&pfd);
	//WriteDebug("Setpixelformat done.\n");
	hRC=wglCreateContext(hDC);
	//WriteDebug("Createcontext done.\n");
	wglMakeCurrent(hDC,hRC);
	//WriteDebug("wglMakeCurrent done.\n");
	ShowWindow(hWnd,SW_SHOW);
	//WriteDebug("ShowWindow done.\n");
	SetForegroundWindow(hWnd);
	//WriteDebug("SetForeground done..\n");
	SetFocus(hWnd);
	//WriteDebug("Setfocus done.\n");
	if (AonTop) {
		SetWindowPos(hWnd,HWND_TOPMOST,
			(GetSystemMetrics(SM_CXSCREEN)-XRes)/2,
			(GetSystemMetrics(SM_CYSCREEN)-YRes)/2,
			0, 0, SWP_NOSIZE);
		//WriteDebug("Always on top.\n");
	}

	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL,0);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}

void SwitchTo2D()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, XRes, YRes);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0.0, XRes, YRes,0.0);
}

#ifdef MINIMALTWEAKER

bool MouseInWindow( float x1,float y1,float x2,float y2)
{
 return ((mx>=x1) && (mx<=x2) && (my>=y1) && (my<=y2));
}

bool MouseInWindow( int x1,int y1,int x2,int y2)
{
 return ((mx>=x1) && (mx<=x2) && (my>=y1) && (my<=y2));
}

bool LeftClickInWindow( float x1,float y1,float x2,float y2)
{
 return ((lx>=x1) && (lx<=x2) && (ly>=y1) && (ly<=y2) && LeftButton);
}

bool LeftClickInWindow( int x1,int y1,int x2,int y2)
{
 return ((lx>=x1) && (lx<=x2) && (ly>=y1) && (ly<=y2) && LeftButton);
}

bool RightClickInWindow( float x1,float y1,float x2,float y2)
{
 return ((rx>=x1) && (rx<=x2) && (ry>=y1) && (ry<=y2) && RightButton);
}

bool RightClickInWindow( int x1,int y1,int x2,int y2)
{
 return ((rx>=x1) && (rx<=x2) && (ry>=y1) && (ry<=y2) && RightButton);
}

bool MidClickInWindow( float x1,float y1,float x2,float y2)
{
 return ((mbx>=x1) && (mbx<=x2) && (mby>=y1) && (mby<=y2) && MiddleButton);
}

bool MidClickInWindow( int x1,int y1,int x2,int y2)
{
 return ((mbx>=x1) && (mbx<=x2) && (mby>=y1) && (mby<=y2) && MiddleButton);
}

void WaitLeftButtonRelease()
{
	MSG msg;
	while (LeftButton)
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) Done=true;
			else
			{ 
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
}

void WaitMiddleButtonRelease()
{
	MSG msg;
	while (MiddleButton)
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) Done=true;
			else
			{ 
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
}

void WaitRightButtonRelease()
{
	MSG msg;
	while (RightButton)
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) Done=true;
			else
			{ 
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
}

#endif