#include "FileIO.h"
#include "IntroEditorEffects.h"
#include <stdio.h>
#include "ChunkHeaders.h"
#include "GuiInterface.h"
#include "AseParser.h"

ITEMLIST *FileList=NULL;
cINTERFACE *Import;
cINTERFACE *StatusBar2;
float UndoStatus2;

char *LastFileName;
int texcnt=0;
int texops[100];
int GuiDrawTimer;

void GUIReDraw()
{
	int tim=timeGetTime();
	if (tim-GuiDrawTimer>100)
	{
		if (Done) exit(0);
		GuiDrawTimer=tim;
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
}

char *ReadASCIIZ(FILE *f)
{
	char *String=new char[256];

	memset(String,0,256);
	char c;
	c=1;
	for (int y=0; c!=0; y++)
	{
		fread(&c,1,1,f);
		String[y]=c;
	}

	return String;
}

void __stdcall GUI_GetFileList(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<Interface->ItemNum; x++) Interface->ItemList[x].ButtonPushed=false;
	Interface->ItemList[ButtonId].ButtonPushed=true;
	Interface->SetActiveBranch(ButtonId);
	if (Interface->ItemList[ButtonId].ToolTip) LastClickedToolTip=Interface->ItemList[ButtonId].ToolTip;
	//TextureFileList=new ITEMLIST();
	FileList->Last=NULL;
	FileList->List=NULL;
	FileList->ItemNum=0;
	// kill old list...
	SetupFileList(FileList,ProjectMsk);
	Interface->ItemList[ButtonId].ItemNum=FileList->ItemNum;
}

void SaveTextureChunk(FILE *f, TEXTURE *t)
{
	int size=8+strlen(t->Name)+1+4;
	int x;
	for (x=0; x<t->CommandNumber; x++)
	{
		size+=7;
		switch (t->Commands[x].Filter)
		{
		case TEXGEN_Envmap:								size+=sizeof(ENVMAPTEMPLATE);			break;					
		case TEXGEN_Plasma:								size+=sizeof(PLASMATEMPLATE);			break;					
		case TEXGEN_Map:								size+=sizeof(MAPTEMPLATE);				break;					
		case TEXGEN_Blur:								size+=sizeof(BLURTEMPLATE);				break;					
		case TEXGEN_DirBlur:							size+=sizeof(DIRBLURTEMPLATE);			break;					
		case TEXGEN_Text:								size+=sizeof(TEXTTEMPLATE)-4;			size+=strlen(((TEXTTEMPLATE*)t->Commands[x].data)->Text)+1;			break;
		case TEXGEN_SubPlasma:							size+=sizeof(SUBPLASMATEMPLATE);		break;							
		case TEXGEN_FractalPlasma:						size+=sizeof(FRACTALPLASMATEMPLATE);	break;					
		case TEXGEN_FractalPlasmaOld:					size+=sizeof(FRACTALPLASMAOLDTEMPLATE);	break;					
		case TEXGEN_Colorize:							size+=sizeof(COLORIZETEMPLATE);			break;					
		case TEXGEN_Shade:								size+=sizeof(SHADETEMPLATE);			break;					
		case TEXGEN_Brightness:							size+=sizeof(BRIGHTNESSTEMPLATE);		break;					
		case TEXGEN_Copy:								size+=sizeof(COPYTEMPLATE);				break;					
		case TEXGEN_Cells:								size+=sizeof(CELLSTEMPLATE);			break;					
		case TEXGEN_Twirl:								size+=sizeof(TWIRLTEMPLATE);			break;					
		case TEXGEN_SineDist:							size+=sizeof(SINEDISTTEMPLATE);			break;					
		case TEXGEN_Mix:								size+=sizeof(MIXTEMPLATE);				break;					
		case TEXGEN_MixMap:								size+=sizeof(MIXMAPTEMPLATE);			break;					
		case TEXGEN_Scale:								size+=sizeof(SCALETEMPLATE);			break;					
		case TEXGEN_SineColor:							size+=sizeof(SINECOLORTEMPLATE);		break;					
		case TEXGEN_Max:								size+=sizeof(MAXTEMPLATE);				break;					
		case TEXGEN_HSV:								size+=sizeof(HSVTEMPLATE);				break;					
		case TEXGEN_Glass:								size+=sizeof(GLASSTEMPLATE);			break;					
		case TEXGEN_Pixelize:							size+=sizeof(PIXELIZETEMPLATE);			break;					
		case TEXGEN_Offset:								size+=sizeof(OFFSETTEMPLATE);			break;					
		case TEXGEN_Crystalize:							size+=sizeof(CRYSTALIZETEMPLATE);		break;					
		case TEXGEN_Rectangle:							size+=sizeof(RECTANGLETEMPLATE);		break;					
		case TEXGEN_Circle:								size+=sizeof(CIRCLETEMPLATE);			break;					
		case TEXGEN_Contrast:							size+=sizeof(CONTRASTTEMPLATE);			break;					
		case TEXGEN_MakeMaterial:						size+=sizeof(MAKEMATERIALTEMPLATE)-4;	size+=strlen(((MAKEMATERIALTEMPLATE*)t->Commands[x].data)->Name)+1;			break;
		case TEXGEN_Gradient:							size+=sizeof(GRADIENTTEMPLATE);			break;					
		case TEXGEN_Rotozoom:							size+=sizeof(ROTOZOOMTEMPLATE);			break;					
		case TEXGEN_ChamferRectangle:					size+=sizeof(CHAMFERRECTANGLETEMPLATE);	break;					
		case TEXGEN_Dots:								size+=sizeof(DOTSTEMPLATE);	break;					
		case TEXGEN_Jpeg:								size+=4+((JPEGDATA*)t->Commands[x].data)->DataSize;	break;
		}
	}
	int i=TEX1;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(t->Name,1,strlen(t->Name),f);
	fprintf(f,"%c",0);
	
	fwrite(&t->Id,4,1,f);

	fwrite(&t->OXRes,4,1,f);
	fwrite(&t->CommandNumber,4,1,f);

	for (x=0; x<t->CommandNumber; x++)
	{
		fwrite(&t->Commands[x].Filter,1,1,f); // 4 bit
		fwrite(&t->Commands[x].Layer,1,1,f); // 4 bit
		fwrite(&t->Commands[x].Operator,1,1,f); //2 bit
		fwrite(&t->Commands[x].OperatorMask,4,1,f); //4 bit
		switch (t->Commands[x].Filter)
		{
		case TEXGEN_Envmap:					
			fwrite(t->Commands[x].data,1,sizeof(ENVMAPTEMPLATE),f);
			break;					
		case TEXGEN_Plasma:					
			fwrite(t->Commands[x].data,1,sizeof(PLASMATEMPLATE),f);
			break;					
		case TEXGEN_Map:					
			fwrite(t->Commands[x].data,1,sizeof(MAPTEMPLATE),f);
			break;					
		case TEXGEN_Blur:					
			fwrite(t->Commands[x].data,1,sizeof(BLURTEMPLATE),f);
			break;					
		case TEXGEN_DirBlur:				
			fwrite(t->Commands[x].data,1,sizeof(DIRBLURTEMPLATE),f);
			break;					
		case TEXGEN_Text:					
			fwrite(t->Commands[x].data,1,sizeof(TEXTTEMPLATE)-4,f);
			fwrite(((TEXTTEMPLATE*)t->Commands[x].data)->Text,1,strlen(((TEXTTEMPLATE*)t->Commands[x].data)->Text),f);
			fprintf(f,"%c",0);
			break;					
		case TEXGEN_SubPlasma:				
			fwrite(t->Commands[x].data,1,sizeof(SUBPLASMATEMPLATE),f);
			break;					
		case TEXGEN_FractalPlasma:			
			fwrite(t->Commands[x].data,1,sizeof(FRACTALPLASMATEMPLATE),f);
			break;					
		case TEXGEN_FractalPlasmaOld:			
			fwrite(t->Commands[x].data,1,sizeof(FRACTALPLASMAOLDTEMPLATE),f);
			break;					
		case TEXGEN_Colorize:				
			fwrite(t->Commands[x].data,1,sizeof(COLORIZETEMPLATE),f);
			break;					
		case TEXGEN_Shade:					
			fwrite(t->Commands[x].data,1,sizeof(SHADETEMPLATE),f);
			break;					
		case TEXGEN_Brightness:				
			fwrite(t->Commands[x].data,1,sizeof(BRIGHTNESSTEMPLATE),f);
			break;					
		case TEXGEN_Copy:					
			fwrite(t->Commands[x].data,1,sizeof(COPYTEMPLATE),f);
			break;					
		case TEXGEN_Cells:					
			fwrite(t->Commands[x].data,1,sizeof(CELLSTEMPLATE),f);
			break;					
		case TEXGEN_Twirl:					
			fwrite(t->Commands[x].data,1,sizeof(TWIRLTEMPLATE),f);
			break;					
		case TEXGEN_SineDist:				
			fwrite(t->Commands[x].data,1,sizeof(SINEDISTTEMPLATE),f);
			break;					
		case TEXGEN_Mix:					
			fwrite(t->Commands[x].data,1,sizeof(MIXTEMPLATE),f);
			break;					
		case TEXGEN_MixMap:					
			fwrite(t->Commands[x].data,1,sizeof(MIXMAPTEMPLATE),f);
			break;					
		case TEXGEN_Scale:					
			fwrite(t->Commands[x].data,1,sizeof(SCALETEMPLATE),f);
			break;					
		case TEXGEN_SineColor:				
			fwrite(t->Commands[x].data,1,sizeof(SINECOLORTEMPLATE),f);
			break;					
		case TEXGEN_Max:					
			fwrite(t->Commands[x].data,1,sizeof(MAXTEMPLATE),f);
			break;					
		case TEXGEN_HSV:					
			fwrite(t->Commands[x].data,1,sizeof(HSVTEMPLATE),f);
			break;					
		case TEXGEN_Glass:				
			fwrite(t->Commands[x].data,1,sizeof(GLASSTEMPLATE),f);
			break;					
		case TEXGEN_Pixelize:					
			fwrite(t->Commands[x].data,1,sizeof(PIXELIZETEMPLATE),f);
			break;					
		case TEXGEN_Offset:					
			fwrite(t->Commands[x].data,1,sizeof(OFFSETTEMPLATE),f);
			break;					
		case TEXGEN_Crystalize:					
			fwrite(t->Commands[x].data,1,sizeof(CRYSTALIZETEMPLATE),f);
			break;					
		case TEXGEN_Rectangle:					
			fwrite(t->Commands[x].data,1,sizeof(RECTANGLETEMPLATE),f);
			break;					
		case TEXGEN_Circle:					
			fwrite(t->Commands[x].data,1,sizeof(CIRCLETEMPLATE),f);
			break;					
		case TEXGEN_Contrast:					
			fwrite(t->Commands[x].data,1,sizeof(CONTRASTTEMPLATE),f);
			break;					
		case TEXGEN_MakeMaterial:					
			fwrite(t->Commands[x].data,1,sizeof(MAKEMATERIALTEMPLATE)-4,f);
			fwrite(((MAKEMATERIALTEMPLATE*)t->Commands[x].data)->Name,1,strlen(((MAKEMATERIALTEMPLATE*)t->Commands[x].data)->Name),f);
			fprintf(f,"%c",0);
			break;					
		case TEXGEN_Gradient:					
			fwrite(t->Commands[x].data,1,sizeof(GRADIENTTEMPLATE),f);
			break;					
		case TEXGEN_Rotozoom:					
			fwrite(t->Commands[x].data,1,sizeof(ROTOZOOMTEMPLATE),f);
			break;					
		case TEXGEN_ChamferRectangle:					
			fwrite(t->Commands[x].data,1,sizeof(CHAMFERRECTANGLETEMPLATE),f);
			break;					
		case TEXGEN_Dots:					
			fwrite(t->Commands[x].data,1,sizeof(DOTSTEMPLATE),f);
			break;					
		case TEXGEN_Jpeg:					{
			JPEGTEMPLATE *a=(JPEGTEMPLATE*)t->Commands[x].data;
			fwrite(&a->JPEGDataSize,1,4,f);
			fwrite(a->JPEGData,1,a->JPEGDataSize,f);
			break;                  }
		}
	}

}

void SavePolySelectChunk(FILE *f, ITEM *it)
{
	int size=strlen(it->Name)+1+4;
	int i=PLS0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(it->Name,1,strlen(it->Name),f);
	fprintf(f,"%c",0);
	fwrite(&it->ID,1,4,f);
}

void SaveVertexSelectChunk(FILE *f, ITEM *it)
{
	int size=strlen(it->Name)+1+4;
	int i=VXS0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(it->Name,1,strlen(it->Name),f);
	fprintf(f,"%c",0);
	fwrite(&it->ID,1,4,f);
}

void SaveBrushChunk(FILE *f, ITEM *it)
{
	int size=strlen(it->Name)+1+sizeof(PVSelect);
	int i=BRS0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(it->Name,1,strlen(it->Name),f);
	fprintf(f,"%c",0);
	fwrite(it->Data,sizeof(PVSelect),1,f);
}

void SaveSceneChunk(FILE *f, SCENE *s)
{
	int size=strlen(s->Name)+1+sizeof(CAMERA)-16+sizeof(RGBA)+8*sizeof(LIGHT)+4+1;
	int i=SCN1;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(s->Name,1,strlen(s->Name),f);
	fprintf(f,"%c",0);
	fwrite(&s->ID,1,4,f);
	fwrite(&s->Camera,1,sizeof(CAMERA)-16,f);
	fwrite(&s->Fog,1,sizeof(RGBA),f);
	fwrite(s->Lights,8,sizeof(LIGHT),f);
	fwrite(&s->ColorDiscard,1,1,f);
}

void SaveObjectChunk(FILE *f, OBJECT *o)
{
	int size=strlen(o->ListItem->Name)+1+1+4+1+sizeof(MATRIX)+1+1+1+6+2*sizeof(float)+2*sizeof(GLuint)+sizeof(RGBA)+8+8+5*4+2+4+2;
	if (o->Primitive==aDDict_STORED) size+=1+8+o->VertexNum*(sizeof(VECTOR3)*2+sizeof(VECTOR2))+o->PolygonNum*(12+sizeof(VECTOR3)+sizeof(VECTOR2)*3);
	if (o->Primitive==aDDict_SUPERSHAPE) size+=sizeof(SUPERSHAPE);
	if (o->Primitive==aDDict_STORED && o->PerVertexColor) size+=o->VertexNum*4;
	int i=OBJ5;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(o->ListItem->Name,1,strlen(o->ListItem->Name),f);
	fprintf(f,"%c",0);
	fwrite(&o->ListItem->Selected,1,1,f);
	fwrite(&o->ID,4,1,f);
	
	fwrite(&o->Param1,4,1,f);
	fwrite(&o->Param2,4,1,f);
	fwrite(&o->Param3,4,1,f);
	fwrite(&o->Param4,4,1,f);
	fwrite(&o->Param5,4,1,f);

	fwrite(&o->Primitive,1,1,f);

	if (o->Primitive==aDDict_STORED)
	{
		fwrite(&o->PerVertexColor,1,1,f);
		fwrite(&o->VertexNum,4,1,f);
		int x;
		for (x=0; x<o->VertexNum; x++)
		{
			fwrite(&(o->VertexList[x].Position),sizeof(VECTOR3),1,f);
			fwrite(&(o->VertexList[x].CurrentNormal),sizeof(VECTOR3),1,f);
			fwrite(&(o->VertexList[x].TextureCoordinate),sizeof(VECTOR2),1,f);
			if (o->PerVertexColor)
			fwrite(&o->VertexList[x].Color,4,1,f);
		}
		fwrite(&o->PolygonNum,4,1,f);
		for (x=0; x<o->PolygonNum; x++)
		{
			fwrite(&o->PolygonList[x].v1,4,1,f);
			fwrite(&o->PolygonList[x].v2,4,1,f);
			fwrite(&o->PolygonList[x].v3,4,1,f);
			fwrite(&o->PolygonList[x].CurrentNormal,sizeof(VECTOR3),1,f);
			fwrite(&o->PolygonList[x].t[0],sizeof(VECTOR2),1,f);
			fwrite(&o->PolygonList[x].t[1],sizeof(VECTOR2),1,f);
			fwrite(&o->PolygonList[x].t[2],sizeof(VECTOR2),1,f);
		}
	}

	fwrite(o->ModelView,1,sizeof(MATRIX),f);
	fwrite(&o->Wireframe,1,1,f);
	fwrite(&o->Textured,1,1,f);
	fwrite(&o->EnvMapped,1,1,f);
	fwrite(&o->NormalsInverted,1,1,f);
	fwrite(&o->Xtile,1,1,f);
	fwrite(&o->Ytile,1,1,f);
	fwrite(&o->XSwap,1,1,f);
	fwrite(&o->YSwap,1,1,f);
	fwrite(&o->Swap,1,1,f);
	fwrite(&o->OffsetX,4,1,f);
	fwrite(&o->OffsetY,4,1,f);
	fwrite(&o->SRCBlend,sizeof(GLuint),1,f);
	fwrite(&o->DSTBlend,sizeof(GLuint),1,f);
	fwrite(&o->Color,1,sizeof(RGBA),f);
	if (o->Textured)
	{
		fwrite(&o->Material->TextureID,4,1,f);
		fwrite(&o->Material->SlotNumber,4,1,f);
	}
	else
	{
		int i=-1;
		fwrite(&i,4,1,f);
		fwrite(&i,4,1,f);
	}
	if (o->EnvMapped)
	{
		fwrite(&o->EnvMap->TextureID,4,1,f);
		fwrite(&o->EnvMap->SlotNumber,4,1,f);
	}
	else
	{
		int i=-1;
		fwrite(&i,4,1,f);
		fwrite(&i,4,1,f);
	}
	fwrite(&o->ZMask,1,1,f);
	fwrite(&o->AEpsilon,1,1,f);
	fwrite(&o->Shading,4,1,f);
	fwrite(&o->Backface,1,1,f);
	fwrite(&o->Backfront,1,1,f);

	if (o->Primitive==aDDict_SUPERSHAPE) fwrite((SUPERSHAPE*)o->Param1,sizeof(SUPERSHAPE),1,f);
}

