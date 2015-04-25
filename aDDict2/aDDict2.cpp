#include "aDDict2.h"
#include "GuiInterface.h"
#include "introeditoreffects.h"

bool fullscreen=false;

ITEMLIST *TextureListScroller=NULL,*SceneListScroller=NULL,*ActualSceneItemList=NULL;
SCENE *Scenes=NULL,*LastScene=NULL,*ActualScene=NULL,*LayouterSelectedScene=NULL;
int lastwheel;

void SetupEnvironment()
{
	CreateDirectory(DataDir,NULL);
	CreateDirectory(ProjectDir,NULL);
	CreateDirectory(TextureDir,NULL);
	CreateDirectory(MusicDir,NULL);
#ifndef Public_Release
	CreateDirectory(MinimalDir,NULL);
#endif
	CreateDirectory(SceneDir,NULL);
	CreateDirectory(JpegDir,NULL);
	CreateDirectory("Data/Dump/",NULL);
}

bool LowRezLoad=false;

void CheckCommand()
{
	for (unsigned int x=0; x<strlen(CommandLine); x++) CommandLine[x]=toupper(CommandLine[x]);
	if (!strcmp(CommandLine,"LOWREZLOAD"))
	{
		LowRezLoad=!LowRezLoad;
		if (LowRezLoad) ErrorMessage="LOW RESOLUTION TEXTURE LOAD ON\0";
		else ErrorMessage="LOW RESOLUTION TEXTURE LOAD OFF\0";
	}
	if (!strcmp(CommandLine,"FLIP"))
	{
		if (ActualWorld)
			for (int x=0; x<ActualWorld->ObjectNum; x++)
				if (ActualWorld->ObjectList[x].ListItem->Selected)
					ActualWorld->ObjectList[x].PosData.Scale.x*=-1;
	}
	if (!strcmp(CommandLine,"MOVE"))
	{
		for (EVENT *e=EventList; e; e=e->Next)
			if (e->StartFrame>=6277+186)
			{
				e->StartFrame+=334-186;
				e->EndFrame+=334-186;
			}
	}
	if (!strcmp(CommandLine,"DEBUG"))
	{
		DebugStart=!DebugStart;
	}

	if (!strcmp(CommandLine,"DUMP"))
	{
		if (ActualTexture)
		{
			#define SETTGAW(x) *(unsigned short*)(tgaheader+0x0C)=(x)
			#define SETTGAH(x) *(unsigned short*)(tgaheader+0x0E)=(x)

			unsigned char tgaheader[]={
							0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,
							0x20,0x00
			};

			int TexCounter=0;

			RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->YRes];

			for (int z=0; z<8; z++)
			{
				char str[256];
				sprintf(str,"data/dump/%s%04d.tga\0",ActualTexture->Name,TexCounter++);

				FILE *f=fopen(str,"w+b");
				SETTGAW(ActualTexture->XRes);
				SETTGAH(ActualTexture->YRes);
				fwrite(tgaheader,18,1,f);
				//fwrite(ActualTexture->Layers[x].Data,1,ActualTexture->XRes*ActualTexture->YRes*4,f);
				for (int x=0; x<ActualTexture->XRes; x++)
					for (int y=0; y<ActualTexture->XRes; y++)
					{
						int a=x+y*ActualTexture->XRes;
						Buffer[a]=ActualTexture->Layers[z].Data[x+(ActualTexture->YRes-y-1)*ActualTexture->XRes];
						int b=Buffer[a].r;
						Buffer[a].r=Buffer[a].b;
						Buffer[a].b=b;
					}
				fwrite(Buffer,1,ActualTexture->XRes*ActualTexture->YRes*4,f);

				fclose(f);			
			}

			delete[] Buffer;

			ErrorMessage="TEXTURE DUMPED.";
		}
		else ErrorMessage="NO TEXTURE TO DUMP.";	
	}

	if (!strcmp(CommandLine,"ADDPASS"))
	{
		for (EVENT *e=EventList; e; e=e->Next) e->Pass++;
	}

	if (!strcmp(CommandLine,"I HATE YOU!") || !strcmp(CommandLine,"I HATE YOU") || !strcmp(CommandLine,"IHATEYOU") || !strcmp(CommandLine,"IHATEYOU!"))
	{
		SwapSplash();
	}

	if (CommandLine[0]=='M' && 
		CommandLine[1]=='E' &&
		CommandLine[2]=='R' &&
		CommandLine[3]=='G' &&
		CommandLine[4]=='E')
	{
		int cnt=0;
		for (int x=strlen(CommandLine)-1; x>=0; x--)
		{
			if (CommandLine[x]!=' ') break;
			else CommandLine[x]='\0';
		}
		for (int x=0; x<(int)strlen(CommandLine); x++) if (CommandLine[x]==' ') cnt++;

		int *marray=new int[cnt];
		memset(marray,0,cnt*4);

		int p=6;
		
		for (int x=0; x<cnt; x++)
		{
			int b=0;
			do
			{
				b*=10;
				b=b+CommandLine[p]-'0';
				p++;
			} while (CommandLine[p]!=' ' && CommandLine[p]!='\0');
			marray[x]=b;
			p++;
		}

		SCENE *i=NULL;
		if (cnt)
			for (SCENE *s=Scenes; s; s=s->Next) if (s->ID==marray[0]) i=s;

		if (i)
		for (int x=1; x<cnt; x++)
		if (marray[x]!=marray[0])
		{
			for (SCENE *s=Scenes; s; s=s->Next)
			{
				if (s->ID==marray[x]) 
				{
					s->ObjectNum=0;
					for (WORLD *w=Worlds; w; w=w->Next)
						for (int x=0; x<w->ObjectCount; x++)
							if (w->ObjectList[x].Data==s) w->ObjectList[x].Data=i;
				}
			}
		}

	}


