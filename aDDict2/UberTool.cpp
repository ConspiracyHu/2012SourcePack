#include "UberTool.h"
#include "aDDict2.h"

int GetPivot(OBJECT *Obj, VECTOR3 &Pivot, VECTOR3 ParentPos)
{
	if (Obj->Primitive!=aDDict_CLONE)
	{
		Pivot=V3_Add(V3_Add(Pivot,V3_Make(Obj->ModelView[0][3],
					                      Obj->ModelView[1][3],
							              Obj->ModelView[2][3])),ParentPos);
		return 1;
	}
	else
	{
		int Cnt=0;
		for (int x=0; x<Obj->Param1; x++)
		{
			Cnt+=GetPivot( (OBJECT*)(((int*)Obj->Param3)[x]), Pivot, V3_Add(V3_Make(Obj->ModelView[0][3],
					                                                                Obj->ModelView[1][3],
							                                                        Obj->ModelView[2][3]),ParentPos));
		}
		return Cnt;
	}

	return 0;
}

int GetPivot2(OBJECT *Obj, VECTOR3 &Pivot, VECTOR3 ParentPos)
{
	if (Obj->Primitive!=aDDict_CLONE)
	{
		Pivot=V3_Add(V3_Add(Pivot,V3_Make(Obj->TransformBuffer[0][3],
					                      Obj->TransformBuffer[1][3],
							              Obj->TransformBuffer[2][3])),ParentPos);
		return 1;
	}
	else
	{
		int Cnt=0;
		for (int x=0; x<Obj->Param1; x++)
		{
			Cnt+=GetPivot2( (OBJECT*)(((int*)Obj->Param3)[x]), Pivot, V3_Add(V3_Make(Obj->TransformBuffer[0][3],
					                                                                 Obj->TransformBuffer[1][3],
							                                                         Obj->TransformBuffer[2][3]),ParentPos));
		}
		return Cnt;
	}

	return 0;
}

