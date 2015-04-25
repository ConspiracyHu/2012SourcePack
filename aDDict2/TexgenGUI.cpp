#include "TexGenGui.h"

ITEMLIST *TextureFileList=NULL;
ITEMLIST *JpegList=NULL;
cINTERFACE *CommandMenu;
void *CommandMenuData;
TEXTURE *ActualTexture=NULL;
cINTERFACE *ActualTextureCommandInterface;
cINTERFACE *DisplayArea;
cINTERFACE *StatusBar;
cINTERFACE *EditMenu;
int SelectedFilter=0;
unsigned char UndoStatus;
cINTERFACE *RandSeedText;
int rndseedhack=0;

GLuint LeftDisplays[8],RightDisplays[8];

void __stdcall Scroller_SelectMaterial(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	Scroller_SelectItem(ScrollerInterface,ScrollerID);
	ITEM *a=ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem);
	ActualMaterial=(MATERIAL*)a->Data;
	MaterialHandle=ActualMaterial->ImageData.TextureHandle;
	if (MainGUI.ItemList[0].ActiveBranch)
	if (SelectedEvent)
	{
		int DataPos=(int)SelectedEvent->EffectData;
		for (int x=0; x<SelectedEvent->Effect->TemplateDescriptor->ItemNum; x++)
		{
			switch (SelectedEvent->Effect->TemplateDescriptor->ItemList[x].Type)
			{
			case EFFECT_BYTE: 
			case EFFECT_RTXT: 
			case EFFECT_BOOL: 
				DataPos+=1; break;
			case EFFECT_INT: 
			case EFFECT_FLOAT: 
			case EFFECT_SCENESEL: 
			case EFFECT_ANIMSEL: 
			case EFFECT_CAMSEL: 
				DataPos+=4; break;
			case EFFECT_MATSEL:
				*(MATERIAL**)DataPos=ActualMaterial;
				DataPos+=4; 
				break;
			case EFFECT_BLENDSEL: 
				DataPos+=8; break;
			default: ;
			}
		}
	}
}

void BuildMaterialList()
{
	MaterialList->Last=NULL;
	MaterialList->List=NULL;
	MaterialList->ItemNum=0;
	VarMaterialsMenu->ItemList[1].ItemNum=0;
	VarMaterialsMenu->ItemList[2].data=&VarMaterialsMenu->ItemList[1].StartItem;
	VarMaterialsMenu->ItemList[2].data2=&VarMaterialsMenu->ItemList[1].ItemNum;
	VarMaterialsMenu->ItemList[3].data=&VarMaterialsMenu->ItemList[1].StartItem;
	VarMaterialsMenu->ItemList[3].data2=&VarMaterialsMenu->ItemList[1].ItemNum;
	VarMaterialsMenu->ItemList[4].data=&VarMaterialsMenu->ItemList[1].StartItem;
	VarMaterialsMenu->ItemList[4].data2=&VarMaterialsMenu->ItemList[1].ItemNum;

	MapXformMenu->ItemList[1].ItemNum=0;

	VarMatMenu->ItemList[1].ItemNum=0;
	VarMatMenu->ItemList[2].data=&VarMatMenu->ItemList[1].StartItem;
	VarMatMenu->ItemList[2].data2=&VarMatMenu->ItemList[1].ItemNum;
	VarMatMenu->ItemList[3].data=&VarMatMenu->ItemList[1].StartItem;
	VarMatMenu->ItemList[3].data2=&VarMatMenu->ItemList[1].ItemNum;
	VarMatMenu->ItemList[4].data=&VarMatMenu->ItemList[1].StartItem;
	VarMatMenu->ItemList[4].data2=&VarMatMenu->ItemList[1].ItemNum;

	for (ITEM *i=TextureListScroller->List; i; i=i->Next)
	{
		TEXTURE *t=(TEXTURE*)i->Data;
		for (int x=0; x<8; x++)
		if (t->Materials[x].Used)
		{
			char *Name=new char[256];
			memset(Name,0,256);
			sprintf(Name,"%s (%d)\0",t->Materials[x].Name,t->Materials[x].SlotNumber);
			MaterialList->AddItem(Name,&t->Materials[x],Scroller_SelectMaterial,SubOpenClose,nop);

			VarMaterialsMenu->ItemList[1].ItemNum++;
			VarMatMenu->ItemList[1].ItemNum++;
			MapXformMenu->ItemList[1].ItemNum++;

		}
	}
}

void PerformCommand(TEXTURE *Texture,COMMAND *Command, RGBA *Buffer)
{
	memcpy(Buffer,Texture->Layers[Command->Layer].Data,4*Texture->XRes*Texture->YRes);
	switch (Command->Filter) {
		case TEXGEN_Envmap:				Texture->Envmap(Buffer,Command);break;
		case TEXGEN_Plasma:				Texture->Plasma(Buffer,Command);break;
		case TEXGEN_Map:					Texture->Map(Buffer,Command);break;
		case TEXGEN_Blur:					Texture->Blur(Buffer,Command);break;
		case TEXGEN_DirBlur:			Texture->DirBlur(Buffer,Command);break;
		case TEXGEN_Text:					Texture->Text(Buffer,Command);break;
		case TEXGEN_SubPlasma:		Texture->SubPlasma(Buffer,Command);break;
		case TEXGEN_FractalPlasma:Texture->FractalPlasma(Buffer,Command);break;
		case TEXGEN_Colorize:			Texture->Colorize(Buffer,Command);break;
		case TEXGEN_Shade:				Texture->Shade(Buffer,Command);break;
		case TEXGEN_Brightness:		Texture->Brightness(Buffer,Command);break;
		case TEXGEN_Copy:					Texture->Copy(Buffer,Command);break;
		case TEXGEN_Cells:				Texture->Cells(Buffer,Command);break;
		case TEXGEN_Twirl:				Texture->Twirl(Buffer,Command);break;
		case TEXGEN_SineDist:			Texture->SineDist(Buffer,Command);break;
		case TEXGEN_Mix:					Texture->Mix(Buffer,Command);break;
		case TEXGEN_MixMap:				Texture->MixMap(Buffer,Command);break;
		case TEXGEN_Scale:				Texture->Scale(Buffer,Command);break;
		case TEXGEN_SineColor:		Texture->SineColor(Buffer,Command);break;
		case TEXGEN_Max:					Texture->Max(Buffer,Command);break;
		case TEXGEN_HSV:					Texture->HSV(Buffer,Command);break;
		case TEXGEN_Emboss:				Texture->Emboss(Buffer,Command);break;
		case TEXGEN_Invert:				Texture->Invert(Buffer,Command);break;
		case TEXGEN_Glass:					Texture->Glass(Buffer,Command);break;
		case TEXGEN_Pixelize:				Texture->Pixelize(Buffer,Command);break;
		case TEXGEN_Offset:				Texture->Offset(Buffer,Command);break;
		case TEXGEN_Crystalize:				Texture->Crystalize(Buffer,Command);break;
		case TEXGEN_Rectangle:				Texture->Rectangle(Buffer,Command);break;
		case TEXGEN_Circle:				Texture->Circle(Buffer,Command);break;
		case TEXGEN_Contrast:				Texture->Contrast(Buffer,Command);break;
		case TEXGEN_MakeMaterial:				Texture->MakeMaterial(Buffer,Command);break;
		case TEXGEN_Gradient:				Texture->Gradient(Buffer,Command);break;
		case TEXGEN_Rotozoom:				Texture->Rotozoom(Buffer,Command);break;
		case TEXGEN_ChamferRectangle:		Texture->ChamferRectangle(Buffer,Command);break;
		case TEXGEN_Dots:		Texture->Dots(Buffer,Command);break;
		case TEXGEN_Jpeg:		Texture->Jpeg(Buffer,Command);break;
		case TEXGEN_FractalPlasmaOld:Texture->FractalPlasmaOld(Buffer,Command);break;
	}
	Texture->PerformFilter(Texture->Layers[Command->Layer].Data,Buffer,Command);
	Texture->UpdateTexture(Texture->Layers[Command->Layer].Data,LeftDisplays[Command->Layer]/*Texture->Layers[Command->Layer].TextureHandle*/);
}

//////////////////////////////////////////////////////////////////////////
// l33t h4x0r m4cr0 

#define DECLARE_FILTER(x) cINTERFACE * ##x##Menu;\
cINTERFACE * x##MenuEdit;\
FILTERTEMPLATE x##Template;\
void __stdcall Change##x##(cINTERFACE* Interface, int ButtonId)\
{\
	if (TextureList) \
	{\
		StatusBar->ParentItem->ActiveBranch=true;\
		ActualTexture->Commands[EditMenu->ClickedNum].Layer=EditMenu->ItemList[SelectedFilter].Interface->TextureCommand.Layer;\
		ActualTexture->Commands[EditMenu->ClickedNum].Operator=EditMenu->ItemList[SelectedFilter].Interface->TextureCommand.Operator;\
		int x;\
		for (x=0; x<4; x++)ActualTexture->Commands[EditMenu->ClickedNum].OperatorMask[x]=EditMenu->ItemList[SelectedFilter].Interface->TextureCommand.OperatorMask[x];\
		memcpy(ActualTexture->Commands[EditMenu->ClickedNum].data,EditMenu->ItemList[SelectedFilter].Interface->TextureCommand.data,EditMenu->ItemList[SelectedFilter].Interface->TextureCommand.DataSize);\
		RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->YRes];\
		for (x=0; x<8; x++)\
		{\
			ActualTexture->Materials[x].Used=false;\
			memset(ActualTexture->Layers[x].Data,0,ActualTexture->XRes*ActualTexture->XRes*4);\
			for (int a=0; a<ActualTexture->XRes*ActualTexture->XRes; a++)	ActualTexture->Layers[x].Data[a].a=255;\
			ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);\
		}\
		for (x=0; x<ActualTexture->CommandNumber; x++)\
		{\
			UndoStatus=x+1;\
			PerformCommand(ActualTexture,&ActualTexture->Commands[x],Buffer);\
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))\
			{\
				TranslateMessage(&msg);\
				DispatchMessage(&msg);\
			}\
			else\
			{\
				glClear(0x4100);\
				glColor4f(highblue);\
				rectangle(-1,35,1025,35);\
				glColor4f(1,1,1,1);\
				MainGUI.DrawGUI();\
				\
				SwapBuffers(hDC);\
			}\
		}\
		for (x=0; x<8; x++)\
		ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);\
		StatusBar->ParentItem->ActiveBranch=false;\
	}\
	else ErrorMessage="CREATE A TEXTURE FIRST!";\
	BuildMaterialList();\
	WaitLeftButton=true;\
}\
void __stdcall Do##x##(cINTERFACE* Interface, int ButtonId)\
{\
	if (TextureList) \
	{\
	RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->YRes];\
		memcpy(Buffer,ActualTexture->Layers[ATCI->TextureCommand.Layer].Data,4*ActualTexture->XRes*ActualTexture->YRes);\
		ActualTexture->##x##(Buffer,&ATCI->TextureCommand);\
		ActualTexture->PerformFilter(ActualTexture->Layers[ATCI->TextureCommand.Layer].Data,Buffer,&ATCI->TextureCommand);\
		ActualTexture->UpdateTexture(ActualTexture->Layers[ATCI->TextureCommand.Layer].Data,LeftDisplays[ATCI->TextureCommand.Layer]);\
		COMMAND *CmdBuffer=new COMMAND;\
		*CmdBuffer=ATCI->TextureCommand;\
		CmdBuffer->data=new char[ATCI->TextureCommand.DataSize];\
		memcpy(CmdBuffer->data,ATCI->TextureCommand.data,ATCI->TextureCommand.DataSize);\
		ActualTexture->AddCommand(CmdBuffer);\
		CommandMenu->ParentItem->ItemNum=ActualTexture->CommandList->ItemNum;\
		delete[] Buffer;\
	}\
	else ErrorMessage="CREATE A TEXTURE FIRST!";\
	BuildMaterialList();\
	WaitLeftButton=true;\
}\
\
void __stdcall Preview##x##(cINTERFACE* Interface, int ButtonId)\
{\
	if (TextureList) \
	{\
		RGBA *Buffer =new RGBA[ActualTexture->XRes*ActualTexture->YRes];\
		RGBA *Buffer2=new RGBA[ActualTexture->XRes*ActualTexture->YRes];\
		memcpy(Buffer ,ActualTexture->Layers[ATCI->TextureCommand.Layer].Data,4*ActualTexture->XRes*ActualTexture->YRes);\
		memcpy(Buffer2,ActualTexture->Layers[ATCI->TextureCommand.Layer].Data,4*ActualTexture->XRes*ActualTexture->YRes);\
		ActualTexture->##x##(Buffer2,&ATCI->TextureCommand);\
		ActualTexture->PerformFilter(Buffer,Buffer2,&ATCI->TextureCommand);\
		ActualTexture->UpdateTexture(Buffer,ATCI->Preview);\
		delete[] Buffer2;\
		delete[] Buffer;\
	}\
	else ErrorMessage="CREATE A TEXTURE FIRST!";\
	WaitLeftButton=true;\
}\
\


