#include "EffectTemplates.h"
#include "IntroEditorEffects.h"
#include "Layouter.h"

ITEMLIST *EffectList;
cINTERFACE *EventParameterInterface;

unsigned char test;

void EFFECTTEMPLATEDESCRIPTOR::AddItem()
{
	if (ItemCapacity==ItemNum)
	{
		ItemCapacity=(ItemCapacity+10)*2;
		EFFECTPARAMETERTYPE *Buffer=new EFFECTPARAMETERTYPE[ItemCapacity];
		memset(Buffer,0,ItemCapacity*sizeof(EFFECTPARAMETERTYPE));
		memcpy(Buffer,ItemList,ItemNum*sizeof(EFFECTPARAMETERTYPE));
		delete ItemList;
		ItemList=Buffer;
	}
	ItemNum++;
}


void EFFECTTEMPLATEDESCRIPTOR::AddByteBar(unsigned char Min, unsigned char Max, unsigned char Default, int Color, char *Text, char *ToolTip)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_BYTE;
	ItemList[ItemNum-1].iMin=Min;
	ItemList[ItemNum-1].iMax=Max;
	ItemList[ItemNum-1].iDefault=Default;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].Color=Color;
}

void EFFECTTEMPLATEDESCRIPTOR::AddIntBar(int Min, int Max, int Default, int Color, char *Text, char *ToolTip)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_INT;
	ItemList[ItemNum-1].iMin=Min;
	ItemList[ItemNum-1].iMax=Max;
	ItemList[ItemNum-1].iDefault=Default;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].Color=Color;
}

void EFFECTTEMPLATEDESCRIPTOR::AddFloatBar(float Min, float Max, float Default, int Color, char *Text, char *ToolTip)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_FLOAT;
	ItemList[ItemNum-1].fMin=Min;
	ItemList[ItemNum-1].fMax=Max;
	ItemList[ItemNum-1].fDefault=Default;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].Color=Color;
}

void EFFECTTEMPLATEDESCRIPTOR::AddRenderTextureSelector(){	AddItem();	ItemList[ItemNum-1].Type=EFFECT_RTXT;}

void EFFECTTEMPLATEDESCRIPTOR::AddBooleanButton(int t1, int t2, bool Default, char *Text, char *ToolTip)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_BOOL;
	ItemList[ItemNum-1].iMin=t1;
	ItemList[ItemNum-1].iMax=t2;
	ItemList[ItemNum-1].iDefault=Default;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
}

void EFFECTTEMPLATEDESCRIPTOR::AddVariableFunction(int t1, int t2, char *Text, char *ToolTip, GUICALLBACK Function)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_VARFUNCTION;
	ItemList[ItemNum-1].iMin=t1;
	ItemList[ItemNum-1].iMax=t2;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].Function=Function;
}

void EFFECTTEMPLATEDESCRIPTOR::AddConstantFunction(int t1, int t2, char *Text, char *ToolTip,  GUICALLBACK Function)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_CNSTFUNCTION;
	ItemList[ItemNum-1].iMin=t1;
	ItemList[ItemNum-1].iMax=t2;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].Function=Function;
}

void EFFECTTEMPLATEDESCRIPTOR::AddVariableText(int Pos, char *Text)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_VARTEXT;
	ItemList[ItemNum-1].iMin=Pos;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip="\0";
}

void EFFECTTEMPLATEDESCRIPTOR::AddConstantText(int Pos, char *Text)
{
	AddItem();
	ItemList[ItemNum-1].Type=EFFECT_CONSTTEXT;
	ItemList[ItemNum-1].iMin=Pos;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip="\0";
}


void EFFECTTEMPLATEDESCRIPTOR::AddMaterialSelector()	{	AddItem();	ItemList[ItemNum-1].Type=EFFECT_MATSEL;}
void EFFECTTEMPLATEDESCRIPTOR::AddBlendModeSelector()	{	AddItem();	ItemList[ItemNum-1].Type=EFFECT_BLENDSEL;}
void EFFECTTEMPLATEDESCRIPTOR::AddSceneSelector()		{	AddItem();	ItemList[ItemNum-1].Type=EFFECT_SCENESEL;}
void EFFECTTEMPLATEDESCRIPTOR::AddAnimSelector()		{	AddItem();	ItemList[ItemNum-1].Type=EFFECT_ANIMSEL;}
void EFFECTTEMPLATEDESCRIPTOR::AddCameraSelector()		{	AddItem();	ItemList[ItemNum-1].Type=EFFECT_CAMSEL;}

void __stdcall SetLastItemListToWorldListScroller(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID-4].data=(void*)WorldListScroller;
	ScrollerInterface->ItemList[ScrollerID-4].ItemNum=WorldListScroller->ItemNum;
}

