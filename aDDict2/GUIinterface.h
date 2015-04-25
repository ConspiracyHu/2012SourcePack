#ifndef __GUIinterface__
#define __GUIinterface__

//#define Public_Release

#include "memtrack.h"

#include <windows.h>
#include <gl\gl.h>
#include "windowhandler.h"
#include "TexGen2.h"
#include "3dEngine.h"
#include "GUIItemList.h"
#include "CommonGUIFunctions.h"

#define backgroundcol	50.0f/255.0f,65.0f/255.0f,89.0f/255.0f,0

#define lightblue		88.0f/127.0f,105.0f/127.0f,133.0f/127.0f,0
#define darkblue		88.0f/511.0f,105.0f/511.0f,133.0f/511.0f,0
#define buttonblue		88.0f/255.0f,105.0f/255.0f,133.0f/255.0f,0
#define highblue		104.0f/255.0f,133.0f/255.0f,179.0f/255.0f,0

#define texton			52.0f/255.0f,52.0f/255.0f,59.0f/255.0f,0
#define textoff			253.0f/255.0f,253.0f/255.0f,1,0

#define GUI_BUTTON 0
#define GUI_TEXTUREDISPLAY 1
#define GUI_SUBINTERFACE 2
#define GUI_SKINNEDBUTTON 3
#define GUI_IMAGE 4
#define GUI_INTPROGRESSBAR 5
#define GUI_FLOATPROGRESSBAR 6
#define GUI_ITEMLIST 7
#define GUI_VERTICALBAR 8
#define GUI_3DVIEW 9
#define GUI_BYTEPROGRESSBAR 10
#define GUI_RADIOBUTTON 11
#define GUI_TEXT 12
#define GUI_VIEWPORT 13
#define GUI_TIMELINER 14
#define GUI_L3DVIEW 15
#define GUI_SPLINEEDITOR 16
#define GUI_INTDISPLAY 17

#define GUI_RED 0
#define GUI_GREEN 1
#define GUI_BLUE 2
#define GUI_NONE 3

class cINTERFACE;

class ITEM;

typedef void (WINAPI *GUICALLBACK) (cINTERFACE*, int);

struct SKINRECTANGLE
{
	int SkinID;
	int XRes,YRes;
	float X1,Y1,X2,Y2;
	float X3,X4;
};

struct GUIITEM
{
	bool ActiveBranch;
	int ItemId,InterfaceItemId;
	int x1,y1,x2,y2,t1,t2;
	int ox1,oy1,ox2,oy2;
	char *Text,*ToolTip;
	bool Skinned;
	unsigned int SkinTexture;
	float tx1,ty1,tx2,ty2;
	bool MouseInWindow,LeftClickInWindow,RightClickInWindow,MidClickInWindow;
	GUICALLBACK OnLeftClick,OnRightClick,OnWheel,OnWheelAfter,OnMidClick;
	cINTERFACE *Interface,*ParentInterface;
	bool Reading;
	SKINRECTANGLE SkinRect;
	char Color;
	int iMin,iMax;
	float fMin,fMax,fSkip;
	int ItemNum,StartItem,SelectedItem,BarSize,HotKey;
	int StartFrame,FrameNum;
	float ZoomFactor;	
	bool LayouterDisplay;

	float Offx,Offy;

	bool ButtonPushed;
	GLuint *Texture;
	char *ReadLine;
	void *data,*data2;

	CAMERA Cam,BCam;
	VECTOR3 i,j;
	char k,i1,j1;
	GLuint CamGrid;
	bool WireFrame;

};

class cINTERFACE 
{
public:
	COMMAND TextureCommand;
	GLuint Preview;
	int SelectedLeftTexture;
	GUIITEM *ItemList;
	ITEM *ClickedItem;
	int ClickedNum;
	int ItemNum;
	int ItemCapacity;

	cINTERFACE() {ItemNum=0; ItemCapacity=0; ItemList=NULL; };
	~cINTERFACE() {
		for (int x=0; x<ItemCapacity; x++)
		{
			delete ItemList[x].Interface;
		}
		delete ItemList;
	}
	void AddGUIItem();
	void DrawGUI();
	void CheckGUI();
	void SetActiveBranch(int Branch) { for (int x=0; x<ItemNum; ItemList[x++].ActiveBranch=Branch==x); };
	void SetActiveBranchNoSubInterface(int Branch) { for (int x=0; x<ItemNum; ItemList[x++].ActiveBranch=(Branch==x) || ItemList[x].ItemId==GUI_SUBINTERFACE); };

