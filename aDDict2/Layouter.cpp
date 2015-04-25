#include "layouter.h"
#include "layoutergui.h"
#include "IntroEditorEffects.h"

WORLD *ActualWorld;
WORLD *Worlds,*LastWorld,*LayouterSelectedWorld;
cINTERFACE *LayouterObjectsMenu,*SwrldAnimList;

cINTERFACE *LayouterDisplays;
cINTERFACE *LayouterDisplay1;
cINTERFACE *LayouterDisplay2;
cINTERFACE *LayouterDisplay3;
cINTERFACE *LayouterDisplay4;
cINTERFACE *VertToggle,*HorToggle;
cINTERFACE *AddModellInterface,*AddSWorldInterface;
cINTERFACE *FogButtonInterface;

cINTERFACE *LSplineMenu,*AnimList,*CamList,*CameraSplines,*SplineEditorInterface,*ObjList,*LightsList,*PrtList,*SwrldList,*SceneSplines;
cINTERFACE *SplineMenuInterface;
cINTERFACE *ParticleSplines,*LightSplines_;
cINTERFACE *MatMenu,*VarMatMenu,*ConstMatMenu;
cINTERFACE *SWorldSplines,*LightingButtonInterface;

cINTERFACE *LLightMenus[8];
LIGHTDATA LLightData[8];

cINTERFACE *MainLayouterScroller,*WorldScroller;
void *MainLayouterScrollerData;
ITEMLIST *WorldListScroller;
ITEMLIST *ObjectSplines,*LightSplines,*EmitterSplines,*CamSplines,*SubWorldSplines,*LightList,*MainAnimList;
float SWAnimPos;
int SWAnimID;

float ParticlesPerFrame,GravityStrength,InnerRadius,DeflectLoss,MoveLoss,DefaultSpeed,SpeedChaos,DirChaos,ParticleSize,Camfade,RotSpeed,RotChaos;
int ParticleLife,ParticleLifeChaos,MaxParticles;
int TailLength,TailLength2;
int r1,g1,b1,a1,c1,r2,g2,b2,a2,c2;
int ActualAnim;
int LObjRed,LObjGreen,LObjBlue,LObjAlpha;
unsigned char FogRed,FogGreen,FogBlue,AEpsilon;
float FogIntensity1,FogIntensity2;

void __stdcall GUI_AddObjKey(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		if (ActualWorld->AnimNum)
		{
			if (ActualWorld->ObjectCount)
			{
				ITEM *o=ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem);
				if (o)
				{
					WORLDOBJECT *Object=(WORLDOBJECT*)o->Data;
					DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];

					if (Interface->ItemList[10+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==0))
					s->Posx->AddKey(Object->PosData.Pos.x,PlaybackPos);
					if (Interface->ItemList[11+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==1))
					s->Posy->AddKey(Object->PosData.Pos.y,PlaybackPos);
					if (Interface->ItemList[12+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==2))
					s->Posz->AddKey(Object->PosData.Pos.z,PlaybackPos);

					if ((Interface->ItemList[13+4].ButtonPushed) && (ScrollerID==9+4 || ScrollerID==3 || ScrollerID==4 || ScrollerID==5))
					{
						s->Rotx->AddKey(Object->PosData.Quaternion.s,PlaybackPos);
						s->Roty->AddKey(Object->PosData.Quaternion.v.x,PlaybackPos);
						s->Rotz->AddKey(Object->PosData.Quaternion.v.y,PlaybackPos);
						s->Rotw->AddKey(Object->PosData.Quaternion.v.z,PlaybackPos);
					}
					
					if (Interface->ItemList[16+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==6))
					s->Sclx->AddKey(Object->PosData.Scale.x,PlaybackPos);
					if (Interface->ItemList[17+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==7))
					s->Scly->AddKey(Object->PosData.Scale.y,PlaybackPos);
					if (Interface->ItemList[18+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==8))
					s->Sclz->AddKey(Object->PosData.Scale.z,PlaybackPos);

					if (Interface->ItemList[19+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==9))
					s->Red->AddKey(Object->Color.r/16.0f,PlaybackPos);
					if (Interface->ItemList[20+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==10))
					s->Green->AddKey(Object->Color.g/16.0f,PlaybackPos);
					if (Interface->ItemList[21+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==11))
					s->Blue->AddKey(Object->Color.b/16.0f,PlaybackPos);
					if (Interface->ItemList[22+4].ButtonPushed && (ScrollerID==9+4 || ScrollerID==12))
					s->Alpha->AddKey(Object->Color.a/16.0f,PlaybackPos);

					s->Posx->InitVectors();
					s->Posy->InitVectors();
					s->Posz->InitVectors();
					s->Sclx->InitVectors();
					s->Scly->InitVectors();
					s->Sclz->InitVectors();
					Spline_QuaternionGetVectors(s->Rotx,s->Roty,s->Rotz,s->Rotw);

					
					WaitLeftButton=true;
				}
				else ErrorMessage="NO OBJECT SELECTED OR OBJECT NOT FOUND (In this case tell BoyC)!";
			}
			else
			ErrorMessage="CREATE AN OBJECT FIRST!";
		}
		else
		ErrorMessage="CREATE AN ANIMATION FIRST!";
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall GUI_AddSubWorldKey(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		if (ActualWorld->AnimNum)
		{
			if (ActualWorld->SubWorldCount)
			{
				ITEM *s=ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem);
				if (s)
				{
					WORLDOBJECT *SubWorld=(WORLDOBJECT*)s->Data;
					DEFAULTOBJECTSPLINES *s=&SubWorld->Animations[ActualAnim];

					if (Interface->ItemList[10+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==0))
					s->Posx->AddKey(SubWorld->PosData.Pos.x,PlaybackPos);
					if (Interface->ItemList[11+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==1))
					s->Posy->AddKey(SubWorld->PosData.Pos.y,PlaybackPos);
					if (Interface->ItemList[12+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==2))
					s->Posz->AddKey(SubWorld->PosData.Pos.z,PlaybackPos);

					if ((Interface->ItemList[13+2].ButtonPushed) && (ScrollerID==9+2 || ScrollerID==3 || ScrollerID==4 || ScrollerID==5))
					{
						s->Rotx->AddKey(SubWorld->PosData.Quaternion.s,PlaybackPos);
						s->Roty->AddKey(SubWorld->PosData.Quaternion.v.x,PlaybackPos);
						s->Rotz->AddKey(SubWorld->PosData.Quaternion.v.y,PlaybackPos);
						s->Rotw->AddKey(SubWorld->PosData.Quaternion.v.z,PlaybackPos);
					}
					
					if (Interface->ItemList[16+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==6))
					s->Sclx->AddKey(SubWorld->PosData.Scale.x,PlaybackPos);
					if (Interface->ItemList[17+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==7))
					s->Scly->AddKey(SubWorld->PosData.Scale.y,PlaybackPos);
					if (Interface->ItemList[18+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==8))
					s->Sclz->AddKey(SubWorld->PosData.Scale.z,PlaybackPos);

					if (Interface->ItemList[19+2].ButtonPushed && (ScrollerID==9+2 || ScrollerID==9))
					{	
						if (!s->AnimID->KeyNum)
						{
							s->AnimTime->AddKey(0,0);
							s->AnimTime->AddKey(1,1);
							s->AnimID->AddKey((float)SwrldAnimList->ParentItem->SelectedItem,0);
						}
						else
						{
							s->AnimID->AddKey((float)SwrldAnimList->ParentItem->SelectedItem,PlaybackPos);
							s->AnimTime->AddKey(SWAnimPos,PlaybackPos);
							s->AnimTime->AddKey(0,PlaybackPos);
						}
					}

					s->Posx->InitVectors();
					s->Posy->InitVectors();
					s->Posz->InitVectors();
					s->Sclx->InitVectors();
					s->Scly->InitVectors();
					s->Sclz->InitVectors();
					Spline_QuaternionGetVectors(s->Rotx,s->Roty,s->Rotz,s->Rotw);

					
					WaitLeftButton=true;
				}
				else ErrorMessage="NO SUBWORLD SELECTED OR SUBWORLD NOT FOUND (In this case tell BoyC)!";
			}
			else
			ErrorMessage="CREATE AN OBJECT FIRST!";
		}
		else
		ErrorMessage="CREATE AN ANIMATION FIRST!";
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}


void __stdcall GUI_AddCamKey(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		if (ActualWorld->CamNum)
		{
			ITEM *C=ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem);
			if (C)
			{
				CAMSPLINES *Camera=(CAMSPLINES*)C->Data;
				CAMERA View=LayouterDisplay4->ParentItem->Cam;

				if (Interface->ItemList[11].ButtonPushed && (ScrollerID==10 || ScrollerID==2))
				Camera->Eyex->AddKey(View.Eye.x,PlaybackPos);
				if (Interface->ItemList[12].ButtonPushed && (ScrollerID==10 || ScrollerID==3))
				Camera->Eyey->AddKey(View.Eye.y,PlaybackPos);
				if (Interface->ItemList[13].ButtonPushed && (ScrollerID==10 || ScrollerID==4))
				Camera->Eyez->AddKey(View.Eye.z,PlaybackPos);
				if (Interface->ItemList[14].ButtonPushed && (ScrollerID==10 || ScrollerID==5))
				Camera->Trgx->AddKey(View.Target.x,PlaybackPos);
				if (Interface->ItemList[15].ButtonPushed && (ScrollerID==10 || ScrollerID==6))
				Camera->Trgy->AddKey(View.Target.y,PlaybackPos);
				if (Interface->ItemList[16].ButtonPushed && (ScrollerID==10 || ScrollerID==7))
				Camera->Trgz->AddKey(View.Target.z,PlaybackPos);
				if (Interface->ItemList[17].ButtonPushed && (ScrollerID==10 || ScrollerID==8))
				Camera->Fov->AddKey(View.Fov,PlaybackPos);
				if (Interface->ItemList[18].ButtonPushed && (ScrollerID==10 || ScrollerID==9))
				Camera->Roll->AddKey(View.Roll,PlaybackPos);

				Camera->Eyex->InitVectors();
				Camera->Eyey->InitVectors();
				Camera->Eyez->InitVectors();
				Camera->Trgx->InitVectors();
				Camera->Trgy->InitVectors();
				Camera->Trgz->InitVectors();
				Camera->Fov->InitVectors();
				Camera->Roll->InitVectors();
				
				WaitLeftButton=true;
			}
			else ErrorMessage="NO CAM SELECTED OR CAM NOT FOUND (in this case tell BoyC)!";
		}
		else ErrorMessage="CREATE A CAMERA FIRST!";
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall GUI_AddParticleKey(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		if (ActualWorld->AnimNum )
		{
			if (ActualWorld->ParticleCount)
			{
				ITEM *o=ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem);
				if (o)
				{
					WORLDOBJECT *Object=(WORLDOBJECT*)o->Data;
					DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];

					if (Interface->ItemList[16].ButtonPushed && (ScrollerID==15 || ScrollerID==0))
					s->Posx->AddKey(Object->PosData.Pos.x,PlaybackPos);
					if (Interface->ItemList[17].ButtonPushed && (ScrollerID==15 || ScrollerID==1))
					s->Posy->AddKey(Object->PosData.Pos.y,PlaybackPos);
					if (Interface->ItemList[18].ButtonPushed && (ScrollerID==15 || ScrollerID==2))
					s->Posz->AddKey(Object->PosData.Pos.z,PlaybackPos);

					if ((Interface->ItemList[19].ButtonPushed)&& (ScrollerID==15 || ScrollerID==3 || ScrollerID==4 || ScrollerID==5))
					{
						s->Rotx->AddKey(Object->PosData.Quaternion.s,PlaybackPos);
						s->Roty->AddKey(Object->PosData.Quaternion.v.x,PlaybackPos);
						s->Rotz->AddKey(Object->PosData.Quaternion.v.y,PlaybackPos);
						s->Rotw->AddKey(Object->PosData.Quaternion.v.z,PlaybackPos);
					}

					if (Interface->ItemList[22].ButtonPushed && (ScrollerID==15 || ScrollerID==6))
					s->Sclx->AddKey(Object->PosData.Scale.x,PlaybackPos);
					if (Interface->ItemList[23].ButtonPushed && (ScrollerID==15 || ScrollerID==7))
					s->Scly->AddKey(Object->PosData.Scale.y,PlaybackPos);
					if (Interface->ItemList[24].ButtonPushed && (ScrollerID==15 || ScrollerID==8))
					s->Sclz->AddKey(Object->PosData.Scale.z,PlaybackPos);

					if (Interface->ItemList[25].ButtonPushed && (ScrollerID==15 || ScrollerID==9))
					s->Red->AddKey(Object->EmitterData->Color1[0]*16,PlaybackPos);
					if (Interface->ItemList[26].ButtonPushed && (ScrollerID==15 || ScrollerID==10))
					s->Green->AddKey(Object->EmitterData->Color1[1]*16,PlaybackPos);
					if (Interface->ItemList[27].ButtonPushed && (ScrollerID==15 || ScrollerID==11))
					s->Blue->AddKey(Object->EmitterData->Color1[2]*16,PlaybackPos);
					if (Interface->ItemList[28].ButtonPushed && (ScrollerID==15 || ScrollerID==12))
					s->Alpha->AddKey(Object->EmitterData->Color1[3]*16,PlaybackPos);
					if (Interface->ItemList[29].ButtonPushed && (ScrollerID==15 || ScrollerID==13))
					s->Size->AddKey(Object->EmitterData->Size,PlaybackPos);
					if (Interface->ItemList[30].ButtonPushed && (ScrollerID==15 || ScrollerID==14))
					s->Prtfrme->AddKey(Object->EmitterData->ParticlesPerFrame,PlaybackPos);


					s->Posx->InitVectors();
					s->Posy->InitVectors();
					s->Posz->InitVectors();
					s->Sclx->InitVectors();
					s->Scly->InitVectors();
					s->Sclz->InitVectors();
					Spline_QuaternionGetVectors(s->Rotx,s->Roty,s->Rotz,s->Rotw);

					
					WaitLeftButton=true;
				}
				else ErrorMessage="NO OBJECT SELECTED OR OBJECT NOT FOUND (In this case tell BoyC)!";
			}
			else ErrorMessage="CREATE AN EMITTER FIRST!";
		}
		else
		ErrorMessage="CREATE AN ANIMATION FIRST!";
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}


void __stdcall GUI_SelectOnlyThis(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
}

void SetObjectSplines()
{
	if (ActualWorld)
	{
		ObjectSplines->ItemNum=0;
		ObjectSplines->Last=NULL;
		ObjectSplines->List=NULL;
		ObjList->ParentItem->ItemNum=0;
		for (ITEM *i=ActualWorld->ItemList->List->Next->Next->Next->Next; i; i=i->Next)
		{		
		if (((WORLDOBJECT*)i->Data)->Primitive!=aDDict_SPHEREEMITTER &&
			((WORLDOBJECT*)i->Data)->Primitive!=aDDict_CUBEEMITTER &&
			((WORLDOBJECT*)i->Data)->Primitive!=aDDict_CYLINDEREMITTER &&
			((WORLDOBJECT*)i->Data)->Primitive!=aDDict_SUBSCENE)
			{
				ObjectSplines->AddItem(i->Name,i->Data,GUI_SelectOnlyThis,nop,nop);
				ObjList->ParentItem->ItemNum++;
			}
		}
	}
	else
	{
		ObjectSplines->ItemNum=0;
		ObjectSplines->Last=NULL;
		ObjectSplines->List=NULL;
		ObjList->ParentItem->ItemNum=0;
	}
}