void SaveMapxform(FILE *f, OBJECT *o)
{
	int size=strlen(o->ListItem->Name)+1+1+4+5*4+1+sizeof(MATRIX)+5+4*2+4*2+4+1+1;
	int i=MAP0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(o->ListItem->Name,1,strlen(o->ListItem->Name),f);
	fprintf(f,"%c",0);
	fwrite(&o->ListItem->Selected,1,1,f);
	fwrite(&o->ID,4,1,f);
	
	fwrite(&o->Param1,4,1,f);
	fwrite(&o->Param2,4,1,f);
	fwrite(&o->Param3,4,1,f);
	fwrite(&o->Param4,4,1,f);
	fwrite(&o->Param5,4,1,f);

	fwrite(&o->Primitive,1,1,f);
	fwrite(o->ModelView,1,sizeof(MATRIX),f);
	fwrite(&o->Xtile,1,1,f);
	fwrite(&o->Ytile,1,1,f);
	fwrite(&o->XSwap,1,1,f);
	fwrite(&o->YSwap,1,1,f);
	fwrite(&o->Swap,1,1,f);
	fwrite(&o->OffsetX,4,1,f);
	fwrite(&o->OffsetY,4,1,f);
	if (o->Material)
	{
		fwrite(&o->Material->TextureID,4,1,f);
		fwrite(&o->Material->SlotNumber,4,1,f);
	}
	else
	{
		int i=-1;
		fwrite(&i,4,1,f);
		fwrite(&i,4,1,f);
	}
	fwrite(&o->TargetObjectID,4,1,f);
	fwrite(&o->MapXformType,1,1,f);
	fwrite(&o->MapXformColor,1,1,f);
}

void SaveMeshSmooth(FILE *f, OBJECT *o)
{
	int size=strlen(o->ListItem->Name)+1+1+4+1+4+4+1;
	int i=MSH1;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(o->ListItem->Name,1,strlen(o->ListItem->Name),f);
	fprintf(f,"%c",0);
	fwrite(&o->Primitive,1,1,f);
	fwrite(&o->ListItem->Selected,1,1,f);
	fwrite(&o->ID,4,1,f);
	bool Linear=o->Primitive==aDDict_LINEARSMOOTH;
	fwrite(&Linear,1,1,f);
	fwrite(&o->Param1,4,1,f);
	fwrite(&o->Param2,4,1,f);
}

void SaveClone(FILE *f, OBJECT *o)
{
	int cnt=0;
	int x;
	for (x=0; x<o->Param1; x++)
	{
		if (!((OBJECT*)(((int*)o->Param3)[x]))->Deleted) cnt++;
	}

	int size=strlen(o->ListItem->Name)+1+1+4+4+cnt*4+1+sizeof(MATRIX);
	int i=CLN0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(o->ListItem->Name,1,strlen(o->ListItem->Name),f);
	fprintf(f,"%c",0);
	fwrite(&o->ListItem->Selected,1,1,f);
	fwrite(&o->ID,4,1,f);
	
	fwrite(&cnt,4,1,f);
	for (x=0; x<o->Param1; x++)
	{
		if (!((OBJECT*)(((int*)o->Param3)[x]))->Deleted) 
		{
			fwrite(&((int*)o->Param2)[x],4,1,f);
		}
	}

	fwrite(&o->Primitive,1,1,f);
	fwrite(o->ModelView,1,sizeof(MATRIX),f);
}

void SaveWorldChunk(FILE *f, WORLD *w)
{
	int size=strlen(w->Name)+1+sizeof(CAMERA)-16+8*sizeof(LIGHT)+3+5+1+2*4;
	int i=WRL2;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(w->Name,strlen(w->Name),1,f);
	fprintf(f,"%c",0);
	fwrite(&w->ID,1,4,f);
	fwrite(&w->Camera,1,sizeof(CAMERA)-16,f);
	fwrite(w->Lights,8,sizeof(LIGHT),f);
	fwrite(&w->Fog,1,1,f);
	fwrite(w->FogCol,3,1,f);
	fwrite(&w->FogStart,4,1,f);
	fwrite(&w->FogEnd,4,1,f);
	fwrite(&w->Lighting,1,1,f);
}

void SaveWorldObjectChunk(FILE *f, WORLDOBJECT *w)
{
	int size=strlen(w->ListItem->Name)+1+4+4+4+sizeof(RST)+sizeof(GLuint)*2+1+8+2+8+8+3+4+5*4*2+12+4+12+4+2+4+4+4+12+1+2;
	int i=WRO5;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(w->ListItem->Name,strlen(w->ListItem->Name),1,f);
	fprintf(f,"%c",0);
	fwrite(&w->ID,1,4,f);
	fwrite(&w->ParentID,1,4,f);
	fwrite(&w->Primitive,1,4,f);
	fwrite(&w->PosData,1,sizeof(RST),f);
	fwrite(&w->SRCBlend,1,sizeof(GLuint),f);
	fwrite(&w->DSTBlend,1,sizeof(GLuint),f);
	if (w->Data)
	{
		if (w->Primitive==aDDict_SCENE)
		fwrite(&((SCENE*)w->Data)->ID,1,4,f);
		else
		if (w->Primitive==aDDict_SUBSCENE)
		fwrite(&((WORLD*)w->Data)->ID,1,4,f);
	}
	else
	{
		int a=-1;
		fwrite(&a,1,4,f);
	}
	fwrite(&w->Textured,1,1,f);
	if (w->Material)
	{
		fwrite(&w->Material->TextureID,4,1,f);
		fwrite(&w->Material->SlotNumber,4,1,f);
	}
	else
	{
		int i=-1;
		fwrite(&i,4,1,f);
		fwrite(&i,4,1,f);
	}
	if (w->EmitterData)
	{
		fwrite(&w->EmitterData->Head,1,1,f);
		fwrite(&w->EmitterData->Tail,1,1,f);

		fwrite(&w->EmitterData->DefaultAge,4,1,f);
		fwrite(&w->EmitterData->AgeChaos,4,1,f);
		fwrite(&w->EmitterData->ParticlesPerFrame,4,1,f);
		fwrite(&w->EmitterData->MaxParticles,4,1,f);

		fwrite(&w->EmitterData->ObjectHead,1,1,f);
		fwrite(&w->EmitterData->TailLength,1,1,f);
		fwrite(&w->EmitterData->TailLength2,1,1,f);

		fwrite(&w->EmitterData->ObjectHeadSceneID,4,1,f);

		fwrite(w->EmitterData->Color1,4,5,f);
		fwrite(w->EmitterData->Color2,4,5,f);

		fwrite(&w->EmitterData->Param1,4,1,f);
		fwrite(&w->EmitterData->Param2,4,1,f);
		fwrite(&w->EmitterData->Size,4,1,f);

		fwrite(&w->EmitterData->Rotspeed,4,1,f);
		fwrite(&w->EmitterData->RotChaos,4,1,f);
		fwrite(&w->EmitterData->CamFade,4,1,f);
	}
	else
	{
		char ures[2+8+8+3+4+5*4*2+12+12];
		memset(ures,0,2+8+8+3+4+5*4*2+12+12);
		fwrite(ures,1,2+8+8+3+4+5*4*2+12+12,f);
	}
	fwrite(&w->Color,4,1,f);
	fwrite(&w->ZMask,1,1,f);
	fwrite(&w->AEpsilon,1,1,f);

	if (w->EmitterData)
	{
		fwrite(&w->EmitterData->Speed,4,1,f);
		fwrite(&w->EmitterData->SpeedChaos,4,1,f);
		fwrite(&w->EmitterData->DirChaos,4,1,f);
		fwrite(w->EmitterData->Dir.a,12,1,f);
	}
	else
	{
		char ures[24];
		memset(ures,0,24);
		fwrite(ures,1,24,f);
	}

	fwrite(&w->ListItem->Selected,1,1,f);

	if (w->EmitterData)
	{
		fwrite(&w->EmitterData->RandRotate,1,1,f);
		fwrite(&w->EmitterData->FixedUp,1,1,f);
	}
	else
	{
		char ures[24];
		memset(ures,0,24);
		fwrite(ures,1,2,f);
	}
}

void SaveSplineData(FILE *f, SPLINE *s)
{
	fwrite(&s->KeyNum,1,4,f);
	fwrite(&s->Interpolation,1,4,f);
	for (int x=0; x<s->KeyNum; x++)
	{
		fwrite(&s->Keys[x].Time,1,4,f);
		fwrite(&s->Keys[x].Value,1,4,f);
	}
}

void SaveWorldAnimChunk(FILE *f, ANIMDATA *w)
{
	int size=strlen(w->Name)+1+8*7*8+4;
	int x;
	for (x=0; x<8; x++)
	{
		size+=w->Lights[x].ARed->KeyNum*8;
		size+=w->Lights[x].AGreen->KeyNum*8;
		size+=w->Lights[x].ABlue->KeyNum*8;
		size+=w->Lights[x].Red->KeyNum*8;
		size+=w->Lights[x].Green->KeyNum*8;
		size+=w->Lights[x].Blue->KeyNum*8;
		size+=w->Lights[x].Lit->KeyNum*8;
	}
	int i=WRA0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(&w->AnimID,1,4,f);
	fwrite(w->Name,strlen(w->Name),1,f);
	fprintf(f,"%c",0);
	for (x=0; x<8; x++)
	{
		SaveSplineData(f,w->Lights[x].ARed);
		SaveSplineData(f,w->Lights[x].AGreen);
		SaveSplineData(f,w->Lights[x].ABlue);
		SaveSplineData(f,w->Lights[x].Red);
		SaveSplineData(f,w->Lights[x].Green);
		SaveSplineData(f,w->Lights[x].Blue);
		SaveSplineData(f,w->Lights[x].Lit);
	}
}

void SaveWorldCamAnimChunk(FILE *f, CAMSPLINES *w)
{
	int size=strlen(w->Name)+1+4+8*8+8*(w->Eyex->KeyNum+
		                                w->Eyey->KeyNum+
										w->Eyez->KeyNum+
										w->Trgx->KeyNum+
										w->Trgy->KeyNum+
										w->Trgz->KeyNum+
										w->Fov->KeyNum+
										w->Roll->KeyNum);
	int i=CAM0;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	fwrite(w->Name,strlen(w->Name),1,f);
	fprintf(f,"%c",0);
	fwrite(&w->CamID,1,4,f);

	SaveSplineData(f,w->Eyex);
	SaveSplineData(f,w->Eyey);
	SaveSplineData(f,w->Eyez);
	SaveSplineData(f,w->Trgx);
	SaveSplineData(f,w->Trgy);
	SaveSplineData(f,w->Trgz);
	SaveSplineData(f,w->Fov);
	SaveSplineData(f,w->Roll);
}

void SaveWorldObjAnimChunk(FILE *f, DEFAULTOBJECTSPLINES *w)
{
	int size=  18*8+8*(w->Red->KeyNum+
		                w->Green->KeyNum+
						w->Blue->KeyNum+
		                w->Alpha->KeyNum+
		                w->Size->KeyNum+
		                w->Prtfrme->KeyNum+
		                w->AnimTime->KeyNum+
						w->AnimID->KeyNum+
		                w->Posx->KeyNum+
		                w->Posy->KeyNum+
		                w->Posz->KeyNum+
		                w->Rotx->KeyNum+
		                w->Roty->KeyNum+
		                w->Rotz->KeyNum+
		                w->Rotw->KeyNum+
		                w->Sclx->KeyNum+
		                w->Scly->KeyNum+
		                w->Sclz->KeyNum);
	int i=WOA2;
	fwrite(&i,1,4,f);
	fwrite(&size,1,4,f);
	SaveSplineData(f,w->Red);
	SaveSplineData(f,w->Green);
	SaveSplineData(f,w->Blue);
	SaveSplineData(f,w->Alpha);
	SaveSplineData(f,w->Size);
	SaveSplineData(f,w->Prtfrme);
	SaveSplineData(f,w->AnimTime);
	SaveSplineData(f,w->AnimID);
	SaveSplineData(f,w->Posx);
	SaveSplineData(f,w->Posy);
	SaveSplineData(f,w->Posz);
	SaveSplineData(f,w->Rotx);
	SaveSplineData(f,w->Roty);
	SaveSplineData(f,w->Rotz);
	SaveSplineData(f,w->Rotw);
	SaveSplineData(f,w->Sclx);
	SaveSplineData(f,w->Scly);
	SaveSplineData(f,w->Sclz);
}