#ifndef Public_Release
	if (!strcmp(CommandLine,"INCLUDELIST")) 
	{
		CreateIncludeList();
		ErrorMessage="INCLUDE LIST CREATED.";
	}
#endif
	if (!strcmp(CommandLine,"D")) 
		if (ActualScene)
		{
			ActualScene->DoRadiosityPass(true,200);
		}

	if (strlen(CommandLine)>2 && CommandLine[0]=='L' && CommandLine[1]==' ') 
		{
		  char *fname=CommandLine+2;
		  char fname2[256];
		  sprintf(fname2,"data\\%s.dat\0",fname);
		  FILE *f=fopen(fname2,"rb");
		  if (f)
		  {
			  int c;
			  fread(&c,4,1,f);

			  for (int x=0; x<c; x++)
			  {
				  int type,sframe,eframe,pass,dsize;
				  fread(&type,4,1,f);
				  fread(&pass,4,1,f);
				  fread(&sframe,4,1,f);
				  fread(&eframe,4,1,f);
				  fread(&dsize,4,1,f);
				  unsigned char *data=NULL;
				  if (dsize)
				  {
					data=new unsigned char[dsize];
					fread(data,dsize,1,f);

					for (EVENT *e=EventList;e;e=e->Next)
					if (e->Pass==pass && e->StartFrame==sframe && e->EndFrame==eframe)
					{
						switch (e->EventType)
						{
						case 1:
							{
								LAYER2DTEMPLATE *t=(LAYER2DTEMPLATE*)data;
								LAYER2DTEMPLATE *o=(LAYER2DTEMPLATE*)e->EffectData;
								o->StartCol=t->StartCol;
								o->EndCol=t->EndCol;
								o->SrcBlend=t->SrcBlend;
								o->DstBlend=t->DstBlend;
								break;
							}
						case 3:
						case 5:
						case 6:
						case 9:
						case 11:
						case 15:
							memcpy(e->EffectData,data,dsize);
							break;
						case 12:
							{
								GRIDFILLERTEMPLATE *t=(GRIDFILLERTEMPLATE*)data;
								GRIDFILLERTEMPLATE *o=(GRIDFILLERTEMPLATE*)e->EffectData;
								o->StartCol=t->StartCol;
								o->EndCol=t->EndCol;
								o->SrcBlend=t->SrcBlend;
								o->DstBlend=t->DstBlend;
								break;
							}
						case 13:
							{
								CAVETEMPLATE *t=(CAVETEMPLATE*)data;
								CAVETEMPLATE *o=(CAVETEMPLATE*)e->EffectData;
								o->r=t->r;
								o->g=t->g;
								o->b=t->b;
								break;
							}
							break;
						}
					}

					delete[] data;
				  }

			  }

			  fclose(f);
		  }
		  else ErrorMessage="TWEAK FILE NOT FOUND!";
		}

	if (!strcmp(CommandLine,"KILLPARTICLEDIRS")) 
	{
		WORLD *w = ActualWorld;
		//for (WORLD *w=Worlds; w; w=w->Next)
		if (w)
		{
			for (int x=0; x<w->ObjectNum; x++)
				if (w->ObjectList[x].EmitterData) w->ObjectList[x].EmitterData->Dir=V3_Make(0,0,0);
		}
		ErrorMessage="EMITTER DIRECTIONS KILLED.";
	}

	if (!strcmp(CommandLine,"CAMINFO")) 
	{
		CamInfo=!CamInfo;
		if (CamInfo)
		ErrorMessage="CAMERA INFO TURNED ON.";
		else
		ErrorMessage="CAMERA INFO TURNED OFF.";
	}

	if (!strcmp(CommandLine,"PLAYBACK"))
	{
		InitDemoPlayback();
		ErrorMessage="PLAYBACK STARTED";
	}
}