void SetCamSplines()
{
	if (ActualWorld)
	{
		CamSplines->ItemNum=0;
		CamSplines->Last=NULL;
		CamSplines->List=NULL;
		CamList->ParentItem->ItemNum=0;
		for (int x=0; x<ActualWorld->CamNum; x++)
		{		
			CAMSPLINES *i=&(ActualWorld->CamAnims[x]);
			CamSplines->AddItem(i->Name,i,GUI_SelectOnlyThis,nop,nop);
			CamList->ParentItem->ItemNum++;
		}
	}
	else
	{
		CamSplines->ItemNum=0;
		CamSplines->Last=NULL;
		CamSplines->List=NULL;
		CamList->ParentItem->ItemNum=0;
	}
}


void __stdcall GUI_SelectOnlyThisSubWorld(cINTERFACE* Interface, int ScrollerID)
{
	GUI_SelectOnlyThis(Interface,ScrollerID);
	SwrldAnimList->ParentItem->data=(((WORLD*)((WORLDOBJECT*)Interface->ClickedItem->Data)->Data)->AnimList);
	SwrldAnimList->ParentItem->ItemNum=(((WORLD*)((WORLDOBJECT*)Interface->ClickedItem->Data)->Data)->AnimNum);
}

void SetSubWorldSplines()
{
	if (ActualWorld)
	{
		SubWorldSplines->ItemNum=0;
		SubWorldSplines->Last=NULL;
		SubWorldSplines->List=NULL;
		SwrldList->ParentItem->ItemNum=0;
		for (ITEM *i=ActualWorld->ItemList->List->Next->Next->Next->Next; i; i=i->Next)
		{		
		if (((WORLDOBJECT*)i->Data)->Primitive==aDDict_SUBSCENE)
			{
				SubWorldSplines->AddItem(i->Name,i->Data,GUI_SelectOnlyThisSubWorld,nop,nop);
				SwrldList->ParentItem->ItemNum++;
			}
		}
	}
	else
	{
		SubWorldSplines->ItemNum=0;
		SubWorldSplines->Last=NULL;
		SubWorldSplines->List=NULL;
		SwrldList->ParentItem->ItemNum=0;
	}
}


void SetEmitterSplines()
{
	if (ActualWorld)
	{
		EmitterSplines->ItemNum=0;
		EmitterSplines->Last=NULL;
		EmitterSplines->List=NULL;
		PrtList->ParentItem->ItemNum=0;
		for (ITEM *i=ActualWorld->ItemList->List->Next->Next->Next->Next; i; i=i->Next)
		{		
		if (((WORLDOBJECT*)i->Data)->Primitive==aDDict_SPHEREEMITTER ||
			((WORLDOBJECT*)i->Data)->Primitive==aDDict_CUBEEMITTER ||
			((WORLDOBJECT*)i->Data)->Primitive==aDDict_CYLINDEREMITTER)
			{
				EmitterSplines->AddItem(i->Name,i->Data,GUI_SelectOnlyThis,nop,nop);
				PrtList->ParentItem->ItemNum++;
			}
		}
	}
	else
	{
		EmitterSplines->ItemNum=0;
		EmitterSplines->Last=NULL;
		EmitterSplines->List=NULL;
		PrtList->ParentItem->ItemNum=0;
	}
}

void SetActualWorldAnimListToMainAnimList()
{
	if (ActualWorld) 
	{
		AnimList->ParentItem->data=ActualWorld->AnimList;
		AnimList->ParentItem->ItemNum=ActualWorld->AnimList->ItemNum;
		AnimList->ParentItem->SelectedItem=min(AnimList->ParentItem->ItemNum-1,AnimList->ParentItem->SelectedItem);
	}
	else
	{
		AnimList->ParentItem->data=NULL;
		AnimList->ParentItem->ItemNum=0;
	}
}

void SetActualWorldCamListToMainCamList()
{
	if (ActualWorld) 
	{
		CamList->ParentItem->data=ActualWorld->CameraList;
		CamList->ParentItem->ItemNum=ActualWorld->CameraList->ItemNum;
	}
	else
	{
		CamList->ParentItem->data=NULL;
		CamList->ParentItem->ItemNum=0;
	}
}


void __stdcall GUI_SetEmitterHead(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			ActualWorld->ObjectList[x].EmitterData->Head=Interface->ItemList[ScrollerID].ButtonPushed;
}

void __stdcall GUI_SetEmitterRandRotate(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			ActualWorld->ObjectList[x].EmitterData->RandRotate=Interface->ItemList[ScrollerID].ButtonPushed;
}

void __stdcall GUI_SetEmitterFixUp(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			ActualWorld->ObjectList[x].EmitterData->FixedUp=Interface->ItemList[ScrollerID].ButtonPushed;
}

void __stdcall GUI_SetEmitterTail(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			ActualWorld->ObjectList[x].EmitterData->Tail=Interface->ItemList[ScrollerID].ButtonPushed;
			if (ActualWorld->ObjectList[x].EmitterData->Tail && !ActualWorld->ObjectList[x].EmitterData->TailInfo)
			{
				for (int y=0; y<ActualWorld->ObjectList[x].EmitterData->MaxParticles; y++) ActualWorld->ObjectList[x].EmitterData->Particles[y].Tail=new VECTOR3[256];
				ActualWorld->ObjectList[x].EmitterData->TailInfo=true;
			}
		}
}

void __stdcall GUI_ResetSceneParticles(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	{
		if (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || 
			ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER ||
			ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER)
			{
				//memset(ActualWorld->ObjectList[x].EmitterData->Particles,0,sizeof(PARTICLE)*ActualWorld->ObjectList[x].EmitterData->MaxParticles);
				for (int y=0; y<ActualWorld->ObjectList[x].EmitterData->MaxParticles; y++) ActualWorld->ObjectList[x].EmitterData->Particles[y].Active=false;
				ActualWorld->ObjectList[x].EmitterData->ParticleNumBuffer=ActualWorld->ObjectList[x].EmitterData->ParticlesPerFrame;
			}
	}
}


void __stdcall CreateCubeEmitter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ParticleCount++;
		//GenerateP rimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_CUBEEMITTER,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_CUBEEMITTER;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="EMITTER CUBE";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles=MaxParticles;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->DefaultAge=ParticleLife;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->AgeChaos=ParticleLifeChaos;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticlesPerFrame=ParticlesPerFrame;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticleNumBuffer=ParticlesPerFrame;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->LastFrameChecked=timeGetTime();
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles=new PARTICLE[MaxParticles];
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[0]=r1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[1]=g1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[2]=b1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[3]=a1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[4]=c1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[0]=r2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[1]=g2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[2]=b2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[3]=a2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[4]=c2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Size=ParticleSize;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].DSTBlend=GL_SRC_ALPHA;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].SRCBlend=GL_ONE;
		memset(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles,0,sizeof(PARTICLE)*MaxParticles);
		for (int x=0; x<ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles; x++) ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles[x].Tail=new VECTOR3[256];
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreateSphereEmitter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ParticleCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_SPHEREEMITTER,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_SPHEREEMITTER;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="EMITTER SPHERE";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=InnerRadius;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles=MaxParticles;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->DefaultAge=ParticleLife;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->AgeChaos=ParticleLifeChaos;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticlesPerFrame=ParticlesPerFrame;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticleNumBuffer=ParticlesPerFrame;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->LastFrameChecked=timeGetTime();
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles=new PARTICLE[MaxParticles];
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[0]=r1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[1]=g1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[2]=b1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[3]=a1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[4]=c1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[0]=r2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[1]=g2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[2]=b2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[3]=a2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[4]=c2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Size=ParticleSize;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].DSTBlend=GL_SRC_ALPHA;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].SRCBlend=GL_ONE;
		memset(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles,0,sizeof(PARTICLE)*MaxParticles);
		for (int x=0; x<ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles; x++) ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles[x].Tail=new VECTOR3[256];
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreateCylinderEmitter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ParticleCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_CYLINDEREMITTER,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_CYLINDEREMITTER;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="EMITTER CYLINDER";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=InnerRadius;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles=MaxParticles;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->DefaultAge=ParticleLife;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->AgeChaos=ParticleLifeChaos;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticlesPerFrame=ParticlesPerFrame;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticleNumBuffer=ParticlesPerFrame;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->LastFrameChecked=timeGetTime();
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles=new PARTICLE[MaxParticles];
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[0]=r1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[1]=g1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[2]=b1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[3]=a1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color1[4]=c1/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[0]=r2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[1]=g2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[2]=b2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[3]=a2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Color2[4]=c2/255.0f;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Size=ParticleSize;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].DSTBlend=GL_SRC_ALPHA;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].SRCBlend=GL_ONE;
		memset(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles,0,sizeof(PARTICLE)*MaxParticles);
		for (int x=0; x<ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles; x++) ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles[x].Tail=new VECTOR3[256];
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreateGravity(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_GRAVITY,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_GRAVITY;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="GRAVITY";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=GravityStrength;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param2=0;
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreateGravity2(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_GRAVITY,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_GRAVITY;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="GRAVITY (DIR)";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=GravityStrength;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param2=1;
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreatePlaneDeflect(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_PLANEDEFLECTOR,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_PLANEDEFLECTOR;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="DEFLECT. PLANE";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=DeflectLoss;
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreateSphereDeflect(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_SPHEREDEFLECTOR,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_SPHEREDEFLECTOR;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="DEFLECT. SPHERE";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=DeflectLoss;
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall CreateSphereSlow(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		//GeneratePrimitive(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1],aDDict_SPHEREDEFLECTOR,0,0,0,0,0);
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_SPHERESLOWER;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="SLOWER SPHERE";
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Param1=DeflectLoss;
		MainLayouterScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else ErrorMessage="CREATE A WORLD FIRST!";
}


void __stdcall GUI_SetParticlesPerFrame(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->ParticlesPerFrame=ParticlesPerFrame;
	}
}

void __stdcall GUI_SetParticlesPerFrameAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			ParticlesPerFrame=ActualWorld->ObjectList[x].EmitterData->ParticlesPerFrame;
			break;
		}
}

void __stdcall GUI_SetParticleLife(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->DefaultAge=ParticleLife;
	}
}

void __stdcall GUI_SetParticleLifeAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			ParticleLife=ActualWorld->ObjectList[x].EmitterData->DefaultAge;
			break;
		}
}

void __stdcall GUI_SetParticleLifeChaos(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->AgeChaos=ParticleLifeChaos;
	}
}

void __stdcall GUI_SetParticleLifeChaosAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			ParticleLifeChaos=ActualWorld->ObjectList[x].EmitterData->AgeChaos;
			break;
		}
}

void __stdcall GUI_SetParticleInnerRadius(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Param1=InnerRadius;
	}
}

void __stdcall GUI_SetParticleInnerRadiusAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			InnerRadius=ActualWorld->ObjectList[x].EmitterData->Param1;
			break;
		}
}

void __stdcall GUI_SetParticleSpeed(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER))
				ActualWorld->ObjectList[x].EmitterData->Speed=DefaultSpeed;
	}
}

void __stdcall GUI_SetParticleSpeedAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER))
		{
			DefaultSpeed=ActualWorld->ObjectList[x].EmitterData->Speed;
			break;
		}
}

void __stdcall GUI_SetParticleSpeedChaos(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER))
				ActualWorld->ObjectList[x].EmitterData->SpeedChaos=SpeedChaos;
	}
}

void __stdcall GUI_SetParticleSpeedChaosAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER))
		{
			SpeedChaos=ActualWorld->ObjectList[x].EmitterData->SpeedChaos;
			break;
		}
}

void __stdcall GUI_SetParticleDirChaos(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->DirChaos=DirChaos;
	}
}

void __stdcall GUI_SetParticleDirChaosAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			DirChaos=ActualWorld->ObjectList[x].EmitterData->DirChaos;
			break;
		}
}


void __stdcall GUI_SetGravityStrength(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_GRAVITY))
				ActualWorld->ObjectList[x].EmitterData->Param1=GravityStrength;
	}
}

void __stdcall GUI_SetGravityStrengthAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_GRAVITY))
		{
			GravityStrength=ActualWorld->ObjectList[x].EmitterData->Param1;
			break;
		}
}

void __stdcall GUI_SetDeflectLoss(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_PLANEDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHERESLOWER))
				ActualWorld->ObjectList[x].EmitterData->Param1=DeflectLoss;
	}
}

void __stdcall GUI_SetDeflectLossAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_PLANEDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHERESLOWER))
		{
			DeflectLoss=ActualWorld->ObjectList[x].EmitterData->Param1;
			break;
		}
}

void __stdcall GUI_SetMoveLoss(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_PLANEDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHERESLOWER))
				ActualWorld->ObjectList[x].EmitterData->Param2=MoveLoss;
	}
}

void __stdcall GUI_SetMoveLossAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_PLANEDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREDEFLECTOR || ActualWorld->ObjectList[x].Primitive==aDDict_SPHERESLOWER))
		{
			MoveLoss=ActualWorld->ObjectList[x].EmitterData->Param2;
			break;
		}
}


void __stdcall GUI_SetRed1(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color1[0]=r1/255.0f;
	}
}

void __stdcall GUI_SetRed1After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			r1=(int)(ActualWorld->ObjectList[x].EmitterData->Color1[0]*255);
			break;
		}
}

void __stdcall GUI_SetGreen1(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color1[1]=g1/255.0f;
	}
}

void __stdcall GUI_SetGreen1After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			g1=(int)(ActualWorld->ObjectList[x].EmitterData->Color1[1]*255);
			break;
		}
}

void __stdcall GUI_SetBlue1(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color1[2]=b1/255.0f;
	}
}

void __stdcall GUI_SetBlue1After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			b1=(int)(ActualWorld->ObjectList[x].EmitterData->Color1[2]*255);
			break;
		}
}

void __stdcall GUI_SetAlpha1(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color1[3]=a1/255.0f;
	}
}

void __stdcall GUI_SetAlpha1After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			a1=(int)(ActualWorld->ObjectList[x].EmitterData->Color1[3]*255);
			break;
		}
}

void __stdcall GUI_SetChaos1(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color1[4]=c1/255.0f;
	}
}

void __stdcall GUI_SetChaos1After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			c1=(int)(ActualWorld->ObjectList[x].EmitterData->Color1[4]*255);
			break;
		}
}

void __stdcall GUI_SetRed2(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color2[0]=r2/255.0f;
	}
}

void __stdcall GUI_SetRed2After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			r2=(int)(ActualWorld->ObjectList[x].EmitterData->Color2[0]*255);
			break;
		}
}

void __stdcall GUI_SetGreen2(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color2[1]=g2/255.0f;
	}
}

void __stdcall GUI_SetGreen2After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			g2=(int)(ActualWorld->ObjectList[x].EmitterData->Color2[1]*255);
			break;
		}
}

void __stdcall GUI_SetBlue2(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color2[2]=b2/255.0f;
	}
}

void __stdcall GUI_SetBlue2After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			b2=(int)(ActualWorld->ObjectList[x].EmitterData->Color2[2]*255);
			break;
		}
}

void __stdcall GUI_SetAlpha2(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color2[3]=a2/255.0f;
	}
}

void __stdcall GUI_SetAlpha2After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			a2=(int)(ActualWorld->ObjectList[x].EmitterData->Color2[3]*255);
			break;
		}
}

void __stdcall GUI_SetChaos2(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Color2[4]=c2/255.0f;
	}
}

void __stdcall GUI_SetChaos2After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			c2=(int)(ActualWorld->ObjectList[x].EmitterData->Color2[4]*255);
			break;
		}
}

void __stdcall GUI_SetParticleSize(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Size=ParticleSize;
	}
}

