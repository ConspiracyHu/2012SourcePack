#include "LayouterGUI.h"
#include "Layouter.h"

float PlaybackPos;

SPLINE *DraggedSpline;
int DraggedKey;
Key OriginalKey;
bool KeyDragging;
bool QuatKeyDragging;
bool AnimKeyDragging;
SPLINE *q1,*q2,*q3,*q4;
Key O1,O2,O3,O4;

SPLINE *a1,*a2;
bool AnimKeyDragged;

bool Panning;
float Panx,Pany;

float Xpos=0,Ypos=0;
int Xzoom=1,Yzoom=1;

void CheckSplineKeyframe(bool SplineVisible, SPLINE *Spline, GUIITEM *View)
{
	if (!KeyDragging)
	{
		if (!SplineVisible)	return;

		for (int x=0; x<Spline->KeyNum; x++)
		{
			int xp,yp;
			xp=(int)(Spline->Keys[x].Time*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom);
			yp=(int)(-Spline->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			if (LeftClickInWindow(xp-3,yp-3,xp+3,yp+3) && !KeyDragging)
			{
				KeyDragging=true;
				DraggedSpline=Spline;
				OriginalKey=Spline->Keys[x];
				DraggedKey=x;
			}
		}
	}
}

void CheckSplineKeyframeAnim(bool SplineVisible, SPLINE *a, SPLINE *b, GUIITEM *View)
{
	if (!KeyDragging)
	{
		if (!SplineVisible)	return;

		for (int x=0; x<b->KeyNum; x++)
		{
			int Akey=(int)floor((x+1)/2.0f);

			int xp1,yp1;
			int yp2;
			xp1=(int)(b->Keys[x].Time*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom);
			
			if (x<b->KeyNum-1)
			yp1=(int)(-a->Keys[Akey].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			else yp1=-2000;

			yp2=(int)(-b->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			
			if (LeftClickInWindow(xp1-3,yp1-3,xp1+3,yp1+3) && !AnimKeyDragging)
			{
				AnimKeyDragging=true;
				AnimKeyDragged=true;
				a1=a;
				a2=b;
				DraggedKey=x;
			}
			if (LeftClickInWindow(xp1-3,yp2-3,xp1+3,yp2+3) && !AnimKeyDragging)
			{
				AnimKeyDragging=true;
				AnimKeyDragged=false;
				a1=a;
				a2=b;
				DraggedKey=x;
			}
		}
	}
}

void CheckSplineKeyframeQuat(bool SplineVisible, SPLINE *a, SPLINE *b, SPLINE *c, SPLINE *d , GUIITEM *View)
{
	if (!QuatKeyDragging)
	{
		if (!SplineVisible)	return;

		for (int x=0; x<a->KeyNum; x++)
		{
			int xp1,yp1;
			int yp2;
			int yp3;
			int yp4;
			xp1=(int)(a->Keys[x].Time*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom);
			yp1=(int)(-a->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			yp2=(int)(-b->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			yp3=(int)(-c->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			yp4=(int)(-d->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);

			if ((
				LeftClickInWindow(xp1-3,yp1-3,xp1+3,yp1+3) ||
				LeftClickInWindow(xp1-3,yp2-3,xp1+3,yp2+3) ||
				LeftClickInWindow(xp1-3,yp3-3,xp1+3,yp3+3) ||
				LeftClickInWindow(xp1-3,yp4-3,xp1+3,yp4+3)
				) && !QuatKeyDragging)
			{
				QuatKeyDragging=true;
				q1=a;
				q2=b;
				q3=c;
				q4=d;
				O1=a->Keys[x];
				O2=b->Keys[x];
				O3=c->Keys[x];
				O4=d->Keys[x];
				DraggedKey=x;
			}
		}
	}
}


void __stdcall GUI_SplineEditorLeftClick(cINTERFACE* Interface, int ButtonId)
{
	if (ActualWorld)
	{
		if (SplineMenuInterface->ItemList[0].ButtonPushed && ActualWorld->AnimNum) //baseobject splines
		if (ActualWorld->ObjectCount && ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			CheckSplineKeyframe(SceneSplines->ItemList[10+4].ButtonPushed,s->Posx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[11+4].ButtonPushed,s->Posy,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[12+4].ButtonPushed,s->Posz,SplineEditorInterface->ParentItem);
			/*CheckSplineKeyframe(SceneSplines->ItemList[13].ButtonPushed,s->Rotx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[14].ButtonPushed,s->Roty,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[15].ButtonPushed,s->Rotz,SplineEditorInterface->ParentItem);*/
			CheckSplineKeyframeQuat(SceneSplines->ItemList[13+4].ButtonPushed,s->Rotx,s->Roty,s->Rotz,s->Rotw,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[16+4].ButtonPushed,s->Sclx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[17+4].ButtonPushed,s->Scly,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[18+4].ButtonPushed,s->Sclz,SplineEditorInterface->ParentItem);

			CheckSplineKeyframe(SceneSplines->ItemList[19+4].ButtonPushed,s->Red,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[20+4].ButtonPushed,s->Green,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[21+4].ButtonPushed,s->Blue,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SceneSplines->ItemList[22+4].ButtonPushed,s->Alpha,SplineEditorInterface->ParentItem);
		}
		if (SplineMenuInterface->ItemList[1].ButtonPushed && ActualWorld->CamNum) //camera splines
		if (ActualWorld->CamNum && ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem))
		{
			CAMSPLINES *Camera=(CAMSPLINES*)ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)->Data;
			CheckSplineKeyframe(CameraSplines->ItemList[11].ButtonPushed,Camera->Eyex,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[12].ButtonPushed,Camera->Eyey,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[13].ButtonPushed,Camera->Eyez,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[14].ButtonPushed,Camera->Trgx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[15].ButtonPushed,Camera->Trgy,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[16].ButtonPushed,Camera->Trgz,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[17].ButtonPushed,Camera->Fov,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(CameraSplines->ItemList[18].ButtonPushed,Camera->Roll,SplineEditorInterface->ParentItem);
		}
		if (SplineMenuInterface->ItemList[3].ButtonPushed && ActualWorld->AnimNum) //particle splines
		if (ActualWorld->ParticleCount && ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			CheckSplineKeyframe(ParticleSplines->ItemList[16].ButtonPushed,s->Posx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[17].ButtonPushed,s->Posy,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[18].ButtonPushed,s->Posz,SplineEditorInterface->ParentItem);
			CheckSplineKeyframeQuat(ParticleSplines->ItemList[19].ButtonPushed,s->Rotx,s->Roty,s->Rotz,s->Rotw,SplineEditorInterface->ParentItem);			
			CheckSplineKeyframe(ParticleSplines->ItemList[22].ButtonPushed,s->Sclx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[23].ButtonPushed,s->Scly,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[24].ButtonPushed,s->Sclz,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[25].ButtonPushed,s->Red,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[26].ButtonPushed,s->Green,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[27].ButtonPushed,s->Blue,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[28].ButtonPushed,s->Alpha,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[29].ButtonPushed,s->Size,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(ParticleSplines->ItemList[30].ButtonPushed,s->Prtfrme,SplineEditorInterface->ParentItem);
		}
		if (SplineMenuInterface->ItemList[4].ButtonPushed && ActualWorld->AnimNum) //subworld splines
		if (ActualWorld->SubWorldCount && ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			CheckSplineKeyframe(SWorldSplines->ItemList[10+2].ButtonPushed,s->Posx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SWorldSplines->ItemList[11+2].ButtonPushed,s->Posy,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SWorldSplines->ItemList[12+2].ButtonPushed,s->Posz,SplineEditorInterface->ParentItem);
			CheckSplineKeyframeQuat(SWorldSplines->ItemList[13+2].ButtonPushed,s->Rotx,s->Roty,s->Rotz,s->Rotw,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SWorldSplines->ItemList[16+2].ButtonPushed,s->Sclx,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SWorldSplines->ItemList[17+2].ButtonPushed,s->Scly,SplineEditorInterface->ParentItem);
			CheckSplineKeyframe(SWorldSplines->ItemList[18+2].ButtonPushed,s->Sclz,SplineEditorInterface->ParentItem);
			CheckSplineKeyframeAnim(SWorldSplines->ItemList[20+2].ButtonPushed,s->AnimID,s->AnimTime,SplineEditorInterface->ParentItem);
		}
	}
}

void UpdateSplines()
{
	if (ActualWorld)
	{
		if (/*SplineMenuInterface->ItemList[0].ButtonPushed &&*/ ActualWorld->AnimNum) //baseobject splines
		if (ActualWorld->ObjectCount)
		{
			//WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem)->Data;
			for (ITEM *x=ObjectSplines->List; x; x=x->Next)
			{
				WORLDOBJECT *Object=(WORLDOBJECT*)(x->Data);
				DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
				if (s->Posx->KeyNum) Object->PosData.Pos.x=s->Posx->GetInterpolatedValue(PlaybackPos);
				if (s->Posy->KeyNum) Object->PosData.Pos.y=s->Posy->GetInterpolatedValue(PlaybackPos);
				if (s->Posz->KeyNum) Object->PosData.Pos.z=s->Posz->GetInterpolatedValue(PlaybackPos);
				if (s->Sclx->KeyNum) Object->PosData.Scale.x=s->Sclx->GetInterpolatedValue(PlaybackPos);
				if (s->Scly->KeyNum) Object->PosData.Scale.y=s->Scly->GetInterpolatedValue(PlaybackPos);
				if (s->Sclz->KeyNum) Object->PosData.Scale.z=s->Sclz->GetInterpolatedValue(PlaybackPos);
				if (s->Red->KeyNum) Object->Color.r=min(255,max(0,(int)(s->Red->GetInterpolatedValue(PlaybackPos)*16.0f)));
				if (s->Green->KeyNum) Object->Color.g=min(255,max(0,(int)(s->Green->GetInterpolatedValue(PlaybackPos)*16.0f)));
				if (s->Blue->KeyNum) Object->Color.b=min(255,max(0,(int)(s->Blue->GetInterpolatedValue(PlaybackPos)*16.0f)));
				if (s->Alpha->KeyNum) Object->Color.a=min(255,max(0,(int)(s->Alpha->GetInterpolatedValue(PlaybackPos)*16.0f)));

				if (s->Rotx->KeyNum)
				{
					Spline_Slerp(s->Rotx,
								 s->Roty,
								 s->Rotz,
								 s->Rotw,
								 &Object->PosData.Quaternion.s,
								 &Object->PosData.Quaternion.v.x,
								 &Object->PosData.Quaternion.v.y,
								 &Object->PosData.Quaternion.v.z,PlaybackPos);
				}
			}
		}
		if (SplineMenuInterface->ItemList[1].ButtonPushed && ActualWorld->CamNum && ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)) //camera splines
		{
			CAMSPLINES *Camera=(CAMSPLINES*)ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)->Data;
			if (Camera->Eyex->KeyNum) LayouterDisplay4->ParentItem->Cam.Eye.x=Camera->Eyex->GetInterpolatedValue(PlaybackPos);
			if (Camera->Eyey->KeyNum) LayouterDisplay4->ParentItem->Cam.Eye.y=Camera->Eyey->GetInterpolatedValue(PlaybackPos);
			if (Camera->Eyez->KeyNum) LayouterDisplay4->ParentItem->Cam.Eye.z=Camera->Eyez->GetInterpolatedValue(PlaybackPos);
			if (Camera->Trgx->KeyNum) LayouterDisplay4->ParentItem->Cam.Target.x=Camera->Trgx->GetInterpolatedValue(PlaybackPos);
			if (Camera->Trgy->KeyNum) LayouterDisplay4->ParentItem->Cam.Target.y=Camera->Trgy->GetInterpolatedValue(PlaybackPos);
			if (Camera->Trgz->KeyNum) LayouterDisplay4->ParentItem->Cam.Target.z=Camera->Trgz->GetInterpolatedValue(PlaybackPos);
			if (Camera->Fov->KeyNum)  LayouterDisplay4->ParentItem->Cam.Fov=Camera->Fov->GetInterpolatedValue(PlaybackPos);
			if (Camera->Roll->KeyNum) LayouterDisplay4->ParentItem->Cam.Roll=Camera->Roll->GetInterpolatedValue(PlaybackPos);
		}
		if (/*SplineMenuInterface->ItemList[3].ButtonPushed &&*/ ActualWorld->AnimNum) //particle splines
		if (ActualWorld->ParticleCount)
		{
			//WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem)->Data;
			for (ITEM *x=EmitterSplines->List; x; x=x->Next)
			{
				WORLDOBJECT *Object=(WORLDOBJECT*)(x->Data);
				DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
				if (s->Posx->KeyNum) Object->PosData.Pos.x=s->Posx->GetInterpolatedValue(PlaybackPos);
				if (s->Posy->KeyNum) Object->PosData.Pos.y=s->Posy->GetInterpolatedValue(PlaybackPos);
				if (s->Posz->KeyNum) Object->PosData.Pos.z=s->Posz->GetInterpolatedValue(PlaybackPos);
				if (s->Sclx->KeyNum) Object->PosData.Scale.x=s->Sclx->GetInterpolatedValue(PlaybackPos);
				if (s->Scly->KeyNum) Object->PosData.Scale.y=s->Scly->GetInterpolatedValue(PlaybackPos);
				if (s->Sclz->KeyNum) Object->PosData.Scale.z=s->Sclz->GetInterpolatedValue(PlaybackPos);
				if (s->Prtfrme->KeyNum) Object->EmitterData->ParticlesPerFrame=s->Prtfrme->GetInterpolatedValue(PlaybackPos);
				if (s->Rotx->KeyNum)
				{
					Spline_Slerp(s->Rotx,s->Roty,s->Rotz,s->Rotw,&Object->PosData.Quaternion.s,&Object->PosData.Quaternion.v.x,&Object->PosData.Quaternion.v.y,&Object->PosData.Quaternion.v.z,PlaybackPos);
				}
			}
		}
		if (ActualWorld->AnimNum) //subworld splines
		if (ActualWorld->SubWorldCount)
		{
			//WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem)->Data;
			for (ITEM *x=SubWorldSplines->List; x; x=x->Next)
			{
				WORLDOBJECT *Object=(WORLDOBJECT*)(x->Data);
				DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
				if (s->Posx->KeyNum) Object->PosData.Pos.x=s->Posx->GetInterpolatedValue(PlaybackPos);
				if (s->Posy->KeyNum) Object->PosData.Pos.y=s->Posy->GetInterpolatedValue(PlaybackPos);
				if (s->Posz->KeyNum) Object->PosData.Pos.z=s->Posz->GetInterpolatedValue(PlaybackPos);
				if (s->Sclx->KeyNum) Object->PosData.Scale.x=s->Sclx->GetInterpolatedValue(PlaybackPos);
				if (s->Scly->KeyNum) Object->PosData.Scale.y=s->Scly->GetInterpolatedValue(PlaybackPos);
				if (s->Sclz->KeyNum) Object->PosData.Scale.z=s->Sclz->GetInterpolatedValue(PlaybackPos);
				if (s->AnimID->KeyNum) Object->APosData.AnimID=(int)s->AnimID->GetInterpolatedValue(PlaybackPos);
				if (s->AnimTime->KeyNum) Object->APosData.AnimPos=s->AnimTime->GetInterpolatedValue(PlaybackPos);
				if (s->Rotx->KeyNum)
				{
					Spline_Slerp(s->Rotx,
								 s->Roty,
								 s->Rotz,
								 s->Rotw,
								 &Object->PosData.Quaternion.s,
								 &Object->PosData.Quaternion.v.x,
								 &Object->PosData.Quaternion.v.y,
								 &Object->PosData.Quaternion.v.z,PlaybackPos);
				}
			}
		}
	}
}

void __stdcall GUI_SplineEditorRightClick(cINTERFACE* Interface, int ButtonId)
{
	//PlaybackPos=(mx-Interface->ItemList[ButtonId].x1)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1);
	//
	PlaybackPos=((Xpos*Xzoom+mx)-Interface->ItemList[ButtonId].x1)/(float) ((Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1))/float(Xzoom);
	PlaybackPos=max(min(PlaybackPos,1),0);

	UpdateSplines();
}

void __stdcall GUI_SplineEditorMidClick(cINTERFACE* Interface, int ButtonId)
{ //pan
	if (!Panning)
	{
		Panning=true;
		Panx=Xpos;
		Pany=Ypos;
	}
}

void KillKeyframe(GUIITEM *View, SPLINE *Spline)
{
	bool DeleteKey=false;
	int KeyDeleted;
	if (Spline && Spline->Interpolation<aDDict_SINE)
	{
		for (int x=0; x<Spline->KeyNum; x++)
		{
			int xp,yp;
			xp=(int)(Spline->Keys[x].Time*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom);
			yp=(int)(-Spline->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			if (MouseInWindow(xp-3,yp-3,xp+3,yp+3))
			{
				if (Keys[(int)('d')])
				{
					DeleteKey=true;
					KeyDeleted=x;
					Keys[(int)('d')]=false;
				}
			}
		}
	}
	if (DeleteKey)
	{
		Spline->Keys[KeyDeleted].Time=1.2f;
		Spline->Arrange(0);
		Spline->KeyNum-=1;
		Spline->InitVectors();
	}
}

void KillQuatKeyframe(GUIITEM *View, SPLINE *a, SPLINE *b, SPLINE *c, SPLINE *d)
{
	bool DeleteKey=false;
	int KeyDeleted;
	if (a)
	{
		for (int x=0; x<a->KeyNum; x++)
		{
			int xp,yp1,yp2,yp3,yp4;
			xp=(int)(a->Keys[x].Time*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom);
			yp1=(int)(-a->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			yp2=(int)(-b->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			yp3=(int)(-c->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			yp4=(int)(-d->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			if (MouseInWindow(xp-3,yp1-3,xp+3,yp1+3) ||
				MouseInWindow(xp-3,yp2-3,xp+3,yp2+3) ||
				MouseInWindow(xp-3,yp3-3,xp+3,yp3+3) ||
				MouseInWindow(xp-3,yp4-3,xp+3,yp4+3))
			{
				if (Keys[(int)('d')])
				{
					DeleteKey=true;
					KeyDeleted=x;
					Keys[(int)('d')]=false;
				}
			}
		}
	}
	if (DeleteKey)
	{
		a->Keys[KeyDeleted].Time=1.2f;
		a->Arrange(0);
		a->KeyNum-=1;

		b->Keys[KeyDeleted].Time=1.2f;
		b->Arrange(0);
		b->KeyNum-=1;

		c->Keys[KeyDeleted].Time=1.2f;
		c->Arrange(0);
		c->KeyNum-=1;

		d->Keys[KeyDeleted].Time=1.2f;
		d->Arrange(0);
		d->KeyNum-=1;

		Spline_QuaternionGetVectors(a,b,c,d);
	}
}


void __stdcall GUI_SplineEditorWheel(cINTERFACE* Interface, int ButtonId)
{ //zoom
	if (!Interface->ItemList[ButtonId].LeftClickInWindow) 
	{
		KeyDragging=false;
		QuatKeyDragging=false;
		AnimKeyDragging=false;
	}
	if (!Interface->ItemList[ButtonId].MidClickInWindow) Panning=false;

	/*Xzoom+=Wheel;
	Xzoom=max(1,Xzoom);*/

	if (Interface->ItemList[ButtonId].MouseInWindow)
	{
		Yzoom+=Wheel;
		Yzoom=max(1,Yzoom);
	}

	if (KeyDragging)
	{
		float Time=max(0,min(1,
			((Xpos*Xzoom+mx)-Interface->ItemList[ButtonId].x1)/(float) ((Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1))/float(Xzoom)
			));
		float Value=(-((my+Ypos*Yzoom)-(Interface->ItemList[ButtonId].y1+Interface->ItemList[ButtonId].y2)/2.0f))/(float)Yzoom;
		
		if (HorToggle->ParentItem->ButtonPushed)
		DraggedSpline->Keys[DraggedKey].Time=KillFloat(Time,2);
		
		if (VertToggle->ParentItem->ButtonPushed)
		DraggedSpline->Keys[DraggedKey].Value=KillFloat(Value,2);

		DraggedKey=DraggedSpline->Arrange(DraggedKey);
		DraggedSpline->InitVectors();
		UpdateSplines();
	}

	if (QuatKeyDragging)
	{
		float Time=max(0,min(1,
			((Xpos*Xzoom+mx)-Interface->ItemList[ButtonId].x1)/(float) ((Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1))/float(Xzoom)
			));
		//float Value=(-((my+Ypos*Yzoom)-(Interface->ItemList[ButtonId].y1+Interface->ItemList[ButtonId].y2)/2.0f))/(float)Yzoom;
		q1->Keys[DraggedKey].Time=KillFloat(Time,2);
		q1->Arrange(DraggedKey);
		q2->Keys[DraggedKey].Time=KillFloat(Time,2);
		q2->Arrange(DraggedKey);
		q3->Keys[DraggedKey].Time=KillFloat(Time,2);
		q3->Arrange(DraggedKey);
		q4->Keys[DraggedKey].Time=KillFloat(Time,2);
		DraggedKey=q4->Arrange(DraggedKey);
		Spline_QuaternionGetVectors(q1,q2,q3,q4);
		UpdateSplines();
	}

	if (AnimKeyDragging)
	{
		float Time=max(0,min(1,
			((Xpos*Xzoom+mx)-Interface->ItemList[ButtonId].x1)/(float) ((Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1))/float(Xzoom)
			));
		float Value=(-((my+Ypos*Yzoom)-(Interface->ItemList[ButtonId].y1+Interface->ItemList[ButtonId].y2)/2.0f))/(float)Yzoom;

		int Akey=(int)floor((DraggedKey+1)/2.0f);

		if (DraggedKey>0) Time=max(Time,a2->Keys[Akey*2-2].Time);
		if (DraggedKey<a2->KeyNum-1) Time=min(Time,a2->Keys[Akey*2+1].Time);

		if (HorToggle->ParentItem->ButtonPushed)
		if (DraggedKey>=1 && DraggedKey<a2->KeyNum-1)
		{
			a1->Keys[Akey].Time=KillFloat(Time,2);
			a2->Keys[Akey*2-1].Time=KillFloat(Time,2);
			a2->Keys[Akey*2].Time=KillFloat(Time,2);
		}

		if (VertToggle->ParentItem->ButtonPushed)
		{
			if (AnimKeyDragged) a1->Keys[Akey].Value=KillFloat(Value,2);
			else a2->Keys[DraggedKey].Value=KillFloat(Value,2);
		}

		/*a1->Keys[DraggedKey].Time=Time;
		a1->Arrange(DraggedKey);
		a2->Keys[DraggedKey].Time=Time;
		DraggedKey=a2->Arrange(DraggedKey);*/

		UpdateSplines();
	}


	if (Panning)
	{
		Xpos=min((Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*((Xzoom-1)/(float)Xzoom),max(0,Panx+(mbx-mx)/(float)Xzoom));
		Ypos=Pany+(mby-my)/(float)Yzoom;
	}

	GUIITEM *View=SplineEditorInterface->ParentItem;

	if (ActualWorld)
	{
		if (SplineMenuInterface->ItemList[0].ButtonPushed && ActualWorld->AnimNum) //baseobject splines
		if (ActualWorld->ObjectCount && ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			if (SceneSplines->ItemList[10+4].ButtonPushed) KillKeyframe(View,s->Posx);
			if (SceneSplines->ItemList[11+4].ButtonPushed) KillKeyframe(View,s->Posy);
			if (SceneSplines->ItemList[12+4].ButtonPushed) KillKeyframe(View,s->Posz);
			if (SceneSplines->ItemList[13+4].ButtonPushed) KillQuatKeyframe(View,s->Rotx,s->Roty,s->Rotz,s->Rotw);
			if (SceneSplines->ItemList[16+4].ButtonPushed) KillKeyframe(View,s->Sclx);
			if (SceneSplines->ItemList[17+4].ButtonPushed) KillKeyframe(View,s->Scly);
			if (SceneSplines->ItemList[18+4].ButtonPushed) KillKeyframe(View,s->Sclz);
			if (SceneSplines->ItemList[19+4].ButtonPushed) KillKeyframe(View,s->Red);
			if (SceneSplines->ItemList[20+4].ButtonPushed) KillKeyframe(View,s->Green);
			if (SceneSplines->ItemList[21+4].ButtonPushed) KillKeyframe(View,s->Blue);
			if (SceneSplines->ItemList[22+4].ButtonPushed) KillKeyframe(View,s->Alpha);
		}
		if (SplineMenuInterface->ItemList[1].ButtonPushed && ActualWorld->CamNum && ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)) //camera splines
		{
			CAMSPLINES *Camera=(CAMSPLINES*)ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)->Data;
			if (CameraSplines->ItemList[11].ButtonPushed) KillKeyframe(View, Camera->Eyex);
			if (CameraSplines->ItemList[12].ButtonPushed) KillKeyframe(View, Camera->Eyey);
			if (CameraSplines->ItemList[13].ButtonPushed) KillKeyframe(View, Camera->Eyez);
			if (CameraSplines->ItemList[14].ButtonPushed) KillKeyframe(View, Camera->Trgx);
			if (CameraSplines->ItemList[15].ButtonPushed) KillKeyframe(View, Camera->Trgy);
			if (CameraSplines->ItemList[16].ButtonPushed) KillKeyframe(View, Camera->Trgz);
			if (CameraSplines->ItemList[17].ButtonPushed) KillKeyframe(View, Camera->Fov);
			if (CameraSplines->ItemList[18].ButtonPushed) KillKeyframe(View, Camera->Roll);
		}
		if (SplineMenuInterface->ItemList[3].ButtonPushed && ActualWorld->AnimNum) //particle splines
		if (ActualWorld->ParticleCount && ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			if (ParticleSplines->ItemList[16].ButtonPushed) KillKeyframe(View,s->Posx);
			if (ParticleSplines->ItemList[17].ButtonPushed) KillKeyframe(View,s->Posy);
			if (ParticleSplines->ItemList[18].ButtonPushed) KillKeyframe(View,s->Posz);
			if (ParticleSplines->ItemList[19].ButtonPushed) KillQuatKeyframe(View,s->Rotx,s->Roty,s->Rotz,s->Rotw);
			if (ParticleSplines->ItemList[22].ButtonPushed) KillKeyframe(View,s->Sclx);
			if (ParticleSplines->ItemList[23].ButtonPushed) KillKeyframe(View,s->Scly);
			if (ParticleSplines->ItemList[24].ButtonPushed) KillKeyframe(View,s->Sclz);
			glColor4f(1,0,0,1);
			if (ParticleSplines->ItemList[25].ButtonPushed) KillKeyframe(View,s->Red);
			glColor4f(0,1,0,1);
			if (ParticleSplines->ItemList[26].ButtonPushed) KillKeyframe(View,s->Green);
			glColor4f(0,0,1,1);
			if (ParticleSplines->ItemList[27].ButtonPushed) KillKeyframe(View,s->Blue);
			glColor4f(1,1,1,1);
			if (ParticleSplines->ItemList[28].ButtonPushed) KillKeyframe(View,s->Alpha);
			glColor4f(highblue);
			if (ParticleSplines->ItemList[29].ButtonPushed) KillKeyframe(View,s->Size);
			if (ParticleSplines->ItemList[30].ButtonPushed) KillKeyframe(View,s->Prtfrme);
		}
		if (SplineMenuInterface->ItemList[4].ButtonPushed && ActualWorld->AnimNum) //baseobject splines
		if (ActualWorld->SubWorldCount && ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			if (SWorldSplines->ItemList[10+2].ButtonPushed) KillKeyframe(View,s->Posx);
			if (SWorldSplines->ItemList[11+2].ButtonPushed) KillKeyframe(View,s->Posy);
			if (SWorldSplines->ItemList[12+2].ButtonPushed) KillKeyframe(View,s->Posz);
			if (SWorldSplines->ItemList[13+2].ButtonPushed) KillQuatKeyframe(View,s->Rotx,s->Roty,s->Rotz,s->Rotw);
			if (SWorldSplines->ItemList[16+2].ButtonPushed) KillKeyframe(View,s->Sclx);
			if (SWorldSplines->ItemList[17+2].ButtonPushed) KillKeyframe(View,s->Scly);
			if (SWorldSplines->ItemList[18+2].ButtonPushed) KillKeyframe(View,s->Sclz);
			if (SWorldSplines->ItemList[19+2].ButtonPushed) KillKeyframe(View,s->AnimID);
			if (SWorldSplines->ItemList[19+2].ButtonPushed) KillKeyframe(View,s->AnimTime);
		}
	}

}


void DisplaySingleSpline(GUIITEM *View, SPLINE *Spline)
{
	if (Spline)
	{
		glBegin(GL_LINE_STRIP);
		int x;
		for (x=0; x<View->x2-View->x1; x++)
		{
			glVertex2i(x+View->x1,(int)(
				
				-Spline->GetInterpolatedValue( (x+Xpos*(float)Xzoom)/(float) (View->x2-View->x1)/(float)Xzoom )*Yzoom

				+(View->y2+View->y1)/2.0f)-(int)(Ypos*(float)Yzoom));
		}
		glEnd();

		for (x=0; x<Spline->KeyNum; x++)
		{
			int xp,yp;
			xp=(int)(Spline->Keys[x].Time*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom);
			yp=(int)(-Spline->Keys[x].Value*Yzoom+(View->y2+View->y1)/2.0f-Ypos*Yzoom);
			if (MouseInWindow(xp-3,yp-3,xp+3,yp+3))
			{
				glPointSize(10);
			}
			else
			glPointSize(7);

			glBegin(GL_POINTS);
			glVertex2i(xp,yp);
			glEnd();
		}
	}
}

void DisplaySplineEditor(GUIITEM *View)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	glScissor(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_DEPTH_TEST);

	glColor4f(darkblue);
	quad(View->x1,View->y1,View->x2,View->y2);
	glColor4f(highblue);
	rectangle(View->x1,View->y1,View->x2,View->y2);


	for (int x=-256; x<=256; x+=8)
	{
		if (x==0)
		glColor4f(1,1,0,0);
		else
		glColor4f(0,0,0,0);
		line(View->x1,(int)(-(Ypos+x)*Yzoom+(View->y2+View->y1)/2.0f),View->x2,(int)(-(Ypos+x)*Yzoom+(View->y2+View->y1)/2.0f));
		glColor4f(highblue);
		glRasterPos2i(View->x1+3,(int)(-(Ypos+x)*Yzoom+(View->y2+View->y1)/2.0f-2));
		char a[20];
		sprintf(a,"%.1f\0",(float)x);
		glPrint(a,base);
	}

	{
	for (float x=0.00f; x<1; x+=1/16.0f)
	{
		glColor4f(0,0,0,0);
		line((int)((int)(x*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom)),
			 View->y1,
			 (int)((int)(x*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom)),
			 View->y2-1);
		glColor4f(highblue);
		glRasterPos2i((int)((int)(x*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom))+5,View->y2-2);
		char a[20];
		sprintf(a,"%.2f\0",(float)x);
		glPrint(a,base);
	}
	}

	glColor4f(highblue);

	if (ActualWorld)
	{
		if (SplineMenuInterface->ItemList[0].ButtonPushed && ActualWorld->AnimNum) //baseobject splines
		if (ActualWorld->ObjectCount && ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)(ScrollerFindItem(ObjectSplines, ObjList->ParentItem->SelectedItem)->Data);
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			if (SceneSplines->ItemList[10+4].ButtonPushed) DisplaySingleSpline(View,s->Posx);
			if (SceneSplines->ItemList[11+4].ButtonPushed) DisplaySingleSpline(View,s->Posy);
			if (SceneSplines->ItemList[12+4].ButtonPushed) DisplaySingleSpline(View,s->Posz);
			if (SceneSplines->ItemList[13+4].ButtonPushed) DisplaySingleSpline(View,s->Rotx);
			if (SceneSplines->ItemList[13+4].ButtonPushed) DisplaySingleSpline(View,s->Roty);
			if (SceneSplines->ItemList[13+4].ButtonPushed) DisplaySingleSpline(View,s->Rotz);
			if (SceneSplines->ItemList[13+4].ButtonPushed) DisplaySingleSpline(View,s->Rotw);
			if (SceneSplines->ItemList[16+4].ButtonPushed) DisplaySingleSpline(View,s->Sclx);
			if (SceneSplines->ItemList[17+4].ButtonPushed) DisplaySingleSpline(View,s->Scly);
			if (SceneSplines->ItemList[18+4].ButtonPushed) DisplaySingleSpline(View,s->Sclz);
			glColor4f(1,0,0,1);
			if (SceneSplines->ItemList[19+4].ButtonPushed) DisplaySingleSpline(View,s->Red);
			glColor4f(0,1,0,1);
			if (SceneSplines->ItemList[20+4].ButtonPushed) DisplaySingleSpline(View,s->Green);
			glColor4f(0,0,1,1);
			if (SceneSplines->ItemList[21+4].ButtonPushed) DisplaySingleSpline(View,s->Blue);
			glColor4f(1,1,1,1);
			if (SceneSplines->ItemList[22+4].ButtonPushed) DisplaySingleSpline(View,s->Alpha);
		}
		if (SplineMenuInterface->ItemList[1].ButtonPushed && ActualWorld->CamNum && ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)) //camera splines
		{
			CAMSPLINES *Camera=(CAMSPLINES*)ScrollerFindItem(ActualWorld->CameraList, CamList->ParentItem->SelectedItem)->Data;
			if (CameraSplines->ItemList[11].ButtonPushed) DisplaySingleSpline(View, Camera->Eyex);
			if (CameraSplines->ItemList[12].ButtonPushed) DisplaySingleSpline(View, Camera->Eyey);
			if (CameraSplines->ItemList[13].ButtonPushed) DisplaySingleSpline(View, Camera->Eyez);
			if (CameraSplines->ItemList[14].ButtonPushed) DisplaySingleSpline(View, Camera->Trgx);
			if (CameraSplines->ItemList[15].ButtonPushed) DisplaySingleSpline(View, Camera->Trgy);
			if (CameraSplines->ItemList[16].ButtonPushed) DisplaySingleSpline(View, Camera->Trgz);
			if (CameraSplines->ItemList[17].ButtonPushed) DisplaySingleSpline(View, Camera->Fov);
			if (CameraSplines->ItemList[18].ButtonPushed) DisplaySingleSpline(View, Camera->Roll);
		}
		if (SplineMenuInterface->ItemList[3].ButtonPushed && ActualWorld->AnimNum) //particle splines
		if (ActualWorld->ParticleCount && ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(EmitterSplines, PrtList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			if (ParticleSplines->ItemList[16].ButtonPushed) DisplaySingleSpline(View,s->Posx);
			if (ParticleSplines->ItemList[17].ButtonPushed) DisplaySingleSpline(View,s->Posy);
			if (ParticleSplines->ItemList[18].ButtonPushed) DisplaySingleSpline(View,s->Posz);
			if (ParticleSplines->ItemList[19].ButtonPushed) DisplaySingleSpline(View,s->Rotx);
			if (ParticleSplines->ItemList[19].ButtonPushed) DisplaySingleSpline(View,s->Roty);
			if (ParticleSplines->ItemList[19].ButtonPushed) DisplaySingleSpline(View,s->Rotz);
			if (ParticleSplines->ItemList[19].ButtonPushed) DisplaySingleSpline(View,s->Rotw);
			if (ParticleSplines->ItemList[22].ButtonPushed) DisplaySingleSpline(View,s->Sclx);
			if (ParticleSplines->ItemList[23].ButtonPushed) DisplaySingleSpline(View,s->Scly);
			if (ParticleSplines->ItemList[24].ButtonPushed) DisplaySingleSpline(View,s->Sclz);
			glColor4f(1,0,0,1);
			if (ParticleSplines->ItemList[25].ButtonPushed) DisplaySingleSpline(View,s->Red);
			glColor4f(0,1,0,1);
			if (ParticleSplines->ItemList[26].ButtonPushed) DisplaySingleSpline(View,s->Green);
			glColor4f(0,0,1,1);
			if (ParticleSplines->ItemList[27].ButtonPushed) DisplaySingleSpline(View,s->Blue);
			glColor4f(1,1,1,1);
			if (ParticleSplines->ItemList[28].ButtonPushed) DisplaySingleSpline(View,s->Alpha);
			glColor4f(highblue);
			if (ParticleSplines->ItemList[29].ButtonPushed) DisplaySingleSpline(View,s->Size);
			if (ParticleSplines->ItemList[30].ButtonPushed) DisplaySingleSpline(View,s->Prtfrme);
		}
		if (SplineMenuInterface->ItemList[4].ButtonPushed && ActualWorld->AnimNum) //subworld splines
		if (ActualWorld->SubWorldCount && ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem))
		{
			WORLDOBJECT *Object=(WORLDOBJECT*)ScrollerFindItem(SubWorldSplines, SwrldList->ParentItem->SelectedItem)->Data;
			DEFAULTOBJECTSPLINES *s=&Object->Animations[ActualAnim];
			if (SWorldSplines->ItemList[10+2].ButtonPushed) DisplaySingleSpline(View,s->Posx);
			if (SWorldSplines->ItemList[11+2].ButtonPushed) DisplaySingleSpline(View,s->Posy);
			if (SWorldSplines->ItemList[12+2].ButtonPushed) DisplaySingleSpline(View,s->Posz);
			if (SWorldSplines->ItemList[13+2].ButtonPushed) DisplaySingleSpline(View,s->Rotx);
			if (SWorldSplines->ItemList[13+2].ButtonPushed) DisplaySingleSpline(View,s->Roty);
			if (SWorldSplines->ItemList[13+2].ButtonPushed) DisplaySingleSpline(View,s->Rotz);
			if (SWorldSplines->ItemList[13+2].ButtonPushed) DisplaySingleSpline(View,s->Rotw);
			if (SWorldSplines->ItemList[16+2].ButtonPushed) DisplaySingleSpline(View,s->Sclx);
			if (SWorldSplines->ItemList[17+2].ButtonPushed) DisplaySingleSpline(View,s->Scly);
			if (SWorldSplines->ItemList[18+2].ButtonPushed) DisplaySingleSpline(View,s->Sclz);
			if (SWorldSplines->ItemList[19+2].ButtonPushed) DisplaySingleSpline(View,s->AnimID);
			if (SWorldSplines->ItemList[19+2].ButtonPushed) DisplaySingleSpline(View,s->AnimTime);
		}
	}


	glColor4f(1,1,1,1);
	
	line((int)((int)(PlaybackPos*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom)),
		 View->y1,
		 (int)((int)(PlaybackPos*(View->x2-View->x1)*Xzoom+View->x1-Xpos*Xzoom)),
		 View->y2-1);

	glDisable(GL_SCISSOR_TEST);
	//glPopAttrib();
}

void __stdcall LCheckTransform(cINTERFACE* Interface, int ScrollerID)
{
	Transforming=false;
	Cammoving=false;
	for (int x=0; x<Interface->ItemList[ScrollerID].Interface->ItemNum; x++)
		if (Interface->ItemList[ScrollerID].Interface->ItemList[x].ActiveBranch)
		{
			if (LeftClickInWindow(Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].x1,
				                  Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].y1,
				                  Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].x2,
				                  Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].y2)) Transforming=true;
		}
	if (ActualWorld)
	for (int i=0; i<ActualWorld->ObjectNum; i++)
	if (!ActualWorld->ObjectList[i].Deleted && !ActualWorld->ObjectList[i].ListItem->Hidden)
	{
		if (Transforming)
		memcpy(&ActualWorld->ObjectList[i].PosData,&ActualWorld->ObjectList[i].PosDataBuffer,sizeof(RST));
		else
		memcpy(&ActualWorld->ObjectList[i].PosDataBuffer,&ActualWorld->ObjectList[i].PosData,sizeof(RST));
	}
}

void __stdcall GUI_L3dLeftClick(cINTERFACE* Interface, int ButtonId)
{
	RST m;
	m.Scale=V3_Make(1,1,1);
	m.Pos=V3_Make(0,0,0);
	m.Quaternion.v=m.Pos;
	m.Quaternion.s=1;
	GUIITEM *View=&Interface->ItemList[ButtonId];

	VECTOR3 Min=V3_Make(0,0,0),Max=V3_Make(0,0,0);
	VECTOR3 Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot=V3_Make(0,0,0);
	if (ActualWorld)
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted && !ActualWorld->ObjectList[x].ListItem->Hidden)
	{
		if (ActualWorld->ObjectList[x].ListItem->Selected)
		{
			Pivot=V3_Add(Pivot,ActualWorld->ObjectList[x].PosData.Pos);
			PCount++;
			Selected++;

			RST2Matrix(&ActualWorld->ObjectList[x].PosData,ActualWorld->ObjectList[x].ModelView,1);

			TestScaleFactors(Min,Max,V3_Make( 1, 1, 1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make( 1,-1, 1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make( 1,-1,-1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make( 1, 1,-1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1, 1, 1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1,-1, 1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1,-1,-1),ActualWorld->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1, 1,-1),ActualWorld->ObjectList[x].ModelView);

		}
	}
	if (PCount) Pivot=V3_Mults(Pivot,1/(float)PCount);

	VECTOR3 ScaleFactors=V3_Sub(Max,Min);

	if (!View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed) Pivot=V3_Make(0,0,0);

	VECTOR3 v;
	v.x=View->i.x*View->i.x+View->j.x*View->j.x;
	v.y=View->i.y*View->i.y+View->j.y*View->j.y;
	v.z=View->i.z*View->i.z+View->j.z*View->j.z;

	float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));

	if (!View->WireFrame) v=V3_Make(1,1,1);

	if (
		!(lx<=Interface->ItemList[ButtonId].x1+19 && ly<Interface->ItemList[ButtonId].y1+19) &&
		!(lx>=Interface->ItemList[ButtonId].x2-19*4 && ly<Interface->ItemList[ButtonId].y1+19) &&
		!(lx>=Interface->ItemList[ButtonId].x2-19 && ly<Interface->ItemList[ButtonId].y1+19*3))
	{
		if (ActualWorld)
		{
			if (MainLayouterScroller->ParentItem->SelectedItem==1) //camera transform
			{
				float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
				float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
				LayouterDisplay4->ParentItem->Cam.Eye.a[View->i1]=xx*View->i.a[View->i1]+View->Cam.Eye.a[View->i1];
				LayouterDisplay4->ParentItem->Cam.Eye.a[View->j1]=yy*View->j.a[View->j1]+View->Cam.Eye.a[View->j1];
			}
			if (MainLayouterScroller->ParentItem->SelectedItem==2) //light transform
			{
				ITEM *a=ActualWorld->ItemList->List->Next->Next->SubItems;
				for (int x=0; x<8; x++)
				{
					if (a->Selected && ActualWorld->Lights[x].Lit)
					{
						if (ActualWorld->Lights[x].Spot_Cutoff==180 && ActualWorld->Lights[x].Position[3]==0) //dirlight
						{
							float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
							float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
							ActualWorld->Lights[x].Position[View->i1]=xx*View->i.a[View->i1];
							ActualWorld->Lights[x].Position[View->j1]=yy*View->j.a[View->j1];
						}

						if (ActualWorld->Lights[x].Spot_Cutoff==180 && ActualWorld->Lights[x].Position[3]==1) //dotlight
						{
							float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
							float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
							ActualWorld->Lights[x].Position[View->i1]=xx*View->i.a[View->i1]+View->Cam.Eye.a[View->i1];
							ActualWorld->Lights[x].Position[View->j1]=yy*View->j.a[View->j1]+View->Cam.Eye.a[View->j1];
						}

						if (ActualWorld->Lights[x].Position[3]==1 && ActualWorld->Lights[x].Spot_Cutoff!=180) //spotlight
						{
							float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
							float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
							ActualWorld->Lights[x].Position[View->i1]=xx*View->i.a[View->i1]+View->Cam.Eye.a[View->i1];
							ActualWorld->Lights[x].Position[View->j1]=yy*View->j.a[View->j1]+View->Cam.Eye.a[View->j1];
						}
					}
					a=a->Next;
				}
			}
			if (!(PolySelect && !View->WireFrame) && MainLayouterScroller->ParentItem->SelectedItem==3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8)
			{
				//for (ITEM *I=ActualWorld->ItemList->List;I;I=I->Next)
				for (int i=0; i<ActualWorld->ObjectNum; i++)
				if (!ActualWorld->ObjectList[i].Deleted && !ActualWorld->ObjectList[i].ListItem->Hidden)
				{
					ITEM *I=ActualWorld->ObjectList[i].ListItem;
					I->Data=&(ActualWorld->ObjectList[i]);
					if (I->Data && I->Selected)
					{
						//Transform kell 
						if (Interface->ItemList[1].Interface->ItemList[2].Interface->ItemList[0].ButtonPushed)
						{ // scale


							int hits=0;
							unsigned int buffer[120];
							if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[0].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
							{							
								glLineWidth(2);
								glPushAttrib(GL_ALL_ATTRIB_BITS);
								glScissor(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));							
								glEnable(GL_SCISSOR_TEST);
								glColor4f(0,0,0,0);
								glDisable(GL_DEPTH_TEST);
								glEnable(GL_DEPTH_TEST);
								glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
								SetCameraView(&View->Cam,4/3.0f,10,lx,ly);
								glSelectBuffer(120,buffer);
								glRenderMode(GL_SELECT);
								glInitNames();
								glPushName(0);
								glLoadName(0);
								VECTOR3 aa,bb;
								aa=View->Cam.Eye;
								if (View->WireFrame)
								for (int x=0; x<3; x++) if (View->k!=x)	aa.a[x]=0;
								bb=Pivot;
								if (View->WireFrame)
								{
									for (int x=0; x<3; x++) if (View->k!=x)	bb.a[x]=0;
									bb.x*=v.x;
									bb.y*=v.y;
									bb.z*=v.z;
								}
								float f=V3_Length(V3_Sub(aa,bb))/10;

								if (v.x)
								{
									glBegin(GL_LINES);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
									glBegin(GL_TRIANGLES);
									for (int x=0; x<12; x++)
									{
										glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
										glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin(x*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos(x*360.0/12.0*radtheta))*v.z);
										glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.z);
									}
									glEnd();
								}

								glLoadName(1);
								if (v.y)
								{
									glBegin(GL_LINES);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
									glEnd();
									glBegin(GL_TRIANGLES);
									for (int x=0; x<12; x++)
									{
										glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos(x*360.0/12.0*radtheta))*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.z);
									}
									glEnd();
								}

								glLoadName(2);
								if (v.z)
								{
									glBegin(GL_LINES);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
									glEnd();
									glBegin(GL_TRIANGLES);
									for (int x=0; x<12; x++)
									{
										glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos(x*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
									}
									glEnd();
								}
								SetCameraView(&View->Cam,4/3.0f,1,lx,ly);
								glLoadName(3);
								if ((v.x && v.y && v.z) || !v.x)
								{
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glEnd();
								}
								glLoadName(4);
								if ((v.x && v.y && v.z) || !v.y)
								{
									glBegin(GL_QUADS);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glEnd();
								}
								glLoadName(5);
								if ((v.x && v.y && v.z) || !v.z)
								{
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
								}

								glLoadName(6);
								if ((v.x && v.y && v.z) || !v.x)
								{
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+1.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
									glEnd();
								}
								glLoadName(7);
								if ((v.x && v.y && v.z) || !v.y)
								{
									glBegin(GL_QUADS);
									glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+1.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
									glEnd();
								}
								glLoadName(8);
								if ((v.x && v.y && v.z) || !v.z)
								{
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+1.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+1.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
								}

								glLoadName(9);
								if ((v.x && v.y && v.z) || !v.x)
								{
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z)*v.z);
									glEnd();
								}
								if ((v.x && v.y && v.z) || !v.y)
								{
									glBegin(GL_QUADS);
									glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z)*v.z);
									glEnd();
								}
								if ((v.x && v.y && v.z) || !v.z)
								{
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
								}
								hits=glRenderMode(GL_RENDER);
								int smallest=buffer[3];
								int depth=buffer[1];
								for (int k=1; k<hits; k++)
								{
									if (buffer[k*4+1]<(GLuint)depth)
									{
										smallest=buffer[k*4+3];
										depth=buffer[k*4+1];
									}
								}
								buffer[3]=smallest;
								SwitchTo2D();
								glPopAttrib();
								glLineWidth(1);
							}

							float x=(mx-lx)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f)/ScaleFactors.a[View->i1]*4;
							float y=(my-ly)/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f)/ScaleFactors.a[View->j1]*4;

							VECTOR3 mask;
							if (hits)
							{
								mask=V3_Make(0,0,0);
								if (buffer[3]<3)
								mask.a[buffer[3]]=1;
								else
								{
									mask=V3_Make(1,1,1);
									if (buffer[3]<6)
									mask.a[buffer[3]-3]=0;
									else
									mask.a[buffer[3]-6]=0;
								}
							}
							m.Scale.x=View->i.x*View->i.x*x+View->j.x*View->j.x*y+1.0f;
							m.Scale.y=View->i.y*View->i.y*x+View->j.y*View->j.y*y+1.0f;
							m.Scale.z=View->i.z*View->i.z*x+View->j.z*View->j.z*y+1.0f;
							if (hits /*&& !View->WireFrame*/)
							{
								SetCameraView(&View->Cam,4/3.0f,0,0,0);
								VECTOR3 Dir=CalcXYDir((lx-View->x1)/(float)(View->x2-View->x1),
									                  (ly-View->y1)/(float)(View->y2-View->y1),View->Cam.Fov,4/3.0f);
								VECTOR3 Dir2=CalcXYDir((mx-View->x1)/(float)(View->x2-View->x1),(my-View->y1)/(float)(View->y2-View->y1),View->Cam.Fov,4/3.0f);
								float matrix[16];
								MATRIX CameraMatrix;
								M_Identity(CameraMatrix);
								glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
								for (int x=0; x<4; x++)
									for (int y=0; y<4; y++)
										CameraMatrix[y][x]=matrix[x+y*4];

								M_Xform3(CameraMatrix,Dir,Dir);
								M_Xform3(CameraMatrix,Dir2,Dir2);

								VECTOR3 PlaneNormal,PB,D;
								float d;

								switch (buffer[3])
								{
								case 0:
									PlaneNormal=V3_Make(0,1,0);
									PB=V3_Make(0,0,1);
									D=V3_Normalize(V3_Sub(View->Cam.Target,View->Cam.Eye));
									if (fabs(V3_Dot(PB,D))>fabs(V3_Dot(PlaneNormal,D))) 
									{
										PlaneNormal=PB;
										d=Pivot.z;
									}
									else
									d=Pivot.y;
									break;
								case 4:
								case 7:
								case 9:
									PlaneNormal=V3_Make(0,1,0);
									d=Pivot.y;
									break;
								case 1:
									PlaneNormal=V3_Make(1,0,0);
									PB=V3_Make(0,0,1);
									D=V3_Normalize(V3_Sub(View->Cam.Target,View->Cam.Eye));
									if (fabs(V3_Dot(PB,D))>fabs(V3_Dot(PlaneNormal,D))) 
									{
										PlaneNormal=PB;
										d=Pivot.z;
									}
									else
									d=Pivot.x;
									break;
								case 3:
								case 6:
									PlaneNormal=V3_Make(1,0,0);
									d=Pivot.x;
									break;
								case 2:
									PlaneNormal=V3_Make(0,1,0);
									PB=V3_Make(1,0,0);
									D=V3_Normalize(V3_Sub(View->Cam.Target,View->Cam.Eye));
									if (fabs(V3_Dot(PB,D))>fabs(V3_Dot(PlaneNormal,D))) 
									{
										PlaneNormal=PB;
										d=Pivot.x;
									}
									else
									d=Pivot.y;
									break;
								case 5:
								case 8:
									PlaneNormal=V3_Make(0,0,1);
									d=Pivot.z;
									break;
								}

								float t1=distRayPlane(View->Cam.Eye,Dir,PlaneNormal,d);
								float t2=distRayPlane(View->Cam.Eye,Dir2,PlaneNormal,d);

								VECTOR3 P1,P2;

								P1=V3_Add(View->Cam.Eye,V3_Mults(Dir,t1));
								P2=V3_Add(View->Cam.Eye,V3_Mults(Dir2,t2));

								VECTOR3 Displacement=V3_Sub(P2,P1);
								if (buffer[3]>=3 && buffer[3]<=5)
								{
									mask.x=mask.y=mask.z=1;
									mask.a[buffer[3]-3]=0;
									Displacement.x=Displacement.y=Displacement.z=-(lx-mx)/30.0f;
									ScaleFactors.x=ScaleFactors.z=ScaleFactors.y;
								}
								if (buffer[3]==9)
								{
									mask=V3_Make(1,1,1);
									Displacement.x=Displacement.y=Displacement.z=-(lx-mx)/30.0f;
									ScaleFactors.x=ScaleFactors.z=ScaleFactors.y;
								}

								m.Scale.x=(Displacement.x/ScaleFactors.x*3)*mask.x+1;
								m.Scale.y=(Displacement.y/ScaleFactors.y*3)*mask.y+1;
								m.Scale.z=(Displacement.z/ScaleFactors.z*3)*mask.z+1;

								SwitchTo2D();
							}

							/*MouseString=new char[20];
							memset(MouseString,0,20);
							sprintf(MouseString,"(%.3fx,%.3fx)",x+1,y+1);


							float x=(mx-lx)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
							float y=(my-ly)/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);

							ActualWorld->ObjectList[i].PosData.Scale.a[View->i1]*=(1+x*View->i.a[View->i1]);
							ActualWorld->ObjectList[i].PosData.Scale.a[View->j1]*=(1+y*View->j.a[View->j1]);*/

							//MouseString=new char[30];
							//memset(MouseString,0,30);
							_snprintf(MouseString,100,"(%.3fx, %.3fx, %.3fx)\0",m.Scale.x,m.Scale.y,m.Scale.z);
						}
						if (Interface->ItemList[1].Interface->ItemList[2].Interface->ItemList[1].ButtonPushed)
						{ // rotate
							VECTOR3 aa,bb;
							aa=View->Cam.Eye;
							if (View->WireFrame)
							for (int x=0; x<3; x++) if (View->k!=x)	aa.a[x]=0;
							bb=Pivot;
							if (View->WireFrame)
							{
								for (int x=0; x<3; x++) if (View->k!=x)	bb.a[x]=0;
								bb.x*=v.x;
								bb.y*=v.y;
								bb.z*=v.z;
							}
							float f=V3_Length(V3_Sub(aa,bb))/10;

							int hits=0;
							unsigned int buffer[12];
							memset(buffer,0,sizeof(int)*12);
							if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[1].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
							{
								glLineWidth(2);
								glPushAttrib(GL_ALL_ATTRIB_BITS);
								glScissor(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));							
								glEnable(GL_SCISSOR_TEST);
								glColor4f(0,0,0,0);
								glDisable(GL_DEPTH_TEST);
								glEnable(GL_DEPTH_TEST);
								glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
								SetCameraView(&View->Cam,4/3.0f,10,lx,ly);
								glSelectBuffer(12,buffer);
								glRenderMode(GL_SELECT);
								glInitNames();
								glPushName(0);

								glLoadName(2);
								glBegin(GL_LINE_STRIP);
								int x;
								for (x=0; x<36*4+1; x++)
									glVertex3d((sin(x*2.5*radtheta)*2*f+Pivot.x)*v.x,(cos(x*2.5*radtheta)*2*f+Pivot.y)*v.y,Pivot.z*v.z);
								glEnd();

								glLoadName(1);
								glBegin(GL_LINE_STRIP);
								for (x=0; x<36*4+1; x++)
									glVertex3d((sin(x*2.5*radtheta)*2*f+Pivot.x)*v.x,Pivot.y*v.y,(cos(x*2.5*radtheta)*2*f+Pivot.z)*v.z);
								glEnd();

								glLoadName(0);
								glBegin(GL_LINE_STRIP);
								for (x=0; x<36*4+1; x++)
									glVertex3d(Pivot.x*v.x,(sin(x*2.5*radtheta)*2*f+Pivot.y)*v.y,(cos(x*2.5*radtheta)*2*f+Pivot.z)*v.z);
								glEnd();
								hits=glRenderMode(GL_RENDER);
								int smallest=buffer[3];
								int depth=buffer[1];
								for (int k=1; k<hits; k++)
								{
									if (buffer[k*4+1]<(GLuint)depth)
									{
										smallest=buffer[k*4+3];
										depth=buffer[k*4+1];
									}
								}
								buffer[3]=smallest;
								SwitchTo2D();
								glPopAttrib();
								glLineWidth(1);
							}

							float x=(float)((lx-mx)*radtheta);
							VECTOR3 v=V3_Make(0,0,0);
							v.a[View->k]=1; if (Interface->ParentItem->InterfaceItemId==2) v.a[View->k]=-1;
							AngleAxis2Quaternion(v.x,v.y,v.z,-x,m.Quaternion);
							int Displacement;
							if (hits && !View->WireFrame)
							{
								SetCameraView(&View->Cam,4/3.0f,0,0,0);
								VECTOR3 Dir=CalcXYDir((lx-View->x1)/(float)(View->x2-View->x1),
									                  (ly-View->y1)/(float)(View->y2-View->y1),View->Cam.Fov,4/3.0f);
								VECTOR3 Dir2=CalcXYDir((mx-View->x1)/(float)(View->x2-View->x1),(my-View->y1)/(float)(View->y2-View->y1),View->Cam.Fov,4/3.0f);
								float matrix[16];
								MATRIX CameraMatrix;
								M_Identity(CameraMatrix);
								glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
								for (int x=0; x<4; x++)
									for (int y=0; y<4; y++)
										CameraMatrix[y][x]=matrix[x+y*4];

								M_Xform3(CameraMatrix,Dir,Dir);
								M_Xform3(CameraMatrix,Dir2,Dir2);

								VECTOR3 mask=V3_Make(0,0,0);
								mask.a[buffer[3]]=1;

								float t1=distRayPlane(View->Cam.Eye,Dir,mask,Pivot.a[buffer[3]]);
								float t2=distRayPlane(View->Cam.Eye,Dir2,mask,Pivot.a[buffer[3]]);

								VECTOR3 P1,P2;

								P1=V3_Add(View->Cam.Eye,V3_Mults(Dir,t1)); //pont a koron
								P2=V3_Add(View->Cam.Eye,V3_Mults(Dir2,t2)); //kurzorpozicio

								VECTOR3 B=V3_Normalize(V3_Sub(P1,Pivot));
								VECTOR3 Planenormal=V3_Cross(mask,B);
								Displacement=-(int)((Planenormal.x*(P2.x-Pivot.x)+Planenormal.y*(P2.y-Pivot.y)+Planenormal.z*(P2.z-Pivot.z))/radtheta*180.0f*180.0f/f);

								AngleAxis2Quaternion(mask.x,mask.y,mask.z,Displacement*(float)radtheta,m.Quaternion);
								SwitchTo2D();
							}
							else if (!View->WireFrame) 
							{
								m.Quaternion.s=1;
								m.Quaternion.v=V3_Make(0,0,0);
							}
							VECTOR3 mask=V3_Make(0,0,0);
							if (hits && View->WireFrame)
							{
								int rot;
								switch (View->k)
								{
								case 0:
									mask.a[buffer[3]]=1;
									if (buffer[3]==0) rot=mx-lx;
									if (buffer[3]==1) rot=mx-lx;
									if (buffer[3]==2) rot=my-ly;
									break;
								case 1:
									mask.a[buffer[3]]=1;
									if (buffer[3]==0) rot=my-ly;
									if (buffer[3]==1) rot=lx-mx;
									if (buffer[3]==2) rot=lx-mx;
									break;
								case 2:
									mask.a[buffer[3]]=1;
									if (buffer[3]==0) rot=my-ly;
									if (buffer[3]==1) rot=mx-lx;
									if (buffer[3]==2) rot=lx-mx;
									break;
								}
								AngleAxis2Quaternion(mask.x,mask.y,mask.z,-rot*(float)radtheta,m.Quaternion);
							}
							//MouseString=new char[30];
							//memset(MouseString,0,30);
							if (View->WireFrame)
							if (mask.a[View->i1]==0)
							_snprintf(MouseString,100,"(%d degrees)\0",mx-lx);
							else
							_snprintf(MouseString,100,"(%d degrees)\0",my-ly);
							if (hits && !View->WireFrame)
							_snprintf(MouseString,100,"(%d degrees)\0",-Displacement);


							/*VECTOR3 v=V3_Make(0,0,0);
							v.a[View->k]=1; if (Interface->ParentItem->InterfaceItemId==2) v.a[View->k]=-1;

							float Angle=-(float)((lx-mx)*radtheta);

							QUATERNION q;
							AngleAxis2Quaternion(v.x,v.y,v.z,Angle,q);

							ActualWorld->ObjectList[i].PosData.Quaternion=Q_Mult(ActualWorld->ObjectList[i].PosData.Quaternion,q);

							MouseString=new char[20];
							memset(MouseString,0,20);
							sprintf(MouseString,"(%d degrees)",mx-lx);*/
						}
						if (Interface->ItemList[1].Interface->ItemList[2].Interface->ItemList[2].ButtonPushed /*&& View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed*/)
						{ // move

							int hits=0;
							unsigned int buffer[120];
							if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[2].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
							{							
								glLineWidth(2);
								glPushAttrib(GL_ALL_ATTRIB_BITS);
								glScissor(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));							
								glEnable(GL_SCISSOR_TEST);
								glColor4f(0,0,0,0);
								glDisable(GL_DEPTH_TEST);
								glEnable(GL_DEPTH_TEST);
								glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
								SetCameraView(&View->Cam,4/3.0f,10,lx,ly);
								glSelectBuffer(120,buffer);
								glRenderMode(GL_SELECT);
								glInitNames();
								glPushName(0);

								VECTOR3 aa,bb;
								aa=View->Cam.Eye;
								if (View->WireFrame)
								for (int x=0; x<3; x++) if (View->k!=x)	aa.a[x]=0;
								bb=Pivot;
								if (View->WireFrame)
								{
									for (int x=0; x<3; x++) if (View->k!=x)	bb.a[x]=0;
									bb.x*=v.x;
									bb.y*=v.y;
									bb.z*=v.z;
								}
								float f=V3_Length(V3_Sub(aa,bb))/10;

								if (v.x)
								{
									glLoadName(0);
									glBegin(GL_LINES);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
									glBegin(GL_TRIANGLES);
									for (int xx=0; xx<12; xx++)
									{
										glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
										glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin(xx*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos(xx*360.0/12.0*radtheta))*v.z);
										glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin((xx+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos((xx+1)*360.0/12.0*radtheta))*v.z);
									}
									glEnd();
								}

								if (v.y)
								{
									glLoadName(1);
									glBegin(GL_LINES);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
									glEnd();
									glBegin(GL_TRIANGLES);
									for (int xx=0; xx<12; xx++)
									{
										glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin(xx*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos(xx*360.0/12.0*radtheta))*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin((xx+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos((xx+1)*360.0/12.0*radtheta))*v.z);
									}
									glEnd();
								}

								if (v.z)
								{
									glLoadName(2);
									glBegin(GL_LINES);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
									glEnd();
									glBegin(GL_TRIANGLES);
									for (int xx=0; xx<12; xx++)
									{
										glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin(xx*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos(xx*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
										glVertex3d((Pivot.x+0.15f*f*sin((xx+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos((xx+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
									}
									glEnd();
								}								
								if ((v.x && v.y && v.z))
								{
									glLoadName(3);
									glBegin(GL_QUADS);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.0f)*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
									glLoadName(4);
									glBegin(GL_QUADS);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
									glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
									glLoadName(5);
									glBegin(GL_QUADS);
									glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
									glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
									glEnd();
								}
																
								hits=glRenderMode(GL_RENDER);
								int smallest=buffer[3];
								int depth=buffer[1];
								for (int k=1; k<hits; k++)
								{
									if (buffer[k*4+1]<(GLuint)depth)
									{
										smallest=buffer[k*4+3];
										depth=buffer[k*4+1];
									}
								}
								buffer[3]=smallest;
								SwitchTo2D();
								glPopAttrib();
								glLineWidth(1);
							}

							VECTOR3 mask=V3_Make(1,1,1);

							float x,y;
							x=((mx-lx))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
							y=(-(my-ly))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
							if (hits)
							{
								mask=V3_Make(0,0,0);
								if (buffer[3]<3)
								mask.a[buffer[3]]=1;
								else
								{
									mask=V3_Make(1,1,1);
									mask.a[buffer[3]-3]=0;
								}
							}

							m.Pos=V3_Make((View->i.x*x+View->j.x*y)*mask.x,(View->i.y*x+View->j.y*y)*mask.y,(View->i.z*x+View->j.z*y)*mask.z);
							if (hits && !View->WireFrame)
							{
								SetCameraView(&View->Cam,4/3.0f,0,0,0);
								VECTOR3 Dir=CalcXYDir((lx-View->x1)/(float)(View->x2-View->x1),
									                  (ly-View->y1)/(float)(View->y2-View->y1),View->Cam.Fov,4/3.0f);
								VECTOR3 Dir2=CalcXYDir((mx-View->x1)/(float)(View->x2-View->x1),(my-View->y1)/(float)(View->y2-View->y1),View->Cam.Fov,4/3.0f);
								float matrix[16];
								MATRIX CameraMatrix;
								M_Identity(CameraMatrix);
								glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
								for (int x=0; x<4; x++)
									for (int y=0; y<4; y++)
										CameraMatrix[y][x]=matrix[x+y*4];

								M_Xform3(CameraMatrix,Dir,Dir);
								M_Xform3(CameraMatrix,Dir2,Dir2);

								VECTOR3 PlaneNormal,PB,D;
								float d;

								switch (buffer[3])
								{
								case 0:
									PlaneNormal=V3_Make(0,1,0);
									PB=V3_Make(0,0,1);
									D=V3_Normalize(V3_Sub(View->Cam.Target,View->Cam.Eye));
									if (fabs(V3_Dot(PB,D))>fabs(V3_Dot(PlaneNormal,D))) 
									{
										PlaneNormal=PB;
										d=Pivot.z;
									}
									else
									d=Pivot.y;
									break;
								case 4:
									PlaneNormal=V3_Make(0,1,0);
									d=Pivot.y;
									break;
								case 1:
									PlaneNormal=V3_Make(1,0,0);
									PB=V3_Make(0,0,1);
									D=V3_Normalize(V3_Sub(View->Cam.Target,View->Cam.Eye));
									if (fabs(V3_Dot(PB,D))>fabs(V3_Dot(PlaneNormal,D))) 
									{
										PlaneNormal=PB;
										d=Pivot.z;
									}
									else
									d=Pivot.x;
									break;
								case 3:
									PlaneNormal=V3_Make(1,0,0);
									d=Pivot.x;
									break;
								case 2:
									PlaneNormal=V3_Make(0,1,0);
									PB=V3_Make(1,0,0);
									D=V3_Normalize(V3_Sub(View->Cam.Target,View->Cam.Eye));
									if (fabs(V3_Dot(PB,D))>fabs(V3_Dot(PlaneNormal,D))) 
									{
										PlaneNormal=PB;
										d=Pivot.x;
									}
									else
									d=Pivot.y;
									break;
								case 5:
									PlaneNormal=V3_Make(0,0,1);
									d=Pivot.z;
									break;
								}

								float t1=distRayPlane(View->Cam.Eye,Dir,PlaneNormal,d);
								float t2=distRayPlane(View->Cam.Eye,Dir2,PlaneNormal,d);

								VECTOR3 P1,P2;

								P1=V3_Add(View->Cam.Eye,V3_Mults(Dir,t1));
								P2=V3_Add(View->Cam.Eye,V3_Mults(Dir2,t2));

								VECTOR3 Displacement=V3_Sub(P2,P1);

								m.Pos=V3_Make((Displacement.x)*mask.x,(Displacement.y)*mask.y,(Displacement.z)*mask.z);
								SwitchTo2D();
							}
							//MouseString=new char[30];
							//memset(MouseString,0,30);
							_snprintf(MouseString,100,"(%.3f, %.3f, %.3f)\0",m.Pos.x,m.Pos.y,m.Pos.z);

							/*float x,y;
							x=((mx-lx))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
							y=(-(my-ly))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);

							ActualWorld->ObjectList[i].PosData.Pos.a[View->i1]+=x*View->i.a[View->i1];
							ActualWorld->ObjectList[i].PosData.Pos.a[View->j1]+=y*View->j.a[View->j1];
							
							MouseString=new char[20];
							memset(MouseString,0,20);
							sprintf(MouseString,"(%.3f,%.3f)",x,y);*/
						}

						if (RightButton) 
						{
							m.Scale=V3_Make(1,1,1);
							m.Pos=V3_Make(0,0,0);
							m.Quaternion.s=1;
							m.Quaternion.v=m.Pos;
							//ActualWorld->ObjectList[i].PosData=ActualWorld->ObjectList[i].PosDataBuffer; //Gyors Undo
						}

						ActualWorld->ObjectList[i].PosData.Pos=V3_Add(ActualWorld->ObjectList[i].PosDataBuffer.Pos,m.Pos);
						ActualWorld->ObjectList[i].PosData.Scale.x=ActualWorld->ObjectList[i].PosDataBuffer.Scale.x*m.Scale.x;
						ActualWorld->ObjectList[i].PosData.Scale.y=ActualWorld->ObjectList[i].PosDataBuffer.Scale.y*m.Scale.y;
						ActualWorld->ObjectList[i].PosData.Scale.z=ActualWorld->ObjectList[i].PosDataBuffer.Scale.z*m.Scale.z;
						ActualWorld->ObjectList[i].PosData.Quaternion=Q_Mult(ActualWorld->ObjectList[i].PosDataBuffer.Quaternion,m.Quaternion);
						RST2Matrix(&ActualWorld->ObjectList[i].PosData,ActualWorld->ObjectList[i].ModelView,1);

					}
				}
			}

		}

	}
};

