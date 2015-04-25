#include "IntroEditor.h"
#include "IntroEditorEffects.h"
#pragma comment( lib , "bass.lib" )
#include "bass.h"

char *SoundSystem[]=
{"NONE",
 "MVX",
 "BASS"};

int SoundSys=0;
int MusicLength=0;

RENDERTEXTURESLOT RenderTextures[6];

#define EventHeight 10

EVENT *EventList=NULL;
bool NewEventInProgress=false;
int NewEventStartPass=0;
int NewEventStartFrame=0;
EVENT *SelectedEvent=NULL;
bool EventGrabbed=false;
int GrabFrame=0,GrabPass=0;
EVENT GrabEvent;
int ActualFrame=0;
GUIITEM *MainDisplay;
bool Scaling=false;
bool EndScale=false;
int OrigiFrame;
GUIITEM *SnapItem,*TimeLineItem;
cINTERFACE *PlayButton,*PauseButton,*SongListItem;
ITEMLIST *SongList;
int StartTime,StartFrame;
int SetStart=0;
bool MidClickDrag;
int OldStartFrame;
int OldStartPass;
GUIITEM *Display;
int LastPassToRender;

HSTREAM str;


bool Playing=false;
bool SongLoaded=false;
bool Info=true;
bool CamInfo=false;

float Linear(float a, float b, float t)
{
	return (b-a)*t+a;
}

void SetOrthoMode()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(MainDisplay->x1+1,768-MainDisplay->y2+1,MainDisplay->x2-MainDisplay->x1-1,MainDisplay->y2-MainDisplay->y1-1);
	glScissor(MainDisplay->x1+1,768-MainDisplay->y2+1,MainDisplay->x2-MainDisplay->x1-1,MainDisplay->y2-MainDisplay->y1-1);
	glEnable(GL_SCISSOR_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0, 799, 599,0);
}

void ArrangeEventList()
{
	EVENT *NewList=NULL;
	for (EVENT *e=EventList; e; e=e->Next)
	{
		EVENT *NewEvent=new EVENT;
		if (e==SelectedEvent) SelectedEvent=NewEvent;
		memcpy(NewEvent,e,sizeof(EVENT));
		EVENT *ReferenceList=NewList;
		EVENT *LastReference=NULL;
		while (ReferenceList && ReferenceList->Pass<e->Pass) 
		{ 
			LastReference=ReferenceList; 
			ReferenceList=ReferenceList->Next; 
		}

		if (NewList==NULL)
		{
			NewList=NewEvent;
			NewEvent->Next=NULL;
		}
		else
		{
			if (LastReference==NULL)
			{
				NewEvent->Next=NewList;
				NewList=NewEvent;
			}
			else
			{
				LastReference->Next=NewEvent;
				NewEvent->Next=ReferenceList;
			}
		}
	}
	EVENT *Last=NULL;
	EVENT *e=EventList;
	if (e)
	do
	{
		if (Last) delete Last;
		Last=e;
		e=e->Next;
	} while (e);
	if (Last) delete Last;

	EventList=NewList;

}

void DisplayViewPort(GUIITEM *I)
{
	Display=I;
	PolyCounter=0;
	VertexCounter=0;
	MainDisplay=I;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	//glColor4f(0,0,0,0);
	//quad(I->x1,I->y1,I->x2,I->y2);
	glColor4f(highblue);
	rectangle(I->x1,I->y1,I->x2,I->y2);

	glEnable(GL_SCISSOR_TEST);
	glScissor(I->x1+1,768-I->y2+1,I->x2-I->x1-1,I->y2-I->y1-1);
	glClearColor(0,0,0,0);
	glClear(0x4100);
	SetOrthoMode();
	EyeShake=TargetShake=V3_Make(0,0,0);

	for (EVENT *e=EventList;e;e=e->Next)
	{
		if (ActualFrame>=e->StartFrame && ActualFrame<=e->EndFrame && e->Pass<=LastPassToRender)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			SetOrthoMode();
			DEFAULTEVENTDATA d;
			d.TimePos=(ActualFrame-e->StartFrame)/(float)(e->EndFrame-e->StartFrame);
			d.x1=(int)Linear((float)e->sx1,(float)e->ex1,d.TimePos);
			d.x2=(int)Linear((float)e->sx2,(float)e->ex2,d.TimePos);
			d.y1=(int)Linear((float)e->sy1,(float)e->ey1,d.TimePos);
			d.y2=(int)Linear((float)e->sy2,(float)e->ey2,d.TimePos);
			d.OnScreenInLastFrame=e->OnScreenInLastFrame;
			d.ActualFrame=ActualFrame;
			e->Effect->EffectProc(&d,e->EffectData);
			glPopAttrib();
			e->OnScreenInLastFrame=true;
		}
		else
			e->OnScreenInLastFrame=false;
	}
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SwitchTo2D();
	glScissor(I->x1,768-I->y2,I->x2-I->x1+1,I->y2-I->y1+1);
	if (SelectedEvent)
	{
		glColor4f(0,1,0,0);
		rectangle(I->x1+SelectedEvent->ex1,I->y1+SelectedEvent->ey1,I->x1+1+SelectedEvent->ex2+1,I->y1+1+SelectedEvent->ey2+1);
		glColor4f(1,1,0,0);
		rectangle(I->x1+SelectedEvent->sx1,I->y1+SelectedEvent->sy1,I->x1+1+SelectedEvent->sx2+1,I->y1+1+SelectedEvent->sy2+1);
	}
	glDisable(GL_SCISSOR_TEST);


	if (Info)
	{
		char InfoString[256];
		memset(InfoString,0,256);

		sprintf(InfoString,"SOUND SYSTEM: %s\0",SoundSystem[SoundSys]);
		glColor4f(0,0,0,0);
		glRasterPos2i(I->x1+6,I->y2-44);
		glPrint(InfoString,base);
		glColor4f(1,1,1,1);
		glRasterPos2i(I->x1+5,I->y2-45);
		glPrint(InfoString,base);


		if (SelectedEvent)
		sprintf(InfoString,"CURRENT FRAME: %.5d  EVENT START: %.5d  EVENT END: %.5d\0",ActualFrame,SelectedEvent->StartFrame,SelectedEvent->EndFrame);
		else
		sprintf(InfoString,"CURRENT FRAME: %.5d\0",ActualFrame);
		glColor4f(0,0,0,0);
		glRasterPos2i(I->x1+6,I->y2-4);
		glPrint(InfoString,base);
		glColor4f(1,1,1,1);
		glRasterPos2i(I->x1+5,I->y2-5);
		glPrint(InfoString,base);

		sprintf(InfoString,"VERTEXCOUNT: %.6d, POLYCOUNT: %.6d\0",VertexCounter,PolyCounter);
		glColor4f(0,0,0,0);
		glRasterPos2i(I->x1+6,I->y2-14);
		glPrint(InfoString,base);
		glColor4f(1,1,1,1);
		glRasterPos2i(I->x1+5,I->y2-15);
		glPrint(InfoString,base);

		if (SelectedEvent)
		{
			sprintf(InfoString,"SX1: %.3d, SY1: %.3d, SX2: %.3d, SY2: %.3d, SXSIZ: %.3d, SYSIZ: %.3d\0",SelectedEvent->sx1,SelectedEvent->sy1,SelectedEvent->sx2,SelectedEvent->sy2,SelectedEvent->sx2-SelectedEvent->sx1,SelectedEvent->sy2-SelectedEvent->sy1);
			glColor4f(0,0,0,0);
			glRasterPos2i(I->x1+6,I->y2-34);
			glPrint(InfoString,base);
			glColor4f(1,1,1,1);
			glRasterPos2i(I->x1+5,I->y2-35);
			glPrint(InfoString,base);

			sprintf(InfoString,"EX1: %.3d, EY1: %.3d, EX2: %.3d, EY2: %.3d, EXSIZ: %.3d, EYSIZ: %.3d\0",SelectedEvent->ex1,SelectedEvent->ey1,SelectedEvent->ex2,SelectedEvent->ey2,SelectedEvent->ex2-SelectedEvent->ex1,SelectedEvent->ey2-SelectedEvent->ey1);
			glColor4f(0,0,0,0);
			glRasterPos2i(I->x1+6,I->y2-24);
			glPrint(InfoString,base);
			glColor4f(1,1,1,1);
			glRasterPos2i(I->x1+5,I->y2-25);
			glPrint(InfoString,base);

			if (CamInfo && SelectedEvent->Effect->Type==7)
			{
				DISPLAY3DTEMPLATE d=*(DISPLAY3DTEMPLATE*)SelectedEvent->EffectData;
				if (d.World)
				{

					CAMERA Cam=d.World->Camera;

					sprintf(InfoString,"EYEX: %.5f, EYEY: %.5f, EYEZ: %.5f, TRGX: %.5f, TRGY: %.5f, TRGZ: %.5f, FOV: %.5f, ROLL: %.5f\0",Cam.Eye.x,Cam.Eye.y,Cam.Eye.z,Cam.Target.x,Cam.Target.y,Cam.Target.z,Cam.Fov,Cam.Roll);
					glColor4f(0,0,0,0);
					glRasterPos2i(I->x1+6,I->y1+24);
					glPrint(InfoString,base);
					glColor4f(1,1,1,1);
					glRasterPos2i(I->x1+5,I->y1+25);
					glPrint(InfoString,base);
				}

			}

		}
	}

	glPopAttrib();
}

