#pragma once

#include <windows.h>
#include <gl/glu.h>
#include "math.h"

#include "includelist.h"

// DEFINE THIS IF YOU WANT TO OPTIMIZE TO SQUARE TEXTURES
#define __TEXGEN_SQUARE_TEXTURES__

// DEFINE THIS IF YOU WANT BILINEAR FILTERED TEXTURES
#define __TEXGEN_BILINEAR__

#pragma pack(1)

#ifdef INCLUDE_TEX_Envmap
#define TEXGEN_Envmap 0
struct ENVMAPTEMPLATE
{
	unsigned char Size;
};
#endif

#ifdef INCLUDE_TEX_Plasma
#define TEXGEN_Plasma 1
struct PLASMATEMPLATE
{
	unsigned char XSines;
	unsigned char YSines;
}; 
#endif

#ifdef INCLUDE_TEX_Map
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
#endif

#ifdef INCLUDE_TEX_Blur
#define TEXGEN_Blur 3
struct BLURTEMPLATE
{
	unsigned char Iterations;
	unsigned char SampleWidth;
}; 
#endif

#ifdef INCLUDE_TEX_DirBlur
#define TEXGEN_DirBlur 4
struct DIRBLURTEMPLATE
{
	unsigned char DataLayer;
	unsigned char DataChannel;
	unsigned char Depth;
}; 
#endif

#ifdef INCLUDE_TEX_Text
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
#endif

#ifdef INCLUDE_TEX_SubPlasma
#define TEXGEN_SubPlasma 6
struct SUBPLASMATEMPLATE
{
	unsigned char Size;
	unsigned char RandSeed;
}; 
#endif

#ifdef INCLUDE_TEX_FractalPlasma
#define TEXGEN_FractalPlasma 7
struct FRACTALPLASMATEMPLATE
{
	unsigned char Min;
	unsigned char Max;
	unsigned char Blend;
	unsigned char RandSeed;
};
#endif

#ifdef INCLUDE_TEX_Colorize
#define TEXGEN_Colorize 8
struct COLORIZETEMPLATE
{
	unsigned int StartColor;
	unsigned int EndColor;
	unsigned char DataChannel;
}; 
#endif

#ifdef INCLUDE_TEX_Shade
#define TEXGEN_Shade 9
struct SHADETEMPLATE
{
	unsigned char DataLayer;
	unsigned char DataChannel;
}; 
#endif

#ifdef INCLUDE_TEX_Brightness
#define TEXGEN_Brightness 10
struct BRIGHTNESSTEMPLATE
{
	unsigned char Amount;
}; 
#endif

#ifdef INCLUDE_TEX_Copy
#define TEXGEN_Copy 11
struct COPYTEMPLATE
{
	unsigned char DataLayer;
}; 
#endif

#ifdef INCLUDE_TEX_Cells
#define TEXGEN_Cells 12
struct CELLSTEMPLATE
{
	unsigned char Points;
	unsigned char Power;
	unsigned char RandSeed;
}; 
#endif

#ifdef INCLUDE_TEX_Twirl
#define TEXGEN_Twirl 13
struct TWIRLTEMPLATE
{
	unsigned char Amount;
}; 
#endif

#ifdef INCLUDE_TEX_SineDist
#define TEXGEN_SineDist 14
struct SINEDISTTEMPLATE
{
	unsigned char XSines;
	unsigned char YSines;
	unsigned char XAmp;
	unsigned char YAmp;
}; 
#endif

#ifdef INCLUDE_TEX_Mix
#define TEXGEN_Mix 15
struct MIXTEMPLATE
{
	unsigned char DataLayer;
	unsigned char Amount;
}; 
#endif

#ifdef INCLUDE_TEX_MixMap
#define TEXGEN_MixMap 16
struct MIXMAPTEMPLATE
{
	unsigned char DataLayer;
	unsigned char MixLayer;
	unsigned char MixChannel;
}; 
#endif

#ifdef INCLUDE_TEX_Scale
#define TEXGEN_Scale 17
struct SCALETEMPLATE
{
	unsigned char DataChannel;
	unsigned char Min;
	unsigned char Max;
}; 
#endif

#ifdef INCLUDE_TEX_SineColor
#define TEXGEN_SineColor 18
struct SINECOLORTEMPLATE
{
	unsigned char DataChannel;
	unsigned char NumSines;
}; 
#endif

#ifdef INCLUDE_TEX_Max
#define TEXGEN_Max 19
struct MAXTEMPLATE
{
	unsigned char DataLayer;
}; 
#endif

#ifdef INCLUDE_TEX_HSV
#define TEXGEN_HSV 20
struct HSVTEMPLATE
{
	unsigned char Hue;
	unsigned char Saturation;
}; 
#endif

#ifdef INCLUDE_TEX_Emboss
#define TEXGEN_Emboss 21
#endif

#ifdef INCLUDE_TEX_Invert
#define TEXGEN_Invert 22
#endif

#ifdef INCLUDE_TEX_Glass
#define TEXGEN_Glass 23
struct GLASSTEMPLATE
{
	unsigned char DataLayer;
	unsigned char DataChannel;
	unsigned char Amount;
}; 
#endif