void SaveEventChunk(FILE *f, EVENT *w)
{
	bool Save=true;

	switch (w->Effect->Type)
	{
	case 2:
		if (!((PARTICLETEMPLATE*)w->EffectData)->World) Save=false;
		break;
	case 7:
		if (!((DISPLAY3DTEMPLATE*)w->EffectData)->World) Save=false;
		break;
	case 8:
		if (!((DISPLAY3DTEMPLATE*)w->EffectData)->World) Save=false;
		break;
	}

	if (Save)
	{
		int size=30+4+8+4+4+40+40+32+w->Effect->DataSize;
		int i=EVT2;
		fwrite(&i,1,4,f);
		fwrite(&size,1,4,f);
		fwrite(w->Name,1,30,f);
		fwrite(&w->Num,1,4,f);
		fwrite(&w->StartFrame,1,4,f);
		fwrite(&w->EndFrame,1,4,f);
		fwrite(&w->Effect->Type,1,4,f);
		fwrite(&w->Pass,1,4,f);
		fwrite(&w->iParameters,10,4,f);
		fwrite(&w->fParameters,10,4,f);
		fwrite(&w->sx1,1,4,f);
		fwrite(&w->sy1,1,4,f);
		fwrite(&w->sx2,1,4,f);
		fwrite(&w->sy2,1,4,f);
		fwrite(&w->ex1,1,4,f);
		fwrite(&w->ey1,1,4,f);
		fwrite(&w->ex2,1,4,f);
		fwrite(&w->ey2,1,4,f);

		switch (w->Effect->Type)
		{
			case 0: //NO EFFECT
				fwrite(w->EffectData,1,w->Effect->DataSize,f);
			break;
			case 1: //2D LAYER
				{
					fwrite(w->EffectData,1,w->Effect->DataSize-4,f);
					int i=0;
					MATERIAL *m=((LAYER2DTEMPLATE*)w->EffectData)->Texture;
					
					if (m)
					{
						fwrite(&m->TextureID,1,3,f);
						fwrite(&m->SlotNumber,1,1,f);
					}

					else
					fwrite(&i,4,1,f);
				}
			break;
			case 2: //PARTICLE RENDER
				{
					int i=((PARTICLETEMPLATE*)w->EffectData)->World->ID;
					fwrite(&i,1,4,f);
					fwrite(&((PARTICLETEMPLATE*)w->EffectData)->CamID,4,1,f);
					fwrite(&((PARTICLETEMPLATE*)w->EffectData)->AnimID,4,1,f);
					fwrite(&((PARTICLETEMPLATE*)w->EffectData)->AnimStart,4,1,f);
					fwrite(&((PARTICLETEMPLATE*)w->EffectData)->AnimEnd,4,1,f);
				}
			break;
			case 3: //CLEAR
				fwrite(w->EffectData,1,w->Effect->DataSize,f);
			break;
			case 4: //RENDERTEXTURE
				fwrite(w->EffectData,1,w->Effect->DataSize,f);
			break;
			case 5: //FEEDBACK
				fwrite(w->EffectData,1,w->Effect->DataSize,f);
			break;
			case 7: //3D SCENE
				{
					int i=((DISPLAY3DTEMPLATE*)w->EffectData)->World->ID;
					fwrite(&i,1,4,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamID,4,1,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimID,4,1,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamStart,4,1,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamEnd,4,1,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimStart,4,1,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimEnd,4,1,f);
					fwrite(&((DISPLAY3DTEMPLATE*)w->EffectData)->DontSaveCam,1,1,f);
				}
			break;
			case 8: //PARTICLE RESET
				{
					int i=((DISPLAY3DTEMPLATE*)w->EffectData)->World->ID;
					fwrite(&i,1,4,f);
				}
			break;
			case 12: //GRIDFILL
				{
					fwrite(w->EffectData,1,w->Effect->DataSize-4,f);
					int i=0;
					MATERIAL *m=((GRIDFILLERTEMPLATE*)w->EffectData)->Texture;
					
					if (m)
					{
						fwrite(&m->TextureID,1,3,f);
						fwrite(&m->SlotNumber,1,1,f);
					}

					else
					fwrite(&i,4,1,f);
				}
			break;
			case 13: //CAVE
				{
					fwrite(w->EffectData,1,w->Effect->DataSize-4,f);
					int i=0;
					MATERIAL *m=((CAVETEMPLATE*)w->EffectData)->Texture;
					
					if (m)
					{
						fwrite(&m->TextureID,1,3,f);
						fwrite(&m->SlotNumber,1,1,f);
					}

					else
					fwrite(&i,4,1,f);
				}
			break;
			default:
				fwrite(w->EffectData,1,w->Effect->DataSize,f);
		}
	}
}

void __stdcall SaveProject(cINTERFACE* Interface, int ButtonId)
{

	FILE *f;
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		{
			char* NewLine=new char[strlen(CommandLine)+strlen(ProjectDir)+10];
			memset(NewLine,0,strlen(CommandLine)+strlen(ProjectDir)+10);
			if (!strstr(CommandLine,".dmo\0"))
			sprintf(NewLine,"%s/%s.dmo\0",ProjectDir,CommandLine);
			else
			sprintf(NewLine,"%s/%s",ProjectDir,CommandLine);

			f=fopen(NewLine,"rb");
			bool Exists=false;
			if (f) { Exists=true; fclose(f); }			

			bool Proceed=true;
			if (Exists) 
			{
				Proceed=!(MessageBox(NULL,"Overwrite it?", "File exists.",MB_OKCANCEL|MB_ICONEXCLAMATION)==IDCANCEL);
				if (Proceed)
				{
					char* NewLine2=new char[255];
					_snprintf(NewLine2,255,"%s.bak\0",NewLine);

					FILE *k=fopen(NewLine2,"rb");
					if (k)
					{
						remove(NewLine2);
						fclose(k);
					}

					rename(NewLine,NewLine2);
					delete NewLine2;
				}
			}

			if (Proceed)
			{
				LastFileName=new char[256];
				_snprintf(LastFileName,256,"%s\0",CommandLine);

				f=fopen(NewLine,"w+b");

				if (f)
				{
					fwrite("aDDict2P",8,1,f);

					//for (TEXTURE *t=TextureList; t; t=t->Next) SaveTextureChunk(f,t);
					for (ITEM *i=TextureListScroller->List; i; i=i->Next)
					{
						SaveTextureChunk(f,(TEXTURE*)i->Data);
					}

					for (ITEM *it=PolySelections->List; it; it=it->Next)
					{
						if (it->Name[0]=='P')
						SavePolySelectChunk(f,it);
						else
						SaveVertexSelectChunk(f,it);
						for (ITEM *k=((ITEMLIST*)it->Data)->List;k;k=k->Next)
						{
							SaveBrushChunk(f,k);
						}
					}

					for (SCENE *s=Scenes; s; s=s->Next)
					{
						SaveSceneChunk(f,s);
						for (int x=0; x<s->ObjectNum; x++)
						if (!s->ObjectList[x].Deleted)
						{
							switch (s->ObjectList[x].Primitive)
							{
							case aDDict_BOX:
							case aDDict_SPHERE:
							case aDDict_CYLINDER:
							case aDDict_CONE:
							case aDDict_PLANE:
							case aDDict_ARC:
							case aDDict_LINE:
							case aDDict_LOFT:
							case aDDict_ICOSAEDER:
							case aDDict_DODECAEDER:
							case aDDict_CIRCLE:
							case aDDict_STORED:
							case aDDict_FUR:
							case aDDict_SUPERSHAPE:
								SaveObjectChunk(f,&s->ObjectList[x]);
								break;
							case aDDict_MAPXFORM:
								SaveMapxform(f,&s->ObjectList[x]);
								break;
							case aDDict_CLONE:
								SaveClone(f,&s->ObjectList[x]);
								break;
							case aDDict_LINEARSMOOTH:
							case aDDict_BUTTERFLYSMOOTH:
							case aDDict_MESHBLUR:
								SaveMeshSmooth(f,&s->ObjectList[x]);
								break;
							}
						}
						
					}

					for (WORLD *w=Worlds; w; w=w->Next)
					{
						SaveWorldChunk(f,w);
						int x;
						for (x=0; x<w->AnimNum; x++) 
						if (!w->Animations[x].Deleted)
						{
							SaveWorldAnimChunk(f,&w->Animations[x]);
						}
						for (x=0; x<w->CamNum; x++)
						if (!w->CamAnims[x].Deleted)
						{
							SaveWorldCamAnimChunk(f,&w->CamAnims[x]);
						}
						for (x=0; x<w->ObjectNum; x++)
						if (!w->ObjectList[x].Deleted)
						{
							SaveWorldObjectChunk(f,&w->ObjectList[x]);
							for (int y=0; y<w->AnimNum; y++)
							{
								SaveWorldObjAnimChunk(f,&w->ObjectList[x].Animations[y]);
							}
						}
					}

					for (EVENT *e=EventList; e; e=e->Next)
					{
						SaveEventChunk(f,e);
					}

					int j=END0;
					fwrite(&j,4,1,f);
					j=0;
					fwrite(&j,4,1,f);

					fclose(f);
					ErrorMessage="FILE EXPORTED SUCCESFULLY.";
				}
				else ErrorMessage="ERROR EXPORTING FILE.";
				GUI_GetFileList(Import,0);
			}
		}
	}

}

#ifndef Public_Release
void __stdcall SaveMinimalProject(cINTERFACE* Interface, int ButtonId)
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
			char* NewLine=new char[strlen(CommandLine)+strlen(ProjectDir)+10];
			memset(NewLine,0,strlen(CommandLine)+strlen(ProjectDir)+10);
			if (!strstr(CommandLine,".min"))
			sprintf(NewLine,"%s/%s.min\0",MinimalDir,CommandLine);
			else
			sprintf(NewLine,"%s/%s\0",MinimalDir,CommandLine);
			ExportMinimalProject(NewLine);
			ErrorMessage="FILE EXPORTED SUCCESFULLY.";
		}
	}

}
#endif

void __stdcall SaveScene(cINTERFACE* Interface, int ButtonId)
{
	FILE *f;
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
		}
		if (CommandLineEnter && strcmp(CommandLine,"")) 
		if (ActualScene)
		{
			char* NewLine=new char[strlen(CommandLine)+strlen(ProjectDir)+10];
			memset(NewLine,0,strlen(CommandLine)+strlen(ProjectDir)+10);
			if (!strstr(CommandLine,".scn"))
			sprintf(NewLine,"%s/%s.scn\0",SceneDir,CommandLine);
			else
			sprintf(NewLine,"%s/%s",SceneDir,CommandLine);
			f=fopen(NewLine,"w+b");

			fwrite("aDDict2S",8,1,f);

			//for (TEXTURE *t=TextureList; t; t=t->Next) SaveTextureChunk(f,t);
			int TexCount=0;

			ITEM *i;
			for (i=TextureListScroller->List; i; i=i->Next) TexCount=max(TexCount,((TEXTURE*)i->Data)->Id);
			bool *UsedTextures=new bool[TexCount+1];
			memset(UsedTextures,0,TexCount+1);

			int x;
			for (x=0; x<ActualScene->ObjectNum; x++)
				if (ActualScene->ObjectList[x].Primitive!=aDDict_LINE && ActualScene->ObjectList[x].Primitive!=aDDict_ARC)
				{
					if (ActualScene->ObjectList[x].Textured || ActualScene->ObjectList[x].Primitive==aDDict_MAPXFORM)
						UsedTextures[ActualScene->ObjectList[x].Material->TextureID]=true;
					if (ActualScene->ObjectList[x].EnvMapped)
						UsedTextures[ActualScene->ObjectList[x].EnvMap->TextureID]=true;
				}

			for (i=TextureListScroller->List; i; i=i->Next)
				if (UsedTextures[((TEXTURE*)i->Data)->Id])
				{
					SaveTextureChunk(f,(TEXTURE*)i->Data);
				}

			delete[] UsedTextures;

			int PolySelects=0;

			ITEM *it;
			for (it=PolySelections->List; it; it=it->Next) PolySelects++;
			bool *PolySelectUsed=new bool[PolySelects];
			memset(PolySelectUsed,0,PolySelects);

			for (x=0; x<ActualScene->ObjectNum; x++)
				if (ActualScene->ObjectList[x].Primitive==aDDict_MAPXFORM && ActualScene->ObjectList[x].Param1)
				{
					PolySelectUsed[ActualScene->ObjectList[x].Param2]=true;
				}

			int *PolySelectMap=new int[PolySelects];
			memset(PolySelectMap,0,PolySelects*4);
			int Counter=0;
			for (x=0; x<PolySelects; x++)
				if (PolySelectUsed[x])
				{
					PolySelectMap[x]=Counter;
					Counter++;
				}

			Counter=0;
			for (it=PolySelections->List; it; it=it->Next)
			{
				if (PolySelectUsed[Counter])
				{
					if (it->Name[0]=='P')
					SavePolySelectChunk(f,it);
					else
					SaveVertexSelectChunk(f,it);
					for (ITEM *k=((ITEMLIST*)it->Data)->List;k;k=k->Next)
					{
						SaveBrushChunk(f,k);
					}
				}
				Counter++;
			}

			SCENE *s=ActualScene;
			//SaveSceneChunk(f,s);
			for (x=0; x<s->ObjectNum; x++)
			if (!s->ObjectList[x].Deleted)
			{
				switch (s->ObjectList[x].Primitive)
				{
				case aDDict_BOX:
				case aDDict_SPHERE:
				case aDDict_CYLINDER:
				case aDDict_CONE:
				case aDDict_PLANE:
				case aDDict_ARC:
				case aDDict_LINE:
				case aDDict_LOFT:
				case aDDict_ICOSAEDER:
				case aDDict_DODECAEDER:
				case aDDict_CIRCLE:
				case aDDict_STORED:
				case aDDict_SUPERSHAPE:
					SaveObjectChunk(f,&s->ObjectList[x]);
					break;
				case aDDict_MAPXFORM:
					{
						int Buffer=s->ObjectList[x].Param2;
						s->ObjectList[x].Param2=PolySelectMap[s->ObjectList[x].Param2];
						SaveMapxform(f,&s->ObjectList[x]);
						s->ObjectList[x].Param2=Buffer;
						break;
					}
				case aDDict_CLONE:
					SaveClone(f,&s->ObjectList[x]);
					break;
				case aDDict_LINEARSMOOTH:
				case aDDict_BUTTERFLYSMOOTH:
				case aDDict_MESHBLUR:
					SaveMeshSmooth(f,&s->ObjectList[x]);
					break;
				}
			}
				
			int j=END0;
			fwrite(&j,4,1,f);
			j=0;
			fwrite(&j,4,1,f);

			fclose(f);
			ErrorMessage="SCENE EXPORTED SUCCESFULLY.";
			GUI_GetFileList(Import,0);
		}
		else ErrorMessage="NO SCENE TO EXPORT";
	}
}


