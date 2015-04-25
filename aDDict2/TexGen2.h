#ifndef __TexGen2__
#define __TexGen2__

#include "memtrack.h"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "aDDictMath.h"
#include "GUIItemList.h"

// DEFINE THIS IF YOU WANT TO OPTIMIZE TO SQUARE TEXTURES
#define __TEXGEN_SQUARE_TEXTURES__

// DEFINE THIS IF YOU WANT BILINEAR FILTERED TEXTURES
#define __TEXGEN_BILINEAR__

#pragma pack(1)

#define TEXGEN_Envmap 0
struct ENVMAPTEMPLATE
{
	unsigned char Size;
};

#define TEXGEN_Plasma 1
struct PLASMATEMPLATE
{
	unsigned char XSines;
	unsigned char YSines;
};

#define TEXGEN_Map 2
struct MAPTEMPLATE
{
	unsigned char XDataLayer;
	unsigned char XDataChannel;
	unsigned char XAmount;
	unsigned char YDataLayer;
	unsigned char YDataChannel;
	unsigned char YAmount;
};

#define TEXGEN_Blur 3
struct BLURTEMPLATE
{
	unsigned char Iterations;
	unsigned char SampleWidth;
};

#define TEXGEN_DirBlur 4
struct DIRBLURTEMPLATE
{
	unsigned char DataLayer;
	unsigned char DataChannel;
	unsigned char Depth;
};

#define TEXGEN_Text 5
struct TEXTTEMPLATE
{
	unsigned char Size;
	unsigned char X;
	unsigned char Y;
	unsigned char Bold;
	unsigned char Italic;
	unsigned char Font;
	unsigned char Spacing;
	char * Text;
};

#define TEXGEN_SubPlasma 6
struct SUBPLASMATEMPLATE
{
	unsigned char Size;
	unsigned char RandSeed;
};

#define TEXGEN_FractalPlasma 7
struct FRACTALPLASMATEMPLATE
{
	unsigned char Min;
	unsigned char Max;
	unsigned char Blend;
	unsigned char RandSeed;
};

#define TEXGEN_Colorize 8
struct COLORIZETEMPLATE
{
	unsigned int StartColor;
	unsigned int EndColor;
	unsigned char DataChannel;
};

#define TEXGEN_Shade 9
struct SHADETEMPLATE
{
	unsigned char DataLayer;
	unsigned char DataChannel;
};

#define TEXGEN_Brightness 10
struct BRIGHTNESSTEMPLATE
{
	unsigned char Amount;
};

#define TEXGEN_Copy 11
struct COPYTEMPLATE
{
	unsigned char DataLayer;
};

#define TEXGEN_Cells 12
struct CELLSTEMPLATE
{
	unsigned char Points;
	unsigned char Power;
	unsigned char RandSeed;
};

#define TEXGEN_Twirl 13
struct TWIRLTEMPLATE
{
	unsigned char Amount;
};

#define TEXGEN_SineDist 14
struct SINEDISTTEMPLATE
{
	unsigned char XSines;
	unsigned char YSines;
	unsigned char XAmp;
	unsigned char YAmp;
};

#define TEXGEN_Mix 15
struct MIXTEMPLATE
{
	unsigned char DataLayer;
	unsigned char Amount;
};

#define TEXGEN_MixMap 16
struct MIXMAPTEMPLATE
{
	unsigned char DataLayer;
	unsigned char MixLayer;
	unsigned char MixChannel;
};

#define TEXGEN_Scale 17
struct SCALETEMPLATE
{
	unsigned char DataChannel;
	unsigned char Min;
	unsigned char Max;
};

#define TEXGEN_SineColor 18
struct SINECOLORTEMPLATE
{
	unsigned char DataChannel;
	unsigned char NumSines;
};

#define TEXGEN_Max 19
struct MAXTEMPLATE
{
	unsigned char DataLayer;
};

#define TEXGEN_HSV 20
struct HSVTEMPLATE
{
	unsigned char Hue;
	unsigned char Saturation;
};