#ifdef INCLUDE_TEX_Pixelize
#define TEXGEN_Pixelize 24
struct PIXELIZETEMPLATE
{
	unsigned char XSquares;
	unsigned char YSquares;
}; 
#endif

#ifdef INCLUDE_TEX_Offset
#define TEXGEN_Offset 25
struct OFFSETTEMPLATE
{
	unsigned char X;
	unsigned char Y;
}; 
#endif

#ifdef INCLUDE_TEX_Crystalize
#define TEXGEN_Crystalize 26
struct CRYSTALIZETEMPLATE
{
	unsigned char Points;
	unsigned char RandSeed;
}; 
#endif

#ifdef INCLUDE_TEX_Rectangle
#define TEXGEN_Rectangle 27
struct RECTANGLETEMPLATE
{
	unsigned char StartX;
	unsigned char StartY;
	unsigned char SizeX;
	unsigned char SizeY;
}; 
#endif

#ifdef INCLUDE_TEX_Circle
#define TEXGEN_Circle 28
struct CIRCLETEMPLATE
{
	unsigned char InnerRadius;
	unsigned char OuterRadius;
	unsigned char X;
	unsigned char Y;
}; 
#endif

#ifdef INCLUDE_TEX_Contrast
#define TEXGEN_Contrast 29
struct CONTRASTTEMPLATE
{
	unsigned char Amount;
}; 
#endif

#ifdef INCLUDE_TEX_MakeMaterial
#define TEXGEN_MakeMaterial 30
struct MAKEMATERIALTEMPLATE
{
	unsigned char MaterialSlot;
	char * Name;
}; 
#endif

#ifdef INCLUDE_TEX_Gradient
#define TEXGEN_Gradient 31
struct GRADIENTTEMPLATE
{
	unsigned int _00;
	unsigned int _10;
	unsigned int _01;
	unsigned int _11;
}; 
#endif

#ifdef INCLUDE_TEX_Rotozoom
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
#endif

#ifdef INCLUDE_TEX_ChamferRectangle
#define TEXGEN_ChamferRectangle 33
struct CHAMFERRECTANGLETEMPLATE
{
	unsigned char StartX;
	unsigned char StartY;
	unsigned char SizeX;
	unsigned char SizeY;
	unsigned char Chamfer;
}; 
#endif

#ifdef INCLUDE_TEX_Dots
#define TEXGEN_Dots 34
struct DOTSTEMPLATE
{
	unsigned short Count;
	unsigned char Min,Max;
	bool Rounded;
	unsigned char SizeMin,SizeMax;
	unsigned char RandSeed;
};
#endif

#ifdef INCLUDE_TEX_Jpeg
#define TEXGEN_Jpeg 35
struct JPEGTEMPLATE
{
	int JPEGDataSize;
	unsigned char *JPEGData;
};
#endif


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
	int TextureID;
	int SlotNumber;
	int Size;
	MATERIAL *Next;
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
	int XRes,YRes;
	char *Name;
	LAYER Layers[8];

	int CommandNumber;
	COMMAND *Commands;

	TEXTURE(int Resolution)
	{
		for (int x=0; x<8; x++)
			Layers[x].Data=new RGBA[Resolution*Resolution];
		CommandNumber=0;
		Commands=new COMMAND[255];
		XRes=Resolution;
		YRes=Resolution;
	}

	TEXTURE *Next;

#ifdef INCLUDE_TEX_Envmap
	void Envmap(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Plasma
	void Plasma(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_SubPlasma
	void SubPlasma(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Text
	void Text(RGBA*, COMMAND*);
#endif
	
#ifdef INCLUDE_TEX_Map
	void Map(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Blur
	void Blur(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_DirBlur
	void DirBlur(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_FractalPlasma
	void FractalPlasma(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Colorize
	void Colorize(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Emboss
	void Emboss(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Shade
	void Shade(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Brightness
	void Brightness(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Copy
	void Copy(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Cells
	void Cells(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Twirl
	void Twirl(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_SineDist
	void SineDist(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Mix
	void Mix(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_MixMap
	void MixMap(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Invert
	void Invert(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Scale
	void Scale(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_SineColor
	void SineColor(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Max
	void Max(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_HSV
	void HSV(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Glass
	void Glass(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Pixelize
	void Pixelize(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Offset
	void Offset(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Crystalize
	void Crystalize(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Rectangle
	void Rectangle(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Circle
	void Circle(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Contrast
	void Contrast(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_MakeMaterial
	void MakeMaterial(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Gradient
	void Gradient(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Rotozoom
	void Rotozoom(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_ChamferRectangle
	void ChamferRectangle(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Dots
	void Dots(RGBA*, COMMAND*);
#endif

#ifdef INCLUDE_TEX_Jpeg
	void Jpeg(RGBA*, COMMAND*);
#endif

	void PerformCommand(RGBA* Layer, COMMAND* Parameters);
	void PerformFilter(RGBA*, RGBA*, COMMAND*);
#ifdef __TEXGEN_BILINEAR__
	unsigned int Bilinear(RGBA*,unsigned int, unsigned int);
#endif
}; 

extern TEXTURE *TextureList;

#pragma pack()

extern char CellsBufferImage[2048][2048];
void TexGenPrecalc();

//SetTextAlign