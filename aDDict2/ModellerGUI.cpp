#include "ModellerGUI.h"

cINTERFACE *StatusBar3;
cINTERFACE *ModellerDisplays,*MainScroller,*ArcScroller1,*ArcScroller2,*SceneScroller,*LightMenu;
cINTERFACE *LightMenus[8];
cINTERFACE* ConstMaterialsMenu;
cINTERFACE *TransformSelector;
cINTERFACE *ColorSelector;
cINTERFACE* MapXformMenu;
cINTERFACE *CreateLoftI,*OffsetInterface,*SuperShapeMenuI;
cINTERFACE *SphereOrToroid,*RadFunc,*VertFunc;

ITEMLIST *ModellFileList=NULL,*MaterialList=NULL,*PolySelections=NULL,*CurrentPolySelection=NULL,*ArcList=NULL;
cINTERFACE *BrushOperator;
MATERIAL *ActualMaterial=NULL;
GLuint MaterialHandle,MaterialHandle2;
int MaterialListItemNum,MaterialListItemNumMapXform;
void *MainScrollerData,*ArcScroller1Data,*ArcScroller2Data;
int BrushSize;
int RndMax,RndSeed,RndCount;
int MXTile,MYTile;

bool ModellLighting=true;

SUPERSHAPE SuperShapeData;

void *MaterialListData;
int XTile,YTile;
float MOffx,MOffy;
unsigned char FurPlanes;
int FurSize,FurHeight;

cINTERFACE *ModellerDisplay1,*PInterface,*BInterface;
cINTERFACE *ModellerDisplay2;
cINTERFACE *ModellerDisplay3;
cINTERFACE *ModellerDisplay4;
cINTERFACE *MXPolySelect;

cINTERFACE *ScenesMenu;
cINTERFACE *ObjectsMenu;
cINTERFACE *MaterialsMenu,*VarMaterialsMenu;
cINTERFACE *LightsMenu;
cINTERFACE *ParticlesMenu;
cINTERFACE *EffectsMenu;
cINTERFACE *PolySelectInterface;
cINTERFACE *BrushList;

LIGHTDATA LightData[8];
int ObjRed,ObjGreen,ObjBlue,ObjAlpha;
unsigned char a,MAEpsilon;
unsigned char SphereX=12,SphereY=12,CylinderX=12,CylinderY=12,ConeX=12,ConeY=1,GridX=10,GridY=10,ArcX=12,LineX,MeshSmoothAmount,MeshBlurAmount,CircleX=24,CircleInner;
bool *CylinderEnd,*ConeEnd;
bool PolySelect=false;
int PolySelectObject;

bool LoftCreating,SuperShapeCreating;

bool Maptransforming;
int MappedObject;
cINTERFACE *MapTMenu;

int f,ArcY=360;

OBJECT *LoftTrack,*LoftShape;
OBJECT *Preview;

void __stdcall GUI_SetLighting(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	GUI_ButtonSwitch(ScrollerInterface,ScrollerID);
	ModellLighting=ScrollerInterface->ItemList[ScrollerID].ButtonPushed;
	WaitLeftButton=true;
}

void __stdcall Scroller_SelectOnlyThisArc(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
	if (ArcScroller1->ParentItem->InterfaceItemId==ScrollerID) LoftTrack=(OBJECT*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
	if (ArcScroller2->ParentItem->InterfaceItemId==ScrollerID) LoftShape=(OBJECT*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
}

void __stdcall Modeller_InvertSelect(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted)
		ActualScene->ObjectList[x].ListItem->Selected=!ActualScene->ObjectList[x].ListItem->Selected;
	WaitLeftButton=true;
}


void __stdcall Modeller_DeSelectAll(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted)
		ActualScene->ObjectList[x].ListItem->Selected=false;
}

void __stdcall Modeller_SelectAll(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	if (!ActualScene->ObjectList[x].Deleted)
		ActualScene->ObjectList[x].ListItem->Selected=true;
}

void DeleteChildren(int ObjectID)
{
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].Primitive>=aDDict_MAPXFORM && ActualScene->ObjectList[x].TargetObjectID==ObjectID)
		{
			ActualScene->ObjectList[x].Deleted=true;
			if (ActualScene->ObjectList[x].ListItem->Prev)
			{
				ActualScene->ObjectList[x].ListItem->Prev->Next=ActualScene->ObjectList[x].ListItem->Next;
				if (ActualScene->ObjectList[x].ListItem->Next)
				ActualScene->ObjectList[x].ListItem->Prev->Next->Prev=ActualScene->ObjectList[x].ListItem->Prev;
				else
				ActualScene->ItemList->Last=ActualScene->ObjectList[x].ListItem->Prev;
			}
			ActualScene->ObjectList[x].ListItem->Selected=false;
			ActualScene->ItemList->ItemNum--;				
		}
	}
}

void __stdcall Modeller_DeleteSelected(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	{
		bool *Changed= new bool[ActualScene->ObjectNum];
		memset(Changed,0,ActualScene->ObjectNum);
		int x;
		for (x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive>=aDDict_MAPXFORM)
			{
				OBJECT *Parent=ActualScene->FindObjectByID(ActualScene->ObjectList[x].TargetObjectID);
				if (Parent)
				{
					if (Parent->ListItem->Selected) ActualScene->ObjectList[x].ListItem->Selected=false;
					else
					{ //effektet torolni kell
						ActualScene->ObjectList[x].Deleted=true;
						if (ActualScene->ObjectList[x].ListItem->Prev)
						{
							ActualScene->ObjectList[x].ListItem->Prev->Next=ActualScene->ObjectList[x].ListItem->Next;
							if (ActualScene->ObjectList[x].ListItem->Next)
							ActualScene->ObjectList[x].ListItem->Prev->Next->Prev=ActualScene->ObjectList[x].ListItem->Prev;
							else
							ActualScene->ItemList->Last=ActualScene->ObjectList[x].ListItem->Prev;
						}
						for (int y=0; y<ActualScene->ObjectNum; y++) if (Parent->ID==ActualScene->ObjectList[y].ID) Changed[y]=true;
						ActualScene->ObjectList[x].ListItem->Selected=false;
						ActualScene->ItemList->ItemNum--;				
						//Changed[x]=true;
					}
				}
			}

		for (x=0; x<ActualScene->ObjectNum; x++)
		{
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM) 
			{
				ActualScene->ObjectList[x].Deleted=true;
				DeleteChildren(ActualScene->ObjectList[x].ID);
				if (ActualScene->ObjectList[x].ListItem->Prev)
				{
					ActualScene->ObjectList[x].ListItem->Prev->Next=ActualScene->ObjectList[x].ListItem->Next;
					if (ActualScene->ObjectList[x].ListItem->Next)
					ActualScene->ObjectList[x].ListItem->Prev->Next->Prev=ActualScene->ObjectList[x].ListItem->Prev;
					else
					ActualScene->ItemList->Last=ActualScene->ObjectList[x].ListItem->Prev;
					ActualScene->ItemList->ItemNum--;				
				}
				if (ActualScene->ObjectList[x].Primitive==aDDict_LINE || ActualScene->ObjectList[x].Primitive==aDDict_ARC)
				{
					if (ActualScene->ObjectList[x].ArcListItem->Prev)
					{
						ActualScene->ObjectList[x].ArcListItem->Prev->Next=ActualScene->ObjectList[x].ArcListItem->Next;
						if (ActualScene->ObjectList[x].ArcListItem->Next)
						ActualScene->ObjectList[x].ArcListItem->Prev->Next->Prev=ActualScene->ObjectList[x].ArcListItem->Prev;
						else
						ActualScene->ArcList->Last=ActualScene->ObjectList[x].ArcListItem->Prev;
						ActualScene->ArcList->ItemNum--;				
					}
					else
					{
						if (ActualScene->ArcList->List)
						{
							ActualScene->ArcList->List=ActualScene->ArcList->List->Next;
							if (ActualScene->ArcList->List) ActualScene->ArcList->List->Prev=NULL;
							if (!ActualScene->ArcList->List) ActualScene->ArcList->Last=NULL;
							ActualScene->ArcList->ItemNum--;
						}
					}
				}
				ActualScene->ObjectList[x].ListItem->Selected=false;
			}
		}

		for (x=0; x<ActualScene->ObjectNum; x++)
			if (Changed[x] && !ActualScene->ObjectList[x].Deleted)
			{
				ActualScene->ObjectList[x].VertexNum=0;
				ActualScene->ObjectList[x].PolygonNum=0;
				ActualScene->ObjectList[x].EdgeNum=0;

				if (ActualScene->ObjectList[x].Primitive!=aDDict_LOFT)
				GeneratePrimitive(&ActualScene->ObjectList[x],ActualScene->ObjectList[x].Primitive,ActualScene->ObjectList[x].Param1,ActualScene->ObjectList[x].Param2,ActualScene->ObjectList[x].Param3,ActualScene->ObjectList[x].Param4,ActualScene->ObjectList[x].Param5);
				else
				{
					OBJECT *a=NULL,*b=NULL;
					for (int y=0; y<ActualScene->ObjectNum; y++)
					{
						if (ActualScene->ObjectList[y].ID==ActualScene->ObjectList[x].Param1) a=&ActualScene->ObjectList[y];
						if (ActualScene->ObjectList[y].ID==ActualScene->ObjectList[x].Param2) b=&ActualScene->ObjectList[y];
					}
					GeneratePrimitive(&ActualScene->ObjectList[x],ActualScene->ObjectList[x].Primitive,(int)a,(int)b,ActualScene->ObjectList[x].Param3,ActualScene->ObjectList[x].Param4,ActualScene->ObjectList[x].Param5);
				}

				for (int y=0; y<ActualScene->ObjectNum; y++)
				if (!ActualScene->ObjectList[y].Deleted && ActualScene->ObjectList[y].TargetObjectID==ActualScene->ObjectList[x].ID)
				{// effektet ujracsinalni
					switch (ActualScene->ObjectList[y].Primitive)
					{
					case aDDict_BUTTERFLYSMOOTH:
					case aDDict_LINEARSMOOTH:
						{
							for (int z=0; z<ActualScene->ObjectList[y].Param2; z++) ActualScene->ObjectList[x].MeshSmooth(ActualScene->ObjectList[y].Primitive==aDDict_LINEARSMOOTH);
							break;
						}
					case aDDict_MESHBLUR:
						{
							for (int z=0; z<ActualScene->ObjectList[y].Param2; z++) ActualScene->ObjectList[x].MeshBlur();
							break;
						}
					case aDDict_MAPXFORM:

						{
							int z;
							for (z=0; z<ActualScene->ObjectNum; z++)
							{
								if (ActualScene->ObjectList[z].ID==ActualScene->ObjectList[y].TargetObjectID)
								{
									MappedObject=z;
									break;
								}
							}

							if (ActualScene->ObjectList[y].Param1==0)
							for (z=0; z<ActualScene->ObjectList[MappedObject].VertexNum; z++)
							{
								ActualScene->ObjectList[MappedObject].VertexList[z].Selected=true;
							}
							else
							{
								ITEM *p=PolySelections->List;
								for (int z=0; z<ActualScene->ObjectList[y].Param2; z++) p=p->Next;
								ITEMLIST *b=(ITEMLIST*)p->Data;
								SelectObjVertices(&ActualScene->ObjectList[MappedObject],b,ActualScene->ObjectList[y].Param3,ActualScene->ObjectList[y].Param1!=2);
							}

							CalculateObjectVertexWeights(&ActualScene->ObjectList[MappedObject],
														  ActualScene->ObjectList[y].Material,
														  ActualScene->ObjectList[y].Xtile,
														  ActualScene->ObjectList[y].Ytile,
														  ActualScene->ObjectList[y].XSwap,
														  ActualScene->ObjectList[y].YSwap,
														  ActualScene->ObjectList[y].Swap,
														  ActualScene->ObjectList[y].OffsetX,
														  ActualScene->ObjectList[y].OffsetY);
							memcpy(MXMatrix,ActualScene->ObjectList[y].ModelView,sizeof(MATRIX));

							CalculateMapTransform(ActualScene->ObjectList[y].MapXformType,ActualScene->ObjectList[y].MapXformColor,MXMatrix,MappedObject);
							for (z=0; z<ActualScene->ObjectList[MappedObject].VertexNum; z++)
								ActualScene->ObjectList[MappedObject].VertexList[z].Position=ActualScene->ObjectList[MappedObject].VertexList[z].MapTransformedPosition;
							ActualScene->ObjectList[MappedObject].CalculateNormals();
							ActualScene->ObjectList[MappedObject].CreateDisplayList();
						}

						break;
					}

				}
			}
		SetEnvironmentToActualScene();
	}
}

void __stdcall GUI_ClearScene(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	{
		ActualScene->ObjectNum=0;
		ActualScene->ItemList->ItemNum=0;
		ActualScene->ItemList->Last=NULL;
		ActualScene->ItemList->List=NULL;
		ITEM *SceneData=ActualScene->ItemList->AddItem("SCENE DATA",NULL,nop,SubOpenClose,nop);
		char *NameString=new char[strlen(CommandLine)+51];
		memset(NameString,0,strlen(CommandLine)+51);
		_snprintf(NameString,strlen(CommandLine)+51,"%s",CommandLine);
		SceneData->AddSubItem(NameString,NULL,nop,nop,nop);
		NameString=new char[10];
		memset(NameString,0,10);
		_snprintf(NameString,10,"ID: %d",ActualScene->ID);
		SceneData->AddSubItem(NameString,NULL,nop,nop,nop);

		ActualScene->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
		ITEM *LightList=ActualScene->ItemList->AddItem("LIGHTS",NULL,nop,nop,nop);
		LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
		LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
		LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
		LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
		LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
		LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
		LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
		LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
		
		ActualScene->ItemList->AddItem("-------OBJECTS-------",NULL,Scroller_SelectItem,nop,nop);

		ActualScene->Camera=ModellerDisplay4->ParentItem->Cam;
		SetEnvironmentToActualScene();
	}
}

void __stdcall GUI_SelectPolySelection(cINTERFACE* Interface, int ButtonId)
{
	if (!PolySelect)
	{
		void *old=CurrentPolySelection;
		Scroller_SelectItem(Interface,ButtonId);
		PolySelectInterface->ItemList[4].data=CurrentPolySelection=(ITEMLIST*)Interface->ClickedItem->Data;		
		PolySelectInterface->ItemList[4].ItemNum=CurrentPolySelection->ItemNum;
		//if (old!=CurrentPolySelection)
		PolySelectInterface->ItemList[4].SelectedItem=min(PolySelectInterface->ItemList[4].SelectedItem,PolySelectInterface->ItemList[4].ItemNum-1);
		PolySelectID=min(PolySelectID,PolySelectInterface->ItemList[4].ItemNum-1);
	}
}