#define TEXGEN_Emboss 21
#define TEXGEN_Invert 22

#define TEXGEN_Glass 23
struct GLASSTEMPLATE
{
	unsigned char DataLayer;
	unsigned char DataChannel;
	unsigned char Amount;
};

#define TEXGEN_Pixelize 24
struct PIXELIZETEMPLATE
{
	unsigned char XSquares;
	unsigned char YSquares;
};

#define TEXGEN_Offset 25
struct OFFSETTEMPLATE
{
	unsigned char X;
	unsigned char Y;
};

#define TEXGEN_Crystalize 26
struct CRYSTALIZETEMPLATE
{
	unsigned char Points;
	unsigned char RandSeed;
};

#define TEXGEN_Rectangle 27
struct RECTANGLETEMPLATE
{
	unsigned char StartX;
	unsigned char StartY;
	unsigned char SizeX;
	unsigned char SizeY;
};

#define TEXGEN_Circle 28
struct CIRCLETEMPLATE
{
	unsigned char InnerRadius;
	unsigned char OuterRadius;
	unsigned char X;
	unsigned char Y;
};

#define TEXGEN_Contrast 29
struct CONTRASTTEMPLATE
{
	unsigned char Amount;
};

#define TEXGEN_MakeMaterial 30
struct MAKEMATERIALTEMPLATE
{
	unsigned char MaterialSlot;
	char * Name;
};

#define TEXGEN_Gradient 31
struct GRADIENTTEMPLATE
{
	unsigned int _00;
	unsigned int _10;
	unsigned int _01;
	unsigned int _11;
};

#define TEXGEN_Rotozoom 32
struct ROTOZOOMTEMPLATE
{
	unsigned char CX;
	unsigned char CY;
	unsigned char ZoomX;
	unsigned char ZoomY;
	unsigned char ZoomIn;
	unsigned char Rotation;
};

#define TEXGEN_ChamferRectangle 33
struct CHAMFERRECTANGLETEMPLATE
{
	unsigned char StartX;
	unsigned char StartY;
	unsigned char SizeX;
	unsigned char SizeY;
	unsigned char Chamfer;
};

#define TEXGEN_Dots 34
struct DOTSTEMPLATE
{
	unsigned short Count;
	unsigned char Min,Max;
	bool Rounded;
	unsigned char SizeMin,SizeMax;
	unsigned char RandSeed;
};

#define TEXGEN_Jpeg 35
struct JPEGTEMPLATE
{
	//void *ClickedItem;
	int JPEGDataSize;
	unsigned char *JPEGData;
};

#define TEXGEN_FractalPlasmaOld 36
struct FRACTALPLASMAOLDTEMPLATE
{
	unsigned char RandSeed;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

union RGBA
{
	unsigned char c[4];
	struct { unsigned char r,g,b,a; };
	unsigned int dw;
};

struct LAYER
{
	GLuint TextureHandle;
	RGBA *Data;
};

struct MATERIAL
{
	LAYER ImageData;
	bool Used;
	char *Name;
	int TextureID;
	int SlotNumber;
	int Size;
};

struct COMMAND
{
	char Filter;
	char Operator;
	char OperatorMask[4];
	char Layer;
	char DataSize;
	void *data;
};

class TEXTURE
{
public:
	int Id;
	int XRes,YRes,OXRes;
	char *Name;
	LAYER Layers[8];
	ITEMLIST *CommandList;
	MATERIAL Materials[8];

	int CommandNumber;
	COMMAND *Commands;