void __stdcall GUI_SetParticleSizeAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			ParticleSize=ActualWorld->ObjectList[x].EmitterData->Size;
			break;
		}
}

void __stdcall GUI_SetCamfade(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->CamFade=Camfade;
	}
}

void __stdcall GUI_SetCamfadeAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			Camfade=ActualWorld->ObjectList[x].EmitterData->CamFade;
			break;
		}
}

void __stdcall GUI_SetParticleRot(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->Rotspeed=RotSpeed;
	}
}

void __stdcall GUI_SetParticleRotAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			RotSpeed=ActualWorld->ObjectList[x].EmitterData->Rotspeed;
			break;
		}
}

void __stdcall GUI_SetParticleRotChs(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->RotChaos=RotChaos;
	}
}

void __stdcall GUI_SetParticleRotChsAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			RotChaos=ActualWorld->ObjectList[x].EmitterData->RotChaos;
			break;
		}
}


void __stdcall GUI_SetTailLength1(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->TailLength=TailLength;
	}
}

void __stdcall GUI_SetTailLength1After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			TailLength=ActualWorld->ObjectList[x].EmitterData->TailLength;
			break;
		}
}

void __stdcall GUI_SetTailLength2(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
				ActualWorld->ObjectList[x].EmitterData->TailLength2=TailLength2;
	}
}

void __stdcall GUI_SetTailLength2After(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
		{
			TailLength2=ActualWorld->ObjectList[x].EmitterData->TailLength2;
			break;
		}
}

void __stdcall SetLightSettingsToActualWorld(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<8; x++)
	{
		//GUIITEM *LightItem=ActualWorld->ItemList->List->Next->Next->SubItems;
		//for (int y=0; y<x; y++) LightItem=LightItem->Next;
		if (ActualWorld && LLightMenus[x]->ItemList[1].Interface->ItemList[2].ButtonPushed) //SpotLight
		{
			ActualWorld->Lights[x].Position[3]=1;
			ActualWorld->Lights[x].Spot_Cutoff=LLightData[x].cu;
		}

		if (ActualWorld && LLightMenus[x]->ItemList[1].Interface->ItemList[1].ButtonPushed) //DotLight
		{
			ActualWorld->Lights[x].Spot_Cutoff=180;
			ActualWorld->Lights[x].Position[3]=1;
		}

		if (ActualWorld && LLightMenus[x]->ItemList[1].Interface->ItemList[0].ButtonPushed) //DirLight
		{
			ActualWorld->Lights[x].Spot_Cutoff=180;
			ActualWorld->Lights[x].Position[3]=0;
		}

		if (ActualWorld)
		{
			ActualWorld->Lights[x].Lit=LLightMenus[x]->ItemList[0].ButtonPushed;
			ActualWorld->Lights[x].Color[0]=LLightData[x].r/255.0f;
			ActualWorld->Lights[x].Color[1]=LLightData[x].g/255.0f;
			ActualWorld->Lights[x].Color[2]=LLightData[x].b/255.0f;
			ActualWorld->Lights[x].Ambient[0]=LLightData[x].ar/255.0f;
			ActualWorld->Lights[x].Ambient[1]=LLightData[x].ag/255.0f;
			ActualWorld->Lights[x].Ambient[2]=LLightData[x].ab/255.0f;
		}
	}
}

void SetActualWorldToLightSettings()
{
	for (int x=0; x<8; x++)
	{
		LLightMenus[x]->ItemList[0].ButtonPushed=ActualWorld->Lights[x].Lit;

		LLightMenus[x]->ItemList[1].Interface->ItemList[2].ButtonPushed=(ActualWorld->Lights[x].Position[3]==1 && ActualWorld->Lights[x].Spot_Cutoff!=180);
		LLightMenus[x]->ItemList[1].Interface->ItemList[1].ButtonPushed=(ActualWorld->Lights[x].Spot_Cutoff==180 && ActualWorld->Lights[x].Position[3]==1);
		LLightMenus[x]->ItemList[1].Interface->ItemList[0].ButtonPushed=(ActualWorld->Lights[x].Spot_Cutoff==180 && ActualWorld->Lights[x].Position[3]==0);

		if (LLightMenus[x]->ItemList[1].Interface->ItemList[2].ButtonPushed) LLightMenus[x]->ItemList[1].Interface->SetActiveBranch(2);
		if (LLightMenus[x]->ItemList[1].Interface->ItemList[1].ButtonPushed) LLightMenus[x]->ItemList[1].Interface->SetActiveBranch(1);
		if (LLightMenus[x]->ItemList[1].Interface->ItemList[0].ButtonPushed) LLightMenus[x]->ItemList[1].Interface->SetActiveBranch(0);

		LLightData[x].r=(int)(ActualWorld->Lights[x].Color[0]*255.0f);
		LLightData[x].g=(int)(ActualWorld->Lights[x].Color[1]*255.0f);
		LLightData[x].b=(int)(ActualWorld->Lights[x].Color[2]*255.0f);
		LLightData[x].ar=(int)(ActualWorld->Lights[x].Ambient[0]*255.0f);
		LLightData[x].ag=(int)(ActualWorld->Lights[x].Ambient[1]*255.0f);
		LLightData[x].ab=(int)(ActualWorld->Lights[x].Ambient[2]*255.0f);
	}
}

void SetEnvironmentToActualWorld()
{
	if (ActualWorld)
	{
		*(int*)MainLayouterScrollerData=(int)ActualWorld->ItemList;
		MainLayouterScroller->ParentItem->ItemNum=ActualWorld->ItemList->ItemNum;
		SetActualWorldToLightSettings();
		FogRed=ActualWorld->FogCol[0];
		FogGreen=ActualWorld->FogCol[1];
		FogBlue=ActualWorld->FogCol[2];
		FogIntensity1=ActualWorld->FogStart;
		FogIntensity2=ActualWorld->FogEnd;

		LightingButtonInterface->ParentItem->ButtonPushed=ActualWorld->Lighting;
		
		FogButtonInterface->ParentItem->ButtonPushed=ActualWorld->Fog;
		ActualAnim=0;//max(0,min(ActualAnim,ActualWorld->AnimNum-1));
		AnimList->ParentItem->SelectedItem=0;

		LayouterDisplay4->ParentItem->Cam=ActualWorld->Camera;
		SetActualWorldAnimListToMainAnimList();
		SetActualWorldCamListToMainCamList();
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else
	{
		*(int*)MainLayouterScrollerData=NULL;
		MainLayouterScroller->ParentItem->ItemNum=0;
		ActualAnim=0;
		memset(&LayouterDisplay4->ParentItem->Cam,0,sizeof(LayouterDisplay4->ParentItem->Cam));
		LayouterDisplay4->ParentItem->Cam.Fov=45;
		LayouterDisplay4->ParentItem->Cam.Up.y=1;
		LayouterDisplay4->ParentItem->Cam.Eye.z=10;
		LayouterDisplay4->ParentItem->Cam.Eye.y=7;
		SetActualWorldAnimListToMainAnimList();
		SetActualWorldCamListToMainCamList();
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
}

void __stdcall SwitchLight(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		GUI_ButtonSwitch(Interface,ButtonId);
		ActualWorld->Lighting=Interface->ItemList[ButtonId].ButtonPushed;
	}
}

void __stdcall SwitchLightCheck(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		Interface->ItemList[ButtonId].ButtonPushed=ActualWorld->Lighting;
	}
}

void __stdcall CreateDummy(cINTERFACE* Interface, int ButtonId)
{
	//MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name="DUMMY\0";
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Data=NULL;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_DUMMY;
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		MainLayouterScroller->ParentItem->ItemNum++;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else
	{
		if (!ActualWorld) ErrorMessage="CREATE A WORLD FIRST!";
	}
	WaitLeftButton=true;
}

void __stdcall CreateModell(cINTERFACE* Interface, int ButtonId)
{
	if (!LayouterSelectedScene && ActualWorld && ActualScene) LayouterSelectedScene=Scenes;
	if (ActualWorld && LayouterSelectedScene)
	{
		MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
		ActualWorld->AddObject();
		ActualWorld->ObjectCount++;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name=new char[strlen(LayouterSelectedScene->Name)+1];
		memset(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name,0,strlen(LayouterSelectedScene->Name)+1);
		memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name,LayouterSelectedScene->Name,strlen(LayouterSelectedScene->Name));
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Data=LayouterSelectedScene;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_SCENE;
		for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
		MainLayouterScroller->ParentItem->ItemNum++;
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
	else
	{
		if (!ActualWorld) ErrorMessage="CREATE A WORLD FIRST!";
	}
	WaitLeftButton=true;
}

bool CheckSubWorldTree(WORLD *Tobefound, WORLD *Tree)
{
	if (Tobefound==Tree) return true;
	for (int x=0; x<Tree->ObjectNum; x++)
	{
		if (Tree->ObjectList[x].Primitive==aDDict_SUBSCENE)
			if (Tree->ObjectList[x].Data==Tobefound) return true;
			else
			{
				bool a=CheckSubWorldTree(Tobefound,(WORLD*)Tree->ObjectList[x].Data);
				if (a) return true;
			}
	}
	return false;
}

void __stdcall CreateSubWorld(cINTERFACE* Interface, int ButtonId)
{
	if (!LayouterSelectedWorld && ActualWorld && ActualWorld) LayouterSelectedWorld=Worlds;
	if (ActualWorld && LayouterSelectedWorld)
	{
		if (!CheckSubWorldTree(ActualWorld,LayouterSelectedWorld))
		{
			MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
			ActualWorld->AddObject();
			ActualWorld->SubWorldCount++;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name=new char[strlen(LayouterSelectedWorld->Name)+1];
			memset(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name,0,strlen(LayouterSelectedWorld->Name)+1);
			memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name,LayouterSelectedWorld->Name,strlen(LayouterSelectedWorld->Name));
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Data=LayouterSelectedWorld;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=aDDict_SUBSCENE;
			for (ITEM *I=ActualWorld->ItemList->List; I; I=I->Next) I->Selected=false;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
			MainLayouterScroller->ParentItem->ItemNum++;
			SetObjectSplines();
			SetEmitterSplines();
			SetSubWorldSplines();
		}
		else
		{
			ErrorMessage="YOU CAN'T PUT A WORLD RECURSIVELY INSIDE ITSELF!";
		}
	}
	else
	{
		if (!ActualWorld) ErrorMessage="CREATE A WORLD FIRST!";
	}
	WaitLeftButton=true;
}


void __stdcall CreateSubScene(cINTERFACE* Interface, int ButtonId)
{
	WaitLeftButton=true;
}