void __stdcall GUI_AddNewPolySelect(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		{//NEW SELECTION
			char *cmdline=new char[256];
			memset(cmdline,0,256);
			sprintf(cmdline,"P: %s\0",CommandLine);
			ITEMLIST *NewDat=new ITEMLIST();
			PolySelections->AddItem(cmdline,NewDat,GUI_SelectPolySelection,GUI_Rename,nop);
			if (PolySelections->ItemNum==1) PolySelectInterface->ItemList[4].data=CurrentPolySelection=(ITEMLIST*)PolySelections->Last->Data;
			PolySelectInterface->ItemList[0].SelectedItem=PolySelectInterface->ItemList[0].ItemNum;
			PolySelectInterface->ItemList[0].ItemNum++;
			//((ITEMLIST*)PolySelections->Last->Data)->AddItem("POLY SELECTION",NULL,nop,nop,nop);
			if (PolySelections->Last->Prev) PolySelections->Last->ID=PolySelections->Last->Prev->ID+1;
			else PolySelections->Last->ID=0;
	
			PolySelectInterface->ItemList[4].data=CurrentPolySelection=NewDat;
			PolySelectInterface->ItemList[4].ItemNum=0;
			//PolySelectInterface->ParentItem->ItemNum++;
		}
	}
}

/*void __stdcall GUI_AddNewVertexSelect(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		{//NEW SELECTION
			char *cmdline=new char[256];
			memset(cmdline,0,256);
			sprintf(cmdline,"V: %s",CommandLine);
			PolySelections->AddItem(cmdline,new ITEMLIST(),GUI_SelectPolySelection,nop,nop);
			if (PolySelections->ItemNum==1) PolySelectInterface->ItemList[4].data=CurrentPolySelection=(ITEMLIST*)PolySelections->Last->Data;
			//((ITEMLIST*)PolySelections->Last->Data)->AddItem("VERTEX SELECTION",NULL,nop,nop,nop);
			if (PolySelections->Last->Prev) PolySelections->Last->ID=PolySelections->Last->Prev->ID+1;
			else PolySelections->Last->ID=0;
		}
	}
}*/


void __stdcall GUI_PolySwitch(cINTERFACE* Interface, int ButtonId)
{
	if (PolySelections->List)
	{
		GUI_ButtonSwitch(Interface,ButtonId);
		PolySelect=Interface->ItemList[ButtonId].ButtonPushed;
		if (ActualScene)
		if (PolySelect)
		{
			int cntr=0;
			int x;
			for (x=0; x<ActualScene->ObjectNum && !cntr; x++)
				if (ActualScene->ObjectList[x].ListItem->Selected) cntr++;
			if (!cntr) 
			{
				ErrorMessage="YOU HAVE TO SELECT AN OBJECT FOR THE POLYSELECT FIRST";
				PolySelect=false;
				Interface->ItemList[ButtonId].ButtonPushed=false;
			}
			else
			{
				PolySelectObject=x-1;
			}
		}
	}
	else
	ErrorMessage="CREATE A POLYSELECTION FIRST";
}

void __stdcall GUI_SelectMaterial(cINTERFACE* Interface, int ButtonId)
{
	ITEM *I=ScrollerFindItem(MaterialList,VarMaterialsMenu->ItemList[MaterialListItemNum].SelectedItem);
	MapXformMenu->ItemList[MaterialListItemNumMapXform].SelectedItem=VarMaterialsMenu->ItemList[MaterialListItemNum].SelectedItem;
	if (I) 
	{
		ActualMaterial=(MATERIAL*)(I->Data);
		MaterialHandle=ActualMaterial->ImageData.TextureHandle;
	}
	for (I=MaterialList->List; I; I=I->Next)
	{
		I->Selected=false;
		I->Hidden=false;
	}
	if (ActualScene)
	{
		int x;
		for (x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				if (ActualScene->ObjectList[x].Textured || ActualScene->ObjectList[x].Primitive==aDDict_MAPXFORM)
				{
					for (I=MaterialList->List; I; I=I->Next)
						if (I->Data==ActualScene->ObjectList[x].Material)
						{
							I->Selected=true;
							break;
						}
				}
				if (ActualScene->ObjectList[x].EnvMapped)
				{
					for (I=MaterialList->List; I; I=I->Next)
						if (I->Data==ActualScene->ObjectList[x].EnvMap)
						{
							I->Hidden=true;
							break;
						}
				}
			}
		for (x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ButtonId].Offx=ActualScene->ObjectList[x].OffsetX;
				Interface->ItemList[ButtonId].Offy=ActualScene->ObjectList[x].OffsetY;
				x=ActualScene->ObjectNum+1;
			}
	}
}

void __stdcall GUI_SelectMaterialMapXform(cINTERFACE* Interface, int ButtonId)
{
	ITEM *I=ScrollerFindItem(MaterialList,MapXformMenu->ItemList[MaterialListItemNumMapXform].SelectedItem);
	VarMaterialsMenu->ItemList[MaterialListItemNum].SelectedItem=MapXformMenu->ItemList[MaterialListItemNumMapXform].SelectedItem;
	if (I) 
	{
		ActualMaterial=(MATERIAL*)(I->Data);
		MaterialHandle=ActualMaterial->ImageData.TextureHandle;
	}
	for (I=MaterialList->List; I; I=I->Next)
	{
		I->Selected=false;
		I->Hidden=false;
	}
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			if (ActualScene->ObjectList[x].Textured || ActualScene->ObjectList[x].Primitive==aDDict_MAPXFORM)
			{
				for (I=MaterialList->List; I; I=I->Next)
					if (I->Data==ActualScene->ObjectList[x].Material)
					{
						I->Selected=true;
						break;
					}
			}
			if (ActualScene->ObjectList[x].EnvMapped)
			{
				for (I=MaterialList->List; I; I=I->Next)
					if (I->Data==ActualScene->ObjectList[x].EnvMap)
					{
						I->Hidden=true;
						break;
					}
			}
		}
}


void __stdcall GUI_SwapX(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].XSwap=!ActualScene->ObjectList[x].XSwap;
			ActualScene->ObjectList[x].CalculateTextureCoordinates();
			ActualScene->ObjectList[x].CreateDisplayList();
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_SwapY(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].YSwap=!ActualScene->ObjectList[x].YSwap;
			ActualScene->ObjectList[x].CalculateTextureCoordinates();
			ActualScene->ObjectList[x].CreateDisplayList();
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_Swap(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].Swap=!ActualScene->ObjectList[x].Swap;
			ActualScene->ObjectList[x].CalculateTextureCoordinates();
			ActualScene->ObjectList[x].CreateDisplayList();
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_InvertNormal(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].NormalsInverted=!ActualScene->ObjectList[x].NormalsInverted;
			ActualScene->ObjectList[x].CalculateNormals();
			ActualScene->ObjectList[x].CreateDisplayList();
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_SetDefaultShading(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			for (int y=0; y<ActualScene->ObjectList[x].PolygonNum; y++)
				ActualScene->ObjectList[x].PolygonList[y].CurrentShading=ActualScene->ObjectList[x].PolygonList[y].Shading;
			ActualScene->ObjectList[x].CalculateNormals();
			ActualScene->ObjectList[x].CreateDisplayList();
			ActualScene->ObjectList[x].Shading=aDDict_DEFAULTSHADE;
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_SetFlatShading(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			for (int y=0; y<ActualScene->ObjectList[x].PolygonNum; y++)
				ActualScene->ObjectList[x].PolygonList[y].CurrentShading=aDDict_FLATSHADE;
			ActualScene->ObjectList[x].CalculateNormals();
			ActualScene->ObjectList[x].CreateDisplayList();
			ActualScene->ObjectList[x].Shading=aDDict_FLATSHADE;
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_SetGouraudShading(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			for (int y=0; y<ActualScene->ObjectList[x].PolygonNum; y++)
				ActualScene->ObjectList[x].PolygonList[y].CurrentShading=aDDict_GOURAUDSHADE;
			ActualScene->ObjectList[x].CalculateNormals();
			ActualScene->ObjectList[x].CreateDisplayList();
			ActualScene->ObjectList[x].Shading=aDDict_GOURAUDSHADE;
			WaitLeftButton=true;
		}
	}
}

void __stdcall GUI_SetTexture(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	if (ActualMaterial)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].Textured=true;
			ActualScene->ObjectList[x].Material=ActualMaterial;
		}
	}
}

void __stdcall GUI_ClearTexture(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive!=aDDict_MAPXFORM)
		{
			ActualScene->ObjectList[x].Textured=false;
			ActualScene->ObjectList[x].Material=NULL;
		}
	}
}


void __stdcall GUI_SetEnvMap(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	if (ActualMaterial)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].EnvMapped=true;
			ActualScene->ObjectList[x].EnvMap=ActualMaterial;
		}
	}
}

void __stdcall GUI_ClearEnvmap(cINTERFACE* Interface, int ButtonId)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ListItem->Selected)
		{
			ActualScene->ObjectList[x].EnvMapped=false;
			ActualScene->ObjectList[x].EnvMap=NULL;
		}
	}
}

void __stdcall GUI_GetModellFileList(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<Interface->ItemNum; x++) Interface->ItemList[x].ButtonPushed=false;
	Interface->ItemList[ButtonId].ButtonPushed=true;
	Interface->SetActiveBranch(ButtonId);
	if (Interface->ItemList[ButtonId].ToolTip) LastClickedToolTip=Interface->ItemList[ButtonId].ToolTip;

	ModellFileList->Last=NULL;
	ModellFileList->List=NULL;
	ModellFileList->ItemNum=0;
	// kill old list...
	SetupFileList(ModellFileList,SceneMsk);
	SetupFileList(ModellFileList,AseMsk);
	SetupFileList(ModellFileList,ObjMsk);
	SetupFileList(ModellFileList,PdoMsk);
	Interface->ItemList[ButtonId].ItemNum=ModellFileList->ItemNum;
}

void __stdcall SetLightSettingsToActualScene(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<8; x++)
	{
		//GUIITEM *LightItem=ActualScene->ItemList->List->Next->Next->SubItems;
		//for (int y=0; y<x; y++) LightItem=LightItem->Next;
		if (ActualScene && LightMenus[x]->ItemList[1].Interface->ItemList[2].ButtonPushed) //SpotLight
		{
			ActualScene->Lights[x].Position[3]=1;
			ActualScene->Lights[x].Spot_Cutoff=LightData[x].cu;
		}

		if (ActualScene && LightMenus[x]->ItemList[1].Interface->ItemList[1].ButtonPushed) //DotLight
		{
			ActualScene->Lights[x].Spot_Cutoff=180;
			ActualScene->Lights[x].Position[3]=1;
		}

		if (ActualScene && LightMenus[x]->ItemList[1].Interface->ItemList[0].ButtonPushed) //DirLight
		{
			ActualScene->Lights[x].Spot_Cutoff=180;
			ActualScene->Lights[x].Position[3]=0;
		}

		if (ActualScene)
		{
			ActualScene->Lights[x].Lit=LightMenus[x]->ItemList[0].ButtonPushed;
			ActualScene->Lights[x].Color[0]=LightData[x].r/255.0f;
			ActualScene->Lights[x].Color[1]=LightData[x].g/255.0f;
			ActualScene->Lights[x].Color[2]=LightData[x].b/255.0f;
			ActualScene->Lights[x].Ambient[0]=LightData[x].ar/255.0f;
			ActualScene->Lights[x].Ambient[1]=LightData[x].ag/255.0f;
			ActualScene->Lights[x].Ambient[2]=LightData[x].ab/255.0f;
		}
	}
}

void SetActualSceneToLightSettings()
{
	for (int x=0; x<8; x++)
	{
		LightMenus[x]->ItemList[0].ButtonPushed=ActualScene->Lights[x].Lit;

		LightMenus[x]->ItemList[1].Interface->ItemList[2].ButtonPushed=(ActualScene->Lights[x].Position[3]==1 && ActualScene->Lights[x].Spot_Cutoff!=180);
		LightMenus[x]->ItemList[1].Interface->ItemList[1].ButtonPushed=(ActualScene->Lights[x].Spot_Cutoff==180 && ActualScene->Lights[x].Position[3]==1);
		LightMenus[x]->ItemList[1].Interface->ItemList[0].ButtonPushed=(ActualScene->Lights[x].Spot_Cutoff==180 && ActualScene->Lights[x].Position[3]==0);

		if (LightMenus[x]->ItemList[1].Interface->ItemList[2].ButtonPushed) LightMenus[x]->ItemList[1].Interface->SetActiveBranch(2);
		if (LightMenus[x]->ItemList[1].Interface->ItemList[1].ButtonPushed) LightMenus[x]->ItemList[1].Interface->SetActiveBranch(1);
		if (LightMenus[x]->ItemList[1].Interface->ItemList[0].ButtonPushed) LightMenus[x]->ItemList[1].Interface->SetActiveBranch(0);

		LightData[x].r=(int)(ActualScene->Lights[x].Color[0]*255.0f);
		LightData[x].g=(int)(ActualScene->Lights[x].Color[1]*255.0f);
		LightData[x].b=(int)(ActualScene->Lights[x].Color[2]*255.0f);
		LightData[x].ar=(int)(ActualScene->Lights[x].Ambient[0]*255.0f);
		LightData[x].ag=(int)(ActualScene->Lights[x].Ambient[1]*255.0f);
		LightData[x].ab=(int)(ActualScene->Lights[x].Ambient[2]*255.0f);

		/*LightData[x].Pos.x=ActualScene->Lights[x].Position[0];
		LightData[x].Pos.y=ActualScene->Lights[x].Position[1];
		LightData[x].Pos.z=ActualScene->Lights[x].Position[2];
		LightData[x].Pos4=ActualScene->Lights[x].Position[3];

		LightData[x].Dir.x=ActualScene->Lights[x].Spot_Direction[0];
		LightData[x].Dir.y=ActualScene->Lights[x].Spot_Direction[1];
		LightData[x].Dir.z=ActualScene->Lights[x].Spot_Direction[2];*/
	}
}

