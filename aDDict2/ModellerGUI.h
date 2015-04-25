#ifndef __MODELLERGUI__
#define __MODELLERGUI__

#include "memtrack.h"

#include "aDDict2.h"
#include "GuiInterface.h"
#include "CommonGUIFunctions.h"
#include "3dengine.h"
#include "ObjectGenerators.h"

struct LIGHTDATA
{
	unsigned char r,g,b,ar,ag,ab,ca,la,qa,cu,ex;
	VECTOR3 Pos,Dir;
	float Pos4;
};

void BuildModellerGUI(cINTERFACE *ModellerGUI);
extern cINTERFACE *ModellerDisplays,*MainScroller,*SceneScroller,*LightMenu;
extern LIGHTDATA LightData[8];
extern ITEMLIST *ModellFileList,*MaterialList,*CurrentPolySelection;
extern MATERIAL *ActualMaterial;
extern GLuint MaterialHandle;
extern bool PolySelect;
extern int PolySelectObject;
extern bool LoftCreating,SuperShapeCreating;
extern OBJECT *LoftTrack,*LoftShape,*Preview;

extern cINTERFACE *ModellerDisplay1;
extern cINTERFACE *ModellerDisplay2;
extern cINTERFACE *ModellerDisplay3;
extern cINTERFACE *ModellerDisplay4;
extern cINTERFACE *PolySelectInterface;
extern cINTERFACE *BrushList,*VarMaterialsMenu;
extern cINTERFACE *BrushOperator;
extern cINTERFACE *AddModellInterface;

extern cINTERFACE *TransformSelector;
extern cINTERFACE *ColorSelector;
extern cINTERFACE* MapXformMenu;
extern cINTERFACE *StatusBar3;


extern int BrushSize;
extern int RndMax,RndSeed,RndCount;
extern int MaterialListItemNum;

extern bool Maptransforming;
extern int MappedObject;

extern MATRIX MXMatrix;
extern SUPERSHAPE SuperShapeData;
extern bool ModellLighting;

void __stdcall Scroller_SelectOnlyThisScene(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light1(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light2(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light3(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light4(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light5(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light6(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light7(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall Light8(cINTERFACE* ScrollerInterface, int ScrollerID);
void SetEnvironmentToActualScene();

extern ITEMLIST *PolySelections;
void __stdcall GUI_SelectPolySelection(cINTERFACE* Interface, int ButtonId);

void __stdcall GUI_SelectMaterial(cINTERFACE* Interface, int ButtonId);

void CalculateMapTransform(int MapXformType, int Color, MATRIX MXMatrix, int MappedObject);
void __stdcall CreateLoft(cINTERFACE* Interface, int ScrollerID);
void __stdcall Scroller_SelectOnlyThisArc(cINTERFACE* ScrollerInterface, int ScrollerID);


#endif