void DisplayLRotateTool(GUIITEM *View)
{
	VECTOR3 Pivot1,Pivot2,Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot1=Pivot2=V3_Make(0,0,0);
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted && !ActualWorld->ObjectList[x].ListItem->Hidden)
	{
		if (ActualWorld->ObjectList[x].ListItem->Selected)
		{
			Pivot1=V3_Add(Pivot1,ActualWorld->ObjectList[x].PosData.Pos);
			Pivot2=V3_Add(Pivot2,ActualWorld->ObjectList[x].PosDataBuffer.Pos);
			Selected++;
			PCount++;
		}
	}
	if (PCount)
	{
		Pivot1=V3_Mults(Pivot1,1/(float)PCount);
		Pivot2=V3_Mults(Pivot2,1/(float)PCount);
	}

	if (Selected)
	{
		glLineWidth(2);
		VECTOR3 v;
		v.x=View->i.x*View->i.x+View->j.x*View->j.x;
		v.y=View->i.y*View->i.y+View->j.y*View->j.y;
		v.z=View->i.z*View->i.z+View->j.z*View->j.z;
		if (!View->WireFrame) v=V3_Make(1,1,1);

		if (!Transforming)
		SetCameraView(&View->Cam,4/3.0f,10,mx,my);
		else
		SetCameraView(&View->Cam,4/3.0f,10,lx,ly);
		unsigned int buffer[12];
		memset(buffer,0,sizeof(int)*12);
		glSelectBuffer(12,buffer);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);

		if (!Transforming) Pivot=Pivot1;
		else Pivot=Pivot2;

		VECTOR3 aa,bb;
		aa=View->Cam.Eye;
		int x;
		if (View->WireFrame)
		for (x=0; x<3; x++) if (View->k!=x)	aa.a[x]=0;
		bb=Pivot1;
		if (View->WireFrame)
		{
			for (x=0; x<3; x++) if (View->k!=x)	bb.a[x]=0;
			bb.x*=v.x;
			bb.y*=v.y;
			bb.z*=v.z;
		}
		float f=V3_Length(V3_Sub(aa,bb))/10;

		glLoadName(0);
		glBegin(GL_LINE_STRIP);
		for (x=0; x<36*4+1; x++)
			glVertex3d((sin(x*2.5*radtheta)*2*f+Pivot.x)*v.x,(cos(x*2.5*radtheta)*2*f+Pivot.y)*v.y,Pivot.z*v.z);
		glEnd();

		glLoadName(1);
		glBegin(GL_LINE_STRIP);
		for (x=0; x<36*4+1; x++)
			glVertex3d((sin(x*2.5*radtheta)*2*f+Pivot.x)*v.x,Pivot.y*v.y,(cos(x*2.5*radtheta)*2*f+Pivot.z)*v.z);
		glEnd();

		glLoadName(2);
		glBegin(GL_LINE_STRIP);
		for (x=0; x<36*4+1; x++)
			glVertex3d(Pivot.x*v.x,(sin(x*2.5*radtheta)*2*f+Pivot.y)*v.y,(cos(x*2.5*radtheta)*2*f+Pivot.z)*v.z);
		glEnd();
		int hits=glRenderMode(GL_RENDER);
		int smallest=buffer[3];
		int depth=buffer[1];
		for (int k=1; k<hits; k++)
		{
			if (buffer[k*4+1]<(GLuint)depth)
			{
				smallest=buffer[k*4+3];
				depth=buffer[k*4+1];
			}
		}
		buffer[3]=smallest;
		SetCameraView(&View->Cam,4/3.0f,0,mx,my);

		Pivot=Pivot1;		

		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDepthFunc(GL_ALWAYS);

		glColor4d(0.6,0.1,0.1,1);
		if (hits && buffer[3]==0) glColor4d(1,0.2,0.2,1);
		glBegin(GL_LINE_STRIP);
		for (x=0; x<36*4+1; x++)
			glVertex3d((sin(x*2.5*radtheta)*2*f+Pivot.x)*v.x,(cos(x*2.5*radtheta)*2*f+Pivot.y)*v.y,Pivot.z*v.z);
		glEnd();

		glColor4d(0.1,0.6,0.1,1);
		if (hits && buffer[3]==1) glColor4d(0.2,1,0.2,1);
		glBegin(GL_LINE_STRIP);
		for (x=0; x<36*4+1; x++)
			glVertex3d((sin(x*2.5*radtheta)*2*f+Pivot.x)*v.x,Pivot.y*v.y,(cos(x*2.5*radtheta)*2*f+Pivot.z)*v.z);
		glEnd();

		glColor4d(0.1,0.1,0.6,1);
		if (hits && buffer[3]==2) glColor4d(0.2,0.2,1,1);
		glBegin(GL_LINE_STRIP);
		for (x=0; x<36*4+1; x++)
			glVertex3d(Pivot.x*v.x,(sin(x*2.5*radtheta)*2*f+Pivot.y)*v.y,(cos(x*2.5*radtheta)*2*f+Pivot.z)*v.z);
		glEnd();

		glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glLineWidth(1);
	}
}