void __stdcall GUI_3dLeftClick(cINTERFACE* Interface, int ButtonId)
{
	VECTOR3 ScaleVector;
	VECTOR3 RotateAxis; float RotateAngle;
	VECTOR3 TransformVector;
	WriteDebug("GUI_3dLeftClick\n");
	GUIITEM *View=&Interface->ItemList[ButtonId];

	VECTOR3 Min=V3_Make(0,0,0),Max=V3_Make(0,0,0);
	VECTOR3 Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot=V3_Make(0,0,0);
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			PCount+=GetPivot(&ActualScene->ObjectList[x],Pivot,V3_Make(0,0,0));
			Selected++;

			TestScaleFactors(Min,Max,V3_Make( 1, 1, 1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make( 1,-1, 1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make( 1,-1,-1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make( 1, 1,-1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1, 1, 1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1,-1, 1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1,-1,-1),ActualScene->ObjectList[x].ModelView);
			TestScaleFactors(Min,Max,V3_Make(-1, 1,-1),ActualScene->ObjectList[x].ModelView);

		}
	}
	if (PCount) Pivot=V3_Mults(Pivot,1/(float)PCount);

	VECTOR3 ScaleFactors=V3_Sub(Max,Min);

	if (!View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed) Pivot=V3_Make(0,0,0);

	VECTOR3 v;
	v.x=View->i.x*View->i.x+View->j.x*View->j.x;
	v.y=View->i.y*View->i.y+View->j.y*View->j.y;
	v.z=View->i.z*View->i.z+View->j.z*View->j.z;
	if (!View->WireFrame) v=V3_Make(1,1,1);

	float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
	//if (Interface->ItemList[ButtonId].WireFrame)
	if (
		!(lx<=Interface->ItemList[ButtonId].x1+19 && ly<Interface->ItemList[ButtonId].y1+19) &&
		!(lx>=Interface->ItemList[ButtonId].x2-19*4 && ly<Interface->ItemList[ButtonId].y1+19) &&
		!(lx>=Interface->ItemList[ButtonId].x2-19 && ly<Interface->ItemList[ButtonId].y1+19*3))
	{
		if (ActualScene)
		{
			if (!Interface->ItemList[ButtonId].WireFrame)
			{
				if (PolySelect)
				{
					WaitLeftButton=true;

					glPushAttrib(GL_ALL_ATTRIB_BITS);
					glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
					SetCameraView(&View->Cam,4/3.0f,1,mx,my);

					unsigned int *buffer=new unsigned int[4*ActualScene->ObjectList[PolySelectObject].PolygonNum];

					glSelectBuffer(4*ActualScene->ObjectList[PolySelectObject].PolygonNum,buffer);
					glRenderMode(GL_SELECT);
					glInitNames();
					glPushName(0);

										
					glPushMatrix();
					MultModelViewMatrix(ActualScene->ObjectList[PolySelectObject].ModelView,1);
					for (int x=0; x<ActualScene->ObjectList[PolySelectObject].PolygonNum; x++)
					{

						glLoadName(x);
						glBegin(GL_TRIANGLES);
						glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v1].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v1].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v1].Position.z);
						glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v2].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v2].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v2].Position.z);
						glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v3].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v3].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[x].v3].Position.z);
						glEnd();
						
					}
					glPopMatrix();
					glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
					SetCameraView(&View->Cam,4/3.0f,0,mx,my);

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

					delete buffer;

					if (hits || BrushList->ItemList[2].ActiveBranch)
					{
						PVSelect SelectData;
						memset(&SelectData,0,sizeof(SelectData));
						for (int kl=0; kl<3; kl++)
						if (BrushOperator->ItemList[kl].ActiveBranch) SelectData.Operator=kl;

						if (BrushList->ItemList[0].ActiveBranch)
						{
							SelectData.Brush=0;
							SelectData.Param1=ActualScene->ObjectList[PolySelectObject].PolygonList[smallest].v1;
							SelectData.Param2=BrushSize-1;
						}
						
						if (BrushList->ItemList[1].ActiveBranch)
						{
							SelectData.Brush=1;
							SelectData.Param1=smallest;
							SelectData.Param2=BrushSize-1;
						}

						if (BrushList->ItemList[2].ActiveBranch)
						{
							SelectData.Brush=2;
							RndMax=ActualScene->ObjectList[PolySelectObject].PolygonNum;
							SelectData.Param1=RndMax;
							SelectData.Param2=RndSeed;
							SelectData.Param3=RndCount;
						}
						
						PVSelect *s=new PVSelect;
						memcpy(s,&SelectData,sizeof(PVSelect));
						switch (SelectData.Brush)
						{
						case 0:
							((ITEMLIST*)PolySelectInterface->ItemList[4].data)->AddItem("VX SELECT",s,GUI_SelectOnlyThisSelection,GUI_Rename,nop);
							break;
						case 1:
							((ITEMLIST*)PolySelectInterface->ItemList[4].data)->AddItem("PLY SELECT",s,GUI_SelectOnlyThisSelection,GUI_Rename,nop);
							break;
						case 2:
							((ITEMLIST*)PolySelectInterface->ItemList[4].data)->AddItem("RND SELECT",s,GUI_SelectOnlyThisSelection,GUI_Rename,nop);
							break;
						case 3:
							((ITEMLIST*)PolySelectInterface->ItemList[4].data)->AddItem("MAP SELECT",s,GUI_SelectOnlyThisSelection,GUI_Rename,nop);
							break;
						}
						PolySelectInterface->ItemList[4].ItemNum++;
						CurrentPolySelection->ItemNum++;
						

						Obj_DoPolySelection(&ActualScene->ObjectList[PolySelectObject],&SelectData,true);

						for (int k=0; k<ActualScene->ObjectList[PolySelectObject].PolygonNum; k++)
						if (ActualScene->ObjectList[PolySelectObject].PolygonList[k].Highlighted)
						{
							ActualScene->ObjectList[PolySelectObject].PolygonList[k].Selected=!ActualScene->ObjectList[PolySelectObject].PolygonList[k].Selected;
						}

					}
					glPopAttrib();
				}

			}
			if (MainScroller->ParentItem->SelectedItem==1) //camera transform
			{
				float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
				float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
				ModellerDisplay4->ParentItem->Cam.Eye.a[View->i1]=xx*View->i.a[View->i1]+View->Cam.Eye.a[View->i1];
				ModellerDisplay4->ParentItem->Cam.Eye.a[View->j1]=yy*View->j.a[View->j1]+View->Cam.Eye.a[View->j1];
			}
			if (!(PolySelect && !View->WireFrame) && MainScroller->ParentItem->SelectedItem==3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8)
			{
				//for (ITEM *I=ActualScene->ItemList->List;I;I=I->Next)
				for (int i=0; i<ActualScene->ObjectNum; i++)
				if (!ActualScene->ObjectList[i].Deleted && !ActualScene->ObjectList[i].ListItem->Hidden  && ActualScene->ObjectList[i].Primitive<aDDict_MAPXFORM)
				{
					ITEM *I=ActualScene->ObjectList[i].ListItem;
					I->Data=&(ActualScene->ObjectList[i]);
					if (I->Data && I->Selected)
					{
						//Transform kell 

						MATRIX ObjPosition;

						//ObjCenter Transform
						if (Interface->ItemList[1].Interface->ItemList[4].ButtonPushed)
						{
							memcpy(ObjPosition,((OBJECT*)I->Data)->TransformBuffer,sizeof(MATRIX));
							int x;
							for (x=0; x<3; x++)
								for (int y=0; y<3; y++)
									if (x==y) ObjPosition[x][y]=1; else ObjPosition[x][y]=0;
							for (x=0; x<3; x++) ObjPosition[x][3]*=-1;
							M_Mult(ObjPosition,((OBJECT*)I->Data)->ModelView,((OBJECT*)I->Data)->ModelView);
						}
						else
						{
							M_Identity(ObjPosition);
							ObjPosition[0][3]=-Pivot.x;
							ObjPosition[1][3]=-Pivot.y;
							ObjPosition[2][3]=-Pivot.z;
							M_Mult(ObjPosition,((OBJECT*)I->Data)->ModelView,((OBJECT*)I->Data)->ModelView);
						}

						MATRIX m;

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

							float x=(int)(((mx-lx)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f)/ScaleFactors.a[View->i1]*4)*1000)/1000.0f;
							float y=(int)(((my-ly)/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f)/ScaleFactors.a[View->j1]*4)*1000)/1000.0f;

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
							M_Scale(View->i.x*View->i.x*x+View->j.x*View->j.x*y+1.0f,
									View->i.y*View->i.y*x+View->j.y*View->j.y*y+1.0f,
									View->i.z*View->i.z*x+View->j.z*View->j.z*y+1.0f,m);
							ScaleVector=V3_Make(View->i.x*View->i.x*x+View->j.x*View->j.x*y,
									View->i.y*View->i.y*x+View->j.y*View->j.y*y,
									View->i.z*View->i.z*x+View->j.z*View->j.z*y);
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

								M_Scale((int)(((Displacement.x/ScaleFactors.x*3)*mask.x+1)*1000)/1000.0f,
									    (int)(((Displacement.y/ScaleFactors.y*3)*mask.y+1)*1000)/1000.0f,
										(int)(((Displacement.z/ScaleFactors.z*3)*mask.z+1)*1000)/1000.0f,m);
								ScaleVector=V3_Make((Displacement.x/ScaleFactors.x*3)*mask.x,(Displacement.y/ScaleFactors.y*3)*mask.y,(Displacement.z/ScaleFactors.z*3)*mask.z);
								SwitchTo2D();
							}

							//MouseString=new char[50];
							//memset(MouseString,0,50);
							_snprintf(MouseString,30,"(%.3fx, %.3fx, %.3fx)\0",m[0][0],m[1][1],m[2][2]);
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
							M_Rotate(v.x,v.y,v.z,x,m);
							RotateAxis=V3_Make(v.x,v.y,v.z);
							RotateAngle=x;
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
								Displacement=(int)((Planenormal.x*(P2.x-Pivot.x)+Planenormal.y*(P2.y-Pivot.y)+Planenormal.z*(P2.z-Pivot.z))/radtheta*180.0f*180.0f/f);

								M_Rotate(mask.x,mask.y,mask.z,Displacement*(float)radtheta,m);
								RotateAxis=V3_Make(mask.x,mask.y,mask.z);
								RotateAngle=Displacement*(float)radtheta/f;
								SwitchTo2D();
							}
							else if (!View->WireFrame) M_Identity(m);
							if (hits && View->WireFrame)
							{
								int rot;
								VECTOR3 mask=V3_Make(0,0,0);
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
								M_Rotate(mask.x,mask.y,mask.z,rot*(float)radtheta,m);
								RotateAxis=V3_Make(mask.x,mask.y,mask.z);
								RotateAngle=rot*(float)radtheta;
							}
							//MouseString=new char[20];
							//memset(MouseString,0,20);
							
							if (View->WireFrame)
							if (RotateAxis.a[View->i1]==0)
							_snprintf(MouseString,100,"(%d degrees)\0",mx-lx);
							else
							_snprintf(MouseString,100,"(%d degrees)\0",my-ly);
							if (hits && !View->WireFrame)
							_snprintf(MouseString,100,"(%d degrees)\0",-Displacement);
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
							x=(int)((((mx-lx))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f))*1000)/1000.0f;
							y=(int)(((-(my-ly))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f))*1000)/1000.0f;
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

							M_XPose((View->i.x*x+View->j.x*y)*mask.x,(View->i.y*x+View->j.y*y)*mask.y,(View->i.z*x+View->j.z*y)*mask.z,m);
							TransformVector=V3_Make((View->i.x*x+View->j.x*y)*mask.x,(View->i.y*x+View->j.y*y)*mask.y,(View->i.z*x+View->j.z*y)*mask.z);
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

								M_XPose((int)(((Displacement.x)*mask.x)*1000)/1000.0f,
									    (int)(((Displacement.y)*mask.y)*1000)/1000.0f,
										(int)(((Displacement.z)*mask.z)*1000)/1000.0f,m);
								TransformVector=V3_Make((Displacement.x)*mask.x,(Displacement.y)*mask.y,(Displacement.z)*mask.z);
								SwitchTo2D();
							}
							//MouseString=new char[50];
							//memset(MouseString,0,50);
							_snprintf(MouseString,100,"(%.3f, %.3f, %.3f)\0",m[0][3],m[1][3],m[2][3]);
						}

						if (Interface->ItemList[1].Interface->ItemList[2].Interface->ItemList[3].ButtonPushed /*&& View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed*/)
						{ // skew
							M_Identity(m);
							float x;
							x=(int)(((mx-lx))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f)*1000.0f)/1000.0f;
							M_Skew(x,0,0,m);
							
							_snprintf(MouseString,100,"(%.3f)\0",x);

							if (View->k==2)
							{
								MATRIX a;
								M_Rotate(1,0,0,3.1415f/2.0f,a);
								M_Mult(a,m,m);
								M_Rotate(1,0,0,-3.1415f/2.0f,a);
								M_Mult(m,a,m);
							}

							if (View->k==0)
							{
								MATRIX a;
								
								M_Rotate(0,0,-1,3.1415f/2.0f,a);
								M_Mult(m,a,m);
								
								M_Rotate(1,0,0,-3.1415f/2.0f,a);
								M_Mult(m,a,m);

								M_Rotate(0,0,-1,-3.1415f/2.0f,a);
								M_Mult(a,m,m);

								M_Rotate(1,0,0,3.1415f/2.0f,a);
								M_Mult(a,m,m);
							}

						}

						if (Interface->ItemList[1].Interface->ItemList[2].Interface->ItemList[4].ButtonPushed /*&& View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed*/)
						{ // trapez
							M_Identity(m);
							float x;
							x=(int)(((mx-lx))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f)*1000.0f)/1000.0f;
							M_Trapez(x,m);
							
							_snprintf(MouseString,100,"(%.3f)\0",x);

							/*if (View->k==2)
							{
								MATRIX a;
								M_Rotate(1,0,0,3.1415f/2.0f,a);
								M_Mult(a,m,m);
								M_Rotate(1,0,0,-3.1415f/2.0f,a);
								M_Mult(m,a,m);
							}

							if (View->k==0)
							{
								MATRIX a;
								
								M_Rotate(0,0,-1,3.1415f/2.0f,a);
								M_Mult(m,a,m);
								
								M_Rotate(1,0,0,-3.1415f/2.0f,a);
								M_Mult(m,a,m);

								M_Rotate(0,0,-1,-3.1415f/2.0f,a);
								M_Mult(a,m,m);

								M_Rotate(1,0,0,3.1415f/2.0f,a);
								M_Mult(a,m,m);
							}*/

						}

						if (RightButton) M_Identity(m); //Gyors Undo

						if (!Maptransforming)
						{
	
							if (!Interface->ItemList[1].Interface->ItemList[3].ButtonPushed) //ObjSpace Transform?
							M_Mult(m,((OBJECT*)I->Data)->ModelView,((OBJECT*)I->Data)->ModelView);
							else
							M_Mult(((OBJECT*)I->Data)->ModelView,m,((OBJECT*)I->Data)->ModelView);

						}
						else
						{
							M_Mult(MXMatrix,m,MXMatrix);
						}

						//ObjCenter Transform
						//if (Interface->ItemList[1].Interface->ItemList[4].ButtonPushed)
						{
							for (int x=0; x<3; x++) ObjPosition[x][3]*=-1;
							M_Mult(ObjPosition,((OBJECT*)I->Data)->ModelView,((OBJECT*)I->Data)->ModelView);
						}

					
					}
				}
			}

			if (!View->WireFrame && PolySelect)
			{

			}
		}

	}
	else
	{
	}
	WriteDebug("GUI_3dLeftClick over\n");
};