void __stdcall Scroller_SelectOnlyThisWorld(cINTERFACE* ScrollerInterface, int ScrollerID)
{
//	sadf
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
	if ((MainGUI.ItemList[3].ButtonPushed && !LayouterObjectsMenu->ParentItem->ButtonPushed) || MainGUI.ItemList[0].ButtonPushed)
	{
		ActualWorld=(WORLD*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
		SetEnvironmentToActualWorld();
		for (int x=0; x<ActualWorld->ObjectNum; x++)
		{
			if (ActualWorld->ObjectList[x].EmitterData)
				ActualWorld->ObjectList[x].EmitterData->LastFrameChecked=timeGetTime();
		}
	}

	if (MainGUI.ItemList[3].ButtonPushed && LayouterObjectsMenu->ParentItem->ButtonPushed)
	{
		LayouterSelectedWorld=(WORLD*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
	}

	if (MainGUI.ItemList[0].ButtonPushed)
	{
		if (SelectedEvent)
		{
			((DISPLAY3DTEMPLATE*)SelectedEvent->EffectData)->World=ActualWorld;
		}
	}
}

void __stdcall GUI_SelectOnlyThisAnim(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
	ANIMDATA *Anim=(ANIMDATA*)(
		                       
		                       ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)
							   
							   ->Data);
	for (int i=0; i<ActualWorld->AnimNum; i++)
	{
		if (ActualWorld->Animations[i].AnimID==Anim->AnimID) ActualAnim=i;
	}
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (!ActualWorld->ObjectList[x].Deleted)
	{
		DEFAULTOBJECTSPLINES *s=&ActualWorld->ObjectList[x].Animations[ActualAnim];
		if (s->Posx->KeyNum) ActualWorld->ObjectList[x].PosData.Pos.x=s->Posx->GetInterpolatedValue(PlaybackPos);
		if (s->Posy->KeyNum) ActualWorld->ObjectList[x].PosData.Pos.y=s->Posy->GetInterpolatedValue(PlaybackPos);
		if (s->Posz->KeyNum) ActualWorld->ObjectList[x].PosData.Pos.z=s->Posz->GetInterpolatedValue(PlaybackPos);
		if (s->Sclx->KeyNum) ActualWorld->ObjectList[x].PosData.Scale.x=s->Sclx->GetInterpolatedValue(PlaybackPos);
		if (s->Scly->KeyNum) ActualWorld->ObjectList[x].PosData.Scale.y=s->Scly->GetInterpolatedValue(PlaybackPos);
		if (s->Sclz->KeyNum) ActualWorld->ObjectList[x].PosData.Scale.z=s->Sclz->GetInterpolatedValue(PlaybackPos);
	}
}

void __stdcall GUI_SelectOnlyThisCam(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
	CAMSPLINES *Camera=(CAMSPLINES*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
	if (Camera->Eyex->KeyNum) LayouterDisplay4->ParentItem->Cam.Eye.x=Camera->Eyex->GetInterpolatedValue(PlaybackPos);
	if (Camera->Eyey->KeyNum) LayouterDisplay4->ParentItem->Cam.Eye.y=Camera->Eyey->GetInterpolatedValue(PlaybackPos);
	if (Camera->Eyez->KeyNum) LayouterDisplay4->ParentItem->Cam.Eye.z=Camera->Eyez->GetInterpolatedValue(PlaybackPos);
	if (Camera->Trgx->KeyNum) LayouterDisplay4->ParentItem->Cam.Target.x=Camera->Trgx->GetInterpolatedValue(PlaybackPos);
	if (Camera->Trgy->KeyNum) LayouterDisplay4->ParentItem->Cam.Target.y=Camera->Trgy->GetInterpolatedValue(PlaybackPos);
	if (Camera->Trgz->KeyNum) LayouterDisplay4->ParentItem->Cam.Target.z=Camera->Trgz->GetInterpolatedValue(PlaybackPos);
	if (Camera->Fov->KeyNum)  LayouterDisplay4->ParentItem->Cam.Fov=Camera->Fov->GetInterpolatedValue(PlaybackPos);
	if (Camera->Roll->KeyNum) LayouterDisplay4->ParentItem->Cam.Roll=Camera->Roll->GetInterpolatedValue(PlaybackPos);
}

void LBuildLightGUI(cINTERFACE *Light, int LightNumber)
{
	//WorldListScroller=new ITEMLIST();
	Light->AddSkinnedButton(899-850,295+20,LitButton,"TURN LIGHT ON/OFF",GUI_ButtonSwitch,nop,nop,SetLightSettingsToActualWorld);
	cINTERFACE *TypeSelector=Light->AddSubInterface();
	TypeSelector->AddSkinnedButton(852-850,313+20,DirectButton,"SET TO DIRECTIONAL LIGHT",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *DotLight=TypeSelector->AddSkinnedButton(909-850,313+20,DotButton,"SET TO DOTLIGHT",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *SpotLight=TypeSelector->AddSkinnedButton(966-850,313+20,SpotButton,"SET TO SPOTLIGHT",GUI_ButtonSelect,nop,nop,nop);
	TypeSelector->ItemList[0].ButtonPushed=true;
	TypeSelector->SetActiveBranch(0);

	Light->AddByteBar(862-850,337+20,136,GUI_RED,&LLightData[LightNumber-1].r,0,255,0,"RED: ","LIGHT RED INTENSITY");
	Light->AddByteBar(862-850,356+20,136,GUI_GREEN,&LLightData[LightNumber-1].g,0,255,0,"GREEN: ","LIGHT GREEN INTENSITY");
	Light->AddByteBar(862-850,375+20,136,GUI_BLUE,&LLightData[LightNumber-1].b,0,255,0,"BLUE: ","LIGHT BLUE INTENSITY");
	
	Light->AddByteBar(862-850,400+20,136,GUI_RED,&LLightData[LightNumber-1].ar,0,255,0,"AMBIENT RED: ","AMBIENT LIGHT RED INTENSITY");
	Light->AddByteBar(862-850,419+20,136,GUI_GREEN,&LLightData[LightNumber-1].ag,0,255,0,"AMBIENT GREEN: ","AMBIENT LIGHT GREEN INTENSITY");
	Light->AddByteBar(862-850,438+20,136,GUI_BLUE,&LLightData[LightNumber-1].ab,0,255,0,"AMBIENT BLUE: ","AMBIENT LIGHT BLUE INTENSITY");

	DotLight->AddByteBar(862-850,465+20,136,GUI_NONE,&LLightData[LightNumber-1].ca,0,255,0,"CONSTANT ATT: ","LIGHT CONSTANT ATTENUATION");
	DotLight->AddByteBar(862-850,484+20,136,GUI_NONE,&LLightData[LightNumber-1].la,0,255,0,"LINEAR ATT: ","LIGHT LINEAR ATTENUATION");
	DotLight->AddByteBar(862-850,503+20,136,GUI_NONE,&LLightData[LightNumber-1].qa,0,255,0,"QUADRATIC ATT: ","LIGHT QUADRATIC ATTENUATION");

	SpotLight->AddByteBar(862-850,465+20,136,GUI_NONE,&LLightData[LightNumber-1].ca,0,255,0,"CONSTANT ATT: ","LIGHT CONSTANT ATTENUATION");
	SpotLight->AddByteBar(862-850,484+20,136,GUI_NONE,&LLightData[LightNumber-1].la,0,255,0,"LINEAR ATT: ","LIGHT LINEAR ATTENUATION");
	SpotLight->AddByteBar(862-850,503+20,136,GUI_NONE,&LLightData[LightNumber-1].qa,0,255,0,"QUADRATIC ATT: ","LIGHT QUADRATIC ATTENUATION");
	
	SpotLight->AddByteBar(862-850,528+20,136,GUI_NONE,&LLightData[LightNumber-1].cu,0,90,45,"CUTOFF: ","SPOTLIGHT CUTOFF");
	SpotLight->AddByteBar(862-850,547+20,136,GUI_NONE,&LLightData[LightNumber-1].ex,0,255,0,"EXPONENT: ","SPOTLIGHT EXPONENT");
}

void __stdcall GUI_SelectNonSpline(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSelect(Interface,ButtonId);
	/*if (!LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[0].Interface->ItemList[1].Interface->ItemList[0].ButtonPushed &&
		!LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[1].Interface->ItemList[1].Interface->ItemList[0].ButtonPushed &&
		!LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[2].Interface->ItemList[1].Interface->ItemList[0].ButtonPushed &&
		!LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[3].Interface->ItemList[1].Interface->ItemList[0].ButtonPushed)
	{
		LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ActiveBranch=true;
		LayouterDisplay2->ParentItem->ParentInterface->ParentItem->ActiveBranch=true;
	}*/
}

void __stdcall GUI_SelectSpline(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSelect(Interface,ButtonId);

	/*LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ActiveBranch=false;
	LayouterDisplay2->ParentItem->ParentInterface->ParentItem->ActiveBranch=false;
	LayouterDisplay3->ParentItem->ParentInterface->ParentItem->ActiveBranch=true;
	LayouterDisplay4->ParentItem->ParentInterface->ParentItem->ActiveBranch=true;
	for (int x=0; x<4; x++)
	{
		GUIITEM *a=&LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[x].Interface->ItemList[0];
		a->x1=a->ox1;
		a->y1=a->oy1;
		a->x2=a->ox2;
		a->y2=a->oy2;
		cINTERFACE *b=LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[x].Interface->ItemList[1].Interface;
		b->ItemList[0].ButtonPushed=false;

		for (int y=0; y<b->ItemNum; y++)
		{
			b->ItemList[y].x1=b->ItemList[y].ox1;
			b->ItemList[y].y1=b->ItemList[y].oy1;
			b->ItemList[y].x2=b->ItemList[y].ox2;
			b->ItemList[y].y2=b->ItemList[y].oy2;
			for (int z=0; z<b->ItemList[y].Interface->ItemNum; z++)
			{
				b->ItemList[y].Interface->ItemList[z].x1=b->ItemList[y].Interface->ItemList[z].ox1;
				b->ItemList[y].Interface->ItemList[z].y1=b->ItemList[y].Interface->ItemList[z].oy1;
				b->ItemList[y].Interface->ItemList[z].x2=b->ItemList[y].Interface->ItemList[z].ox2;
				b->ItemList[y].Interface->ItemList[z].y2=b->ItemList[y].Interface->ItemList[z].oy2;
			}
		}

	}*/

	SetObjectSplines();
	SetEmitterSplines();
	SetSubWorldSplines();
}

void __stdcall GUI_ButtonSelect_LLightMenuMain(cINTERFACE* Interface, int ScrollerID)
{
	GUI_SelectNonSpline(Interface,ScrollerID);
	if (ActualWorld)
	{
		MainLayouterScroller->ParentItem->SelectedItem=2;
		//ActualWorld->ItemList->List->Next->Next->SubOpened=true;
		ITEM *Buffer=ActualWorld->ItemList->List->Next->Next->SubItems;
		for (int x=0; x<8; x++) 
		{
			Buffer->Selected=Interface->ItemList[ScrollerID].Interface->ItemList[0].Interface->ItemList[x].ButtonPushed;
			Buffer=Buffer->Next;
		}
	}
}
void __stdcall GUI_ButtonSelect_LLightMenu(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);

	if (ActualWorld)
	{
		ITEM *Buffer=ActualWorld->ItemList->List->Next->Next->SubItems;
		int x;
		for (x=0; x<8; x++) 
		{
			Buffer->Selected=false;
			Buffer=Buffer->Next;
		}

		Buffer=ActualWorld->ItemList->List->Next->Next->SubItems;
		for (x=0; x<ScrollerID; x++) Buffer=Buffer->Next;
		Buffer->Selected=true;	
		MainLayouterScroller->ParentItem->SelectedItem=2;
	}
}

void __stdcall Modeller_NewWorldCallback(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		{
			// ADD NEW World
			WORLD *Buffer=new WORLD();
			Buffer->Name=new char[strlen(CommandLine)+1];
			memset(Buffer->Name,0,strlen(CommandLine)+1);
			memcpy(Buffer->Name,CommandLine,strlen(CommandLine));

			if (!Worlds)
			{
				LastWorld=Worlds=Buffer;
				Buffer->ID=0;
			}
			else
			{
				WorldScroller->ParentItem->SelectedItem++;
				Buffer->ID=LastWorld->ID+1;
				LastWorld->Next=Buffer;
				LastWorld=Buffer;
			}

			ActualWorld=LastWorld;
			LastWorld->ItemList=new ITEMLIST();
			ITEM *WorldData=LastWorld->ItemList->AddItem("WORLD DATA",NULL,nop,SubOpenClose,nop);
			char *NameString=new char[strlen(CommandLine)+51];
			memset(NameString,0,strlen(CommandLine)+51);
			_snprintf(NameString,strlen(CommandLine)+51,"%s",CommandLine);
			WorldData->AddSubItem(NameString,NULL,nop,nop,nop);
			NameString=new char[10];
			memset(NameString,0,10);
			_snprintf(NameString,10,"ID: %d",LastWorld->ID);
			WorldData->AddSubItem(NameString,NULL,nop,nop,nop);

			LastWorld->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
			ITEM *LightList=LastWorld->ItemList->AddItem("LIGHTS",NULL,Scroller_SelectItem,SubOpenClose,nop);
			LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
			LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
			LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
			LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
			LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
			LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
			LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
			LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
			
			LastWorld->ItemList->AddItem("--------SCENES-------",NULL,Scroller_SelectItem,nop,nop);

			char *NewLine=new char[strlen(CommandLine)+51];
			memset(NewLine,0,strlen(CommandLine)+51);
			_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s",LastWorld->ID,CommandLine);
			WorldListScroller->AddItem(NewLine,LastWorld,Scroller_SelectOnlyThisWorld,GUI_RenameWorld,nop);
			WorldScroller->ParentItem->ItemNum++;
			memset(&LayouterDisplay4->ParentItem->Cam,0,sizeof(CAMERA));
			LayouterDisplay4->ParentItem->Cam.Fov=45;
			LayouterDisplay4->ParentItem->Cam.Up.y=1;
			LayouterDisplay4->ParentItem->Cam.Eye.z=10;
			LayouterDisplay4->ParentItem->Cam.Eye.y=7;
			ActualWorld->Camera=LayouterDisplay4->ParentItem->Cam;
			SetEnvironmentToActualWorld();
			AddSWorldInterface->ParentItem->ItemNum++;
		}

	}
}

void __stdcall Modeller_NewAnimationCallback(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		{
			// ADD NEW Anim
			ActualWorld->AddAnimation(CommandLine);
			ActualWorld->AnimList->AddItem(CommandLine,&ActualWorld->Animations[ActualWorld->AnimNum-1],GUI_SelectOnlyThisAnim,GUI_Rename,nop);
			ActualWorld->Animations[ActualWorld->AnimNum-1].ListItem=ActualWorld->AnimList->Last;
			SetActualWorldAnimListToMainAnimList();
		}

	}
}

void __stdcall Modeller_NewCameraCallback(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		{
			// ADD NEW Anim
			ActualWorld->AddCamera(CommandLine);
			ActualWorld->CameraList->AddItem(CommandLine,&ActualWorld->CamAnims[ActualWorld->CamNum-1],GUI_SelectOnlyThisCam,GUI_Rename,nop);
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Eyex=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Eyey=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Eyez=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Trgx=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Trgy=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Trgz=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Fov=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Roll=new SPLINE();
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Eyex->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Eyey->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Eyez->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Trgx->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Trgy->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Trgz->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Fov->Interpolation=aDDict_CATMULL;
			ActualWorld->CamAnims[ActualWorld->CamNum-1].Roll->Interpolation=aDDict_CATMULL;
			SetActualWorldCamListToMainCamList();
		}

	}
}


void __stdcall GUI_ReadLine_Anim(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		GUI_ReadLine(Interface,ButtonId);
	}
	else
		ErrorMessage="CREATE A WORLD FIRST!";
}

void __stdcall GUI_ButtonSelect_ObjSplines(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSelect(Interface,ButtonId);
	SetObjectSplines();
	SetEmitterSplines();
	SetSubWorldSplines();
}

void __stdcall GUI_ButtonSelect_ParticleSplines(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSelect(Interface,ButtonId);
	SetObjectSplines();
	SetEmitterSplines();
	SetSubWorldSplines();
}


void __stdcall GUI_ClearWorld(cINTERFACE* Interface, int ButtonId)
{
}

void __stdcall GUI_SetLTexture(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	if (ActualMaterial)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (!ActualWorld->ObjectList[x].Deleted)
	{
		if (ActualWorld->ObjectList[x].ListItem->Selected)
		{
			ActualWorld->ObjectList[x].Textured=true;
			ActualWorld->ObjectList[x].Material=ActualMaterial;
		}
	}
}

void __stdcall GUI_ClearLTexture(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (!ActualWorld->ObjectList[x].Deleted)
	{
		if (ActualWorld->ObjectList[x].ListItem->Selected)
		{
			ActualWorld->ObjectList[x].Textured=false;
			ActualWorld->ObjectList[x].Material=NULL;
		}
	}
}

void __stdcall GUI_SetObjLDSTBlend(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && Interface->ItemList[ScrollerID].LeftClickInWindow)
		{
			switch (ScrollerID)
			{
			case 0: ActualWorld->ObjectList[x].DSTBlend=GL_ZERO; break;
			case 1: ActualWorld->ObjectList[x].DSTBlend=GL_ONE; break;
			case 2: ActualWorld->ObjectList[x].DSTBlend=GL_DST_COLOR; break;
			case 3: ActualWorld->ObjectList[x].DSTBlend=GL_ONE_MINUS_DST_COLOR; break;
			case 4: ActualWorld->ObjectList[x].DSTBlend=GL_DST_ALPHA; break;
			case 5: ActualWorld->ObjectList[x].DSTBlend=GL_ONE_MINUS_DST_ALPHA; break;
			case 6: ActualWorld->ObjectList[x].DSTBlend=GL_SRC_ALPHA; break;
			case 7: ActualWorld->ObjectList[x].DSTBlend=GL_ONE_MINUS_SRC_ALPHA; break;
			case 8: ActualWorld->ObjectList[x].DSTBlend=GL_SRC_ALPHA_SATURATE; break;
			}
		}
}