bool Collide(EVENT *a, EVENT *b)
{
	return (a->StartFrame>=b->StartFrame && a->StartFrame<=b->EndFrame) || 
		   (a->EndFrame>=b->StartFrame && a->EndFrame<=b->EndFrame) ||
		   (a->StartFrame<=b->StartFrame && a->EndFrame>=b->EndFrame);// || (a->EndFrame==b->StartFrame) || (b->EndFrame==a->StartFrame);
}

int CurrentFrame(cINTERFACE* Interface, int ButtonId)
{
	return min(Interface->ItemList[ButtonId].FrameNum,max(0,(int)(Interface->ItemList[ButtonId].FrameNum/Interface->ItemList[ButtonId].ZoomFactor*(mx-50)/801.0f+Interface->ItemList[ButtonId].StartFrame)));
}

int FramePos(cINTERFACE* Interface, int ButtonId, int Frame)
{
	return min(Interface->ItemList[ButtonId].x2,max(Interface->ItemList[ButtonId].x1, (int)((Frame-Interface->ItemList[ButtonId].StartFrame)*801.0f*Interface->ItemList[ButtonId].ZoomFactor/(float)Interface->ItemList[ButtonId].FrameNum+50)));
}

bool MovableUp(EVENT *e)
{
	bool Movable=false;
	for (int PassChecked=e->Pass-1; PassChecked>=0; PassChecked--)
	{
		bool Clear=true;
		for (EVENT *ee=EventList;ee;ee=ee->Next)
			if (!ee->Selected && ee!=e)
				if (PassChecked==ee->Pass && Collide(ee,e)) Clear=false;
		if (Clear) Movable=true;
	}
	return Movable;
}

void __stdcall GUI_EventUp(cINTERFACE* Interface, int ButtonId)
{
	bool Moveable=true;
	EVENT *ee;
	for (ee=EventList;ee;ee=ee->Next) if (ee->Selected && !MovableUp(ee)) Moveable=false;
	if (Moveable)
	for (ee=EventList;ee;ee=ee->Next)
	{
		if (ee->Selected && ee->Pass>0)
		{
			int Pass=ee->Pass;

			bool Collision=true;
			while (Pass>0 && Collision)
			{
				Pass--;
				Collision=false;
				for (EVENT *e=EventList; e; e=e->Next)
					if (e->Pass==Pass && !e->Selected && Collide(ee,e)) Collision=true;
			}
			if (!Collision) ee->Pass=Pass;		
		}
	}
	ArrangeEventList();
	WaitLeftButton=true;
}

void __stdcall GUI_EventDown(cINTERFACE* Interface, int ButtonId)
{
	for (EVENT *ee=EventList;ee;ee=ee->Next)
	{
		if (ee->Selected && ee->Pass<255)
		{
			int Pass=ee->Pass;

			bool Collision=true;
			while (Pass<255 && Collision)
			{
				Pass++;
				Collision=false;
				for (EVENT *e=EventList; e; e=e->Next)
					if (e->Pass==Pass && !e->Selected && Collide(ee,e)) Collision=true;
			}
			if (!Collision) ee->Pass=Pass;		
		}
	}
	ArrangeEventList();
	WaitLeftButton=true;
}


void __stdcall GUI_DeleteEvent(cINTERFACE* Interface, int ButtonId)
{
	bool Selected=true;

	while (Selected)
	{
		Selected=false;
		EVENT *SelectedEvent=NULL;
		for (EVENT *ee=EventList;ee;ee=ee->Next) if (ee->Selected) { Selected=true; SelectedEvent=ee; break;}
	
		if (Selected)
		{
			EventParameterInterface->SetActiveBranch(-1);
			EventParameterInterface->ParentItem->SelectedItem=0;
			for (EVENT *e=EventList;e->Next;e=e->Next)
			{
				if (e->Next==SelectedEvent)
				{
					e->Next=SelectedEvent->Next;
					delete SelectedEvent;
					SelectedEvent=NULL;
					break;
				}
			}
			if (SelectedEvent)
			{
				SelectedEvent->Selected=false;
				EventList=SelectedEvent->Next;
				delete SelectedEvent;
			}
			SelectedEvent=NULL;			
		}
	}
	SelectedEvent=NULL;
}

void __stdcall GUI_TimelinerLeftClick(cINTERFACE* Interface, int ButtonId)
{
	int Pass=(my-Interface->ItemList[ButtonId].y1-1)/EventHeight+Interface->ItemList[ButtonId].StartItem;
	int CurrFrame=CurrentFrame(Interface,ButtonId);
	if (LeftShift)
	{
		if (!EventGrabbed)
		{
			bool hit=false;
			for (EVENT *e=EventList;e;e=e->Next)
			{
				e->SF=e->StartFrame;
				e->EF=e->EndFrame;
				e->P=e->Pass;
				if (e->Pass==Pass && e->StartFrame<=CurrFrame && e->EndFrame>=CurrFrame && (mx==lx && my==ly))
				{
					/**/
					hit=true;
					if (!LeftCtrl)
					{
						e->Selected=!e->Selected;
						if (e==SelectedEvent) e->Selected=true;
						WaitLeftButton=true;
					}
					else
					{
						if (e->Selected)
						{
							EventGrabbed=true;
							GrabFrame=CurrFrame;
							GrabPass=e->Pass;
							GrabEvent=*e;
						}
						else
						{
							for (EVENT *ee=EventList;ee;ee=ee->Next) ee->Selected=false;
							//ArrangeEventList();
							//break;					
						}			
						if (e!=SelectedEvent)
						{
							SelectedEvent=e;
							e->Selected=true;
							//WaitLeftButton=true;
							AdjustGUIToEvent(e);
						}
					}
				}
			}
			if (!hit && LeftCtrl)
			{
				for (EVENT *e=EventList;e;e=e->Next)
				{
					if (e->Selected) e->Selected=false;
					if (e==SelectedEvent) e->Selected=true;
				}
			}
		}
		else //drag
		{
			ArrangeEventList();
			CurrFrame=min(Interface->ItemList[ButtonId].FrameNum-(GrabEvent.EndFrame-GrabFrame),max(-GrabEvent.StartFrame+GrabFrame,CurrFrame));
			int FrameDelta=CurrFrame-GrabFrame;
			int PassDelta=max(0,Pass)-GrabPass;

			int minp=0;
			int minf=0;
			int maxf=60000;
			EVENT *e;
			for (e=EventList; e; e=e->Next)
			if (e->Selected)
			{
				e->StartFrame=e->SF+FrameDelta;
				e->EndFrame=e->EF+FrameDelta;
				e->Pass=e->P+PassDelta;
				minp=min(minp,e->Pass);
				minf=min(minf,e->StartFrame);
				maxf=max(maxf,e->EndFrame);
			}
			if (minp<0) PassDelta-=minp;
			if (minf<0) FrameDelta-=minf;
			if (maxf>60000) FrameDelta-=(maxf-60000);
			int OFrameDelta=FrameDelta;
			

			if (RightButton) 
			{
				FrameDelta=0;
				PassDelta=0;
			}

			bool ok=false;
			while (!ok)
			{
				ok=true;

				if (ok)
				for (e=EventList; e; e=e->Next)
				if (e->Selected)
				{
					e->StartFrame=e->SF+FrameDelta;
					e->EndFrame=e->EF+FrameDelta;
					e->Pass=e->P+PassDelta;
					for (EVENT *k=EventList; k; k=k->Next)
						if (!k->Selected && e->Pass==k->Pass && Collide(e,k))
							ok=false;
				}

				if (!ok)
				{
					if (FrameDelta>0) FrameDelta--; else
					if (FrameDelta<0) FrameDelta++; else
					if (FrameDelta==0)
					{
						if (PassDelta>0) PassDelta--;
						if (PassDelta<0) PassDelta++;
						FrameDelta=OFrameDelta;
					}
				}
			}			
		}
	}
	else
	{
		LastPassToRender=max(0,Pass);
	}
}