#pragma pack(1)

struct DEMODATA
{
	unsigned char Type;
	unsigned short xpos,ypos;
	int value;
	bool shift,ctrl;
	int Time;
};

DEMODATA DemoData;
int CountDown=-1;

int Msg=0;
int wParam=0;
int lParam=0;

void PlayBackFunction()
{
	if (DemoPlayback)
	{
		CountDown--;
		if (DemoData.Type==255 && CountDown<0)
		{
			if (!fread(&DemoData,1,sizeof(DEMODATA),DemoSave))
			{
				DemoPlayback=false;
				fclose(DemoSave);
				ErrorMessage="PLAYBACK ENDED";
			}
		}
		else
		//if (DemoData.Time<=(int)(timeGetTime()-DemoSaveStartTime))
		{
			POINT ap;

			switch (DemoData.Type)
			{
			case DEMOSAVE_LeftButton: 
				if (DemoData.value)	Msg=WM_LBUTTONDOWN; else Msg=WM_LBUTTONUP;
				ap.x=DemoData.xpos;
				ap.y=DemoData.ypos;
				ClientToScreen(hWnd,&ap);
				SetCursorPos(ap.x,ap.y);
				SendMessage(hWnd,WM_MOUSEMOVE,0,0);
				wParam=!DemoData.shift & MK_SHIFT + !DemoData.ctrl & MK_CONTROL;
				CountDown=1;
				break;
			case DEMOSAVE_RightButton: 
				if (DemoData.value)	Msg=WM_RBUTTONDOWN; else Msg=WM_RBUTTONUP;
				ap.x=DemoData.xpos;
				ap.y=DemoData.ypos;
				ClientToScreen(hWnd,&ap);
				SetCursorPos(ap.x,ap.y);
				SendMessage(hWnd,WM_MOUSEMOVE,0,0);
				wParam=!DemoData.shift & MK_SHIFT + !DemoData.ctrl & MK_CONTROL;
				CountDown=1;
				break;
			case DEMOSAVE_MiddleButton: 
				if (DemoData.value) Msg=WM_MBUTTONDOWN; else Msg=WM_MBUTTONUP;
				ap.x=DemoData.xpos;
				ap.y=DemoData.ypos;
				ClientToScreen(hWnd,&ap);
				SetCursorPos(ap.x,ap.y);
				SendMessage(hWnd,WM_MOUSEMOVE,0,0);
				wParam=!DemoData.shift & MK_SHIFT + !DemoData.ctrl & MK_CONTROL;
				CountDown=1;
				break;
			case DEMOSAVE_Wheel:
				Msg=0x020A;
				wParam=DemoData.value;
				ap.x=DemoData.xpos;
				ap.y=DemoData.ypos;
				ClientToScreen(hWnd,&ap);
				SetCursorPos(ap.x,ap.y);
				SendMessage(hWnd,WM_MOUSEMOVE,0,0);
				CountDown=1;
				break;
			case DEMOSAVE_Key:
				Msg=WM_CHAR;
				wParam=DemoData.value;
				CountDown=1;
				break;
			}

			if (CountDown<=0 && Msg!=-1)
			{
				SendMessage(hWnd,Msg,wParam,lParam);
				Msg=-1;
				CountDown=1;
			}

			DemoData.Type=-1;
		}
	}
}