void __stdcall GUI_SetObjLDSTBlendCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected)
			{

				switch (ScrollerID)
				{
					case 0: if(ActualWorld->ObjectList[x].DSTBlend==GL_ZERO) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 1: if(ActualWorld->ObjectList[x].DSTBlend==GL_ONE) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 2: if(ActualWorld->ObjectList[x].DSTBlend==GL_DST_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 3: if(ActualWorld->ObjectList[x].DSTBlend==GL_ONE_MINUS_DST_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 4: if(ActualWorld->ObjectList[x].DSTBlend==GL_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 5: if(ActualWorld->ObjectList[x].DSTBlend==GL_ONE_MINUS_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 6: if(ActualWorld->ObjectList[x].DSTBlend==GL_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 7: if(ActualWorld->ObjectList[x].DSTBlend==GL_ONE_MINUS_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 8: if(ActualWorld->ObjectList[x].DSTBlend==GL_SRC_ALPHA_SATURATE) GUI_ButtonSelect(Interface,ScrollerID); break;
				}

				break;
			}
	}
}

void __stdcall GUI_SetObjLSRCBlend(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
		if (ActualWorld->ObjectList[x].ListItem->Selected && Interface->ItemList[ScrollerID].LeftClickInWindow)
		{
			switch (ScrollerID)
			{
			case 0: ActualWorld->ObjectList[x].SRCBlend=GL_ZERO; break;
			case 1: ActualWorld->ObjectList[x].SRCBlend=GL_ONE; break;
			case 2: ActualWorld->ObjectList[x].SRCBlend=GL_SRC_COLOR; break;
			case 3: ActualWorld->ObjectList[x].SRCBlend=GL_ONE_MINUS_SRC_COLOR; break;
			case 4: ActualWorld->ObjectList[x].SRCBlend=GL_DST_ALPHA; break;
			case 5: ActualWorld->ObjectList[x].SRCBlend=GL_ONE_MINUS_DST_ALPHA; break;
			case 6: ActualWorld->ObjectList[x].SRCBlend=GL_SRC_ALPHA; break;
			case 7: ActualWorld->ObjectList[x].SRCBlend=GL_ONE_MINUS_SRC_ALPHA; break;
			}
		}
}

void __stdcall GUI_SetObjLSRCBlendCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected)
			{

				switch (ScrollerID)
				{
				case 0: if (ActualWorld->ObjectList[x].SRCBlend==GL_ZERO) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 1: if (ActualWorld->ObjectList[x].SRCBlend==GL_ONE) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 2: if (ActualWorld->ObjectList[x].SRCBlend==GL_SRC_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 3: if (ActualWorld->ObjectList[x].SRCBlend==GL_ONE_MINUS_SRC_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 4: if (ActualWorld->ObjectList[x].SRCBlend==GL_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 5: if (ActualWorld->ObjectList[x].SRCBlend==GL_ONE_MINUS_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 6: if (ActualWorld->ObjectList[x].SRCBlend==GL_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 7: if (ActualWorld->ObjectList[x].SRCBlend==GL_ONE_MINUS_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				}

				break;
			}
	}
}

void __stdcall GUI_SelectMat(cINTERFACE* Interface, int ButtonId)
{
	ITEM *I=ScrollerFindItem(MaterialList,VarMatMenu->ItemList[MaterialListItemNum].SelectedItem);
	if (I) 
	{
		ActualMaterial=(MATERIAL*)(I->Data);
		MaterialHandle=ActualMaterial->ImageData.TextureHandle;
	}
}

void __stdcall Layouter_DeleteSelected(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
		{
			if (ActualWorld->ObjectList[x].ListItem->Selected) 
			{
				ActualWorld->ObjectList[x].Deleted=true;
				if (ActualWorld->ObjectList[x].ListItem->Prev)
				{
					ActualWorld->ObjectList[x].ListItem->Prev->Next=ActualWorld->ObjectList[x].ListItem->Next;
					if (ActualWorld->ObjectList[x].ListItem->Next)
					ActualWorld->ObjectList[x].ListItem->Prev->Next->Prev=ActualWorld->ObjectList[x].ListItem->Prev;
					else
					ActualWorld->ItemList->Last=ActualWorld->ObjectList[x].ListItem->Prev;
					ActualWorld->ItemList->ItemNum--;				
				}
				ActualWorld->ObjectList[x].ListItem->Selected=false;
			}
		}
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
	}
}

void __stdcall GUI_ButtonSwitch_Fog(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		GUI_ButtonSwitch(Interface,ButtonId);
		ActualWorld->Fog=Interface->ItemList[ButtonId].ButtonPushed;
	}
}

void __stdcall GUI_ButtonSwitch_SetFog(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld && ActualWorld->Fog)
	{
		ActualWorld->FogCol[0]=FogRed;
		ActualWorld->FogCol[1]=FogGreen;
		ActualWorld->FogCol[2]=FogBlue;
		//ActualWorld->FogCol[3]=FogIntensity;
		ActualWorld->FogStart=FogIntensity1;
		ActualWorld->FogEnd=FogIntensity2;
	}
}

void __stdcall GUI_StorePos(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			ActualWorld->ObjectList[x].StoredPos=ActualWorld->ObjectList[x].PosData;
		ErrorMessage="POSITION STORED\0";
	}
}

void __stdcall GUI_LoadPos(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			ActualWorld->ObjectList[x].PosData=ActualWorld->ObjectList[x].StoredPos;
	}
}

void __stdcall GUI_LSetObjRed(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
			{
				ActualWorld->ObjectList[x].Color.r=LObjRed;
			}
	}
}

void __stdcall GUI_LSetObjRedAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
		{
				LObjRed=ActualWorld->ObjectList[x].Color.r;
			break;
		}
}

void __stdcall GUI_LSetObjAEps(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
			{
				ActualWorld->ObjectList[x].AEpsilon=AEpsilon;
			}
	}
}

void __stdcall GUI_LSetObjAEpsAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
		{
				AEpsilon=ActualWorld->ObjectList[x].AEpsilon;
			break;
		}
}

void __stdcall GUI_LSetObjGreen(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
			{
				ActualWorld->ObjectList[x].Color.g=LObjGreen;
			}
	}
}

void __stdcall GUI_LSetObjGreenAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
		{
				LObjGreen=ActualWorld->ObjectList[x].Color.g;
			break;
		}
}

void __stdcall GUI_LSetObjBlue(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
			{
				ActualWorld->ObjectList[x].Color.b=LObjBlue;
			}
	}
}

void __stdcall GUI_LSetObjBlueAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
		{
				LObjBlue=ActualWorld->ObjectList[x].Color.b;
			break;
		}
}

void __stdcall GUI_LSetObjAlpha(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualWorld)
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
			{
				ActualWorld->ObjectList[x].Color.a=LObjAlpha;
			}
	}
}

void __stdcall GUI_LSetObjAlphaAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].Primitive<=aDDict_CIRCLE)
		{
				LObjAlpha=ActualWorld->ObjectList[x].Color.a;
			break;
		}
}

void __stdcall Layouter_InvertSelect(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted)
		ActualWorld->ObjectList[x].ListItem->Selected=!ActualWorld->ObjectList[x].ListItem->Selected;
	WaitLeftButton=true;
}


void __stdcall Layouter_DeSelectAll(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted)
		ActualWorld->ObjectList[x].ListItem->Selected=false;
}

void __stdcall Layouter_SelectAll(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted)
		ActualWorld->ObjectList[x].ListItem->Selected=true;
}

void __stdcall GUI_LSetObjDepthMask(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected)
			{
				ActualWorld->ObjectList[x].ZMask=ConstMatMenu->ItemList[2].ButtonPushed;
			}
	}
}

void __stdcall GUI_LSetObjDepthMaskCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected)
			{
				ConstMatMenu->ItemList[2].ButtonPushed=ActualWorld->ObjectList[x].ZMask;
				break;
			}
	}
}

void __stdcall CheckSelectedHead(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].EmitterData && 
				(ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || 
				 ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
				 ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualWorld->ObjectList[x].EmitterData->Head;
				break;
			}
	}
}

void __stdcall CheckSelectedTail(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].EmitterData && 
				(ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || 
				 ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
				 ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualWorld->ObjectList[x].EmitterData->Tail;
				break;
			}
	}
}

void __stdcall CheckSelectedRandRotate(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].EmitterData && 
				(ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || 
				 ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
				 ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualWorld->ObjectList[x].EmitterData->RandRotate;
				break;
			}
	}
}

void __stdcall CheckSelectedFixUp(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++)
			if (ActualWorld->ObjectList[x].ListItem->Selected && ActualWorld->ObjectList[x].EmitterData && 
				(ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || 
				 ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
				 ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER))
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualWorld->ObjectList[x].EmitterData->FixedUp;
				break;
			}
	}
}


void __stdcall GUI_LayouterCopy(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualWorld)
	{
		int i=ActualWorld->ObjectNum;
		for (int x=0; x<i; x++)
		{
			if (!ActualWorld->ObjectList[x].Deleted && ActualWorld->ObjectList[x].ListItem->Selected)
			{
				ActualWorld->ObjectList[x].ListItem->Selected=false;
				ActualWorld->AddObject();
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected=true;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name=new char[50];
				sprintf(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name,"%s\0",ActualWorld->ObjectList[x].ListItem->Name);

				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ParentID=ActualWorld->ObjectList[x].ParentID;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Data=ActualWorld->ObjectList[x].Data;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Primitive=ActualWorld->ObjectList[x].Primitive;
				memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ModelView,ActualWorld->ObjectList[x].ModelView,sizeof(MATRIX));
				memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].TransformBuffer,ActualWorld->ObjectList[x].TransformBuffer,sizeof(MATRIX));
				memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Inverted,ActualWorld->ObjectList[x].Inverted,sizeof(MATRIX));
				memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].IT,ActualWorld->ObjectList[x].IT,sizeof(MATRIX));
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].SRCBlend=ActualWorld->ObjectList[x].SRCBlend;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].DSTBlend=ActualWorld->ObjectList[x].DSTBlend;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Textured=ActualWorld->ObjectList[x].Textured;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ZMask=ActualWorld->ObjectList[x].ZMask;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material=ActualWorld->ObjectList[x].Material;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].PosData=ActualWorld->ObjectList[x].PosData;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].PosDataBuffer=ActualWorld->ObjectList[x].PosDataBuffer;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Color=ActualWorld->ObjectList[x].Color;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].TargetWorldID=ActualWorld->ObjectList[x].TargetWorldID;
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].AEpsilon=ActualWorld->ObjectList[x].AEpsilon;

				if (ActualWorld->ObjectList[x].EmitterData)
				{
					ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData=new EMITTERDATA;
					memcpy(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData,ActualWorld->ObjectList[x].EmitterData,sizeof(EMITTERDATA));
					ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles=new PARTICLE[ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles];
					memset(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles,0,ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles*sizeof(PARTICLE));
					ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticleNumBuffer=ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticlesPerFrame;
					if (ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->TailInfo)
					for (int y=0; y<ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles; y++)
					{
						ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles[y].Tail=new VECTOR3[256];
					}
				}
				MainLayouterScroller->ParentItem->ItemNum++;
				if (ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || 
					ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER ||
					ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER)
				{
							ActualWorld->ParticleCount++;
							for (int z=0; z<ActualWorld->AnimNum; z++)
							{
								for (int i=0; i<ActualWorld->ObjectList[x].Animations[z].Red->KeyNum; i++)
									ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Animations[z].Red->AddKey(ActualWorld->ObjectList[x].Animations[z].Red->Keys[i].Value,ActualWorld->ObjectList[x].Animations[z].Red->Keys[i].Time);
								for (i=0; i<ActualWorld->ObjectList[x].Animations[z].Green->KeyNum; i++)
									ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Animations[z].Green->AddKey(ActualWorld->ObjectList[x].Animations[z].Green->Keys[i].Value,ActualWorld->ObjectList[x].Animations[z].Green->Keys[i].Time);
								for (i=0; i<ActualWorld->ObjectList[x].Animations[z].Blue->KeyNum; i++)
									ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Animations[z].Blue->AddKey(ActualWorld->ObjectList[x].Animations[z].Blue->Keys[i].Value,ActualWorld->ObjectList[x].Animations[z].Blue->Keys[i].Time);
								for (i=0; i<ActualWorld->ObjectList[x].Animations[z].Alpha->KeyNum; i++)
									ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Animations[z].Alpha->AddKey(ActualWorld->ObjectList[x].Animations[z].Alpha->Keys[i].Value,ActualWorld->ObjectList[x].Animations[z].Alpha->Keys[i].Time);
								for (i=0; i<ActualWorld->ObjectList[x].Animations[z].Size->KeyNum; i++)
									ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Animations[z].Size->AddKey(ActualWorld->ObjectList[x].Animations[z].Size->Keys[i].Value,ActualWorld->ObjectList[x].Animations[z].Size->Keys[i].Time);
							}
				}
				else 
					if (ActualWorld->ObjectList[x].Primitive==aDDict_SUBSCENE) ActualWorld->SubWorldCount++;
					else ActualWorld->ObjectCount++;

			}
		}
		SetObjectSplines();
		SetEmitterSplines();
		SetSubWorldSplines();
		WaitLeftButton=true;
	}
}

void __stdcall SelectSceneSplineForm(cINTERFACE* Interface, int ScrollerID)
{
	int I=aDDict_NONE;
	switch (ScrollerID)
	{
		case 0: I=aDDict_NONE; break;
		case 1: I=aDDict_LINEAR; break;
		case 2: I=aDDict_CATMULL; break;
		case 3: I=aDDict_SINE; break;
		case 4: I=aDDict_SAW; break;
		case 5: I=aDDict_SQUARE; break;
		case 6: I=aDDict_BEZIER; break;
	}


	WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem)->Data;
	DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
	if (SceneSplines->ItemList[14].ButtonPushed) s->Posx->ChangeInterpolation(I);
	if (SceneSplines->ItemList[15].ButtonPushed) s->Posy->ChangeInterpolation(I);
	if (SceneSplines->ItemList[16].ButtonPushed) s->Posz->ChangeInterpolation(I);

	if (SceneSplines->ItemList[20].ButtonPushed) s->Sclx->ChangeInterpolation(I);
	if (SceneSplines->ItemList[21].ButtonPushed) s->Scly->ChangeInterpolation(I);
	if (SceneSplines->ItemList[22].ButtonPushed) s->Sclz->ChangeInterpolation(I);

	if (SceneSplines->ItemList[23].ButtonPushed) s->Red->ChangeInterpolation(I);
	if (SceneSplines->ItemList[24].ButtonPushed) s->Green->ChangeInterpolation(I);
	if (SceneSplines->ItemList[25].ButtonPushed) s->Blue->ChangeInterpolation(I);
	if (SceneSplines->ItemList[26].ButtonPushed) s->Alpha->ChangeInterpolation(I);
}

void __stdcall SelectCameraSplineForm(cINTERFACE* Interface, int ScrollerID)
{
	int I=aDDict_NONE;
	switch (ScrollerID)
	{
		case 0: I=aDDict_NONE; break;
		case 1: I=aDDict_LINEAR; break;
		case 2: I=aDDict_CATMULL; break;
		case 3: I=aDDict_SINE; break;
		case 4: I=aDDict_SAW; break;
		case 5: I=aDDict_SQUARE; break;
		case 6: I=aDDict_BEZIER; break;
	}


	CAMSPLINES *s=(CAMSPLINES*)ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)->Data;
	if (CameraSplines->ItemList[11].ButtonPushed) s->Eyex->ChangeInterpolation(I);
	if (CameraSplines->ItemList[12].ButtonPushed) s->Eyey->ChangeInterpolation(I);
	if (CameraSplines->ItemList[13].ButtonPushed) s->Eyez->ChangeInterpolation(I);
	if (CameraSplines->ItemList[14].ButtonPushed) s->Trgx->ChangeInterpolation(I);
	if (CameraSplines->ItemList[15].ButtonPushed) s->Trgy->ChangeInterpolation(I);
	if (CameraSplines->ItemList[16].ButtonPushed) s->Trgz->ChangeInterpolation(I);
	if (CameraSplines->ItemList[17].ButtonPushed) s->Fov->ChangeInterpolation(I);
	if (CameraSplines->ItemList[18].ButtonPushed) s->Roll->ChangeInterpolation(I);
}

void __stdcall SelectParticleSplineForm(cINTERFACE* Interface, int ScrollerID)
{
	int I=aDDict_NONE;
	switch (ScrollerID)
	{
		case 0: I=aDDict_NONE; break;
		case 1: I=aDDict_LINEAR; break;
		case 2: I=aDDict_CATMULL; break;
		case 3: I=aDDict_SINE; break;
		case 4: I=aDDict_SAW; break;
		case 5: I=aDDict_SQUARE; break;
		case 6: I=aDDict_BEZIER; break;
	}


	WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem)->Data;
	DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
	if (ParticleSplines->ItemList[16].ButtonPushed) s->Posx->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[17].ButtonPushed) s->Posy->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[18].ButtonPushed) s->Posz->ChangeInterpolation(I);

	if (ParticleSplines->ItemList[22].ButtonPushed) s->Sclx->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[23].ButtonPushed) s->Scly->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[24].ButtonPushed) s->Sclz->ChangeInterpolation(I);

	if (ParticleSplines->ItemList[25].ButtonPushed) s->Red->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[26].ButtonPushed) s->Green->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[27].ButtonPushed) s->Blue->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[28].ButtonPushed) s->Alpha->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[29].ButtonPushed) s->Size->ChangeInterpolation(I);
	if (ParticleSplines->ItemList[30].ButtonPushed) s->Prtfrme->ChangeInterpolation(I);
}