void __stdcall GUI_TimelinerMidClick(cINTERFACE* Interface, int ButtonId)
{
	if (!MidClickDrag)
	{
		MidClickDrag=true;
		OldStartFrame=Interface->ItemList[ButtonId].StartFrame;
		OldStartPass=Interface->ItemList[ButtonId].StartItem;
	}
}

void __stdcall GUI_TimelinerRightClick(cINTERFACE* Interface, int ButtonId)
{
	if (RightShift)
	{
		if (!NewEventInProgress && !EventGrabbed)
		{
			NewEventInProgress=true;
			NewEventStartFrame=CurrentFrame(Interface,ButtonId);
			NewEventStartPass=(my-Interface->ItemList[ButtonId].y1-1)/EventHeight+Interface->ItemList[ButtonId].StartItem;
		}
	}
	else LastPassToRender=255;
}

void __stdcall GUI_TimelinerWheel(cINTERFACE* Interface, int ButtonId)
{
	if (!LeftButton) EventGrabbed=false;
	if (!MiddleButton) MidClickDrag=false;
	if (NewEventInProgress && !RightButton)
	{
		NewEventInProgress=false;
		int CurrFrame=CurrentFrame(Interface,ButtonId);
		int sf,ef;
		sf=min(NewEventStartFrame,CurrFrame);
		ef=max(NewEventStartFrame,CurrFrame);
		if (NewEventStartFrame!=CurrFrame)
		{
			EVENT *e;
			bool DoNew=true;
			for (e=EventList; e && DoNew; e=e->Next)
			if (e->Pass==NewEventStartPass)
				if ((sf>e->StartFrame && sf<e->EndFrame && ef>e->StartFrame && ef<e->EndFrame) || (sf<e->StartFrame && ef>e->EndFrame)) DoNew=false;
			for (e=EventList; e && DoNew; e=e->Next)
			if (e->Pass==NewEventStartPass)
			{
				if (sf>e->StartFrame && sf<e->EndFrame) sf=e->EndFrame+1;
				if (ef>e->StartFrame && ef<e->EndFrame) ef=e->StartFrame-1;
			}
			if (sf>=ef) DoNew=false;
			if (DoNew)
			{
				if (!EventList)
				{
					EventList=new EVENT;
					memset(EventList,0,sizeof(EVENT));
					EventList->Num=1;
				}
				else
				{
					EVENT *Buffer=new EVENT;
					memset(Buffer,0,sizeof(EVENT));
					Buffer->Next=EventList;
					EventList=Buffer;
					int a=0;
					for (EVENT *e=EventList;e;e=e->Next)
					{
						if (e->Num>a) a=e->Num;
					}
					EventList->Num=a+1;
				}
				EventList->StartFrame=sf;
				EventList->EndFrame=ef;
				EventList->Pass=NewEventStartPass;
				EventList->Effect=(EFFECT*)EffectList->List->Data;
				EventList->sx1=EventList->ex1=0;
				EventList->sy1=EventList->ey1=74;
				EventList->sx2=EventList->ex2=799;
				EventList->sy2=EventList->ey2=524;
				SelectedEvent=EventList;
				for (EVENT *ee=EventList;ee;ee=ee->Next) ee->Selected=false;
				SelectedEvent->Selected=true;
				sprintf(SelectedEvent->Name,"#%d: END DEMO\0",SelectedEvent->Num);
				AdjustGUIToEvent(SelectedEvent);
				ArrangeEventList();
			}
		}
	}

	if (MidClickDrag)
	{
		//OldStartFrame=Interface->ItemList[ButtonId].StartFrame;
		//OldStartPass=Interface->ItemList[ButtonId].StartItem;
		Interface->ItemList[ButtonId].StartFrame=min((int)(Interface->ItemList[ButtonId].FrameNum-Interface->ItemList[ButtonId].FrameNum/Interface->ItemList[ButtonId].ZoomFactor),max(0,(int)(OldStartFrame+ (mbx-mx)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*(float)(Interface->ItemList[ButtonId].FrameNum/(float)Interface->ItemList[ButtonId].ZoomFactor))));
		int Passa=(mby-Interface->ItemList[ButtonId].y1-1)/EventHeight+Interface->ItemList[ButtonId].StartItem;
		int Passb=(my-Interface->ItemList[ButtonId].y1-1)/EventHeight+Interface->ItemList[ButtonId].StartItem;
		Interface->ItemList[ButtonId].StartItem=min(max(0,OldStartPass+Passa-Passb),245);
	}

	Interface->ItemList[ButtonId].StartItem=min(max(0,Interface->ItemList[ButtonId].StartItem),245);
	Interface->ItemList[ButtonId].StartFrame=min((int)(Interface->ItemList[ButtonId].FrameNum-Interface->ItemList[ButtonId].FrameNum/Interface->ItemList[ButtonId].ZoomFactor),max(0,Interface->ItemList[ButtonId].StartFrame));
	
	if (Interface->ItemList[ButtonId].MouseInWindow)
	Interface->ItemList[ButtonId].ZoomFactor+=Wheel/10.0f;
	if (Interface->ItemList[ButtonId].ZoomFactor<1) Interface->ItemList[ButtonId].ZoomFactor=1;
}