	GUIITEM *ParentItem;

	cINTERFACE *AddButton           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, int TEXT_X, int TEXT_Y, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddButton           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, int TEXT_X, int TEXT_Y, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, void **data);
	cINTERFACE *AddButton           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, int TEXT_X, int TEXT_Y, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, int HotKey);
	cINTERFACE *AddSkinnedButton    (int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddSkinnedButton    (int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, void **data);
	cINTERFACE *AddSkinnedButton    (int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, int HotKey);
	cINTERFACE *AddRadioButton      (int ITEM_RECT_X1, int ITEM_RECT_Y1, char *Text,char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddText			   (int ITEM_RECT_X1, int ITEM_RECT_Y1, char *Text);
	cINTERFACE *AddTextureDisplay   (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, GLuint *Texture, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddImage            (int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin);
	cINTERFACE *AddIntBar           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip);
	cINTERFACE *AddIntBar           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip, GUICALLBACK Controll);
	cINTERFACE *AddIntBar           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip, GUICALLBACK Controll, GUICALLBACK ControllAfter);
	cINTERFACE *AddByteBar          (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip);
	cINTERFACE *AddByteBar          (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip, GUICALLBACK Controll);
	cINTERFACE *AddByteBar          (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip, GUICALLBACK Controll, GUICALLBACK ControllAfter);
	cINTERFACE *AddFloatBar         (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Skip, float Default, char *Text, char *ToolTip);
	cINTERFACE *AddFloatBar         (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Skip, float Default, char *Text, char *ToolTip, GUICALLBACK Controll);
	cINTERFACE *AddFloatBar         (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Skip, float Default, char *Text, char *ToolTip, GUICALLBACK Controll, GUICALLBACK ControllAfter);
	cINTERFACE *AddIntProgressBar   (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddByteProgressBar  (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddFloatProgressBar (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Default, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddItemList         (int ITEM_RECT_X1, int ITEM_RECT_Y1, int XSize, int DisplayedNum, ITEMLIST *List, char *Text);
	cINTERFACE *AddVerticalBar      (int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int *Position, int *ItemNumber);
	cINTERFACE *Add3dView           (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, char *Text, bool WireFrame, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddL3dView          (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, char *Text, bool WireFrame, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel);
	cINTERFACE *AddSubInterface     ();
	cINTERFACE *AddViewPort         (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2);
	cINTERFACE *AddTimeliner        (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ROWS);
	cINTERFACE *AddSplineEditor     (int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2);
	cINTERFACE *AddIntDisplay       (int ITEM_RECT_X1, int ITEM_RECT_Y1, int *Data);
};

extern SKINRECTANGLE IntroEditorMenu,TextureGeneratorMenu,ModellerMenu,KeyframerMenu,FileMenu,aDDictLogo,BarLeft,BarRight,RedButton,GreenButton,BlueButton,AlphaButton,UpButton,ScrollButton,DownButton,ScenesButton,ObjectsButton,MaterialsButton,LightsButton,ParticlesButton,EffectsButton,EditButton,FileButton;
extern SKINRECTANGLE CamerasButton,EnvironmentButton,FogButton,CopyButton,CloneButton,DeleteButton,CombinedButton,DotButton,SpotButton,DirectButton,SetButton,NoneButton,DelButton,EnvButton,NoEnvButton,NormalButton,DefaultButton,FlatButton,GouraudButton,DeleteSelectionButton,UnHideButtonSmall,UberToolButton,AdditiveButton2,SubstractiveButton2,MultiplicativeButton,OverwriteButton,HideButtonSmall,UndoButton;
extern SKINRECTANGLE PCameraButton,PStartButton,PBackButton,PLastKeyframeButton,PStopButton,PPlayButton,PNextKeyframeButton,PForwardButton,PEndButton,PPauseButton,NoButton;
extern SKINRECTANGLE LoftButton,CreateButton,ButterflyButton,LinearButton,NewButton,Del_Button,ImportButton,ExportButton,ClearButton,Scenes_Button,AdditiveButton,SubstractiveButton,XorButton;
extern SKINRECTANGLE MapXformButton,MeshSmoothButton,MeshBlurButton,BooleanButton,ImportButtonBig,LitButton,ExportButtonBig,MinimalExportButton;
extern SKINRECTANGLE StoreButton,GridButton,PerspectiveButton,ShowHiddenButton,NormalsButton,FullScreenButton,MoveButton,RotateButton,ScaleButton,CenterButton,ObjSpaceButton,LoadButton,BoxButton,SphereButton,ConeButton,CylinderButton,IcosaederButton,DodecaederButton,LineButton,ArcButton,EscaleButton,GrowButton,Del__Button,Light1Button,Light2Button,Light3Button,Light4Button,Light5Button,Light6Button,Light7Button,Light8Button,RadioButton,AllButton,InvertButton,CloneTransformButton,TopButton;
extern SKINRECTANGLE FinaliseButton,CancelButton,XSwapButton,SwapButton,YSwapButton,SaveKeyFrameButton,DeleteKeyFrameButton,HideButton,UnHideButton,FramesButton,MToKFButton;
extern SKINRECTANGLE EmitterButton,DeflectorButton,GravityButton;
extern SKINRECTANGLE VertexSelectButton,PolySelectButton,PolySelectBrushButton,RandomBrushButton,TextureBrushButton;
extern SKINRECTANGLE VertexSelectButton,PolySelectButton,PolySelectBrushButton,RandomBrushButton,TextureBrushButton;
extern SKINRECTANGLE HeadButton,TailButton,WireframeButton,ExtrudeButton,ConstantsButton,VariablesButton,GlenzButton,SolidButton,AlphaMatButton,ZbufferButton,AlphaEpsilonButton,SnapButton,SplinesButton,ResetButton,SkewButton,InfoButton;
extern SKINRECTANGLE VerticalButton,HorizontalButton,AddModellButton,AddSubWorldButton,AddDummyButton,LightingButton,CopyButtonSmall,MusicButton,FurButton,RulersButton,SuperShapeButton,SuperArcButton,BigStoreButton,BigLoadButton,RndRot,Fixy,DiscCol;
extern SKINRECTANGLE Splash,MXNoneButton,MXBigButton,MXSmallButton,JumpButton,LinearSplineButton,SplineButton,SineButton,SawButton,SquareButton;

extern char *DirNames[];

void InitGui();

void __stdcall nop(cINTERFACE*, int);
void __stdcall GUI_ButtonSelect(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_ButtonSelect_DataChange(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_ButtonSwitch(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_ButtonSwitch_DataChange(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_ReadLine(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_CheckRead(cINTERFACE* Interface, int ButtonId);
void __stdcall Object_LeftClick(cINTERFACE* Interface, int ScrollerID);
void __stdcall Object_RightClick(cINTERFACE* Interface, int ScrollerID);
void __stdcall Object_LLeftClick(cINTERFACE* Interface, int ScrollerID);
void __stdcall Object_LRightClick(cINTERFACE* Interface, int ScrollerID);
void __stdcall World_LLeftClick(cINTERFACE* Interface, int ScrollerID);
void __stdcall World_LRightClick(cINTERFACE* Interface, int ScrollerID);

ITEM *ReturnItem(ITEMLIST *List, int ItemNum);
void __stdcall CheckTransform(cINTERFACE* Interface, int ScrollerID);

extern GLuint base;
extern char *ToolTip;
extern char *LastClickedToolTip;
extern char *ErrorMessage;
extern char *MouseString;
extern bool ReadLine;
extern char ItemNameString[256];

extern int PolySelectID;

GLvoid BuildFont(GLvoid);
void glPrint(const char* text, GLuint font);

float CalcXGLScene(float z, float fov, float aspect);
float CalcYGLScene(float z, float fov, float aspect);

void quad(int x1, int y1, int x2, int y2);
void rectangle(int x1, int y1, int x2, int y2);
void line(int x1, int y1, int x2, int y2);

void TestScaleFactors(VECTOR3 &Min, VECTOR3 &Max, VECTOR3 Pos, MATRIX m);
VECTOR3 CalcXYDir(float x, float y, float fov, float aspect);
void __stdcall GUI_SelectOnlyThisSelection(cINTERFACE* Interface, int x);
void __stdcall GUI_Rename(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_RenameScene(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_RenameWorld(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_RenameTexture(cINTERFACE* Interface, int ButtonId);
void SwapSplash();


#endif