void DisplayRotateTool(GUIITEM *View)
{
	VECTOR3 Pivot1,Pivot2,Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot1=Pivot2=V3_Make(0,0,0);
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden  && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			PCount+=GetPivot(&ActualScene->ObjectList[x],Pivot1,V3_Make(0,0,0));
			GetPivot2(&ActualScene->ObjectList[x],Pivot2,V3_Make(0,0,0));
			Selected++;
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

void DisplayMoveTool(GUIITEM *View)
{
	VECTOR3 Pivot1,Pivot2,Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot1=Pivot2=V3_Make(0,0,0);
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden  && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			PCount+=GetPivot(&ActualScene->ObjectList[x],Pivot1,V3_Make(0,0,0));
			GetPivot2(&ActualScene->ObjectList[x],Pivot2,V3_Make(0,0,0));
			Selected++;
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

void DisplayScaleTool(GUIITEM *View)
{
	VECTOR3 Pivot1,Pivot2,Pivot;
	bool P=false; int Selected=0;
	int PCount=0;
	Pivot1=Pivot2=V3_Make(0,0,0);
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden  && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			PCount+=GetPivot(&ActualScene->ObjectList[x],Pivot1,V3_Make(0,0,0));
			GetPivot2(&ActualScene->ObjectList[x],Pivot2,V3_Make(0,0,0));
			Selected++;
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