void DisplayTimelineBase(GUIITEM *I)
{
	glBegin(GL_LINES);

	glColor4f(50.0f/300.0f,65.0f/300.0f,89.0f/300.0f,0);
	int s=(I->x2-I->x1)/10;
	int x;
	for (x=0; x<10; x++)
	{
		glVertex2i(I->x1+s*x,I->y1);
		glVertex2i(I->x1+s*x,I->y2);
	}

	glColor4f(0,0,0,0);
	for (x=0; x<I->BarSize; x++)
	{
		if (I->StartItem+x-1==LastPassToRender) glColor4f(1,1,1,1); else glColor4f(0,0,0,0);
		glVertex2i(I->x1,I->y1+x*EventHeight);
		glVertex2i(I->x2,I->y1+x*EventHeight);
	}
	glColor4f(0,0,0,0);

	if (NewEventInProgress)
	{
		glColor4f(1,1,1,1);
		int a,b;
		a=FramePos(I->ParentInterface,I->InterfaceItemId,NewEventStartFrame);
		b=FramePos(I->ParentInterface,I->InterfaceItemId,CurrentFrame(I->ParentInterface,I->InterfaceItemId));
		glVertex2i(a,I->y1+(NewEventStartPass-I->StartItem)*EventHeight);
		glVertex2i(a,I->y1+((NewEventStartPass-I->StartItem)+1)*EventHeight);
		glVertex2i(b,I->y1+(NewEventStartPass-I->StartItem)*EventHeight);
		glVertex2i(b,I->y1+((NewEventStartPass-I->StartItem)+1)*EventHeight);
	}

	glEnd();

	char str[100];
	glColor4f(highblue);
	for (x=0; x<11; x++)
	{
		memset(str,0,100);
		sprintf(str,"%d\0",(int)(I->StartFrame+I->FrameNum/I->ZoomFactor/10.0f*x));
		glRasterPos2i(I->x1+s*x-20,I->y1-5);
		glPrint(str,base);
	}

	for (x=0; x<I->ItemNum; x++)
	{
		memset(str,0,100);
		sprintf(str,"%d\0",I->StartItem+x);
		glRasterPos2i(I->x1-16,I->y1+x*EventHeight+8);
		glPrint(str,base);
	}
}

void DisplayTimelineEvents(GUIITEM *I)
{
	int sf,ef;
	for (EVENT *a=EventList;a;a=a->Next)
	if (a->Pass>=I->StartItem && a->Pass<I->StartItem+10)
	{
		sf=FramePos(I->ParentInterface,I->InterfaceItemId,a->StartFrame);
		ef=FramePos(I->ParentInterface,I->InterfaceItemId,a->EndFrame+1);
		if (!((sf<I->x1 && ef<I->x1) || (sf>I->x2 && ef>I->x2)))
		{
			if (sf<I->x1) sf=I->x1;
			if (ef>I->x2) ef=I->x2;
			glEnable(GL_SCISSOR_TEST);
			glScissor(sf,0,ef-sf+1,768);

			if (a->Effect->Type==7)
			{
				DISPLAY3DTEMPLATE d=*(DISPLAY3DTEMPLATE*)a->EffectData;
				if (d.World && d.World->CamAnims && d.CamID<d.World->CamCapacity)
				{
					CAMSPLINES c=d.World->CamAnims[d.CamID];
					if ((c.Eyex && !c.Eyex->KeyNum) || !c.Eyex || d.DontSaveCam)
					{				
						glColor4f(0,0,0,0);
						for (int x=sf; x<ef+20; x+=10)
						{
							glBegin(GL_LINES);

							glVertex2i(x,I->y1+(a->Pass-I->StartItem)*EventHeight);
							glVertex2i(x-10,I->y1+((a->Pass-I->StartItem)+1)*EventHeight);

							glEnd();
						}
					}
				}
				else
				if (d.DontSaveCam)
				{
					glColor4f(0,0,0,0);
					for (int x=sf; x<ef+20; x+=10)
					{
						glBegin(GL_LINES);

						glVertex2i(x,I->y1+(a->Pass-I->StartItem)*EventHeight);
						glVertex2i(x-10,I->y1+((a->Pass-I->StartItem)+1)*EventHeight);

						glEnd();
					}
				}
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			if (a!=SelectedEvent)
			glColor4f(a->Effect->r,a->Effect->g,a->Effect->b,0.5f);
			else
			glColor4f(a->Effect->r,a->Effect->g,a->Effect->b,0.7f);
			quad(sf,I->y1+(a->Pass-I->StartItem)*EventHeight,ef,I->y1+((a->Pass-I->StartItem)+1)*EventHeight);
			glDisable(GL_BLEND);

			glColor4f(0,0,0,0);
			glRasterPos2i(sf+4,I->y1+(a->Pass-I->StartItem)*EventHeight+8);
			glPrint(a->Name,base);

			glColor4f(highblue);
			rectangle(sf,I->y1+(a->Pass-I->StartItem)*EventHeight,ef,I->y1+((a->Pass-I->StartItem)+1)*EventHeight);
			glDisable(GL_SCISSOR_TEST);
		}
	}
	glColor4f(highblue);
	rectangle(I->x1,I->y1,I->x2,I->y2);

	glBegin(GL_LINES);
	for (int x=0; x<I->BarSize; x++)
	{
		if (I->StartItem+x-1==LastPassToRender) 
		{
			glColor4f(1,1,1,1);
			glVertex2i(I->x1,I->y1+x*EventHeight);
			glVertex2i(I->x2,I->y1+x*EventHeight);
		}
	}
	glEnd();
}

void DisplayTimeliner(GUIITEM *I)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	TimeLineItem=I;

	DisplayTimelineBase(I);
	DisplayTimelineEvents(I);

	for (EVENT *a=EventList;a;a=a->Next)
		if (a->Selected)
		{
			glColor4f(1,0,0,1);
			int s,e;
			s=FramePos(I->ParentInterface,I->InterfaceItemId,a->StartFrame);
			e=FramePos(I->ParentInterface,I->InterfaceItemId,a->EndFrame+1);
			if (a->Pass>=I->StartItem && a->Pass<I->StartItem+10)
			rectangle(s,I->y1+(a->Pass-I->StartItem)*EventHeight,e,I->y1+((a->Pass-I->StartItem)+1)*EventHeight);
		}

	if (SelectedEvent)
	{
		glColor4f(1,0,0,1);
		int s,e;
		s=FramePos(I->ParentInterface,I->InterfaceItemId,SelectedEvent->StartFrame);
		e=FramePos(I->ParentInterface,I->InterfaceItemId,SelectedEvent->EndFrame+1);
		if (SelectedEvent->Pass>=I->StartItem && SelectedEvent->Pass<I->StartItem+10)
		rectangle(s,I->y1+(SelectedEvent->Pass-I->StartItem)*EventHeight,e,I->y1+((SelectedEvent->Pass-I->StartItem)+1)*EventHeight);
		glColor4f(highblue);
		line(s,I->y1-20,s,I->y1);
		line(e,I->y1-20,e,I->y1);
		if (MouseInWindow(s-3,I->y1-20,s+3,I->y1-1) || (Scaling && !EndScale))
		{
			glColor4f(0,0.25f,0,1);
			line(s-3,I->y1-20,s-3,I->y1-1);
			line(s+3,I->y1-20,s+3,I->y1-1);
			glColor4f(0,0.5f,0,1);
			line(s-2,I->y1-20,s-2,I->y1-1);
			line(s+2,I->y1-20,s+2,I->y1-1);
			glColor4f(0,0.75f,0,1);
			line(s-1,I->y1-20,s-1,I->y1-1);
			line(s+1,I->y1-20,s+1,I->y1-1);
			glColor4f(0,1,0,1);
			line(s,I->y1-20,s,I->y1-1);
			if (LeftClickInWindow(I->x1-3,I->y1-19,I->x2+3,I->y1-1) && !Scaling)
			{
				Scaling=true;
				EndScale=false;
				OrigiFrame=SelectedEvent->StartFrame;
			}
		}
		else
		if (MouseInWindow(e-3,I->y1-20,e+3,I->y1-1) || (Scaling && EndScale))
		{
			glColor4f(0,0.25f,0,1);
			line(e-3,I->y1-20,e-3,I->y1-1);
			line(e+3,I->y1-20,e+3,I->y1-1);
			glColor4f(0,0.5f,0,1);
			line(e-2,I->y1-20,e-2,I->y1-1);
			line(e+2,I->y1-20,e+2,I->y1-1);
			glColor4f(0,0.75f,0,1);
			line(e-1,I->y1-20,e-1,I->y1-1);
			line(e+1,I->y1-20,e+1,I->y1-1);
			glColor4f(0,1,0,1);
			line(e,I->y1-20,e,I->y1-1);
			if (LeftClickInWindow(I->x1-3,I->y1-19,I->x2+3,I->y1-1) && !Scaling)
			{
				Scaling=true;
				EndScale=true;
				OrigiFrame=SelectedEvent->EndFrame;
			}
		}
		if (Scaling)
		{
			int Curr=CurrentFrame(I->ParentInterface,I->InterfaceItemId);
			if (SnapItem->ButtonPushed)
			{
				EVENT *e;
				for (e=EventList; e; e=e->Next)
					if (e->Pass!=SelectedEvent->Pass)
					{
						int s=FramePos(I->ParentInterface,I->InterfaceItemId,e->StartFrame);
						int e_=FramePos(I->ParentInterface,I->InterfaceItemId,e->EndFrame);
						if (!EndScale && mx>=e_-5 && mx<=e_+5) Curr=e->EndFrame+1;
						if (EndScale && mx>=s-5 && mx<=s+5) Curr=e->StartFrame-1;
					}
				for (e=EventList; e; e=e->Next)
					if (e->Pass!=SelectedEvent->Pass)
					{
						int s=FramePos(I->ParentInterface,I->InterfaceItemId,e->StartFrame);
						int e_=FramePos(I->ParentInterface,I->InterfaceItemId,e->EndFrame);
						if (!EndScale && mx>=s-5 && mx<=s+5) Curr=e->StartFrame;
						if (EndScale && mx>=e_-5 && mx<=e_+5) Curr=e->EndFrame;
					}
			}
			if (!EndScale)
			{
				for (EVENT *e=EventList; e; e=e->Next)
					if (e->Pass==SelectedEvent->Pass)
					{
						if (e!=SelectedEvent)
						{
							if (e->EndFrame>Curr && e->EndFrame<=SelectedEvent->EndFrame) Curr=e->EndFrame+1;
						}
						else if (Curr>e->EndFrame) Curr=e->EndFrame;
					}
				SelectedEvent->StartFrame=Curr;
			}
			else
			{
				for (EVENT *e=EventList; e; e=e->Next)
					if (e->Pass==SelectedEvent->Pass)
					{
						if (e!=SelectedEvent)
						{
							if (e->StartFrame<Curr && e->StartFrame>=SelectedEvent->StartFrame) Curr=e->StartFrame-1;
						}
						else if (Curr<e->StartFrame) Curr=e->StartFrame;
					}
				SelectedEvent->EndFrame=Curr;
			}
			
		}
		if (!LeftButton) Scaling=false;
	}

	if (SoundSys==2)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_SRC_ALPHA);
		glColor4f(0,0,0,0.8f);
		quad(FramePos(I->ParentInterface,I->InterfaceItemId,MusicLength),I->y1,
			 FramePos(I->ParentInterface,I->InterfaceItemId,60000),I->y2-1);
		glDisable(GL_BLEND);
	}

	if (RightClickInWindow(I->x1,I->y1-20,I->x2,I->y1-1))
	{
		ActualFrame=CurrentFrame(I->ParentInterface,I->InterfaceItemId);
		if (SoundSys==2 && ActualFrame>=MusicLength) ActualFrame=MusicLength;
	}

	if (MidClickInWindow(I->x1,I->y1-20,I->x2,I->y1-1))
	{
		SetStart=(int)CurrentFrame(I->ParentInterface,I->InterfaceItemId);
		ActualFrame=SetStart;
	}

	glColor4f(0,1,1,1);
	line(FramePos(I->ParentInterface,I->InterfaceItemId,SetStart),I->y1-20,FramePos(I->ParentInterface,I->InterfaceItemId,SetStart),I->y2);
	glColor4f(1,1,1,1);
	line(FramePos(I->ParentInterface,I->InterfaceItemId,(int)ActualFrame),I->y1-20,FramePos(I->ParentInterface,I->InterfaceItemId,(int)ActualFrame),I->y2);

	//glPopAttrib();
}