void __stdcall SetLastItemListToSelectedWorldCameras(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	if (SelectedEvent)
	{
		if (((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World)
		{
			ScrollerInterface->ItemList[ScrollerID-4].data=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->CameraList;
			ScrollerInterface->ItemList[ScrollerID-4].ItemNum=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->CameraList->ItemNum;
			((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->CamID=ScrollerInterface->ItemList[ScrollerID-4].SelectedItem;
		}
	}
}

void __stdcall SetLastItemListToSelectedWorldAnims(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	if (SelectedEvent)
	{
		if (((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World)
		{
			ScrollerInterface->ItemList[ScrollerID-4].data=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->AnimList;
			ScrollerInterface->ItemList[ScrollerID-4].ItemNum=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->AnimList->ItemNum;
			((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->AnimID=ScrollerInterface->ItemList[ScrollerID-4].SelectedItem;
		}
	}
}

void __stdcall SetLastItemListToSelectedMaterial(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID-4].data=MaterialList;
	ScrollerInterface->ItemList[ScrollerID-4].ItemNum=MaterialList->ItemNum;
}


void __stdcall GUI_SetEvtDSTBlend(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	if (ScrollerInterface->ParentItem->data)
	{
		switch (ScrollerID)
		{
			case 0: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ZERO; break;
			case 1: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE; break;
			case 2: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_DST_COLOR; break;
			case 3: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE_MINUS_DST_COLOR; break;
			case 4: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_DST_ALPHA; break;
			case 5: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE_MINUS_DST_ALPHA; break;
			case 6: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_SRC_ALPHA; break;
			case 7: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE_MINUS_SRC_ALPHA; break;
			case 8: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_SRC_ALPHA_SATURATE; break;
			}
	}
}

void __stdcall GUI_SetEvtDSTBlendCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ParentItem->data)
	{
		switch (ScrollerID)
		{
			case 0: if (*(GLuint*)(Interface->ParentItem->data)==GL_ZERO) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 1: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 2: if (*(GLuint*)(Interface->ParentItem->data)==GL_DST_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 3: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE_MINUS_DST_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 4: if (*(GLuint*)(Interface->ParentItem->data)==GL_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 5: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE_MINUS_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 6: if (*(GLuint*)(Interface->ParentItem->data)==GL_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 7: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE_MINUS_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
			case 8: if (*(GLuint*)(Interface->ParentItem->data)==GL_SRC_ALPHA_SATURATE) GUI_ButtonSelect(Interface,ScrollerID); break;
		}
	}
}

void __stdcall GUI_SetEvtSRCBlend(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	if (ScrollerInterface->ParentItem->data)
	{
		switch (ScrollerID)
		{
			case 0: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ZERO; break;
			case 1: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE; break;
			case 2: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_SRC_COLOR; break;
			case 3: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE_MINUS_SRC_COLOR; break;
			case 4: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_DST_ALPHA; break;
			case 5: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE_MINUS_DST_ALPHA; break;
			case 6: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_SRC_ALPHA; break;
			case 7: *(GLuint*)(ScrollerInterface->ParentItem->data)=GL_ONE_MINUS_SRC_ALPHA; break;
		}
	}
}

void __stdcall GUI_SetEvtSRCBlendCheck(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ParentItem->data)
	{
		switch (ScrollerID)
		{
		case 0: if (*(GLuint*)(Interface->ParentItem->data)==GL_ZERO) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 1: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 2: if (*(GLuint*)(Interface->ParentItem->data)==GL_SRC_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 3: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE_MINUS_SRC_COLOR) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 4: if (*(GLuint*)(Interface->ParentItem->data)==GL_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 5: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE_MINUS_DST_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 6: if (*(GLuint*)(Interface->ParentItem->data)==GL_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
		case 7: if (*(GLuint*)(Interface->ParentItem->data)==GL_ONE_MINUS_SRC_ALPHA) GUI_ButtonSelect(Interface,ScrollerID); break;
		}
	}
}

void __stdcall GUI_SetToLastTextureSlot(cINTERFACE* Interface, int ScrollerID)
{
	if (Interface->ItemList[ScrollerID-1].Interface->ItemList[1].data)
	{
		Interface->ItemList[ScrollerID].Texture=&RenderTextures[*(char*)Interface->ItemList[ScrollerID-1].Interface->ItemList[1].data].TexImage;
		Interface->ItemList[ScrollerID].LayouterDisplay=true;
		Interface->ItemList[ScrollerID].tx1=RenderTextures[*(char*)Interface->ItemList[ScrollerID-1].Interface->ItemList[1].data].x1;
		Interface->ItemList[ScrollerID].ty1=RenderTextures[*(char*)Interface->ItemList[ScrollerID-1].Interface->ItemList[1].data].y1;
		Interface->ItemList[ScrollerID].tx2=RenderTextures[*(char*)Interface->ItemList[ScrollerID-1].Interface->ItemList[1].data].x2;
		Interface->ItemList[ScrollerID].ty2=RenderTextures[*(char*)Interface->ItemList[ScrollerID-1].Interface->ItemList[1].data].y2;
	}
}

void GenerateEffectGUIFromTemplate(EFFECT *Effect, EFFECTTEMPLATEDESCRIPTOR *Template)
{
	cINTERFACE *WorkInterface=EventParameterInterface->AddSubInterface();
	WorkInterface->ParentItem->ParentInterface->SetActiveBranch(WorkInterface->ParentItem->InterfaceItemId);

	if (Template->ItemNum)
	{
		bool StaticTab=false,VariableTab=false;
		Effect->DataSize=0;
		int x;
		for (x=0; x<Template->ItemNum; x++) 
		{
			if (Template->ItemList[x].Type<=EFFECT_VARFUNCTION) VariableTab=true; else StaticTab=true;
			switch (Template->ItemList[x].Type)
			{
			case EFFECT_BYTE: 
			case EFFECT_RTXT: 
			case EFFECT_BOOL: 
				Effect->DataSize+=1; break;
			case EFFECT_INT: 
			case EFFECT_FLOAT: 
			case EFFECT_MATSEL: 
			case EFFECT_SCENESEL: 
			case EFFECT_ANIMSEL: 
			case EFFECT_CAMSEL: 
				Effect->DataSize+=4; break;
			case EFFECT_BLENDSEL: 
				Effect->DataSize+=8; break;
			default: ;
			}
		}

		cINTERFACE *ConstantInterface,*VariableInterface;
		if (VariableTab && StaticTab)
		{
			VariableInterface=WorkInterface->AddSkinnedButton(854,145,VariablesButton,"VARIABLE VALUES",GUI_ButtonSelect,nop,nop,nop);
			ConstantInterface=WorkInterface->AddSkinnedButton(939,145,ConstantsButton,"CONSTANT VALUES",GUI_ButtonSelect,nop,nop,nop);
			WorkInterface->SetActiveBranch(0);
			VariableInterface->ParentItem->ButtonPushed=true;
		}
		else 
		{
			ConstantInterface=VariableInterface=WorkInterface->AddSubInterface();
			ConstantInterface->ParentItem->ActiveBranch=true;
		}

		int *VPos=new int;
		*VPos=165;
		if (!StaticTab || !VariableTab) *VPos=149;
		int *CPos=new int;
		*CPos=*VPos;
		if (!StaticTab || !VariableTab) 
		{
			delete CPos;
			CPos=VPos;
		}

		for (x=0; x<Template->ItemNum; x++) 
		{
			switch (Template->ItemList[x].Type)
			{
			case EFFECT_BYTE: 
				VariableInterface->AddByteBar(861,*VPos,136,Template->ItemList[x].Color,NULL,Template->ItemList[x].iMin,Template->ItemList[x].iMax,Template->ItemList[x].iDefault,Template->ItemList[x].Text,Template->ItemList[x].ToolTip);
				*VPos+=20;
				break;
			case EFFECT_INT: 
				VariableInterface->AddIntBar(861,*VPos,136,Template->ItemList[x].Color,NULL,Template->ItemList[x].iMin,Template->ItemList[x].iMax,Template->ItemList[x].iDefault,Template->ItemList[x].Text,Template->ItemList[x].ToolTip);
				*VPos+=20;
				break;
			case EFFECT_FLOAT: 
				VariableInterface->AddFloatBar(861,*VPos,136,Template->ItemList[x].Color,NULL,Template->ItemList[x].fMin,Template->ItemList[x].fMax,0.01f,Template->ItemList[x].fDefault,Template->ItemList[x].Text,Template->ItemList[x].ToolTip);
				*VPos+=20;
				break;
			case EFFECT_BOOL:
				*CPos+=5;
				ConstantInterface->AddButton(861,*CPos,861+136+19,*CPos+16,Template->ItemList[x].iMin,Template->ItemList[x].iMax,Template->ItemList[x].Text,Template->ItemList[x].ToolTip,GUI_ButtonSwitch_DataChange,nop,nop,GUI_CheckByteSwitch);
				*CPos+=18;
				break;
			case EFFECT_VARFUNCTION:
				*VPos+=7;
				VariableInterface->AddButton(861,*VPos,861+136+19,*VPos+16,Template->ItemList[x].iMin,Template->ItemList[x].iMax,Template->ItemList[x].Text,Template->ItemList[x].ToolTip,Template->ItemList[x].Function,nop,nop,nop);
				*VPos+=22;
				break;
			case EFFECT_CNSTFUNCTION:
				*CPos+=7;
				ConstantInterface->AddButton(861,*CPos,861+136+19,*CPos+16,Template->ItemList[x].iMin,Template->ItemList[x].iMax,Template->ItemList[x].Text,Template->ItemList[x].ToolTip,Template->ItemList[x].Function,nop,nop,nop);
				*CPos+=22;
				break;
			case EFFECT_BLENDSEL:
				{
					*CPos+=15;
					cINTERFACE *SrcBlend=ConstantInterface->AddText(877,*CPos,"SOURCE BLEND MODE");
					*CPos+=7;
					ConstantInterface->ItemList[ConstantInterface->ItemNum-1].ActiveBranch=true;
					SrcBlend->AddRadioButton(853,*CPos,"ZERO","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"ONE","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->ItemList[SrcBlend->ItemNum-1].ButtonPushed=true;
					SrcBlend->AddRadioButton(853,*CPos,"BACKGROUND COLOR","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"1-BACKGROUND COLOR","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"1-BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"1-SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=15;
					SrcBlend->AddRadioButton(853,*CPos,"SOURCE ALPHA SATURATE","TEST RADIOBUTTON",GUI_SetEvtDSTBlend,nop,nop,GUI_SetEvtDSTBlendCheck);
					*CPos+=33;

					cINTERFACE *DstBlend=ConstantInterface->AddText(861,*CPos,"BACKGROUND BLEND MODE");
					*CPos+=7;
					ConstantInterface->ItemList[ConstantInterface->ItemNum-1].ActiveBranch=true;
					DstBlend->AddRadioButton(853,*CPos,"ZERO","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					DstBlend->ItemList[DstBlend->ItemNum-1].ButtonPushed=true;
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"ONE","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"SOURCE COLOR","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"1-SOURCE COLOR","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"1-BACKGROUND ALPHA","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
					DstBlend->AddRadioButton(853,*CPos,"1-SOURCE ALPHA","TEST RADIOBUTTON",GUI_SetEvtSRCBlend,nop,nop,GUI_SetEvtSRCBlendCheck);
					*CPos+=15;
				}
				break;
			case EFFECT_SCENESEL:
				ConstantInterface->AddItemList(860,*CPos,140,8,(ITEMLIST*)0x25472ded,"SELECT A WORLD:");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-4].data=0;
				ConstantInterface->AddText(0,0,"");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-1].OnWheel=SetLastItemListToWorldListScroller;
				*CPos+=100;
				break;
			case EFFECT_CAMSEL:
				ConstantInterface->AddItemList(860,*CPos,140,8,NULL,"SELECT A CAMERA:");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-4].data=0;
				ConstantInterface->AddText(0,0,"");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-1].OnWheel=SetLastItemListToSelectedWorldCameras;
				*CPos+=100;
				break;
			case EFFECT_ANIMSEL:
				ConstantInterface->AddItemList(860,*CPos,140,8,NULL,"SELECT AN ANIMATION:");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-4].data=0;
				ConstantInterface->AddText(0,0,"");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-1].OnWheel=SetLastItemListToSelectedWorldAnims;
				*CPos+=100;
				break;
			case EFFECT_RTXT:
				ConstantInterface->AddByteBar(861,*CPos,136,GUI_NONE,NULL,0,5,0,"SLOT: ","SELECT RENDERTEXTURE SLOT");
				*CPos+=30;
				ConstantInterface->AddTextureDisplay(861+12,*CPos,861+12+128,*CPos+128,NULL,nop,nop,nop,GUI_SetToLastTextureSlot);
				*CPos+=140;
				break;
			case EFFECT_VARTEXT:
				*VPos+=10;
				VariableInterface->AddText(Template->ItemList[x].iMin,*VPos,Template->ItemList[x].Text);
				*VPos+=10;
				break;
			case EFFECT_CONSTTEXT:
				*CPos+=10;
				ConstantInterface->AddText(Template->ItemList[x].iMin,*CPos,Template->ItemList[x].Text);
				*CPos+=10;
				break;
			case EFFECT_MATSEL:
				*CPos+=5;
				ConstantInterface->AddItemList(860,*CPos,140,8,MaterialList,"SELECT A MATERIAL:");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-4].data=MaterialList;
				ConstantInterface->AddText(0,0,"");
				ConstantInterface->ItemList[ConstantInterface->ItemNum-1].OnWheel=SetLastItemListToSelectedMaterial;
				*CPos+=100;
				break;
			}
		}
		delete VPos;
		if (!(!StaticTab || !VariableTab)) delete CPos;

	}
}

void SetDefaultSettings(EVENT *Event,EFFECT *Effect)
{
	if (Event->EffectData)
	{
		unsigned char *DataPos=(unsigned char*)Event->EffectData;
		memset(Event->EffectData,0,Effect->DataSize);
		for (int x=0;x<Effect->TemplateDescriptor->ItemNum;x++)
		{
			switch (Effect->TemplateDescriptor->ItemList[x].Type)
			{
				case EFFECT_BYTE: 
					*DataPos=(unsigned char)Effect->TemplateDescriptor->ItemList[x].iDefault;
					DataPos+=1;
					break;
				case EFFECT_RTXT: 
					*DataPos=0;
					DataPos+=1;
					break;
				case EFFECT_BOOL: 
					*DataPos=(unsigned char)Effect->TemplateDescriptor->ItemList[x].iDefault;
					DataPos+=1;
					break;
				case EFFECT_INT: 
					*(int*)DataPos=Effect->TemplateDescriptor->ItemList[x].iDefault;
					DataPos+=4;
					break;
				case EFFECT_FLOAT: 
					*(float*)DataPos=Effect->TemplateDescriptor->ItemList[x].fDefault;
					DataPos+=4;
					break;
				case EFFECT_MATSEL: 
					DataPos+=4;
					break;
				case EFFECT_SCENESEL:
					if (Worlds)
					memcpy(DataPos,Worlds,4);
					//*(ITEMLIST*)DataPos=WorldListScroller;
					DataPos+=4;
					break;
				case EFFECT_ANIMSEL: 
					DataPos+=4;
					break;
				case EFFECT_CAMSEL: 
					DataPos+=4;
					break;
				case EFFECT_BLENDSEL: 
					*(GLuint*)DataPos=GL_ONE;
					DataPos+=8;
					break;
			}
		}
	}
}

void AdjustGUIToEvent(EVENT *Event)
{
	if (!Event) return;
	EventParameterInterface->SetActiveBranch(Event->Effect->Type);
	EventParameterInterface->ParentItem->SelectedItem=Event->Effect->Type;
	if (Event->Effect->TemplateDescriptor->ItemNum)
	{
		bool StaticTab=false,VariableTab=false;
		int x;
		for (x=0; x<Event->Effect->TemplateDescriptor->ItemNum; x++) if (Event->Effect->TemplateDescriptor->ItemList[x].Type<=EFFECT_VARFUNCTION) VariableTab=true; else StaticTab=true;
		cINTERFACE *WorkInterface=EventParameterInterface->ItemList[Event->Effect->Type].Interface;
		int *ConstCounter=new int;
		int *VarCounter=new int;
		*ConstCounter=0;
		*VarCounter=0;

		cINTERFACE *ConstInterface,*VarInterface;


		if (!StaticTab || !VariableTab) 
		{
			VarCounter=ConstCounter;
			ConstInterface=VarInterface=WorkInterface->ItemList[0].Interface;
		}
		else
		{
			VarInterface=WorkInterface->ItemList[0].Interface;
			ConstInterface=WorkInterface->ItemList[1].Interface;
		}

		unsigned char *DataPos=(unsigned char*)Event->EffectData;

		for (x=0; x<Event->Effect->TemplateDescriptor->ItemNum; x++)
		{
			switch (Event->Effect->TemplateDescriptor->ItemList[x].Type)
			{
			case EFFECT_BYTE: 
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[0].data=DataPos;
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[1].data=DataPos;
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[2].data=DataPos;
				(*VarCounter)++;
				DataPos+=1;
				break;
			case EFFECT_RTXT: 
				ConstInterface->ItemList[(*ConstCounter)].Interface->ItemList[0].data=DataPos;
				ConstInterface->ItemList[(*ConstCounter)].Interface->ItemList[1].data=DataPos;
				ConstInterface->ItemList[(*ConstCounter)].Interface->ItemList[2].data=DataPos;
				(*ConstCounter)++;
				DataPos+=1;
				break;
			case EFFECT_INT: 
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[0].data=DataPos;
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[1].data=DataPos;
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[2].data=DataPos;
				(*VarCounter)++;
				DataPos+=4;
				break;
			case EFFECT_FLOAT: 
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[0].data=DataPos;
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[1].data=DataPos;
				VarInterface->ItemList[(*VarCounter)].Interface->ItemList[2].data=DataPos;
				(*VarCounter)++;
				DataPos+=4;
				break;
			case EFFECT_BOOL:
				ConstInterface->ItemList[(*ConstCounter)++].data=DataPos;
				DataPos+=1;
				break;
			case EFFECT_VARFUNCTION:
				(*VarCounter)++;
				break;
			case EFFECT_CNSTFUNCTION:
				(*ConstCounter)++;
				break;
			case EFFECT_VARTEXT:
				(*VarCounter)++;
				break;
			case EFFECT_CONSTTEXT:
				(*ConstCounter)++;
				break;
			case EFFECT_BLENDSEL:
				ConstInterface->ItemList[(*ConstCounter)++].data=DataPos;
				DataPos+=4;
				ConstInterface->ItemList[(*ConstCounter)++].data=DataPos;
				DataPos+=4;
				break;
			case EFFECT_SCENESEL:
				ConstInterface->ItemList[*ConstCounter].data=(void*)WorldListScroller;
				if (Worlds && !((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World) ((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World=Worlds;
				if (((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World)
				{
					ConstInterface->ItemList[*ConstCounter].SelectedItem=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->ID;
				}
				else ConstInterface->ItemList[*ConstCounter].SelectedItem=-1;
				(*ConstCounter)+=5;
				DataPos+=4;
				break;
			case EFFECT_CAMSEL:
				if (((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World)
				{
					ConstInterface->ItemList[*ConstCounter].data=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->CameraList;
					ConstInterface->ItemList[*ConstCounter].SelectedItem=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->CamID;
				}
				(*ConstCounter)+=5;
				DataPos+=4;
				break;
			case EFFECT_ANIMSEL:
				if (((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World)
				{
					ConstInterface->ItemList[*ConstCounter].data=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->World->AnimList;
					ConstInterface->ItemList[*ConstCounter].SelectedItem=((DISPLAY3DTEMPLATE*)(SelectedEvent->EffectData))->AnimID;
				}
				(*ConstCounter)+=5;
				DataPos+=4;
				break;
			case EFFECT_MATSEL:
				if ((MATERIAL*)(*DataPos))
				{
					int SelectedItem=0;
					if ((ITEMLIST*)ConstInterface->ItemList[*ConstCounter].data)
					for (ITEM *I=((ITEMLIST*)ConstInterface->ItemList[*ConstCounter].data)->List; I; I=I->Next)
					{
						if (I->Data==(MATERIAL*)*(int*)DataPos)
						{
							ConstInterface->ItemList[*ConstCounter].SelectedItem=SelectedItem;
							//Scroller_SelectItem(ConstInterface,*ConstCounter);
						}
						SelectedItem++;
					}
				}
				else
				{
					ConstInterface->ItemList[*ConstCounter].SelectedItem=-1;
				}
				(*ConstCounter)+=5;
				DataPos+=4;
				break;
			}
		}
	}
}

void __stdcall Scroller_SelectEffect(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	Scroller_SelectItem(ScrollerInterface,ScrollerID);
	ITEM *a=ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem);
	if (SelectedEvent)
	{
		SelectedEvent->Effect=(EFFECT*)a->Data;
		SelectedEvent->EventType=((EFFECT*)a->Data)->Type;
		if (((EFFECT*)a->Data)->DataSize)
		{
			SelectedEvent->EffectData=new char[((EFFECT*)a->Data)->DataSize];
			memset(SelectedEvent->EffectData,0,((EFFECT*)a->Data)->DataSize);
		}
		else SelectedEvent->EffectData=NULL;
		SetDefaultSettings(SelectedEvent,(EFFECT*)a->Data);
		memset(SelectedEvent->Name,0,30);
		sprintf(SelectedEvent->Name,"#%d: %s\0",SelectedEvent->Num,a->Name);
		AdjustGUIToEvent(SelectedEvent);
		if (SelectedEvent->EventType==13) LastTime=timeGetTime();
	}
}

void RegisterEffect(int Type, char *Name,EFFECTCALLBACK EffectProc, EFFECTTEMPLATEDESCRIPTOR *TemplateDescriptor, unsigned char r, unsigned char g, unsigned char b)
{
	EFFECT *NewEffect=new EFFECT;
	NewEffect->EffectProc=EffectProc;
	NewEffect->r=r/255.0f;
	NewEffect->g=g/255.0f;
	NewEffect->b=b/255.0f;
	NewEffect->DataSize=0;
	NewEffect->TemplateDescriptor=TemplateDescriptor;
	NewEffect->Type=Type;
	EffectList->AddItem(Name,NewEffect,Scroller_SelectEffect,nop,nop);
	NewEffect->ListItem=EffectList->Last;
	EventParameterInterface->ParentItem->ItemNum++;
	GenerateEffectGUIFromTemplate(NewEffect,TemplateDescriptor);
}


void __stdcall Effect_CopyColors(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	EVENT *Event=SelectedEvent;
	((LAYER2DTEMPLATE*)Event->EffectData)->EndCol=((LAYER2DTEMPLATE*)Event->EffectData)->StartCol;
}

void __stdcall NoGridTex(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	EVENT *Event=SelectedEvent;
	((GRIDFILLERTEMPLATE*)Event->EffectData)->Texture=NULL;
}

void __stdcall Effect_FeedbackCopyColors(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	EVENT *Event=SelectedEvent;
	((FEEDBACKTEMPLATE*)Event->EffectData)->EndCol=((FEEDBACKTEMPLATE*)Event->EffectData)->StartCol;
}

void __stdcall CaveReset(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	time=0;
	LastTime=timeGetTime();
}

extern char TextBuffer[20000];
bool FirstRun;

void __stdcall TextRefresh(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	memset(TextBuffer,0,20000);
	FILE * f = fopen("text.txt","rb");
	if (f)
	{
		fread(TextBuffer,20000,1,f);
		fclose(f);
	}
	else
		if (!FirstRun) ErrorMessage="FILE TEXT.TXT NOT FOUND!";
}

void RegisterEffects()
{
	FirstRun=true;
	TextRefresh(NULL,0);
	FirstRun=false;

	int EffectTypeCounter=0;
	RegisterEffect(EffectTypeCounter++,"END DEMO",NoEffect,new EFFECTTEMPLATEDESCRIPTOR(),50,50,50);

	EFFECTTEMPLATEDESCRIPTOR *Layer2dTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	Layer2dTemplate->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	Layer2dTemplate->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	Layer2dTemplate->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	Layer2dTemplate->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	Layer2dTemplate->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_CopyColors);
	Layer2dTemplate->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	Layer2dTemplate->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	Layer2dTemplate->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	Layer2dTemplate->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	Layer2dTemplate->AddBlendModeSelector();
	Layer2dTemplate->AddBooleanButton(45,11,false,"TEXTURED","TURN TEXTURE ON/OFF");
	Layer2dTemplate->AddMaterialSelector();
	RegisterEffect(EffectTypeCounter++,"2D LAYER",Layer2d,Layer2dTemplate,0,0,255);

	EFFECTTEMPLATEDESCRIPTOR *ParticleTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	ParticleTemplate->AddConstantText(871,"SELECT WORLD:");
	ParticleTemplate->AddSceneSelector();
	ParticleTemplate->AddConstantText(871,"SELECT CAMERA:");
	ParticleTemplate->AddCameraSelector();
	ParticleTemplate->AddConstantText(871,"SELECT ANIMATION:");
	ParticleTemplate->AddAnimSelector();
	ParticleTemplate->AddFloatBar(0,1,0,GUI_NONE,"ANIM START: ","ANIMATION START");
	ParticleTemplate->AddFloatBar(0,1,1,GUI_NONE,"ANIM END: ","ANIMATION END");
	RegisterEffect(EffectTypeCounter++,"PARTICLE CALC",CalcParticle,ParticleTemplate,255,255,0);

	EFFECTTEMPLATEDESCRIPTOR *ClearTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	ClearTemplate->AddVariableText(871,"CLEAR COLOR:");
	ClearTemplate->AddByteBar(0,255,0,GUI_RED,"RED: ","CLEAR COLOR RED");
	ClearTemplate->AddByteBar(0,255,0,GUI_GREEN,"GREEN: ","CLEAR COLOR GREEN");
	ClearTemplate->AddByteBar(0,255,0,GUI_BLUE,"BLUE: ","CLEAR COLOR BLUE");
	ClearTemplate->AddByteBar(0,255,0,GUI_NONE,"ALPHA: ","CLEAR COLOR ALPHA");
	ClearTemplate->AddBooleanButton(59,11,true,"IMAGE","CLEAR IMAGE");
	ClearTemplate->AddBooleanButton(51,11,false,"ZBUFFER","CLEAR ZBUFFER");
	RegisterEffect(EffectTypeCounter++,"CLEAR",Clear,ClearTemplate,255,255,255);

	EFFECTTEMPLATEDESCRIPTOR *RtxtTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	RtxtTemplate->AddRenderTextureSelector();
	RegisterEffect(EffectTypeCounter++,"RENDER TO TEXTURE",R2Txt,RtxtTemplate,255,0,255);

	EFFECTTEMPLATEDESCRIPTOR *FeedbackTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	FeedbackTemplate->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	FeedbackTemplate->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	FeedbackTemplate->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	FeedbackTemplate->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	FeedbackTemplate->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_FeedbackCopyColors);
	FeedbackTemplate->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	FeedbackTemplate->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	FeedbackTemplate->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	FeedbackTemplate->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	FeedbackTemplate->AddBlendModeSelector();
	FeedbackTemplate->AddRenderTextureSelector();
	FeedbackTemplate->AddByteBar(0,255,5,GUI_NONE,"LAYERS: ","FEEDBACK LAYER NUM");
//	FeedbackTemplate->AddByteBar(0,255,128,GUI_NONE,"DENSITY: ","LAYER DENSITY");
	FeedbackTemplate->AddByteBar(0,255,230,GUI_NONE,"ZOOM: ","LAYER ZOOM");
	RegisterEffect(EffectTypeCounter++,"RECURSIVE-FEEDBACK",Feedback,FeedbackTemplate,0,255,255);

	EFFECTTEMPLATEDESCRIPTOR *BlurTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	BlurTemplate->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	BlurTemplate->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	BlurTemplate->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	BlurTemplate->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	BlurTemplate->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_FeedbackCopyColors);
	BlurTemplate->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	BlurTemplate->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	BlurTemplate->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	BlurTemplate->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	BlurTemplate->AddBlendModeSelector();
	BlurTemplate->AddRenderTextureSelector();
	BlurTemplate->AddByteBar(0,255,5,GUI_NONE,"LAYERS: ","Blur LAYER NUM");
//	BlurTemplate->AddByteBar(0,255,128,GUI_NONE,"DENSITY: ","LAYER DENSITY");
	BlurTemplate->AddByteBar(0,255,230,GUI_NONE,"ZOOM: ","LAYER ZOOM");
	RegisterEffect(EffectTypeCounter++,"BLUR",Blur,BlurTemplate,255,0,0);

	EFFECTTEMPLATEDESCRIPTOR *Scene3dTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	Scene3dTemplate->AddConstantText(871,"SELECT WORLD:");
	Scene3dTemplate->AddSceneSelector();
	Scene3dTemplate->AddConstantText(871,"SELECT CAMERA:");
	Scene3dTemplate->AddCameraSelector();
	Scene3dTemplate->AddConstantText(871,"SELECT ANIMATION:");
	Scene3dTemplate->AddAnimSelector();
	Scene3dTemplate->AddFloatBar(0,1,0,GUI_NONE,"CAM START: ","CAMERA START");
	Scene3dTemplate->AddFloatBar(0,1,1,GUI_NONE,"CAM END: ","CAMERA END");
	Scene3dTemplate->AddFloatBar(0,1,0,GUI_NONE,"ANIM START: ","ANIMATION START");
	Scene3dTemplate->AddFloatBar(0,1,1,GUI_NONE,"ANIM END: ","ANIMATION END");
	Scene3dTemplate->AddBooleanButton(30,11,false,"DON'T SAVE CAM","DON'T SAVE CAMERA WHEN PROCESSING EVENT");
	RegisterEffect(EffectTypeCounter++,"3D SCENE",Display3d,Scene3dTemplate,0,255,0);

	EFFECTTEMPLATEDESCRIPTOR *ParticleResetTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	ParticleResetTemplate->AddConstantText(871,"SELECT WORLD:");
	ParticleResetTemplate->AddSceneSelector();
	RegisterEffect(EffectTypeCounter++,"PARTICLE RESET",ParticleReset,ParticleResetTemplate,128,128,128);

	EFFECTTEMPLATEDESCRIPTOR *ContrastTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	ContrastTemplate->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	ContrastTemplate->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	ContrastTemplate->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	ContrastTemplate->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	ContrastTemplate->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_FeedbackCopyColors);
	ContrastTemplate->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	ContrastTemplate->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	ContrastTemplate->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	ContrastTemplate->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	ContrastTemplate->AddBlendModeSelector();
	ContrastTemplate->AddRenderTextureSelector();
	ContrastTemplate->AddByteBar(0,255,1,GUI_NONE,"MUL AMOUNT: ","CONTRAST MUL AMOUNT");
	ContrastTemplate->AddByteBar(0,255,1,GUI_NONE,"ADD AMOUNT: ","CONTRAST ADD AMOUNT");
	RegisterEffect(EffectTypeCounter++,"CONTRAST",Contrast,ContrastTemplate,255,0,127);

	EFFECTTEMPLATEDESCRIPTOR *EndScrollTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	EndScrollTemplate->AddConstantFunction(48,11,"REFRESH","REFRESH TEXT FROM text.txt",TextRefresh);
	RegisterEffect(EffectTypeCounter++,"TEXTWRITER",EndScroll,EndScrollTemplate,127,0,255);

	EFFECTTEMPLATEDESCRIPTOR *CrystalizeTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	CrystalizeTemplate->AddIntBar(0,65535,1000,GUI_NONE,"CRYSTALS: ","NUMBER OF CRYSTALS");
	CrystalizeTemplate->AddByteBar(0,255,0,GUI_NONE,"RAND: ","RAND SEED");
	CrystalizeTemplate->AddByteBar(3,12,7,GUI_NONE,"DETAIL: ","CRYSTAL DETAIL");
	CrystalizeTemplate->AddFloatBar(0,0.5f,0.12f,GUI_NONE,"STARTSIZE: ","STARTING CELL SIZE");
	CrystalizeTemplate->AddFloatBar(0,0.5f,0.12f,GUI_NONE,"ENDSIZE: ","ENDING CELL SIZE");
	CrystalizeTemplate->AddFloatBar(0,1,0.1f,GUI_NONE,"CHAOS: ","ANIMATION CHAOS");
	CrystalizeTemplate->AddByteBar(0,255,0,GUI_NONE,"START: ","ANIMATION START");
	CrystalizeTemplate->AddByteBar(0,255,255,GUI_NONE,"END: ","ANIMATION END");
	CrystalizeTemplate->AddByteBar(0,2,0,GUI_NONE,"ANIMTYPE: ","ALTERNATE ANIMATION SELECTOR");
	CrystalizeTemplate->AddBlendModeSelector();

	CrystalizeTemplate->AddRenderTextureSelector();

//#ifndef Public_Release
	RegisterEffect(EffectTypeCounter++,"CRYSTALIZE",Crystalize,CrystalizeTemplate,0,127,255);
//#else
//	RegisterEffect(EffectTypeCounter++,"CRYSTLZE (n/a in public)",Crystalize,CrystalizeTemplate,0,127,255);
//#endif


	EFFECTTEMPLATEDESCRIPTOR *GridfillerTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	
	GridfillerTemplate->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	GridfillerTemplate->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	GridfillerTemplate->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	GridfillerTemplate->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	GridfillerTemplate->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_CopyColors);
	GridfillerTemplate->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	GridfillerTemplate->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	GridfillerTemplate->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	GridfillerTemplate->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	GridfillerTemplate->AddBlendModeSelector();

	GridfillerTemplate->AddFloatBar(0,100,1,GUI_NONE,"SPEED:","REPEAT TIME");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"XSIN 1: ","WAVE VARIABLE 1");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"XSIN 2: ","WAVE VARIABLE 2");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"XSIN 3: ","WAVE VARIABLE 3");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"XSIN 4: ","WAVE VARIABLE 4");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"YSIN 1: ","WAVE VARIABLE 5");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"YSIN 2: ","WAVE VARIABLE 6");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"YSIN 3: ","WAVE VARIABLE 7");
	GridfillerTemplate->AddFloatBar(-15,15,0,GUI_NONE,"YSIN 4: ","WAVE VARIABLE 8");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"ASIN 1: ","WAVE VARIABLE 9");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"ASIN 2: ","WAVE VARIABLE 10");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"ASIN 3: ","WAVE VARIABLE 11");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"ASIN 4: ","WAVE VARIABLE 12");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"BSIN 1: ","WAVE VARIABLE 13");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"BSIN 2: ","WAVE VARIABLE 14");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"BSIN 3: ","WAVE VARIABLE 15");
	GridfillerTemplate->AddFloatBar(-15,15,1,GUI_NONE,"BSIN 4: ","WAVE VARIABLE 16");

	GridfillerTemplate->AddBooleanButton(51,11,false,"SHADING","SHADING");
	GridfillerTemplate->AddBooleanButton(51,11,false,"TEXTURE","TEXTURE");


	GridfillerTemplate->AddMaterialSelector();
	RegisterEffect(EffectTypeCounter++,"GRIDFILLER",Gridfiller,GridfillerTemplate,0,255,127);

	EFFECTTEMPLATEDESCRIPTOR *CaveTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	CaveTemplate->AddByteBar(0,255,0,GUI_NONE,"RANDSEED: ","RANDSEED");
	CaveTemplate->AddFloatBar(0,180,55,GUI_NONE,"FOV: ","FIELD OF VIEW");
	CaveTemplate->AddByteBar(0,255,255,GUI_RED,"FOG RED: ","FOG RED COLOR");
	CaveTemplate->AddByteBar(0,255,255,GUI_GREEN,"FOG GREEN: ","FOG GREEN COLOR");
	CaveTemplate->AddByteBar(0,255,255,GUI_BLUE,"FOG BLUE: ","FOG BLUE COLOR");

	CaveTemplate->AddByteBar(55,255,(int)(255*0.7f),GUI_NONE,"XVAR1: ","VARIABLE 1");
	CaveTemplate->AddByteBar(55,255,(int)(255*0.7f),GUI_NONE,"YVAR1: ","VARIABLE 2");
	CaveTemplate->AddByteBar(55,255,(int)(255*0.6f),GUI_NONE,"XAMP2: ","VARIABLE 3");
	CaveTemplate->AddByteBar(55,255,(int)(255*0.6f),GUI_NONE,"YAMP2: ","VARIABLE 4");
	CaveTemplate->AddByteBar(0,255,(int)(255*0.7f),GUI_NONE,"BXVAR3: ","VARIABLE 5");
	CaveTemplate->AddByteBar(0,255,(int)(255*0.7f),GUI_NONE,"BYVAR3: ","VARIABLE 6");
	CaveTemplate->AddByteBar(0,255,(int)(255*0.6f),GUI_NONE,"BXAMP4: ","VARIABLE 7");
	CaveTemplate->AddByteBar(0,255,(int)(255*0.6f),GUI_NONE,"BYAMP4: ","VARIABLE 8");
	CaveTemplate->AddByteBar(0,255,(int)(255*0.6f),GUI_NONE,"RAD: ","RADIUS VARIABLE");

	CaveTemplate->AddBooleanButton(58,11,true,"SOLID","SOLID");
	CaveTemplate->AddBooleanButton(44,11,false,"WIREFRAME","WIREFRAME");
	CaveTemplate->AddBooleanButton(48,11,true,"LIGHTING","LIGHTING");

	CaveTemplate->AddByteBar(1,255,50,GUI_NONE,"FPS: ","FPS");
	CaveTemplate->AddByteBar(0,255,1,GUI_NONE,"SKIP: ","SKIP");

	CaveTemplate->AddVariableFunction(58,11,"RESET","RESTART TUNNEL",CaveReset);

	CaveTemplate->AddBooleanButton(51,11,false,"TEXTURE","TEXTURE");
	CaveTemplate->AddMaterialSelector();