void MainLoop()
{
	DemoData.Type=255;
	MouseString=new char[100];
	memset(MouseString,0,100);
	while (!Done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
		else if (Keys[VK_ESCAPE]) Done=true;
		else
		{
			PlayBackFunction();
			WriteDebug("Main Loop\n\n");
			CurrentCursor=Arrow;
			glClear(0x4100);
			SwitchTo2D();

			ToolTip=LastClickedToolTip;
			memset(ItemNameString,0,256);
			//glFlush();

			glColor4f(1,1,1,1);
			if (CommandLineCommand) ErrorMessage="";
			if (CommandLineCommand && CommandLineEnter)
			{				
				CommandLineCommand=false;
				CommandLineEnter=false;
				CommandLineReading=false;
				CheckCommand();
				CommandLine="";
			}

			MainGUI.CheckGUI();
			for (int x=0; x<256; x++) Keys[x]=false;
			if (MainGUI.ItemList[3].ButtonPushed && ActualWorld) ActualWorld->CalculateParticles();

			glColor4f(highblue);
			rectangle(-1,35,1025,35);
			glColor4f(1,1,1,1);

			PolyCounter=0;
			VertexCounter=0;
			ObjMemCounter=0;
			CallCounter=0;

			/*if (TextureList)
			{
				for (TEXTURE *t=TextureList; t; t=t->Next)
				{
					ObjMemCounter+=sizeof(TEXTURE);
					ObjMemCounter+=4*t->XRes*t->XRes*8;
				}
			}

			if (Scenes)
			{
				for (SCENE *s=Scenes; s; s=s->Next)
				{
					ObjMemCounter+=s->ObjectCapacity*sizeof(OBJECT);
					for (int x=0; x<s->ObjectNum; x++)
					{
						ObjMemCounter+=s->ObjectList[x].VertexCapacity*sizeof(VERTEX);
						for (int y=0; y<s->ObjectList[x].VertexNum; y++) ObjMemCounter+=s->ObjectList[x].VertexList[y].EdgeCapacity*4;
						ObjMemCounter+=s->ObjectList[x].PolygonCapacity*sizeof(POLYGON);
						ObjMemCounter+=s->ObjectList[x].EdgeCapacity*sizeof(EDGE);
					}
				}
			}

			if (Worlds)
			{
				for (WORLD *w=Worlds;w;w=w->Next)
				{
					ObjMemCounter+=w->ObjectCapacity*sizeof(WORLDOBJECT);
					for (int x=0; x<w->ObjectNum; x++)
					if (w->ObjectList[x].EmitterData)
						{
							ObjMemCounter+=sizeof(EMITTERDATA);
							if (w->ObjectList[x].EmitterData->Particles) ObjMemCounter+=(sizeof(PARTICLE))*w->ObjectList[x].EmitterData->MaxParticles;
							if (w->ObjectList[x].EmitterData->Particles && w->ObjectList[x].EmitterData->Tail) ObjMemCounter+=256*sizeof(VECTOR3)*w->ObjectList[x].EmitterData->MaxParticles;
						}
				}
			}*/

			MainGUI.DrawGUI();

			glErrorCheck();
			
			if (!MainGUI.ItemList[3].ActiveBranch && !MainGUI.ItemList[0].ActiveBranch)
			{
				for (WORLD *a=Worlds;a;a=a->Next)
					for (int x=0; x<a->ObjectNum; x++)
						if (a->ObjectList[x].Primitive==aDDict_SPHEREEMITTER ||
							a->ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
							a->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER)
						a->ObjectList[x].EmitterData->LastFrameChecked=timeGetTime();
			}

			glDisable(GL_DEPTH_TEST);
			if (strcmp(ErrorMessage,"") && !ReadLine) glColor4f(1,1,0.3f,1);
			glRasterPos2i(410,21);
			if (!CommandLineReading && !ReadLine)
			{
				if (!strcmp(ErrorMessage,""))
				glPrint(ToolTip,base);
				else
				glPrint(ErrorMessage,base);
			}
			else
			{
				char *Buffer=new char [strlen(CommandLine)+100];
				sprintf(Buffer,"%s_",CommandLine);
				glPrint(Buffer,base);
				delete Buffer;
			}

			if (strlen(MouseString) /*&& !strcmp(MouseString,"")*/)
			{
				glColor4f(0,0,0,1);
				glRasterPos2i(mx+16,my+12);
				glPrint(MouseString,base);
				glColor4f(1,1,1,1);
				glRasterPos2i(mx+15,my+11);
				glPrint(MouseString,base);
			}
			memset(MouseString,0,100);

			glColor4f(0,0,0,1);
			glRasterPos2i(mx+16,my);
			glPrint(ItemNameString,base);
			glColor4f(1,1,1,1);
			glRasterPos2i(mx+15,my-1);
			glPrint(ItemNameString,base);

			glEnable(GL_DEPTH_TEST);

			glColor4f(1,1,1,1);

			SetCursor(CurrentCursor);
			SwapBuffers(hDC);
			if (WaitLeftButton) { WaitLeftButtonRelease(); WaitLeftButton=false; }
			if (WaitMiddleButton) { WaitMiddleButtonRelease(); WaitMiddleButton=false; }
			if (WaitRightButton) { WaitRightButtonRelease(); WaitRightButton=false; }

			if (CommandLineEnter) { CommandLine=""; CommandLineEnter=false; ReadLine=false;}
			if (LeftButton || RightButton || MiddleButton) SetCapture(hWnd); else ReleaseCapture();
			lastwheel=Wheel;
			Wheel=0;

			Sleep(10);
		}

	}
}