void ResetWorldParticles()
{
	for (WORLD *ActualWorld=Worlds; ActualWorld; ActualWorld=ActualWorld->Next)
	{
		int time=timeGetTime();
		for (int x=0; x<ActualWorld->ObjectNum; x++)
		{
			if (ActualWorld->ObjectList[x].EmitterData)
				{
					//memset(ActualWorld->ObjectList[x].EmitterData->Particles,0,sizeof(PARTICLE)*ActualWorld->ObjectList[x].EmitterData->MaxParticles);
					for (int z=0; z<ActualWorld->ObjectList[x].EmitterData->MaxParticles; z++) ActualWorld->ObjectList[x].EmitterData->Particles[z].Active=false;
					ActualWorld->ObjectList[x].EmitterData->ParticleNumBuffer=ActualWorld->ObjectList[x].EmitterData->ParticlesPerFrame;
					ActualWorld->ObjectList[x].EmitterData->LastFrameChecked=time;
				}
		}
		ActualWorld->CalculateParticles();
	}
}

void __stdcall GUI_Play(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSwitch(Interface,ButtonId);
	Playing=!Playing;
	PlayButton->ParentItem->ButtonPushed=Playing;
	if (Playing)
	{
		ResetWorldParticles();
		switch (SoundSys)
		{
		case 0: StartTime=timeGetTime(); StartFrame=(int)ActualFrame; break;
		//case 1: mvxSystem_Play((int)(ActualFrame*10)); break;
		case 2: BASS_ChannelSetPosition(str,BASS_ChannelSeconds2Bytes(str,ActualFrame/100.0f));
				BASS_StreamPlay(str,FALSE,0); break;
		}
	}
	else
	{
		switch (SoundSys)
		{
		case 0: break;
		//case 1: mvxSystem_Stop(); break;
		case 2: BASS_ChannelStop(str); break;
		}
	}

	if (ActualFrame>=MusicLength && SoundSys>1)
	{
		ActualFrame=MusicLength;
	}
	
	WaitLeftButton=true;
}

void __stdcall GUI_Start(cINTERFACE* Interface, int ButtonId)
{
	ActualFrame=SetStart;
	if (Playing)
	{
		switch (SoundSys)
		{
		case 0: StartTime=timeGetTime(); break;
		//case 1: mvxSystem_Play(SetStart); break;
		case 2: BASS_ChannelSetPosition(str,BASS_ChannelSeconds2Bytes(str,SetStart/100.0f));
				BASS_StreamPlay(str,FALSE,0); break;
		}
	}
}

void __stdcall GUI_Stop(cINTERFACE* Interface, int ButtonId)
{
	if (Playing)
	{
		switch (SoundSys)
		{
			case 0: StartTime=timeGetTime(); break;
			//case 1: mvxSystem_Stop(); break;
			case 2: BASS_ChannelStop(str);
					BASS_ChannelSetPosition(str,BASS_ChannelSeconds2Bytes(str,SetStart/100.0f)); break;
		}
		ActualFrame=SetStart;
	}

	PlayButton->ParentItem->ButtonPushed=false;
	Playing=false;
}

void __stdcall GUI_Timer(cINTERFACE* Interface, int ButtonId)
{
	if (Playing)
	{
		switch (SoundSys)
		{
		case 0:
			ActualFrame=(int)((timeGetTime()-StartTime)/10.0f)+StartFrame;
			break;
		case 1:
			//ActualFrame=(int)(mvxSystem_GetSync()/10.0f);
			break;
		case 2:
			ActualFrame=(int)(BASS_ChannelBytes2Seconds(str,BASS_ChannelGetPosition(str))*100.0f);
			if (ActualFrame>=MusicLength-1)
			{
				PlayButton->ParentItem->ButtonPushed=false;
				BASS_ChannelStop(str);
				Playing=false;
			}
			break;
		}
	}
	if (SoundSys==2 && ActualFrame>=MusicLength)
	{
		ActualFrame=MusicLength;
	}
}