	TEXTURE(int Resolution)
	{
		for (int x=0; x<8; x++)
		{
			memset(&Materials[x],0,sizeof(MATERIAL));
			Materials[x].Used=false;
			Materials[x].Name="";
			Materials[x].ImageData.Data=new RGBA[Resolution*Resolution];
			memset(Materials[x].ImageData.Data,0,Resolution*Resolution*4);
			Layers[x].Data=new RGBA[Resolution*Resolution];
			memset(Layers[x].Data,0,Resolution*Resolution*4);
			for (int a=0; a<Resolution*Resolution; a++)	Layers[x].Data[a].a=255;
			/*glGenTextures(1,&Layers[x].TextureHandle);
			glBindTexture(GL_TEXTURE_2D,Layers[x].TextureHandle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,Resolution,Resolution,0,GL_RGBA,GL_UNSIGNED_BYTE,Layers[x].Data);*/
			glGenTextures(1,&Materials[x].ImageData.TextureHandle);
			/*glBindTexture(GL_TEXTURE_2D,Materials[x].ImageData.TextureHandle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,Resolution,Resolution,0,GL_RGBA,GL_UNSIGNED_BYTE,Materials[x].ImageData.Data);*/
			//delete CommandList;
			CommandList=new ITEMLIST();
		}
		//RightDisplayStart=0;
		CommandNumber=0;
		Commands=new COMMAND[255];
		memset(Commands,0,255*sizeof(COMMAND));
		XRes=OXRes=Resolution;
		YRes=Resolution;
		//for (x=0; x<8; x++) LeftDisplay[x]=x;
	}

	~TEXTURE()
	{
		for (int x=0; x<8; x++)
			delete Layers[x].Data;
	}

	TEXTURE *Next;

	void Envmap(RGBA*, COMMAND*);
	void Plasma(RGBA*, COMMAND*);
	void SubPlasma(RGBA*, COMMAND*);
	void Text(RGBA*, COMMAND*);
	void Map(RGBA*, COMMAND*);
	void Blur(RGBA*, COMMAND*);
	void DirBlur(RGBA*, COMMAND*);
	void FractalPlasma(RGBA*, COMMAND*);
	void Colorize(RGBA*, COMMAND*);
	void Emboss(RGBA*, COMMAND*);
	void Shade(RGBA*, COMMAND*);
	void Brightness(RGBA*, COMMAND*);
	void Copy(RGBA*, COMMAND*);
	void Cells(RGBA*, COMMAND*);
	void Twirl(RGBA*, COMMAND*);
	void SineDist(RGBA*, COMMAND*);
	void Mix(RGBA*, COMMAND*);
	void MixMap(RGBA*, COMMAND*);
	void Invert(RGBA*, COMMAND*);
	void Scale(RGBA*, COMMAND*);
	void SineColor(RGBA*, COMMAND*);
	void Max(RGBA*, COMMAND*);
	void HSV(RGBA*, COMMAND*);
	void Glass(RGBA*, COMMAND*);
	void Pixelize(RGBA*, COMMAND*);
	void Offset(RGBA*, COMMAND*);
	void Crystalize(RGBA*, COMMAND*);
	void Rectangle(RGBA*, COMMAND*);
	void Circle(RGBA*, COMMAND*);
	void Contrast(RGBA*, COMMAND*);
	void MakeMaterial(RGBA*, COMMAND*);
	void Gradient(RGBA*, COMMAND*);
	void Rotozoom(RGBA*, COMMAND*);
	void ChamferRectangle(RGBA*, COMMAND*);
	void Dots(RGBA*, COMMAND*);
	void Jpeg(RGBA*, COMMAND*);
	void FractalPlasmaOld(RGBA*, COMMAND*);


	void AddCommand(COMMAND*);
	void PerformFilter(RGBA*, RGBA*, COMMAND*);
	void UpdateTexture(RGBA*, GLuint);
#ifdef __TEXGEN_BILINEAR__
	unsigned int Bilinear(RGBA*,unsigned int, unsigned int);
#endif
};

struct JPEGDATA
{
	int DataSize;
	unsigned char *Data;
};

extern TEXTURE *TextureList;

void __stdcall Scroller_SelectItem_TextureEditSelector(cINTERFACE* Interface, int ScrollerID);

#pragma pack()

#endif

extern int CellsBufferImage[1024][1024];
void TexGenPrecalc();

//SetTextAlign