typedef void (APIENTRY * WGLSWAPINTERVALEXT) (int);

INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
	FILE *f=fopen("bass.dll","rb");
	if (f)
	{
		fclose(f);
		DebugStart=false;
		int MemUsage=0;
		if ((GetSystemMetrics(SM_CXSCREEN)>=1024 && GetSystemMetrics(SM_CYSCREEN)>=768) || strstr(GetCommandLine(),"-fullscreen") && !strstr(GetCommandLine(),"-windowed"))
		{
	#ifndef Public_Release
			LastClickedToolTip="ADVANCED DIGITAL DYNAMITE INTRO CREATION TOOL V1.97";
	#else
			LastClickedToolTip="ADVANCED DIGITAL DYNAMITE INTRO CREATION TOOL V1.97 PUBLIC RELEASE";
	#endif
			SetupEnvironment();
			if (strstr(GetCommandLine(),"-fullscreen") || (GetSystemMetrics(SM_CXSCREEN)==1024 && GetSystemMetrics(SM_CYSCREEN==768)) && !strstr(GetCommandLine(),"-windowed")) fullscreen=true;
	#ifndef Public_Release
			Intro_CreateWindow("a.D.D.i.c.t. V1.97 (Build Date: " __DATE__ " " __TIME__ ")\0", 1024, 768, 32, fullscreen, LoadIcon(hInstance, MAKEINTRESOURCE(101)));
	#else
			Intro_CreateWindow("a.D.D.i.c.t. V1.97 Public Release (Build Date: " __DATE__ " " __TIME__ ")\0", 1024, 768, 32, fullscreen, LoadIcon(hInstance, MAKEINTRESOURCE(101)));
	#endif
			InitRadiosityStuff();
			
			WGLSWAPINTERVALEXT wglSwapIntervalEXT = (WGLSWAPINTERVALEXT) wglGetProcAddress("wglSwapIntervalEXT");
			if (wglSwapIntervalEXT && fullscreen) wglSwapIntervalEXT(1); // enable vertical synchronisation
		
			//InitDemoSave();
			InitGUI();
			initVertexBlend();
			MainLoop();
			//DeinitDemoSave();
			ReleaseDC(hWnd,hDC);
			if (fullscreen)	ChangeDisplaySettings(NULL,0);
			return 0;
		}
		else
		MessageBox( 0, "Sorry a.D.D.i.c.t. 2 requires at least a screen resolution of 1024x768 pixels.", "Error!", MB_OK );
	}
	else MessageBox( 0, "bass.dll not found", "Error!", MB_OK );
	return 0;
}