cINTERFACE *MusicButtonInterface;
int MusicButtonId;

void __stdcall GUI_GetSongList(cINTERFACE* Interface, int ButtonId)
{
	MusicButtonInterface=Interface;
	MusicButtonId=ButtonId;
	if (!Interface->ItemList[ButtonId].ButtonPushed)
	{
		for (int x=0; x<Interface->ItemNum; x++) Interface->ItemList[x].ButtonPushed=false;
		Interface->ItemList[ButtonId].ButtonPushed=true;
		Interface->SetActiveBranch(ButtonId);
		if (Interface->ItemList[ButtonId].ToolTip) LastClickedToolTip=Interface->ItemList[ButtonId].ToolTip;
		//TextureSongList=new ITEMLIST();
		SongList->Last=NULL;
		SongList->List=NULL;
		SongList->ItemNum=0;
		// kill old list...
		SetupFileList(SongList,MusicMsk);
		SetupFileList(SongList,MusicMsk2);
		SetupFileList(SongList,MusicMsk3);
		Interface->ItemList[ButtonId].ItemNum=SongList->ItemNum;
	}
	else
	{
		Interface->ItemList[ButtonId].ButtonPushed=false;
		Interface->ItemList[ButtonId].ActiveBranch=false;
	}
	WaitLeftButton=true;
}

void __stdcall GUI_LoadSong(cINTERFACE* Interface, int ButtonId)
{
	ITEM *Selected=ReturnItem(SongList,*(int*)Interface->ItemList[ButtonId].data);
	if (Selected)
	{
		char name[256];
		memset(name,0,256);
		sprintf(name,"data\\musics\\%s\0",Selected->Name);
		int SoundS=0;
		if (strstr(name,".mp3\0") || strstr(name,".ogg\0")) SoundS=2;
		if (strstr(name,".wav\0")) SoundS=1;
		Playing=false;

		switch (SoundSys)
		{
		case 0:
			break;
		case 1:
			//if (Playing) mvxSystem_Stop();
			//if (SongLoaded) mvxSystem_DeInit();
			break;
		case 2:
			if (SongLoaded) BASS_StreamFree(str);
			break;
		}

		SoundSys=SoundS;

		switch (SoundSys)
		{
		case 0:
			break;
		case 1:
			//mvxSystem_Init(hWnd,name);
			SongLoaded=true;
			break;
		case 2:
			str=BASS_StreamCreateFile(FALSE,name,0,0,0);
			MusicLength=(int)(BASS_ChannelBytes2Seconds(str,BASS_StreamGetLength(str))*100.0f);
			SongLoaded=true;
			break;
		}

		MusicButtonInterface->ItemList[MusicButtonId].ActiveBranch=false;
		MusicButtonInterface->ItemList[MusicButtonId].ButtonPushed=false;

	}
}

void __stdcall GUI_CopyEvent(cINTERFACE* Interface, int ButtonId)
{
	//EVENT *LastEvent=EventList;
	//for (EVENT *e=EventList;e;e=e->Next) if (!e->Next) LastEvent=e;
	if (EventList)
	{
		for (EVENT *e=EventList; e; e=e->Next)
		if (e->Selected)
		{
			e->Selected=false;
			if (!EventList)
			{
				EventList=new EVENT;
				memset(EventList,0,sizeof(EVENT));
				EventList->Num=1;
			}
			else
			{
				EVENT *Buffer=new EVENT;
				memset(Buffer,0,sizeof(EVENT));
				Buffer->Next=EventList;
				EventList=Buffer;
				int a=0;
				for (EVENT *e=EventList;e;e=e->Next)
				{
					if (e->Num>a) a=e->Num;
				}
				EventList->Num=a+1;
			}
			EventList->StartFrame=e->StartFrame;
			EventList->EndFrame=e->EndFrame;
			EventList->Pass=e->Pass;
			EventList->Effect=e->Effect;
			EventList->sx1=e->sx1;
			EventList->sy1=e->sy1;
			EventList->sx2=e->sx2;
			EventList->sy2=e->sy2;
			EventList->ex1=e->ex1;
			EventList->ey1=e->ey1;
			EventList->ex2=e->ex2;
			EventList->ey2=e->ey2;
			EventList->EffectData=new char[EventList->Effect->DataSize];
			memcpy(EventList->EffectData,e->EffectData,EventList->Effect->DataSize);
			if (e==SelectedEvent)
			SelectedEvent=EventList;
			EventList->Selected=true;
			memset(EventList->Name,0,30);
			sprintf(EventList->Name,"#%d: %s\0",EventList->Num,EventList->Effect->ListItem->Name);
		}
		GUI_EventDown(Interface,ButtonId);
		AdjustGUIToEvent(SelectedEvent);
		ArrangeEventList();
	}
}

int x1s,y1s,x2s,y2s;
bool x1m,x2m,y1m,y2m;

void __stdcall GUI_ViewPortLeftClick(cINTERFACE* Interface, int ButtonId)
{
	if (SelectedEvent)
	{

		if (!x1m && !x2m && !y1m && !y2m)
		{
			if (LeftCtrl)
			{
				x1s=SelectedEvent->sx1;
				y1s=SelectedEvent->sy1;
				x2s=SelectedEvent->sx2;
				y2s=SelectedEvent->sy2;
			}
			else
			{
				x1s=SelectedEvent->ex1;
				y1s=SelectedEvent->ey1;
				x2s=SelectedEvent->ex2;
				y2s=SelectedEvent->ey2;
			}
		}

		int x1,y1,x2,y2;
		if (LeftCtrl)
		{
			x1=Display->x1+SelectedEvent->sx1;
			y1=Display->y1+SelectedEvent->sy1;
			x2=Display->x1+SelectedEvent->sx2+2;
			y2=Display->y1+SelectedEvent->sy2+2;
		}
		else
		{
			x1=Display->x1+SelectedEvent->ex1;
			y1=Display->y1+SelectedEvent->ey1;
			x2=Display->x1+SelectedEvent->ex2+2;
			y2=Display->y1+SelectedEvent->ey2+2;
		}

		if (LeftClickInWindow(x1-5,y1-5,x1+5,y2+5) && !x1m) x1m=true; 
		if (LeftClickInWindow(x2-5,y1-5,x2+5,y2+5) && !x2m && !x1m) x2m=true; 

		if (LeftClickInWindow(x1-5,y1-5,x2+5,y1+5) && !y1m) y1m=true; 
		if (LeftClickInWindow(x1-5,y2-5,x2+5,y2+5) && !y2m && !y1m) y2m=true; 

		if (LeftCtrl)
		{
			if (LeftClickInWindow(x1s-5+Display->x1,y1s+Display->y1-5,x1s+5+Display->x1+2,y2s+Display->y1+7) && x1m) SelectedEvent->sx1=min(799,min(SelectedEvent->sx2,max(0,x1s-lx+mx)));
			if (LeftClickInWindow(x2s-5+Display->x1,y1s+Display->y1-5,x2s+5+Display->x1+2,y2s+Display->y1+7) && x2m) SelectedEvent->sx2=min(799,max(SelectedEvent->sx1,max(0,x2s-lx+mx)));
			if (LeftClickInWindow(x1s+Display->x1-5,y1s-5+Display->y1,x2s+Display->x1+7,y1s+5+Display->y1+2) && y1m) SelectedEvent->sy1=min(599,min(SelectedEvent->sy2,max(0,y1s-ly+my)));
			if (LeftClickInWindow(x1s+Display->x1-5,y2s-5+Display->y1,x2s+Display->x1+7,y2s+5+Display->y1+2) && y2m) SelectedEvent->sy2=min(599,max(SelectedEvent->sy1,max(0,y2s-ly+my)));
			if (!LeftShift)
			{
				if (LeftClickInWindow(x1s-5+Display->x1,y1s+Display->y1-5,x1s+5+Display->x1+2,y2s+Display->y1+7) && x1m) SelectedEvent->ex1=min(799,min(SelectedEvent->ex2,max(0,x1s-lx+mx)));
				if (LeftClickInWindow(x2s-5+Display->x1,y1s+Display->y1-5,x2s+5+Display->x1+2,y2s+Display->y1+7) && x2m) SelectedEvent->ex2=min(799,max(SelectedEvent->ex1,max(0,x2s-lx+mx)));
				if (LeftClickInWindow(x1s+Display->x1-5,y1s-5+Display->y1,x2s+Display->x1+7,y1s+5+Display->y1+2) && y1m) SelectedEvent->ey1=min(599,min(SelectedEvent->ey2,max(0,y1s-ly+my)));
				if (LeftClickInWindow(x1s+Display->x1-5,y2s-5+Display->y1,x2s+Display->x1+7,y2s+5+Display->y1+2) && y2m) SelectedEvent->ey2=min(599,max(SelectedEvent->ey1,max(0,y2s-ly+my)));
			}
		}
		else
		{
			if (LeftClickInWindow(x1s-5+Display->x1,y1s+Display->y1-5,x1s+5+Display->x1+2,y2s+Display->y1+7) && x1m) SelectedEvent->ex1=min(799,min(SelectedEvent->ex2,max(0,x1s-lx+mx)));
			if (LeftClickInWindow(x2s-5+Display->x1,y1s+Display->y1-5,x2s+5+Display->x1+2,y2s+Display->y1+7) && x2m) SelectedEvent->ex2=min(799,max(SelectedEvent->ex1,max(0,x2s-lx+mx)));
			if (LeftClickInWindow(x1s+Display->x1-5,y1s-5+Display->y1,x2s+Display->x1+7,y1s+5+Display->y1+2) && y1m) SelectedEvent->ey1=min(599,min(SelectedEvent->ey2,max(0,y1s-ly+my)));
			if (LeftClickInWindow(x1s+Display->x1-5,y2s-5+Display->y1,x2s+Display->x1+7,y2s+5+Display->y1+2) && y2m) SelectedEvent->ey2=min(599,max(SelectedEvent->ey1,max(0,y2s-ly+my)));
		}

		if (RightButton)
		{
			if (LeftCtrl)
			{
				SelectedEvent->sx1=x1s;
				SelectedEvent->sy1=y1s;
				SelectedEvent->sx2=x2s;
				SelectedEvent->sy2=y2s;
				if (!LeftShift)
				{
					SelectedEvent->ex1=x1s;
					SelectedEvent->ey1=y1s;
					SelectedEvent->ex2=x2s;
					SelectedEvent->ey2=y2s;
				}
			}
			else
			{
				SelectedEvent->ex1=x1s;
				SelectedEvent->ey1=y1s;
				SelectedEvent->ex2=x2s;
				SelectedEvent->ey2=y2s;
			}
		}
	}
}