void BuildLightGUI(cINTERFACE *Light, int LightNumber)
{
	Light->AddSkinnedButton(899,295,LitButton,"TURN LIGHT ON/OFF",GUI_ButtonSwitch,nop,nop,SetLightSettingsToActualScene);
	cINTERFACE *TypeSelector=Light->AddSubInterface();
	TypeSelector->AddSkinnedButton(852,313,DirectButton,"SET TO DIRECTIONAL LIGHT",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *DotLight=TypeSelector->AddSkinnedButton(909,313,DotButton,"SET TO DOTLIGHT",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *SpotLight=TypeSelector->AddSkinnedButton(966,313,SpotButton,"SET TO SPOTLIGHT",GUI_ButtonSelect,nop,nop,nop);
	TypeSelector->ItemList[0].ButtonPushed=true;
	TypeSelector->SetActiveBranch(0);

	Light->AddByteBar(862,337,136,GUI_RED,&LightData[LightNumber-1].r,0,255,0,"RED: ","LIGHT RED INTENSITY");
	Light->AddByteBar(862,356,136,GUI_GREEN,&LightData[LightNumber-1].g,0,255,0,"GREEN: ","LIGHT GREEN INTENSITY");
	Light->AddByteBar(862,375,136,GUI_BLUE,&LightData[LightNumber-1].b,0,255,0,"BLUE: ","LIGHT BLUE INTENSITY");
	
	Light->AddByteBar(862,400,136,GUI_RED,&LightData[LightNumber-1].ar,0,255,0,"AMBIENT RED: ","AMBIENT LIGHT RED INTENSITY");
	Light->AddByteBar(862,419,136,GUI_GREEN,&LightData[LightNumber-1].ag,0,255,0,"AMBIENT GREEN: ","AMBIENT LIGHT GREEN INTENSITY");
	Light->AddByteBar(862,438,136,GUI_BLUE,&LightData[LightNumber-1].ab,0,255,0,"AMBIENT BLUE: ","AMBIENT LIGHT BLUE INTENSITY");

	DotLight->AddByteBar(862,465,136,GUI_NONE,&LightData[LightNumber-1].ca,0,255,0,"CONSTANT ATT: ","LIGHT CONSTANT ATTENUATION");
	DotLight->AddByteBar(862,484,136,GUI_NONE,&LightData[LightNumber-1].la,0,255,0,"LINEAR ATT: ","LIGHT LINEAR ATTENUATION");
	DotLight->AddByteBar(862,503,136,GUI_NONE,&LightData[LightNumber-1].qa,0,255,0,"QUADRATIC ATT: ","LIGHT QUADRATIC ATTENUATION");

	SpotLight->AddByteBar(862,465,136,GUI_NONE,&LightData[LightNumber-1].ca,0,255,0,"CONSTANT ATT: ","LIGHT CONSTANT ATTENUATION");
	SpotLight->AddByteBar(862,484,136,GUI_NONE,&LightData[LightNumber-1].la,0,255,0,"LINEAR ATT: ","LIGHT LINEAR ATTENUATION");
	SpotLight->AddByteBar(862,503,136,GUI_NONE,&LightData[LightNumber-1].qa,0,255,0,"QUADRATIC ATT: ","LIGHT QUADRATIC ATTENUATION");
	
	SpotLight->AddByteBar(862,528,136,GUI_NONE,&LightData[LightNumber-1].cu,0,90,45,"CUTOFF: ","SPOTLIGHT CUTOFF");
	SpotLight->AddByteBar(862,547,136,GUI_NONE,&LightData[LightNumber-1].ex,0,255,0,"EXPONENT: ","SPOTLIGHT EXPONENT");
}

void SetEnvironmentToActualScene()
{
	if (ActualScene)
	{
		*(int*)MainScrollerData=(int)ActualScene->ItemList;
		MainScroller->ParentItem->ItemNum=ActualScene->ItemList->ItemNum;
		ModellerDisplay4->ParentItem->Cam=ActualScene->Camera;

		if (ActualScene->ArcList)
		{
			*(int*)ArcScroller1Data=*(int*)ArcScroller2Data=(int)ActualScene->ArcList;
			ArcScroller1->ParentItem->ItemNum=ArcScroller2->ParentItem->ItemNum=ActualScene->ArcList->ItemNum;
			ArcScroller1->ParentItem->SelectedItem=ArcScroller2->ParentItem->SelectedItem=0;
			if (ActualScene->ArcList->List)
			LoftTrack=LoftShape=(OBJECT*)ActualScene->ArcList->List->Data;
			else
			LoftTrack=LoftShape=NULL;
		}
	}
	else
	{
		*(int*)MainScrollerData=NULL;
		MainScroller->ParentItem->ItemNum=0;
		memset(&ModellerDisplay4->ParentItem->Cam,0,sizeof(ModellerDisplay4->ParentItem->Cam));
		ModellerDisplay4->ParentItem->Cam.Fov=45;
		ModellerDisplay4->ParentItem->Cam.Up.y=1;
		ModellerDisplay4->ParentItem->Cam.Eye.z=10;
		ModellerDisplay4->ParentItem->Cam.Eye.y=7;
		*(int*)ArcScroller1Data=*(int*)ArcScroller2Data=NULL;
		ArcScroller1->ParentItem->ItemNum=ArcScroller2->ParentItem->ItemNum=0;
	}
}

void __stdcall Scroller_SelectOnlyThisScene(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
	if (MainGUI.ItemList[2].ActiveBranch)
	{
		ActualScene=(SCENE*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
		SetEnvironmentToActualScene();
	}
	if (MainGUI.ItemList[3].ActiveBranch)
	{
		LayouterSelectedScene=(SCENE*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
	}
}

void __stdcall Light1(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}

void __stdcall Light2(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}
void __stdcall Light3(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}
void __stdcall Light4(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}
void __stdcall Light5(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}

void __stdcall Light6(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}
void __stdcall Light7(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}
void __stdcall Light8(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ClickedItem->Selected=!ScrollerInterface->ClickedItem->Selected;
	MainScroller->ParentItem->SelectedItem=2;
	WaitLeftButton=true;
}

void __stdcall Modeller_NewSceneCallback(cINTERFACE* Interface, int ButtonId)
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
			// ADD NEW SCENE
			SCENE *Buffer=new SCENE();
			Buffer->Name=new char[strlen(CommandLine)+1];
			memset(Buffer->Name,0,strlen(CommandLine)+1);
			memcpy(Buffer->Name,CommandLine,strlen(CommandLine));

			if (!Scenes)
			{
				LastScene=Scenes=Buffer;
				Buffer->ID=0;
			}
			else
			{
				SceneScroller->ParentItem->SelectedItem++;
				Buffer->ID=LastScene->ID+1;
				LastScene->Next=Buffer;
				LastScene=Buffer;
			}

			ActualScene=LastScene;
			LastScene->ItemList=new ITEMLIST();
			LastScene->ArcList=new ITEMLIST();
			ITEM *SceneData=LastScene->ItemList->AddItem("SCENE DATA",NULL,nop,SubOpenClose,nop);
			char *NameString=new char[strlen(CommandLine)+51];
			memset(NameString,0,strlen(CommandLine)+51);
			_snprintf(NameString,strlen(CommandLine)+51,"%s",CommandLine);
			SceneData->AddSubItem(NameString,NULL,nop,nop,nop);
			NameString=new char[10];
			memset(NameString,0,10);
			_snprintf(NameString,10,"ID: %d",LastScene->ID);
			SceneData->AddSubItem(NameString,NULL,nop,nop,nop);

			LastScene->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
			ITEM *LightList=LastScene->ItemList->AddItem("LIGHTS",NULL,nop,nop,nop);
			LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
			LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
			LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
			LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
			LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
			LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
			LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
			LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
			
			LastScene->ItemList->AddItem("-------OBJECTS-------",NULL,Scroller_SelectItem,nop,nop);

			char *NewLine=new char[strlen(CommandLine)+51];
			memset(NewLine,0,strlen(CommandLine)+51);
			_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s",LastScene->ID,CommandLine);
			SceneListScroller->AddItem(NewLine,LastScene,Scroller_SelectOnlyThisScene,GUI_RenameScene,nop);
			SceneScroller->ParentItem->ItemNum++;
			AddModellInterface->ParentItem->ItemNum++;
			ActualScene->Camera=ModellerDisplay4->ParentItem->Cam;
			SetEnvironmentToActualScene();
		}

	}
}

void __stdcall CreateBox(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_BOX,0,0,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="BOX";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall CreateSphere(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_SPHERE,SphereX,SphereY,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="SPHERE";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall CreateCylinder(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_CYLINDER,CylinderX,CylinderY,Interface->ItemList[ScrollerID+1].ButtonPushed,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="CYLINDER";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall CreateCone(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_CONE,ConeX,Interface->ItemList[ScrollerID+1].ButtonPushed,ConeY,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="CONE";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall CreateLine(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_LINE,LineX+1,0,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="LINE";
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ArcListItem=ActualScene->ArcList->AddItem("LINE",&ActualScene->ObjectList[ActualScene->ObjectNum-1],Scroller_SelectOnlyThisArc,nop,nop);
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;
		ArcScroller1->ParentItem->ItemNum++;
		ArcScroller2->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall CreateArc(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_ARC,ArcX+1,ArcY,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="ARC";
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ArcListItem=ActualScene->ArcList->AddItem("ARC",&ActualScene->ObjectList[ActualScene->ObjectNum-1],Scroller_SelectOnlyThisArc,nop,nop);
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;
		ArcScroller1->ParentItem->ItemNum++;
		ArcScroller2->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall StartLoft(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		Interface->ItemList[ScrollerID].ButtonPushed=true;
		LoftCreating=true;
		CreateLoftI->ParentItem->ActiveBranch=true;
		if (ActualScene->ArcList->List)
		{	
			ITEM *i=ActualScene->ArcList->List;
			int x;
			for (x=0; x<ArcScroller1->ParentItem->SelectedItem; x++) if (i) i=i->Next;
			if (i) LoftTrack=(OBJECT*)i->Data; else LoftTrack=NULL;

			i=ActualScene->ArcList->List;
			for (x=0; x<ArcScroller2->ParentItem->SelectedItem; x++) if (i) i=i->Next;
			if (i) LoftShape=(OBJECT*)i->Data; else LoftShape=NULL;
		}
		else
		LoftTrack=LoftShape=NULL;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall StopLoft(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].ButtonPushed=false;
	LoftCreating=false;
	CreateLoftI->ParentItem->ActiveBranch=false;
	Interface->ParentItem->ParentInterface->ItemList[Interface->ParentItem->InterfaceItemId-1].ButtonPushed=false;
}

void __stdcall CreateLoft(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		if (LoftTrack && LoftShape)
		{
			MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
			ActualScene->AddObject();
			GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_LOFT,(int)LoftTrack,(int)LoftShape,0,0,0);
			LoftTrack->ListItem->Hidden=true;
			LoftShape->ListItem->Hidden=true;
			ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="LOFT";
			for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
			ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
			MainScroller->ParentItem->ItemNum++;
			StopLoft(Interface,ScrollerID);
		}
		else ErrorMessage="SOMETHING WENT WRONG WITH THE ARC SELECTION...";

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

//#ifndef Public_Release

void __stdcall StartSuperShape(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		Interface->ItemList[ScrollerID].ButtonPushed=true;
		SuperShapeCreating=true;
		SuperShapeMenuI->ParentItem->ActiveBranch=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall StopSuperShape(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].ButtonPushed=false;
	SuperShapeCreating=false;
	SuperShapeMenuI->ParentItem->ActiveBranch=false;
	Interface->ParentItem->ParentInterface->ItemList[Interface->ParentItem->InterfaceItemId-1].ButtonPushed=false;
}

void __stdcall CreateSuperShape(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();

		SUPERSHAPE *s=new SUPERSHAPE;
		memcpy(s,&SuperShapeData,sizeof(SUPERSHAPE));

		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_SUPERSHAPE,(int)s,0,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="SUPERSHAPE";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;
		StopSuperShape(Interface,ScrollerID);

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

//#endif

void __stdcall CreatePlane(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_PLANE,GridX,GridY,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="PLANE";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall CreateCircle(cINTERFACE* Interface, int ScrollerID)
{
	/*if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
		ActualScene->AddObject();
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],aDDict_CIRCLE,CircleX,CircleInner,0,0,0);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="CIRCLE";
		for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
		MainScroller->ParentItem->ItemNum++;

		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";*/
	ErrorMessage="CIRCLES HAVE BEEN DISABLED. USE LOFTS.";
}


void __stdcall GUI_ButtonSelect_LightMenuMain(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	if (ActualScene)
	{
		MainScroller->ParentItem->SelectedItem=2;
		//ActualScene->ItemList->List->Next->Next->SubOpened=true;
		ITEM *Buffer=ActualScene->ItemList->List->Next->Next->SubItems;
		for (int x=0; x<8; x++) 
		{
			Buffer->Selected=Interface->ItemList[ScrollerID].Interface->ItemList[0].Interface->ItemList[x].ButtonPushed;
			Buffer=Buffer->Next;
		}
	}
}

void __stdcall GUI_ButtonSelect_LightMenu(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);

	if (ActualScene)
	{
		ITEM *Buffer=ActualScene->ItemList->List->Next->Next->SubItems;
		int x;
		for (x=0; x<8; x++) 
		{
			Buffer->Selected=false;
			Buffer=Buffer->Next;
		}

		Buffer=ActualScene->ItemList->List->Next->Next->SubItems;
		for (x=0; x<ScrollerID; x++) Buffer=Buffer->Next;
		Buffer->Selected=true;	
		MainScroller->ParentItem->SelectedItem=2;
	}
}

void __stdcall GUI_SetXTile(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].Xtile=XTile;
				ActualScene->ObjectList[x].CalculateTextureCoordinates();
				ActualScene->ObjectList[x].CreateDisplayList();
			}
	}
}

void __stdcall GUI_SetXTileAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
			XTile=ActualScene->ObjectList[x].Xtile;
			break;
		}
}

void __stdcall GUI_SetYTile(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].Ytile=YTile;
				ActualScene->ObjectList[x].CalculateTextureCoordinates();
				ActualScene->ObjectList[x].CreateDisplayList();
			}
	}
}

void __stdcall GUI_SetYTileAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
			YTile=ActualScene->ObjectList[x].Ytile;
			break;
		}
}

void __stdcall GUI_SetObjRed(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].Color.r=ObjRed;
			}
	}
}

void __stdcall GUI_SetObjRedAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
				ObjRed=ActualScene->ObjectList[x].Color.r;
			break;
		}
}

void __stdcall GUI_SetObjAEpsilon(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].AEpsilon=MAEpsilon;
			}
	}
}

void __stdcall GUI_SetObjAEpsilonAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
				MAEpsilon=ActualScene->ObjectList[x].AEpsilon;
			break;
		}
}

void __stdcall GUI_SetObjGreen(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].Color.g=ObjGreen;
			}
	}
}

void __stdcall GUI_SetObjGreenAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
				ObjGreen=ActualScene->ObjectList[x].Color.g;
			break;
		}
}

void __stdcall GUI_SetObjBlue(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].Color.b=ObjBlue;
			}
	}
}

void __stdcall GUI_SetObjBlueAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
				ObjBlue=ActualScene->ObjectList[x].Color.b;
			break;
		}
}