void __stdcall SelectSubWorldSplineForm(cINTERFACE* Interface, int ScrollerID)
{
	int I=aDDict_NONE;
	switch (ScrollerID)
	{
		case 0: I=aDDict_NONE; break;
		case 1: I=aDDict_LINEAR; break;
		case 2: I=aDDict_CATMULL; break;
		case 3: I=aDDict_SINE; break;
		case 4: I=aDDict_SAW; break;
		case 5: I=aDDict_SQUARE; break;
		case 6: I=aDDict_BEZIER; break;
	}


	WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem)->Data;
	DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
	if (SWorldSplines->ItemList[12].ButtonPushed) s->Posx->ChangeInterpolation(I);
	if (SWorldSplines->ItemList[13].ButtonPushed) s->Posy->ChangeInterpolation(I);
	if (SWorldSplines->ItemList[14].ButtonPushed) s->Posz->ChangeInterpolation(I);

	if (SWorldSplines->ItemList[18].ButtonPushed) s->Sclx->ChangeInterpolation(I);
	if (SWorldSplines->ItemList[19].ButtonPushed) s->Scly->ChangeInterpolation(I);
	if (SWorldSplines->ItemList[20].ButtonPushed) s->Sclz->ChangeInterpolation(I);

	if (SWorldSplines->ItemList[22].ButtonPushed) s->AnimTime->ChangeInterpolation(I);
}


void __stdcall GUI_ButtonSelect_DontChangeSubinterface(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<Interface->ItemNum; x++) if (Interface->ItemList[x].ItemId!=GUI_SUBINTERFACE) Interface->ItemList[x].ButtonPushed=false;
	Interface->ItemList[ButtonId].ButtonPushed=true;
	Interface->SetActiveBranchNoSubInterface(ButtonId);
	if (Interface->ItemList[ButtonId].ToolTip) 
	{
		ErrorMessage="";
		LastClickedToolTip=Interface->ItemList[ButtonId].ToolTip;
	}
}