void __stdcall GUI_ViewPortMidClick(cINTERFACE* Interface, int ButtonId)
{
	if (SelectedEvent && !LeftButton)
	{
		CurrentCursor=Crosshair;
		if (!x1m && !x2m && !y1m && !y2m)
		{
			if (MidCtrl)
			{
				x1s=SelectedEvent->sx1;
				y1s=SelectedEvent->sy1;
				x2s=SelectedEvent->sx2;
				y2s=SelectedEvent->sy2;
			}
			else
			{
				x1s=SelectedEvent->ex1;
				y1s=SelectedEvent->ey1;
				x2s=SelectedEvent->ex2;
				y2s=SelectedEvent->ey2;
			}
		}
		x1m=true;
		if (MidCtrl)
		{
			SelectedEvent->sx1=x1s-mbx+mx;
			SelectedEvent->sx2=x2s-mbx+mx;
			SelectedEvent->sy1=y1s-mby+my;
			SelectedEvent->sy2=y2s-mby+my;
			if (!MidShift)
			{
				SelectedEvent->ex1=x1s-mbx+mx;
				SelectedEvent->ex2=x2s-mbx+mx;
				SelectedEvent->ey1=y1s-mby+my;
				SelectedEvent->ey2=y2s-mby+my;
			}
		}
		else
		{
			SelectedEvent->ex1=x1s-mbx+mx;
			SelectedEvent->ex2=x2s-mbx+mx;
			SelectedEvent->ey1=y1s-mby+my;
			SelectedEvent->ey2=y2s-mby+my;
		}

		if (RightButton)
		{
			if (MidCtrl)
			{
				SelectedEvent->sx1=x1s;
				SelectedEvent->sy1=y1s;
				SelectedEvent->sx2=x2s;
				SelectedEvent->sy2=y2s;
				if (!MidShift)
				{
					SelectedEvent->ex1=x1s;
					SelectedEvent->ey1=y1s;
					SelectedEvent->ex2=x2s;
					SelectedEvent->ey2=y2s;
				}
			}
			else
			{
				SelectedEvent->ex1=x1s;
				SelectedEvent->ey1=y1s;
				SelectedEvent->ex2=x2s;
				SelectedEvent->ey2=y2s;
			}
		}
	}
}

void __stdcall GUI_ViewPortWheel(cINTERFACE* Interface, int ButtonId)
{
	if (SelectedEvent && Display->MouseInWindow)
	{
		int x1,y1,x2,y2;

		x1=Display->x1+SelectedEvent->ex1;
		y1=Display->y1+SelectedEvent->ey1;
		x2=Display->x1+1+SelectedEvent->ex2+1;
		y2=Display->y1+1+SelectedEvent->ey2+1;

		if (MouseInWindow(x1-5,y1-5,x1+5,y2+5)) CurrentCursor=Horizontal;
		if (MouseInWindow(x2-5,y1-5,x2+5,y2+5)) CurrentCursor=Horizontal;
		
		if (MouseInWindow(x1-5,y1-5,x2+5,y1+2)) CurrentCursor=Vertical;
		if (MouseInWindow(x1-5,y2-5,x2+5,y2+2)) CurrentCursor=Vertical;

		if (MouseInWindow(x1-5,y1-5,x1+5,y1+5)) CurrentCursor=HV2;
		if (MouseInWindow(x2-5,y2-5,x2+5,y2+5)) CurrentCursor=HV2;

		if (MouseInWindow(x1-5,y2-5,x1+5,y2+5)) CurrentCursor=HV1;
		if (MouseInWindow(x2-5,y1-5,x2+5,y1+5)) CurrentCursor=HV1;

		x1=Display->x1+SelectedEvent->sx1;
		y1=Display->y1+SelectedEvent->sy1;
		x2=Display->x1+1+SelectedEvent->sx2+1;
		y2=Display->y1+1+SelectedEvent->sy2+1;

		if (MouseInWindow(x1-5,y1-5,x1+5,y2+5)) CurrentCursor=Horizontal;
		if (MouseInWindow(x2-5,y1-5,x2+5,y2+5)) CurrentCursor=Horizontal;
		
		if (MouseInWindow(x1-5,y1-5,x2+5,y1+2)) CurrentCursor=Vertical;
		if (MouseInWindow(x1-5,y2-5,x2+5,y2+2)) CurrentCursor=Vertical;

		if (MouseInWindow(x1-5,y1-5,x1+5,y1+5)) CurrentCursor=HV2;
		if (MouseInWindow(x2-5,y2-5,x2+5,y2+5)) CurrentCursor=HV2;

		if (MouseInWindow(x1-5,y2-5,x1+5,y2+5)) CurrentCursor=HV1;
		if (MouseInWindow(x2-5,y1-5,x2+5,y1+5)) CurrentCursor=HV1;

		if (!LeftButton && !MiddleButton)
		x1m=x2m=y1m=y2m=false;
	}
}

