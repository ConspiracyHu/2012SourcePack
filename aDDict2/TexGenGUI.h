#ifndef __TexGenGUI__
#define __TexGenGUI__

#include "memtrack.h"

#include "aDDict2.h"
#include "GUIinterface.h"
#include "CommonGUIFunctions.h"
#include "texgen2.h"
#include "ModellerGui.h"
#include "Layouter.h"

#define TXGEN_LAYER 0
#define TXGEN_BYTE  1
#define TXGEN_CHANNEL 2
#define TXGEN_BOOLEAN 3
#define TXGEN_RANDSEED 4
#define TXGEN_SELECTOR 5
#define TXGEN_TEXT 6
#define TXGEN_JPEGSELECTOR 7

#define ATCI ActualTextureCommandInterface

struct COMMANDPARAMETERTYPE
{
	int Type;
	int iMinValue, iMaxValue, iDefaultValue, Color;
	char *Text,*ToolTip;
};

class FILTERTEMPLATE
{
	void AddItem();
public:
	int ItemNum;
	int ItemCapacity;
	COMMANDPARAMETERTYPE *ItemList;
	FILTERTEMPLATE() {ItemNum=0; ItemCapacity=0; ItemList=NULL;}
	~FILTERTEMPLATE() {delete ItemList;}
	void AddLayerSelector();                              // byte
	void AddByteBar(unsigned char Min, unsigned char Max, unsigned char Default, int Color, char *Text, char *ToolTip);    // byte
	void AddChannelSelector();                            // byte
	void AddBooleanButton(int t1, int t2, char *Text);    // byte
	void AddRandSeed();                                   // byte
	void AddSelectTool();                                 // byte
	void AddTextButton();                                 // asciiz
	void AddJpegSelector();                               // datalength+data
};

extern cINTERFACE *EditMenu;
extern TEXTURE *ActualTexture;
extern int SelectedFilter;
extern cINTERFACE *ActualTextureCommandInterface;
extern cINTERFACE *DisplayArea;
extern cINTERFACE *TextureSelect;
extern cINTERFACE *JpegMenu;


void BuildTexGenGUI(cINTERFACE *TexGenGUI);
void PerformCommand(TEXTURE *Texture,COMMAND *Command, RGBA *Buffer);
void __stdcall Scroller_SelectOnlyThisTexture(cINTERFACE* ScrollerInterface, int ScrollerID);
void BuildMaterialList();
void SetEnvironmentToActualTexture();

#endif