void BuildLayouterGUI(cINTERFACE *Layouter)
{
	LayouterDisplays=Layouter->AddSubInterface();
	LayouterDisplays->ParentItem->OnWheel=LCheckTransform;

	LayouterDisplay1=LayouterDisplays->AddSubInterface()->AddL3dView(174,40,574,340,"TOP VIEW (zoom shortcut: j/m)",true,nop,nop,nop,nop);
	LayouterDisplay1->ParentItem->Cam.Fov=45;
	LayouterDisplay1->ParentItem->Cam.Up.z=-1;
	LayouterDisplay1->ParentItem->Cam.Eye.y=10;
	LayouterDisplay1->ParentItem->i.x=1;
	LayouterDisplay1->ParentItem->j.z=-1;
	LayouterDisplay1->ParentItem->k=1;
	LayouterDisplay1->ParentItem->i1=0;
	LayouterDisplay1->ParentItem->j1=2;
	LayouterDisplay1->ParentItem->LayouterDisplay=true;
	LayouterDisplay2=LayouterDisplays->AddSubInterface()->AddL3dView(574,40,974,340,"FRONT VIEW (zoom shortcut: j/m)",true,nop,nop,nop,nop);
	LayouterDisplay2->ParentItem->Cam.Fov=45;
	LayouterDisplay2->ParentItem->Cam.Up.y=1;
	LayouterDisplay2->ParentItem->Cam.Eye.z=10;
	LayouterDisplay2->ParentItem->i.x=1;
	LayouterDisplay2->ParentItem->j.y=1;
	LayouterDisplay2->ParentItem->k=2;
	LayouterDisplay2->ParentItem->i1=0;
	LayouterDisplay2->ParentItem->j1=1;
	LayouterDisplay2->ParentItem->LayouterDisplay=true;
	LayouterDisplay3=LayouterDisplays->AddSubInterface()->AddL3dView(174,340,574,640,"LEFT VIEW (zoom shortcut: j/m)",true,nop,nop,nop,nop);
	LayouterDisplay3->ParentItem->Cam.Fov=45;
	LayouterDisplay3->ParentItem->Cam.Up.y=1;
	LayouterDisplay3->ParentItem->Cam.Eye.x=-10;
	LayouterDisplay3->ParentItem->i.z=1;
	LayouterDisplay3->ParentItem->j.y=1;
	LayouterDisplay3->ParentItem->k=0;
	LayouterDisplay3->ParentItem->i1=2;
	LayouterDisplay3->ParentItem->j1=1;
	LayouterDisplay3->ParentItem->LayouterDisplay=true;
	LayouterDisplay4=LayouterDisplays->AddSubInterface()->AddL3dView(574,340,974,640,"CAMERA VIEW",false,nop,nop,nop,nop);
	LayouterDisplay4->ParentItem->CamGrid=glGenLists(1);
	LayouterDisplay4->ParentItem->CamGrid=glGenLists(1);
	glNewList(LayouterDisplay4->ParentItem->CamGrid,GL_COMPILE);
	glBegin(GL_LINES);
	VECTOR3 v1,v2;
	memset(&v1,0,sizeof(v1));
	for (int z=0; z<3; z++)
	for (int x=-80; x<=80; x++)
	{
		int y = abs(x+1) % 2;
		if ((z==1 && y==1 && x!=0) || (z==2 && y==0 && x!=0) || (z==0 && x==0))
		{
			v2.x=(float)(0.1*(y+1.0));
			v2.y=(float)(0.15*(y+1.0));
			v2.z=(float)(0.25*(y+1.0));
			v1.a[0]=(float)((float)x/2.0);
			if (x==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.x,v2.y,v2.z,1);
			v1.a[2]=-40;
			glVertex3f(v1.x,v1.y,v1.z);
			v1.a[2]=40;
			glVertex3f(v1.x,v1.y,v1.z);		
			glVertex3f(v1.z,v1.y,v1.x);
			v1.a[2]=-40;
			glVertex3f(v1.z,v1.y,v1.x);
		}
	}
	glEnd();
	glEndList();
	LayouterDisplay4->ParentItem->Cam.Fov=45;
	LayouterDisplay4->ParentItem->Cam.Up.y=1;
	LayouterDisplay4->ParentItem->Cam.Eye.z=10;
	LayouterDisplay4->ParentItem->Cam.Eye.y=7;
	LayouterDisplay4->ParentItem->LayouterDisplay=true;

	WorldListScroller=new ITEMLIST();
	MainLayouterScroller=Layouter->AddItemList(35,50,116,17,NULL,"");
	MainLayouterScrollerData=&(Layouter->ItemList[Layouter->ItemNum-4].data);
	Layouter->AddSkinnedButton(862-850,49,AllButton,"SELECT ALL OBJECTS ( a )",Layouter_SelectAll,nop,nop,nop,'a');
	Layouter->AddSkinnedButton(862-850,68,NoButton,"DESELECT ALL OBJECTS ( n )",Layouter_DeSelectAll,nop,nop,nop,'n');
	Layouter->AddSkinnedButton(862-850,87,InvertButton,"INVERT SELECTION ( i )",Layouter_InvertSelect,nop,nop,nop,'i');
	Layouter->AddSkinnedButton(862-850,163,Del__Button,"DELETE SELECTED OBJECTS ( DEL )",Layouter_DeleteSelected,nop,nop,nop,VK_DELETE);

	cINTERFACE *LayouterMainMenu=Layouter->AddSubInterface();
	cINTERFACE *WorldsMenu         = LayouterMainMenu->AddSkinnedButton( 36-9,241,ScenesButton,"WORLD SETTINGS MENU",GUI_SelectNonSpline,nop,nop,nop);
	           LayouterObjectsMenu= LayouterMainMenu->AddSkinnedButton( 55-9,241,ObjectsButton,"OBJECTS MENU",GUI_SelectNonSpline,nop,nop,nop);
	cINTERFACE *LightsMenu=          LayouterMainMenu->AddSkinnedButton( 74-9,241,LightsButton,"LIGHTS MENU",GUI_ButtonSelect_LLightMenuMain,nop,nop,nop);
	               MatMenu=         LayouterMainMenu->AddSkinnedButton( 84  ,241,MaterialsButton,"MATERIALS MENU",GUI_SelectNonSpline,nop,nop,nop);
	cINTERFACE *ParticlesMenu=       LayouterMainMenu->AddSkinnedButton( 93+10,241,ParticlesButton,"PARTICLE MENU",GUI_SelectNonSpline,nop,nop,nop);
	           LSplineMenu=         LayouterMainMenu->AddSkinnedButton(112+10,241,SplinesButton,"SPLINE EDITOR",GUI_SelectSpline,nop,nop,nop);
	cINTERFACE *LFileMenu=           LayouterMainMenu->AddSkinnedButton(131+10,241,FileButton,"FILE MENU",GUI_SelectNonSpline,nop,nop,nop);

	LayouterMainMenu->ItemList[6].ButtonPushed=true;
	LayouterMainMenu->SetActiveBranch(6);

	cINTERFACE *CamerasMenu=WorldsMenu->AddSkinnedButton(895-850,275,CamerasButton,"CAMERA SETUP MENU",GUI_ButtonSelect,nop,nop,nop)->AddSubInterface();
	cINTERFACE *EnvironmentMenu=WorldsMenu->AddSkinnedButton(895-850,291,EnvironmentButton,"SCENE ENVIRONMENT SETUP MENU",GUI_ButtonSelect,nop,nop,nop)->AddSubInterface();

	CamerasMenu->AddFloatBar(862-850,340,136,GUI_NONE,&(LayouterDisplay4->ParentItem->Cam.Fov),0.0f,180.0f,1.0f,45.0f,"CAM FOV: ","SET CAMERA FOV");
	CamerasMenu->AddFloatBar(862-850,360,136,GUI_NONE,&(LayouterDisplay4->ParentItem->Cam.Roll),-255.0f,255.0f,1.0f,0.0f,"CAM ROLL: ","SET CAMERA ROLL");

	WorldsMenu->SetActiveBranch(1);
	WorldsMenu->ItemList[1].ButtonPushed=true;

	FogButtonInterface=EnvironmentMenu->AddSkinnedButton(895-850,319,FogButton,"TURN FOG ON/OFF",GUI_ButtonSwitch_Fog,nop,nop,GUI_ButtonSwitch_SetFog);

	EnvironmentMenu->AddByteBar(862-850,340,136,GUI_RED,&FogRed,0,255,0,"FOG RED: ","SET FOG RED INTENSITY");
	EnvironmentMenu->AddByteBar(862-850,359,136,GUI_GREEN,&FogGreen,0,255,0,"FOG GREEN: ","SET FOG GREEN INTENSITY");
	EnvironmentMenu->AddByteBar(862-850,378,136,GUI_BLUE,&FogBlue,0,255,0,"FOG BLUE: ","SET FOG BLUE INTENSITY");

	EnvironmentMenu->AddFloatBar(862-850,400,136,GUI_NONE,&FogIntensity1,0,500,1,0,"START: ","SET FOG START DISTANCE");
	EnvironmentMenu->AddFloatBar(862-850,419,136,GUI_NONE,&FogIntensity2,0,500,1,200,"END: ","SET FOG END DISTANCE");

	LayouterObjectsMenu->AddSkinnedButton(49,280,AddDummyButton,"DUMMY OBJECT",CreateDummy,nop,nop,nop);
	AddModellInterface=LayouterObjectsMenu->AddItemList(16,310,135,10,SceneListScroller,"SELECT MODELL");
	LayouterObjectsMenu->AddSkinnedButton(49,430,AddModellButton,"ADD SELECTED MODELL TO THE WORLD",CreateModell,nop,nop,nop);

	AddSWorldInterface=LayouterObjectsMenu->AddItemList(16,454,135,10,WorldListScroller,"SELECT WORLD");
	LayouterObjectsMenu->AddSkinnedButton(49,574,AddSubWorldButton,"ADD SELECTED WORLD TO THE WORLD",CreateSubWorld,nop,nop,nop);

	LLightMenus[0]=LightsMenu->AddSkinnedButton(870-850,295,Light1Button,"LIGHT 1 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[1]=LightsMenu->AddSkinnedButton(887-850,295,Light2Button,"LIGHT 2 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[2]=LightsMenu->AddSkinnedButton(904-850,295,Light3Button,"LIGHT 3 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[3]=LightsMenu->AddSkinnedButton(921-850,295,Light4Button,"LIGHT 4 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[4]=LightsMenu->AddSkinnedButton(938-850,295,Light5Button,"LIGHT 5 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[5]=LightsMenu->AddSkinnedButton(955-850,295,Light6Button,"LIGHT 6 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[6]=LightsMenu->AddSkinnedButton(972-850,295,Light7Button,"LIGHT 7 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);
	LLightMenus[7]=LightsMenu->AddSkinnedButton(989-850,295,Light8Button,"LIGHT 8 PROPERTIES",GUI_ButtonSelect_LLightMenu,nop,nop,nop);

	LightingButtonInterface=LightsMenu->AddSkinnedButton(63,275,LightingButton,"TURN LIGHTING ON/OFF",SwitchLight,nop,nop,SwitchLightCheck);

	LBuildLightGUI(LLightMenus[0],1);
	LBuildLightGUI(LLightMenus[1],2);
	LBuildLightGUI(LLightMenus[2],3);
	LBuildLightGUI(LLightMenus[3],4);
	LBuildLightGUI(LLightMenus[4],5);
	LBuildLightGUI(LLightMenus[5],6);
	LBuildLightGUI(LLightMenus[6],7);
	LBuildLightGUI(LLightMenus[7],8);

	LightsMenu->ItemList[0].ButtonPushed=true;
	LightsMenu->SetActiveBranch(0);

	VarMatMenu=MatMenu->AddSkinnedButton(853-850,275,VariablesButton,"VARIABLE MATERIAL SETTINGS",GUI_ButtonSelect,nop,nop,nop);
	MatMenu->ItemList[MatMenu->ItemNum-1].ButtonPushed=true;
	MatMenu->SetActiveBranch(MatMenu->ItemNum-1);
	ConstMatMenu=MatMenu->AddSkinnedButton(938-850,275,ConstantsButton,"CONSTANT MATERIAL SETTINGS (BLENDING)",GUI_ButtonSelect,nop,nop,nop);

	VarMatMenu->AddTextureDisplay(874-850,272+23,1002-850,400+23,&MaterialHandle,nop,nop,nop,GUI_SelectMat);
	VarMatMenu->AddItemList(865-850,403+23,137,6,MaterialList,"MATERIAL SELECTOR");

	VarMatMenu->AddIntBar(12,475+23,136,GUI_RED,&LObjRed,0,255,255,"OBJECT RED: ","SET OBJECT RED COLOR",GUI_LSetObjRed,GUI_LSetObjRedAfter);
	VarMatMenu->AddIntBar(12,494+24,136,GUI_GREEN,&LObjGreen,0,255,255,"OBJECT GREEN: ","SET OBJECT GREEN COLOR",GUI_LSetObjGreen,GUI_LSetObjGreenAfter);
	VarMatMenu->AddIntBar(12,512+25,136,GUI_BLUE,&LObjBlue,0,255,255,"OBJECT BLUE: ","SET OBJECT BLUE COLOR",GUI_LSetObjBlue,GUI_LSetObjBlueAfter);
	VarMatMenu->AddIntBar(12,531+26,136,GUI_NONE,&LObjAlpha,0,255,255,"OBJECT ALPHA: ","SET OBJECT ALPHA COLOR",GUI_LSetObjAlpha,GUI_LSetObjAlphaAfter);

	VarMatMenu->AddSkinnedButton(870-850,551+27,SetButton,"SET TEXTURE TO SELECTED OBJECTS",GUI_SetLTexture,nop,nop,nop);
	VarMatMenu->AddSkinnedButton(915-850,551+27,NoneButton,"SET NO TEXTURE TO SELECTED OBJECTS",GUI_ClearLTexture,nop,nop,nop);
	//MatMenu->AddSkinnedButton(960,551+27,DelButton,"DELETE SELECTED MATERIAL",nop,nop,nop,nop);

	cINTERFACE *SrcBlend=ConstMatMenu->AddText(877-850,303,"SOURCE BLEND MODE");
	ConstMatMenu->ItemList[0].ActiveBranch=true;
	SrcBlend->AddRadioButton(853-850,310,"ZERO","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,325,"ONE","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->ItemList[SrcBlend->ItemNum-1].ButtonPushed=true;
	SrcBlend->AddRadioButton(853-850,340,"BACKGROUND COLOR","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,355,"1-BACKGROUND COLOR","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,370,"BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,385,"1-BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,400,"SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,415,"1-SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);
	SrcBlend->AddRadioButton(853-850,430,"SOURCE ALPHA SATURATE","TEST RADIOBUTTON",GUI_SetObjLDSTBlend,nop,nop,GUI_SetObjLDSTBlendCheck);

	cINTERFACE *DstBlend=ConstMatMenu->AddText(861-850,463,"BACKGROUND BLEND MODE");
	ConstMatMenu->ItemList[1].ActiveBranch=true;
	DstBlend->AddRadioButton(853-850,470,"ZERO","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->ItemList[DstBlend->ItemNum-1].ButtonPushed=true;
	DstBlend->AddRadioButton(853-850,485,"ONE","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->AddRadioButton(853-850,500,"SOURCE COLOR","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->AddRadioButton(853-850,515,"1-SOURCE COLOR","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->AddRadioButton(853-850,530,"BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->AddRadioButton(853-850,545,"1-BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->AddRadioButton(853-850,560,"SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);
	DstBlend->AddRadioButton(853-850,575,"1-SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjLSRCBlend,nop,nop,GUI_SetObjLSRCBlendCheck);

	ConstMatMenu->AddSkinnedButton(910-850,596,ZbufferButton,"SWITCH DEPTH MASKING ON/OFF FOR OBJECT",GUI_LSetObjDepthMask,nop,nop,GUI_LSetObjDepthMaskCheck);
	ConstMatMenu->AddByteBar(12,613,136,GUI_NONE,&AEpsilon,0,255,0,"AEPSILON: ","CHANGE ALPHA TEST PASS VALUE",GUI_LSetObjAEps,GUI_LSetObjAEpsAfter);
	ConstMatMenu->AddText(882-850,648,"DEFAULT SETTINGS");
	
	ConstMatMenu->AddSkinnedButton(853-850,656,SolidButton,"SET NON TRANSPARENT BLENDING",nop,nop,nop,nop);
	ConstMatMenu->AddSkinnedButton(910-850,656,GlenzButton,"SET GLENZ TRANSPARENT BLENDING",nop,nop,nop,nop);
	ConstMatMenu->AddSkinnedButton(967-850,656,AlphaMatButton,"SET ALPHA MAPPED TRANSPARENT BLENDING",nop,nop,nop,nop);


	cINTERFACE* EmitterMenu=ParticlesMenu->AddSkinnedButton(863+1-10-850,261,EmitterButton,"EMITTER MENU",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* DeflectorMenu=ParticlesMenu->AddSkinnedButton(863+1+52-10-850,261,DeflectorButton,"DEFLECTOR MENU",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* GravityMenu=ParticlesMenu->AddSkinnedButton(863+1+52+63-10-850,261,GravityButton,"GRAVITY MENU",GUI_ButtonSelect,nop,nop,nop);
	ParticlesMenu->ItemList[0].ButtonPushed=true;
	ParticlesMenu->SetActiveBranch(0);
	EmitterMenu->AddSkinnedButton(860-850,291-13,HeadButton,"TURN PARTICLE HEAD ON/OFF",GUI_SetEmitterHead,nop,nop,CheckSelectedHead);
	EmitterMenu->AddSkinnedButton(970-850,291-13,TailButton,"TURN PARTICLE TAIL ON/OFF",GUI_SetEmitterTail,nop,nop,CheckSelectedTail);
	EmitterMenu->AddSkinnedButton(908-850,290-13,BoxButton,"CREATE CUBE SHAPED EMITTER",CreateCubeEmitter,nop,nop,nop);
	EmitterMenu->AddSkinnedButton(928-850,290-13,SphereButton,"CREATE SPHERE SHAPED EMITTER",CreateSphereEmitter,nop,nop,nop);
	EmitterMenu->AddSkinnedButton(948-850,290-13,CylinderButton,"CREATE CYLINDER SHAPED EMITTER",CreateCylinderEmitter,nop,nop,nop);
	
	EmitterMenu->AddSkinnedButton(910-850,309-13,ResetButton,"RESET PARTICLES ( r )",GUI_ResetSceneParticles,nop,nop,nop,'r');
	EmitterMenu->AddSkinnedButton(860-850,309-13,RndRot,"TURN DEFAULT RANDOM ROTATION ON/OFF",GUI_SetEmitterRandRotate,nop,nop,CheckSelectedRandRotate);
	EmitterMenu->AddSkinnedButton(970-850,309-13,Fixy,"FIX UP DIRECTION OF PARTICLES",GUI_SetEmitterFixUp,nop,nop,CheckSelectedFixUp);

	EmitterMenu->AddFloatBar(862-850,325-13,136,GUI_NONE,&ParticlesPerFrame,0,255,1,10,"PRT /  FRAME: ","PARTICLES EMITTED PER FRAME",GUI_SetParticlesPerFrame,GUI_SetParticlesPerFrameAfter);
	EmitterMenu->AddIntBar(862-850,344-13,136,GUI_NONE,&ParticleLife,0,6000,200,"PRT LIFE: ","DEFAULT PARTICLE LIFE LENGTH (IN FRAMES) (0 FOR NEVER DIE)",GUI_SetParticleLife,GUI_SetParticleLifeAfter);
	EmitterMenu->AddIntBar(862-850,363-13,136,GUI_NONE,&ParticleLifeChaos,0,1000,0,"PRT LIFE CHAOS: ","PARTICLE LIFE MODULATION (IN FRAMES)",GUI_SetParticleLifeChaos,GUI_SetParticleLifeChaosAfter);
	EmitterMenu->AddIntBar(862-850,382-13,136,GUI_NONE,&MaxParticles,0,4000,2000,"MAX PARTICLES: ","MAXIMAL NUMBER OF PARTICLES FROM THIS EMITTER");
	EmitterMenu->AddFloatBar(862-850,401-13,136,GUI_NONE,&InnerRadius,0,1,0.05f,0.8f,"INNER RADIUS: ","INNER RADIUS OF SPHERE / CYLINDER EMITTER",GUI_SetParticleInnerRadius,GUI_SetParticleInnerRadiusAfter);
	EmitterMenu->AddFloatBar(862-850,420-13,136,GUI_NONE,&DefaultSpeed,0,1,0.05f,0.1f,"SPEED: ","PARTICLE DEFAULT SPEED (RIGHTCLICK TO SET DIRECTION FOR EMITTER)",GUI_SetParticleSpeed,GUI_SetParticleSpeedAfter);
	EmitterMenu->AddFloatBar(862-850,439-13,136,GUI_NONE,&SpeedChaos,0,1,0.05f,0,"SPEED CHAOS: ","PARTICLE RANDOM SPEED (RIGHTCLICK TO SET DIRECTION FOR EMITTER)",GUI_SetParticleSpeedChaos,GUI_SetParticleSpeedChaosAfter);
	EmitterMenu->AddFloatBar(862-850,458-13,136,GUI_NONE,&DirChaos,0,180,1.0f,0,"DIR CHAOS: ","PARTICLE RANDOM DIRECTION (RIGHTCLICK TO SET DIRECTION FOR EMITTER)",GUI_SetParticleDirChaos,GUI_SetParticleDirChaosAfter);
	EmitterMenu->AddIntBar(862-850,477-13,136,GUI_RED,&r1,0,255,255,"RED1: ","PARTICLE RED START",GUI_SetRed1,GUI_SetRed1After);
	EmitterMenu->AddIntBar(862-850,496-13,136,GUI_GREEN,&g1,0,255,255,"GREEN1: ","PARTICLE GREEN START",GUI_SetGreen1,GUI_SetGreen1After);
	EmitterMenu->AddIntBar(862-850,515-13,136,GUI_BLUE,&b1,0,255,255,"BLUE1: ","PARTICLE BLUE START",GUI_SetBlue1,GUI_SetBlue1After);
	EmitterMenu->AddIntBar(862-850,534-13,136,GUI_NONE,&a1,0,255,255,"ALPHA1: ","PARTICLE ALPHA START",GUI_SetAlpha1,GUI_SetAlpha1After);
	EmitterMenu->AddIntBar(862-850,553-13,136,GUI_NONE,&c1,0,255,0,"CHAOS1: ","PARTICLE START COLOR CHAOS",GUI_SetChaos1,GUI_SetChaos1After);
	EmitterMenu->AddIntBar(862-850,572-13,136,GUI_RED,&r2,0,255,255,"RED2: ","PARTICLE RED END",GUI_SetRed2,GUI_SetRed2After);
	EmitterMenu->AddIntBar(862-850,591-13,136,GUI_GREEN,&g2,0,255,255,"GREEN2: ","PARTICLE GREEN END",GUI_SetGreen2,GUI_SetGreen2After);
	EmitterMenu->AddIntBar(862-850,610-13,136,GUI_BLUE,&b2,0,255,255,"BLUE2: ","PARTICLE BLUE END",GUI_SetBlue2,GUI_SetBlue2After);
	EmitterMenu->AddIntBar(862-850,629-13,136,GUI_NONE,&a2,0,255,0,"ALPHA2: ","PARTICLE ALPHA END",GUI_SetAlpha2,GUI_SetAlpha2After);
	EmitterMenu->AddIntBar(862-850,648-13,136,GUI_NONE,&c2,0,255,0,"SIZE CHAOS: ","PARTICLE END COLOR CHAOS",GUI_SetChaos2,GUI_SetChaos2After);
	EmitterMenu->AddFloatBar(862-850,667-13,136,GUI_NONE,&ParticleSize,0,15,0.01f,0.01f,"SIZE: ","TEXTURED PARTICLE SIZE",GUI_SetParticleSize,GUI_SetParticleSizeAfter);
	EmitterMenu->AddIntBar(862-850,686-13,136,GUI_NONE,&TailLength,1,16,3,"TAIL RES: ","PARTICLE TAIL RESOLUTION",GUI_SetTailLength1,GUI_SetTailLength1After);
	EmitterMenu->AddIntBar(862-850,705-13,136,GUI_NONE,&TailLength2,1,16,2,"TAIL MOD: ","PARTICLE TAIL MULTIPLYER",GUI_SetTailLength2,GUI_SetTailLength2After);

	EmitterMenu->AddFloatBar(862-850,724-13,136,GUI_NONE,&Camfade,0,50,0.0f,0.01f,"CAMFADE: ","START FADING WHEN THIS FAR FROM CAMERA",GUI_SetCamfade,GUI_SetCamfadeAfter);
	EmitterMenu->AddFloatBar(862-850,742-13,136,GUI_NONE,&RotSpeed,-180,180,0.0f,0.0f,"ROTSPD: ","ROTATION SPEED",GUI_SetParticleRot,GUI_SetParticleRotAfter);
	EmitterMenu->AddFloatBar(862-850,761-13,136,GUI_NONE,&RotChaos,-180,180,0.0f,0.0f,"ROTCHS: ","ROTATION SPEED CHAOS",GUI_SetParticleRotChs,GUI_SetParticleRotChsAfter);

	DeflectorMenu->AddSkinnedButton(927+10-850,280,SphereButton,"CREATE SPHERE SLOWER",CreateSphereSlow,nop,nop,nop);
	DeflectorMenu->AddSkinnedButton(927-9-850,300,GridButton,"CREATE PLANE DEFLECTOR",CreatePlaneDeflect,nop,nop,nop);
	DeflectorMenu->AddSkinnedButton(927+10-850,300,SphereButton,"CREATE SPHERE DEFLECTOR",CreateSphereDeflect,nop,nop,nop);
	DeflectorMenu->AddFloatBar(862-850,325,136,GUI_NONE,&DeflectLoss,0,2,0.05f,0.8f,"POWER: ","SET THE REMAINING SPEED (%) AFTER COLLISION",GUI_SetDeflectLoss,GUI_SetDeflectLossAfter);
	DeflectorMenu->AddFloatBar(862-850,350,136,GUI_NONE,&MoveLoss,0,0.5f,0.01f,0.8f,"PUSH: ","SET THE REMAINING SPEED (%) AFTER COLLISION",GUI_SetMoveLoss,GUI_SetMoveLossAfter);

	GravityMenu->AddSkinnedButton(892-856,300,DotButton,"CREATE GRAVITATIONAL FIELD",CreateGravity,nop,nop,nop);
	GravityMenu->AddSkinnedButton(949-856,300,DirectButton,"CREATE DIRECTIONAL GRAVITATIONAL FIELD",CreateGravity2,nop,nop,nop);
	GravityMenu->AddFloatBar(862-850,325,136,GUI_NONE,&GravityStrength,-5.0f,5.0f,0.01f,0,"GRAVITY: ","GRAVITY PULL STRENGTH / ROTATION STRENGTH",GUI_SetGravityStrength,GUI_SetGravityStrengthAfter);

	SplineEditorInterface=Layouter->AddSplineEditor(334,645,974,763);
	Layouter->AddIntBar(247,744,70,GUI_NONE,&Xzoom,1,10,1,"T ZOOM: ","TIMELINE ZOOM",nop,nop);
	VertToggle=Layouter->AddSkinnedButton(975,645,VerticalButton,"TOGGLE TIME VERTICAL MOVE ON/OFF ( t )",GUI_ButtonSwitch,nop,nop,nop,'t');
	VertToggle->ParentItem->ButtonPushed=true;
	HorToggle=Layouter->AddSkinnedButton(975,664,HorizontalButton,"TOGGLE VALUE HORIZONTAL MOVE ON/OFF ( h )",GUI_ButtonSwitch,nop,nop,nop,'h');
	HorToggle->ParentItem->ButtonPushed=true;

	MainAnimList=new ITEMLIST();
	AnimList=LSplineMenu->AddItemList(35,280,116,8,MainAnimList,"");

	ObjectSplines=new ITEMLIST();
	LightSplines=new ITEMLIST();
	EmitterSplines=new ITEMLIST();
	CamSplines=new ITEMLIST();
	SubWorldSplines=new ITEMLIST();

	LSplineMenu->AddSkinnedButton(48,380,NewButton,"NEW ANIMATION",GUI_ReadLine_Anim,nop,nop,Modeller_NewAnimationCallback);
	LSplineMenu->AddSkinnedButton(93,380,DelButton,"DELETE ANIMATION",nop,nop,nop,nop);

	LightSplines->AddItem("LIGHT 1",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 2",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 3",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 4",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 5",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 6",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 7",NULL,nop,nop,nop);
	LightSplines->AddItem("LIGHT 8",NULL,nop,nop,nop);

	SplineMenuInterface=LSplineMenu->AddSubInterface();
	SceneSplines=SplineMenuInterface->AddSkinnedButton( 45,400,ObjectsButton,"OBJECT SPLINES",GUI_ButtonSelect_ObjSplines,nop,nop,nop);

	SceneSplines->AddSkinnedButton(185,645,MoveButton,"ADD POS X KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(204,645,MoveButton,"ADD POS Y KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(223,645,MoveButton,"ADD POS Z KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(204,664,RotateButton,"ADD ROTATION KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(-251,50,ObjectsButton,"ADD ROT Y KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(-270,50,ObjectsButton,"ADD ROT Z KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(185,683,ScaleButton,"ADD SCALE X KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(204,683,ScaleButton,"ADD SCALE Y KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(223,683,ScaleButton,"ADD SCALE Z KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(175,702,RedButton,"ADD RED KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(194,702,GreenButton,"ADD GREEN KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(213,702,BlueButton,"ADD BLUE KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(232,702,AlphaButton,"ADD ALPHA KEY",GUI_AddObjKey,nop,nop,nop);
	SceneSplines->AddSkinnedButton(204,725,ObjectsButton,"ADD FULL KEY",GUI_AddObjKey,nop,nop,nop);

	SceneSplines->AddSkinnedButton(261+4,645,MoveButton,"DISPLAY POS X KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(280+4,645,MoveButton,"DISPLAY POS Y KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(299+4,645,MoveButton,"DISPLAY POS Z KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(280+4,664,RotateButton,"DISPLAY ROTATION KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(-251,330,ObjectsButton,"DISPLAY ROT Y KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(-270,330,ObjectsButton,"DISPLAY ROT Z KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(261+4,683,ScaleButton,"DISPLAY SCL X KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(280+4,683,ScaleButton,"DISPLAY SCL Y KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(299+4,683,ScaleButton,"DISPLAY SCL Z KEY",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(175+80,702,RedButton,"DISPLAY RED SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(194+80,702,GreenButton,"DISPLAY GREEN SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(213+80,702,BlueButton,"DISPLAY BLUE SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;
	SceneSplines->AddSkinnedButton(232+80,702,AlphaButton,"DISPLAY ALPHA SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SceneSplines->ItemList[SceneSplines->ItemNum-1].ButtonPushed=true;

	cINTERFACE *SceneSplineForms=SceneSplines->AddSubInterface();

	SceneSplineForms->AddSkinnedButton(975,688,JumpButton,"DISCRETE SPLINE",SelectSceneSplineForm,nop,nop,nop);
	SceneSplineForms->AddSkinnedButton(975,707,LinearSplineButton,"LINEAR SPLINE",SelectSceneSplineForm,nop,nop,nop);
	SceneSplineForms->AddSkinnedButton(975,726,SplineButton,"CATMULL-ROM SPLINE",SelectSceneSplineForm,nop,nop,nop);

	SceneSplineForms->AddSkinnedButton(997,688,SineButton,"SINUS WAVEFORM",SelectSceneSplineForm,nop,nop,nop);
	SceneSplineForms->AddSkinnedButton(997,707,SawButton,"SAW WAVEFORM",SelectSceneSplineForm,nop,nop,nop);
	SceneSplineForms->AddSkinnedButton(997,726,SquareButton,"SQUARE WAVEFORM",SelectSceneSplineForm,nop,nop,nop);

	SceneSplineForms->AddSkinnedButton(975,745,SplineButton,"B-SPLINE",SelectSceneSplineForm,nop,nop,nop);


	CameraSplines=SplineMenuInterface->AddSkinnedButton( 64,400,PCameraButton,"CAMERA SPLINES",GUI_ButtonSelect,nop,nop,nop);
	CameraSplines->AddSkinnedButton(48,427,NewButton,"NEW CAMERA TRACK",GUI_ReadLine_Anim,nop,nop,Modeller_NewCameraCallback);
	CameraSplines->AddSkinnedButton(93,427,DelButton,"DELETE CAMERA TRACK",nop,nop,nop,nop);
	CameraSplines->AddSkinnedButton(185,645,ObjectsButton,"ADD EYE X KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(204,645,ObjectsButton,"ADD EYE Y KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(223,645,ObjectsButton,"ADD EYE Z KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(185,664,ObjectsButton,"ADD TARGET X KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(204,664,ObjectsButton,"ADD TARGET Y KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(223,664,ObjectsButton,"ADD TARGET Z KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(195,683,ObjectsButton,"ADD FOV KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(214,683,ObjectsButton,"ADD ROLL KEY",GUI_AddCamKey,nop,nop,nop);
	CameraSplines->AddSkinnedButton(204,712,ObjectsButton,"ADD FULL KEY",GUI_AddCamKey,nop,nop,nop);

	CameraSplines->AddSkinnedButton(261,645,ObjectsButton,"DISPLAY EYE X SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(280,645,ObjectsButton,"DISPLAY EYE Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(299,645,ObjectsButton,"DISPLAY EYE Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(261,664,ObjectsButton,"DISPLAY TARGET X SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(280,664,ObjectsButton,"DISPLAY TARGET Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(299,664,ObjectsButton,"DISPLAY TARGET Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(271,683,ObjectsButton,"DISPLAY FOV SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;
	CameraSplines->AddSkinnedButton(290,683,ObjectsButton,"DISPLAY ROLL SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); CameraSplines->ItemList[CameraSplines->ItemNum-1].ButtonPushed=true;

	cINTERFACE *CameraSplineForms=CameraSplines->AddSubInterface();

	CameraSplineForms->AddSkinnedButton(975,688,JumpButton,"DISCRETE SPLINE",SelectCameraSplineForm,nop,nop,nop);
	CameraSplineForms->AddSkinnedButton(975,707,LinearSplineButton,"LINEAR SPLINE",SelectCameraSplineForm,nop,nop,nop);
	CameraSplineForms->AddSkinnedButton(975,726,SplineButton,"CATMULL-ROM SPLINE",SelectCameraSplineForm,nop,nop,nop);

	CameraSplineForms->AddSkinnedButton(997,688,SineButton,"SINUS WAVEFORM",SelectCameraSplineForm,nop,nop,nop);
	CameraSplineForms->AddSkinnedButton(997,707,SawButton,"SAW WAVEFORM",SelectCameraSplineForm,nop,nop,nop);
	CameraSplineForms->AddSkinnedButton(997,726,SquareButton,"SQUARE WAVEFORM",SelectCameraSplineForm,nop,nop,nop);
	CameraSplineForms->AddSkinnedButton(975,745,SplineButton,"B-SPLINE",SelectCameraSplineForm,nop,nop,nop);

	LightSplines_=SplineMenuInterface->AddSkinnedButton( 83,400,LightsButton,"LIGHT SPLINES",GUI_ButtonSelect,nop,nop,nop);
	ParticleSplines=SplineMenuInterface->AddSkinnedButton(102,400,ParticlesButton,"EMITTER SPLINES",GUI_ButtonSelect_ParticleSplines,nop,nop,nop);

	ParticleSplines->AddSkinnedButton(185,645,MoveButton,"ADD POS X KEY",GUI_AddParticleKey,nop,nop,nop); 
	ParticleSplines->AddSkinnedButton(204,645,MoveButton,"ADD POS Y KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(223,645,MoveButton,"ADD POS Z KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(204,664,RotateButton,"ADD ROTATION KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(-204,664,ObjectsButton,"ADD ROT Y KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(-223,664,ObjectsButton,"ADD ROT Z KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(185,683,ScaleButton,"ADD SCL X KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(204,683,ScaleButton,"ADD SCL Y KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(223,683,ScaleButton,"ADD SCL Z KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(175,702,RedButton,"ADD RED KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(194,702,GreenButton,"ADD GREEN KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(213,702,BlueButton,"ADD BLUE KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(232,702,AlphaButton,"ADD ALPHA KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(195,721,ScaleButton,"ADD SIZE KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(214,721,ObjectsButton,"ADD PARTICLES/FRAME KEY",GUI_AddParticleKey,nop,nop,nop);
	ParticleSplines->AddSkinnedButton(204,740,ObjectsButton,"ADD FULL KEY",GUI_AddParticleKey,nop,nop,nop);

	ParticleSplines->AddSkinnedButton(185+80,645,MoveButton,"DISPLAY POS X SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(204+80,645,MoveButton,"DISPLAY POS Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(223+80,645,MoveButton,"DISPLAY POS Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(204+80,664,RotateButton,"DISPLAY ROTATION SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(-204,664,ObjectsButton,"DISPLAY ROT Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(-223,664,ObjectsButton,"DISPLAY ROT Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(185+80,683,ScaleButton,"DISPLAY SCALE X SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(204+80,683,ScaleButton,"DISPLAY SCALE Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(223+80,683,ScaleButton,"DISPLAY SCALE Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(175+80,702,RedButton,"DISPLAY RED SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(194+80,702,GreenButton,"DISPLAY GREEN SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(213+80,702,BlueButton,"DISPLAY BLUE SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(232+80,702,AlphaButton,"DISPLAY ALPHA SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(195+80,721,ScaleButton,"DISPLAY SIZE SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;
	ParticleSplines->AddSkinnedButton(214+80,721,ObjectsButton,"DISPLAY PARTICLES/FRAME SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); ParticleSplines->ItemList[ParticleSplines->ItemNum-1].ButtonPushed=true;

	cINTERFACE *ParticleSplineForms=ParticleSplines->AddSubInterface();

	ParticleSplineForms->AddSkinnedButton(975,688,JumpButton,"DISCRETE SPLINE",SelectParticleSplineForm,nop,nop,nop);
	ParticleSplineForms->AddSkinnedButton(975,707,LinearSplineButton,"LINEAR SPLINE",SelectParticleSplineForm,nop,nop,nop);
	ParticleSplineForms->AddSkinnedButton(975,726,SplineButton,"CATMULL-ROM SPLINE",SelectParticleSplineForm,nop,nop,nop);

	ParticleSplineForms->AddSkinnedButton(997,688,SineButton,"SINUS WAVEFORM",SelectParticleSplineForm,nop,nop,nop);
	ParticleSplineForms->AddSkinnedButton(997,707,SawButton,"SAW WAVEFORM",SelectParticleSplineForm,nop,nop,nop);
	ParticleSplineForms->AddSkinnedButton(997,726,SquareButton,"SQUARE WAVEFORM",SelectParticleSplineForm,nop,nop,nop);
	ParticleSplineForms->AddSkinnedButton(975,745,SplineButton,"B-SPLINE",SelectParticleSplineForm,nop,nop,nop);


	SWorldSplines=SplineMenuInterface->AddSkinnedButton(121,400,ObjectsButton,"SUBWORLD SPLINES",GUI_ButtonSelect,nop,nop,nop);

	SWorldSplines->AddSkinnedButton(185,645,MoveButton,"ADD POS X KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(204,645,MoveButton,"ADD POS Y KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(223,645,MoveButton,"ADD POS Z KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(204,664,RotateButton,"ADD ROTATION KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(-204,664,ObjectsButton,"ADD ROT Y KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(-223,664,ObjectsButton,"ADD ROT Z KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(185,683,ScaleButton,"ADD SCALE X KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(204,683,ScaleButton,"ADD SCALE Y KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(223,683,ScaleButton,"ADD SCALE Z KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(204,721-19,SplinesButton,"ADD SUBWORLDANIM KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(214-800,721-19,SplinesButton,"ADD ANIMATIONTIMER KEY",GUI_AddSubWorldKey,nop,nop,nop);
	SWorldSplines->AddSkinnedButton(204,740-19,ObjectsButton,"ADD FULL KEY",GUI_AddSubWorldKey,nop,nop,nop);

	SWorldSplines->AddSkinnedButton(185+80,645,MoveButton,"DISPLAY POS X SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(204+80,645,MoveButton,"DISPLAY POS Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(223+80,645,MoveButton,"DISPLAY POS Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(204+80,664,RotateButton,"DISPLAY ROTATION SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(-204,664,ObjectsButton,"DISPLAY ROT Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(-223,664,ObjectsButton,"DISPLAY ROT Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(185+80,683,ScaleButton,"DISPLAY SCALE X SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(204+80,683,ScaleButton,"DISPLAY SCALE Y SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(223+80,683,ScaleButton,"DISPLAY SCALE Z SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(204+80,721-19,SplinesButton,"DISPLAY SUBWORLDANIM SPLINES",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;
	SWorldSplines->AddSkinnedButton(214+80-800,721-19,SplinesButton,"DISPLAY ANIMATIONTIMER SPLINE",GUI_ButtonSwitch,GUI_ButtonSelect_DontChangeSubinterface,nop,nop); SWorldSplines->ItemList[SWorldSplines->ItemNum-1].ButtonPushed=true;

	cINTERFACE *SWorldSplineForms=SWorldSplines->AddSubInterface();

	SWorldSplineForms->AddSkinnedButton(975,688,JumpButton,"DISCRETE SPLINE",SelectSubWorldSplineForm,nop,nop,nop);
	SWorldSplineForms->AddSkinnedButton(975,707,LinearSplineButton,"LINEAR SPLINE",SelectSubWorldSplineForm,nop,nop,nop);
	SWorldSplineForms->AddSkinnedButton(975,726,SplineButton,"CATMULL-ROM SPLINE",SelectSubWorldSplineForm,nop,nop,nop);

	SWorldSplineForms->AddSkinnedButton(997,688,SineButton,"SINUS WAVEFORM",SelectSubWorldSplineForm,nop,nop,nop);
	SWorldSplineForms->AddSkinnedButton(997,707,SawButton,"SAW WAVEFORM",SelectSubWorldSplineForm,nop,nop,nop);
	SWorldSplineForms->AddSkinnedButton(997,726,SquareButton,"SQUARE WAVEFORM",SelectSubWorldSplineForm,nop,nop,nop);
	SWorldSplineForms->AddSkinnedButton(975,745,SplineButton,"B-SPLINE",SelectSubWorldSplineForm,nop,nop,nop);

	SplineMenuInterface->AddSkinnedButton(10,740,BigStoreButton,"STORE CURRENT POSITION FOR LATER LOADING (c)",GUI_StorePos,nop,nop,nop,'c');
	SplineMenuInterface->AddSkinnedButton(95,740,BigLoadButton,"LOAD STORED POSITION (v)",GUI_LoadPos,nop,nop,nop,'v');
	SplineMenuInterface->SetActiveBranch(0);
	SplineMenuInterface->ItemList[0].ButtonPushed=true;

	ObjList=SceneSplines->AddItemList(35,430,116,8,ObjectSplines,"");
	
	CamList=CameraSplines->AddItemList(35,450,116,8,CamSplines,"");
	LightsList=LightSplines_->AddItemList(35,430,116,8,LightSplines,"");
	PrtList=ParticleSplines->AddItemList(35,430,116,8,EmitterSplines,"");
	SwrldList=SWorldSplines->AddItemList(35,430,116,8,SubWorldSplines,"");
	SWorldSplines->AddText(40,540,"SUBWORLD ANIM:\0");

	SwrldAnimList=SWorldSplines->AddItemList(35,550,116,8,NULL,"");
	SWorldSplines->AddFloatBar(862-850,670,136,GUI_NONE,&SWAnimPos,0,1,0.01f,1,"TIME:","SET SUBWORLD ANIMATION POSITION",nop,nop);

	WorldScroller=LFileMenu->AddItemList(865-850,295,137,14,WorldListScroller,"");
	LFileMenu->AddSkinnedButton(893-850,460,NewButton,"CREATE NEW WORLD",GUI_ReadLine,nop,nop,Modeller_NewWorldCallback);
	LFileMenu->AddSkinnedButton(938-850,460,DelButton,"DELETE SCENE",nop,nop,nop,nop);
	LFileMenu->AddSkinnedButton(893-850,476,ClearButton,"DELETE EVERYTHING FROM WORLD",GUI_ClearWorld,nop,nop,nop);
	LFileMenu->AddSkinnedButton(938-850,476,ExportButton,"EXPORT WORLD",nop,nop,nop,nop);

	Layouter->AddSkinnedButton(862-850,87+19,CopyButtonSmall,"COPY SELECTED OBJECTS ( c )",GUI_LayouterCopy,nop,nop,nop,'c');
	//LayouterDisplay1->ParentItem->ParentInterface->ParentItem->ActiveBranch=false;
}