#define GENGUI(x)	{\
	GenerateGUIFromTemplate(##x##Menu,&##x##Template,Do##x##,Preview##x##,TEXGEN_##x##,150,false);\
	##x##MenuEdit=EditMenu->AddSubInterface();\
	GenerateGUIFromTemplate(##x##MenuEdit,&##x##Template,Change##x##,Change##x##,TEXGEN_##x##,250,true);\
}

//////////////////////////////////////////////////////////////////////////

DECLARE_FILTER(Jpeg);
DECLARE_FILTER(Envmap);
DECLARE_FILTER(Plasma);
DECLARE_FILTER(Text);
DECLARE_FILTER(Map);
DECLARE_FILTER(Blur);
DECLARE_FILTER(DirBlur);
DECLARE_FILTER(SubPlasma);
DECLARE_FILTER(FractalPlasma);
DECLARE_FILTER(Colorize);
DECLARE_FILTER(Emboss);
DECLARE_FILTER(Shade);
DECLARE_FILTER(Brightness);
DECLARE_FILTER(Copy);
DECLARE_FILTER(Cells);
DECLARE_FILTER(Twirl);
DECLARE_FILTER(SineDist);
DECLARE_FILTER(Mix);
DECLARE_FILTER(MixMap);
DECLARE_FILTER(Invert);
DECLARE_FILTER(Scale);
DECLARE_FILTER(SineColor);
DECLARE_FILTER(Max);
DECLARE_FILTER(HSV);
DECLARE_FILTER(Glass);
DECLARE_FILTER(Pixelize);
DECLARE_FILTER(Offset);
DECLARE_FILTER(Crystalize);
DECLARE_FILTER(Rectangle);
DECLARE_FILTER(Circle);
DECLARE_FILTER(Contrast);
DECLARE_FILTER(MakeMaterial);
DECLARE_FILTER(Gradient);
DECLARE_FILTER(Rotozoom);
DECLARE_FILTER(ChamferRectangle);
DECLARE_FILTER(Dots);
DECLARE_FILTER(FractalPlasmaOld);

void __stdcall TexGen_Clear(cINTERFACE* Interface, int ButtonId)
{
	if (ActualTexture)
	{
		for (int x=0; x<8; x++)
		{
			ActualTexture->Materials[x].Used=false;
			//if (ActualTexture->Materials[x].Name) delete ActualTexture->Materials[x].Name;
			memset(ActualTexture->Layers[x].Data,0,ActualTexture->XRes*ActualTexture->XRes*4);
			for (int a=0; a<ActualTexture->XRes*ActualTexture->XRes; a++)	ActualTexture->Layers[x].Data[a].a=255;
			ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);
		}
		ActualTexture->CommandNumber=0;
		ActualTexture->CommandList=new ITEMLIST();
		*(int*)CommandMenuData=(int)ActualTexture->CommandList;
		CommandMenu->ParentItem->ItemNum=ActualTexture->CommandList->ItemNum;
	}
	else ErrorMessage="CREATE A TEXTURE FIRST!";
	BuildMaterialList();
	WaitLeftButton=true;
}

void __stdcall UndoTextureEffect(cINTERFACE* Interface, int ButtonId)
{
	if (TextureList) 
	{
		StatusBar->ParentItem->ActiveBranch=true;
		if (ActualTexture->CommandNumber)
		{
			RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->YRes];
			ActualTexture->CommandNumber--;
			if (ActualTexture->CommandList->Last->Prev)	ActualTexture->CommandList->Last=ActualTexture->CommandList->Last->Prev;
			else ActualTexture->CommandList->Last=NULL;
			if (ActualTexture->CommandList->Last) ActualTexture->CommandList->Last->Next=NULL;
			else ActualTexture->CommandList->List=NULL;
			ActualTexture->CommandList->ItemNum--;
			StatusBar->ItemList[0].iMax=ActualTexture->CommandNumber;
			CommandMenu->ParentItem->ItemNum=ActualTexture->CommandList->ItemNum;
			int x;
			for (x=0; x<8; x++)
			{
				ActualTexture->Materials[x].Used=false;
				//glDeleteTextures(1,&ActualTexture->Materials[x].ImageData.TextureHandle);
				RGBA Black;
				Black.dw=0;
				glBindTexture(GL_TEXTURE_2D,ActualTexture->Materials[x].ImageData.TextureHandle);
				glTexImage2D(GL_TEXTURE_2D,0,4,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,&Black);
				//if (ActualTexture->Materials[x].Name) delete ActualTexture->Materials[x].Name;
				memset(ActualTexture->Layers[x].Data,0,ActualTexture->XRes*ActualTexture->XRes*4);
				for (int a=0; a<ActualTexture->XRes*ActualTexture->XRes; a++)	ActualTexture->Layers[x].Data[a].a=255;
			}
			for (x=0; x<ActualTexture->CommandNumber; x++)
			{
				UndoStatus=x+1;
				PerformCommand(ActualTexture,&ActualTexture->Commands[x],Buffer);				
				if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg); 
				}
				else
				{
					glClear(0x4100);
					glColor4f(highblue);
					rectangle(-1,35,1025,35);
					glColor4f(1,1,1,1);

					MainGUI.DrawGUI();
					//glRasterPos2i(410,21);
					//glPrint("PERFORMING UNDO...",base);
					SwapBuffers(hDC);
				}
			}
			for (x=0; x<8; x++)
				ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);
		}
		else ErrorMessage="NOTHING TO UNDO!";
	}
	else ErrorMessage="CREATE A TEXTURE FIRST!";
	WaitLeftButton=true;
	BuildMaterialList();
	StatusBar->ParentItem->ActiveBranch=false;
}

void __stdcall ExportTexture(cINTERFACE* Interface, int ButtonId)
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
				if (TextureList)
				{

					char *NewLine=new char[strlen(CommandLine)+strlen(TextureDir)+10];
					memset(NewLine,0,strlen(CommandLine)+10);
					sprintf(NewLine,"%s\\%s.tex\0",TextureDir,CommandLine);

					FILE *f=fopen(NewLine,"w+b");

					if (f)
					{
						fwrite("aDDict2T",8,1,f);
						fwrite(&ActualTexture->XRes,4,1,f);
						fwrite(&ActualTexture->CommandNumber,4,1,f);

						for (int x=0; x<ActualTexture->CommandNumber; x++)
						{
							fwrite(&ActualTexture->Commands[x].Filter,1,1,f); // 4 bit
							fwrite(&ActualTexture->Commands[x].Layer,1,1,f); // 4 bit
							fwrite(&ActualTexture->Commands[x].Operator,1,1,f); //2 bit
							fwrite(&ActualTexture->Commands[x].OperatorMask,4,1,f); //4 bit
							switch (ActualTexture->Commands[x].Filter)
							{
							case TEXGEN_Envmap:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(ENVMAPTEMPLATE),f);
								break;					}
							case TEXGEN_Plasma:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(PLASMATEMPLATE),f);
								break;					}
							case TEXGEN_Map:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(MAPTEMPLATE),f);
								break;					}
							case TEXGEN_Blur:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(BLURTEMPLATE),f);
								break;					}
							case TEXGEN_DirBlur:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(DIRBLURTEMPLATE),f);
								break;					}
							case TEXGEN_Text:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(TEXTTEMPLATE)-4,f);
								fwrite(((TEXTTEMPLATE*)ActualTexture->Commands[x].data)->Text,1,strlen(((TEXTTEMPLATE*)ActualTexture->Commands[x].data)->Text),f);
								fprintf(f,"%c",0);
								break;					}
							case TEXGEN_SubPlasma:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(SUBPLASMATEMPLATE),f);
								break;					}
							case TEXGEN_FractalPlasma:			{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(FRACTALPLASMATEMPLATE),f);
								break;					}
							case TEXGEN_Colorize:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(COLORIZETEMPLATE),f);
								break;					}
							case TEXGEN_Shade:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(SHADETEMPLATE),f);
								break;					}
							case TEXGEN_Brightness:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(BRIGHTNESSTEMPLATE),f);
								break;					}
							case TEXGEN_Copy:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(COPYTEMPLATE),f);
								break;					}
							case TEXGEN_Cells:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(CELLSTEMPLATE),f);
								break;					}
							case TEXGEN_Twirl:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(TWIRLTEMPLATE),f);
								break;					}
							case TEXGEN_SineDist:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(SINEDISTTEMPLATE),f);
								break;					}
							case TEXGEN_Mix:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(MIXTEMPLATE),f);
								break;					}
							case TEXGEN_MixMap:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(MIXMAPTEMPLATE),f);
								break;					}
							case TEXGEN_Scale:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(SCALETEMPLATE),f);
								break;					}
							case TEXGEN_SineColor:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(SINECOLORTEMPLATE),f);
								break;					}
							case TEXGEN_Max:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(MAXTEMPLATE),f);
								break;					}
							case TEXGEN_HSV:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(HSVTEMPLATE),f);
								break;					}
							case TEXGEN_Glass:				{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(GLASSTEMPLATE),f);
								break;					}
							case TEXGEN_Pixelize:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(PIXELIZETEMPLATE),f);
								break;					}
							case TEXGEN_Offset:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(OFFSETTEMPLATE),f);
								break;					}
							case TEXGEN_Crystalize:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(CRYSTALIZETEMPLATE),f);
								break;					}
							case TEXGEN_Rectangle:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(RECTANGLETEMPLATE),f);
								break;					}
							case TEXGEN_Circle:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(CIRCLETEMPLATE),f);
								break;					}
							case TEXGEN_Contrast:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(CONTRASTTEMPLATE),f);
								break;					}
							case TEXGEN_Gradient:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(GRADIENTTEMPLATE),f);
								break;					}
							case TEXGEN_Rotozoom:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(ROTOZOOMTEMPLATE),f);
								break;					}
							case TEXGEN_ChamferRectangle:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(CHAMFERRECTANGLETEMPLATE),f);
								break;					}
							case TEXGEN_Dots:					{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(DOTSTEMPLATE),f);
								break;					}
							case TEXGEN_MakeMaterial:			{
								fwrite(ActualTexture->Commands[x].data,1,sizeof(MAKEMATERIALTEMPLATE)-4,f);
								fwrite(((MAKEMATERIALTEMPLATE*)ActualTexture->Commands[x].data)->Name,1,strlen(((MAKEMATERIALTEMPLATE*)ActualTexture->Commands[x].data)->Name),f);
								fprintf(f,"%c",0);
								break;					}
							case TEXGEN_Jpeg:					{
								JPEGTEMPLATE *a=(JPEGTEMPLATE*)ActualTexture->Commands[x].data;
								fwrite(&a->JPEGDataSize,1,4,f);
								fwrite(a->JPEGData,1,a->JPEGDataSize,f);
								break;                  }
							}
						}

						fclose(f);
						LastClickedToolTip="TEXTURE EXPORTED SUCCESFULLY.";
					}
					else ErrorMessage="ERROR WHILE EXPORTING TEXTURE!";
				}
				else ErrorMessage="CREATE A TEXTURE FIRST!";
			}
	}
}

