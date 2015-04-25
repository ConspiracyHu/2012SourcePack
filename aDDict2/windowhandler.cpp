#include "windowhandler.h"

FILE *DebugText;

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

bool	Keys[256];
bool	Active=TRUE;

bool    Done = false;
bool	WaitLeftButton=false,WaitMiddleButton=false,WaitRightButton=false;

MSG msg;

int XRes,YRes,mx,my,lx,ly,rx,ry,mbx,mby,wheel,fwKeys,Wheel;
bool LeftShift,LeftCtrl,RightShift,RightCtrl,MidShift,MidCtrl;

char *CommandLine="";
bool CommandLineEnter=false;
bool CommandLineReading=false;
bool CommandLineCommand=false;

bool    LeftButton,RightButton,MiddleButton,LeftButtonReleased,RightButtonReleased,MiddleButtonReleased = false;
bool	Transforming=false,Cammoving=false;
bool	TransformingWhenRightClicked=false;
bool	TransformingWhenMidClicked=false;
bool    CammovingWhenRightClicked=false;

HCURSOR Crosshair,Arrow,Horizontal,Vertical,HV1,HV2;
HCURSOR CurrentCursor;

bool DebugStart;

int DemoSaveStartTime=0;
int DemoSaveDeltaTime=0;

FILE *DemoSave=NULL;
bool DemoPlayback=false;

void InitDemoSave()
{
	DemoSaveStartTime=timeGetTime();
	DemoSave=fopen("demo.sav","w+b");
}

void DemoSaveEvent(unsigned char EventType, short int xpos, short int ypos, int value, bool ShiftValue, bool CtrlValue)
{
	if (DemoSave && !DemoPlayback)
	{
		int Time=timeGetTime()-DemoSaveStartTime+DemoSaveDeltaTime;
		fwrite(&EventType,1,1,DemoSave);
		fwrite(&xpos,2,1,DemoSave);
		fwrite(&ypos,2,1,DemoSave);
		fwrite(&value,4,1,DemoSave);
		fwrite(&ShiftValue,1,1,DemoSave);
		fwrite(&CtrlValue,1,1,DemoSave);
		fwrite(&Time,4,1,DemoSave);
	}
}

void DeinitDemoSave()
{
	if (DemoSave) fclose(DemoSave);
}

void InitDemoPlayback()
{
	DemoPlayback=true;
	DemoSave=fopen("demo.sav","rb");
	DemoSaveStartTime=timeGetTime();
}

void WriteDebug(const char *fmt, ...) 
{
	if (DebugStart)
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
	}
}

GLvoid KillGLWindow(GLvoid)
{
	ChangeDisplaySettings(NULL,0);
	ShowCursor(TRUE);
}