void LoadTextureChunk(FILE *f, int TexCount, int Start, int BaseID)
{
	char *Name=ReadASCIIZ(f);
	int ID,Res,CommandNo;
	fread(&ID,1,4,f);
	ID+=BaseID;
	fread(&Res,1,4,f);
	fread(&CommandNo,1,4,f);
	int Ores=Res;
	if (LowRezLoad) Res=32;

	if (!TextureList)
	{
		TextureList=new TEXTURE(Res);
		TextureList->Next=NULL;
		TextureList->Id=ID;
	}
	else
	{
		TextureSelect->ItemList[0].SelectedItem++;
		TEXTURE *Buffer=new TEXTURE(Res);
		Buffer->Next=TextureList;
		Buffer->Id=ID;
		TextureList=Buffer;
	}

	TextureList->OXRes=Ores;

	int x;
	for (x=0; x<CommandNo; x++)
	{
		COMMAND *cmd=new COMMAND;
		memset(cmd,0,sizeof(COMMAND));
		fread(&cmd->Filter,1,1,f); // 4 bit
		fread(&cmd->Layer,1,1,f); // 4 bit
		fread(&cmd->Operator,1,1,f); //2 bit
		fread(&cmd->OperatorMask,4,1,f); //4 bit
		int Size=0;
		switch (cmd->Filter)
		{
		case TEXGEN_Envmap:
			Size = sizeof(ENVMAPTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Plasma:					
			Size = sizeof(PLASMATEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Map:					
			Size = sizeof(MAPTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			((MAPTEMPLATE*)cmd->data)->YDataChannel=((MAPTEMPLATE*)cmd->data)->XDataChannel;
			((MAPTEMPLATE*)cmd->data)->YDataLayer=((MAPTEMPLATE*)cmd->data)->XDataLayer;
			break;					
		case TEXGEN_Blur:					
			Size = sizeof(BLURTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_DirBlur:				
			Size = sizeof(DIRBLURTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Text:					
			{
			Size = sizeof(TEXTTEMPLATE);
			cmd->data=new char[Size];
			memset(cmd->data,0,Size);
			cmd->DataSize=Size;
			memset(cmd->data,0,Size);
			fread(cmd->data,1,Size-5,f);
			char* NewLine=new char[256];
			memset(NewLine,0,256);
			char c;
			c=1;
			for (int y=0; c!=0; y++)
			{
				fread(&c,1,1,f);
				NewLine[y]=c;
			}
			void *MemPos=(char*)cmd->data+Size-4;
			memcpy(MemPos,&NewLine,4);

			break;					
			}
		case TEXGEN_SubPlasma:				
			Size = sizeof(SUBPLASMATEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_FractalPlasmaOld:				
			Size = sizeof(FRACTALPLASMAOLDTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_FractalPlasma:			
			Size = sizeof(FRACTALPLASMAOLDTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			cmd->Filter=TEXGEN_FractalPlasmaOld;
			unsigned char k;
			fread(cmd->data,1,Size,f);
			//fread(&k,1,1,f);
			/*((FRACTALPLASMATEMPLATE*)cmd->data)->RandSeed=k;
			((FRACTALPLASMATEMPLATE*)cmd->data)->Blend=0;
			((FRACTALPLASMATEMPLATE*)cmd->data)->Min=1;
			((FRACTALPLASMATEMPLATE*)cmd->data)->Max=6;*/

			break;					
		case TEXGEN_Colorize:				
			Size = sizeof(COLORIZETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Shade:					
			Size = sizeof(SHADETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Brightness:				
			Size = sizeof(BRIGHTNESSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Copy:					
			Size = sizeof(COPYTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Cells:					
			Size = sizeof(CELLSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Twirl:					
			Size = sizeof(TWIRLTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_SineDist:				
			Size = sizeof(SINEDISTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Mix:					
			Size = sizeof(MIXTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_MixMap:					
			Size = sizeof(MIXMAPTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Scale:					
			Size = sizeof(SCALETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_SineColor:				
			Size = sizeof(SINECOLORTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Max:					
			Size = sizeof(MAXTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_HSV:					
			Size = sizeof(HSVTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Glass:				
			Size = sizeof(GLASSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Pixelize:					
			Size = sizeof(PIXELIZETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Offset:					
			Size = sizeof(OFFSETTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Crystalize:					
			Size = sizeof(CRYSTALIZETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Rectangle:					
			Size = sizeof(RECTANGLETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Circle:					
			Size = sizeof(CIRCLETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Contrast:					
			Size = sizeof(CONTRASTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Gradient:					
			Size = sizeof(GRADIENTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Rotozoom:					
			Size = sizeof(ROTOZOOMTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_ChamferRectangle:					
			Size = sizeof(CHAMFERRECTANGLETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Dots:					
			Size = sizeof(DOTSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_MakeMaterial: 
			{
			Size = sizeof(MAKEMATERIALTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			memset(cmd->data,0,Size);
			fread(cmd->data,1,Size-4,f);
			char* NewLine=new char[256];
			memset(NewLine,0,256);
			char c;
			c=1;
			for (int y=0; c!=0; y++)
			{
				fread(&c,1,1,f);
				NewLine[y]=c;
			}

			void *MemPos=(char*)cmd->data+Size-4;
			memcpy(MemPos,&NewLine,4); 

			//delete NewLine;
			break; 
			}
		case TEXGEN_Jpeg:
			{
				cmd->data=new char[8];
				JPEGTEMPLATE *a=(JPEGTEMPLATE*)cmd->data;
				fread(&a->JPEGDataSize,1,4,f);
				a->JPEGData=new unsigned char[a->JPEGDataSize];
				fread(a->JPEGData,1,a->JPEGDataSize,f);
			}
			break;
		}


		TextureList->AddCommand(cmd);
	}

	ActualTexture=TextureList;

	RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->XRes];
	//StatusBar2->ParentItem->ActiveBranch=true;
	for (int z=0; z<CommandNo; z++)
	{
		UndoStatus2=((z+1)/(float)CommandNo+Start)/(float)TexCount;
		PerformCommand(ActualTexture,&ActualTexture->Commands[z],Buffer);
		GUIReDraw();
	}

	ActualTexture->Name=Name;

	char *NewLine=new char[strlen(CommandLine)+51];
	memset(NewLine,0,strlen(CommandLine)+51);
	_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s (%dx%d)",TextureList->Id,Name,ActualTexture->XRes,ActualTexture->YRes);
	TextureListScroller->AddItem(NewLine,TextureList,Scroller_SelectOnlyThisTexture,GUI_RenameTexture,nop);
	//StatusBar2->ParentItem->ActiveBranch=false;

	EditMenu->ParentItem->ParentInterface->ClickedItem=ActualTexture->CommandList->List;
	EditMenu->ParentItem->ParentInterface->ClickedNum=0;
	EditMenu->ParentItem->ParentInterface->ItemList[0].SelectedItem=0;
	EditMenu->ParentItem->ParentInterface->ItemList[0].StartItem=0;

	cINTERFACE *Interface=EditMenu->ParentItem->ParentInterface;

	/*if (Interface->ClickedItem)
	{
		EditMenu->SetActiveBranch(((COMMAND*)Interface->ClickedItem->Data)->Filter);
		SelectedFilter=((COMMAND*)Interface->ClickedItem->Data)->Filter;
		EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Layer=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
		EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Operator=ActualTexture->Commands[Interface->ClickedItem->Num].Operator;
		for (x=0; x<4; x++) EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.OperatorMask[x]=ActualTexture->Commands[Interface->ClickedItem->Num].OperatorMask[x];
		memcpy(EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.data,ActualTexture->Commands[Interface->ClickedItem->Num].data,EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.DataSize);
		EditMenu->ClickedNum=Interface->ClickedItem->Num;
		ATCI=EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface;
		ActualTextureCommandInterface->SelectedLeftTexture=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
	}*/

	for (x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
	DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;

	EditMenu->ParentItem->SelectedItem=0;
	EditMenu->ParentItem->StartItem=0;

	SetEnvironmentToActualTexture();

	delete Buffer;
}

void LoadTexture1Chunk(FILE *f, int TexCount, int Start, int BaseID)
{
	char *Name=ReadASCIIZ(f);
	int ID,Res,CommandNo;
	fread(&ID,1,4,f);
	ID+=BaseID;
	fread(&Res,1,4,f);
	fread(&CommandNo,1,4,f);
	int Ores=Res;
	if (LowRezLoad) Res=32;

	if (!TextureList)
	{
		TextureList=new TEXTURE(Res);
		TextureList->Next=NULL;
		TextureList->Id=ID;
	}
	else
	{
		TextureSelect->ItemList[0].SelectedItem++;
		TEXTURE *Buffer=new TEXTURE(Res);
		Buffer->Next=TextureList;
		Buffer->Id=ID;
		TextureList=Buffer;
	}

	TextureList->OXRes=Ores;

	texcnt+=CommandNo;
	int x;
	for (x=0; x<CommandNo; x++)
	{
		COMMAND *cmd=new COMMAND;
		memset(cmd,0,sizeof(COMMAND));
		fread(&cmd->Filter,1,1,f); // 4 bit
		fread(&cmd->Layer,1,1,f); // 4 bit
		fread(&cmd->Operator,1,1,f); //2 bit
		fread(&cmd->OperatorMask,4,1,f); //4 bit
		texops[cmd->Filter]++;
		int Size=0;
		switch (cmd->Filter)
		{
		case TEXGEN_Envmap:
			Size = sizeof(ENVMAPTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Plasma:					
			Size = sizeof(PLASMATEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Map:					
			Size = sizeof(MAPTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Blur:					
			Size = sizeof(BLURTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_DirBlur:				
			Size = sizeof(DIRBLURTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Text:					
			{
			Size = sizeof(TEXTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			memset(cmd->data,0,Size);
			fread(cmd->data,1,Size-4,f);
			char* NewLine=new char[256];
			memset(NewLine,0,256);
			char c;
			c=1;
			for (int y=0; c!=0; y++)
			{
				fread(&c,1,1,f);
				NewLine[y]=c;
			}
			void *MemPos=(char*)cmd->data+Size-4;
			memcpy(MemPos,&NewLine,4);

			break;					
			}
		case TEXGEN_SubPlasma:				
			Size = sizeof(SUBPLASMATEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_FractalPlasmaOld:				
			Size = sizeof(FRACTALPLASMAOLDTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_FractalPlasma:			
			Size = sizeof(FRACTALPLASMATEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Colorize:				
			Size = sizeof(COLORIZETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Shade:					
			Size = sizeof(SHADETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Brightness:				
			Size = sizeof(BRIGHTNESSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Copy:					
			Size = sizeof(COPYTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Cells:					
			Size = sizeof(CELLSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Twirl:					
			Size = sizeof(TWIRLTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_SineDist:				
			Size = sizeof(SINEDISTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Mix:					
			Size = sizeof(MIXTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_MixMap:					
			Size = sizeof(MIXMAPTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Scale:					
			Size = sizeof(SCALETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_SineColor:				
			Size = sizeof(SINECOLORTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Max:					
			Size = sizeof(MAXTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_HSV:					
			Size = sizeof(HSVTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Glass:				
			Size = sizeof(GLASSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Pixelize:					
			Size = sizeof(PIXELIZETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Offset:					
			Size = sizeof(OFFSETTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Crystalize:					
			Size = sizeof(CRYSTALIZETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Rectangle:					
			Size = sizeof(RECTANGLETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Circle:					
			Size = sizeof(CIRCLETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Contrast:					
			Size = sizeof(CONTRASTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Gradient:					
			Size = sizeof(GRADIENTTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Rotozoom:					
			Size = sizeof(ROTOZOOMTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_ChamferRectangle:					
			Size = sizeof(CHAMFERRECTANGLETEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_Dots:					
			Size = sizeof(DOTSTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			fread(cmd->data,1,Size,f);
			break;					
		case TEXGEN_MakeMaterial: 
			{
			Size = sizeof(MAKEMATERIALTEMPLATE);
			cmd->data=new char[Size];
			cmd->DataSize=Size;
			memset(cmd->data,0,Size);
			fread(cmd->data,1,Size-4,f);
			char* NewLine=new char[256];
			memset(NewLine,0,256);
			char c;
			c=1;
			for (int y=0; c!=0; y++)
			{
				fread(&c,1,1,f);
				NewLine[y]=c;
			}

			void *MemPos=(char*)cmd->data+Size-4;
			memcpy(MemPos,&NewLine,4); 

			//delete NewLine;
			break; 
			}
		case TEXGEN_Jpeg:
			{
				cmd->data=new char[8];
				JPEGTEMPLATE *a=(JPEGTEMPLATE*)cmd->data;
				fread(&a->JPEGDataSize,1,4,f);
				a->JPEGData=new unsigned char[a->JPEGDataSize];
				fread(a->JPEGData,1,a->JPEGDataSize,f);
			}
			break;
		}


		TextureList->AddCommand(cmd);
	}

	ActualTexture=TextureList;

	RGBA *Buffer=new RGBA[ActualTexture->XRes*ActualTexture->XRes];
	//StatusBar2->ParentItem->ActiveBranch=true;
	for (int z=0; z<CommandNo; z++)
	{
		UndoStatus2=((z+1)/(float)CommandNo+Start)/(float)TexCount;
		PerformCommand(ActualTexture,&ActualTexture->Commands[z],Buffer);
		GUIReDraw();
	}

	ActualTexture->Name=Name;

	char *NewLine=new char[strlen(CommandLine)+51];
	memset(NewLine,0,strlen(CommandLine)+51);
	_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s (%dx%d)",TextureList->Id,Name,ActualTexture->XRes,ActualTexture->YRes);
	TextureListScroller->AddItem(NewLine,TextureList,Scroller_SelectOnlyThisTexture,GUI_RenameTexture,nop);
	//StatusBar2->ParentItem->ActiveBranch=false;

	EditMenu->ParentItem->ParentInterface->ClickedItem=ActualTexture->CommandList->List;
	EditMenu->ParentItem->ParentInterface->ClickedNum=0;
	EditMenu->ParentItem->ParentInterface->ItemList[0].SelectedItem=0;
	EditMenu->ParentItem->ParentInterface->ItemList[0].StartItem=0;

	cINTERFACE *Interface=EditMenu->ParentItem->ParentInterface;

	if (Interface->ClickedItem)
	{
		EditMenu->SetActiveBranch(((COMMAND*)Interface->ClickedItem->Data)->Filter);
		SelectedFilter=((COMMAND*)Interface->ClickedItem->Data)->Filter;
		EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Layer=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
		EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.Operator=ActualTexture->Commands[Interface->ClickedItem->Num].Operator;
		for (x=0; x<4; x++) EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.OperatorMask[x]=ActualTexture->Commands[Interface->ClickedItem->Num].OperatorMask[x];
		memcpy(EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.data,ActualTexture->Commands[Interface->ClickedItem->Num].data,EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface->TextureCommand.DataSize);
		EditMenu->ClickedNum=Interface->ClickedItem->Num;
		ATCI=EditMenu->ItemList[ActualTexture->Commands[Interface->ClickedItem->Num].Filter].Interface;
		ActualTextureCommandInterface->SelectedLeftTexture=ActualTexture->Commands[Interface->ClickedItem->Num].Layer;
	}

	for (x=0; x<DisplayArea->ItemNum; x++) DisplayArea->ItemList[x].ButtonPushed=false;
	DisplayArea->ItemList[ActualTextureCommandInterface->SelectedLeftTexture].ButtonPushed=true;

	EditMenu->ParentItem->SelectedItem=0;
	EditMenu->ParentItem->StartItem=0;

	SetEnvironmentToActualTexture();

	delete Buffer;
}

FILE *OutText=fopen("outdata.txt","w+t");

void LoadSceneChunk(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	fprintf(OutText,"scene: %s\n",Name);
	SCENE *s=new SCENE;
	s->Name=Name;
	fread(&s->ID,1,4,f);
	fread(&s->Camera,1,sizeof(CAMERA)-16,f);
	fread(&s->Fog,1,sizeof(RGBA),f);
	fread(s->Lights,8,sizeof(LIGHT),f);
	s->ColorDiscard=false;
	if (!Scenes)
	{
		LastScene=Scenes=s;
		SceneScroller->ParentItem->ItemNum++;
		AddModellInterface->ParentItem->ItemNum++;
	}
	else
	{
		SceneScroller->ParentItem->SelectedItem++;
		AddModellInterface->ParentItem->SelectedItem++;
		SceneScroller->ParentItem->ItemNum++;
		AddModellInterface->ParentItem->ItemNum++;
		LastScene->Next=s;
		LastScene=s;
	}
	ActualScene=LastScene;

	LastScene->ItemList=new ITEMLIST();
	LastScene->ArcList=new ITEMLIST();
	ITEM *SceneData=LastScene->ItemList->AddItem("SCENE DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(Name)+51];
	memset(NameString,0,strlen(Name)+51);
	_snprintf(NameString,strlen(Name)+51,"%s",Name);
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
	char *NewLine=new char[strlen(Name)+51];
	memset(NewLine,0,strlen(Name)+51);
	_snprintf(NewLine,strlen(Name)+51,"%d: %s",LastScene->ID,Name);
	SceneListScroller->AddItem(NewLine,LastScene,Scroller_SelectOnlyThisScene,GUI_RenameScene,nop);
	SetEnvironmentToActualScene();

	memset(LastScene->Lights,0,sizeof(LIGHT)*8);
	for (int x=0; x<8; x++) 
	{
		LastScene->Lights[x].Identifier=GL_LIGHT0+x;
		LastScene->Lights[x].Constant_Attenuation=1.0f;
		LastScene->Lights[x].Spot_Cutoff=180.0f;
	}
	LastScene->Lights[0].Lit=true;
	LastScene->Lights[0].Position[0]=-1.0f;
	LastScene->Lights[0].Position[1]=1.0f;
	LastScene->Lights[0].Position[2]=1.0f;
	LastScene->Lights[0].Color[0]=1.0f;
	LastScene->Lights[0].Color[1]=1.0f;
	LastScene->Lights[0].Color[2]=1.0f;
	LastScene->Lights[1].Lit=true;
	LastScene->Lights[1].Position[0]=1.0f;
	LastScene->Lights[1].Position[1]=-1.0f;
	LastScene->Lights[1].Position[2]=-1.0f;
	LastScene->Lights[1].Color[0]=0.3f;
	LastScene->Lights[1].Color[1]=0.3f;
	LastScene->Lights[1].Color[2]=0.3f;
}

void LoadSceneChunk1(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	fprintf(OutText,"scene: %s\n",Name);
	SCENE *s=new SCENE;
	s->Name=Name;
	fread(&s->ID,1,4,f);
	fread(&s->Camera,1,sizeof(CAMERA)-16,f);
	fread(&s->Fog,1,sizeof(RGBA),f);
	fread(s->Lights,8,sizeof(LIGHT),f);
	fread(&s->ColorDiscard,1,1,f);
	if (!Scenes)
	{
		LastScene=Scenes=s;
		SceneScroller->ParentItem->ItemNum++;
		AddModellInterface->ParentItem->ItemNum++;
	}
	else
	{
		SceneScroller->ParentItem->SelectedItem++;
		AddModellInterface->ParentItem->SelectedItem++;
		SceneScroller->ParentItem->ItemNum++;
		AddModellInterface->ParentItem->ItemNum++;
		LastScene->Next=s;
		LastScene=s;
	}
	ActualScene=LastScene;

	LastScene->ItemList=new ITEMLIST();
	LastScene->ArcList=new ITEMLIST();
	ITEM *SceneData=LastScene->ItemList->AddItem("SCENE DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(Name)+51];
	memset(NameString,0,strlen(Name)+51);
	_snprintf(NameString,strlen(Name)+51,"%s",Name);
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
	char *NewLine=new char[strlen(Name)+51];
	memset(NewLine,0,strlen(Name)+51);
	_snprintf(NewLine,strlen(Name)+51,"%d: %s",LastScene->ID,Name);
	SceneListScroller->AddItem(NewLine,LastScene,Scroller_SelectOnlyThisScene,GUI_RenameScene,nop);
	SetEnvironmentToActualScene();

	memset(LastScene->Lights,0,sizeof(LIGHT)*8);
	for (int x=0; x<8; x++) 
	{
		LastScene->Lights[x].Identifier=GL_LIGHT0+x;
		LastScene->Lights[x].Constant_Attenuation=1.0f;
		LastScene->Lights[x].Spot_Cutoff=180.0f;
	}
	LastScene->Lights[0].Lit=true;
	LastScene->Lights[0].Position[0]=-1.0f;
	LastScene->Lights[0].Position[1]=1.0f;
	LastScene->Lights[0].Position[2]=1.0f;
	LastScene->Lights[0].Color[0]=1.0f;
	LastScene->Lights[0].Color[1]=1.0f;
	LastScene->Lights[0].Color[2]=1.0f;
	LastScene->Lights[1].Lit=true;
	LastScene->Lights[1].Position[0]=1.0f;
	LastScene->Lights[1].Position[1]=-1.0f;
	LastScene->Lights[1].Position[2]=-1.0f;
	LastScene->Lights[1].Color[0]=0.3f;
	LastScene->Lights[1].Color[1]=0.3f;
	LastScene->Lights[1].Color[2]=0.3f;
}


MATERIAL *FindMaterial(int Textureid, int Textureslot)
{
	if (Textureslot<0 || Textureslot>=8) return NULL;
	MATERIAL *m=NULL;

	for (TEXTURE *t=TextureList;t;t=t->Next)
	{
		if (t->Id==Textureid) m=&t->Materials[Textureslot];
	}

	return m;
}

void LoadObjectChunk(FILE *f, int BaseID, int TextureBase, bool HaveStoredColorInfo)
{
	char *Name=ReadASCIIZ(f);
	fprintf(OutText,"%s\n",Name);

	ActualScene->AddObject();

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected,1,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name=Name;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Data=&ActualScene->ObjectList[ActualScene->ObjectNum-1];
	MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
	MainScroller->ParentItem->ItemNum++;
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ID,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ID+=BaseID;

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param5,4,1,f);

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive,1,1,f);
	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_CONE && ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3==0) 
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3=1;

	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_STORED)
	{
		OBJECT *o=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

		o->PerVertexColor=false;
		if (HaveStoredColorInfo) fread(&o->PerVertexColor,1,1,f);

		int onum;
		fread(&onum,4,1,f);
		int x;
		for (x=0; x<onum; x++)
		{
			VECTOR3 p,n;
			VECTOR2 t;
			fread(&p,sizeof(VECTOR3),1,f);
			fread(&n,sizeof(VECTOR3),1,f);
			fread(&t,sizeof(VECTOR2),1,f);
			o->AddVertex(p.x,p.y,p.z,t.x,t.y);
			o->VertexList[x].CurrentNormal=o->VertexList[x].Normal=n;
			if (o->PerVertexColor)
			fread(&o->VertexList[x].Color,4,1,f);
		}

		fread(&onum,4,1,f);
		for (x=0; x<onum; x++)
		{
			int a,b,c;
			VECTOR3 n;
			VECTOR2 u,v,w;
			fread(&a,4,1,f);
			fread(&b,4,1,f);
			fread(&c,4,1,f);
			fread(&n,sizeof(VECTOR3),1,f);
			fread(&u,sizeof(VECTOR2),1,f);
			fread(&v,sizeof(VECTOR2),1,f);
			fread(&w,sizeof(VECTOR2),1,f);
			o->AddPolygon(a,b,c,aDDict_GOURAUDSHADE,1,1,1);
			o->PolygonList[x].CurrentNormal=o->PolygonList[x].Normal=n;
			o->PolygonList[x].t[0]=u;
			o->PolygonList[x].t[1]=v;
			o->PolygonList[x].t[2]=w;
			//o->PolygonList[x].Shading=aDDict_GOURAUDSHADE;
		}
	}

	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_LOFT)
	{
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1+=BaseID;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2+=BaseID;
		bool Found1=false,Found2=false;
		int x;
		for (x=0; x<ActualScene->ObjectNum; x++)
		{
			if (ActualScene->ObjectList[x].ID==ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1) 
			{
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=(int)&ActualScene->ObjectList[x];
				ActualScene->ObjectList[x].ListItem->Hidden=true;
				Found1=true;
				break;
			}
		}
		for (x=0; x<ActualScene->ObjectNum; x++)
		{
			if (ActualScene->ObjectList[x].ID==ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2) 
			{
				ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2=(int)&ActualScene->ObjectList[x];
				ActualScene->ObjectList[x].ListItem->Hidden=true;
				Found2=true;
				break;
			}
		}
		if (!Found1) ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=0;
		if (!Found2) ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2=0;
		if (!Found1 || !Found2) 
		{
			ActualScene->ObjectList[ActualScene->ObjectNum-1].Deleted=true;
			ErrorMessage="LOFT BASE ARC MISSING, BUGGY LOFT DELETED";
			ActualScene->ItemList->ItemNum--;
			if (ActualScene->ItemList->List==ActualScene->ItemList->Last)
			{
				ActualScene->ItemList->List=NULL;
				ActualScene->ItemList->Last=NULL;
			}
			else
			{
				ActualScene->ItemList->Last=ActualScene->ItemList->Last->Prev;
				ActualScene->ItemList->Last->Next=NULL;
			}
		}
	}

	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_FUR)
	{
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4+=BaseID;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Param5=(int)ActualScene->FindObjectByID(ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4);
	}

	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive!=aDDict_SUPERSHAPE)	
	GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param5);

	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_ARC)
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ArcListItem=ActualScene->ArcList->AddItem("ARC",&ActualScene->ObjectList[ActualScene->ObjectNum-1],Scroller_SelectOnlyThisArc,nop,nop);

	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_LINE)
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ArcListItem=ActualScene->ArcList->AddItem("LINE",&ActualScene->ObjectList[ActualScene->ObjectNum-1],Scroller_SelectOnlyThisArc,nop,nop);
	
	fread(ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView,1,sizeof(MATRIX),f);
	for (int x=0; x<4; x++)
	{
		for (int y=0; y<4; y++)
		{
			fprintf(OutText,"%8.5f ",ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView[y][x]);
			ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView[x][y]=KillFloat(ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView[x][y],2);
		}
		fprintf(OutText,"\n");
	}
	memcpy(ActualScene->ObjectList[ActualScene->ObjectNum-1].TransformBuffer,ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView,sizeof(MATRIX));
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Wireframe,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Textured,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].EnvMapped,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].NormalsInverted,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Xtile,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Ytile,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].XSwap,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].YSwap,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Swap,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetX,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetY,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].SRCBlend,sizeof(GLuint),1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].DSTBlend,sizeof(GLuint),1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Color,1,sizeof(RGBA),f);
	
	//ActualScene->ObjectList[ActualScene->ObjectNum-1].Material=new MATERIAL;
	int TID,SNum;
	fread(&TID,4,1,f);
	fread(&SNum,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].Material=FindMaterial(TID+TextureBase,SNum);

	//ActualScene->ObjectList[ActualScene->ObjectNum-1].EnvMap=new MATERIAL;
	fread(&TID,4,1,f);
	fread(&SNum,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].EnvMap=FindMaterial(TID+TextureBase,SNum);

	ActualScene->ObjectList[ActualScene->ObjectNum-1].ZMask=true;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].AEpsilon=0;
}

void LoadMapxform(FILE *f, int BaseID, int TextureBase, int PolySelectBase)
{
	char *Name=ReadASCIIZ(f);

	ActualScene->AddObject();
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected,1,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Hidden=true;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name=Name;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Data=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ID,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ID+=BaseID;

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2+=PolySelectBase;
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param5,4,1,f);

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive,1,1,f);

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView,1,sizeof(MATRIX),f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Xtile,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Ytile,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].XSwap,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].YSwap,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Swap,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetX,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetY,4,1,f);

	ActualScene->ObjectList[ActualScene->ObjectNum-1].Material=new MATERIAL;
	memset(ActualScene->ObjectList[ActualScene->ObjectNum-1].Material,0,sizeof(MATERIAL));
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Material->TextureID,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].Material->TextureID+=TextureBase;
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Material->SlotNumber,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].Material=FindMaterial(ActualScene->ObjectList[ActualScene->ObjectNum-1].Material->TextureID,ActualScene->ObjectList[ActualScene->ObjectNum-1].Material->SlotNumber);

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID+=BaseID;
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].MapXformType,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].MapXformColor,1,1,f);


	bool Found=false;
	int x;
	for (x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ID==ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID  && !ActualScene->ObjectList[x].Deleted)
		{
			MappedObject=x;
			Found=true;
			break;
		}
	}

	if (Found)
	{
		if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1==0 || PolySelections->List==NULL)
		{
			for (int x=0; x<ActualScene->ObjectList[MappedObject].VertexNum; x++)
			{
				ActualScene->ObjectList[MappedObject].VertexList[x].Selected=true;
			}
			ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=0;
		}
		else
		{
			ITEM *p=PolySelections->List;
			for (int x=0; x<ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2; x++) p=p->Next;
			ITEMLIST *b=(ITEMLIST*)p->Data;
			SelectObjVertices(&ActualScene->ObjectList[MappedObject],b,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1!=2);
		}

		CalculateObjectVertexWeights(&ActualScene->ObjectList[MappedObject],
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].Material,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].Xtile,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].Ytile,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].XSwap,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].YSwap,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].Swap,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetX,
									  ActualScene->ObjectList[ActualScene->ObjectNum-1].OffsetY);
		memcpy(MXMatrix,ActualScene->ObjectList[ActualScene->ObjectNum-1].ModelView,sizeof(MATRIX));

		CalculateMapTransform(ActualScene->ObjectList[ActualScene->ObjectNum-1].MapXformType,ActualScene->ObjectList[ActualScene->ObjectNum-1].MapXformColor,MXMatrix,MappedObject);
		for (x=0; x<ActualScene->ObjectList[MappedObject].VertexNum; x++)
			ActualScene->ObjectList[MappedObject].VertexList[x].Position=ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition;
		ActualScene->ObjectList[MappedObject].CalculateNormals();
		ActualScene->ObjectList[MappedObject].CreateDisplayList();
	}
	else
	{
		ActualScene->ItemList->ItemNum--;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Deleted=true;
		if (ActualScene->ItemList->List==ActualScene->ItemList->Last)
		{
			ActualScene->ItemList->List=NULL;
			ActualScene->ItemList->Last=NULL;
		}
		else
		{
			ActualScene->ItemList->Last=ActualScene->ItemList->Last->Prev;
			ActualScene->ItemList->Last->Next=NULL;
		}
	}
}

void LoadMeshSmooth(FILE *f, int BaseID)
{
	char *Name=ReadASCIIZ(f);

	ActualScene->AddObject();
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected,1,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Hidden=true;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name=Name;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Data=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ID,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ID+=BaseID;

	bool Linear;
	fread(&Linear,1,1,f);
	if (Linear) ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_LINEARSMOOTH;
	else ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_BUTTERFLYSMOOTH;

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1,4,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID=ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1+BaseID;

	bool Found=false;
	int x;
	for (x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ID==ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1  && !ActualScene->ObjectList[x].Deleted)
		{
			MappedObject=x;
			Found=true;
			break;
		}
	}

	if (Found)
	{
		for (x=0; x<ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2; x++)
			ActualScene->ObjectList[MappedObject].MeshSmooth(Linear);

	}
	else
	{
		ActualScene->ItemList->ItemNum--;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Deleted=true;
		if (ActualScene->ItemList->List==ActualScene->ItemList->Last)
		{
			ActualScene->ItemList->List=NULL;
			ActualScene->ItemList->Last=NULL;
		}
		else
		{
			ActualScene->ItemList->Last=ActualScene->ItemList->Last->Prev;
			ActualScene->ItemList->Last->Next=NULL;
		}
	}
}

void LoadMeshSmooth2(FILE *f, int BaseID)
{
	char *Name=ReadASCIIZ(f);

	ActualScene->AddObject();
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected,1,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Hidden=true;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name=Name;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Data=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ID,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ID+=BaseID;

	bool Linear;
	fread(&Linear,1,1,f);

	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1+=BaseID;
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2,4,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].TargetObjectID=ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1;

	bool Found=false;
	int x;
	for (x=0; x<ActualScene->ObjectNum; x++)
	{
		if (ActualScene->ObjectList[x].ID==ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1 && !ActualScene->ObjectList[x].Deleted)
		{
			MappedObject=x;
			Found=true;
			break;
		}
	}

	if (Found)
	{
		for (x=0; x<ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2; x++)		
		{
			if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive!=aDDict_MESHBLUR)
			ActualScene->ObjectList[MappedObject].MeshSmooth(Linear);
			else
			ActualScene->ObjectList[MappedObject].MeshBlur();
		}
	}
	else
	{
		ActualScene->ItemList->ItemNum--;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Deleted=true;
		if (ActualScene->ItemList->List==ActualScene->ItemList->Last)
		{
			ActualScene->ItemList->List=NULL;
			ActualScene->ItemList->Last=NULL;
		}
		else
		{
			ActualScene->ItemList->Last=ActualScene->ItemList->Last->Prev;
			ActualScene->ItemList->Last->Next=NULL;
		}
	}

}


void LoadClone(FILE *f, int BaseID)
{
	char *Name=ReadASCIIZ(f);

	ActualScene->AddObject();
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Selected,1,1,f);
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name=Name;
	ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Data=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

	OBJECT *o=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

	fread(&o->ID,4,1,f);
	o->ID+=BaseID;
	
	fread(&o->Param1,4,1,f);
	o->Param2=(int)new int[o->Param1];
	o->Param3=(int)new int[o->Param1];
	fread((int*)o->Param2,4,o->Param1,f);

	int x;
	for (x=0; x<o->Param1; x++) ((int*)o->Param2)[x]+=BaseID;

	for (x=0; x<ActualScene->ObjectNum; x++)
		for (int y=0; y<o->Param1; y++)
		{
			if (ActualScene->ObjectList[x].ID==((int*)o->Param2)[y])
			{
				((int*)o->Param3)[y]=(int)&ActualScene->ObjectList[x];
			}
		}

	fread(&o->Primitive,1,1,f);
	fread(o->ModelView,1,sizeof(MATRIX),f);
	memcpy(o->TransformBuffer,o->ModelView,sizeof(MATRIX));

	fprintf(OutText,"%s\n",Name);
	for (x=0; x<4; x++)
	{
		for (int y=0; y<4; y++)
		{
			fprintf(OutText,"%8.5f ",o->ModelView[y][x]);
		}
		fprintf(OutText,"\n");
	}
}

void LoadObject1Chunk(FILE *f, int BaseID, int TextureBase, bool HaveStoredColorInfo)
{
	LoadObjectChunk(f,BaseID,TextureBase,HaveStoredColorInfo);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].ZMask,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].AEpsilon,1,1,f);
}

void LoadObject2Chunk(FILE *f, int BaseID, int TextureBase, bool HaveStoredColorInfo)
{
	LoadObject1Chunk(f,BaseID,TextureBase,HaveStoredColorInfo);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Shading,4,1,f);
	for (int x=0; x<ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonNum; x++)
	{
		switch (ActualScene->ObjectList[ActualScene->ObjectNum-1].Shading)
		{
		case aDDict_FLATSHADE:
			ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonList[x].CurrentShading=aDDict_FLATSHADE;
			break;
		case aDDict_GOURAUDSHADE:
			ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonList[x].CurrentShading=aDDict_GOURAUDSHADE;
			break;
		}
	}
	//ActualScene->ObjectList[MappedObject].CalculateNormals();
	//ActualScene->ObjectList[MappedObject].CreateDisplayList();
}

void LoadObject3Chunk(FILE *f, int BaseID, int TextureBase, bool HaveStoredColorInfo)
{
	LoadObject2Chunk(f,BaseID,TextureBase,HaveStoredColorInfo);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Backface,1,1,f);
	fread(&ActualScene->ObjectList[ActualScene->ObjectNum-1].Backfront,1,1,f);
}

void LoadObject4Chunk(FILE *f, int BaseID, int TextureBase, bool HaveStoredColorInfo)
{
	LoadObject3Chunk(f,BaseID,TextureBase,HaveStoredColorInfo);
	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive==aDDict_SUPERSHAPE)
	{
		SUPERSHAPE *s=new SUPERSHAPE;
		fread(s,sizeof(SUPERSHAPE),1,f);
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1=(int)s;
		GeneratePrimitive(&ActualScene->ObjectList[ActualScene->ObjectNum-1],ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param1,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param2,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param3,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param4,ActualScene->ObjectList[ActualScene->ObjectNum-1].Param5);
		for (int x=0; x<ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonNum; x++)
		{
			switch (ActualScene->ObjectList[ActualScene->ObjectNum-1].Shading)
			{
			case aDDict_FLATSHADE:
				ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonList[x].CurrentShading=aDDict_FLATSHADE;
				break;
			case aDDict_GOURAUDSHADE:
				ActualScene->ObjectList[ActualScene->ObjectNum-1].PolygonList[x].CurrentShading=aDDict_GOURAUDSHADE;
				break;
			}
		}
	}
	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive!=aDDict_STORED)
	{
		ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateNormals();
	}
	ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateTextureCoordinates();
	ActualScene->ObjectList[ActualScene->ObjectNum-1].CreateDisplayList();
}

void LoadObject5Chunk(FILE *f, int BaseID, int TextureBase)
{
	LoadObject4Chunk(f,BaseID,TextureBase,true);
}


void LoadPolySelectChunk(FILE *f, int BaseID)
{
	char *Name=ReadASCIIZ(f);
	int i;
	fread(&i,4,1,f);
	PolySelections->AddItem(Name,new ITEMLIST(),GUI_SelectPolySelection,GUI_Rename,nop);
	PolySelectInterface->ItemList[0].ItemNum++;
	PolySelections->Last->ID=i;
}

void LoadBrushChunk(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	PVSelect *sel=new PVSelect;
	fread(sel,sizeof(PVSelect),1,f);
	((ITEMLIST*)PolySelections->Last->Data)->AddItem(Name,sel,GUI_SelectOnlyThisSelection,GUI_Rename,nop);
}

void LoadWorldChunk(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	WORLD *w=new WORLD;
	if (Worlds)
	{
		WorldScroller->ParentItem->SelectedItem++;
		WorldScroller->ParentItem->ItemNum++;
		LastWorld->Next=w;
		LastWorld=w;
	}
	else
	{
		WorldScroller->ParentItem->ItemNum++;
		Worlds=w;
		LastWorld=w;
	}
	ActualWorld=LastWorld;
	w->Name=Name;
	fread(&w->ID,1,4,f);
	fread(&w->Camera,1,sizeof(CAMERA)-16,f);
	fread(w->Lights,8,sizeof(LIGHT),f);
	w->Lighting=true;

	LastWorld->ItemList=new ITEMLIST();
	ITEM *WorldData=LastWorld->ItemList->AddItem("WORLD DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(Name)+51];
	memset(NameString,0,strlen(Name)+51);
	_snprintf(NameString,strlen(Name)+51,"%s",Name);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);
	NameString=new char[10];
	memset(NameString,0,10);
	_snprintf(NameString,10,"ID: %d",LastWorld->ID);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);

	LastWorld->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
	ITEM *LightList=LastWorld->ItemList->AddItem("LIGHTS",NULL,nop,nop,nop);
	LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
	LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
	LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
	LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
	LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
	LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
	LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
	LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
	
	LastWorld->ItemList->AddItem("--------SCENES-------",NULL,Scroller_SelectItem,nop,nop);
	char *NewLine=new char[strlen(Name)+51];
	memset(NewLine,0,strlen(Name)+51);
	_snprintf(NewLine,strlen(Name)+51,"%d: %s",LastWorld->ID,Name);
	WorldListScroller->AddItem(NewLine,LastWorld,Scroller_SelectOnlyThisWorld,GUI_RenameWorld,nop);
	SetEnvironmentToActualWorld();
	AddSWorldInterface->ParentItem->ItemNum++;
}

void LoadWorldChunk1(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	WORLD *w=new WORLD;
	if (Worlds)
	{
		WorldScroller->ParentItem->SelectedItem++;
		WorldScroller->ParentItem->ItemNum++;
		LastWorld->Next=w;
		LastWorld=w;
	}
	else
	{
		WorldScroller->ParentItem->ItemNum++;
		Worlds=w;
		LastWorld=w;
	}
	ActualWorld=LastWorld;
	w->Name=Name;
	fread(&w->ID,1,4,f);
	fread(&w->Camera,1,sizeof(CAMERA)-16,f);
	fread(w->Lights,8,sizeof(LIGHT),f);
	fread(&w->Fog,1,1,f);
	fread(w->FogCol,4,1,f);

	w->FogStart=0;
	w->FogEnd=500;
	w->Lighting=true;

	LastWorld->ItemList=new ITEMLIST();
	ITEM *WorldData=LastWorld->ItemList->AddItem("WORLD DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(Name)+51];
	memset(NameString,0,strlen(Name)+51);
	_snprintf(NameString,strlen(Name)+51,"%s",Name);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);
	NameString=new char[10];
	memset(NameString,0,10);
	_snprintf(NameString,10,"ID: %d",LastWorld->ID);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);

	LastWorld->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
	ITEM *LightList=LastWorld->ItemList->AddItem("LIGHTS",NULL,nop,nop,nop);
	LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
	LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
	LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
	LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
	LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
	LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
	LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
	LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
	
	LastWorld->ItemList->AddItem("--------SCENES-------",NULL,Scroller_SelectItem,nop,nop);
	char *NewLine=new char[strlen(Name)+51];
	memset(NewLine,0,strlen(Name)+51);
	_snprintf(NewLine,strlen(Name)+51,"%d: %s",LastWorld->ID,Name);
	WorldListScroller->AddItem(NewLine,LastWorld,Scroller_SelectOnlyThisWorld,GUI_RenameWorld,nop);
	SetEnvironmentToActualWorld();
	AddSWorldInterface->ParentItem->ItemNum++;
}

void LoadWorldChunk2(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	WORLD *w=new WORLD;
	if (Worlds)
	{
		WorldScroller->ParentItem->SelectedItem++;
		WorldScroller->ParentItem->ItemNum++;
		LastWorld->Next=w;
		LastWorld=w;
	}
	else
	{
		WorldScroller->ParentItem->ItemNum++;
		Worlds=w;
		LastWorld=w;
	}
	ActualWorld=LastWorld;
	w->Name=Name;
	fread(&w->ID,1,4,f);
	fread(&w->Camera,1,sizeof(CAMERA)-16,f);
	fread(w->Lights,8,sizeof(LIGHT),f);
	fread(&w->Fog,1,1,f);
	fread(w->FogCol,3,1,f);
	fread(&w->FogStart,4,1,f);
	fread(&w->FogEnd,4,1,f);
	fread(&w->Lighting,1,1,f);

	LastWorld->ItemList=new ITEMLIST();
	ITEM *WorldData=LastWorld->ItemList->AddItem("WORLD DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(Name)+51];
	memset(NameString,0,strlen(Name)+51);
	_snprintf(NameString,strlen(Name)+51,"%s",Name);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);
	NameString=new char[10];
	memset(NameString,0,10);
	_snprintf(NameString,10,"ID: %d",LastWorld->ID);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);

	LastWorld->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
	ITEM *LightList=LastWorld->ItemList->AddItem("LIGHTS",NULL,nop,nop,nop);
	LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
	LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
	LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
	LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
	LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
	LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
	LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
	LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
	
	LastWorld->ItemList->AddItem("--------SCENES-------",NULL,Scroller_SelectItem,nop,nop);
	char *NewLine=new char[strlen(Name)+51];
	memset(NewLine,0,strlen(Name)+51);
	_snprintf(NewLine,strlen(Name)+51,"%d: %s",LastWorld->ID,Name);
	WorldListScroller->AddItem(NewLine,LastWorld,Scroller_SelectOnlyThisWorld,GUI_RenameWorld,nop);
	SetEnvironmentToActualWorld();
	AddSWorldInterface->ParentItem->ItemNum++;
}


SCENE *FindSceneByID(int ID)
{
	for (SCENE *s=Scenes; s; s=s->Next)
	{
		if (s->ID==ID) return s;
	}
	return NULL;
}

WORLD *FindWorldByID(int ID)
{
	for (WORLD *s=Worlds; s; s=s->Next)
	{
		if (s->ID==ID) return s;
	}
	return NULL;
}


void LoadWorldObjectChunk(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	ActualWorld->AddObject();
	WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];

	//fread(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Selected,1,1,f);
	//ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Data=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
	
	MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
	MainLayouterScroller->ParentItem->ItemNum++;
	fread(&w->ID,1,4,f);
	fread(&w->ParentID,1,4,f);

	w->Parent=ActualWorld->FindObjectByID(w->ParentID);

	//char *ExtendedName=new char[60];
	//sprintf(ExtendedName,"%d:%s(%d)\0",w->ID,Name,w->ParentID);
	ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ListItem->Name=Name;
	
	fread(&w->Primitive,1,4,f);
	fread(&w->PosData,1,sizeof(RST),f);
	w->StoredPos=w->PosData;
	if (w->PosData.Quaternion.s==0) w->PosData.Quaternion.s=1;
	
	RST2Matrix(&w->PosData,w->ModelView,1);
	//RST2Matrix(&w->PosData,w->TransformBuffer);
	w->PosDataBuffer=w->PosData;
	
	fread(&w->SRCBlend,1,sizeof(GLuint),f);
	fread(&w->DSTBlend,1,sizeof(GLuint),f);	
	int a;
	fread(&a,1,4,f);
	if (w->Primitive==aDDict_SCENE)
	w->Data=FindSceneByID(a);
	else if (w->Primitive==aDDict_SUBSCENE) w->Data=FindWorldByID(a);
	w->TargetWorldID=a;
	fread(&w->Textured,1,1,f);
	ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material=new MATERIAL;
	fread(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material->TextureID,4,1,f);
	fread(&ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material->SlotNumber,4,1,f);
	ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material=FindMaterial(ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material->TextureID,ActualWorld->ObjectList[ActualWorld->ObjectNum-1].Material->SlotNumber);
	w->EmitterData=new EMITTERDATA();
	fread(&w->EmitterData->Head,1,1,f);
	fread(&w->EmitterData->Tail,1,1,f);

	fread(&w->EmitterData->DefaultAge,4,1,f);
	fread(&w->EmitterData->AgeChaos,4,1,f);
	fread(&w->EmitterData->ParticlesPerFrame,4,1,f);
	fread(&w->EmitterData->MaxParticles,4,1,f);

	fread(&w->EmitterData->ObjectHead,1,1,f);
	fread(&w->EmitterData->TailLength,1,1,f);
	fread(&w->EmitterData->TailLength2,1,1,f);

	fread(&w->EmitterData->ObjectHeadSceneID,4,1,f);

	fread(w->EmitterData->Color1,4,5,f);
	fread(w->EmitterData->Color2,4,5,f);

	fread(&w->EmitterData->Param1,4,1,f);
	fread(&w->EmitterData->Param2,4,1,f);
	fread(&w->EmitterData->Size,4,1,f);

	w->EmitterData->Rotspeed=0;
	w->EmitterData->RotChaos=0;
	w->EmitterData->CamFade=0;

	switch (w->Primitive)
	{
	case aDDict_SPHEREEMITTER:
	case aDDict_CUBEEMITTER:
	case aDDict_CYLINDEREMITTER:
		{
			//ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles=10;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->TailInfo=false;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->FixedUp=false;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->RandRotate=false;
			ActualWorld->ParticleCount++;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticleNumBuffer=ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->ParticlesPerFrame;
			ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles=new PARTICLE[ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles];
			if (ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Tail)
			{
				for (int x=0; x<ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->MaxParticles; x++) ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->Particles[x].Tail=new VECTOR3[256];
				ActualWorld->ObjectList[ActualWorld->ObjectNum-1].EmitterData->TailInfo=true;
			}
			break;
		}
	case aDDict_SUBSCENE:
		ActualWorld->SubWorldCount++;
		break;
	default:
		ActualWorld->ObjectCount++;
	}
	w->Color.dw=0xffffffff;
}

void LoadWorldObjectChunk1(FILE *f)
{
	LoadWorldObjectChunk(f);
	WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
	fread(&w->EmitterData->Rotspeed,4,1,f);
	fread(&w->EmitterData->RotChaos,4,1,f);
	fread(&w->EmitterData->CamFade,4,1,f);
	w->Color.dw=0xffffffff;
	w->ZMask=false;
	w->AEpsilon=0;
	w->EmitterData->Speed=0;
	w->EmitterData->SpeedChaos=0;
	w->EmitterData->DirChaos=0;
	w->EmitterData->Dir.x=0;
	w->EmitterData->Dir.y=0;
	w->EmitterData->Dir.z=0;
	w->ListItem->Selected=0;
}

void LoadWorldObjectChunk2(FILE *f)
{
	LoadWorldObjectChunk1(f);
	WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
	fread(&w->Color,4,1,f);
}

void LoadWorldObjectChunk3(FILE *f)
{
	LoadWorldObjectChunk2(f);
	WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
	fread(&w->ZMask,1,1,f);
	fread(&w->AEpsilon,1,1,f);
}

void LoadWorldObjectChunk4(FILE *f)
{
	LoadWorldObjectChunk3(f);
	WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
	fread(&w->EmitterData->Speed,4,1,f);
	fread(&w->EmitterData->SpeedChaos,4,1,f);
	fread(&w->EmitterData->DirChaos,4,1,f);
	fread(w->EmitterData->Dir.a,12,1,f);
	fread(&w->ListItem->Selected,1,1,f);
}

void LoadWorldObjectChunk5(FILE *f)
{
	LoadWorldObjectChunk4(f);
	WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
	fread(&w->EmitterData->RandRotate,1,1,f);
	fread(&w->EmitterData->FixedUp,1,1,f);
}

void LoadSplineData(FILE *f, SPLINE **s)
{
	*s=new SPLINE();
	int k;
	fread(&k,1,4,f);
	fread(&(*s)->Interpolation,1,4,f);
	for (int x=0; x<k; x++)
	{
		float a,b;
		fread(&a,1,4,f); //time
		fread(&b,1,4,f); //value
		(*s)->AddKey(b,a);
	}
	(*s)->InitVectors();
}

void LoadWorldAnimChunk(FILE *f)
{
	ActualWorld->AddAnimation("");
	ANIMDATA *w=&ActualWorld->Animations[ActualWorld->AnimNum-1];
	fread(&w->AnimID,1,4,f);
	char *Name=ReadASCIIZ(f);
	w->Name=Name;
	ActualWorld->AnimList->AddItem(Name,&ActualWorld->Animations[ActualWorld->AnimNum-1],GUI_SelectOnlyThisAnim,GUI_Rename,nop);
	ActualWorld->Animations[ActualWorld->AnimNum-1].ListItem=ActualWorld->AnimList->Last;
	for (int x=0; x<8; x++)
	{
		LoadSplineData(f,&w->Lights[x].ARed);
		LoadSplineData(f,&w->Lights[x].AGreen);
		LoadSplineData(f,&w->Lights[x].ABlue);
		LoadSplineData(f,&w->Lights[x].Red);
		LoadSplineData(f,&w->Lights[x].Green);
		LoadSplineData(f,&w->Lights[x].Blue);
		LoadSplineData(f,&w->Lights[x].Lit);
	}
}

void LoadWorldCamAnimChunk(FILE *f)
{
	char *Name=ReadASCIIZ(f);
	ActualWorld->AddCamera(Name);
	CAMSPLINES *w=&ActualWorld->CamAnims[ActualWorld->CamNum-1];
	ActualWorld->CameraList->AddItem(Name,&ActualWorld->CamAnims[ActualWorld->CamNum-1],GUI_SelectOnlyThisCam,GUI_Rename,nop);
	fread(&w->CamID,1,4,f);

	LoadSplineData(f,&w->Eyex);
	LoadSplineData(f,&w->Eyey);
	LoadSplineData(f,&w->Eyez);
	LoadSplineData(f,&w->Trgx);
	LoadSplineData(f,&w->Trgy);
	LoadSplineData(f,&w->Trgz);
	LoadSplineData(f,&w->Fov);
	LoadSplineData(f,&w->Roll);
}

int ObjCounter;
int AnimCounter;

void LoadWorldObjAnimChunk(FILE *f)
{
	DEFAULTOBJECTSPLINES *w=&ActualWorld->ObjectList[ObjCounter].Animations[AnimCounter];
	LoadSplineData(f,&w->Red);
	LoadSplineData(f,&w->Green);
	LoadSplineData(f,&w->Blue);
	LoadSplineData(f,&w->Alpha);
	LoadSplineData(f,&w->Size);
	LoadSplineData(f,&w->Prtfrme);
	LoadSplineData(f,&w->AnimTime);
	LoadSplineData(f,&w->Posx);
	LoadSplineData(f,&w->Posy);
	LoadSplineData(f,&w->Posz);
	LoadSplineData(f,&w->Rotx);
	LoadSplineData(f,&w->Roty);
	LoadSplineData(f,&w->Rotz);
	LoadSplineData(f,&w->Sclx);
	LoadSplineData(f,&w->Scly);
	LoadSplineData(f,&w->Sclz);
}

void LoadWorldObjAnimChunk2(FILE *f)
{
	DEFAULTOBJECTSPLINES *w=&ActualWorld->ObjectList[ObjCounter].Animations[AnimCounter];
	LoadSplineData(f,&w->Red);
	LoadSplineData(f,&w->Green);
	LoadSplineData(f,&w->Blue);
	LoadSplineData(f,&w->Alpha);
	LoadSplineData(f,&w->Size);
	LoadSplineData(f,&w->Prtfrme);
	LoadSplineData(f,&w->AnimTime);
	LoadSplineData(f,&w->Posx);
	LoadSplineData(f,&w->Posy);
	LoadSplineData(f,&w->Posz);
	LoadSplineData(f,&w->Rotx);
	LoadSplineData(f,&w->Roty);
	LoadSplineData(f,&w->Rotz);
	LoadSplineData(f,&w->Rotw);
	LoadSplineData(f,&w->Sclx);
	LoadSplineData(f,&w->Scly);
	LoadSplineData(f,&w->Sclz);
	Spline_QuaternionGetVectors(w->Rotx,w->Roty,w->Rotz,w->Rotw);
}

void LoadWorldObjAnimChunk3(FILE *f)
{
	DEFAULTOBJECTSPLINES *w=&ActualWorld->ObjectList[ObjCounter].Animations[AnimCounter];
	LoadSplineData(f,&w->Red);
	LoadSplineData(f,&w->Green);
	LoadSplineData(f,&w->Blue);
	LoadSplineData(f,&w->Alpha);
	LoadSplineData(f,&w->Size);
	LoadSplineData(f,&w->Prtfrme);
	LoadSplineData(f,&w->AnimTime);
	LoadSplineData(f,&w->AnimID);
	LoadSplineData(f,&w->Posx);
	LoadSplineData(f,&w->Posy);
	LoadSplineData(f,&w->Posz);
	LoadSplineData(f,&w->Rotx);
	LoadSplineData(f,&w->Roty);
	LoadSplineData(f,&w->Rotz);
	LoadSplineData(f,&w->Rotw);
	LoadSplineData(f,&w->Sclx);
	LoadSplineData(f,&w->Scly);
	LoadSplineData(f,&w->Sclz);
	Spline_QuaternionGetVectors(w->Rotx,w->Roty,w->Rotz,w->Rotw);
}


void LoadEventChunk(FILE *f)
{
	if (!EventList)
	{
		EventList=new EVENT;
		memset(EventList,0,sizeof(EVENT));
	}
	else
	{
		EVENT *Buffer=new EVENT;
		memset(Buffer,0,sizeof(EVENT));
		Buffer->Next=EventList;
		EventList=Buffer;
	}
	EVENT *w=EventList;
	
	
	fread(w->Name,1,30,f);
	fread(&w->Num,1,4,f);
	fread(&w->StartFrame,1,4,f);
	fread(&w->EndFrame,1,4,f);
	fread(&w->EventType,1,4,f);
	fread(&w->Pass,1,4,f);
	fread(&w->iParameters,10,4,f);
	fread(&w->fParameters,10,4,f);
	fread(&w->sx1,1,4,f);
	fread(&w->sy1,1,4,f);
	fread(&w->sx2,1,4,f);
	fread(&w->sy2,1,4,f);
	fread(&w->ex1,1,4,f);
	fread(&w->ey1,1,4,f);
	fread(&w->ex2,1,4,f);
	fread(&w->ey2,1,4,f);

	ITEM *a=ScrollerFindItem((ITEMLIST*)EventParameterInterface->ParentItem->data,w->EventType);
	w->Effect=(EFFECT*)a->Data;
	if (((EFFECT*)a->Data)->DataSize)
	{
		w->EffectData=new char[((EFFECT*)a->Data)->DataSize];
		memset(w->EffectData,0,((EFFECT*)a->Data)->DataSize);
	}
	else w->EffectData=NULL;

	switch (w->EventType)
	{
		case 0: //NO EFFECT
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 1: //2D LAYER
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((LAYER2DTEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		case 2: //PARTICLE COUNT
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((PARTICLETEMPLATE*)w->EffectData)->World=a;
				}
				fread(&((PARTICLETEMPLATE*)w->EffectData)->CamID,4,1,f);
				fread(&((PARTICLETEMPLATE*)w->EffectData)->AnimID,4,1,f);
				((PARTICLETEMPLATE*)w->EffectData)->AnimEnd=1;
				((PARTICLETEMPLATE*)w->EffectData)->AnimStart=0;
			}
		break;
		case 3: //CLEAR
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 4: //RENDERTEXTURE
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 5: //FEEDBACK
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 7: //3D SCENE
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((DISPLAY3DTEMPLATE*)w->EffectData)->World=a;
				}
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamID,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimID,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamStart,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamEnd,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimStart,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimEnd,4,1,f);
				((DISPLAY3DTEMPLATE*)w->EffectData)->DontSaveCam=false;
			}
		break;
		case 8: //PARTICLE RESET
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((DISPLAY3DTEMPLATE*)w->EffectData)->World=a;
				}
			}
		break;
		case 12: //GRIDFILLER
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((GRIDFILLERTEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		case 13: //CAVE
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((CAVETEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		default: //FEEDBACK
			fread(w->EffectData,1,w->Effect->DataSize,f);
	}
}

void LoadEventChunk1(FILE *f)
{
	if (!EventList)
	{
		EventList=new EVENT;
		memset(EventList,0,sizeof(EVENT));
	}
	else
	{
		EVENT *Buffer=new EVENT;
		memset(Buffer,0,sizeof(EVENT));
		Buffer->Next=EventList;
		EventList=Buffer;
	}
	EVENT *w=EventList;
	
	
	fread(w->Name,1,30,f);
	fread(&w->Num,1,4,f);
	fread(&w->StartFrame,1,4,f);
	fread(&w->EndFrame,1,4,f);
	fread(&w->EventType,1,4,f);
	fread(&w->Pass,1,4,f);
	fread(&w->iParameters,10,4,f);
	fread(&w->fParameters,10,4,f);
	fread(&w->sx1,1,4,f);
	fread(&w->sy1,1,4,f);
	fread(&w->sx2,1,4,f);
	fread(&w->sy2,1,4,f);
	fread(&w->ex1,1,4,f);
	fread(&w->ey1,1,4,f);
	fread(&w->ex2,1,4,f);
	fread(&w->ey2,1,4,f);

	ITEM *a=ScrollerFindItem((ITEMLIST*)EventParameterInterface->ParentItem->data,w->EventType);
	w->Effect=(EFFECT*)a->Data;
	if (((EFFECT*)a->Data)->DataSize)
	{
		w->EffectData=new char[((EFFECT*)a->Data)->DataSize];
		memset(w->EffectData,0,((EFFECT*)a->Data)->DataSize);
	}
	else w->EffectData=NULL;

	switch (w->EventType)
	{
		case 0: //NO EFFECT
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 1: //2D LAYER
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((LAYER2DTEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		case 2: //PARTICLE COUNT
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((PARTICLETEMPLATE*)w->EffectData)->World=a;
				}
				fread(&((PARTICLETEMPLATE*)w->EffectData)->CamID,4,1,f);
				fread(&((PARTICLETEMPLATE*)w->EffectData)->AnimID,4,1,f);
				((PARTICLETEMPLATE*)w->EffectData)->AnimEnd=1;
				((PARTICLETEMPLATE*)w->EffectData)->AnimStart=0;
			}
		break;
		case 3: //CLEAR
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 4: //RENDERTEXTURE
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 5: //FEEDBACK
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 7: //3D SCENE
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((DISPLAY3DTEMPLATE*)w->EffectData)->World=a;
				}
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamID,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimID,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamStart,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamEnd,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimStart,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimEnd,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->DontSaveCam,1,1,f);
			}
		break;
		case 8: //PARTICLE RESET
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((DISPLAY3DTEMPLATE*)w->EffectData)->World=a;
				}
			}
		break;
		case 12: //GRIDFILLER
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((GRIDFILLERTEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		case 13: //CAVE
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((CAVETEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		default: //FEEDBACK
			fread(w->EffectData,1,w->Effect->DataSize,f);
	}
}

void LoadEventChunk2(FILE *f)
{
	if (!EventList)
	{
		EventList=new EVENT;
		memset(EventList,0,sizeof(EVENT));
	}
	else
	{
		EVENT *Buffer=new EVENT;
		memset(Buffer,0,sizeof(EVENT));
		Buffer->Next=EventList;
		EventList=Buffer;
	}
	EVENT *w=EventList;
	
	
	fread(w->Name,1,30,f);
	fread(&w->Num,1,4,f);
	fread(&w->StartFrame,1,4,f);
	fread(&w->EndFrame,1,4,f);
	fread(&w->EventType,1,4,f);
	fread(&w->Pass,1,4,f);
	fread(&w->iParameters,10,4,f);
	fread(&w->fParameters,10,4,f);
	fread(&w->sx1,1,4,f);
	fread(&w->sy1,1,4,f);
	fread(&w->sx2,1,4,f);
	fread(&w->sy2,1,4,f);
	fread(&w->ex1,1,4,f);
	fread(&w->ey1,1,4,f);
	fread(&w->ex2,1,4,f);
	fread(&w->ey2,1,4,f);

	ITEM *a=ScrollerFindItem((ITEMLIST*)EventParameterInterface->ParentItem->data,w->EventType);
	w->Effect=(EFFECT*)a->Data;
	if (((EFFECT*)a->Data)->DataSize)
	{
		w->EffectData=new char[((EFFECT*)a->Data)->DataSize];
		memset(w->EffectData,0,((EFFECT*)a->Data)->DataSize);
	}
	else w->EffectData=NULL;

	switch (w->EventType)
	{
		case 0: //NO EFFECT
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 1: //2D LAYER
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((LAYER2DTEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		case 2: //PARTICLE COUNT
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((PARTICLETEMPLATE*)w->EffectData)->World=a;
				}
				fread(&((PARTICLETEMPLATE*)w->EffectData)->CamID,4,1,f);
				fread(&((PARTICLETEMPLATE*)w->EffectData)->AnimID,4,1,f);
				fread(&((PARTICLETEMPLATE*)w->EffectData)->AnimStart,4,1,f);
				fread(&((PARTICLETEMPLATE*)w->EffectData)->AnimEnd,4,1,f);
			}
		break;
		case 3: //CLEAR
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 4: //RENDERTEXTURE
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 5: //FEEDBACK
			fread(w->EffectData,1,w->Effect->DataSize,f);
		break;
		case 7: //3D SCENE
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((DISPLAY3DTEMPLATE*)w->EffectData)->World=a;
				}
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamID,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimID,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamStart,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->CamEnd,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimStart,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->AnimEnd,4,1,f);
				fread(&((DISPLAY3DTEMPLATE*)w->EffectData)->DontSaveCam,1,1,f);
			}
		break;
		case 8: //PARTICLE RESET
			{
				int i;
				fread(&i,1,4,f); //world
				for (WORLD *a=Worlds; a; a=a->Next)
				{
					if (a->ID==i) ((DISPLAY3DTEMPLATE*)w->EffectData)->World=a;
				}
			}
		break;
		case 12: //GRIDFILLER
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((GRIDFILLERTEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		case 13: //CAVE
			{
				fread(w->EffectData,1,w->Effect->DataSize-4,f);
				int Mat=0;
				int Slot=0;
				fread(&Mat,1,3,f); //material
				fread(&Slot,1,1,f);
				((CAVETEMPLATE*)w->EffectData)->Texture=FindMaterial(Mat,Slot);
			}
		break;
		default: //FEEDBACK
			fread(w->EffectData,1,w->Effect->DataSize,f);
	}
}


void LoadScene(char *FileName)
{
	GuiDrawTimer=timeGetTime()-101;
	for (unsigned int x=0; x<strlen(FileName); x++) FileName[x]=tolower(FileName[x]);
	if (!strstr(FileName,".wdo\0"))
	{
	if (!strstr(FileName,".ase\0") && !strstr(FileName,".obj\0"))
	{
		FILE *f;
		f=fopen(FileName,"rb");

		char FileID[9];
		memset(FileID,0,9);
		fread(FileID,8,1,f);
		if (strcmp(FileID,"aDDict2S\0")) ErrorMessage="THE FILE IS NOT AN A.D.D.I.C.T.2 SCENE FILE!";
		else
		{
			ChunkHeader Chunk;
			memset(&Chunk,0,sizeof(Chunk));

			int LastTextureID=0;
			int LastObjectID=ActualScene->ObjectList[ActualScene->ObjectNum-1].ID+1;
			int PolySelectCount=0;
			for (ITEM *i=PolySelections->List;i;i=i->Next) PolySelectCount++;
			//PolySelectCount++;

			for (TEXTURE *t=TextureList; t; t=t->Next) LastTextureID=max(LastTextureID,t->Id);
			LastTextureID++;

			int TexCount=0;
			int TexCount2=0;
			//EventList=NULL;

			while (Chunk.ID!=END0)
			{
				fread(&Chunk,8,1,f);
				if (Chunk.ID==TEX0 || Chunk.ID==TEX1) TexCount++;
				fseek(f,Chunk.Size,SEEK_CUR);
			}

			Chunk.ID=0;
			fseek(f,8,SEEK_SET);

			StatusBar3->ParentItem->ActiveBranch=true;
			while (Chunk.ID!=END0)
			{
				fread(&Chunk,8,1,f);
				switch (Chunk.ID)
				{
				case TEX0:
					StatusBar3->ItemList->Text="GENERATING TEXTURES... ";
					LoadTextureChunk(f,TexCount,TexCount2++,LastTextureID);
					GUIReDraw();
					break;
				case TEX1:
					StatusBar3->ItemList->Text="GENERATING TEXTURES... ";
					LoadTexture1Chunk(f,TexCount,TexCount2++,LastTextureID);
					GUIReDraw();
					break;
				case OBJ3:
					LoadObject3Chunk(f,LastObjectID,LastTextureID,false);
	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive!=aDDict_STORED)
	{
		ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateNormals();
	}
	ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateTextureCoordinates();
	ActualScene->ObjectList[ActualScene->ObjectNum-1].CreateDisplayList();
					break;
				case OBJ4:
					LoadObject4Chunk(f,LastObjectID,LastTextureID,false);
					break;
				case OBJ5:
					LoadObject5Chunk(f,LastObjectID,LastTextureID);
					break;
				case VXS0:
				case PLS0:
					LoadPolySelectChunk(f,PolySelectCount);
					break;
				case BRS0:
					LoadBrushChunk(f);
					break;
				case MAP0:
					LoadMapxform(f,LastObjectID,LastTextureID,PolySelectCount);
					break;
				case CLN0:
					LoadClone(f,LastObjectID);
					break;
				case MSH0:
					LoadMeshSmooth(f,LastObjectID);
					break;
				case MSH1:
					LoadMeshSmooth2(f,LastObjectID);
					break;
				default:
					fseek(f,Chunk.Size,SEEK_CUR);
				}
			}
			StatusBar3->ItemList->Text="BUILDING MATERIALS... ";
			UndoStatus2=0;
			GUIReDraw();
			BuildMaterialList();

			if (PolySelections->List)
			PolySelectInterface->ItemList[4].data=CurrentPolySelection=(ITEMLIST*)PolySelections->List->Data;
			StatusBar2->ParentItem->ActiveBranch=false;
			/*if (ActualTexture)*/ SetEnvironmentToActualTexture();

			if ((ITEMLIST*)PolySelections->List)
				PolySelectInterface->ItemList[4].ItemNum=((ITEMLIST*)PolySelections->List->Data)->ItemNum;
			SetEnvironmentToActualScene();

		}

		fclose(f);
	}
	else ParseASE(FileName);
	}
	else loadwdo(*ActualScene,FileName);
}

void ClearTextureList()
{
	for (TEXTURE *t=TextureList; t; t=t->Next)
	{
		for (int x=0; x<8; x++) 
		{
			delete[] t->Materials[x].ImageData.Data;
			glDeleteTextures(1,&t->Materials[x].ImageData.TextureHandle);
			delete[] t->Layers[x].Data;
			/*if (glIsTexture(t->Layers[x].TextureHandle))
			glDeleteTextures(1,&t->Layers[x].TextureHandle);*/
		}
	}
	TextureList=NULL;
	TextureListScroller->ItemNum=0;
	TextureListScroller->Last=NULL;
	TextureListScroller->List=NULL;
}

void ClearScenes()
{
	for (SCENE *s=Scenes; s; s=s->Next)
	{
		for (int x=0; x<s->ObjectNum; x++)
		{
			glDeleteLists(s->ObjectList[x].DisplayList,1);
			glDeleteLists(s->ObjectList[x].WireDisplayList,1);
		}
		delete[] s->ObjectList;
	}

	Scenes=NULL;
	LastScene=NULL;
}

void LoadProject(char *FileName)
{
	GuiDrawTimer=timeGetTime()-101;
	memset(texops,0,sizeof(int)*100);
	FILE *f;
	f=fopen(FileName,"rb");

	char FileID[9];
	memset(FileID,0,9);
	fread(FileID,8,1,f);
	if (strcmp(FileID,"aDDict2P\0")) ErrorMessage="THE FILE IS NOT AN A.D.D.I.C.T.2 PROJECT FILE!";
	else
	{
		ActualTexture=NULL;
		ClearTextureList();
		TextureSelect->ItemList[0].SelectedItem=0;
		SceneScroller->ParentItem->SelectedItem=0;
		SceneScroller->ParentItem->ItemNum=0;
		AddModellInterface->ParentItem->ItemNum=0;
		AddModellInterface->ParentItem->SelectedItem=0;
		SceneListScroller->ItemNum=0;
		SceneListScroller->List=NULL;
		SceneListScroller->Last=NULL;
		WorldListScroller->ItemNum=0;
		WorldListScroller->Last=NULL;
		WorldListScroller->List=NULL;
		PolySelections->ItemNum=0;
		PolySelections->List=NULL;
		PolySelections->Last=NULL;
		ChunkHeader Chunk;
		memset(&Chunk,0,sizeof(Chunk));
		ClearScenes();
		Worlds=NULL;
		LastWorld=NULL;
		SelectedEvent=NULL;
		PolySelectInterface->ItemList[0].ItemNum=0;
		PolySelectInterface->ItemList[0].SelectedItem=0;
		ActualWorld=NULL;
		ActualScene=NULL;
		SelectedEvent=NULL;
		EventParameterInterface->SetActiveBranch(-1);
		EventParameterInterface->ParentItem->SelectedItem=0;
		AddSWorldInterface->ParentItem->ItemNum=0;
		AddSWorldInterface->ParentItem->SelectedItem=0;
		WorldScroller->ParentItem->ItemNum=0;
		WorldScroller->ParentItem->SelectedItem=0;

		int TexCount=0;
		int TexCount2=0;
		int SceneCount=0;
		int SceneCount2=0;
		int WorldCount=0;
		int WorldCount2=0;
		int EventCount=0;
		int EventCount2=0;
		EventList=NULL;

		while (Chunk.ID!=END0)
		{
			fread(&Chunk,8,1,f);
			if (Chunk.ID==TEX0 || Chunk.ID==TEX1) TexCount++;
			if (Chunk.ID==SCN0 || Chunk.ID==SCN1) SceneCount++;
			if (Chunk.ID==WRL0 || Chunk.ID==WRL1 || Chunk.ID==WRL2) WorldCount++;
			if (Chunk.ID==EVT0 || Chunk.ID==EVT1 || Chunk.ID==EVT2) EventCount++;
			fseek(f,Chunk.Size,SEEK_CUR);
		}

		Chunk.ID=0;
		fseek(f,8,SEEK_SET);

		StatusBar2->ParentItem->ActiveBranch=true;
		while (Chunk.ID!=END0)
		{
			fread(&Chunk,8,1,f);
			switch (Chunk.ID)
			{
			case TEX0:
				StatusBar2->ItemList->Text="GENERATING TEXTURES... ";
				LoadTextureChunk(f,TexCount,TexCount2++,0);
				GUIReDraw();
				break;
			case TEX1:
				StatusBar2->ItemList->Text="GENERATING TEXTURES... ";
				LoadTexture1Chunk(f,TexCount,TexCount2++,0);
				GUIReDraw();
				break;
			case SCN0:
				StatusBar2->ItemList->Text="GENERATING SCENES... ";
				LoadSceneChunk(f);				
				SceneCount2++;
				UndoStatus2=SceneCount2/(float)SceneCount;
				GUIReDraw();
				break;
			case SCN1:
				StatusBar2->ItemList->Text="GENERATING SCENES... ";
				LoadSceneChunk1(f);
				SceneCount2++;
				UndoStatus2=SceneCount2/(float)SceneCount;
				GUIReDraw();
				break;
			case OBJ0:
				LoadObjectChunk(f,0,0,false);
				break;
			case OBJ1:
				LoadObject1Chunk(f,0,0,false);
				break;
			case OBJ2:
				LoadObject2Chunk(f,0,0,false);
				break;
			case OBJ3:
				LoadObject3Chunk(f,0,0,false);
	if (ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive!=aDDict_STORED)
	{
		ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateNormals();
	}
	ActualScene->ObjectList[ActualScene->ObjectNum-1].CalculateTextureCoordinates();
	ActualScene->ObjectList[ActualScene->ObjectNum-1].CreateDisplayList();
				break;
			case OBJ4:
				LoadObject4Chunk(f,0,0,false);
				break;
			case OBJ5:
				LoadObject5Chunk(f,0,0);
				break;
			case VXS0:
			case PLS0:
				LoadPolySelectChunk(f,0);
				break;
			case BRS0:
				LoadBrushChunk(f);
				break;
			case WRL0:
				StatusBar2->ItemList->Text="GENERATING WORLDS... ";
				LoadWorldChunk(f);
				ObjCounter=-1;
				AnimCounter=0;
				WorldCount2++;
				UndoStatus2=WorldCount2/(float)WorldCount;
				GUIReDraw();
				break;
			case WRL1:
				StatusBar2->ItemList->Text="GENERATING WORLDS... ";
				LoadWorldChunk1(f);
				ObjCounter=-1;
				AnimCounter=0;
				WorldCount2++;
				UndoStatus2=WorldCount2/(float)WorldCount;
				GUIReDraw();
				break;
			case WRL2:
				StatusBar2->ItemList->Text="GENERATING WORLDS... ";
				LoadWorldChunk2(f);
				ObjCounter=-1;
				AnimCounter=0;
				WorldCount2++;
				UndoStatus2=WorldCount2/(float)WorldCount;
				GUIReDraw();
				break;
			case WRO0:
				LoadWorldObjectChunk(f);
				ObjCounter++;
				AnimCounter=0;
				break;
			case WRO1:
				LoadWorldObjectChunk1(f);
				ObjCounter++;
				AnimCounter=0;
				break;
			case WRO2:
				LoadWorldObjectChunk2(f);
				ObjCounter++;
				AnimCounter=0;
				break;
			case WRO3:
				LoadWorldObjectChunk3(f);
				ObjCounter++;
				AnimCounter=0;
				break;
			case WRO4:
				LoadWorldObjectChunk4(f);
				ObjCounter++;
				AnimCounter=0;
				break;
			case WRO5:
				LoadWorldObjectChunk5(f);
				ObjCounter++;
				AnimCounter=0;
				break;
			case WRA0:
				LoadWorldAnimChunk(f);
				ObjCounter=-1;
				AnimCounter=0;
				break;
			case CAM0:
				LoadWorldCamAnimChunk(f);
				ObjCounter=-1;
				AnimCounter=0;
				break;
			case WOA0:
				LoadWorldObjAnimChunk(f);
				AnimCounter++;
				break;
			case WOA1:
				LoadWorldObjAnimChunk2(f);
				AnimCounter++;
				break;
			case WOA2:
				LoadWorldObjAnimChunk3(f);
				AnimCounter++;
				break;
			case EVT0:
				StatusBar2->ItemList->Text="LOADING EVENTS... ";
				LoadEventChunk(f);
				EventCount2++;
				UndoStatus2=EventCount2/(float)EventCount;
				GUIReDraw();
				break;
			case EVT1:
				StatusBar2->ItemList->Text="LOADING EVENTS... ";
				LoadEventChunk1(f);
				EventCount2++;
				UndoStatus2=EventCount2/(float)EventCount;
				GUIReDraw();
				break;
			case EVT2:
				StatusBar2->ItemList->Text="LOADING EVENTS... ";
				LoadEventChunk2(f);
				EventCount2++;
				UndoStatus2=EventCount2/(float)EventCount;
				GUIReDraw();
				break;
			case MAP0:
				LoadMapxform(f,0,0,0);
				break;
			case CLN0:
				LoadClone(f,0);
				break;
			case MSH0:
				LoadMeshSmooth(f,0);
				break;
			case MSH1:
				LoadMeshSmooth2(f,0);
				break;
			default:
				fseek(f,Chunk.Size,SEEK_CUR);

				/*SaveWorldChunk(f,w); *
				SaveWorldAnimChunk(f,&w->Animations[x]); *
				SaveWorldCamAnimChunk(f,&w->CamAnims[x]); *
				SaveWorldObjectChunk(f,&w->ObjectList[x]); *
				SaveWorldObjAnimChunk(f,&w->ObjectList[x].Animations[y]);*/
			}
		}
		StatusBar2->ItemList->Text="BUILDING MATERIALS... ";
		UndoStatus2=0;
		GUIReDraw();
		BuildMaterialList();

		StatusBar2->ItemList->Text="RESETTING EMITTERS... ";
		WorldCount2=0;
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
			WorldCount2++;
			UndoStatus2=WorldCount2/(float)WorldCount;
			GUIReDraw();
		}

		/*StatusBar2->ItemList->Text="SETTING WORLD SPLINES... ";
		WorldCount2=0;*/
		for (ActualWorld=Worlds; ActualWorld; ActualWorld=ActualWorld->Next)
		{
			for (int x=0; x<ActualWorld->ObjectNum; x++)
				if (ActualWorld->ObjectList[x].Primitive==aDDict_SUBSCENE)
				{
					if (!ActualWorld->ObjectList[x].Data) ActualWorld->ObjectList[x].Data=FindWorldByID(ActualWorld->ObjectList[x].TargetWorldID);
				}
			SetObjectSplines();
			SetEmitterSplines();
			SetSubWorldSplines();
			/*WorldCount2++;
			UndoStatus2=WorldCount2/(float)WorldCount;
			GUIReDraw();*/
		}
		ActualWorld=Worlds;
		if (Worlds && Worlds->Next) for (ActualWorld=Worlds; ActualWorld->Next; ActualWorld=ActualWorld->Next);
		if (PolySelections->List)
		PolySelectInterface->ItemList[4].data=CurrentPolySelection=(ITEMLIST*)PolySelections->List->Data;
		ArrangeEventList();
	    StatusBar2->ParentItem->ActiveBranch=false;
		SetActualWorldAnimListToMainAnimList();
		SetActualWorldCamListToMainCamList();
		/*if (ActualTexture)*/ SetEnvironmentToActualTexture();
		/*if (ActualScene)*/ SetEnvironmentToActualScene();
		/*if (ActualWorld)*/ SetEnvironmentToActualWorld();

		if ((ITEMLIST*)PolySelections->List)
			PolySelectInterface->ItemList[4].ItemNum=((ITEMLIST*)PolySelections->List->Data)->ItemNum;
	}

	fclose(f);
}

void __stdcall ImportProject(cINTERFACE* Interface, int ButtonId)
{
	ITEM *Selected=ReturnItem(FileList,*(int*)Interface->ItemList[ButtonId].data);
	if (Selected)
	{
		char* NewLine=new char[strlen(Selected->Name)+strlen(ProjectDir)+10];
		memset(NewLine,0,strlen(Selected->Name)+strlen(ProjectDir)+10);
		sprintf(NewLine,"%s/%s\0",ProjectDir,Selected->Name);

		LoadProject(NewLine);
		sprintf(NewLine,"%s\0",Selected->Name);
		LastFileName=NewLine;

		WaitLeftButton=true;
	}
}

void __stdcall ImportScene(cINTERFACE* Interface, int ButtonId)
{
	ITEM *Selected=ReturnItem(ModellFileList,*(int*)Interface->ItemList[ButtonId].data);
	if (ActualScene || (Selected && strstr(Selected->Name,".WDO\0")) || (Selected && strstr(Selected->Name,".wdo\0")))
	{
		if (Selected)
		{
			char* NewLine=new char[strlen(Selected->Name)+strlen(ProjectDir)+10];
			memset(NewLine,0,strlen(Selected->Name)+strlen(ProjectDir)+10);
			sprintf(NewLine,"%s/%s\0",SceneDir,Selected->Name);

			LoadScene(NewLine);

			WaitLeftButton=true;
		}
	}
	else ErrorMessage="CREATE A SCENE FIRST";
	StatusBar3->ParentItem->ActiveBranch=false;
}


void __stdcall GUI_ReadLine_LastFileName(cINTERFACE* Interface, int ButtonId)
{
	ReadLine=true;
	Interface->ItemList[ButtonId].Reading=true;
	Interface->ItemList[ButtonId].ReadLine=NULL;
	CommandLineReading=true;
	CommandLine=new char[strlen(LastFileName)+1];
	sprintf(CommandLine,"%s\0",LastFileName);
};


void BuildFileMenuGui(cINTERFACE *FileMenu)
{
	LastFileName=new char[10];
	memset(LastFileName,0,10);
	FileList=new ITEMLIST();
	Import=FileMenu->AddSubInterface();
	Import->AddItemList(758+6,130,222,30,FileList,"SELECT FILE:");
	Import->AddSkinnedButton(834,470,ImportButtonBig,"LOAD SELECTED PROJECT",ImportProject,nop,nop,nop);
	Import->ItemList[Import->ItemNum-1].data=&Import->ItemList[Import->ItemNum-5].SelectedItem;
	GUI_GetFileList(Import,0);
	FileMenu->AddSkinnedButton(834,486,ExportButtonBig,"EXPORT PROJECT",GUI_ReadLine_LastFileName,nop,nop,SaveProject);
#ifndef Public_Release
	FileMenu->AddSkinnedButton(834,502,MinimalExportButton,"EXPORT MINIMAL PROJECT",GUI_ReadLine,nop,nop,SaveMinimalProject);
#endif

	FileMenu->AddImage(126,170,Splash);

	StatusBar2=FileMenu->AddSubInterface();
	StatusBar2->AddFloatProgressBar(407,8,440,GUI_NONE,&UndoStatus2,0,1,0,"GENERATING TEXTURES... ","",nop,nop,nop,nop);
	StatusBar2->ParentItem->ActiveBranch=false;

	//FileMenu->AddIntDisplay(50,100,&MemUsage);
}