void __stdcall GUI_ButtonSelect_TextureMenuEdit(cINTERFACE* ButtonInterface, int ButtonId)
{
	GUI_ButtonSelect(ButtonInterface,ButtonId);
	cINTERFACE *Interface=EditMenu->ParentItem->ParentInterface;

	if (ActualTexture && Interface->ClickedItem)
	{
		Interface->ClickedItem->Num=max(0,min(Interface->ClickedItem->Num,ActualTexture->CommandNumber-1));
		ATCI=EditMenu->ItemList[EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Filter].Interface;
		ActualTextureCommandInterface->SelectedLeftTexture=EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Layer;
		for (int x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
		DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;
	}
}

void __stdcall ImportTexture(cINTERFACE* Interface, int ButtonId)
{
	ITEM *Selected=ReturnItem(TextureFileList,*(int*)Interface->ItemList[ButtonId].data);
	char* NewLine=new char[strlen(Selected->Name)+strlen(TextureDir)+10];
	memset(NewLine,0,strlen(Selected->Name)+strlen(TextureDir)+10);
	sprintf(NewLine,"%s\\%s\0",TextureDir,Selected->Name);
	if (TextureList)
	{
		FILE *f;
		f=fopen(NewLine,"rb");
		if (f)
		{
			char ID[9];
			memset(ID,0,9);
			fread(ID,8,1,f);
			if (strcmp(ID,"aDDict2T\0")) ErrorMessage="THE FILE IS NOT AN A.D.D.I.C.T.2 TEXTURE FILE!";
				else
				{
					RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->XRes];
					ActualTexture->CommandNumber=0;
					ActualTexture->CommandList=new ITEMLIST();
					*(int*)CommandMenuData=(int)ActualTexture->CommandList;
					int x;
					for (x=0; x<8; x++)
					{
						ActualTexture->Materials[x].Used=false;
						//if (ActualTexture->Materials[x].Name) delete ActualTexture->Materials[x].Name;
						memset(ActualTexture->Layers[x].Data,0,ActualTexture->XRes*ActualTexture->XRes*4);
						for (int a=0; a<ActualTexture->XRes*ActualTexture->XRes; a++)	ActualTexture->Layers[x].Data[a].a=255;
						ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);
					}
					StatusBar->ParentItem->ActiveBranch=true;
					int i;
					fread(&i,4,1,f); // Texture Size
					fread(&i,4,1,f); // CommandNumber
					for (x=0; x<i; x++)
					{
						ActualTexture->Commands[x].data=NULL;
						ActualTexture->Commands[x].DataSize=0;
						fread(&ActualTexture->Commands[x].Filter,1,1,f); // 4 bit
						fread(&ActualTexture->Commands[x].Layer,1,1,f); // 4 bit
						fread(&ActualTexture->Commands[x].Operator,1,1,f); //2 bit
						fread(&ActualTexture->Commands[x].OperatorMask,4,1,f); //4 bit
						switch (ActualTexture->Commands[x].Filter)
						{
						case TEXGEN_Envmap:					{
							int Size = sizeof(ENVMAPTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Plasma:					{
							int Size = sizeof(PLASMATEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Map:					{
							int Size = sizeof(MAPTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Blur:					{
							int Size = sizeof(BLURTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_DirBlur:				{
							int Size = sizeof(DIRBLURTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Text:					{
							int Size = sizeof(TEXTTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							memset(ActualTexture->Commands[x].data,0,Size);
							fread(ActualTexture->Commands[x].data,1,Size-4,f);
							char* NewLine=new char[256];
							memset(NewLine,0,256);
							char c;
							c=1;
							for (int y=0; c!=0; y++)
							{
								fread(&c,1,1,f);
								NewLine[y]=c;
							}

							void *MemPos=(char*)ActualTexture->Commands[x].data+Size-4;
							memcpy(MemPos,&NewLine,4);

							break;					}
						case TEXGEN_SubPlasma:				{
							int Size = sizeof(SUBPLASMATEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_FractalPlasma:			{
							int Size = sizeof(FRACTALPLASMATEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Colorize:				{
							int Size = sizeof(COLORIZETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Shade:					{
							int Size = sizeof(SHADETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Brightness:				{
							int Size = sizeof(BRIGHTNESSTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Copy:					{
							int Size = sizeof(COPYTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Cells:					{
							int Size = sizeof(CELLSTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Twirl:					{
							int Size = sizeof(TWIRLTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_SineDist:				{
							int Size = sizeof(SINEDISTTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Mix:					{
							int Size = sizeof(MIXTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_MixMap:					{
							int Size = sizeof(MIXMAPTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Scale:					{
							int Size = sizeof(SCALETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_SineColor:				{
							int Size = sizeof(SINECOLORTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Max:					{
							int Size = sizeof(MAXTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_HSV:					{
							int Size = sizeof(HSVTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Glass:				{
							int Size = sizeof(GLASSTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Pixelize:					{
							int Size = sizeof(PIXELIZETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Offset:					{
							int Size = sizeof(OFFSETTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Crystalize:					{
							int Size = sizeof(CRYSTALIZETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Rectangle:					{
							int Size = sizeof(RECTANGLETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Circle:					{
							int Size = sizeof(CIRCLETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Contrast:					{
							int Size = sizeof(CONTRASTTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Gradient:					{
							int Size = sizeof(GRADIENTTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Rotozoom:					{
							int Size = sizeof(ROTOZOOMTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_ChamferRectangle:					{
							int Size = sizeof(CHAMFERRECTANGLETEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_Dots:					{
							int Size = sizeof(DOTSTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							ActualTexture->Commands[x].DataSize=Size;
							fread(ActualTexture->Commands[x].data,1,Size,f);
							break;					}
						case TEXGEN_MakeMaterial:					{
							int Size = sizeof(MAKEMATERIALTEMPLATE);
							ActualTexture->Commands[x].data=new char[Size];
							memset(ActualTexture->Commands[x].data,0,Size);
							fread(ActualTexture->Commands[x].data,1,Size-4,f);
							char* NewLine=new char[256];
							memset(NewLine,0,256);
							char c;
							c=1;
							for (int y=0; c!=0; y++)
							{
								fread(&c,1,1,f);
								NewLine[y]=c;
							}

							void *MemPos=(char*)ActualTexture->Commands[x].data+Size-4;
							memcpy(MemPos,&NewLine,4);

							break;					}
						case TEXGEN_Jpeg:
							{
								ActualTexture->Commands[x].data=new char[8];
								JPEGTEMPLATE *a=(JPEGTEMPLATE*)ActualTexture->Commands[x].data;
								fread(&a->JPEGDataSize,1,4,f);
								a->JPEGData=new unsigned char[a->JPEGDataSize];
								fread(a->JPEGData,1,a->JPEGDataSize,f);
							}
						}
						//ActualTexture->CommandNumber++;
						ActualTexture->AddCommand(&(ActualTexture->Commands[x]));
						CommandMenu->ParentItem->ItemNum=ActualTexture->CommandList->ItemNum;

					}
					StatusBar->ItemList[0].iMax=i;
					for (x=0; x<i; x++)
					{
						UndoStatus=x+1;
						PerformCommand(ActualTexture,&ActualTexture->Commands[x],Buffer);
						if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg); 
						}
						else
						{
							glClear(0x4100);
							glColor4f(highblue);
							rectangle(-1,35,1025,35);
							glColor4f(1,1,1,1);

							MainGUI.DrawGUI();
							//glRasterPos2i(410,21);
							//glPrint("LOADING...",base);
							SwapBuffers(hDC);
						}
					}
					
					EditMenu->ParentItem->ParentInterface->ClickedItem=ActualTexture->CommandList->List;
					EditMenu->ParentItem->ParentInterface->ClickedNum=0;
					EditMenu->ParentItem->ParentInterface->ItemList[0].SelectedItem=0;
					EditMenu->ParentItem->ParentInterface->ItemList[0].StartItem=0;

					cINTERFACE *Interface=EditMenu->ParentItem->ParentInterface;
					
					EditMenu->SetActiveBranch(((COMMAND*)Interface->ClickedItem->Data)->Filter);
					SelectedFilter=((COMMAND*)Interface->ClickedItem->Data)->Filter;
					EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Layer=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
					EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Operator=ActualTexture->Commands[Interface->ClickedItem->Num].Operator;
					for (x=0; x<4; x++) EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.OperatorMask[x]=ActualTexture->Commands[Interface->ClickedItem->Num].OperatorMask[x];
					memcpy(EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.data,ActualTexture->Commands[Interface->ClickedItem->Num].data,EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.DataSize);
					EditMenu->ClickedNum=Interface->ClickedItem->Num;
					ATCI=EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface;
					ActualTextureCommandInterface->SelectedLeftTexture=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
					for (x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
					DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;

					EditMenu->ParentItem->SelectedItem=0;
					EditMenu->ParentItem->StartItem=0;

					for (x=0; x<8; x++)
						ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);
			}
			StatusBar->ParentItem->ActiveBranch=false;
			fclose(f);
		}
		else
			ErrorMessage="ERROR WHILE OPENING FILE!";
	}
	else ErrorMessage="CREATE A TEXTURE FIRST!";
	BuildMaterialList();
	WaitLeftButton=true;
}

void __stdcall GUI_CheckByteSelect(cINTERFACE* Interface, int ButtonId)
{
	Interface->ItemList[ButtonId].ButtonPushed=(*(unsigned char*)Interface->ItemList[ButtonId].data)==Interface->ItemList[ButtonId].InterfaceItemId;
}

void __stdcall GUI_ButtonSelect_TextureMenu(cINTERFACE* Interface, int ButtonId)
{
 GUI_ButtonSelect(Interface,ButtonId);
 for (int x=0; x<Interface->ItemList[ButtonId].Interface->ItemNum; x++)
  if (Interface->ItemList[ButtonId].Interface->ItemList[x].ActiveBranch) {
		ActualTextureCommandInterface=Interface->ItemList[ButtonId].Interface->ItemList[x].Interface;
		for (int x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
		DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;
	}
}


void __stdcall GUI_ButtonSelect_SetTextureCommandInterface(cINTERFACE* Interface, int ButtonId)
{
	ActualTextureCommandInterface=Interface->ItemList[ButtonId].Interface;
	for (int x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
	DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;
	GUI_ButtonSelect(Interface,ButtonId);
};

void __stdcall GUI_ButtonSelect_SetTextureCommandInterfaceJPG(cINTERFACE* Interface, int ButtonId)
{
	JpegList->Last=NULL;
	JpegList->List=NULL;
	JpegList->ItemNum=0;
	// kill old list...
	SetupFileList(JpegList,JpegMsk);
	//Interface->ItemList[ButtonId].ItemNum=JpegList->ItemNum;

	ActualTextureCommandInterface=Interface->ItemList[ButtonId].Interface;
	ActualTextureCommandInterface->ItemList[0].ItemNum=JpegList->ItemNum;
	for (int x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
	DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;
	GUI_ButtonSelect(Interface,ButtonId);
};

void __stdcall Scroller_SelectItem_TextureEditSelector(cINTERFACE* Interface, int ScrollerID)
{
	Scroller_SelectItem(Interface,ScrollerID);
	EditMenu->SetActiveBranch(((COMMAND*)Interface->ClickedItem->Data)->Filter);
    SelectedFilter=((COMMAND*)Interface->ClickedItem->Data)->Filter;
	EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Layer=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
	EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Operator=ActualTexture->Commands[Interface->ClickedItem->Num].Operator;
	int x;
	for (x=0; x<4; x++) EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.OperatorMask[x]=ActualTexture->Commands[Interface->ClickedItem->Num].OperatorMask[x];
	memcpy(EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.data,ActualTexture->Commands[Interface->ClickedItem->Num].data,EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.DataSize);
	EditMenu->ClickedNum=Interface->ClickedItem->Num;
	ATCI=EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface;
	ActualTextureCommandInterface->SelectedLeftTexture=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
	for (x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
	DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;
	if (SelectedFilter==TEXGEN_Jpeg)
	{
		JpegList->Last=NULL;
		JpegList->List=NULL;
		JpegList->ItemNum=0;
		// kill old list...
		SetupFileList(JpegList,JpegMsk);
		ActualTextureCommandInterface->ItemList[0].ItemNum=JpegList->ItemNum;
	}
}

void __stdcall GUI_ButtonSelect_ChangeLayer(cINTERFACE* Interface, int ButtonId)
{
	if (ActualTexture) ActualTextureCommandInterface->TextureCommand.Layer=ButtonId;
	else ActualTextureCommandInterface->TextureCommand.Layer=ButtonId;
	ActualTextureCommandInterface->SelectedLeftTexture=ButtonId;
	GUI_ButtonSelect(Interface,ButtonId);
};

void __stdcall GUI_GetTextureFileList(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<Interface->ItemNum; x++) Interface->ItemList[x].ButtonPushed=false;
	Interface->ItemList[ButtonId].ButtonPushed=true;
	Interface->SetActiveBranch(ButtonId);
	if (Interface->ItemList[ButtonId].ToolTip) LastClickedToolTip=Interface->ItemList[ButtonId].ToolTip;
	//TextureFileList=new ITEMLIST();
	TextureFileList->Last=NULL;
	TextureFileList->List=NULL;
	TextureFileList->ItemNum=0;
	// kill old list...
	SetupFileList(TextureFileList,TextureMsk);
	Interface->ItemList[ButtonId].ItemNum=TextureFileList->ItemNum;
}

void FILTERTEMPLATE::AddItem()
{
	if (ItemCapacity==ItemNum)
	{
		ItemCapacity=(ItemCapacity+10)*2;
		COMMANDPARAMETERTYPE *Buffer=new COMMANDPARAMETERTYPE[ItemCapacity];
		memset(Buffer,0,ItemCapacity*sizeof(COMMANDPARAMETERTYPE));
		memcpy(Buffer,ItemList,ItemNum*sizeof(COMMANDPARAMETERTYPE));
		delete ItemList;
		ItemList=Buffer;
	}

	ItemNum++;
}

void FILTERTEMPLATE::AddLayerSelector()
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_LAYER;
}

void FILTERTEMPLATE::AddByteBar(unsigned char Min, unsigned char Max, unsigned char Default, int Color, char *Text, char *ToolTip)
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_BYTE;
	ItemList[ItemNum-1].iMinValue=Min;
	ItemList[ItemNum-1].iMaxValue=Max;
	ItemList[ItemNum-1].iDefaultValue=Default;
	ItemList[ItemNum-1].Text=Text;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].Color=Color;
}

void FILTERTEMPLATE::AddChannelSelector()
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_CHANNEL;
}

void FILTERTEMPLATE::AddBooleanButton(int t1, int t2, char *Text)
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_BOOLEAN;
	ItemList[ItemNum-1].iMinValue=t1;
	ItemList[ItemNum-1].iMaxValue=t2;
	ItemList[ItemNum-1].Text=Text;
}

void FILTERTEMPLATE::AddRandSeed()
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_RANDSEED;
}

void FILTERTEMPLATE::AddSelectTool()
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_SELECTOR;
}

void FILTERTEMPLATE::AddTextButton()
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_TEXT;
}

void FILTERTEMPLATE::AddJpegSelector()
{
	AddItem();
	ItemList[ItemNum-1].Type=TXGEN_JPEGSELECTOR;
}

void __stdcall TexGen_TextButtonCheckCallBack(cINTERFACE* Interface, int ButtonId)
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
			char *NewLine=new char[strlen(CommandLine)+1];
			memset(NewLine,0,strlen(CommandLine)+1);
			memcpy(NewLine,CommandLine,strlen(CommandLine));
			memcpy(Interface->ItemList[ButtonId].data,&NewLine,4);
		}
	}

	glRasterPos2i(Interface->ItemList[ButtonId].x1+10,Interface->ItemList[ButtonId].y2+Interface->ItemList[ButtonId].t2+5);
	glPrint((char*)*(int*)Interface->ItemList[ButtonId].data,base);
}

void SetEnvironmentToActualTexture()
{
	if (ActualTexture)
	{
		int x;
		for (x=0; x<8; x++) 
			ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);

			//LeftDisplays[x]=ActualTexture->Layers[ActualTexture->LeftDisplay[x]].TextureHandle;
		for (x=0; x<8; x++) RightDisplays[x]=ActualTexture->Materials[x].ImageData.TextureHandle;
		//for (x=0; x<8; x++) TextureLayers[x]=ActualTexture->Layers[x].TextureHandle;
		*(int*)CommandMenuData=(int)ActualTexture->CommandList;
		CommandMenu->ParentItem->ItemNum=ActualTexture->CommandList->ItemNum;
		*((char**)MakeMaterialMenu->ItemList[1].data)=new char[30];
		sprintf(*((char**)MakeMaterialMenu->ItemList[1].data),"%s\0",ActualTexture->Name);
		//Scroller_SelectItem(CommandMenu->ParentItem->ParentInterface,CommandMenu->ParentItem->InterfaceItemId);
		//EditMenu->SetActiveBranch(ActualTexture->Commands[CommandMenu->ParentItem->SelectedItem].Filter);
		if (!ActualTexture->CommandNumber) EditMenu->SetActiveBranch(-1);
		else EditMenu->SetActiveBranch(ActualTexture->Commands[CommandMenu->ParentItem->SelectedItem].Filter);
	}
	else
	{
		for (int x=0; x<8; x++) RightDisplays[x]=-1;
		glDeleteTextures(8,LeftDisplays);
		*(int*)CommandMenuData=NULL;
		EditMenu->SetActiveBranch(-1);
	}
}

void __stdcall Scroller_SelectOnlyThisTexture(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
	ActualTexture=(TEXTURE*)(ScrollerFindItem((ITEMLIST*)ScrollerInterface->ItemList[ScrollerID].data,ScrollerInterface->ItemList[ScrollerID].SelectedItem)->Data);
	SetEnvironmentToActualTexture();
}

cINTERFACE *TextureSizeSelector;
cINTERFACE *TextureSelect;

void __stdcall TexGen_NewTextureCheckCallBack(cINTERFACE* Interface, int ButtonId)
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

			int TextureSize=256;

			if (TextureSizeSelector->ItemList[0].ButtonPushed) TextureSize=32;
			if (TextureSizeSelector->ItemList[1].ButtonPushed) TextureSize=64;
			if (TextureSizeSelector->ItemList[2].ButtonPushed) TextureSize=128;
			//if (TextureSizeSelector->ItemList[3].ButtonPushed) TextureSize=256;
			if (TextureSizeSelector->ItemList[4].ButtonPushed) TextureSize=512;
			if (TextureSizeSelector->ItemList[5].ButtonPushed) TextureSize=1024;
			if (TextureSizeSelector->ItemList[6].ButtonPushed) TextureSize=2048;

			if (!TextureList)
			{
				TextureList=new TEXTURE(TextureSize);
				TextureList->Next=NULL;
				TextureList->Id=0;
			}
			else
			{
				TextureSelect->ItemList[0].SelectedItem++;
				TEXTURE *Buffer=new TEXTURE(TextureSize);
				Buffer->Next=TextureList;
				Buffer->Id=TextureList->Id+1;
				TextureList=Buffer;
			}

			char *NewLine=new char[strlen(CommandLine)+51];
			memset(NewLine,0,strlen(CommandLine)+51);
			//memcpy(NewLine,CommandLine,strlen(CommandLine));
			//memcpy(Interface->ItemList[ButtonId].data,&NewLine,4);

			_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s (%dx%d)",TextureList->Id,CommandLine,TextureSize,TextureSize);

			TextureListScroller->AddItem(NewLine,TextureList,Scroller_SelectOnlyThisTexture,GUI_RenameTexture,nop);

			
			ActualTexture=TextureList;
			ActualTexture->Name=new char[strlen(CommandLine)+1];
			memset(ActualTexture->Name,0,strlen(CommandLine)+1);
			memcpy(ActualTexture->Name,CommandLine,strlen(CommandLine));
			SetEnvironmentToActualTexture();
			// ADD NEW TEXTURE
		}

		//glRasterPos2i(Interface->ItemList[ButtonId].x1+10,Interface->ItemList[ButtonId].y2+Interface->ItemList[ButtonId].t2+5);
		//int *Pointer;
		//memcpy(&Pointer,Interface->ItemList[ButtonId].data,4);

		//glPrint((char*)(Pointer),base);
	}
}

/*
void __stdcall GUI_ButtonSwitch_DataChange(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSwitch(Interface,ButtonId);
	((char*)Interface->ItemList[ButtonId].data)[ButtonId]=!((char*)Interface->ItemList[ButtonId].data)[ButtonId];
};
*/
void __stdcall ActivateIfLastButtonPushed(cINTERFACE* Interface, int ButtonId)
{
	if (ButtonId>0)
	{
		Interface->ItemList[ButtonId].ActiveBranch=Interface->ItemList[ButtonId-1].ButtonPushed;
	}
}

void __stdcall GUI_RandomButtonPrev(cINTERFACE* Interface, int ButtonId)
{
	rndseedhack=--(*(unsigned char*)Interface->ItemList[ButtonId].data);
	((GUICALLBACK)Interface->ItemList[ButtonId].data2)(Interface,ButtonId);
	WaitLeftButton=true;
};

void __stdcall GUI_RandomButtonNext(cINTERFACE* Interface, int ButtonId)
{
	rndseedhack=++(*(unsigned char*)Interface->ItemList[ButtonId].data);
	((GUICALLBACK)Interface->ItemList[ButtonId].data2)(Interface,ButtonId);
	WaitLeftButton=true;
};

void __stdcall GUI_ButtonSelect_SelectTextureSize(cINTERFACE* Interface, int ButtonId)
{
	if (ActualTexture)
	{
		switch (ActualTexture->XRes)		
		{
		case 32: GUI_ButtonSelect(Interface,0); break;
		case 64: GUI_ButtonSelect(Interface,1); break;
		case 128: GUI_ButtonSelect(Interface,2); break;
		case 256: GUI_ButtonSelect(Interface,3); break;
		case 512: GUI_ButtonSelect(Interface,4); break;
		case 1024: GUI_ButtonSelect(Interface,5); break;
		}
	}
}


void __stdcall GUI_ButtonSelect_TextureSizeChange(cINTERFACE* Interface, int ButtonId)
{
	if (ActualTexture)
	{
		GUI_ButtonSelect(Interface,ButtonId);
		int NewSize;
		switch (ButtonId)
		{
			case 0: NewSize=32; break;
			case 1: NewSize=64; break;
			case 2: NewSize=128; break;
			case 3: NewSize=256; break;
			case 4: NewSize=512; break;
			case 5: NewSize=1024; break;
		}
		if (NewSize!=ActualTexture->XRes)
		{
			int x;
			for (x=0; x<8; x++)
			{
			}
			for (x=0; x<8; x++)
			{
				delete ActualTexture->Layers[x].Data;
				delete ActualTexture->Materials[x].ImageData.Data;
				ActualTexture->Materials[x].ImageData.Data=new RGBA[NewSize*NewSize];
				memset(ActualTexture->Materials[x].ImageData.Data,0,NewSize*NewSize*4);
				ActualTexture->Layers[x].Data=new RGBA[NewSize*NewSize];
				memset(ActualTexture->Layers[x].Data,0,NewSize*NewSize*4);
				for (int a=0; a<NewSize*NewSize; a++) ActualTexture->Layers[x].Data[a].a=255;
				/*glBindTexture(GL_TEXTURE_2D,ActualTexture->Layers[x].TextureHandle);
				glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,NewSize,NewSize,0,GL_RGBA,GL_UNSIGNED_BYTE,ActualTexture->Layers[x].Data);
				glBindTexture(GL_TEXTURE_2D,ActualTexture->Materials[x].ImageData.TextureHandle);
				glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,NewSize,NewSize,0,GL_RGBA,GL_UNSIGNED_BYTE,ActualTexture->Materials[x].ImageData.Data);*/
			}
			ActualTexture->XRes=ActualTexture->OXRes=NewSize;
			ActualTexture->YRes=NewSize;

			StatusBar->ParentItem->ActiveBranch=true;
			StatusBar->ItemList[0].iMax=ActualTexture->CommandNumber;
			RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->YRes];
			for (x=0; x<8; x++)
			{
				ActualTexture->Materials[x].Used=false;
				memset(ActualTexture->Layers[x].Data,0,ActualTexture->XRes*ActualTexture->XRes*4);
				for (int a=0; a<ActualTexture->XRes*ActualTexture->XRes; a++)	ActualTexture->Layers[x].Data[a].a=255;
				ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);
			}
			for (x=0; x<ActualTexture->CommandNumber; x++)
			{
				UndoStatus=x+1;
				PerformCommand(ActualTexture,&ActualTexture->Commands[x],Buffer);
				if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					glClear(0x4100);
					glColor4f(highblue);
					rectangle(-1,35,1025,35);
					glColor4f(1,1,1,1);
					MainGUI.DrawGUI();
					
					SwapBuffers(hDC);
				}
			}
			for (x=0; x<8; x++)
				ActualTexture->UpdateTexture(ActualTexture->Layers[x].Data,LeftDisplays[x]);
			StatusBar->ParentItem->ActiveBranch=false;

			BuildMaterialList();
			WaitLeftButton=true;

		}
	}
};

void __stdcall GUI_ResizeTexture(cINTERFACE* Interface, int ButtonId)
{	
	bool r=false;
	for (int x=0; x<8; x++) if (RightClickInWindow(Interface->ItemList[x].x1,Interface->ItemList[x].y1,Interface->ItemList[x].x2,Interface->ItemList[x].y2)) r=true;

	if (r)
	{
		Interface->ItemList[ButtonId].x1=-1;
		Interface->ItemList[ButtonId].x2=-1;
		Interface->ItemList[ButtonId].y1=-1;
		Interface->ItemList[ButtonId].y2=-1;
	}
	else
	{
		Interface->ItemList[ButtonId].x1=Interface->ItemList[ButtonId].ox1;
		Interface->ItemList[ButtonId].x2=Interface->ItemList[ButtonId].ox2;
		Interface->ItemList[ButtonId].y1=Interface->ItemList[ButtonId].oy1;
		Interface->ItemList[ButtonId].y2=Interface->ItemList[ButtonId].oy2;
	}

	if (Interface->ItemList[ButtonId].RightClickInWindow)
	{		
		Interface->ItemList[ButtonId].x1=50;
		if (ButtonId>=4) Interface->ItemList[ButtonId].y1=50+130;
		else Interface->ItemList[ButtonId].y1=50;

		Interface->ItemList[ButtonId].x2=307+257;
		if (ButtonId>=4) Interface->ItemList[ButtonId].y2=307+257+130;
		else Interface->ItemList[ButtonId].y2=307+257;
	}
};

void __stdcall UpdateRandSeedText(cINTERFACE* Interface, int ButtonId)
{
	//if (ActualTexture)
	{
		delete[] RandSeedText->ParentItem->Text;
		RandSeedText->ParentItem->Text=new char[20];
		sprintf(RandSeedText->ParentItem->Text,"%d\0",rndseedhack);	
	}
};

void GenerateGUIFromTemplate(cINTERFACE *Parent, FILTERTEMPLATE *Template, GUICALLBACK DoEffect, GUICALLBACK PreView, int CommandID, int pos,bool PostEditMenu)
{
	cINTERFACE *SubInterface;
	int Position=pos;
	int TemplateSize=Template->ItemNum;

	int x;
	for (x=0; x<Template->ItemNum; x++) 
	{
		if (Template->ItemList[x].Type==TXGEN_TEXT) TemplateSize+=3;
		if (Template->ItemList[x].Type==TXGEN_JPEGSELECTOR) TemplateSize+=7;
	}

	Parent->TextureCommand.data=new char[TemplateSize];
	memset(Parent->TextureCommand.data,0,TemplateSize);
	void *TemplateDataPosition=Parent->TextureCommand.data;

	for (x=0; x<Template->ItemNum; x++)
	{
		switch (Template->ItemList[x].Type)
		{
		case TXGEN_LAYER:
			{	
				Position+=10;
				SubInterface=Parent->AddSubInterface();
				Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				*(unsigned char*)(Parent->ItemList[Parent->ItemNum-1].data)=0;
				for (int x=0; x<8; x++)
				{
					SubInterface->AddTextureDisplay(753+x*32,Position,753+(x+1)*32,Position+32,&LeftDisplays[x],GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
					SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				}
				/*for (x=0; x<8; x++)
				{
					SubInterface->AddTextureDisplay(753+x*32,Position+32,753+(x+1)*32,Position+64,&TextureLayers[x+8],GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
					SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				}*/
				SubInterface->ItemList[0].ButtonPushed=true;
				*(unsigned char*)TemplateDataPosition=0;
				TemplateDataPosition=(void*)((int)TemplateDataPosition+1);
				Position+=42;
				break;
			}
		case TXGEN_BYTE:
			{
				Parent->AddByteBar(744,Position,257,Template->ItemList[x].Color,(unsigned char*)TemplateDataPosition,Template->ItemList[x].iMinValue,Template->ItemList[x].iMaxValue,Template->ItemList[x].iDefaultValue,Template->ItemList[x].Text,Template->ItemList[x].ToolTip);
				*(unsigned char*)TemplateDataPosition=Template->ItemList[x].iDefaultValue;
				TemplateDataPosition=(void*)((int)TemplateDataPosition+1);
				Position+=19;
				break;
			}
		case TXGEN_CHANNEL:
			{
				Position+=4;
				SubInterface=Parent->AddSubInterface();
				SubInterface->AddSkinnedButton(823,Position,RedButton,"RED CHANNEL",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddSkinnedButton(853,Position,GreenButton,"GREEN CHANNEL",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddSkinnedButton(883,Position,BlueButton,"BLUE CHANNEL",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddSkinnedButton(913,Position,AlphaButton,"ALPHA CHANNEL",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->ItemList[0].ButtonPushed=true;
				Position+=22;
				*(unsigned char*)TemplateDataPosition=0;
				//Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				TemplateDataPosition=(void*)((int)TemplateDataPosition+1);
				break;
			}
		case TXGEN_BOOLEAN:
			{
				Position+=4;
				Parent->AddButton(781+4,Position,969+5,Position+16,Template->ItemList[x].iMinValue,Template->ItemList[x].iMaxValue,Template->ItemList[x].Text,Template->ItemList[x].Text,GUI_ButtonSwitch_DataChange,nop,nop,GUI_CheckByteSwitch);
				Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				*(unsigned char*)TemplateDataPosition=0;
				TemplateDataPosition=(void*)((int)TemplateDataPosition+1);
				Position+=16;
				break;
			}
		case TXGEN_RANDSEED:
			{
				Parent->AddButton(760,543,810,559,9,11,"RAND-","GENERATE PREVIOUS RANDOM PREVIEW",GUI_RandomButtonPrev,nop,nop,UpdateRandSeedText);
				Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				Parent->ItemList[Parent->ItemNum-1].data2=PreView;
				Parent->AddButton(811,543,860,559,7,11,"RAND+","GENERATE NEXT RANDOM PREVIEW",GUI_RandomButtonNext,nop,nop,nop);
				Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				Parent->ItemList[Parent->ItemNum-1].data2=PreView;
				*(unsigned char*)TemplateDataPosition=0;
				TemplateDataPosition=(void*)((int)TemplateDataPosition+1);
				break;
			}
		case TXGEN_SELECTOR:
			{
				Position+=4;
				//Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				SubInterface=Parent->AddSubInterface();
				SubInterface->AddButton(781+4,Position,879,Position+16,30,11,"ARIAL","SET FONT TO ARIAL",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddButton(880,Position,969+5,Position+16,20,11,"VERDANA","SET FONT TO VERDANA",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition; Position+=17;
				SubInterface->AddButton(781+4,Position,879,Position+16,8,11,"COURIER NEW","SET FONT TO COURIER NEW",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddButton(880,Position,969+5,Position+16,19,11,"WEBDINGS","SET FONT TO WEBDINGS",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition; Position+=17;
				SubInterface->AddButton(781+4,Position,879,Position+16,27,11,"LUCIDA","SET FONT TO LUCIDA CONSOLE",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddButton(880,Position,969+5,Position+16,23,11,"TAHOMA","SET FONT TO TAHOMA",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition; Position+=17;
				SubInterface->AddButton(781+4,Position,879,Position+16,27,11,"IMPACT","SET FONT TO IMPACT",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddButton(880,Position,969+5,Position+16,14,11,"WINGDINGS","SET FONT TO WINGDINGS",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition; Position+=17;
				SubInterface->AddButton(781+4,Position,879,Position+16,7,11,"T. NEW ROMAN","SET FONT TO TIMES NEW ROMAN",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddButton(880,Position,969+5,Position+16,14,11,"SANS SERIF","SET FONT TO MICROSOFT SANS SERIF",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition; Position+=17;
				SubInterface->AddButton(781+4,Position,879,Position+16,13,11,"TREBUCHET","SET FONT TO TREBUCHET",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition;
				SubInterface->AddButton(880,Position,969+5,Position+16,24,11,"SYMBOL","SET FONT TO SYMBOL",GUI_ButtonSelect_DataChange,nop,nop,nop);
				SubInterface->ItemList[SubInterface->ItemNum-1].data=TemplateDataPosition; Position+=17;
				SubInterface->ItemList[0].ButtonPushed=true;
				*(unsigned char*)TemplateDataPosition=0;
				TemplateDataPosition=(void*)((int)TemplateDataPosition+1);
				break;
			}
		case TXGEN_TEXT:
			{
				Position+=4;
				Parent->AddButton(781+4,Position,969+5,Position+16,58,11,"ENTER TEXT","ENTER TEXT TO BE PRINTED",GUI_ReadLine,nop,nop,TexGen_TextButtonCheckCallBack);
				Parent->ItemList[Parent->ItemNum-1].data=TemplateDataPosition;
				Position+=36;

				/*unsigned char *TextPointer=new unsigned char[2];
				memset(TextPointer,0,2);
				*(long int*)TemplateDataPosition=(long int)TextPointer;*/
				*(long int*)TemplateDataPosition=0;

				TemplateDataPosition=(void*)((int)TemplateDataPosition+4);
				break;
			}
		case TXGEN_JPEGSELECTOR:
			{
				Position+=4;
				Parent->AddItemList(764,Position,222,20,JpegList,"SELECT JPG");
				Parent->ItemList[Parent->ItemNum-4].data2=TemplateDataPosition;
				if (JpegList->ItemNum) 
				{
					JPEGDATA *a=(JPEGDATA*)Parent->ItemList[Parent->ItemNum-4].data2;
					char *fname=JpegList->List->Name;
					char longfile[256];
					memset(longfile,0,256);
					sprintf(longfile,"%s/%s\0",JpegDir,fname);
					FILE *f=fopen(longfile,"rb");
					if (f)
					{
						fseek(f,0,SEEK_END);
						__int64 pos;
						fgetpos(f,&pos);
						fseek(f,0,SEEK_SET);
						a->DataSize=(int)pos;
						a->Data=new unsigned char[(int)pos];
						memset(a->Data,0,(int)pos);
						fread(a->Data,a->DataSize,1,f);
						fclose(f);
					}
				}
				TemplateDataPosition=(void*)((int)TemplateDataPosition+4);
			}
		}
	}

	Parent->TextureCommand.DataSize=TemplateSize;
	Parent->TextureCommand.Layer=0;
	Parent->TextureCommand.Filter=CommandID;
	Parent->SelectedLeftTexture=0;
	glGenTextures(1,&Parent->Preview);
	if (!PostEditMenu)
	{
		if (Template!=&MakeMaterialTemplate)
		Parent->AddButton(760,600,860,616,25,11,"PREVIEW","GENERATE PREVIEW IMAGE (SPACE)",PreView,nop,nop,nop,' ');
		Parent->AddButton(760,617,860,633,19,11,"DO EFFECT","APPLY FILTER",DoEffect,nop,nop,nop);
		Parent->AddButton(760,637,860,653,35,11,"UNDO","UNDO LAST FILTER",UndoTextureEffect,nop,nop,nop);
		Parent->AddTextureDisplay(880,525,1004,653,&(Parent->Preview),nop,nop,nop,nop);
	}
	else
	{
		Parent->AddButton(760,617,860,633,27,11,"CHANGE","CHANGE FILTER AND RECALCULATE TEXTURE (SPACE)",DoEffect,nop,nop,nop,' ');
		cINTERFACE *TextureSizeSelector=Parent->AddSubInterface();
		TextureSizeSelector->AddButton(760,637,783,653,6,11,"32","CHANGE TEXTURE SIZE TO 32x32",GUI_ButtonSelect_TextureSizeChange,nop,nop,GUI_ButtonSelect_SelectTextureSize);
		TextureSizeSelector->AddButton(784,637,807,653,6,11,"64","CHANGE TEXTURE SIZE TO 64x64",GUI_ButtonSelect_TextureSizeChange,nop,nop,GUI_ButtonSelect_SelectTextureSize);
		TextureSizeSelector->AddButton(808,637,837,653,6,11,"128","CHANGE TEXTURE SIZE TO 128x128",GUI_ButtonSelect_TextureSizeChange,nop,nop,GUI_ButtonSelect_SelectTextureSize);
		TextureSizeSelector->AddButton(838,637,867,653,6,11,"256","CHANGE TEXTURE SIZE TO 256x256",GUI_ButtonSelect_TextureSizeChange,nop,nop,GUI_ButtonSelect_SelectTextureSize);
		TextureSizeSelector->AddButton(868,637,897,653,6,11,"512","CHANGE TEXTURE SIZE TO 512x512",GUI_ButtonSelect_TextureSizeChange,nop,nop,GUI_ButtonSelect_SelectTextureSize);
		TextureSizeSelector->AddButton(898,637,932,653,6,11,"1024","CHANGE TEXTURE SIZE TO 1024x1024",GUI_ButtonSelect_TextureSizeChange,nop,nop,GUI_ButtonSelect_SelectTextureSize);
		//TextureSizeSelector->AddButton(937+12,500,971+12,516,6,11,"2048","SET NEW TEXTURE SIZE TO 2048x2048",GUI_ButtonSelect,nop,nop,nop);
		TextureSizeSelector->ItemList[3].ButtonPushed=true;
	}
	
	cINTERFACE *Selector=Parent->AddSubInterface();
	Selector->AddSkinnedButton(769+2,560,RedButton,"TOGGLE FILTERING FOR RED CHANNEL",GUI_ButtonSwitch_DataChange,nop,nop,GUI_CheckByteSwitch);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.OperatorMask[0];
	Selector->AddSkinnedButton(788+2,560,GreenButton,"TOGGLE FILTERING FOR GREEN CHANNEL",GUI_ButtonSwitch_DataChange,nop,nop,GUI_CheckByteSwitch);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.OperatorMask[1];
	Selector->AddSkinnedButton(807+2,560,BlueButton,"TOGGLE FILTERING FOR BLUE CHANNEL",GUI_ButtonSwitch_DataChange,nop,nop,GUI_CheckByteSwitch);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.OperatorMask[2];
	Selector->AddSkinnedButton(826+2,560,AlphaButton,"TOGGLE FILTERING FOR ALPHA CHANNEL",GUI_ButtonSwitch_DataChange,nop,nop,GUI_CheckByteSwitch);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.OperatorMask[3];
	Selector->ItemList[0].ButtonPushed=true;
	Selector->ItemList[1].ButtonPushed=true;
	Selector->ItemList[2].ButtonPushed=true;
	Parent->TextureCommand.OperatorMask[0]=true;
	Parent->TextureCommand.OperatorMask[1]=true;
	Parent->TextureCommand.OperatorMask[2]=true;
	Parent->TextureCommand.OperatorMask[3]=false;
	
	Selector=Parent->AddSubInterface();
	Selector->AddSkinnedButton(769+2,579,OverwriteButton,"SET OVERWRITE FILTERING",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.Operator;
	Selector->AddSkinnedButton(790,579,AdditiveButton2,"SET ADDITIVE FILTERING",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.Operator;
	Selector->AddSkinnedButton(809,579,SubstractiveButton2,"SET SUBSTRACTIVE FILTERING",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.Operator;
	Selector->AddSkinnedButton(828,579,MultiplicativeButton,"SET MULTIPLICATIVE FILTERING",GUI_ButtonSelect_DataChange,nop,nop,GUI_CheckByteSelect);
	Selector->ItemList[Selector->ItemNum-1].data=&Parent->TextureCommand.Operator;
	Selector->ItemList[0].ButtonPushed=true;
}

void InitTemplates()
{
//	EnvmapTemplate.AddBooleanButton(53,11,"SIZED ENVMAP");
//	EnvmapTemplate.AddChannelSelector();
	EnvmapTemplate.AddByteBar(0,255,128,GUI_NONE,"SIZE: ","ENVMAP SIZE");

	TextTemplate.AddByteBar(0,255,30,GUI_NONE,"SIZE: ","FONT SIZE");
	TextTemplate.AddByteBar(0,255, 0,GUI_NONE,"X POSITION: ","X POSITION");
	TextTemplate.AddByteBar(0,255, 0,GUI_NONE,"Y POSITION: ","Y POSITION");
	TextTemplate.AddBooleanButton(77,11,"BOLD");
	TextTemplate.AddBooleanButton(75,11,"ITALIC");
	TextTemplate.AddSelectTool();
	TextTemplate.AddByteBar(0,255,0x00,GUI_NONE,"CHARSPACING: ","CHARSPACING");
	TextTemplate.AddTextButton();

//	PlasmaTemplate.AddChannelSelector();
	PlasmaTemplate.AddByteBar(0,30,3,GUI_NONE,"X SINES: ","NUMBER OF X SINES");
	PlasmaTemplate.AddByteBar(0,30,3,GUI_NONE,"Y SINES: ","NUMBER OF Y SINES");

	MapTemplate.AddLayerSelector();
	MapTemplate.AddChannelSelector();
	MapTemplate.AddByteBar(0,255, 0,GUI_NONE,"X AMOUNT: ","X AMOUNT");
	MapTemplate.AddLayerSelector();
	MapTemplate.AddChannelSelector();
	MapTemplate.AddByteBar(0,255, 0,GUI_NONE,"Y AMOUNT: ","Y AMOUNT");

	BlurTemplate.AddByteBar(1,255, 1,GUI_NONE,"AMOUNT: ","NUMBER OF BLUR ITERATIONS");
	BlurTemplate.AddByteBar(1,255, 1,GUI_NONE,"WIDTH: ", "DIAMETER OF SAMPLING");

	DirBlurTemplate.AddLayerSelector();
	DirBlurTemplate.AddChannelSelector();
	DirBlurTemplate.AddByteBar(1,255, 1,GUI_NONE,"DEPTH: ", "NUMBER OF SAMPLES");

	SubPlasmaTemplate.AddByteBar(1,10, 3,GUI_NONE,"SIZE: ", "\"CHUNKINESS\" OF THE SUBPLASMA (1 = BIG LUMPS, 10 = WHITE NOISE)");
	SubPlasmaTemplate.AddRandSeed();

	FractalPlasmaTemplate.AddByteBar(1, 10,   1,GUI_NONE,"MIN: ", "MINIMUM \"CHUNKINESS\" LEVEL (1 = BIG LUMPS, 10 = WHITE NOISE)");
	FractalPlasmaTemplate.AddByteBar(1, 10,  10,GUI_NONE,"MAX: ", "MAXIMUM \"CHUNKINESS\" LEVEL (1 = BIG LUMPS, 10 = WHITE NOISE)");
	FractalPlasmaTemplate.AddByteBar(1,255,0x80,GUI_NONE,"BLEND: ", "BLENDFACTOR");
	FractalPlasmaTemplate.AddRandSeed();

	ColorizeTemplate.AddByteBar(0,255,  0,GUI_RED  ,"START R: ","STARTING COLOR, RED CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,  0,GUI_GREEN,"START G: ","STARTING COLOR, GREEN CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,  0,GUI_BLUE ,"START B: ","STARTING COLOR, BLUE CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,  0,GUI_NONE ,"START A: ","STARTING COLOR, ALPHA CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,255,GUI_RED  ,"END R: ","ENDING COLOR, RED CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,255,GUI_GREEN,"END G: ","ENDING COLOR, GREEN CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,255,GUI_BLUE ,"END B: ","ENDING COLOR, BLUE CHANNEL");
	ColorizeTemplate.AddByteBar(0,255,255,GUI_NONE ,"END A: ","ENDING COLOR, ALPHA CHANNEL");
	ColorizeTemplate.AddChannelSelector();

	BrightnessTemplate.AddByteBar(0,255,127,GUI_NONE ,"AMOUNT: ","AMOUNT");

	ShadeTemplate.AddLayerSelector();
	ShadeTemplate.AddChannelSelector();

	CopyTemplate.AddLayerSelector();

	CellsTemplate.AddByteBar(1,255,10,GUI_NONE,"CELLS:","NUMBER OF CELLS");
	CellsTemplate.AddByteBar(1,255,50,GUI_NONE,"POWER:","SIZE OF CELLS");
	CellsTemplate.AddRandSeed();

	TwirlTemplate.AddByteBar(0,255,127,GUI_NONE,"AMOUNT:","AMOUNT OF TWIRL IN DEGREES");

	SineDistTemplate.AddByteBar(0, 30, 3,GUI_NONE,"X SINES: ","NUMBER OF X SINES");
	SineDistTemplate.AddByteBar(0, 30, 3,GUI_NONE,"Y SINES: ","NUMBER OF Y SINES");
	SineDistTemplate.AddByteBar(0,255,20,GUI_NONE,"X AMPLITUDE: ","AMPLITUDE OF X SINES");
	SineDistTemplate.AddByteBar(0,255,20,GUI_NONE,"Y AMPLITUDE: ","AMPLITUDE OF Y SINES");

	MixTemplate.AddLayerSelector();
	MixTemplate.AddByteBar(0,255,127,GUI_NONE,"AMOUNT: ","AMOUNT OF MIX (0 = LAYER 1, 255 = LAYER 2");

	MixMapTemplate.AddLayerSelector();
	MixMapTemplate.AddLayerSelector();
	MixMapTemplate.AddChannelSelector();

	ScaleTemplate.AddChannelSelector();
	ScaleTemplate.AddByteBar(0,255, 30,GUI_NONE,"MIN: ","MINIMUM");
	ScaleTemplate.AddByteBar(0,255,225,GUI_NONE,"MAX: ","MAXIMUM");

	SineColorTemplate.AddChannelSelector();
	SineColorTemplate.AddByteBar(0,255,3,GUI_NONE,"NUMBER OF SINES: ","NUMBER OF SINES");

	MaxTemplate.AddLayerSelector();

	HSVTemplate.AddByteBar(0,255,10,GUI_NONE,"HUE: ","HUE");
	HSVTemplate.AddByteBar(0,255,10,GUI_NONE,"SATURATION: ","SATURATION");

	GlassTemplate.AddLayerSelector();
	GlassTemplate.AddChannelSelector();
	GlassTemplate.AddByteBar(0,255,127,GUI_NONE,"AMOUNT: ","AMOUNT");

	PixelizeTemplate.AddByteBar(1,255,32,GUI_NONE,"X SQUARES: ","NUMBER OF SQUARES");
	PixelizeTemplate.AddByteBar(1,255,32,GUI_NONE,"Y SQUARES: ","NUMBER OF SQUARES");

	OffsetTemplate.AddByteBar(0,255,32,GUI_NONE,"X OFFSET: ","X OFFSET");
	OffsetTemplate.AddByteBar(0,255,32,GUI_NONE,"Y OFFSET: ","Y OFFSET");

	CrystalizeTemplate.AddByteBar(1,255,10,GUI_NONE,"CRYSTALS:","NUMBER OF CRYSTAL BLOCKS");
//	CrystalizeTemplate.AddByteBar(1,255,50,GUI_NONE,"SIZE:","SIZE OF CRYSTALS");
	CrystalizeTemplate.AddRandSeed();

	RectangleTemplate.AddByteBar(0,255,32,GUI_NONE,"X START: ","X STARTING POINT");
	RectangleTemplate.AddByteBar(0,255,32,GUI_NONE,"Y START: ","Y STARTING POINT");
	RectangleTemplate.AddByteBar(0,255,192,GUI_NONE,"X SIZE: ","X SIZE");
	RectangleTemplate.AddByteBar(0,255,192,GUI_NONE,"Y SIZE: ","Y SIZE");

	CircleTemplate.AddByteBar(0,255,64,GUI_NONE,"RADIUS: ","CIRCLE RADIUS");
	CircleTemplate.AddByteBar(0,255,32,GUI_NONE,"WIDTH: ","CIRCLE WIDTH");
	CircleTemplate.AddByteBar(0,255,127,GUI_NONE,"X OFFSET: ","X OFFSET");
	CircleTemplate.AddByteBar(0,255,127,GUI_NONE,"Y OFFSET: ","Y OFFSET");

	ContrastTemplate.AddByteBar(0,255,127,GUI_NONE,"AMOUNT:","CONTRAST AMOUNT");

	GradientTemplate.AddByteBar(0,255,  0,GUI_RED  ,"TOPLEFT R: ","TOP LEFT RED CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_GREEN,"TOPLEFT G: ","TOP LEFT GREEN CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_BLUE ,"TOPLEFT B: ","TOP LEFT BLUE CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_NONE ,"TOPLEFT A: ","TOP LEFT ALPHA CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_RED  ,"TOPRIGHT R: ","TOP RIGHT RED CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_GREEN,"TOPRIGHT G: ","TOP RIGHT GREEN CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_BLUE ,"TOPRIGHT B: ","TOP RIGHT BLUE CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_NONE ,"TOPRIGHT A: ","TOP RIGHT ALPHA CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_RED  ,"BOTTOMLEFT R: ","BOTTOM LEFT RED CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_GREEN,"BOTTOMLEFT G: ","BOTTOM LEFT GREEN CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_BLUE ,"BOTTOMLEFT B: ","BOTTOM LEFT BLUE CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_NONE ,"BOTTOMLEFT A: ","BOTTOM LEFT ALPHA CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_RED  ,"BOTTOMRIGHT R: ","BOTTOM RIGHT RED CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_GREEN,"BOTTOMRIGHT G: ","BOTTOM RIGHT GREEN CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_BLUE ,"BOTTOMRIGHT B: ","BOTTOM RIGHT BLUE CHANNEL");
	GradientTemplate.AddByteBar(0,255,  0,GUI_NONE ,"BOTTOMRIGHT A: ","BOTTOM RIGHT ALPHA CHANNEL");

	RotozoomTemplate.AddByteBar(0,255,127,GUI_NONE,"CENTER X: ","ROTATION CENTER X");
	RotozoomTemplate.AddByteBar(0,255,127,GUI_NONE,"CENTER Y: ","ROTATION CENTER Y");
	RotozoomTemplate.AddByteBar(0,255,  4,GUI_NONE,"ZOOM X: ","ZOOM X");
	RotozoomTemplate.AddByteBar(0,255,  4,GUI_NONE,"ZOOM Y: ","ZOOM Y");
	RotozoomTemplate.AddBooleanButton(77,11,"ZOOM IN");
	RotozoomTemplate.AddByteBar(0,255,  0,GUI_NONE,"ANGLE: ","ROTATION ANGLE");

	ChamferRectangleTemplate.AddByteBar(0,255,32,GUI_NONE,"X START: ","X STARTING POINT");
	ChamferRectangleTemplate.AddByteBar(0,255,32,GUI_NONE,"Y START: ","Y STARTING POINT");
	ChamferRectangleTemplate.AddByteBar(0,255,192,GUI_NONE,"X SIZE: ","X SIZE");
	ChamferRectangleTemplate.AddByteBar(0,255,192,GUI_NONE,"Y SIZE: ","Y SIZE");
	ChamferRectangleTemplate.AddByteBar(0,255,0,GUI_NONE,"CHAMFER VALUE: ","CHAMFER VALUE");

	DotsTemplate.AddByteBar(1,255,1,GUI_NONE,"COUNT1: ","NUMBER OF DOTS 1");
	DotsTemplate.AddByteBar(1,255,1,GUI_NONE,"COUNT2: ","NUMBER OF DOTS 2");
	DotsTemplate.AddByteBar(0,255,0,GUI_NONE,"MIN COL: ","SMALLEST BRIGHTNESS VALUE");
	DotsTemplate.AddByteBar(0,255,255,GUI_NONE,"MAX COL: ","GREATEST BRIGHTNESS VALUE");
	DotsTemplate.AddBooleanButton(51,11,"ROUNDED DOTS");
	DotsTemplate.AddByteBar(1,255,1,GUI_NONE,"MIN SIZE: ","MINIMAL DOT SIZE");
	DotsTemplate.AddByteBar(1,255,1,GUI_NONE,"MAX SIZE: ","MAXIMAL DOT SIZE");

	DotsTemplate.AddRandSeed();

	JpegTemplate.AddJpegSelector();

	FractalPlasmaOldTemplate.AddRandSeed(); 
	
	MakeMaterialTemplate.AddByteBar(0,7,0,GUI_NONE,"MATERIAL SLOT:","SELECT THE MATERIAL SLOT");
	MakeMaterialTemplate.AddTextButton();
}

void BuildTexGenGUI(cINTERFACE *TexGenGUI)
{
	RandSeedText=TexGenGUI->AddText(960,750,"RNDSEED");

	TexGenPrecalc();
	TextureListScroller=new ITEMLIST();
	TextureFileList=new ITEMLIST();
	JpegList=new ITEMLIST();
	SetupFileList(JpegList,JpegMsk);
	InitTemplates();

	DisplayArea=TexGenGUI->AddSubInterface();
	cINTERFACE *ScrollArea=TexGenGUI->AddSubInterface();
	cINTERFACE *TexGenMenu=TexGenGUI->AddSubInterface();

	glGenTextures(8,LeftDisplays);

	DisplayArea->AddTextureDisplay(50,50,307,307,&LeftDisplays[0],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);
	DisplayArea->AddTextureDisplay(307,50,307+257,307,&LeftDisplays[1],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);
	DisplayArea->AddTextureDisplay(50,307,307,307+257,&LeftDisplays[2],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);
	DisplayArea->AddTextureDisplay(307,307,307+257,307+257,&LeftDisplays[3],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);

	DisplayArea->AddTextureDisplay(50,307+257,179,307+387,&LeftDisplays[4],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);
	DisplayArea->AddTextureDisplay(179,307+257,179+128,307+387,&LeftDisplays[5],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);
	DisplayArea->AddTextureDisplay(50+257,307+257,179+257,307+387,&LeftDisplays[6],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);
	DisplayArea->AddTextureDisplay(179+257,307+257,179+128+257,307+387,&LeftDisplays[7],GUI_ButtonSelect_ChangeLayer,nop,nop,GUI_ResizeTexture);

	DisplayArea->ItemList[0].ButtonPushed=true;

	for (int x=0; x<8; x++)
	ScrollArea->AddTextureDisplay(347+276,60+80*x,347+276+65,60+65+80*x,&RightDisplays[x],nop,nop,nop,nop);
	/*ScrollArea->AddTextureDisplay(347+256,80+149,347+129+256,209+149,&RightDisplays[1],nop,nop,nop,nop);
	ScrollArea->AddTextureDisplay(347+256,80+149*2,347+129+256,209+149*2,&RightDisplays[2],nop,nop,nop,nop);
	ScrollArea->AddTextureDisplay(347+256,80+149*3,347+129+256,209+149*3,&RightDisplays[3],nop,nop,nop,nop);*/

	/*ScrollArea->AddButton(347+256+40,57,347+129+256-40,73,18,11,"UP","SCROLL UP",nop,nop,nop,nop);
	ScrollArea->AddButton(347+256+40,69+149*4,347+129+256-40,85+149*4,7,11,"DOWN","SCROLL DOWN",nop,nop,nop,nop);*/

	cINTERFACE * CreateMenu=TexGenMenu->AddButton(760-2,57,813-2,73,5,11,"CREATE","IMAGE GENERATOR FILTERS",GUI_ButtonSelect_TextureMenu,nop,nop,nop);
	cINTERFACE *DistortMenu=TexGenMenu->AddButton(814-2,57,870-2,73,5,11,"DISTORT","LAYER DISTORTION FILTERS",GUI_ButtonSelect_TextureMenu,nop,nop,nop);
	cINTERFACE *  LayerMenu=TexGenMenu->AddButton(871-2,57,916-2,73,5,11,"LAYER","LAYER MIXING FILTERS",GUI_ButtonSelect_TextureMenu,nop,nop,nop);
	cINTERFACE *  ColorMenu=TexGenMenu->AddButton(917-2,57,964-2,73,5,11,"COLOR","COLOR MANIPULATION FILTERS",GUI_ButtonSelect_TextureMenu,nop,nop,nop);
	cINTERFACE *  BlursMenu=TexGenMenu->AddButton(965-2,57,1007,73,5,11,"BLURS","BLUR FILTERS",GUI_ButtonSelect_TextureMenu,nop,nop,nop);
	cINTERFACE *   FileMenu=TexGenMenu->AddButton(863,74,894,90,5,11,"FILE","FILE MENU",GUI_ButtonSelect_TextureMenuEdit,nop,nop,nop);

	TexGenMenu->ItemList[0].ButtonPushed=true;
	TexGenMenu->SetActiveBranch(0);

	//////////////////////////////////////////////////////////////////////////
	// CREATE
	EnvmapMenu       =CreateMenu->AddButton(758+9+2-27,100,812+9+2-27,116,5,11,"ENVMAP","ENVIRONMENT MAP",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	PlasmaMenu       =CreateMenu->AddButton(813+9+2-27,100,868+9+2-27,116,5,11,"PLASMA","SINUS PLASMA",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	SubPlasmaMenu    =CreateMenu->AddButton(869+9+2-27,100,947+9+2-27,116,5,11,"SUBPLASMA","SUBPLASMA",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	TextMenu         =CreateMenu->AddButton(948+9+2-27,100,986+9+2-27,116,5,11,"TEXT","TEXT",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	MakeMaterialMenu =CreateMenu->AddButton(987+9+2-27,100,1013+9+2+20-27,116,5,11,"MATER","CREATE MATERIAL FROM SELECTED LAYER",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);

	FractalPlasmaMenu=CreateMenu->AddButton(736,117,780,133,9,11,"FRC1","FRACTALPLASMA (NEW STYLE)",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	FractalPlasmaOldMenu=CreateMenu->AddButton(781,117,825,133,9,11,"FRC2","FRACTALPLASMA (OLD STYLE)",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);

	CellsMenu        =CreateMenu->AddButton(865+5+2-52+6,117,908-52+5+2+6,133,5,11,"CELLS","CELLULAR TEXTURE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	RectangleMenu    =CreateMenu->AddButton(-1,-1,-1,-1,5,11,"RECT","RECTANGLE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	CircleMenu       =CreateMenu->AddButton(950+2+2-52+6,117,999-52+2+6,133,5,11,"CIRCLE","CIRCLE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	GradientMenu     =CreateMenu->AddButton(998+2+2-52+6,117,1013+2+6,133,5,11,"GRADIENT","GRADIENT",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	ChamferRectangleMenu=CreateMenu->AddButton(909+5+2-52+6,117,949-52+2+2+6,133,5,11,"RECT","CHAMFER RECTANGLE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	DotsMenu         =CreateMenu->AddButton(909+5+2-52+6-21-8,134,949-52+2+2+6-19-8,149,5,11,"DOTS","RANDOM DOTS",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	JpegMenu         =CreateMenu->AddButton(950-52+2+2+6-19-8,134,951-52+2+2+6+21-10,149,5,11,"JPEG","IMPORT JPG IMAGE",GUI_ButtonSelect_SetTextureCommandInterfaceJPG,nop,nop,nop);

	CreateMenu->ItemList[0].ButtonPushed=true;
	CreateMenu->SetActiveBranch(0);

	//////////////////////////////////////////////////////////////////////////
	// DISTORT
	MapMenu     =DistortMenu->AddButton(758+4+3,100,787+6+3,116,5,11,"MAP","MAP-DISTORTION",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	TwirlMenu   =DistortMenu->AddButton(788+6+3,100,832+4+3,116,5,11,"TWIRL","TWIRL",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	SineDistMenu=DistortMenu->AddButton(833+4+3,100,894+2+3,116,5,11,"SINEDIST","SINE-DISTORTION",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	GlassMenu   =DistortMenu->AddButton(895+2+3,100,942+1+3,116,5,11,"GLASS","GLASS",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	OffsetMenu  =DistortMenu->AddButton(942+2+3,100,997+3,116,5,11,"OFFSET","OFFSET",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	PixelizeMenu=DistortMenu->AddButton(758+4+17,117,819+17,133,5,11,"PIXELIZE","PIXELIZE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	CrystalizeMenu=DistortMenu->AddButton(820+17,117,897+17,133,5,11,"CRYSTALIZE","CRYSTALIZE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	RotozoomMenu=DistortMenu->AddButton(898+17,117,973+17,133,5,11,"ROTOZOOM","ROTOZOOM",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	DistortMenu->ItemList[0].ButtonPushed=true;
	DistortMenu->SetActiveBranch(0);

	//////////////////////////////////////////////////////////////////////////
	// COLOR
	ColorizeMenu  =ColorMenu->AddButton(758+5+9,100,821+6+9,116,5,11,"COLORIZE","COLORIZE",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	EmbossMenu    =ColorMenu->AddButton(822+6+9,100,879+6+9,116,5,11,"EMBOSS","EMBOSS",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	ShadeMenu     =ColorMenu->AddButton(880+6+9,100,929+4+9,116,5,11,"SHADE","SHADE (MAPPED BRIGHTNESS)",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	BrightnessMenu=ColorMenu->AddButton(929+5+9,100,979+5+9,116,5,11,"BRIGHT","BRIGHTNESS",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	InvertMenu    =ColorMenu->AddButton(758+5+11+9-25,117,808+3+11+9-25,133,5,11,"INVERT","INVERT",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	ScaleMenu     =ColorMenu->AddButton(809+3+11+9-25,117,850+6+11+9-25,133,5,11,"SCALE","SCALE COLORS",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	SineColorMenu =ColorMenu->AddButton(851+6+11+9-25,117,929+2+11+9-25,133,5,11,"SINECOLOR","SINE-COLOR",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	HSVMenu       =ColorMenu->AddButton(930+2+11+9-25,117,960+4+11+9-25,133,5,11,"HSV","HUE/SATURATION ADJUSTMENT",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	ContrastMenu  =ColorMenu->AddButton(961+4+11+9-25,117,1024+8-25,133,5,11,"CONTR","CONTRAST",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	ColorMenu->ItemList[0].ButtonPushed=true;
	ColorMenu->SetActiveBranch(0);

	//////////////////////////////////////////////////////////////////////////
	// LAYER
	CopyMenu      =LayerMenu->AddButton(758+5+41,100,798+5+41,116,5,11,"COPY","COPY",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	MixMenu       =LayerMenu->AddButton(799+5+41,100,828+3+41,116,5,11,"MIX","MIX",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	MixMapMenu    =LayerMenu->AddButton(829+3+41,100,879+3+41,116,5,11,"MIXMAP","MIXMAP",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	MaxMenu       =LayerMenu->AddButton(880+3+41,100,910+5+41,116,5,11,"MAX","MAX",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	LayerMenu->ItemList[0].ButtonPushed=true;
	LayerMenu->SetActiveBranch(0);

	//////////////////////////////////////////////////////////////////////////
	// BLUR
	BlurMenu   =BlursMenu->AddButton(758+4+70,100,793+6+70,116,5,11,"BLUR","GAUSSIAN BLUR",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	DirBlurMenu=BlursMenu->AddButton(794+6+70,100,850+6+70,116,5,11,"DIRBLUR","DIRECTIONAL BLUR",GUI_ButtonSelect_SetTextureCommandInterface,nop,nop,nop);
	BlursMenu->ItemList[0].ButtonPushed=true;
	BlursMenu->SetActiveBranch(0);

	TextureSelect=FileMenu->AddButton(758+4+15,100,817+15,116,6,11,"SELECT","SELECT TEXTURE, CREATE NEW TEXTURE, CLEAR TEXTURE, DELETE TEXTURE",GUI_ButtonSelect,nop,nop,nop);
	cINTERFACE *EditMenuMain=FileMenu->AddButton(818+15,100,853+15,116,6,11,"EDIT","EDIT CURRENT TEXTURE",GUI_ButtonSelect_TextureMenuEdit,nop,nop,nop);
	CommandMenu=EditMenuMain->AddItemList(764,130,222,10,NULL,"SELECT COMMAND TO EDIT");
	CommandMenuData=&(CommandMenu->ParentItem->data);
	cINTERFACE *ImportTex=FileMenu->AddButton(854+15,100,906+15,116,6,11,"IMPORT","IMPORT TEXTURE",GUI_GetTextureFileList,nop,nop,nop);
	FileMenu->AddButton(907+15,100,963+15,116,6,11,"EXPORT","EXPORT TEXTURE",GUI_ReadLine,nop,nop,ExportTexture);
	TextureSelect->AddItemList(758+6,130+80,222,30,TextureListScroller,"SELECT TEXTURE:");
	TextureSelect->AddButton(764+100,480-350,801+100,496-350,6,11,"NEW","ADD NEW TEXTURE TO LIST",GUI_ReadLine,nop,nop,TexGen_NewTextureCheckCallBack);
	TextureSizeSelector=TextureSelect->AddSubInterface();
	EditMenu=EditMenuMain->AddSubInterface();

	GENGUI(Envmap); //0 //GARGAJ SORRENDBE RAKD MAJD AZONOSITO SZERINT AZ EFFEKTEKET MERT
	GENGUI(Plasma); //1 //A GUIBAN IS SORBAN SZAMOZODNAK ES A SZERKESZTO GUI A TEXTURAFILTER SZAMAVAL
	GENGUI(Map);    //2 //MEGEGYEZO GUI CSATORNAT ENGEDELYEZI, IGY LATSZANAK AZ EPP AKTUALISAN
	GENGUI(Blur);   //3 //KIJELOLT FILTERHEZ TARTOZO PARAMETEREK
	GENGUI(DirBlur);//4
	GENGUI(Text);   //5
	GENGUI(SubPlasma);//6
	GENGUI(FractalPlasma);//7
	GENGUI(Colorize);//8
	GENGUI(Shade);//9
	GENGUI(Brightness);//10
	GENGUI(Copy);//11
	GENGUI(Cells);//12
	GENGUI(Twirl);//13
	GENGUI(SineDist);//14
	GENGUI(Mix);//15
	GENGUI(MixMap);//16
	GENGUI(Scale);//17
	GENGUI(SineColor);//18
	GENGUI(Max);//19
	GENGUI(HSV);//20
	GENGUI(Emboss);//21
	GENGUI(Invert);//22
	GENGUI(Glass);//23
	GENGUI(Pixelize);//24
	GENGUI(Offset);//25
	GENGUI(Crystalize);//26
	GENGUI(Rectangle);//27
	GENGUI(Circle);//28
	GENGUI(Contrast);//29
	GENGUI(MakeMaterial);//30
	GENGUI(Gradient);//31
	GENGUI(Rotozoom);//32
	GENGUI(ChamferRectangle);//33
	GENGUI(Dots);//34
	GENGUI(Jpeg);//35
	GENGUI(FractalPlasmaOld);//36

	EditMenu->SetActiveBranch(-1);

	ActualTextureCommandInterface=EnvmapMenu;//->ItemList[0].Interface;;

	TextureSizeSelector->AddButton(764+29,500-350,787+29,516-350,6,11,"32","SET NEW TEXTURE SIZE TO 32x32",GUI_ButtonSelect,nop,nop,nop);
	TextureSizeSelector->AddButton(788+29,500-350,811+29,516-350,6,11,"64","SET NEW TEXTURE SIZE TO 64x64",GUI_ButtonSelect,nop,nop,nop);
	TextureSizeSelector->AddButton(812+29,500-350,841+29,516-350,6,11,"128","SET NEW TEXTURE SIZE TO 128x128",GUI_ButtonSelect,nop,nop,nop);
	TextureSizeSelector->AddButton(842+29,500-350,871+29,516-350,6,11,"256","SET NEW TEXTURE SIZE TO 256x256",GUI_ButtonSelect,nop,nop,nop);
	TextureSizeSelector->AddButton(872+29,500-350,901+29,516-350,6,11,"512","SET NEW TEXTURE SIZE TO 512x512",GUI_ButtonSelect,nop,nop,nop);
	TextureSizeSelector->AddButton(902+29,500-350,936+29,516-350,6,11,"1024","SET NEW TEXTURE SIZE TO 1024x1024",GUI_ButtonSelect,nop,nop,nop);
	//TextureSizeSelector->AddButton(937+12,500,971+12,516,6,11,"2048","SET NEW TEXTURE SIZE TO 2048x2048",GUI_ButtonSelect,nop,nop,nop);
	TextureSizeSelector->ItemList[3].ButtonPushed=true;
	
	TextureSelect->AddButton(802+29,530-350,855+29,546-350,6,11,"DELETE","REMOVE TEXTURE FROM LIST",nop,nop,nop,nop);
	TextureSelect->AddButton(856+29,530-350,903+29,546-350,6,11,"CLEAR","CLEAR ALL COMMANDS FROM TEXTURE",TexGen_Clear,nop,nop,nop);
	ImportTex->AddItemList(758+6,130,222,30,TextureFileList,"SELECT FILE:");
	ImportTex->AddSkinnedButton(834,470,ImportButtonBig,"IMPORT SELECTED TEXTURE",ImportTexture,nop,nop,nop);
	ImportTex->ItemList[ImportTex->ItemNum-1].data=&ImportTex->ItemList[ImportTex->ItemNum-5].SelectedItem;
	GUI_GetTextureFileList(ImportTex,0);

	FileMenu->ItemList[0].ButtonPushed=true;
	FileMenu->SetActiveBranch(0);

	StatusBar=TexGenGUI->AddSubInterface();
	//StatusBar->AddByteProgressBar(50,700,514,GUI_NONE,&UndoStatus,0,255,0,"GENERATING... ","",nop,nop,nop,nop);
	StatusBar->AddByteProgressBar(407,8,440,GUI_NONE,&UndoStatus,0,255,0,"GENERATING... ","",nop,nop,nop,nop);
	StatusBar->ParentItem->ActiveBranch=false;
}