void DisplayLMoveTool(GUIITEM *View)
{
	VECTOR3 Pivot1,Pivot2,Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot1=Pivot2=V3_Make(0,0,0);
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted && !ActualWorld->ObjectList[x].ListItem->Hidden)
	{
		if (ActualWorld->ObjectList[x].ListItem->Selected)
		{
			Pivot1=V3_Add(Pivot1,ActualWorld->ObjectList[x].PosData.Pos);
			Pivot2=V3_Add(Pivot2,ActualWorld->ObjectList[x].PosDataBuffer.Pos);
			Selected++;
			PCount++;
		}
	}
	if (PCount)
	{
		Pivot1=V3_Mults(Pivot1,1/(float)PCount);
		Pivot2=V3_Mults(Pivot2,1/(float)PCount);
	}

	if (Selected)
	{
		glLineWidth(2);
		VECTOR3 v;
		v.x=View->i.x*View->i.x+View->j.x*View->j.x;
		v.y=View->i.y*View->i.y+View->j.y*View->j.y;
		v.z=View->i.z*View->i.z+View->j.z*View->j.z;
		if (!View->WireFrame) v=V3_Make(1,1,1);

		if (!Transforming)
		SetCameraView(&View->Cam,4/3.0f,10,mx,my);
		else
		SetCameraView(&View->Cam,4/3.0f,10,lx,ly);
		unsigned int buffer[12];
		memset(buffer,0,sizeof(int)*12);
		glSelectBuffer(12,buffer);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);

		if (!Transforming) Pivot=Pivot1;
		else Pivot=Pivot2;

		VECTOR3 aa,bb;
		aa=View->Cam.Eye;
		int x;
		if (View->WireFrame)
		for (x=0; x<3; x++) if (View->k!=x)	aa.a[x]=0;
		bb=Pivot1;
		if (View->WireFrame)
		{
			for (x=0; x<3; x++) if (View->k!=x)	bb.a[x]=0;
			bb.x*=v.x;
			bb.y*=v.y;
			bb.z*=v.z;
		}
		float f=V3_Length(V3_Sub(aa,bb))/10;

		glLoadName(0);
		if (v.x)
		{
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (int x=0; x<12; x++)
			{
				glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+f*0.15f*sin(x*360.0/12.0*radtheta))*v.y,(Pivot.z+f*0.15f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+f*0.15f*sin((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+f*0.15f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		glLoadName(1);
		if (v.y)
		{
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+f*0.15f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+f*0.15f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		glLoadName(2);
		if (v.z)
		{
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+f*0.15f*cos(x*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+f*0.15f*cos((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
			}
			glEnd();
		}
		glLoadName(3);
		if ((v.x && v.y && v.z) || !v.x)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}
		glLoadName(4);
		if ((v.x && v.y && v.z) || !v.y)
		{
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}
		glLoadName(5);
		if ((v.x && v.y && v.z) || !v.z)
		{
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x)*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}
		
		int hits=glRenderMode(GL_RENDER);
		int smallest=buffer[3];
		int depth=buffer[1];
		for (int k=1; k<hits; k++)
		{
			if (buffer[k*4+1]<(GLuint)depth)
			{
				smallest=buffer[k*4+3];
				depth=buffer[k*4+1];
			}
		}
		buffer[3]=smallest;
		SetCameraView(&View->Cam,4/3.0f,0,mx,my);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glDepthMask(false);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Pivot=Pivot1;		

		if (v.x)
		{
			glColor4d(0.6,0.1,0.1,1); if (hits && buffer[3]==0) glColor4d(1,0.2,0.2,1);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,1); if (hits && buffer[3]==0) glColor4d(0.3,0.1,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+f*0.15f*sin(x*360.0/12.0*radtheta))*v.y,(Pivot.z+f*0.15f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+f*0.15f*sin((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+f*0.15f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		if (v.y)
		{
			glColor4d(0.1,0.6,0.1,1); if (hits && buffer[3]==1) glColor4d(0.2,1,0.2,1);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,1); if (hits && buffer[3]==1) glColor4d(0.1,0.3,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+f*0.15f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+f*0.15f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		if (v.z)
		{
			glColor4d(0.1,0.1,0.6,1); if (hits && buffer[3]==2) glColor4d(0.2,0.2,1,1);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,1); if (hits && buffer[3]==2) glColor4d(0.1,0.1,0.3,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+f*0.15f*cos(x*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
				glVertex3d((Pivot.x+f*0.15f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+f*0.15f*cos((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
			}
			glEnd();
		}

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glLineWidth(1.5);

		if ((v.x && v.y && v.z) || !v.x)
		{
			glColor4d(0.6,0.1,0.1,1); if (hits && buffer[3]==3) glColor4d(1,0.2,0.2,1);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,1); if (hits && buffer[3]==3) glColor4d(0.3,0.1,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glColor4d(0.1,0.6,0.1,1); if (hits && buffer[3]==4) glColor4d(0.2,1,0.2,1);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,1); if (hits && buffer[3]==4) glColor4d(0.1,0.3,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x)*v.x,(Pivot.y)*v.y,(Pivot.z+0.5f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glColor4d(0.1,0.1,0.6,1); if (hits && buffer[3]==5) glColor4d(0.2,0.2,1,1);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,1); if (hits && buffer[3]==5) glColor4d(0.1,0.1,0.3,1); }
			glBegin(GL_LINES);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glEnd();
		}

		if ((v.x && v.y && v.z) || !v.x)
		{
			glColor4d(0.6,0.1,0.1,0.5); if (hits && buffer[3]==3) glColor4d(1,0.2,0.2,0.5);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,0.5); if (hits && buffer[3]==3) glColor4d(0.3,0.1,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glColor4d(0.1,0.6,0.1,0.5); if (hits && buffer[3]==4) glColor4d(0.2,1,0.2,0.5);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,0.5); if (hits && buffer[3]==4) glColor4d(0.1,0.3,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x)*v.x,(Pivot.y)*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glColor4d(0.1,0.1,0.6,0.5); if (hits && buffer[3]==5) glColor4d(0.2,0.2,1,0.5);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,0.5); if (hits && buffer[3]==5) glColor4d(0.1,0.1,0.3,0.5); }
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		glLineWidth(2);
		glDisable(GL_BLEND);

		glDepthMask(true);
		glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glLineWidth(1);
	}
}

void DisplayLScaleTool(GUIITEM *View)
{
	VECTOR3 Pivot1,Pivot2,Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot1=Pivot2=V3_Make(0,0,0);
	for (int x=0; x<ActualWorld->ObjectNum; x++)
	if (!ActualWorld->ObjectList[x].Deleted && !ActualWorld->ObjectList[x].ListItem->Hidden)
	{
		if (ActualWorld->ObjectList[x].ListItem->Selected)
		{
			Pivot1=V3_Add(Pivot1,ActualWorld->ObjectList[x].PosData.Pos);
			Pivot2=V3_Add(Pivot2,ActualWorld->ObjectList[x].PosDataBuffer.Pos);
			Selected++;
			PCount++;
		}
	}
	if (PCount)
	{
		Pivot1=V3_Mults(Pivot1,1/(float)PCount);
		Pivot2=V3_Mults(Pivot2,1/(float)PCount);
	}

	if (Selected)
	{
		glLineWidth(2);
		VECTOR3 v;
		v.x=View->i.x*View->i.x+View->j.x*View->j.x;
		v.y=View->i.y*View->i.y+View->j.y*View->j.y;
		v.z=View->i.z*View->i.z+View->j.z*View->j.z;
		if (!View->WireFrame) v=V3_Make(1,1,1);

		if (!Transforming)
		SetCameraView(&View->Cam,4/3.0f,10,mx,my);
		else
		SetCameraView(&View->Cam,4/3.0f,10,lx,ly);
		unsigned int buffer[12];
		memset(buffer,0,sizeof(int)*12);
		glSelectBuffer(12,buffer);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);

		if (!Transforming) Pivot=Pivot1;
		else Pivot=Pivot2;

		VECTOR3 aa,bb;
		aa=View->Cam.Eye;
		int x;
		if (View->WireFrame)
		for (x=0; x<3; x++) if (View->k!=x)	aa.a[x]=0;
		bb=Pivot1;
		if (View->WireFrame)
		{
			for (x=0; x<3; x++) if (View->k!=x)	bb.a[x]=0;
			bb.x*=v.x;
			bb.y*=v.y;
			bb.z*=v.z;
		}
		float f=V3_Length(V3_Sub(aa,bb))/10;

		glLoadName(0);
		if (v.x)
		{
			glBegin(GL_LINES);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (int x=0; x<12; x++)
			{
				glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin(x*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		glLoadName(1);
		if (v.y)
		{
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		glLoadName(2);
		if (v.z)
		{
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos(x*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
			}
			glEnd();
		}
		if (!Transforming)
		SetCameraView(&View->Cam,4/3.0f,1,mx,my);
		else
		SetCameraView(&View->Cam,4/3.0f,1,lx,ly);
		glLoadName(3);
		if ((v.x && v.y && v.z) || !v.x)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glEnd();
		}
		glLoadName(4);
		if ((v.x && v.y && v.z) || !v.y)
		{
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glEnd();
		}
		glLoadName(5);
		if ((v.x && v.y && v.z) || !v.z)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		glLoadName(6);
		if ((v.x && v.y && v.z) || !v.x)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glEnd();
		}
		glLoadName(7);
		if ((v.x && v.y && v.z) || !v.y)
		{
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glEnd();
		}
		glLoadName(8);
		if ((v.x && v.y && v.z) || !v.z)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		glLoadName(9);
		if ((v.x && v.y && v.z) || !v.x)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}
		
		int hits=glRenderMode(GL_RENDER);
		int smallest=buffer[3];
		int depth=buffer[1];
		for (int k=1; k<hits; k++)
		{
			if (buffer[k*4+1]<(GLuint)depth)
			{
				smallest=buffer[k*4+3];
				depth=buffer[k*4+1];
			}
		}
		buffer[3]=smallest;
		SetCameraView(&View->Cam,4/3.0f,0,mx,my);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glDepthMask(false);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Pivot=Pivot1;		

		if (v.x)
		{
			glColor4d(0.6,0.1,0.1,1); if (hits && buffer[3]==0) glColor4d(1,0.2,0.2,1);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,1); if (hits && buffer[3]==0) glColor4d(0.3,0.1,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f((Pivot.x+2.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin(x*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+1.7f*f)*v.x,(Pivot.y+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		if (v.y)
		{
			glColor4d(0.1,0.6,0.1,1); if (hits && buffer[3]==1) glColor4d(0.2,1,0.2,1);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,1); if (hits && buffer[3]==1) glColor4d(0.1,0.3,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,(Pivot.y+2.0f*f)*v.y,Pivot.z*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos(x*360.0/12.0*radtheta))*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+1.7f*f)*v.y,(Pivot.z+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.z);
			}
			glEnd();
		}

		if (v.z)
		{
			glColor4d(0.1,0.1,0.6,1); if (hits && buffer[3]==2) glColor4d(0.2,0.2,1,1);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,1); if (hits && buffer[3]==2) glColor4d(0.1,0.1,0.3,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
			glEnd();
			glBegin(GL_TRIANGLES);
			for (x=0; x<12; x++)
			{
				glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+2.0f*f)*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin(x*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos(x*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
				glVertex3d((Pivot.x+0.15f*f*sin((x+1)*360.0/12.0*radtheta))*v.x,(Pivot.y+0.15f*f*cos((x+1)*360.0/12.0*radtheta))*v.y,(Pivot.z+1.7f*f)*v.z);
			}
			glEnd();
		}

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glLineWidth(1.5);

		if ((v.x && v.y && v.z) || !v.x)
		{
			glColor4d(0.6,0.1,0.1,1); if (hits && buffer[3]==3) glColor4d(1,0.2,0.2,1);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,1); if (hits && buffer[3]==3) glColor4d(0.3,0.1,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glColor4d(0.1,0.6,0.1,1); if (hits && buffer[3]==4) glColor4d(0.2,1,0.2,1);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,1); if (hits && buffer[3]==4) glColor4d(0.1,0.3,0.1,1); }
			glBegin(GL_LINES);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glColor4d(0.1,0.1,0.6,1); if (hits && buffer[3]==5) glColor4d(0.2,0.2,1,1);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,1); if (hits && buffer[3]==5) glColor4d(0.1,0.1,0.3,1); }
			glBegin(GL_LINES);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		if ((v.x && v.y && v.z) || !v.x)
		{
			glColor4d(0.6,0.1,0.1,0.5); if (hits && buffer[3]==3) glColor4d(1,0.2,0.2,0.5);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,0.5); if (hits && buffer[3]==3) glColor4d(0.3,0.1,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glColor4d(0.1,0.6,0.1,0.5); if (hits && buffer[3]==4) glColor4d(0.2,1,0.2,0.5);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,0.5); if (hits && buffer[3]==4) glColor4d(0.1,0.3,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glColor4d(0.1,0.1,0.6,0.5); if (hits && buffer[3]==5) glColor4d(0.2,0.2,1,0.5);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,0.5); if (hits && buffer[3]==5) glColor4d(0.1,0.1,0.3,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		if ((v.x && v.y && v.z) || !v.x)
		{
			glColor4d(0.6,0.1,0.1,0.5); if (hits && buffer[3]==6) glColor4d(1,0.2,0.2,0.5);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,0.5); if (hits && buffer[3]==6) glColor4d(0.3,0.1,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glColor4d(0.1,0.6,0.1,0.5); if (hits && buffer[3]==7) glColor4d(0.2,1,0.2,0.5);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,0.5); if (hits && buffer[3]==7) glColor4d(0.1,0.3,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+1.0f*f)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glColor4d(0.1,0.1,0.6,0.5); if (hits && buffer[3]==8) glColor4d(0.2,0.2,1,0.5);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,0.5); if (hits && buffer[3]==8) glColor4d(0.1,0.1,0.3,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.0f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+1.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+1.0f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		if ((v.x && v.y && v.z) || !v.x)
		{
			glColor4d(0.6,0.1,0.1,0.5); if (hits && buffer[3]==9) glColor4d(1,0.2,0.2,0.5);
			//if (z==1) {	glColor4d(0.2,0.05,0.05,0.5); if (hits && buffer[3]==9) glColor4d(0.3,0.1,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.y)
		{
			glColor4d(0.1,0.6,0.1,0.5); if (hits && buffer[3]==9) glColor4d(0.2,1,0.2,0.5);
			//if (z==1) {	glColor4d(0.05,0.2,0.05,0.5); if (hits && buffer[3]==9) glColor4d(0.1,0.3,0.1,0.5); }
			glBegin(GL_QUADS);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z+0.5f*f)*v.z);
			glVertex3f(Pivot.x*v.x,Pivot.y*v.y,(Pivot.z)*v.z);
			glEnd();
		}
		if ((v.x && v.y && v.z) || !v.z)
		{
			glColor4d(0.1,0.1,0.6,0.5); if (hits && buffer[3]==9) glColor4d(0.2,0.2,1,0.5);
			//if (z==1) {	glColor4d(0.05,0.05,0.2,0.5); if (hits && buffer[3]==9) glColor4d(0.1,0.1,0.3,0.5); }
			glBegin(GL_QUADS);
			glVertex3f(Pivot.x*v.x,(Pivot.y)*v.y,Pivot.z*v.z);
			glVertex3f(Pivot.x*v.x,(Pivot.y+0.5f*f)*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x+0.5f*f)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glVertex3f((Pivot.x)*v.x,Pivot.y*v.y,Pivot.z*v.z);
			glEnd();
		}

		glLineWidth(2);
		glDisable(GL_BLEND);

		glDepthMask(true);
		glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glLineWidth(1);
	}
}


void DisplayL3DView(GUIITEM *View)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glScissor(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
	glEnable(GL_SCISSOR_TEST);
	glColor4f(0,0,0,0);
	glDisable(GL_DEPTH_TEST);
	quad(View->x1,View->y1,View->x2,View->y2);
	glEnable(GL_DEPTH_TEST);
	glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
	SetCameraView(&View->Cam,4/3.0f,0,mx,my);
	glColor4f(1,1,1,1);

	float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
	float XSize=CalcXGLScene(z, View->Cam.Fov, 4/3.0f)/2.0f;
	float YSize=CalcYGLScene(z, View->Cam.Fov, 4/3.0f)/2.0f;

	if (View->ParentInterface->ItemList[1].Interface->ItemList[5].ButtonPushed)
	{
		if (View->WireFrame)
		{
			glBegin(GL_LINES);

			VECTOR3 v2;

			for (int z=0; z<3; z++)
			{
				float x;
				for (x=(float)((int)(View->Cam.Target.a[View->i1]-XSize-0.5)); x<View->Cam.Target.a[View->i1]+XSize+0.5; x+=0.5)
				{
					int y = abs((int)(x*2)+1) % 2;
					if ((z==1 && y==1 && x!=0) || (z==2 && y==0 && x!=0) || (z==0 && x==0))
					{
						v2.x=(float)(0.1*(y+1.0));
						v2.y=(float)(0.15*(y+1.0));
						v2.z=(float)(0.25*(y+1.0));

						if (x==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.x,v2.y,v2.z,1);
						float a=View->Cam.Target.a[View->j1]-YSize;
						float b=View->Cam.Target.a[View->j1]+YSize;

						glVertex3f(x*View->i.x*View->i.a[View->i1]+a*View->j.x*View->j.a[View->j1],
								   x*View->i.y*View->i.a[View->i1]+a*View->j.y*View->j.a[View->j1],
								   x*View->i.z*View->i.a[View->i1]+a*View->j.z*View->j.a[View->j1]);
						glVertex3f(x*View->i.x*View->i.a[View->i1]+b*View->j.x*View->j.a[View->j1],
								   x*View->i.y*View->i.a[View->i1]+b*View->j.y*View->j.a[View->j1],
								   x*View->i.z*View->i.a[View->i1]+b*View->j.z*View->j.a[View->j1]);
					}
				}

				for (x=(float)((int)(View->Cam.Target.a[View->j1]-YSize-0.5)); x<View->Cam.Target.a[View->j1]+YSize+0.5; x+=0.5)
				{
					int y = abs((int)(x*2)+1) % 2;
					if ((z==1 && y==1 && x!=0) || (z==2 && y==0 && x!=0) || (z==0 && x==0))
					{
						v2.x=(float)(0.1*(y+1.0));
						v2.y=(float)(0.15*(y+1.0));
						v2.z=(float)(0.25*(y+1.0));

						if (x==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.x,v2.y,v2.z,1);
						float a=View->Cam.Target.a[View->i1]-XSize;
						float b=View->Cam.Target.a[View->i1]+XSize;

						glVertex3f(x*View->j.x*View->j.a[View->j1]+a*View->i.x*View->i.a[View->i1],
								   x*View->j.y*View->j.a[View->j1]+a*View->i.y*View->i.a[View->i1],
								   x*View->j.z*View->j.a[View->j1]+a*View->i.z*View->i.a[View->i1]);
						glVertex3f(x*View->j.x*View->j.a[View->j1]+b*View->i.x*View->i.a[View->i1],
								   x*View->j.y*View->j.a[View->j1]+b*View->i.y*View->i.a[View->i1],
								   x*View->j.z*View->j.a[View->j1]+b*View->i.z*View->i.a[View->i1]);
					}
				}
			}

			glEnd();
		}
		else glCallList(View->CamGrid);
	}

	if (ActualWorld) ActualWorld->CalculateObjectMatrices();

	if (ActualWorld && !View->WireFrame) ActualWorld->Render(false,false);

	VECTOR3 v;
	v.x=View->i.x*View->i.x+View->j.x*View->j.x;
	v.y=View->i.y*View->i.y+View->j.y*View->j.y;
	v.z=View->i.z*View->i.z+View->j.z*View->j.z;
	if (View->WireFrame && ActualWorld)
		ActualWorld->RenderWireFrame(v,false);

	MATRIX m;
	M_Identity(m);
	if (View->ParentInterface->ItemList[1].Interface->ItemList[6].ButtonPushed && ActualWorld) ActualWorld->RenderParticleTree(&View->Cam,ActualAnim,&m);
	

	int Mask[3]; Mask[0]=1; Mask[1]=1; Mask[2]=1;
	Mask[View->k]=0;

	glDepthFunc(GL_ALWAYS);
	if (View->WireFrame && ActualWorld)
	{
		glBegin(GL_LINES);
		glColor4f(1,0,0,1);
		glVertex3f(ActualWorld->Camera.Eye.a[0]*v.x,
				   ActualWorld->Camera.Eye.a[1]*v.y,
				   ActualWorld->Camera.Eye.a[2]*v.z);
		glColor4f(1,1,0,1);
		glVertex3f(ActualWorld->Camera.Target.a[0]*v.x,
				   ActualWorld->Camera.Target.a[1]*v.y,
				   ActualWorld->Camera.Target.a[2]*v.z);
		glEnd();
	}
	glColor4f(1,1,1,1);
	if (View->WireFrame && ActualWorld)
		for (int x=0; x<8; x++)
			if (ActualWorld->Lights[x].Lit)
			{
				if (ActualWorld->Lights[x].Spot_Cutoff==180 && ActualWorld->Lights[x].Position[3]==0) //dirlight
				{
					glBegin(GL_LINES);
					glVertex3f(View->Cam.Eye.a[0]*v.x,
							   View->Cam.Eye.a[1]*v.y,
							   View->Cam.Eye.a[2]*v.z);
					glVertex3f((ActualWorld->Lights[x].Position[0]+View->Cam.Eye.a[0])*v.x,
							   (ActualWorld->Lights[x].Position[1]+View->Cam.Eye.a[1])*v.y,
							   (ActualWorld->Lights[x].Position[2]+View->Cam.Eye.a[2])*v.z);
					glEnd();
				}

				if (ActualWorld->Lights[x].Spot_Cutoff==180 && ActualWorld->Lights[x].Position[3]==1) //dotlight
				{
					glBegin(GL_LINES);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glEnd();
				}

				if (ActualWorld->Lights[x].Position[3]==1 && ActualWorld->Lights[x].Spot_Cutoff!=180) //spotlight
				{
					glBegin(GL_LINES);
					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualWorld->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualWorld->Lights[x].Position[0]*v.x,
							   ActualWorld->Lights[x].Position[1]*v.y,
							   ActualWorld->Lights[x].Position[2]*v.z);
					glVertex3f((ActualWorld->Lights[x].Position[0]+ActualWorld->Lights[x].Spot_Direction[0])*v.x,
							   (ActualWorld->Lights[x].Position[1]+ActualWorld->Lights[x].Spot_Direction[1])*v.y,
							   (ActualWorld->Lights[x].Position[2]+ActualWorld->Lights[x].Spot_Direction[2])*v.z);
					glEnd();
				}
			}

	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	if (ActualWorld && MainLayouterScroller->ParentItem->SelectedItem==3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8)
	{
		if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[1].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
		DisplayLRotateTool(View);
		if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[2].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
		DisplayLMoveTool(View);
		if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[0].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
		DisplayLScaleTool(View);
	}
	SwitchTo2D();
	if (View->WireFrame)
	{
		glColor4f(highblue);
		glBegin(GL_LINES);
		glVertex2f(View->x1+7.0f,View->y2-7.0f);
		glVertex2f(View->x1+50.0f,View->y2-7.0f);
		glVertex2f(View->x1+7.0f,View->y2-7.0f);
		glVertex2f(View->x1+7.0f,View->y2-50.0f);
		glEnd();
		glRasterPos2i(View->x1+10,View->y2-43);
		glPrint(DirNames[View->j1],base);
		glRasterPos2i(View->x1+43,View->y2-10);
		glPrint(DirNames[View->i1],base);
	}
	glColor4f(0,0,0,0);
	if (View->WireFrame)
	glRasterPos2i(View->x1+31,View->y1+12);
	else
	glRasterPos2i(View->x1+31+19,View->y1+12);
	glPrint(View->Text,base);
	glColor4f(1,1,1,1);
	if (View->WireFrame)
	glRasterPos2i(View->x1+30,View->y1+11);
	else
	glRasterPos2i(View->x1+30+19,View->y1+11);
	glPrint(View->Text,base);

	if (!View->WireFrame)
	{
		char text[100];
		sprintf(text,"VCOUNT: %.6d, PCOUNT: %.6d, CCOUNT: %.6d\0",VertexCounter,PolyCounter,CallCounter);
		glRasterPos2i(View->x1+5,View->y2-4);
		glPrint(text,base);
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(highblue);
	rectangle(View->x1,View->y1,View->x2,View->y2);
	glColor4f(1,1,1,1);
	glDepthFunc(GL_LESS);
	glDisable(GL_SCISSOR_TEST);
	glAlphaFunc(GL_GEQUAL,0);
	//glPopAttrib();
};

void __stdcall GUI_LMiddleClickFollowCursor(cINTERFACE* Interface, int ButtonId)
{
	GUIITEM *View=&Interface->ItemList[ButtonId];
	if (Interface->ItemList[ButtonId].WireFrame)
	{
		if (RightButton) {mx=mbx; my=mby;}
		float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
		float XSize=CalcXGLScene(z, View->Cam.Fov, 4/3.0f)/(float)(View->x2-View->x1)*(mby-my);
		float YSize=CalcYGLScene(z, View->Cam.Fov, 4/3.0f)/(float)(View->y2-View->y1)*(mbx-mx);
		VECTOR3 i1,j1;
		i1=V3_Mults(View->i,YSize);
		j1=V3_Mults(View->j,-XSize);
		View->Cam.Eye=V3_Add(V3_Add(View->BCam.Eye,i1),j1);
		View->Cam.Target=V3_Add(V3_Add(View->BCam.Target,i1),j1);
	}
	else
	if (ActualWorld)
	{
		if (RightButton) {mx=mbx; my=mby;}
		if (MidShift)
		{

			View->Cam.Eye=V3_Sub(View->BCam.Eye,View->BCam.Target);
			//View->Cam.Up=View->BCam.Up;
			//View->Cam.Target=V3_Sub(View->Cam.Target,View->BCam.Target);

			MATRIX m;
			
			M_Rotate(View->Cam.Up.x,View->Cam.Up.y,View->Cam.Up.z,-(float)((mx-mbx)*radtheta),m);
			M_Xform3(m,View->Cam.Eye,View->Cam.Eye);

			VECTOR3 Axis2=V3_Cross(View->Cam.Up,View->Cam.Eye);
			
			M_Rotate(Axis2.x,Axis2.y,Axis2.z,-(float)((my-mby)*radtheta),m);
			M_Xform3(m,View->Cam.Eye,View->Cam.Eye);
			
			//M_Xform3(m,View->Cam.Up,View->Cam.Up);

			View->Cam.Eye=V3_Add(View->Cam.Eye,View->BCam.Target);
			//View->Cam.Target=V3_Add(View->Cam.Target,View->BCam.Target);

		}
		else
		{
			VECTOR3 Displace=V3_Make(0,0,0);
			Displace=V3_Add(Displace,V3_Mults(View->Cam.i,(mbx-mx)/10.0f));
			Displace=V3_Add(Displace,V3_Mults(View->Cam.j,(mby-my)/10.0f));
			View->Cam.Target=V3_Add(View->BCam.Target,Displace);
			View->Cam.Eye=V3_Add(View->BCam.Eye,Displace);
		}
	
	}
};

void __stdcall GUI_L3dViewCheck(cINTERFACE* Interface, int ButtonId)
{
	GUIITEM *View=&Interface->ItemList[ButtonId];
	if (Interface->ItemList[ButtonId].WireFrame)
	{
		if (View->MouseInWindow)
		{
			float s=-View->Cam.Eye.a[View->k]/(float)fabs(View->Cam.Eye.a[View->k]);
			if ((View->Cam.Eye.a[View->k]+Wheel*s)/View->Cam.Eye.a[View->k]>0)
			View->Cam.Eye.a[View->k]+=Wheel*s;
			if ((View->Cam.Eye.a[View->k]+1*s)/View->Cam.Eye.a[View->k]>0)
			if (Keys['j']) View->Cam.Eye.a[View->k]+=1*s;			
			if (Keys['m']) View->Cam.Eye.a[View->k]-=1*s;
		}
		if (!View->MidClickInWindow) View->BCam=View->Cam;
	}
	else
	{
		if (View->MouseInWindow && ActualWorld)
		{
			VECTOR3 Displace=V3_Mults( V3_Normalize( V3_Sub(View->Cam.Target,View->Cam.Eye) ) ,Wheel*0.5f);
			if (V3_Length(View->Cam.Eye)+V3_Length(V3_Add(View->Cam.Eye,Displace))>1 || Wheel<0)
			View->Cam.Eye=V3_Add(View->Cam.Eye,Displace);

			if (!View->MidClickInWindow) View->BCam=View->Cam;
		}
		if (ActualWorld)
		ActualWorld->Camera=LayouterDisplay4->ParentItem->Cam;
	}
};

void __stdcall GUI_L3dRightClick(cINTERFACE* Interface, int ButtonId)
{
	GUIITEM *View=&Interface->ItemList[ButtonId];
	float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
	if (Interface->ItemList[ButtonId].WireFrame)
	{
		if (
			!(rx<=Interface->ItemList[ButtonId].x1+19 && ry<Interface->ItemList[ButtonId].y1+19) &&
			!(rx>=Interface->ItemList[ButtonId].x2-19*4 && ry<Interface->ItemList[ButtonId].y1+19) &&
			!(rx>=Interface->ItemList[ButtonId].x2-19 && ry<Interface->ItemList[ButtonId].y1+19*3))
		{
			if (ActualWorld)
			{
				if (MainLayouterScroller->ParentItem->SelectedItem==1) //camera transform
				{
					float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
					float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
					LayouterDisplay4->ParentItem->Cam.Target.a[View->i1]=xx*View->i.a[View->i1]+View->Cam.Target.a[View->i1];
					LayouterDisplay4->ParentItem->Cam.Target.a[View->j1]=yy*View->j.a[View->j1]+View->Cam.Target.a[View->j1];
				}
				if (MainLayouterScroller->ParentItem->SelectedItem==2) //light transform
				{
					ITEM *a=ActualWorld->ItemList->List->Next->Next->SubItems;
					for (int x=0; x<8; x++)
					{
						if (a->Selected && ActualWorld->Lights[x].Lit)
						{
							if (ActualWorld->Lights[x].Position[3]==1 && ActualWorld->Lights[x].Spot_Cutoff!=180) //spotlight
							{
								float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
								float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
								ActualWorld->Lights[x].Spot_Direction[View->i1]=xx*View->i.a[View->i1]+View->Cam.Eye.a[View->i1]-ActualWorld->Lights[x].Position[View->i1];
								ActualWorld->Lights[x].Spot_Direction[View->j1]=yy*View->j.a[View->j1]+View->Cam.Eye.a[View->j1]-ActualWorld->Lights[x].Position[View->j1];
							}
						}
						a=a->Next;
					}
				}
				if (MainLayouterScroller->ParentItem->SelectedItem==3) //particle dir transform
				{
					float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
					float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
					for (int x=0; x<ActualWorld->ObjectNum; x++)
						if (ActualWorld->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CUBEEMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || ActualWorld->ObjectList[x].Primitive==aDDict_SPHERESLOWER)
							if (ActualWorld->ObjectList[x].ListItem->Selected)
							{
								ActualWorld->ObjectList[x].EmitterData->Dir.a[View->i1]=xx*View->i.a[View->i1]-ActualWorld->ObjectList[x].PosData.Pos.a[View->i1]+View->Cam.Target.a[View->i1];
								ActualWorld->ObjectList[x].EmitterData->Dir.a[View->j1]=yy*View->j.a[View->j1]-ActualWorld->ObjectList[x].PosData.Pos.a[View->j1]+View->Cam.Target.a[View->j1];
							}
				}
			}
		}
	}
}