void __stdcall GUI_SetObjAlpha(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID].LeftClickInWindow || (MouseInWindow(Interface->ItemList[ScrollerID].x1,Interface->ItemList[ScrollerID].y1,Interface->ItemList[ScrollerID].x2,Interface->ItemList[ScrollerID].y2)))
	{
		if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if ((Wheel!=0 || Interface->ItemList[ScrollerID].LeftClickInWindow || Interface->ItemList[ScrollerID-1].LeftClickInWindow || Interface->ItemList[ScrollerID+1].LeftClickInWindow) && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].Color.a=ObjAlpha;
			}
	}
}

void __stdcall GUI_SetObjAlphaAfter(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<=aDDict_MAPXFORM)
		{
				ObjAlpha=ActualScene->ObjectList[x].Color.a;
			break;
		}
}


void __stdcall GUI_SetObjSRCBlend(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
		if (ActualScene->ObjectList[x].ListItem->Selected && Interface->ItemList[ScrollerID].LeftClickInWindow)
		{
			switch (ScrollerID)
			{
			case 0: ActualScene->ObjectList[x].SRCBlend=GL_ZERO; break;
			case 1: ActualScene->ObjectList[x].SRCBlend=GL_ONE; break;
			case 2: ActualScene->ObjectList[x].SRCBlend=GL_SRC_COLOR; break;
			case 3: ActualScene->ObjectList[x].SRCBlend=GL_ONE_MINUS_SRC_COLOR; break;
			case 4: ActualScene->ObjectList[x].SRCBlend=GL_DST_ALPHA; break;
			case 5: ActualScene->ObjectList[x].SRCBlend=GL_ONE_MINUS_DST_ALPHA; break;
			case 6: ActualScene->ObjectList[x].SRCBlend=GL_SRC_ALPHA; break;
			case 7: ActualScene->ObjectList[x].SRCBlend=GL_ONE_MINUS_SRC_ALPHA; break;
			}
		}
}

void __stdcall GUI_SetObjSRCBlendCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{

				switch (ScrollerID)
				{
				case 0: if (ActualScene->ObjectList[x].SRCBlend==GL_ZERO) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 1: if (ActualScene->ObjectList[x].SRCBlend==GL_ONE) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 2: if (ActualScene->ObjectList[x].SRCBlend==GL_SRC_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 3: if (ActualScene->ObjectList[x].SRCBlend==GL_ONE_MINUS_SRC_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 4: if (ActualScene->ObjectList[x].SRCBlend==GL_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 5: if (ActualScene->ObjectList[x].SRCBlend==GL_ONE_MINUS_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 6: if (ActualScene->ObjectList[x].SRCBlend==GL_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				case 7: if (ActualScene->ObjectList[x].SRCBlend==GL_ONE_MINUS_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
				}

				break;
			}
	}
}

void __stdcall GUI_SetObjDSTBlend(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
		if (ActualScene->ObjectList[x].ListItem->Selected && Interface->ItemList[ScrollerID].LeftClickInWindow)
		{
			switch (ScrollerID)
			{
			case 0: ActualScene->ObjectList[x].DSTBlend=GL_ZERO; break;
			case 1: ActualScene->ObjectList[x].DSTBlend=GL_ONE; break;
			case 2: ActualScene->ObjectList[x].DSTBlend=GL_DST_COLOR; break;
			case 3: ActualScene->ObjectList[x].DSTBlend=GL_ONE_MINUS_DST_COLOR; break;
			case 4: ActualScene->ObjectList[x].DSTBlend=GL_DST_ALPHA; break;
			case 5: ActualScene->ObjectList[x].DSTBlend=GL_ONE_MINUS_DST_ALPHA; break;
			case 6: ActualScene->ObjectList[x].DSTBlend=GL_SRC_ALPHA; break;
			case 7: ActualScene->ObjectList[x].DSTBlend=GL_ONE_MINUS_SRC_ALPHA; break;
			case 8: ActualScene->ObjectList[x].DSTBlend=GL_SRC_ALPHA_SATURATE; break;
			}
		}
}

void __stdcall GUI_SetSuperShapeShape(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	SuperShapeData.Sphere=!ScrollerID;
}

void __stdcall GUI_SetSuperShapeRadial(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	SuperShapeData.RadFunc=ScrollerID;
}

void __stdcall GUI_SetSuperShapeVertical(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	SuperShapeData.VertFunc=ScrollerID;
}

void __stdcall GUI_SetObjDSTBlendCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{

				switch (ScrollerID)
				{
					case 0: if(ActualScene->ObjectList[x].DSTBlend==GL_ZERO) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 1: if(ActualScene->ObjectList[x].DSTBlend==GL_ONE) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 2: if(ActualScene->ObjectList[x].DSTBlend==GL_DST_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 3: if(ActualScene->ObjectList[x].DSTBlend==GL_ONE_MINUS_DST_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 4: if(ActualScene->ObjectList[x].DSTBlend==GL_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 5: if(ActualScene->ObjectList[x].DSTBlend==GL_ONE_MINUS_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 6: if(ActualScene->ObjectList[x].DSTBlend==GL_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 7: if(ActualScene->ObjectList[x].DSTBlend==GL_ONE_MINUS_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
					case 8: if(ActualScene->ObjectList[x].DSTBlend==GL_SRC_ALPHA_SATURATE) GUI_ButtonSelect(Interface,ScrollerID); break;
				}

				break;
			}
	}
}

void __stdcall GUI_XSwapCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].XSwap;
				break;
			}
	}
}

void __stdcall GUI_YSwapCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].YSwap;
				break;
			}
	}
}

void __stdcall GUI_SwapCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].Swap;
				break;
			}
	}
}

void __stdcall GUI_WireframeCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].Wireframe;
				break;
			}
	}
}

void __stdcall GUI_NormalCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].NormalsInverted;
				break;
			}
	}
}

void __stdcall GUI_DefCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].Shading==aDDict_DEFAULTSHADE;
				break;
			}
	}
}

void __stdcall GUI_FlatCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].Shading==aDDict_FLATSHADE;
				break;
			}
	}
}

void __stdcall GUI_GouraudCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ObjectList[x].Shading==aDDict_GOURAUDSHADE;
				break;
			}
	}
}


void __stdcall GUI_SetObjDepthMask(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].ZMask=ConstMaterialsMenu->ItemList[2].ButtonPushed;
			}
	}
}

void __stdcall GUI_SetObjDepthMaskCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ConstMaterialsMenu->ItemList[2].ButtonPushed=ActualScene->ObjectList[x].ZMask;
				break;
			}
	}
}

void __stdcall GUI_SetBackface(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].Backface=ConstMaterialsMenu->ItemList[8].ButtonPushed;
			}
	}
}

void __stdcall GUI_SetBackfaceCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ConstMaterialsMenu->ItemList[8].ButtonPushed=ActualScene->ObjectList[x].Backface;
				break;
			}
	}
}

void __stdcall GUI_SetBackfront(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].Backfront=ConstMaterialsMenu->ItemList[9].ButtonPushed;
			}
	}
}

void __stdcall GUI_SetBackfrontCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ConstMaterialsMenu->ItemList[9].ButtonPushed=ActualScene->ObjectList[x].Backfront;
				break;
			}
	}
}

void __stdcall GUI_SetAA(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].AA=ConstMaterialsMenu->ItemList[10].ButtonPushed;
			}
	}
}

void __stdcall GUI_SetAACheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ConstMaterialsMenu->ItemList[10].ButtonPushed=ActualScene->ObjectList[x].AA;
				break;
			}
	}
}


void __stdcall GUI_SetObjWireframe(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSwitch(Interface,ScrollerID);
	if (ActualScene)
	for (int x=0; x<ActualScene->ObjectNum; x++)
		if (ActualScene->ObjectList[x].ListItem->Selected)
			ActualScene->ObjectList[x].Wireframe=Interface->ItemList[ScrollerID].ButtonPushed;
}

void __stdcall GUI_CopyObject(cINTERFACE* Interface, int ScrollerID)
{
	WriteDebug("GUI_CopyObject\n");
	if (ActualScene)
	{
		WaitLeftButton=true;
		int i=ActualScene->ObjectNum;
	    WriteDebug("Ciklus\n");
		for (int x=0; x<i; x++)
			if (!ActualScene->ObjectList[x].Deleted && ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
			{
				ActualScene->ObjectList[x].ListItem->Selected=false;
				ActualScene->AddObject();
				WriteDebug("AddObject\n");
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
				if (ActualScene->ObjectList[x].Primitive!=aDDict_LOFT)
				GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],ActualScene->ObjectList[x].Primitive,ActualScene->ObjectList[x].Param1,ActualScene->ObjectList[x].Param2,ActualScene->ObjectList[x].Param3,ActualScene->ObjectList[x].Param4,ActualScene->ObjectList[x].Param5);
				else
				{
					OBJECT *a=NULL,*b=NULL;
					for (int y=0; y<ActualScene->ObjectNum; y++)
					{
						if (ActualScene->ObjectList[y].ID==ActualScene->ObjectList[x].Param1) a=&ActualScene->ObjectList[y];
						if (ActualScene->ObjectList[y].ID==ActualScene->ObjectList[x].Param2) b=&ActualScene->ObjectList[y];
					}
					GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],ActualScene->ObjectList[x].Primitive,(int)a,(int)b,ActualScene->ObjectList[x].Param3,ActualScene->ObjectList[x].Param4,ActualScene->ObjectList[x].Param5);
				}
				if (ActualScene->ObjectList[x].Primitive==aDDict_ARC || ActualScene->ObjectList[x].Primitive==aDDict_LINE)
				{
					ActualScene->ObjectList[ActualScene->ObjectNum-1].ArcListItem=ActualScene->ArcList->AddItem("",&ActualScene->ObjectList[ActualScene->ObjectNum-1],Scroller_SelectOnlyThisArc,nop,nop);
					ActualScene->ArcList->Last->Name=new char[10];
					memset(ActualScene->ArcList->Last->Name,0,10);
					memcpy(ActualScene->ArcList->Last->Name,ActualScene->ObjectList[x].ListItem->Name,strlen(ActualScene->ObjectList[x].ListItem->Name));
					ArcScroller1->ParentItem->ItemNum++;
					ArcScroller2->ParentItem->ItemNum++;
				}
				ActualScene->ObjectList[ActualScene->ObjectNum-1].AEpsilon=ActualScene->ObjectList[x].AEpsilon;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ZMask=ActualScene->ObjectList[x].ZMask;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=ActualScene->ObjectList[x].Primitive;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=ActualScene->ObjectList[x].Param1;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2=ActualScene->ObjectList[x].Param2;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3=ActualScene->ObjectList[x].Param3;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4=ActualScene->ObjectList[x].Param4;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param5=ActualScene->ObjectList[x].Param5;
				WriteDebug("j\n");
				WriteDebug("%d %d %d\n",(int)ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem,(int)ActualScene->ObjectList[x].ListItem,strlen(ActualScene->ObjectList[x].ListItem->Name));
				char *s=new char[strlen(ActualScene->ObjectList[x].ListItem->Name)+1];
				WriteDebug("k\n");
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name=s;
				WriteDebug("l\n");
				strcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name,ActualScene->ObjectList[x].ListItem->Name);
				WriteDebug("m\n");
				ActualScene->ObjectList[ActualScene->ObjectNum-1].SRCBlend=ActualScene->ObjectList[x].SRCBlend;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].DSTBlend=ActualScene->ObjectList[x].DSTBlend;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Wireframe=ActualScene->ObjectList[x].Wireframe;
				memcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView,ActualScene->ObjectList[x].ModelView,sizeof(MATRIX));
				memcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].TransformBuffer,ActualScene->ObjectList[x].TransformBuffer,sizeof(MATRIX));
				memcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].Inverted,ActualScene->ObjectList[x].Inverted,sizeof(MATRIX));
				memcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].IT,ActualScene->ObjectList[x].IT,sizeof(MATRIX));
				/*if (ActualScene->ObjectList[x].EmitterData)
				{
					ActualScene->ObjectList[ActualScene->ObjectNum-1].EmitterData=new EMITTERDATA;
					memcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].EmitterData,ActualScene->ObjectList[x].EmitterData,sizeof(EMITTERDATA));
				}*/
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Textured=ActualScene->ObjectList[x].Textured;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Material=ActualScene->ObjectList[x].Material;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].EnvMapped=ActualScene->ObjectList[x].EnvMapped;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].EnvMap=ActualScene->ObjectList[x].EnvMap;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].NormalsInverted=ActualScene->ObjectList[x].NormalsInverted;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Xtile=ActualScene->ObjectList[x].Xtile;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Ytile=ActualScene->ObjectList[x].Ytile;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].XSwap=ActualScene->ObjectList[x].XSwap;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].YSwap=ActualScene->ObjectList[x].YSwap;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Swap=ActualScene->ObjectList[x].Swap;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetX=ActualScene->ObjectList[x].OffsetX;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetY=ActualScene->ObjectList[x].OffsetY;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Backface=ActualScene->ObjectList[x].Backface;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Backfront=ActualScene->ObjectList[x].Backfront;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Shading=ActualScene->ObjectList[x].Shading;
				if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Shading!=aDDict_DEFAULTSHADE)
				for (int y=0; y<ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonNum; y++)
					ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonList[y].CurrentShading=ActualScene->ObjectList[x].Shading;


				ActualScene->ObjectList[ActualScene->ObjectNum-1].Color=ActualScene->ObjectList[x].Color;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateTextureCoordinates();
				ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateNormals();
				ActualScene->ObjectList[ActualScene->ObjectNum-1].CreateDisplayList();
				MainScroller->ParentItem->ItemNum++;
			}
	}
	WriteDebug("GUI_CopyObject_Done\n");
}

void __stdcall GUI_HideObjects(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
		for (int x=0;x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected) ActualScene->ObjectList[x].ListItem->Hidden=true;
	WaitLeftButton=true;
}

void __stdcall GUI_UnHideObjects(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
		for (int x=0;x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM) ActualScene->ObjectList[x].ListItem->Hidden=false;
	WaitLeftButton=true;
}

void __stdcall GUI_StartMapxform(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		MappedObject=-1;
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && !ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden && (ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM && ActualScene->ObjectList[x].Primitive!=aDDict_CLONE))
			{
				MappedObject=x;
				break;
			}
		if (MappedObject==-1) ErrorMessage="SELECT AN OBJECT FIRST!";
		else
		{
			if (ActualMaterial)
			{
				Maptransforming=true;
				Interface->ItemList[ScrollerID].ActiveBranch=true;
				Interface->ItemList[ScrollerID].ButtonPushed=true;
				for (int x=0; x<ActualScene->ObjectList[MappedObject].VertexNum; x++)
				{
					ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition=ActualScene->ObjectList[MappedObject].VertexList[x].Position;
				}
			}
			else 
				ErrorMessage="SELECT A MATERIAL FIRST!";
		}
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall GUI_CancelMapxform(cINTERFACE* Interface, int ScrollerID)
{
	Maptransforming=false;
	Interface->ParentItem->ActiveBranch=false;
	Interface->ParentItem->ButtonPushed=false;
}