//#ifndef Public_Release
	RegisterEffect(EffectTypeCounter++,"CAVE",Cave,CaveTemplate,255,127,0);
//#else
//	RegisterEffect(EffectTypeCounter++,"CAVE (n/a in public)",Cave,CaveTemplate,255,127,0);
//#endif

	EFFECTTEMPLATEDESCRIPTOR *ShakeTemplate=new EFFECTTEMPLATEDESCRIPTOR();
	ShakeTemplate->AddFloatBar(0,50,5,GUI_NONE,"EYE AMP: ","EYE SHAKE AMPLITUDE");
	ShakeTemplate->AddFloatBar(0,50,5,GUI_NONE,"TRG AMP: ","TARGET SHAKE AMPLITUDE");
	ShakeTemplate->AddFloatBar(0,1,0,GUI_NONE,"START INT: ","START INTENSITY");
	ShakeTemplate->AddFloatBar(0,1,1,GUI_NONE,"END INT: ","END INTENSITY");
	ShakeTemplate->AddByteBar(1,255,1,GUI_NONE,"FREQ: ","NEW RANDOM VALUE EVERY FREQ FRAMES");
	RegisterEffect(EffectTypeCounter++,"CAMERA SHAKE",CamShake,ShakeTemplate,127,255,0);


	EFFECTTEMPLATEDESCRIPTOR *MrcTemplate=new EFFECTTEMPLATEDESCRIPTOR();

	MrcTemplate->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	MrcTemplate->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	MrcTemplate->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	MrcTemplate->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	MrcTemplate->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_CopyColors);
	MrcTemplate->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	MrcTemplate->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	MrcTemplate->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	MrcTemplate->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	MrcTemplate->AddBlendModeSelector();

	MrcTemplate->AddByteBar(0,6,0,GUI_NONE,"EFFECTTYPE: ","EFFECT SELECTOR");
	MrcTemplate->AddFloatBar(0,1,0,GUI_NONE,"START: ","START POSITION");
	MrcTemplate->AddFloatBar(0,1,1,GUI_NONE,"END: ","END POSITION");
	MrcTemplate->AddRenderTextureSelector();
	RegisterEffect(EffectTypeCounter++,"MRC EFFECT",MrcEffekt,MrcTemplate,127,127,255);

	EFFECTTEMPLATEDESCRIPTOR *Feedback2Template=new EFFECTTEMPLATEDESCRIPTOR();
	Feedback2Template->AddByteBar(0,255,255,GUI_RED,"START RED: ","START RED COLOR");
	Feedback2Template->AddByteBar(0,255,255,GUI_GREEN,"START GREEN: ","START GREEN COLOR");
	Feedback2Template->AddByteBar(0,255,255,GUI_BLUE,"START BLUE: ","START BLUE COLOR");
	Feedback2Template->AddByteBar(0,255,255,GUI_NONE,"START ALPHA: ","START ALPHA COLOR");
	Feedback2Template->AddVariableFunction(35,11,"COPY COLORS","COPY START COLORS TO END COLORS",Effect_FeedbackCopyColors);
	Feedback2Template->AddByteBar(0,255,255,GUI_RED,"END RED: ","END RED COLOR");
	Feedback2Template->AddByteBar(0,255,255,GUI_GREEN,"END GREEN: ","END GREEN COLOR");
	Feedback2Template->AddByteBar(0,255,255,GUI_BLUE,"END BLUE: ","END BLUE COLOR");
	Feedback2Template->AddByteBar(0,255,255,GUI_NONE,"END ALPHA: ","END ALPHA COLOR");
	Feedback2Template->AddBlendModeSelector();
	Feedback2Template->AddRenderTextureSelector();
	Feedback2Template->AddByteBar(0,255,5,GUI_NONE,"LAYERS: ","Feedback2 LAYER NUM");
//	Feedback2Template->AddByteBar(0,255,128,GUI_NONE,"DENSITY: ","LAYER DENSITY");
	Feedback2Template->AddByteBar(0,255,230,GUI_NONE,"DENSITY: ","LAYER DENSITY");
	RegisterEffect(EffectTypeCounter++,"FEEDBACK",Feedback2,Feedback2Template,0,255,255);

	EventParameterInterface->ParentItem->ActiveBranch=true;
	EventParameterInterface->SetActiveBranch(0);
}