BOOL Intro_CreateWindow(char* Title, int Width, int Height, int Bits, bool FullScreenFlag, HICON Icon)
{
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
	wc.hCursor			= NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";
	Crosshair=LoadCursor(NULL, IDC_CROSS);
	Arrow=LoadCursor(NULL, IDC_ARROW);
	Horizontal=LoadCursor(NULL, IDC_SIZEWE);
	Vertical=LoadCursor(NULL, IDC_SIZENS);
	HV1=LoadCursor(NULL, IDC_SIZENESW);
	HV2=LoadCursor(NULL, IDC_SIZENWSE);

	RegisterClass(&wc);
	
	if (FullScreenFlag)
	{		

		DEVMODE Test;
		DEVMODE Used;
		int mode=0;

		while (EnumDisplaySettings(NULL,mode++,&Test))
		{
			if (Test.dmPelsWidth==1024 && Test.dmPelsHeight==768) Used=Test;
		}

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= Width;
		dmScreenSettings.dmPelsHeight	= Height;
		dmScreenSettings.dmBitsPerPel	= Bits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		dmScreenSettings.dmDisplayFrequency=100;

		ChangeDisplaySettings(&Used,CDS_FULLSCREEN);
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
		//ShowCursor(FALSE);
	}
	else
	{
	    dwExStyle=WS_EX_APPWINDOW + WS_EX_WINDOWEDGE;
        dwStyle=WS_OVERLAPPED+ WS_CAPTION+ WS_SYSMENU+WS_MINIMIZEBOX;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

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
	PixelFormat=ChoosePixelFormat(hDC,&pfd);
	SetPixelFormat(hDC,PixelFormat,&pfd);
	hRC=wglCreateContext(hDC);
	wglMakeCurrent(hDC,hRC);
	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	glEnable(GL_DEPTH_TEST);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glColorMask(TRUE,TRUE,TRUE,FALSE);

	SetCursor(Arrow);
	CurrentCursor=Arrow;
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
			if (!CommandLineReading)
			{
				Keys[wParam] = true;

				if (wParam==VK_ESCAPE) 
				{
					char *ExitStr=new char[256];
					
					srand(timeGetTime());
					int r=(int)((rand()/(float)RAND_MAX)*6.0f);
					switch (r)
					{
					case 0: sprintf(ExitStr,"Kill 2 hours of unsaved work?\0"); break;
					case 1: sprintf(ExitStr,"Are you a true addict?\0"); break;
					case 2: sprintf(ExitStr,"Come baaaaack!\0"); break;
					case 3: sprintf(ExitStr,"Beware the dealine is getting closer by the minute!\0"); break;
					case 4: sprintf(ExitStr,"You sure you pressed the right button?\0"); break;
					case 5: sprintf(ExitStr,"Are you sure you finished the release?\0"); break;
					}

					Keys[wParam]=!(MessageBox(NULL,ExitStr, "Wanna Quit?",MB_OKCANCEL|MB_ICONEXCLAMATION)==IDCANCEL);
					delete ExitStr;
				}
			}
			else
			{

			}

			return 0;					
		}

		case WM_KEYUP:					
		{
			Keys[wParam] = false;
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
			WriteDebug("Left mouse click\n");
            LeftButton=true;
			POINT ap;
			GetCursorPos(&ap);
			LeftShift=(wParam & MK_SHIFT)==0;
			LeftCtrl=(wParam & MK_CONTROL)==0;
			ScreenToClient(hWnd,&ap);
            lx=ap.x;
            ly=ap.y;
			DemoSaveEvent(DEMOSAVE_LeftButton,lx,ly,1,LeftShift,LeftCtrl);
			return 0;
        }
        
		case WM_LBUTTONUP:
        {
			DemoSaveEvent(DEMOSAVE_LeftButton,mx,my,0,LeftShift,LeftCtrl);
			WriteDebug("Left mouse release\n");
            LeftButton=false;
            LeftButtonReleased=true;
			return 0;
        }
        
		case WM_RBUTTONDOWN:
        {
            RightButton=true;
			TransformingWhenRightClicked=Transforming;
			CammovingWhenRightClicked=Cammoving;
			POINT ap;
			GetCursorPos(&ap);
			RightShift=(wParam & MK_SHIFT)==0;
			RightCtrl=(wParam & MK_CONTROL)==0;
			ScreenToClient(hWnd,&ap);
            rx=ap.x;
            ry=ap.y;
			DemoSaveEvent(DEMOSAVE_RightButton,rx,ry,1,RightShift,RightCtrl);
			return 0;
        }
        
		case WM_RBUTTONUP:
        {
			DemoSaveEvent(DEMOSAVE_RightButton,mx,my,0,RightShift,RightCtrl);
            RightButton=false;
            RightButtonReleased=true;
			TransformingWhenRightClicked=false;
			CammovingWhenRightClicked=false;
			return 0;
        }
        
		case WM_MBUTTONDOWN:
        {
            MiddleButton=true;
			TransformingWhenMidClicked=Transforming;
			POINT ap;
			GetCursorPos(&ap);
			MidShift=(wParam & MK_SHIFT)==0;
			MidCtrl=(wParam & MK_CONTROL)==0;
			ScreenToClient(hWnd,&ap);
            mbx=ap.x;
            mby=ap.y;
			DemoSaveEvent(DEMOSAVE_MiddleButton,mbx,mby,1,MidShift,MidCtrl);
			//DemoPlayback=false;
			return 0;
        }
        
		case WM_MBUTTONUP:
        {
			DemoSaveEvent(DEMOSAVE_MiddleButton,mx,my,0,MidShift,MidCtrl);
            MiddleButton=false;
            MiddleButtonReleased=true;
			TransformingWhenMidClicked=false;
			return 0;	
        }

        case 0x020A:
        {
            //fwKeys = GET_KEYSTATE_WPARAM(wParam);
            //wheel  = GET_WHEEL_DELTA_WPARAM(wParam);
			Wheel  = (int)((int)(wParam) / 7854320);
			DemoSaveEvent(DEMOSAVE_Wheel,mx,my,wParam,MidShift,MidCtrl);
			return 0;
        }

		case WM_CHAR: 
		{
			/*key = new tkey;
			key->next=keyboardbuffer;
			key->key=wParam;
			keyboardbuffer=key;*/
			DemoSaveEvent(DEMOSAVE_Key,0,0,wParam,0,0);
			
			if (CommandLineReading)
			{
				switch (wParam)
				{
				case VK_BACK:
					{
						if (strcmp(CommandLine,""))
						CommandLine[strlen(CommandLine)-1]=0;
						break;
					}

				case VK_RETURN:
					{
						CommandLineEnter=true;
						CommandLineReading=false;
						break;
					}
				case VK_ESCAPE:
					{
						if (strlen(CommandLine))
						memset(CommandLine,0,strlen(CommandLine));
						else
						{
							CommandLineEnter=true;
							CommandLineReading=false;
						}
						break;
					}

				default:
					{
						char *Buffer=new char[strlen(CommandLine)+100];
						memset(Buffer,0,strlen(CommandLine)+1);
						sprintf(Buffer,"%s%c\0",CommandLine,wParam);
						delete CommandLine;
						CommandLine=Buffer;
						break;
					}

				}
			}
			else
			{
				if (wParam==VK_RETURN) 
				{
					CommandLineReading=true;
					CommandLineCommand=true;
					CommandLine=new char[10];
					memset(CommandLine,0,10);
				}
				else
					Keys[wParam]=true;
			}
			return 0;
		}
	
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

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
		else PlayBackFunction();
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
		else PlayBackFunction();
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
		else PlayBackFunction();
	}
}

void SwitchTo2D()
{
	glLoadIdentity();
	glViewport(0, 0, XRes-1, YRes-1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0.0, XRes-1, YRes-1,0.0);
}