void __stdcall GUI_SaveMapxform(cINTERFACE* Interface, int ScrollerID)
{
	GUI_CancelMapxform(Interface,ScrollerID);
	for (int x=0; x<ActualScene->ObjectList[MappedObject].VertexNum; x++)
	{
		ActualScene->ObjectList[MappedObject].VertexList[x].Position=ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition;
	}
	ActualScene->ObjectList[MappedObject].CalculateNormals();
	ActualScene->ObjectList[MappedObject].CreateDisplayList();
	ActualScene->AddObject();
	OBJECT *o=&ActualScene->ObjectList[ActualScene->ObjectNum-1];
	o->Primitive=aDDict_MAPXFORM;
	o->ListItem->Name="MAPXFORM";
	o->ListItem->Hidden=true;
	o->Xtile=MXTile;
	o->Ytile=MYTile;
	o->XSwap=MapTMenu->ItemList[0].ButtonPushed;
	o->YSwap=MapTMenu->ItemList[2].ButtonPushed;
	o->Swap=MapTMenu->ItemList[1].ButtonPushed;
	o->OffsetX=MOffx;
	o->OffsetY=MOffy;
	o->TargetObjectID=ActualScene->ObjectList[MappedObject].ID;
	memcpy(o->ModelView,MXMatrix,sizeof(MATRIX));
	o->Material=ActualMaterial;
	MainScroller->ParentItem->ItemNum++;
	
	for (int a=0; a<5; a++)
	if (ColorSelector->ItemList[a].ButtonPushed) o->MapXformColor=a;
	for (a=0; a<3; a++)
		if (MXPolySelect->ItemList[a].ButtonPushed) o->Param1=a;
	o->Param2=-1;
	o->Param3=-1;
	if ((MXPolySelect->ItemList[1].ButtonPushed || MXPolySelect->ItemList[2].ButtonPushed) && CurrentPolySelection)
	{
		o->Param2=PInterface->ParentItem->SelectedItem;
		o->Param3=BInterface->ParentItem->SelectedItem;
	}

	if (TransformSelector->ItemList[2].ButtonPushed) o->MapXformType=aDDict_SCALE;
	if (TransformSelector->ItemList[1].ButtonPushed) o->MapXformType=aDDict_ROTATE;
	if (TransformSelector->ItemList[0].ButtonPushed) o->MapXformType=aDDict_MOVE;
}

void __stdcall GUI_SetObjectTexMap(cINTERFACE* Interface, int ScrollerID)
{
	if (Maptransforming)
	{
		CalculateObjectVertexWeights(&ActualScene->ObjectList[MappedObject],ActualMaterial, MXTile, MYTile, MapTMenu->ItemList[0].ButtonPushed, MapTMenu->ItemList[2].ButtonPushed, MapTMenu->ItemList[1].ButtonPushed, MOffx, MOffy);
		int color;
		for (int a=0; a<5; a++)
		if (ColorSelector->ItemList[a].ButtonPushed) color=a;
		if (TransformSelector->ItemList[0].ButtonPushed) CalculateMapTransform(aDDict_MOVE,color,MXMatrix,MappedObject);
		if (TransformSelector->ItemList[1].ButtonPushed) CalculateMapTransform(aDDict_ROTATE,color,MXMatrix,MappedObject);
		if (TransformSelector->ItemList[2].ButtonPushed) CalculateMapTransform(aDDict_SCALE,color,MXMatrix,MappedObject);
		for (int x=0; x<3; x++)
			for (int y=0; y<4; y++)
			ModellerDisplay1->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[y].Interface->ItemList[1].Interface->ItemList[2].Interface->ItemList[x].ButtonPushed=TransformSelector->ItemList[2-x].ButtonPushed;

		/*for (x=0; x<ActualScene->ObjectList[MappedObject].VertexNum;x++)
		{
			ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition=ActualScene->ObjectList[MappedObject].VertexList[x].Position;
		}*/
			
		if (MXPolySelect->ItemList[0].ButtonPushed)
			for (int x=0; x<ActualScene->ObjectList[MappedObject].VertexNum;x++)
			{
				ActualScene->ObjectList[MappedObject].VertexList[x].Selected=true;
			}
		if (CurrentPolySelection)
		{
			if (MXPolySelect->ItemList[1].ButtonPushed) SelectObjVertices(&ActualScene->ObjectList[MappedObject],CurrentPolySelection, PolySelectID, true);
			if (MXPolySelect->ItemList[2].ButtonPushed) SelectObjVertices(&ActualScene->ObjectList[MappedObject],CurrentPolySelection, PolySelectID, false);
		}
	}
}

void __stdcall GUI_ButtonSelect_MXNull(cINTERFACE* Interface, int ScrollerID)
{
	GUI_ButtonSelect(Interface,ScrollerID);
	M_Identity(MXMatrix);
}

void __stdcall GUI_DoClone(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		int cnt=0;
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected && !ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
				cnt++;
		if (cnt)
		{
			ActualScene->AddObject();
			MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;

			OBJECT *o=&ActualScene->ObjectList[ActualScene->ObjectNum-1];
			o->Primitive=aDDict_CLONE;

			o->Param1=cnt;
			o->Param2=(int)new int[cnt];
			o->Param3=(int)new int[cnt];

			int a=0;
			for (int x=0; x<ActualScene->ObjectNum-1; x++)
			{
				if (ActualScene->ObjectList[x].ListItem->Selected && !ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
				{
					((int*)o->Param2)[a]=ActualScene->ObjectList[x].ID;
					((int*)o->Param3)[a]=(int)(&ActualScene->ObjectList[x]);
					a++;
				}
			}

			M_Identity(o->ModelView);
			
			ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="CLONE";
			for (ITEM *I=ActualScene->ItemList->List; I; I=I->Next) I->Selected=false;
			ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected=true;
			MainScroller->ParentItem->ItemNum++;

			WaitLeftButton=true;

		}
		else
			ErrorMessage="SELECT SOMETHING TO CLONE FIRST!";
	}
	else ErrorMessage="CREATE A SCENE FIRST!";	
}

void __stdcall GUI_SetSolid(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].DSTBlend=GL_ONE;
				ActualScene->ObjectList[x].SRCBlend=GL_ZERO;
				ActualScene->ObjectList[x].ZMask=true;
			}
}

void __stdcall GUI_SetGlenz(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].DSTBlend=GL_SRC_ALPHA;
				ActualScene->ObjectList[x].SRCBlend=GL_ONE;
				ActualScene->ObjectList[x].ZMask=false;
			}
}

void __stdcall GUI_SetAlpha(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].DSTBlend=GL_SRC_ALPHA;
				ActualScene->ObjectList[x].SRCBlend=GL_ONE_MINUS_SRC_ALPHA;
				ActualScene->ObjectList[x].ZMask=false;
			}
}

void __stdcall DoLinearSmooth(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
				ActualScene->AddObject();
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_LINEARSMOOTH;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="LINEAR";
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Hidden=true;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=ActualScene->ObjectList[x].ID;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2=MeshSmoothAmount;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID=ActualScene->ObjectList[x].ID;
				for (int y=0; y<MeshSmoothAmount; y++) ActualScene->ObjectList[x].MeshSmooth(true);
				MainScroller->ParentItem->ItemNum++;
				break;
			}
		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall DoButterflySmooth(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
				ActualScene->AddObject();
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_BUTTERFLYSMOOTH;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="BUTTERFLY";
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Hidden=true;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=ActualScene->ObjectList[x].ID;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2=MeshSmoothAmount;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID=ActualScene->ObjectList[x].ID;
				for (int y=0; y<MeshSmoothAmount; y++) ActualScene->ObjectList[x].MeshSmooth(false);
				MainScroller->ParentItem->ItemNum++;
				break;
			}
		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}

void __stdcall DoMeshBlur(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
				ActualScene->AddObject();
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_MESHBLUR;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="BLUR";
				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Hidden=true;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=ActualScene->ObjectList[x].ID;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2=MeshBlurAmount;
				ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID=ActualScene->ObjectList[x].ID;
				for (int y=0; y<MeshBlurAmount; y++) ActualScene->ObjectList[x].MeshBlur();
				MainScroller->ParentItem->ItemNum++;
				break;
			}
		WaitLeftButton=true;
	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}


void __stdcall GUI_RollBackBrush(cINTERFACE* Interface, int ScrollerID)
{
	if (CurrentPolySelection && CurrentPolySelection->List)
	{
		if (CurrentPolySelection->Last->Prev)
		{
			CurrentPolySelection->Last=CurrentPolySelection->Last->Prev;
			CurrentPolySelection->Last->Next=NULL;
			PolySelectInterface->ItemList[4].ItemNum--;
		}
		else
		{
			CurrentPolySelection->List=CurrentPolySelection->Last=NULL;
			PolySelectInterface->ItemList[4].ItemNum--;
		}
		PolySelectInterface->ItemList[4].SelectedItem=min(PolySelectInterface->ItemList[4].SelectedItem,PolySelectInterface->ItemList[4].ItemNum-1);
		WaitLeftButton=true;
	}
}

void __stdcall GUI_SetOffset(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				ActualScene->ObjectList[x].OffsetX=((int)((mx-Interface->ItemList[ScrollerID].x1)/(float)(Interface->ItemList[ScrollerID].x2-Interface->ItemList[ScrollerID].x1)*1024+10240000))%1024/1024.0f;
				ActualScene->ObjectList[x].OffsetY=((int)((my-Interface->ItemList[ScrollerID].y1)/(float)(Interface->ItemList[ScrollerID].y2-Interface->ItemList[ScrollerID].y1)*1024+10240000))%1024/1024.0f;
				Interface->ItemList[ScrollerID].Offx=ActualScene->ObjectList[x].OffsetX;
				Interface->ItemList[ScrollerID].Offy=ActualScene->ObjectList[x].OffsetY;
				ActualScene->ObjectList[x].CalculateTextureCoordinates();
				ActualScene->ObjectList[x].CreateDisplayList();
			}
	}
}

void __stdcall GUI_SetMOffset(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		for (int x=0; x<ActualScene->ObjectNum; x++)
			if (ActualScene->ObjectList[x].ListItem->Selected)
			{
				MOffx=((int)((mx-Interface->ItemList[ScrollerID].x1)/(float)(Interface->ItemList[ScrollerID].x2-Interface->ItemList[ScrollerID].x1)*1024+10240000))%1024/1024.0f;
				MOffy=((int)((my-Interface->ItemList[ScrollerID].y1)/(float)(Interface->ItemList[ScrollerID].y2-Interface->ItemList[ScrollerID].y1)*1024+10240000))%1024/1024.0f;
				Interface->ItemList[ScrollerID].Offx=MOffx;
				Interface->ItemList[ScrollerID].Offy=MOffy;
			}
	}
}

void __stdcall GUI_SetColorDiscard(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		GUI_ButtonSwitch(Interface,ScrollerID);
		ActualScene->ColorDiscard=Interface->ItemList[ScrollerID].ButtonPushed;
	}
}

void __stdcall GUI_SetColorDiscardCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		Interface->ItemList[ScrollerID].ButtonPushed=ActualScene->ColorDiscard;
	}
}

//#ifndef Public_Release
void __stdcall GUI_DoFur(cINTERFACE* Interface, int ScrollerID)
{
	if (ActualScene)
	{
		int SelectedCount=0;
		int ObjNum=ActualScene->ObjectNum;
		for (int x=0; x<ObjNum; x++)
		{
			int p=ActualScene->ObjectList[x].Primitive;
			if (ActualScene->ObjectList[x].ListItem->Selected && p<aDDict_MAPXFORM && p!=aDDict_ARC && p!=aDDict_LINE && p!=aDDict_CLONE)
			{
				SelectedCount++;

				ActualScene->AddObject();
				OBJECT *o=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

				GeneratePrimitive(o,aDDict_FUR,FurSize,FurPlanes,FurHeight,ActualScene->ObjectList[x].ID,(int)(&ActualScene->ObjectList[x]));

				ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="FUR";
				MainScroller->ParentItem->ItemNum++;

				o->ListItem->Selected=true;
			}
			ActualScene->ObjectList[x].ListItem->Selected=false;
		}

		if (!SelectedCount)	ErrorMessage="YOU NEED AN OBJECT SELECTED FOR THIS TO WORK!";
		else WaitLeftButton=true;

	}
	else ErrorMessage="CREATE A SCENE FIRST!";
}
//#endif

cINTERFACE *ImportMenu;