void __stdcall GUI_SwitchInfo(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSwitch(Interface,ButtonId);
	Info=!Info;
}

void __stdcall GUI_UnselectEvent(cINTERFACE* Interface, int ButtonId)
{
	SelectedEvent->Selected=false;
	SelectedEvent=NULL;
	EventParameterInterface->SetActiveBranch(-1);
	EventParameterInterface->ParentItem->SelectedItem=0;
	for (EVENT *e=EventList; e; e=e->Next) e->Selected=false;
}

void __stdcall GUI_ZoomMin(cINTERFACE* Interface, int ButtonId)
{
	TimeLineItem->ZoomFactor=1;
}

void __stdcall GUI_ZoomEvt(cINTERFACE* Interface, int ButtonId)
{
	if (SelectedEvent)
	{
		TimeLineItem->ZoomFactor=(float)min(60000/800.0f*5.0f,60000/(float)(SelectedEvent->EndFrame-SelectedEvent->StartFrame)/1.1);
		
		TimeLineItem->StartFrame=(int)(SelectedEvent->StartFrame-(60000/(float)TimeLineItem->ZoomFactor-(SelectedEvent->EndFrame-SelectedEvent->StartFrame))/2.0f);
	}
}

void __stdcall GUI_ZoomMax(cINTERFACE* Interface, int ButtonId)
{
	TimeLineItem->ZoomFactor=60000/800.0f;
}

void __stdcall GUI_ZoomMax5(cINTERFACE* Interface, int ButtonId)
{
	TimeLineItem->ZoomFactor=60000/800.0f*5;
}


void __stdcall GUI_ZoomPlus(cINTERFACE* Interface, int ButtonId)
{
	TimeLineItem->ZoomFactor+=2;
}

void __stdcall GUI_ZoomMinus(cINTERFACE* Interface, int ButtonId)
{
	TimeLineItem->ZoomFactor-=2;
	TimeLineItem->ZoomFactor=max(1,TimeLineItem->ZoomFactor);
}

void __stdcall GUI_JumpEventStart(cINTERFACE* Interface, int ButtonId)
{
	if (SelectedEvent)
		ActualFrame=SelectedEvent->StartFrame;
}

void BuildIntroEditorGUI(cINTERFACE *IntroEditor)
{
	LastPassToRender=255;
	InitIntroEditorEffects();
	if (!BASS_Init(1,44100,0,hWnd,NULL)) ErrorMessage="BASS INIT ERROR!";
	SetTimer(hWnd,1,250,NULL);

	for (int x=0; x<6; x++)
	{
		glGenTextures(1,&RenderTextures[x].TexImage);
		glBindTexture(GL_TEXTURE_2D,RenderTextures[x].TexImage);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}

	EffectList=new ITEMLIST();
	
	cINTERFACE *DisplayInterface=IntroEditor->AddSubInterface();
	cINTERFACE *MenuInterface=IntroEditor->AddSubInterface();
	cINTERFACE *TimeLinerInterface=IntroEditor->AddTimeliner(50,665,851,10);
	SnapItem=IntroEditor->AddSkinnedButton(30,631,SnapButton,"TURN SNAP ON/OFF ( s )",GUI_ButtonSwitch,nop,nop,nop,'s')->ParentItem;
	IntroEditor->ItemList[IntroEditor->ItemNum-1].ButtonPushed=true;
	IntroEditor->AddSkinnedButton(30,612,Del__Button,"DELETE SELECTED EVENT ( del )",GUI_DeleteEvent,nop,nop,nop,VK_DELETE);
	IntroEditor->AddSkinnedButton(855,729,UpButton,"MOVE EVENT UP ( UP )",GUI_EventUp,nop,nop,nop,VK_UP);
	IntroEditor->AddSkinnedButton(855,745,DownButton,"MOVE EVENT DOWN ( DOWN )",GUI_EventDown,nop,nop,nop,VK_DOWN);
	IntroEditor->AddSkinnedButton(30,612-19,InfoButton,"TURN INFO DISPLAY ON/OFF (i)",GUI_SwitchInfo,nop,nop,nop,'i');
	IntroEditor->ItemList[IntroEditor->ItemNum-1].ButtonPushed=true;
	IntroEditor->AddSkinnedButton(30,612-38,Del__Button,"UNSELECT EVENT ( u )",GUI_UnselectEvent,nop,nop,nop,'u');
	IntroEditor->AddSkinnedButton(30,612-38-19,UndoButton,"JUMP TO START OF EVENT ( j )",GUI_JumpEventStart,nop,nop,nop,'j');

	DisplayInterface->AddViewPort(50,50,851,651);
	TimeLinerInterface->ParentItem->ActiveBranch=true;
	EventParameterInterface=TimeLinerInterface->AddItemList(860,50,140,8,EffectList,"EFFECTS LIST");

	IntroEditor->AddSkinnedButton(928,749,CopyButton,"COPY EVENT ( c )",GUI_CopyEvent,nop,nop,nop,'c');
	cINTERFACE *MusicInterface=IntroEditor->AddSubInterface();
	MusicInterface->AddSkinnedButton(860+15,726,PStartButton,"TO START ( q )",GUI_Start,nop,nop,nop,'q');
	MusicInterface->AddSkinnedButton(888+15,726,PBackButton,"BACK ( w )",nop,nop,nop,nop,'w');
	MusicInterface->AddSkinnedButton(908+15,726,PStopButton,"STOP ( e )",GUI_Stop,nop,nop,nop,'e');
	PlayButton=MusicInterface->AddSkinnedButton(928+15,726,PPlayButton,"PLAY ( SPACE )",GUI_Play,nop,nop,GUI_Timer,' ');
	MusicInterface->AddSkinnedButton(948+15,726,PForwardButton,"FORWARD ( t )",nop,nop,nop,nop,'t');
	MusicInterface->AddSkinnedButton(968+15,726,PEndButton,"END ( y )",nop,nop,nop,nop,'y');

	cINTERFACE *MusicMenuInterface=MusicInterface->AddSubInterface();
	cINTERFACE *MusicLoadInterface=MusicMenuInterface->AddSkinnedButton(860+11,749,MusicButton,"MUSIC LOAD MENU",GUI_GetSongList,nop,nop,nop);
	SongList=new ITEMLIST();
	MusicLoadInterface->AddItemList(878,640,122,6,SongList,"SELECT SONG");
	MusicLoadInterface->AddSkinnedButton(898,710,ImportButtonBig,"LOAD SONG",GUI_LoadSong,nop,nop,nop);
	MusicLoadInterface->ItemList[MusicLoadInterface->ItemNum-1].data=&MusicLoadInterface->ItemList[MusicLoadInterface->ItemNum-5].SelectedItem;
	GUI_GetSongList(MusicLoadInterface,0);

	IntroEditor->AddButton(5,670-7,30,686-7,4,11,"MIN","ZOOM OUT FULL ( 1 )",GUI_ZoomMin,nop,nop,nop,'1');
	IntroEditor->AddButton(5,687-7,30,703-7,3,11,"EVT","ZOOM TO EVENT ( 2 )",GUI_ZoomEvt,nop,nop,nop,'2');
	IntroEditor->AddButton(5,704-7,30,720-7,3,11,"Max","ZOOM MAX ( 3 )",GUI_ZoomMax,nop,nop,nop,'3');
	IntroEditor->AddButton(5,721-7,30,737-7,3,11,"Mx5","ZOOM MAX x 5 ( 4 )",GUI_ZoomMax5,nop,nop,nop,'4');
	IntroEditor->AddButton(5,740-7,30,756-7,8,11,"IN","ZOOM IN ( 5 )",GUI_ZoomPlus,nop,nop,nop,'5');
	IntroEditor->AddButton(5,757-7,30,766,2,11,"OUT","ZOOM OUT ( 6 )",GUI_ZoomMinus,nop,nop,nop,'6');

	RegisterEffects();
}