void BuildModellerGUI(cINTERFACE *ModellerGUI)
{
	memset(&SuperShapeData,0,sizeof(SUPERSHAPE));
	SuperShapeData.Sphere=true;
	LoftShape=LoftTrack=NULL;

	Preview=new OBJECT();
	Preview->DisplayList=glGenLists(1);
	Preview->WireDisplayList=glGenLists(1);

	SceneListScroller=new ITEMLIST();
	MaterialList=new ITEMLIST();
	ModellFileList=new ITEMLIST();
	ModellerDisplays=ModellerGUI->AddSubInterface();
	ModellerDisplays->ParentItem->OnWheel=CheckTransform;

	ModellerDisplay1=ModellerDisplays->AddSubInterface()->Add3dView(50,50,450,350,"TOP VIEW (zoom shortcut: j/m)",true,nop,nop,nop,nop);
	ModellerDisplay1->ParentItem->Cam.Fov=45;
	ModellerDisplay1->ParentItem->Cam.Up.z=-1;
	ModellerDisplay1->ParentItem->Cam.Eye.y=10;
	ModellerDisplay1->ParentItem->i.x=1;
	ModellerDisplay1->ParentItem->j.z=-1;
	ModellerDisplay1->ParentItem->k=1;
	ModellerDisplay1->ParentItem->i1=0;
	ModellerDisplay1->ParentItem->j1=2;
	ModellerDisplay2=ModellerDisplays->AddSubInterface()->Add3dView(450,50,850,350,"FRONT VIEW (zoom shortcut: j/m)",true,nop,nop,nop,nop);
	ModellerDisplay2->ParentItem->Cam.Fov=45;
	ModellerDisplay2->ParentItem->Cam.Up.y=1;
	ModellerDisplay2->ParentItem->Cam.Eye.z=10;
	ModellerDisplay2->ParentItem->i.x=1;
	ModellerDisplay2->ParentItem->j.y=1;
	ModellerDisplay2->ParentItem->k=2;
	ModellerDisplay2->ParentItem->i1=0;
	ModellerDisplay2->ParentItem->j1=1;
	ModellerDisplay3=ModellerDisplays->AddSubInterface()->Add3dView(50,350,450,650,"LEFT VIEW (zoom shortcut: j/m)",true,nop,nop,nop,nop);
	ModellerDisplay3->ParentItem->Cam.Fov=45;
	ModellerDisplay3->ParentItem->Cam.Up.y=1;
	ModellerDisplay3->ParentItem->Cam.Eye.x=-10;
	ModellerDisplay3->ParentItem->i.z=1;
	ModellerDisplay3->ParentItem->j.y=1;
	ModellerDisplay3->ParentItem->k=0;
	ModellerDisplay3->ParentItem->i1=2;
	ModellerDisplay3->ParentItem->j1=1;
	ModellerDisplay4=ModellerDisplays->AddSubInterface()->Add3dView(450,350,850,650,"CAMERA VIEW",false,nop,nop,nop,nop);
	ModellerDisplay4->ParentItem->CamGrid=glGenLists(1);
	glNewList(ModellerDisplay4->ParentItem->CamGrid,GL_COMPILE);
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
	ModellerDisplay4->ParentItem->Cam.Fov=45;
	ModellerDisplay4->ParentItem->Cam.Up.y=1;
	ModellerDisplay4->ParentItem->Cam.Eye.z=10;
	ModellerDisplay4->ParentItem->Cam.Eye.y=7;

	MainScroller=ModellerGUI->AddItemList(881,50,116,17,NULL,"");
	MainScrollerData=&(ModellerGUI->ItemList[ModellerGUI->ItemNum-4].data);
	ModellerGUI->AddSkinnedButton(862,49,AllButton,"SELECT ALL OBJECTS ( a )",Modeller_SelectAll,nop,nop,nop,'a');
	ModellerGUI->AddSkinnedButton(862,68,NoButton,"DESELECT ALL OBJECTS ( n )",Modeller_DeSelectAll,nop,nop,nop,'n');
	ModellerGUI->AddSkinnedButton(862,87,InvertButton,"INVERT SELECTION ( i )",Modeller_InvertSelect,nop,nop,nop,'i');
	ModellerGUI->AddSkinnedButton(862,106,StoreButton,"SAVE SELECTION",nop,nop,nop,nop);
	ModellerGUI->AddSkinnedButton(862,125,LoadButton,"LOAD SELECTION",nop,nop,nop,nop);
	ModellerGUI->AddSkinnedButton(862,144,DeleteSelectionButton,"DELETE SELECTED SELECTION",nop,nop,nop,nop);
	ModellerGUI->AddSkinnedButton(862,163,Del__Button,"DELETE SELECTED OBJECTS ( DEL )",Modeller_DeleteSelected,nop,nop,nop,VK_DELETE);
	ModellerGUI->AddSkinnedButton(862,182,HideButtonSmall,"HIDE SELECTED OBJECTS ( h )",GUI_HideObjects,nop,nop,nop,'h');
	ModellerGUI->AddSkinnedButton(862,201,UnHideButtonSmall,"UNHIDE SELECTED OBJECTS ( u )",GUI_UnHideObjects,nop,nop,nop,'u');
	ModellerGUI->AddSkinnedButton(862,220,UndoButton,"UNDO",nop,nop,nop,nop);

	cINTERFACE *MainMenu=ModellerGUI->AddSubInterface();

	ScenesMenu=MainMenu->AddSkinnedButton(879+2,250,ScenesButton,"SCENE SETUP MENU ( 1 )",GUI_ButtonSelect,nop,nop,nop,'1')->AddSubInterface();
	ObjectsMenu=MainMenu->AddSkinnedButton(898+2,250,ObjectsButton,"CREATE OBJECTS MENU ( 2 )",GUI_ButtonSelect,nop,nop,nop,'2');
	MaterialsMenu=MainMenu->AddSkinnedButton(917+2,250,MaterialsButton,"MATERIALS MENU ( 3 )",GUI_ButtonSelect,nop,nop,nop,'3');
	//LightsMenu=MainMenu->AddSkinnedButton(917+2,250,LightsButton,"LIGHTS MENU ( 4 )",GUI_ButtonSelect_LightMenuMain,nop,nop,nop,'4')->AddSubInterface();
	//ParticlesMenu=MainMenu->AddSkinnedButton(936+2,250,ParticlesButton,"PARTICLES MENU ( 5 )",GUI_ButtonSelect,nop,nop,nop,'5');
	EffectsMenu=MainMenu->AddSkinnedButton(936+2,250,EffectsButton,"EFFECTS MENU ( 4 )",GUI_ButtonSelect,nop,nop,nop,'6')->AddSubInterface();
	cINTERFACE *EditMenu=MainMenu->AddSkinnedButton(955+2,250,EditButton,"OBJECT EDIT MENU ( 5 )",GUI_ButtonSelect,nop,nop,nop,'7');
	cINTERFACE *FileMenu=MainMenu->AddSkinnedButton(974+2,250,FileButton,"FILE MENU ( 6 )",GUI_ButtonSelect,nop,nop,nop,'8')->AddSubInterface();
	MainMenu->ItemList[5].ButtonPushed=true;
	MainMenu->SetActiveBranch(5);

	cINTERFACE *CamerasMenu=ScenesMenu->AddSkinnedButton(895,275,CamerasButton,"CAMERA SETUP MENU",GUI_ButtonSelect,nop,nop,nop)->AddSubInterface();
	cINTERFACE *EnvironmentMenu=ScenesMenu->AddSkinnedButton(895,291,EnvironmentButton,"SCENE ENVIRONMENT SETUP MENU",GUI_ButtonSelect,nop,nop,nop)->AddSubInterface();
	EnvironmentMenu->AddSkinnedButton(895,315,DiscCol,"DISCARD OBJECT COLORS FOR COLOR ANIMATION IN LAYOUTER",GUI_SetColorDiscard,nop,nop,GUI_SetColorDiscardCheck);
	EnvironmentMenu->AddSkinnedButton(895+15,333,LightingButton,"TURN LIGHTING ON/OFF",GUI_SetLighting,nop,nop,nop);
	EnvironmentMenu->ItemList[EnvironmentMenu->ItemNum-1].ButtonPushed=true;

	CamerasMenu->AddFloatBar(862,340,136,GUI_NONE,&(ModellerDisplay4->ParentItem->Cam.Fov),0.0f,180.0f,1.0f,45.0f,"CAM FOV: ","SET CAMERA FOV");

	ScenesMenu->SetActiveBranch(1);
	ScenesMenu->ItemList[1].ButtonPushed=true;

	ObjectsMenu->AddSkinnedButton(890,275,BoxButton,"CREATE BOX",CreateBox,nop,nop,nop);
	ObjectsMenu->AddSkinnedButton(928,275,IcosaederButton,"CREATE ICOSAEDER",nop,nop,nop,nop);
	ObjectsMenu->AddSkinnedButton(966,275,DodecaederButton,"CREATE DODECAEDER",nop,nop,nop,nop);

	ObjectsMenu->AddSkinnedButton(928,301,SphereButton,"CREATE SPHERE",CreateSphere,nop,nop,nop);
	ObjectsMenu->AddByteBar(862,321,136,GUI_NONE,&SphereX,3,255,12,"SPHERE-X: ","SET SPHERE X RESOLUTION");
	ObjectsMenu->AddByteBar(862,340,136,GUI_NONE,&SphereY,3,255,12,"SPHERE-Y: ","SET SPHERE Y RESOLUTION");

	ObjectsMenu->AddSkinnedButton(909,367,CylinderButton,"CREATE CYLINDER",CreateCylinder,nop,nop,nop);
	ObjectsMenu->AddSkinnedButton(947,367,TopButton,"CLOSE CYLINDER ENDS/DON'T",GUI_ButtonSwitch,nop,nop,nop,(void**)&CylinderEnd);
	ObjectsMenu->ItemList[ObjectsMenu->ItemNum-1].ButtonPushed=true;
	ObjectsMenu->AddByteBar(862,387,136,GUI_NONE,&CylinderX,3,255,12,"CYLINDER-X: ","SET CYLINDER X RESOLUTION");
	ObjectsMenu->AddByteBar(862,406,136,GUI_NONE,&CylinderY,1,255,12,"CYLINDER-Y: ","SET CYLINDER Y RESOLUTION");

	ObjectsMenu->AddSkinnedButton(909,432,ConeButton,"CREATE CONE",CreateCone,nop,nop,nop);
	ObjectsMenu->AddSkinnedButton(947,432,TopButton,"CLOSE CONE END/DON'T",GUI_ButtonSwitch,nop,nop,nop,(void**)&ConeEnd);
	ObjectsMenu->ItemList[ObjectsMenu->ItemNum-1].ButtonPushed=true;
	ObjectsMenu->AddByteBar(862,452,136,GUI_NONE,&ConeX,3,255,12,"CONE-X: ","SET CONE RESOLUTION");
	ObjectsMenu->AddByteBar(862,468,136,GUI_NONE,&ConeY,1,255,1,"CONE-Y: ","SET CONE RESOLUTION");

	ObjectsMenu->AddSkinnedButton(928,478+16,GridButton,"CREATE PLANE",CreatePlane,nop,nop,nop);
	ObjectsMenu->AddByteBar(862,498+16,136,GUI_NONE,&GridX,1,255,10,"PLANE-X: ","SET PLANE X RESOLUTION");
	ObjectsMenu->AddByteBar(862,517+16,136,GUI_NONE,&GridY,1,255,10,"PLANE-Y: ","SET PLANE Y RESOLUTION");

	ObjectsMenu->AddSkinnedButton(928,543+16,ArcButton,"CREATE ARC",CreateArc,nop,nop,nop);
	ObjectsMenu->AddByteBar(862,563+16,136,GUI_NONE,&ArcX,1,255,12,"ARC-X: ","SET ARC RESOLUTION");
	ObjectsMenu->AddIntBar(862,582+16,136,GUI_NONE,&ArcY,0,360,360,"ARC-Y: ","SET ARC DEGREE");

	ObjectsMenu->AddSkinnedButton(928,609+16,LineButton,"CREATE LINE",CreateLine,nop,nop,nop);
	ObjectsMenu->AddByteBar(862,629+16,136,GUI_NONE,&LineX,1,255,10,"LINE-X: ","SET LINE RESOLUTION");

	ObjectsMenu->AddSkinnedButton(928,659+16,ArcButton,"CREATE CIRCLE",CreateCircle,nop,nop,nop);
	ObjectsMenu->AddByteBar(862,679+16,136,GUI_NONE,&CircleX,1,255,24,"CIRCLE-X: ","SET CIRCLE RESOLUTION");
	ObjectsMenu->AddByteBar(862,698+16,136,GUI_NONE,&CircleInner,0,255,192,"CIRCLE-R: ","SET CIRCLE INNER RADIUS");


	VarMaterialsMenu=MaterialsMenu->AddSkinnedButton(853,275,VariablesButton,"VARIABLE MATERIAL SETTINGS",GUI_ButtonSelect,nop,nop,nop);
	MaterialsMenu->ItemList[MaterialsMenu->ItemNum-1].ButtonPushed=true;
	MaterialsMenu->SetActiveBranch(MaterialsMenu->ItemNum-1);
	ConstMaterialsMenu=MaterialsMenu->AddSkinnedButton(938,275,ConstantsButton,"CONSTANT MATERIAL SETTINGS (BLENDING)",GUI_ButtonSelect,nop,nop,nop);

	OffsetInterface=VarMaterialsMenu->AddTextureDisplay(874,272+23,1002,400+23,&MaterialHandle,GUI_SetOffset,nop,nop,GUI_SelectMaterial);
	VarMaterialsMenu->AddItemList(865,403+23,137,6,MaterialList,"MATERIAL SELECTOR");
	MaterialListItemNum=VarMaterialsMenu->ItemNum-4;
	VarMaterialsMenu->AddIntBar(862,475+23,136,GUI_RED,&ObjRed,0,255,255,"OBJECT RED: ","SET OBJECT RED COLOR",GUI_SetObjRed,GUI_SetObjRedAfter);
	VarMaterialsMenu->AddIntBar(862,494+24,136,GUI_GREEN,&ObjGreen,0,255,255,"OBJECT GREEN: ","SET OBJECT GREEN COLOR",GUI_SetObjGreen,GUI_SetObjGreenAfter);
	VarMaterialsMenu->AddIntBar(862,512+25,136,GUI_BLUE,&ObjBlue,0,255,255,"OBJECT BLUE: ","SET OBJECT BLUE COLOR",GUI_SetObjBlue,GUI_SetObjBlueAfter);
	VarMaterialsMenu->AddIntBar(862,531+26,136,GUI_NONE,&ObjAlpha,0,255,255,"OBJECT ALPHA: ","SET OBJECT ALPHA COLOR",GUI_SetObjAlpha,GUI_SetObjAlphaAfter);

	VarMaterialsMenu->AddSkinnedButton(870,551+27,SetButton,"SET TEXTURE TO SELECTED OBJECTS (o)",GUI_SetTexture,nop,nop,nop,'o');
	VarMaterialsMenu->AddSkinnedButton(915,551+27,NoneButton,"SET NO TEXTURE TO SELECTED OBJECTS (p)",GUI_ClearTexture,nop,nop,nop,'p');
	//MaterialsMenu->AddSkinnedButton(960,551+27,DelButton,"DELETE SELECTED MATERIAL",nop,nop,nop,nop);
	VarMaterialsMenu->AddSkinnedButton(870,567+27,EnvButton,"SET ENVIRONMENT MAP TO SELECTED OBJECTS",GUI_SetEnvMap,nop,nop,nop);
	VarMaterialsMenu->AddSkinnedButton(915,567+27,NoEnvButton,"SET NO ENVIRONMENT MAP TO SELECTED OBJECTS",GUI_ClearEnvmap,nop,nop,nop);
	VarMaterialsMenu->AddSkinnedButton(960,567+27,NormalButton,"INVERT NORMALS OF SELECTED OBJECTS",GUI_InvertNormal,nop,nop,GUI_NormalCheck);
	VarMaterialsMenu->AddSkinnedButton(870,583+27,DefaultButton,"SET THEIR DEFAULT SHADING TO SELECTED OBJECTS",GUI_SetDefaultShading,nop,nop,GUI_DefCheck);
	VarMaterialsMenu->AddSkinnedButton(915,583+27,FlatButton,"SET FLAT SHADING TO SELECTED OBJECTS",GUI_SetFlatShading,nop,nop,GUI_FlatCheck);
	VarMaterialsMenu->AddSkinnedButton(960,583+27,GouraudButton,"SET GOURAUD SHADING TO SELECTED OBJECTS",GUI_SetGouraudShading,nop,nop,GUI_GouraudCheck);
	VarMaterialsMenu->AddIntBar(862,599+27,136,GUI_NONE,&XTile,0,255,1,"X-TILING: ","SET TEXTURE X-TILING",GUI_SetXTile,GUI_SetXTileAfter);
	VarMaterialsMenu->AddIntBar(862,618+28,136,GUI_NONE,&YTile,0,255,1,"Y-TILING: ","SET TEXTURE Y-TILING",GUI_SetYTile,GUI_SetYTileAfter);
	VarMaterialsMenu->AddSkinnedButton(870,637+29,XSwapButton,"SWAP TEXTURE ON X-AXIS ON SELECTED OBJECTS",GUI_SwapX,nop,nop,GUI_XSwapCheck);
	VarMaterialsMenu->AddSkinnedButton(915,637+29,SwapButton,"SWAP TEXTURE X-Y AXIS ON SELECTED OBJECTS",GUI_Swap,nop,nop,GUI_SwapCheck);
	VarMaterialsMenu->AddSkinnedButton(960,637+29,YSwapButton,"SWAP TEXTURE ON Y-AXIS ON SELECTED OBJECTS",GUI_SwapY,nop,nop,GUI_YSwapCheck);
	VarMaterialsMenu->AddSkinnedButton(896,653+29,WireframeButton,"TURN WIREFRAME DISPLAY ON/OFF",GUI_SetObjWireframe,nop,nop,GUI_WireframeCheck);

	cINTERFACE *SrcBlend=ConstMaterialsMenu->AddText(877,303,"SOURCE BLEND MODE");
	ConstMaterialsMenu->ItemList[0].ActiveBranch=true;
	SrcBlend->AddRadioButton(853,310,"ZERO","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,325,"ONE","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->ItemList[SrcBlend->ItemNum-1].ButtonPushed=true;
	SrcBlend->AddRadioButton(853,340,"BACKGROUND COLOR","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,355,"1-BACKGROUND COLOR","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,370,"BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,385,"1-BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,400,"SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,415,"1-SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);
	SrcBlend->AddRadioButton(853,430,"SOURCE ALPHA SATURATE","TEST RADIOBUTTON",GUI_SetObjDSTBlend,nop,nop,GUI_SetObjDSTBlendCheck);

	cINTERFACE *DstBlend=ConstMaterialsMenu->AddText(861,463,"BACKGROUND BLEND MODE");
	ConstMaterialsMenu->ItemList[1].ActiveBranch=true;
	DstBlend->AddRadioButton(853,470,"ZERO","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->ItemList[DstBlend->ItemNum-1].ButtonPushed=true;
	DstBlend->AddRadioButton(853,485,"ONE","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->AddRadioButton(853,500,"SOURCE COLOR","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->AddRadioButton(853,515,"1-SOURCE COLOR","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->AddRadioButton(853,530,"BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->AddRadioButton(853,545,"1-BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->AddRadioButton(853,560,"SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);
	DstBlend->AddRadioButton(853,575,"1-SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetObjSRCBlend,nop,nop,GUI_SetObjSRCBlendCheck);

	ConstMaterialsMenu->AddSkinnedButton(910,596,ZbufferButton,"SWITCH DEPTH MASKING ON/OFF FOR OBJECT",GUI_SetObjDepthMask,nop,nop,GUI_SetObjDepthMaskCheck);
	//ConstMaterialsMenu->AddSkinnedButton(880+57,596,AlphaEpsilonButton,"SWITCH ALPHA EPSILON TEST ON/OFF FOR OBJECT",GUI_ButtonSwitch,nop,nop,nop);
	ConstMaterialsMenu->AddByteBar(862,613,136,GUI_NONE,&MAEpsilon,0,255,0,"AEPSILON: ","CHANGE ALPHA TEST PASS VALUE",GUI_SetObjAEpsilon,GUI_SetObjAEpsilonAfter);
	ConstMaterialsMenu->AddText(882,648,"DEFAULT SETTINGS");
	ConstMaterialsMenu->AddSkinnedButton(853,656,SolidButton,"SET NON TRANSPARENT BLENDING",GUI_SetSolid,nop,nop,nop);
	ConstMaterialsMenu->AddSkinnedButton(910,656,GlenzButton,"SET GLENZ TRANSPARENT BLENDING",GUI_SetGlenz,nop,nop,nop);
	ConstMaterialsMenu->AddSkinnedButton(967,656,AlphaMatButton,"SET ALPHA MAPPED TRANSPARENT BLENDING",GUI_SetAlpha,nop,nop,nop);
	ConstMaterialsMenu->AddButton(882,675,982,691,16,11,"BACKFACE","TURN BACKFACE CULLING ON/OFF",GUI_SetBackface,nop,nop,GUI_SetBackfaceCheck);
	ConstMaterialsMenu->AddButton(882,694,982,710,16,11,"BACKFRONT","SWITCH BACKFACE CULL AFFECTED POLYS",GUI_SetBackfront,nop,nop,GUI_SetBackfrontCheck);
	//ConstMaterialsMenu->AddButton(882,694+19,982,710+19,16,11,"ANTIALIAS","TURN ANTIALIASING ON FOR THIS OBJECT",GUI_SetAA,nop,nop,GUI_SetAACheck);

	/*LightMenus[0]=LightsMenu->AddSkinnedButton(870,275,Light1Button,"LIGHT 1 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[1]=LightsMenu->AddSkinnedButton(887,275,Light2Button,"LIGHT 2 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[2]=LightsMenu->AddSkinnedButton(904,275,Light3Button,"LIGHT 3 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[3]=LightsMenu->AddSkinnedButton(921,275,Light4Button,"LIGHT 4 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[4]=LightsMenu->AddSkinnedButton(938,275,Light5Button,"LIGHT 5 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[5]=LightsMenu->AddSkinnedButton(955,275,Light6Button,"LIGHT 6 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[6]=LightsMenu->AddSkinnedButton(972,275,Light7Button,"LIGHT 7 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	LightMenus[7]=LightsMenu->AddSkinnedButton(989,275,Light8Button,"LIGHT 8 PROPERTIES",GUI_ButtonSelect_LightMenu,nop,nop,nop);
	BuildLightGUI(LightMenus[0],1);
	BuildLightGUI(LightMenus[1],2);
	BuildLightGUI(LightMenus[2],3);
	BuildLightGUI(LightMenus[3],4);
	BuildLightGUI(LightMenus[4],5);
	BuildLightGUI(LightMenus[5],6);
	BuildLightGUI(LightMenus[6],7);
	BuildLightGUI(LightMenus[7],8);

	LightsMenu->ItemList[0].ButtonPushed=true;
	LightsMenu->SetActiveBranch(0);*/

	cINTERFACE* MeshSmoothMenu=EffectsMenu->AddSkinnedButton(853,275,MeshSmoothButton,"MESH SMOOTH",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* MeshBlurMenu=EffectsMenu->AddSkinnedButton(938,275,MeshBlurButton,"MESH BLUR",GUI_ButtonSelect,nop,nop,nop);
	MapXformMenu=EffectsMenu->AddSkinnedButton(853,291,MapXformButton,"MAPPED TRANSFORMATION",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* ExtrudeMenu=EffectsMenu->AddSkinnedButton(938,291,ExtrudeButton,"EXTRUDE",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* LoftMenu=EffectsMenu->AddSkinnedButton(853+28,323,LoftButton,"LOFT",GUI_ButtonSelect,nop,nop,nop);
//#ifndef Public_Release
	cINTERFACE* FurMenu=EffectsMenu->AddSkinnedButton(910+28,323,FurButton,"""FUR""",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* SuperShapeMenu=EffectsMenu->AddSkinnedButton(853,307,SuperShapeButton,"SUPERSHAPE",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE* SuperArcMenu=EffectsMenu->AddSkinnedButton(938,307,SuperArcButton,"SUPERARC",GUI_ButtonSelect,nop,nop,nop);
//#endif

	MeshSmoothMenu->AddByteBar(862,320+36,136,GUI_NONE,&MeshSmoothAmount,1,5,1,"SMOOTH AMOUNT: ","SET SMOOTH AMOUNT");
	MeshSmoothMenu->AddSkinnedButton(877,345+36,LinearButton,"LINEAR MESH SMOOTH",DoLinearSmooth,nop,nop,nop);
	MeshSmoothMenu->AddSkinnedButton(938,345+36,ButterflyButton,"BUTTERFLY MESH SMOOTH",DoButterflySmooth,nop,nop,nop);

	MeshBlurMenu->AddByteBar(862,320+36,136,GUI_NONE,&MeshBlurAmount,1,5,1,"BLUR AMOUNT: ","SET BLUR AMOUNT");
	MeshBlurMenu->AddSkinnedButton(895,345+36,MeshBlurButton,"DO MESH BLUR",DoMeshBlur,nop,nop,nop);

	MapXformMenu->AddTextureDisplay(874,311+36,1002,439+36,&MaterialHandle,GUI_SetMOffset,nop,nop,GUI_SelectMaterialMapXform);
	MapXformMenu->AddItemList(865,442+36,137,6,MaterialList,"MATERIAL SELECTOR");
	MaterialListItemNumMapXform=MapXformMenu->ItemNum-4;
	TransformSelector=MapXformMenu->AddSubInterface();
	TransformSelector->AddSkinnedButton(909,520+36,MoveButton,"SET MOVE TRANSFORM",GUI_ButtonSelect_MXNull,nop,nop,nop);
	TransformSelector->AddSkinnedButton(928,520+36,RotateButton,"SET ROTATE TRANSFORM",GUI_ButtonSelect_MXNull,nop,nop,nop);
	TransformSelector->AddSkinnedButton(947,520+36,ScaleButton,"SET SCALE TRANSFORM",GUI_ButtonSelect_MXNull,nop,nop,nop);
	TransformSelector->ItemList[0].ButtonPushed=true;
	ColorSelector=MapXformMenu->AddSubInterface();
	ColorSelector->AddSkinnedButton(899,539+36,RedButton,"SET RED CHANNEL",GUI_ButtonSelect,nop,nop,nop);
	ColorSelector->AddSkinnedButton(918,539+36,GreenButton,"SET GREEN CHANNEL",GUI_ButtonSelect,nop,nop,nop);
	ColorSelector->AddSkinnedButton(937,539+36,BlueButton,"SET BLUE CHANNEL",GUI_ButtonSelect,nop,nop,nop);
	ColorSelector->AddSkinnedButton(956,539+36,AlphaButton,"SET ALPHA CHANNEL",GUI_ButtonSelect,nop,nop,nop);
	ColorSelector->AddSkinnedButton(909,558+36,CombinedButton,"SET ALL CHANNELS COMBINED",GUI_ButtonSelect,nop,nop,nop);
	ColorSelector->ItemList[0].ButtonPushed=true;
	MapTMenu=MapXformMenu->AddSkinnedButton(895,583+36,MapXformButton,"MAPPED TRANSFORMATION",GUI_StartMapxform,nop,nop,nop);
	MapTMenu->ParentItem->ActiveBranch=false;
	MapTMenu->AddSkinnedButton(870,637+24+36,XSwapButton,"SWAP MAP TEXTURE ON X-AXIS",GUI_ButtonSwitch,nop,nop,nop);
	MapTMenu->AddSkinnedButton(915,637+24+36,SwapButton,"SWAP MAP TEXTURE X-Y AXIS",GUI_ButtonSwitch,nop,nop,nop);
	MapTMenu->AddSkinnedButton(960,637+24+36,YSwapButton,"SWAP MAP TEXTURE ON Y-AXIS",GUI_ButtonSwitch,nop,nop,nop);
	MapTMenu->AddSkinnedButton(881,603+36,FinaliseButton,"FINALIZE AND SAVE TRANSFORMATION",GUI_SaveMapxform,nop,nop,GUI_SetObjectTexMap);
	MapTMenu->AddSkinnedButton(938,603+36,CancelButton,"CANCEL TRANSFORMATION",GUI_CancelMapxform,nop,nop,nop);
	MapTMenu->AddIntBar(862,620+36,136,GUI_NONE,&MXTile,0,255,1,"X-TILE: ","X TILING OF MAP TEXTURE");
	MapTMenu->AddIntBar(862,640+36,136,GUI_NONE,&MYTile,0,255,1,"Y-TILE: ","Y TILING OF MAP TEXTURE");
	MapTMenu->AddText(880,690+36,"VERTEX SELECTION:");
	MXPolySelect=MapTMenu->AddSubInterface();
	MXPolySelect->AddSkinnedButton(870,690+40,MXNoneButton,"NO VERTEX SELECTION",GUI_ButtonSelect,nop,nop,nop);
	MXPolySelect->ItemList[0].ButtonPushed=true;
	MXPolySelect->AddSkinnedButton(915,690+40,MXBigButton,"SELECT VERTICES (WITH BORDER)",GUI_ButtonSelect,nop,nop,nop);
	MXPolySelect->AddSkinnedButton(960,690+40,MXSmallButton,"SELECT VERTICES (WITHOUT BORDER)",GUI_ButtonSelect,nop,nop,nop);

	ArcScroller1=LoftMenu->AddItemList(881,351+36,116,10,NULL,"");
	ArcScroller1Data=&(LoftMenu->ItemList[LoftMenu->ItemNum-4].data);

	ArcScroller2=LoftMenu->AddItemList(881,491+36,116,10,NULL,"");
	ArcScroller2Data=&(LoftMenu->ItemList[LoftMenu->ItemNum-4].data);

	LoftMenu->AddText(894,344+36,"SELECT TRACK:\0");
	LoftMenu->AddText(899,484+36,"SELECT MESH:\0");

	LoftMenu->AddSkinnedButton(909,611+36,LoftButton,"LOFT",StartLoft,nop,nop,nop);
	CreateLoftI=LoftMenu->AddSubInterface();
	CreateLoftI->AddSkinnedButton(881,630+36,FinaliseButton,"FINALIZE AND CREATE LOFT",CreateLoft,nop,nop,nop);
	CreateLoftI->AddSkinnedButton(937,630+36,CancelButton,"CANCEL LOFT",StopLoft,nop,nop,nop);
	CreateLoftI->ParentItem->ActiveBranch=false;

//#ifndef Public_Release
	FurMenu->AddIntBar(862,320+36,136,GUI_NONE,&FurSize,0,10000,50,"""FUR"" SIZE: ","SIZE OF THE PLANES");
	FurMenu->AddIntBar(862,320+20+36,136,GUI_NONE,&FurHeight,0,10000,50,"""FUR"" HEIGHT: ","HEIGHT OF THE PLANES");
	FurMenu->AddByteBar(862,320+20+20+36,136,GUI_NONE,&FurPlanes,1,5,1,"PLANES PER FACE: ","""FUR"" PLANES PER FACE");
	FurMenu->AddSkinnedButton(910,400+36,FurButton,"DO FUR",GUI_DoFur,nop,nop,nop);

	SphereOrToroid=SuperShapeMenu->AddSubInterface();
	SphereOrToroid->AddRadioButton(853+16+4,303+36,"SPHERE","ZERO VERTICAL FUNCTION",GUI_SetSuperShapeShape,nop,nop,nop);
	SphereOrToroid->AddRadioButton(905+29+4,303+36,"TOROID","LINEAR VERTICAL FUNCTION z=z*a*phi",GUI_SetSuperShapeShape,nop,nop,nop);
	SphereOrToroid->ItemList[0].ButtonPushed=true;
	SuperShapeMenu->AddFloatBar(862,317+36,136,GUI_NONE,&SuperShapeData.Trad0,0,100,0.001f,1,"TORUS R0: ","TORUS RADIUS",nop);
	SuperShapeMenu->AddFloatBar(862,336+36,136,GUI_NONE,&SuperShapeData.TradPhi,0,100,0.001f,0,"TORUS R(phi): ","TORUS RADIUS 2",nop);

	SuperShapeMenu->AddText(895,323+75,"SUPERSHAPE 1");
	SuperShapeMenu->AddByteBar(853,323+76,136/2,GUI_NONE,&SuperShapeData.SuperShape1.mint,0,255,0,"M INT: ","M INTEGER PART",nop);
	SuperShapeMenu->AddByteBar(871+136/2,323+76,136/2,GUI_NONE,&SuperShapeData.SuperShape1.mfloat,0,255,0,"M FLT: ","M FLOAT PART",nop);
	SuperShapeMenu->AddFloatBar(853,342+76,136/2,GUI_NONE,&SuperShapeData.SuperShape1.a,-5,6,0.001f,1,"A: ","A VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(871+136/2,342+76,136/2,GUI_NONE,&SuperShapeData.SuperShape1.b,-5,6,0.001f,1,"B: ","B VARIABLE",nop);

	SuperShapeMenu->AddFloatBar(862,361+76,136,GUI_NONE,&SuperShapeData.SuperShape1.n1,-100,100,0.001f,1,"N1: ","N1 VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(862,380+76,136,GUI_NONE,&SuperShapeData.SuperShape1.n2,-100,100,0.001f,1,"N2: ","N2 VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(862,399+76,136,GUI_NONE,&SuperShapeData.SuperShape1.n3,-100,100,0.001f,1,"N3: ","N3 VARIABLE",nop);

	RadFunc=SuperShapeMenu->AddSubInterface();
	RadFunc->AddRadioButton(853+9,418+75,"UNITY","UNITY RADIAL FUNCTION",GUI_SetSuperShapeRadial,nop,nop,nop);
	RadFunc->AddRadioButton(905+9,418+75,"ARCHM","ARCHIMEDES RADIAL FUNCTION r=r*a*phi",GUI_SetSuperShapeRadial,nop,nop,nop);
	RadFunc->AddRadioButton(963+9,418+75,"LOG","LOGARITHMIC RADIAL FUNCTION r=r*a*exp(b*phi)",GUI_SetSuperShapeRadial,nop,nop,nop);
	RadFunc->ItemList[0].ButtonPushed=true;
	SuperShapeMenu->AddFloatBar(853,      432+75,136/2,GUI_NONE,&SuperShapeData.Rada,0,10,0.001f,1,"rA: ","RADIAL A VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(871+136/2,432+75,136/2,GUI_NONE,&SuperShapeData.Radb,0,10,0.001f,1,"rB: ","RADIAL B VARIABLE",nop);
	VertFunc=SuperShapeMenu->AddSubInterface();
	VertFunc->AddRadioButton(853+29,451+75,"ZERO","ZERO VERTICAL FUNCTION",GUI_SetSuperShapeVertical,nop,nop,nop);
	VertFunc->AddRadioButton(905+29,451+75,"LINEAR","LINEAR VERTICAL FUNCTION z=z*a*phi",GUI_SetSuperShapeVertical,nop,nop,nop);
	VertFunc->ItemList[0].ButtonPushed=true;
	SuperShapeMenu->AddFloatBar(862,465+75,136,GUI_NONE,&SuperShapeData.Verta,0,10,0.001f,1,"vA: ","VERTICAL A VARIABLE",nop);


	SuperShapeMenu->AddText(895,320+213+36,"SUPERSHAPE 2");
	SuperShapeMenu->AddByteBar(853,323+213+36,136/2,GUI_NONE,&SuperShapeData.SuperShape2.mint,0,255,0,"M INT: ","M INTEGER PART",nop);
	SuperShapeMenu->AddByteBar(871+136/2,323+213+36,136/2,GUI_NONE,&SuperShapeData.SuperShape2.mfloat,0,255,0,"M FLT: ","M FLOAT PART",nop);
	SuperShapeMenu->AddFloatBar(853,342+213+36,136/2,GUI_NONE,&SuperShapeData.SuperShape2.a,-5,6,0.001f,1,"A: ","A VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(871+136/2,342+213+36,136/2,GUI_NONE,&SuperShapeData.SuperShape2.b,-5,6,0.001f,1,"B: ","B VARIABLE",nop);

	SuperShapeMenu->AddFloatBar(862,361+213+36,136,GUI_NONE,&SuperShapeData.SuperShape2.n1,-100,100,0.001f,1,"N1: ","N1 VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(862,380+213+36,136,GUI_NONE,&SuperShapeData.SuperShape2.n2,-100,100,0.001f,1,"N2: ","N2 VARIABLE",nop);
	SuperShapeMenu->AddFloatBar(862,399+213+36,136,GUI_NONE,&SuperShapeData.SuperShape2.n3,-100,100,0.001f,1,"N3: ","N3 VARIABLE",nop);

	SuperShapeMenu->AddText(912,491+150+36,"GENERAL");

	SuperShapeMenu->AddIntBar(853,494+150+36,136/2,GUI_NONE,&SuperShapeData.Rangex1,-1800,1800,-180,"Rx1: ","START RANGE",nop);
	SuperShapeMenu->AddIntBar(871+136/2,494+150+36,136/2,GUI_NONE,&SuperShapeData.Rangex2,-1800,1800,180,"Rx2: ","END RANGE",nop);
	
	SuperShapeMenu->AddIntBar(853,513+150+36,136/2,GUI_NONE,&SuperShapeData.Rangey1,-1800,1800,-90,"Ry1: ","START RANGE",nop);
	SuperShapeMenu->AddIntBar(871+136/2,513+150+36,136/2,GUI_NONE,&SuperShapeData.Rangey2,-1800,1800,90,"Ry2: ","END RANGE",nop);

	SuperShapeMenu->AddByteBar(853,532+150+36,136/2,GUI_NONE,&SuperShapeData.Xres,2,255,24,"X RES: ","X RESOLUTION",nop);
	SuperShapeMenu->AddByteBar(871+136/2,532+150+36,136/2,GUI_NONE,&SuperShapeData.Yres,2,255,24,"Y RES: ","Y RESOLUTION",nop);

	SuperShapeMenu->AddSkinnedButton(938-85/2,701+36,SuperShapeButton,"LOFT",StartSuperShape,nop,nop,nop);
	SuperShapeMenuI=SuperShapeMenu->AddSubInterface();
	SuperShapeMenuI->AddSkinnedButton(884,717+36,FinaliseButton,"FINALIZE AND CREATE SUPERSHAPE",CreateSuperShape,nop,nop,nop);
	SuperShapeMenuI->AddSkinnedButton(940,717+36,CancelButton,"CANCEL SUPERSHAPE",StopSuperShape,nop,nop,nop);
	SuperShapeMenuI->ParentItem->ActiveBranch=false;
//#endif
	//int rangex1,rangex2,rangey1,rangey2,sshapex,sshapey;


	EffectsMenu->ItemList[0].ButtonPushed=true;
	EffectsMenu->SetActiveBranch(0);

	ImportMenu=FileMenu->AddSkinnedButton(893,275,ImportButton,"IMPORT SCENE",GUI_GetModellFileList,nop,nop,nop);
	cINTERFACE *ScenesFileMenu=FileMenu->AddSkinnedButton(938,275,Scenes_Button,"NEW SCENE, EXPORT SCENE, DELETE SCENE, CLEAR SCENE",GUI_ButtonSelect,nop,nop,nop);

	ImportMenu->AddItemList(865,295,137,18,ModellFileList,"SELECT A FILE:");
	ImportMenu->AddSkinnedButton(895,505,ImportButtonBig,"IMPORT SCENE",ImportScene,nop,nop,nop);
	ImportMenu->ItemList[ImportMenu->ItemNum-1].data=&ImportMenu->ItemList[ImportMenu->ItemNum-5].SelectedItem;
	GUI_GetModellFileList(ImportMenu,0);

	GUI_GetModellFileList(ImportMenu,0);
	FileMenu->ItemList[1].ButtonPushed=true;
	FileMenu->SetActiveBranch(1);

	SceneScroller=ScenesFileMenu->AddItemList(865,295,137,14,SceneListScroller,"");
	ScenesFileMenu->AddSkinnedButton(893,460,NewButton,"CREATE NEW SCENE",GUI_ReadLine,nop,nop,Modeller_NewSceneCallback);
	ScenesFileMenu->AddSkinnedButton(938,460,DelButton,"DELETE SCENE",nop,nop,nop,nop);
	ScenesFileMenu->AddSkinnedButton(893,476,ClearButton,"DELETE EVERYTHING FROM SCENE",GUI_ClearScene,nop,nop,nop);
	ScenesFileMenu->AddSkinnedButton(938,476,ExportButton,"EXPORT SCENE",GUI_ReadLine,nop,nop,SaveScene);

	PolySelections=new ITEMLIST();
	PolySelectInterface=ModellerGUI->AddSubInterface();
	PInterface=PolySelectInterface->AddItemList(70,660,130,9,PolySelections,"SELECT POLYSELECTION");
	BInterface=PolySelectInterface->AddItemList(220,660,130,9,NULL,"SELECT COMMAND");

	//PolySelectInterface->AddSkinnedButton(50,659,VertexSelectButton,"CREATE NEW VERTEX SELECTION",GUI_ReadLine,nop,nop,GUI_AddNewVertexSelect);
	PolySelectInterface->AddSkinnedButton(50,678,PolySelectButton,"CREATE NEW POLY SELECTION",GUI_ReadLine,nop,nop,GUI_AddNewPolySelect);
	PolySelectInterface->AddSkinnedButton(50,697,Del__Button,"DELETE SELECTION",nop,nop,nop,nop);
	PolySelectInterface->AddSkinnedButton(50,716,PolySelectBrushButton,"START/STOP POLY/VERTEX SELECTION ( s )",GUI_PolySwitch,nop,nop,nop,'s');

	BrushOperator=PolySelectInterface->AddSubInterface();
	BrushOperator->AddSkinnedButton(380,678,AdditiveButton,"ADDITIVE SELECTION ( x )",GUI_ButtonSelect,nop,nop,nop,'x');
	BrushOperator->AddSkinnedButton(380,697,SubstractiveButton,"SUBSTRACTIVE SELECTION ( c )",GUI_ButtonSelect,nop,nop,nop,'c');
	BrushOperator->AddSkinnedButton(380,716,XorButton,"XOR SELECTION ( v )",GUI_ButtonSelect,nop,nop,nop,'v');
	BrushOperator->AddSkinnedButton(380,740,UndoButton,"ROLL BACK BRUSH ( f )",GUI_RollBackBrush,nop,nop,nop,'f');
	BrushOperator->ItemList[0].ButtonPushed=true;
	BrushOperator->SetActiveBranch(0);

	BrushList=PolySelectInterface->AddSubInterface();
	cINTERFACE *VertexBrush=BrushList->AddSkinnedButton(400,659,VertexSelectButton,"SET VERTEX SELECTOR BRUSH",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *SelectBrush=BrushList->AddSkinnedButton(419,659,PolySelectBrushButton,"SET STANDARD SELECTOR BRUSH",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *RandomBrush=BrushList->AddSkinnedButton(438,659,RandomBrushButton,"SET RANDOM SELECTOR BRUSH",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *MapBrush=BrushList->AddSkinnedButton(457,659,TextureBrushButton,"SET MAP SELECTOR BRUSH",GUI_ButtonSelect,nop,nop,nop);
	BrushList->ItemList[0].ButtonPushed=true;
	BrushList->SetActiveBranch(0);

	VertexBrush->AddIntBar(410,698,100,GUI_NONE,&BrushSize,1,255,1,"SIZE: ","SELECTION AREA SIZE");

	SelectBrush->AddIntBar(410,698,100,GUI_NONE,&BrushSize,1,255,1,"SIZE: ","SELECTION AREA SIZE");

	RandomBrush->AddIntBar(410,679,100,GUI_NONE,&RndMax,1,65535,1,"MAX: ","HIGHEST POLYGON ID TO BE SELECTED BY RANDOM (OBJ POLYNUM BY DEFAULT)");
	RandomBrush->AddIntBar(410,698,100,GUI_NONE,&RndSeed,1,255,1,"SEED: ","RANDOM SEED");
	RandomBrush->AddIntBar(410,717,100,GUI_NONE,&RndCount,1,65535,1,"COUNT: ","POLYGON COUNT TO BE SELECTED BY RANDOM");

	PolySelectInterface->AddSkinnedButton(495,659,AllButton,"SELECT ALL",nop,nop,nop,nop);
	PolySelectInterface->AddSkinnedButton(514,659,NoButton,"SELECT NONE",nop,nop,nop,nop);
	PolySelectInterface->AddSkinnedButton(533,659,InvertButton,"INVERT SELECTION",nop,nop,nop,nop);

	cINTERFACE *Controls=ModellerGUI->AddSubInterface();
	Controls->ParentItem->ActiveBranch=true;
	Controls->AddSkinnedButton(795,659,CopyButton,"COPY SELECTED OBJECTS ( c )",GUI_CopyObject,nop,nop,nop,'c');
	Controls->AddSkinnedButton(795,675,CloneButton,"CLONE SELECTED OBJECTS",GUI_DoClone,nop,nop,nop);
	/*Controls->AddSkinnedButton(795,691,HideButton,"HIDE SELECTED OBJECTS ( h )",GUI_HideObjects,nop,nop,nop,'h');
	Controls->AddSkinnedButton(795,707,UnHideButton,"UNHIDE SELECTED OBJECTS ( u )",GUI_UnHideObjects,nop,nop,nop,'u');*/

	/*////////////////////// CSAK A KODEREKNEK ;) ////////////////////////
	ScenesFileMenu->ItemList[ScenesFileMenu->ItemNum-4].Reading=true;
	CommandLineEnter=true;
	CommandLine="TEST";
	Modeller_NewSceneCallback(ScenesFileMenu,ScenesFileMenu->ItemNum-4);
	////////////////////////////////////////////////////////////////////*/

	StatusBar3=ModellerGUI->AddSubInterface();
	StatusBar3->AddFloatProgressBar(407,8,440,GUI_NONE,&UndoStatus2,0,1,0,"GENERATING TEXTURES... ","",nop,nop,nop,nop);
	StatusBar3->ParentItem->ActiveBranch=false;
}
