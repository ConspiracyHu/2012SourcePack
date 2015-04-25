#include "GUIinterface.h"

#include "GuiImages.h"

#include "math.h"
#include "aDDict2.h"
#include <stdio.h>
#include "aDDictMath.h"
#include "LayouterGUI.h"

#include "UberTool.h"

char *MouseString;
char ItemNameString[256];

char *DirNames[]={
	"x",
	"y",
	"z",
};

GLuint TransparencyBackground;

struct SKINIMAGE
{
	int XRes,YRes;
	void *Data;
	GLuint Handle;
};

MATRIX MXMatrix,MXBuffer;

SKINIMAGE Skin[9];

int RenameItemID;

SKINRECTANGLE InitSkinnedButton(int SkinLayer, int X1, int Y1, int XRes, int YRes)
{
	SKINRECTANGLE Buffer;

	Buffer.SkinID=SkinLayer;
	
	Buffer.XRes=XRes-1;
	Buffer.YRes=YRes-1;

	Buffer.X1=(float)X1/(float)Skin[SkinLayer-1].XRes;
	Buffer.Y1=(float)Y1/(float)Skin[SkinLayer-1].YRes;
	Buffer.X2=(float)(X1+XRes)/(float)Skin[SkinLayer-1].XRes;
	Buffer.Y2=(float)(Y1+YRes)/(float)Skin[SkinLayer-1].YRes;

	Buffer.X3=(float)(X1+(XRes)*2)/(float)Skin[SkinLayer-1].XRes;
	Buffer.X4=(float)(X1+(XRes)*3)/(float)Skin[SkinLayer-1].XRes;

	return Buffer;
}

SKINRECTANGLE IntroEditorMenu,TextureGeneratorMenu,ModellerMenu,KeyframerMenu,FileMenu,aDDictLogo,BarLeft,BarRight,RedButton,GreenButton,BlueButton,AlphaButton,UpButton,ScrollButton,DownButton,ScenesButton,ObjectsButton,MaterialsButton,LightsButton,ParticlesButton,EffectsButton,EditButton,FileButton;
SKINRECTANGLE CamerasButton,EnvironmentButton,FogButton,CopyButton,CloneButton,DeleteButton,CombinedButton,DotButton,SpotButton,DirectButton,SetButton,NoneButton,DelButton,EnvButton,NoEnvButton,NormalButton,DefaultButton,FlatButton,GouraudButton,DeleteSelectionButton,UnHideButtonSmall,UberToolButton,AdditiveButton2,SubstractiveButton2,MultiplicativeButton,OverwriteButton,HideButtonSmall,UndoButton;
SKINRECTANGLE PCameraButton,PStartButton,PBackButton,PLastKeyframeButton,PStopButton,PPlayButton,PNextKeyframeButton,PForwardButton,PEndButton,PPauseButton;
SKINRECTANGLE LoftButton,CreateButton,ButterflyButton,LinearButton,NewButton,Del_Button,ImportButton,ExportButton,ClearButton,Scenes_Button,AdditiveButton,SubstractiveButton,XorButton,NoButton;
SKINRECTANGLE MapXformButton,MeshSmoothButton,MeshBlurButton,BooleanButton,ImportButtonBig,LitButton,ExportButtonBig,MinimalExportButton;
SKINRECTANGLE StoreButton,GridButton,PerspectiveButton,ShowHiddenButton,NormalsButton,FullScreenButton,MoveButton,RotateButton,ScaleButton,CenterButton,ObjSpaceButton,LoadButton,BoxButton,SphereButton,ConeButton,CylinderButton,IcosaederButton,DodecaederButton,LineButton,ArcButton,EscaleButton,GrowButton,Del__Button,Light1Button,Light2Button,Light3Button,Light4Button,Light5Button,Light6Button,Light7Button,Light8Button,RadioButton,AllButton,InvertButton,CloneTransformButton,TopButton;
SKINRECTANGLE FinaliseButton,CancelButton,XSwapButton,SwapButton,YSwapButton,SaveKeyFrameButton,DeleteKeyFrameButton,HideButton,UnHideButton,FramesButton,MToKFButton;
SKINRECTANGLE EmitterButton,DeflectorButton,GravityButton;
SKINRECTANGLE VertexSelectButton,PolySelectButton,PolySelectBrushButton,RandomBrushButton,TextureBrushButton;
SKINRECTANGLE HeadButton,TailButton,WireframeButton,ExtrudeButton,ConstantsButton,VariablesButton,GlenzButton,SolidButton,AlphaMatButton,ZbufferButton,AlphaEpsilonButton,SnapButton,SplinesButton,ResetButton,SkewButton,InfoButton;
SKINRECTANGLE VerticalButton,HorizontalButton,AddModellButton,AddSubWorldButton,AddDummyButton,LightingButton,CopyButtonSmall,MusicButton,FurButton,RulersButton,SuperShapeButton,SuperArcButton,BigStoreButton,BigLoadButton,RndRot,Fixy,DiscCol;
SKINRECTANGLE Splash,MXNoneButton,MXBigButton,MXSmallButton,JumpButton,LinearSplineButton,SplineButton,SineButton,SawButton,SquareButton;

extern "C" char splash[];
extern "C" char hidden[];

bool HiddenPart=false;

void SwapSplash()
{
	if (HiddenPart)
	Skin[8].Data=splash;
	else
	Skin[8].Data=hidden;

	glBindTexture(GL_TEXTURE_2D,Skin[8].Handle);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,Skin[8].XRes,Skin[8].YRes,0,GL_RGB,GL_UNSIGNED_BYTE,Skin[8].Data);
	HiddenPart=!HiddenPart;
}

void InitGui()
{
	RGBA chessboard[4];
	chessboard[0].dw=chessboard[3].dw=0x40404040;
	chessboard[1].dw=chessboard[2].dw=0x80808080;

	glGenTextures(1,&TransparencyBackground);
	glBindTexture(GL_TEXTURE_2D,TransparencyBackground);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,4,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,chessboard);

	Skin[0].Data=raw_buttons_01;
	Skin[0].XRes=Skin[0].YRes=256;

	Skin[1].Data=raw_buttons_02;
	Skin[1].XRes=Skin[1].YRes=256;
	
	Skin[2].Data=raw_buttons_03;
	Skin[2].XRes=512;Skin[2].YRes=128;
	
	Skin[3].Data=raw_buttons_04;
	Skin[3].XRes=Skin[3].YRes=256;
	
	Skin[4].Data=raw_buttons_05;
	Skin[4].XRes=Skin[4].YRes=256;

	Skin[5].Data=raw_buttons_06;
	Skin[5].XRes=Skin[5].YRes=256;

	Skin[6].Data=raw_buttons_07;
	Skin[6].XRes=256; Skin[6].YRes=128;

	Skin[7].Data=raw_buttons_08;
	Skin[7].XRes=Skin[7].YRes=256;

	Skin[8].Data=splash;
	Skin[8].XRes=512; Skin[8].YRes=256;

	for (int x=0; x<9; x++)
	{
		glGenTextures(1,&Skin[x].Handle);
		glBindTexture(GL_TEXTURE_2D,Skin[x].Handle);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,Skin[x].XRes,Skin[x].YRes,0,GL_RGB,GL_UNSIGNED_BYTE,Skin[x].Data);
	}

	aDDictLogo           =InitSkinnedButton(1,0,224,171,32);

	UpButton             =InitSkinnedButton(1,0,176,16,16);
	ScrollButton         =InitSkinnedButton(1,0,192,16,16);
	DownButton           =InitSkinnedButton(1,0,208,16,16);

	ScenesButton         =InitSkinnedButton(1,0,0,19,19);
	ObjectsButton        =InitSkinnedButton(1,0,19,19,19);
	ParticlesButton      =InitSkinnedButton(1,0,38,19,19);
	LightsButton         =InitSkinnedButton(1,0,57,19,19);
	EffectsButton        =InitSkinnedButton(1,0,76,19,19);
	EditButton           =InitSkinnedButton(1,0,95,19,19);
	FileButton           =InitSkinnedButton(1,0,114,19,19);
	MaterialsButton      =InitSkinnedButton(1,0,133,19,19);
	StoreButton          =InitSkinnedButton(1,0,152,19,19);

	GridButton           =InitSkinnedButton(1,57,0,19,19);
	PerspectiveButton    =InitSkinnedButton(1,57,19,19,19);
	ShowHiddenButton     =InitSkinnedButton(1,57,38,19,19);
	NormalsButton        =InitSkinnedButton(1,57,57,19,19);
	FullScreenButton     =InitSkinnedButton(1,57,76,19,19);
	MoveButton           =InitSkinnedButton(1,57,95,19,19);
	RotateButton         =InitSkinnedButton(1,57,114,19,19);
	ScaleButton          =InitSkinnedButton(1,57,133,19,19);
	CenterButton         =InitSkinnedButton(1,57,152,19,19);
	ObjSpaceButton       =InitSkinnedButton(1,57,171,19,19);
	LoadButton           =InitSkinnedButton(1,57,190,19,19);
	
	BoxButton            =InitSkinnedButton(1,114,0,19,19);
	SphereButton         =InitSkinnedButton(1,114,19,19,19);
	ConeButton           =InitSkinnedButton(1,114,38,19,19);
	CylinderButton       =InitSkinnedButton(1,114,57,19,19);
	IcosaederButton      =InitSkinnedButton(1,114,76,19,19);
	DodecaederButton     =InitSkinnedButton(1,114,95,19,19);
	LineButton           =InitSkinnedButton(1,114,114,19,19);
	ArcButton            =InitSkinnedButton(1,114,133,19,19);
	EscaleButton         =InitSkinnedButton(1,114,152,19,19);
	GrowButton           =InitSkinnedButton(1,114,171,19,19);
	Del__Button          =InitSkinnedButton(1,114,190,19,19);
	
	Light1Button         =InitSkinnedButton(1,171,0,17,17);
	Light2Button         =InitSkinnedButton(1,171,17,17,17);
	Light3Button         =InitSkinnedButton(1,171,34,17,17);
	Light4Button         =InitSkinnedButton(1,171,51,17,17);
	Light5Button         =InitSkinnedButton(1,171,68,17,17);
	Light6Button         =InitSkinnedButton(1,171,85,17,17);
	Light7Button         =InitSkinnedButton(1,171,102,17,17);
	Light8Button         =InitSkinnedButton(1,171,119,17,17);
	
	RadioButton          =InitSkinnedButton(1,171,137,14,14);

	AllButton            =InitSkinnedButton(1,171,152,19,19);
	NoButton             =InitSkinnedButton(1,171,171,19,19);
	InvertButton         =InitSkinnedButton(1,171,190,19,19);
	CloneTransformButton =InitSkinnedButton(1,171,209,19,19);
	TopButton            =InitSkinnedButton(1,171,228,19,19);


	MapXformButton       =InitSkinnedButton(2,0,0,85,16);
	MeshSmoothButton     =InitSkinnedButton(2,0,16,85,16);
	MeshBlurButton       =InitSkinnedButton(2,0,32,85,16);
	BooleanButton        =InitSkinnedButton(2,0,48,85,16);
	CamerasButton        =InitSkinnedButton(2,0,64,85,16);
	EnvironmentButton    =InitSkinnedButton(2,0,80,85,16);
	ImportButtonBig      =InitSkinnedButton(2,0,96,85,16);
	LitButton            =InitSkinnedButton(2,0,112,85,16);
	FogButton            =InitSkinnedButton(2,0,128,85,16);
	ExportButtonBig      =InitSkinnedButton(2,0,144,85,16);
	MinimalExportButton  =InitSkinnedButton(2,0,160,85,16);

	IntroEditorMenu      =InitSkinnedButton(3,0,  0,107,23);
	TextureGeneratorMenu =InitSkinnedButton(3,0, 23, 63,23);
	ModellerMenu         =InitSkinnedButton(3,0, 46, 78,23);
	KeyframerMenu        =InitSkinnedButton(3,0, 69, 84,23);
	FileMenu             =InitSkinnedButton(3,0, 92, 51,23);

	BarLeft              =InitSkinnedButton(3,189,23,9,19);
	BarRight             =InitSkinnedButton(3,216,23,9,19);

	RedButton            =InitSkinnedButton(3,263,32,19,19);
	GreenButton          =InitSkinnedButton(3,263,51,19,19);
	BlueButton           =InitSkinnedButton(3,263,70,19,19);
	AlphaButton          =InitSkinnedButton(5,171,0,19,19);

    LoftButton           =InitSkinnedButton(3,153,92,57,16);
	CreateButton         =InitSkinnedButton(3,153,108,57,16);
	ButterflyButton      =InitSkinnedButton(3,329,0,61,16);
	LinearButton         =InitSkinnedButton(3,329,16,61,16);
	NewButton            =InitSkinnedButton(3,377,32,45,16);
	Del_Button           =InitSkinnedButton(3,377,48,45,16);
	ImportButton         =InitSkinnedButton(3,377,64,45,16);
	ExportButton         =InitSkinnedButton(3,377,80,45,16);
	ClearButton          =InitSkinnedButton(3,377,96,45,16);
	Scenes_Button        =InitSkinnedButton(3,377,112,45,16);
	AdditiveButton       =InitSkinnedButton(3,320,32,19,19);
	SubstractiveButton   =InitSkinnedButton(3,320,51,19,19);
	XorButton            =InitSkinnedButton(3,320,70,19,19);

	CopyButton           =InitSkinnedButton(4,0,0,57,16);
	CloneButton          =InitSkinnedButton(4,0,16,57,16);
	DeleteButton         =InitSkinnedButton(4,0,32,57,16);
	CombinedButton       =InitSkinnedButton(4,0,48,57,16);
	DotButton            =InitSkinnedButton(4,0,64,57,16);
	SpotButton           =InitSkinnedButton(4,0,80,57,16);
	DirectButton         =InitSkinnedButton(4,0,96,57,16);

	SetButton            =InitSkinnedButton(4,0,112,45,16);
	NoneButton           =InitSkinnedButton(4,0,128,45,16);
	DelButton            =InitSkinnedButton(4,0,144,45,16);
	EnvButton            =InitSkinnedButton(4,0,160,45,16);
	NoEnvButton          =InitSkinnedButton(4,0,176,45,16);
	NormalButton         =InitSkinnedButton(4,0,192,45,16);
	DefaultButton        =InitSkinnedButton(4,0,208,45,16);
	FlatButton           =InitSkinnedButton(4,0,224,45,16);
	GouraudButton        =InitSkinnedButton(4,0,240,45,16);

    PCameraButton        =InitSkinnedButton(4,171,0,19,19);
	PStartButton         =InitSkinnedButton(4,171,19,28,23);
	PBackButton          =InitSkinnedButton(4,171,42,20,23);
	PLastKeyframeButton  =InitSkinnedButton(4,171,65,20,23);
	PStopButton          =InitSkinnedButton(4,171,88,20,23);
	PPlayButton          =InitSkinnedButton(4,171,111,20,23);
	PNextKeyframeButton  =InitSkinnedButton(4,171,134,20,23);
	PForwardButton       =InitSkinnedButton(4,171,157,20,23);
	PEndButton           =InitSkinnedButton(4,171,180,28,23);
	PPauseButton         =InitSkinnedButton(4,171,203,20,23);

	FinaliseButton       =InitSkinnedButton(5,0,0,57,16);
	CancelButton         =InitSkinnedButton(5,0,16,57,16);
	XSwapButton          =InitSkinnedButton(5,0,32,45,16);
	SwapButton           =InitSkinnedButton(5,0,48,45,16);
	YSwapButton          =InitSkinnedButton(5,0,64,45,16);
	SaveKeyFrameButton   =InitSkinnedButton(5,0,80,29,29);
	DeleteKeyFrameButton =InitSkinnedButton(5,0,109,29,29);
	HideButton           =InitSkinnedButton(5,0,138,57,16);
	UnHideButton         =InitSkinnedButton(5,0,154,57,16);
	FramesButton         =InitSkinnedButton(5,0,170,57,16);
	MToKFButton          =InitSkinnedButton(5,0,186,57,16);
	ResetButton          =InitSkinnedButton(5,0,202,57,16);
	DeleteSelectionButton=InitSkinnedButton(5,171,19,19,19);
	UnHideButtonSmall    =InitSkinnedButton(5,171,38,19,19);
	UberToolButton       =InitSkinnedButton(5,171,57,19,19);
	AdditiveButton2      =InitSkinnedButton(5,171,76,19,19);
	SubstractiveButton2  =InitSkinnedButton(5,171,95,19,19);
	MultiplicativeButton =InitSkinnedButton(5,171,114,19,19);
	OverwriteButton      =InitSkinnedButton(5,171,133,19,19);
	HideButtonSmall      =InitSkinnedButton(5,171,152,19,19);
	UndoButton           =InitSkinnedButton(5,171,171,19,19);
	SkewButton           =InitSkinnedButton(5,171,190,19,19);
	WireframeButton		 =InitSkinnedButton(5,  0,224,85,16);
	ExtrudeButton		 =InitSkinnedButton(5,  0,240,85,16);

	EmitterButton        =InitSkinnedButton(6,  0,  0,52,16);
	DeflectorButton      =InitSkinnedButton(6,  0, 16,63,16);
	GravityButton        =InitSkinnedButton(6,  0, 32,52,16);
	VertexSelectButton   =InitSkinnedButton(6,198, 80,19,19);
	PolySelectButton     =InitSkinnedButton(6,198, 99,19,19);
	PolySelectBrushButton=InitSkinnedButton(6,198,118,19,19);
	RandomBrushButton    =InitSkinnedButton(6,198,137,19,19);
	TextureBrushButton   =InitSkinnedButton(6,198,156,19,19);
	HeadButton			 =InitSkinnedButton(6,  0,128,45,16);
	TailButton			 =InitSkinnedButton(6,  0,144,45,16);
	ConstantsButton		 =InitSkinnedButton(6,  0, 48,85,16);
	VariablesButton		 =InitSkinnedButton(6,  0, 64,85,16);
	GlenzButton			 =InitSkinnedButton(6,  0, 80,57,16);
	SolidButton			 =InitSkinnedButton(6,  0, 96,57,16);
	AlphaMatButton		 =InitSkinnedButton(6,  0,112,57,16);
	ZbufferButton        =InitSkinnedButton(6,  0,160,57,16);
	AlphaEpsilonButton   =InitSkinnedButton(6,  0,208,57,16);
	SplinesButton        =InitSkinnedButton(6,198, 10,19,19);
	SnapButton           =InitSkinnedButton(6,198, 29,19,19);
	InfoButton           =InitSkinnedButton(6,198,208,19,19);
	HorizontalButton     =InitSkinnedButton(6,141,227,19,19);
	VerticalButton       =InitSkinnedButton(6,198,227,19,19);
	CopyButtonSmall      =InitSkinnedButton(6,141,137,19,19);

	MXNoneButton         =InitSkinnedButton(7,0,0,45,16);
	MXBigButton          =InitSkinnedButton(7,0,16,45,16);
	MXSmallButton        =InitSkinnedButton(7,0,32,45,16);
	JumpButton           =InitSkinnedButton(7,198,0,19,19);
	LinearSplineButton   =InitSkinnedButton(7,198,19,19,19);
	SplineButton         =InitSkinnedButton(7,198,38,19,19);
	SineButton           =InitSkinnedButton(7,198,57,19,19);
	SawButton            =InitSkinnedButton(7,198,76,19,19);
	SquareButton         =InitSkinnedButton(7,198,95,19,19);

	AddModellButton      =InitSkinnedButton(8,0,0    ,85,16);
	AddSubWorldButton    =InitSkinnedButton(8,0,16*1 ,85,16);
	AddDummyButton       =InitSkinnedButton(8,0,16*2 ,85,16);
	LightingButton       =InitSkinnedButton(8,0,16*3 ,57,16);
	MusicButton          =InitSkinnedButton(8,0,16*4 ,57,16);
	FurButton            =InitSkinnedButton(8,0,16*5 ,57,16);
	RulersButton         =InitSkinnedButton(8,0,16*6 ,57,16);
	SuperShapeButton     =InitSkinnedButton(8,0,16*7 ,85,16);
	SuperArcButton       =InitSkinnedButton(8,0,16*8 ,85,16);
	BigStoreButton       =InitSkinnedButton(8,0,16*9 ,85,16);
	BigLoadButton        =InitSkinnedButton(8,0,16*10,85,16);
	DiscCol              =InitSkinnedButton(8,0,16*11,85,16);
	RndRot               =InitSkinnedButton(8,0,16*13,45,16);
	Fixy                 =InitSkinnedButton(8,0,16*14,45,16);

	Splash               =InitSkinnedButton(9,0,0,512,256);
}

int PolySelectID;

float CalcXGLScene(float z, float fov, float aspect)
{
 return (float)(fabs((
	 
	              z*( -tan( radtheta*(fov/2.0) ) *aspect)
				  
				  ))*2.0);
}

float CalcYGLScene(float z, float fov, float aspect)
{
 return (float)(fabs((z*(tan(radtheta*(fov/2.0)))))*2.0);
}

VECTOR3 CalcXYDir(float x, float y, float fov, float aspect)
{
	float XSize=CalcXGLScene(1,fov,aspect)/2.0f;
	float YSize=CalcYGLScene(1,fov,aspect)/2.0f;
	return V3_Normalize(V3_Make(-XSize*(x*2.0f-1.0f),YSize*(y*2.0f-1.0f),1));
}

char *ToolTip="";
char *LastClickedToolTip="";
char *ErrorMessage="";

bool ReadLine=false;

void __stdcall nop(cINTERFACE* Interface, int x) {};

void __stdcall GUI_ButtonSelect(cINTERFACE* Interface, int ButtonId)
{
	for (int x=0; x<Interface->ItemNum; x++) Interface->ItemList[x].ButtonPushed=false;
	Interface->ItemList[ButtonId].ButtonPushed=true;
	Interface->SetActiveBranch(ButtonId);
	if (Interface->ItemList[ButtonId].ToolTip) 
	{
		ErrorMessage="";
		LastClickedToolTip=Interface->ItemList[ButtonId].ToolTip;
	}
};

void __stdcall GUI_ButtonSelect_DataChange(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSelect(Interface,ButtonId);
	*(char*)Interface->ItemList[ButtonId].data=ButtonId;
};

void __stdcall GUI_ButtonSwitch(cINTERFACE* Interface, int ButtonId)
{
	Interface->ItemList[ButtonId].ButtonPushed=!Interface->ItemList[ButtonId].ButtonPushed;
	WaitLeftButton=true;
};

void __stdcall GUI_ButtonSwitch_DataChange(cINTERFACE* Interface, int ButtonId)
{
	Interface->ItemList[ButtonId].ButtonPushed=!Interface->ItemList[ButtonId].ButtonPushed;
	*(bool*)Interface->ItemList[ButtonId].data=Interface->ItemList[ButtonId].ButtonPushed;
	WaitLeftButton=true;
};

void __stdcall GUI_ReadLine(cINTERFACE* Interface, int ButtonId)
{
	ReadLine=true;
	Interface->ItemList[ButtonId].Reading=true;
	Interface->ItemList[ButtonId].ReadLine=NULL;
	CommandLineReading=true;
	CommandLine=new char[10];
	memset(CommandLine,0,10);
};

void __stdcall GUI_Rename(cINTERFACE* Interface, int ButtonId)
{
	ReadLine=true;
	Interface->ItemList[ButtonId].Reading=true;
	Interface->ItemList[ButtonId].ReadLine=NULL;
	CommandLineReading=true;
	CommandLine=new char[10];
	memset(CommandLine,0,10);
	ITEMLIST *I=(ITEMLIST*)Interface->ItemList[ButtonId].data;
	for (ITEM *L=I->List; L; L=L->Next)
	{
		L->OldSelected=L->Selected;
		L->Selected=false;
	}
	Interface->ClickedItem->Selected=true;
	RenameItemID=0;
};

void __stdcall GUI_RenameScene(cINTERFACE* Interface, int ButtonId)
{
	ReadLine=true;
	Interface->ItemList[ButtonId].Reading=true;
	Interface->ItemList[ButtonId].ReadLine=NULL;
	CommandLineReading=true;
	CommandLine=new char[10];
	memset(CommandLine,0,10);
	ITEMLIST *I=(ITEMLIST*)Interface->ItemList[ButtonId].data;
	for (ITEM *L=I->List; L; L=L->Next)
	{
		L->OldSelected=L->Selected;
		L->Selected=false;
	}
	Interface->ClickedItem->Selected=true;
	RenameItemID=1;
};

void __stdcall GUI_RenameWorld(cINTERFACE* Interface, int ButtonId)
{
	ReadLine=true;
	Interface->ItemList[ButtonId].Reading=true;
	Interface->ItemList[ButtonId].ReadLine=NULL;
	CommandLineReading=true;
	CommandLine=new char[10];
	memset(CommandLine,0,10);
	ITEMLIST *I=(ITEMLIST*)Interface->ItemList[ButtonId].data;
	for (ITEM *L=I->List; L; L=L->Next)
	{
		L->OldSelected=L->Selected;
		L->Selected=false;
	}
	Interface->ClickedItem->Selected=true;
	RenameItemID=2;
};

void __stdcall GUI_RenameTexture(cINTERFACE* Interface, int ButtonId)
{
	ReadLine=true;
	Interface->ItemList[ButtonId].Reading=true;
	Interface->ItemList[ButtonId].ReadLine=NULL;
	CommandLineReading=true;
	CommandLine=new char[10];
	memset(CommandLine,0,10);
	ITEMLIST *I=(ITEMLIST*)Interface->ItemList[ButtonId].data;
	for (ITEM *L=I->List; L; L=L->Next)
	{
		L->OldSelected=L->Selected;
		L->Selected=false;
	}
	Interface->ClickedItem->Selected=true;
	RenameItemID=3;
};


void __stdcall GUI_CheckRead(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].Reading)
	{
		if (CommandLineEnter) 
		{
			Interface->ItemList[ButtonId].ReadLine=CommandLine; 
			Interface->ItemList[ButtonId].Reading=false;
			LastClickedToolTip=CommandLine;
		}
	}
};

void __stdcall GUI_ChangeIntBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(int*)Interface->ItemList[ButtonId].data=Interface->ItemList[ButtonId].iMin+ (int)((float)(mx-Interface->ItemList[ButtonId].x1)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*(float)(Interface->ItemList[ButtonId].iMax-Interface->ItemList[ButtonId].iMin));
};

void __stdcall GUI_ChangeIntBarValueWheel(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].MouseInWindow)
	*(int*)Interface->ItemList[ButtonId].data=min(max(Interface->ItemList[ButtonId].iMin,*(int*)Interface->ItemList[ButtonId].data+Wheel),Interface->ItemList[ButtonId].iMax);
	else
	*(int*)Interface->ItemList[ButtonId].data=min(max(Interface->ItemList[ButtonId].iMin,*(int*)Interface->ItemList[ButtonId].data),Interface->ItemList[ButtonId].iMax);
};

void __stdcall GUI_DecIntBarValue(cINTERFACE* Interface, int ButtonId)
{
	(*(int*)Interface->ItemList[ButtonId].data)--;
	*(int*)Interface->ItemList[ButtonId].data=min(max(Interface->ItemList[ButtonId].iMin,*(int*)Interface->ItemList[ButtonId].data),Interface->ItemList[ButtonId].iMax);
	WaitLeftButton=true;
};

void __stdcall GUI_IncIntBarValue(cINTERFACE* Interface, int ButtonId)
{
	(*(int*)Interface->ItemList[ButtonId].data)++;
	*(int*)Interface->ItemList[ButtonId].data=min(*(int*)Interface->ItemList[ButtonId].data,Interface->ItemList[ButtonId].iMax);
	WaitLeftButton=true;
};

void __stdcall GUI_DecScrollBarStart(cINTERFACE* Interface, int ButtonId)
{
	Interface->ItemList[ButtonId-1].StartItem--;
	Interface->ItemList[ButtonId-1].StartItem=min(max(0,Interface->ItemList[ButtonId-1].StartItem),Interface->ItemList[ButtonId-1].ItemNum-Interface->ItemList[ButtonId-1].BarSize);
	WaitLeftButton=true;
};

void __stdcall GUI_IncScrollBarStart(cINTERFACE* Interface, int ButtonId)
{
	Interface->ItemList[ButtonId-2].StartItem++;
	Interface->ItemList[ButtonId-2].StartItem=min(max(0,Interface->ItemList[ButtonId-2].StartItem),Interface->ItemList[ButtonId-2].ItemNum-Interface->ItemList[ButtonId-2].BarSize);
	WaitLeftButton=true;
};

void __stdcall GUI_ChangeBigSmallWindow(cINTERFACE* Interface, int ButtonId)
{
	//if (!(LSplineMenu->ParentItem->ButtonPushed && MainGUI.ItemList[3].ButtonPushed))
	{
		if (!Interface->ItemList[ButtonId].ButtonPushed) 
		{   
			//SET FULLSCREEN AND ONLY ONE ACTIVE
			Interface->ParentItem->ParentInterface->ParentItem->ParentInterface->SetActiveBranch(Interface->ParentItem->ParentInterface->ParentItem->InterfaceItemId);
			
			int xc=Interface->ParentItem->ParentInterface->ItemList[0].x1-50,
				yc=Interface->ParentItem->ParentInterface->ItemList[0].y1-50;

			if (MainGUI.ItemList[3].ButtonPushed)
			{
				xc=Interface->ParentItem->ParentInterface->ItemList[0].x1-40;
				yc=Interface->ParentItem->ParentInterface->ItemList[0].y1-40;
			}
			if (Interface->ParentItem->ParentInterface->ItemList[0].x1==174 || Interface->ParentItem->ParentInterface->ItemList[0].x1==574) xc=Interface->ParentItem->ParentInterface->ItemList[0].x1-174;
			for (int x=0; x<Interface->ItemNum; x++)
			{
				Interface->ItemList[x].x1-=xc;
				Interface->ItemList[x].y1-=yc;
				Interface->ItemList[x].x2-=xc;
				Interface->ItemList[x].y2-=yc;
				//if (Interface->ParentItem->ParentInterface->ItemList[0].WireFrame)
				{
					if (x==1 || (x>=3 && x<5))
					{
						Interface->ItemList[x].x1+=400;
						Interface->ItemList[x].x2+=400;
					}
					if (x==2)
					for (int y=0; y<Interface->ItemList[x].Interface->ItemNum; y++)
					{
						Interface->ItemList[x].x1-=xc;
						Interface->ItemList[x].y1-=yc;
						Interface->ItemList[x].x2-=xc;
						Interface->ItemList[x].y2-=yc;
						if (Interface->ParentItem->ParentInterface->ItemList[0].ParentInterface->ParentItem->InterfaceItemId==2 || Interface->ParentItem->ParentInterface->ItemList[0].ParentInterface->ParentItem->InterfaceItemId==0)
						{
							Interface->ItemList[x].Interface->ItemList[y].x1+=400;
							Interface->ItemList[x].Interface->ItemList[y].x2+=400;
						}
						if (Interface->ParentItem->ParentInterface->ItemList[0].ParentInterface->ParentItem->InterfaceItemId==2 || Interface->ParentItem->ParentInterface->ItemList[0].ParentInterface->ParentItem->InterfaceItemId==3)
						{
							Interface->ItemList[x].Interface->ItemList[y].y1-=300;
							Interface->ItemList[x].Interface->ItemList[y].y2-=300;
						}
					}
				}
			}
			
			if (!MainGUI.ItemList[3].ButtonPushed)
			{
				if (Interface->ParentItem->ParentInterface->ItemList[0].x1!=174 && Interface->ParentItem->ParentInterface->ItemList[0].x1!=574)
				{
					Interface->ParentItem->ParentInterface->ItemList[0].x1=50;
					Interface->ParentItem->ParentInterface->ItemList[0].y1=50;
					Interface->ParentItem->ParentInterface->ItemList[0].x2=850;
					Interface->ParentItem->ParentInterface->ItemList[0].y2=650;
				}
				else
				{
					Interface->ParentItem->ParentInterface->ItemList[0].x1=50+124;
					Interface->ParentItem->ParentInterface->ItemList[0].y1=50;
					Interface->ParentItem->ParentInterface->ItemList[0].x2=850+124;
					Interface->ParentItem->ParentInterface->ItemList[0].y2=650;
				}
			}
			else
			{
				if (Interface->ParentItem->ParentInterface->ItemList[0].x1!=174 && Interface->ParentItem->ParentInterface->ItemList[0].x1!=574)
				{
					Interface->ParentItem->ParentInterface->ItemList[0].x1=50;
					Interface->ParentItem->ParentInterface->ItemList[0].y1=40;
					Interface->ParentItem->ParentInterface->ItemList[0].x2=850;
					Interface->ParentItem->ParentInterface->ItemList[0].y2=640;
				}
				else
				{
					Interface->ParentItem->ParentInterface->ItemList[0].x1=50+124;
					Interface->ParentItem->ParentInterface->ItemList[0].y1=40;
					Interface->ParentItem->ParentInterface->ItemList[0].x2=850+124;
					Interface->ParentItem->ParentInterface->ItemList[0].y2=640;
				}
			}
		}
		else
		for (int z=0; z<Interface->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemNum; z++)
		{   
			//SET WINDOWED AND ALL ACTIVE
			Interface->ParentItem->ParentInterface->ParentItem->ParentInterface->ItemList[z].ActiveBranch=true;
			for (int x=0; x<Interface->ItemNum; x++)
			{
				Interface->ItemList[x].x1=Interface->ItemList[x].ox1;
				Interface->ItemList[x].y1=Interface->ItemList[x].oy1;
				Interface->ItemList[x].x2=Interface->ItemList[x].ox2;
				Interface->ItemList[x].y2=Interface->ItemList[x].oy2;
				//if (Interface->ParentItem->ParentInterface->ItemList[0].WireFrame)
					if (x==2)
					for (int y=0; y<Interface->ItemList[x].Interface->ItemNum; y++)
					{
						Interface->ItemList[x].Interface->ItemList[y].x1=Interface->ItemList[x].Interface->ItemList[y].ox1;
						Interface->ItemList[x].Interface->ItemList[y].y1=Interface->ItemList[x].Interface->ItemList[y].oy1;
						Interface->ItemList[x].Interface->ItemList[y].x2=Interface->ItemList[x].Interface->ItemList[y].ox2;
						Interface->ItemList[x].Interface->ItemList[y].y2=Interface->ItemList[x].Interface->ItemList[y].oy2;
					}
			}
			Interface->ParentItem->ParentInterface->ItemList[0].x1=Interface->ParentItem->ParentInterface->ItemList[0].ox1;
			Interface->ParentItem->ParentInterface->ItemList[0].y1=Interface->ParentItem->ParentInterface->ItemList[0].oy1;
			Interface->ParentItem->ParentInterface->ItemList[0].x2=Interface->ParentItem->ParentInterface->ItemList[0].ox2;
			Interface->ParentItem->ParentInterface->ItemList[0].y2=Interface->ParentItem->ParentInterface->ItemList[0].oy2;
		}

		Interface->ItemList[ButtonId].ButtonPushed=!Interface->ItemList[ButtonId].ButtonPushed;
		WaitLeftButton=true;
	}
};

void __stdcall GUI_ChangeByteBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(255,max(0,Interface->ItemList[ButtonId].iMin+(0.5f+(float)(mx-Interface->ItemList[ButtonId].x1)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*(float)(Interface->ItemList[ButtonId].iMax-Interface->ItemList[ButtonId].iMin))));
};

void __stdcall GUI_ChangeByteBarValueWheel(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].MouseInWindow)
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(max((int)Interface->ItemList[ButtonId].iMin,(int)*(unsigned char*)Interface->ItemList[ButtonId].data+Wheel),(int)Interface->ItemList[ButtonId].iMax);
	else
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(max((int)Interface->ItemList[ButtonId].iMin,(int)*(unsigned char*)Interface->ItemList[ButtonId].data),(int)Interface->ItemList[ButtonId].iMax);
};

void __stdcall GUI_DecByteBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(max((int)Interface->ItemList[ButtonId].iMin,(int)*(unsigned char*)Interface->ItemList[ButtonId].data-1),(int)Interface->ItemList[ButtonId].iMax);
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(max((int)Interface->ItemList[ButtonId].iMin,(int)*(unsigned char*)Interface->ItemList[ButtonId].data),(int)Interface->ItemList[ButtonId].iMax);
	WaitLeftButton=true;
};

void __stdcall GUI_IncByteBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(max((int)Interface->ItemList[ButtonId].iMin,(int)*(unsigned char*)Interface->ItemList[ButtonId].data+1),(int)Interface->ItemList[ButtonId].iMax);
	*(unsigned char*)Interface->ItemList[ButtonId].data=(unsigned char)min(max((int)Interface->ItemList[ButtonId].iMin,(int)*(unsigned char*)Interface->ItemList[ButtonId].data),(int)Interface->ItemList[ButtonId].iMax);
	WaitLeftButton=true;
};

void __stdcall GUI_ChangeFloatBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(float*)Interface->ItemList[ButtonId].data=Interface->ItemList[ButtonId].fMin+((float)(mx-Interface->ItemList[ButtonId].x1)/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*(float)(Interface->ItemList[ButtonId].fMax-Interface->ItemList[ButtonId].fMin));
};

void __stdcall GUI_ChangeFloatBarValueWheel(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].MouseInWindow)
	*(float*)Interface->ItemList[ButtonId].data=(float)min(max(Interface->ItemList[ButtonId].fMin,*(float*)Interface->ItemList[ButtonId].data+Wheel*0.01f),Interface->ItemList[ButtonId].fMax);
	else
	*(float*)Interface->ItemList[ButtonId].data=(float)min(max(Interface->ItemList[ButtonId].fMin,*(float*)Interface->ItemList[ButtonId].data),Interface->ItemList[ButtonId].fMax);
};

void __stdcall GUI_DecFloatBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(float*)Interface->ItemList[ButtonId].data=(*(float*)Interface->ItemList[ButtonId].data)-Interface->ItemList[ButtonId].fSkip;
	*(float*)Interface->ItemList[ButtonId].data=(float)min(max(Interface->ItemList[ButtonId].fMin,*(float*)Interface->ItemList[ButtonId].data),Interface->ItemList[ButtonId].fMax);
	WaitLeftButton=true;
};

void __stdcall GUI_IncFloatBarValue(cINTERFACE* Interface, int ButtonId)
{
	*(float*)Interface->ItemList[ButtonId].data=(*(float*)Interface->ItemList[ButtonId].data)+Interface->ItemList[ButtonId].fSkip;
	*(float*)Interface->ItemList[ButtonId].data=(float)min(max(Interface->ItemList[ButtonId].fMin,*(float*)Interface->ItemList[ButtonId].data),Interface->ItemList[ButtonId].fMax);
	WaitLeftButton=true;
};


void __stdcall GUI_ListWheel(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].MouseInWindow)
	{
		Interface->ItemList[ButtonId].StartItem-=Wheel;
	}
	Interface->ItemList[ButtonId].StartItem=min(max(0,Interface->ItemList[ButtonId].StartItem),max(0,Interface->ItemList[ButtonId].ItemNum-Interface->ItemList[ButtonId].BarSize));
	if (Interface->ItemList[ButtonId].Reading && CommandLineEnter)
	{
		Interface->ItemList[ButtonId].ReadLine=CommandLine; 
		Interface->ItemList[ButtonId].Reading=false;
		ITEMLIST *I=(ITEMLIST*)Interface->ItemList[ButtonId].data;
		for (ITEM *L=I->List; L; L=L->Next)
		{
			if (L->Selected) 
			{
				//delete L->Name;
				if (strlen(CommandLine))
				{
					switch (RenameItemID)
					{
					case 0: //default rename
						{
							L->Name=new char[strlen(CommandLine)+1];
							sprintf(L->Name,"%s\0",CommandLine);
						}
						break;
					case 1: //scene rename
						{
							SCENE *s=(SCENE*)L->Data;
							s->Name=new char[strlen(CommandLine)+1];
							sprintf(s->Name,"%s\0",CommandLine);

							char *NewLine=new char[strlen(s->Name)+51];
							_snprintf(NewLine,strlen(s->Name)+51,"%d: %s\0",s->ID,s->Name);

							L->Name=new char[strlen(NewLine)+1];
							sprintf(L->Name,"%s\0",NewLine);
						}
						break;
					case 2: //world rename
						{
							WORLD *s=(WORLD*)L->Data;
							s->Name=new char[strlen(CommandLine)+1];
							sprintf(s->Name,"%s\0",CommandLine);

							char *NewLine=new char[strlen(s->Name)+51];
							_snprintf(NewLine,strlen(s->Name)+51,"%d: %s\0",s->ID,s->Name);

							L->Name=new char[strlen(NewLine)+1];
							sprintf(L->Name,"%s\0",NewLine);
						}
						break;
					case 3: //texture rename
						{
							TEXTURE *s=(TEXTURE*)L->Data;
							s->Name=new char[strlen(CommandLine)+1];
							sprintf(s->Name,"%s\0",CommandLine);

							char *NewLine=new char[strlen(s->Name)+51];
							_snprintf(NewLine,strlen(s->Name)+51,"%d: %s (%dx%d)\0",s->Id,s->Name,s->XRes,s->YRes);

							L->Name=new char[strlen(NewLine)+1];
							sprintf(L->Name,"%s\0",NewLine);
						}
						break;
					}
				}
			}
			L->Selected=L->OldSelected;
		}
	}
};

void __stdcall GUI_ScrollWheel(cINTERFACE* Interface, int ButtonId)
{
	if (!Interface->ItemList[ButtonId].LeftClickInWindow)
	{
		Interface->ItemList[ButtonId].iMax=*(int*)Interface->ItemList[ButtonId].data;
		Interface->ItemList[ButtonId].ButtonPushed=false;
	}
	
	if (Interface->ItemList[ButtonId].MouseInWindow) (*(int*)Interface->ItemList[ButtonId].data)-=Wheel;
		*(int*)Interface->ItemList[ButtonId].data=min(max(0,*(int*)Interface->ItemList[ButtonId].data),max(0,*(int*)Interface->ItemList[ButtonId].data2-Interface->ItemList[ButtonId].BarSize));
};

void __stdcall GUI_PushWheel(cINTERFACE* Interface, int ButtonId)
{
	int a=Interface->ItemList[ButtonId].iMax,b=*((int*)Interface->ItemList[ButtonId].data2);
	float r2=(float)a/((float)(b-Interface->ItemList[ButtonId].BarSize));
	int res=(int)((float)((Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)-16)*r2)+Interface->ItemList[ButtonId].y1+8;

	int a2=*((int*)Interface->ItemList[ButtonId].data),b2=*((int*)Interface->ItemList[ButtonId].data2);
	float r22=(float)a2/((float)(b2-Interface->ItemList[ButtonId].BarSize));
	int res2=(int)((float)((Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)-16)*r22)+Interface->ItemList[ButtonId].y1+8;
	
	if (ly<res && res2>=my)
		*((int*)Interface->ItemList[ButtonId].data)-=2;

	if (ly>res && res2<=my)
		*((int*)Interface->ItemList[ButtonId].data)+=2;

	if (ly>=res-8 && ly<=res+8)
	{
		*((int*)Interface->ItemList[ButtonId].data)=(int)(
			          (float)(my-Interface->ItemList[ButtonId].y1-8)/
			          (float)((Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)-16)*
					  (float)(b-Interface->ItemList[ButtonId].BarSize)
					 );
		Interface->ItemList[ButtonId].ButtonPushed=true;
	}
	*(int*)Interface->ItemList[ButtonId].data=min(max(0,*(int*)Interface->ItemList[ButtonId].data),max(0,*(int*)Interface->ItemList[ButtonId].data2-Interface->ItemList[ButtonId].BarSize));
};

void __stdcall GUI_SelectOnlyThisSelection(cINTERFACE* Interface, int x) 
{
	Interface->ItemList[x].SelectedItem=PolySelectID=(min(max(my,Interface->ItemList[x].y1),Interface->ItemList[x].y2-11)-Interface->ItemList[x].y1)/11+Interface->ItemList[x].StartItem;
	//PolySelectID=min(PolySelectID,PolySelectInterface->ItemList[4].ItemNum-1);
	//ActualTexture=(TEXTURE*)(FindItem((ITEMLIST*)Interface->ItemList[ID].data,Interface->ItemList[ID].SelectedItem)->Data);
};

int Level=0;
ITEM *CheckSub(ITEM *List,int &y, int ItemNum)
{
	ITEM *SubList=List;
	while (SubList)
	{
		y++;
		if (y==ItemNum) return SubList;
		if (SubList->SubOpened) 
		{
			Level+=1;
			ITEM *I=CheckSub(SubList->SubItems,y,ItemNum);
			if (I && !(y<ItemNum)) return I;
			Level-=1;
		}
		if (SubList) SubList=SubList->Next;
	}
	return NULL;
}

ITEM *ReturnItem(ITEMLIST *List, int ItemNum)
{
	ITEM *Item=List->List;
	if (Item) for (int y=0; y<ItemNum; y++)
		if (Item)
		{
			if (Item->SubOpened) 
			{
				Level+=1;
				ITEM *I=CheckSub(Item->SubItems,y,ItemNum);				
				if (I && !(y<ItemNum)) return I;
				Level-=1;
			}
			if (Item && y<ItemNum) Item=Item->Next;
		}	
	return Item;
}

void __stdcall GUI_OnScrollLeftClick(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].data)
	{
		Interface->ClickedNum=(min(max(my,Interface->ItemList[ButtonId].y1),Interface->ItemList[ButtonId].y2-11)-Interface->ItemList[ButtonId].y1)/11+Interface->ItemList[ButtonId].StartItem;
		Interface->ClickedItem=ReturnItem((ITEMLIST*)Interface->ItemList[ButtonId].data,Interface->ClickedNum);
		//if (Interface->ItemList[ButtonId].data2) memcpy(Interface->ItemList[ButtonId].data2,&Interface->ClickedItem,4);
		if (Interface->ItemList[ButtonId].data2 && Interface->ClickedItem)
		{
			JPEGDATA *a=(JPEGDATA*)Interface->ItemList[ButtonId].data2;
			if (a->Data) delete[] a->Data;
			char *fname=Interface->ClickedItem->Name;
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

		if (Interface->ClickedItem) 
			Interface->ClickedItem->OnLeftClick(Interface,ButtonId);
	}
};

void __stdcall GUI_OnScrollRightClick(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].data)
	{
		Interface->ClickedNum=(min(max(my,Interface->ItemList[ButtonId].y1),Interface->ItemList[ButtonId].y2-11)-Interface->ItemList[ButtonId].y1)/11+Interface->ItemList[ButtonId].StartItem;
		Interface->ClickedItem=ReturnItem((ITEMLIST*)Interface->ItemList[ButtonId].data,Interface->ClickedNum);
		if (Interface->ClickedItem) 
			Interface->ClickedItem->OnRightClick(Interface,ButtonId);
	}
};

void __stdcall GUI_OnScrollMiddleClick(cINTERFACE* Interface, int ButtonId)
{
	if (Interface->ItemList[ButtonId].data)
	{
		Interface->ClickedNum=(min(max(my,Interface->ItemList[ButtonId].y1),Interface->ItemList[ButtonId].y2-11)-Interface->ItemList[ButtonId].y1)/11+Interface->ItemList[ButtonId].StartItem;
		Interface->ClickedItem=ReturnItem((ITEMLIST*)Interface->ItemList[ButtonId].data,Interface->ClickedNum);
		if (Interface->ClickedItem) 
			Interface->ClickedItem->OnMiddleClick(Interface,ButtonId);
	}
};

void __stdcall GUI_MiddleClickFollowCursor(cINTERFACE* Interface, int ButtonId)
{
	Cammoving=false;
	GUIITEM *View=&Interface->ItemList[ButtonId];
	if (Interface->ItemList[ButtonId].WireFrame)
	{
		if (RightButton) {mx=mbx; my=mby;}
		float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
		float XSize=CalcXGLScene(z, View->Cam.Fov, 4/3.0f)/(float)(View->x2-View->x1)*(mby-my);
		float YSize=CalcYGLScene(z, View->Cam.Fov, 4/3.0f)/(float)(View->y2-View->y1)*(mbx-mx);
		VECTOR3 i1,j1;
		i1=V3_Mults(View->i,YSize);
		j1=V3_Mults(View->j,-XSize);
		View->Cam.Eye=V3_Add(V3_Add(View->BCam.Eye,i1),j1);
		View->Cam.Target=V3_Add(V3_Add(View->BCam.Target,i1),j1);
	}
	else
	if (ActualScene)
	{
		Cammoving=true;
		if (RightButton) {mx=mbx; my=mby;}

		if (MidShift)
		{

			View->Cam.Eye=V3_Sub(View->BCam.Eye,View->BCam.Target);
			//View->Cam.Up=View->BCam.Up;
			//View->Cam.Target=V3_Sub(View->Cam.Target,View->BCam.Target);

			MATRIX m;
			
			M_Rotate(View->Cam.Up.x,View->Cam.Up.y,View->Cam.Up.z,-(float)((mx-mbx)*radtheta),m);
			M_Xform3(m,View->Cam.Eye,View->Cam.Eye);

			VECTOR3 Axis2=V3_Cross(View->Cam.Up,View->Cam.Eye);
			
			M_Rotate(Axis2.x,Axis2.y,Axis2.z,-(float)((my-mby)*radtheta),m);
			M_Xform3(m,View->Cam.Eye,View->Cam.Eye);
			
			//M_Xform3(m,View->Cam.Up,View->Cam.Up);

			View->Cam.Eye=V3_Add(View->Cam.Eye,View->BCam.Target);
			//View->Cam.Target=V3_Add(View->Cam.Target,View->BCam.Target);

		}
		else
		{
			VECTOR3 Displace=V3_Make(0,0,0);
			Displace=V3_Add(Displace,V3_Mults(View->Cam.i,(mbx-mx)/10.0f));
			Displace=V3_Add(Displace,V3_Mults(View->Cam.j,(mby-my)/10.0f));
			View->Cam.Target=V3_Add(View->BCam.Target,Displace);
			View->Cam.Eye=V3_Add(View->BCam.Eye,Displace);
		}

	}
};

void __stdcall CheckTransform(cINTERFACE* Interface, int ScrollerID)
{
	Transforming=false;
	Cammoving=false;
	for (int x=0; x<Interface->ItemList[ScrollerID].Interface->ItemNum; x++)
		if (Interface->ItemList[ScrollerID].Interface->ItemList[x].ActiveBranch)
		{
			if (LeftClickInWindow(Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].x1,
				                  Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].y1,
				                  Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].x2,
				                  Interface->ItemList[ScrollerID].Interface->ItemList[x].Interface->ItemList[0].y2)) Transforming=true;
		}
	if (ActualScene)
	for (int i=0; i<ActualScene->ObjectNum; i++)
	if (!ActualScene->ObjectList[i].Deleted && !ActualScene->ObjectList[i].ListItem->Hidden  && ActualScene->ObjectList[i].Primitive<aDDict_MAPXFORM)
	{
		if (Transforming)
		{
			memcpy(ActualScene->ObjectList[i].ModelView,ActualScene->ObjectList[i].TransformBuffer,sizeof(MATRIX));
			memcpy(MXMatrix,MXBuffer,sizeof(MATRIX));
		}
		else
		{
			memcpy(ActualScene->ObjectList[i].TransformBuffer,ActualScene->ObjectList[i].ModelView,sizeof(MATRIX));
			memcpy(MXBuffer,MXMatrix,sizeof(MATRIX));
		}
		if (!Maptransforming) M_Identity(MXMatrix);
	}
}

void CloneClick(OBJECT *o)
{
	for (int x=0; x<o->Param1; x++)
	{
		OBJECT *o2=((OBJECT*)((int*)o->Param3)[x]);
		if (!o2->Deleted)
		{
			glPushMatrix();
			MultModelViewMatrix(o2->ModelView,1);
			if (o2->Backface) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
			if (o2->Backfront) glCullFace(GL_FRONT); else glCullFace(GL_BACK);

			if (o2->Primitive!=aDDict_CLONE)
			glCallList(o2->DisplayList);
			else
			CloneClick(o2);
			glPopMatrix();
		}
	}
}

void __stdcall GUI_3dViewCheck(cINTERFACE* Interface, int ButtonId)
{
	GUIITEM *View=&Interface->ItemList[ButtonId];
	if (Interface->ItemList[ButtonId].WireFrame)
	{
		if (View->MouseInWindow)
		{
			float s=-View->Cam.Eye.a[View->k]/(float)fabs(View->Cam.Eye.a[View->k]);
			if ((View->Cam.Eye.a[View->k]+Wheel*s)/View->Cam.Eye.a[View->k]>0)
			View->Cam.Eye.a[View->k]+=Wheel*s;

			if ((View->Cam.Eye.a[View->k]+1*s)/View->Cam.Eye.a[View->k]>0)
			if (Keys['j']) View->Cam.Eye.a[View->k]+=1*s;			
			if (Keys['m']) View->Cam.Eye.a[View->k]-=1*s;
		}
		if (!View->MidClickInWindow) View->BCam=View->Cam;
	}
	else
	{
		if (PolySelect)
		{
			SelectObjPolys(&ActualScene->ObjectList[PolySelectObject], CurrentPolySelection, -1);
		}
		if (View->MouseInWindow && ActualScene)
		{

			VECTOR3 Displace=V3_Mults( V3_Normalize( V3_Sub(View->Cam.Target,View->Cam.Eye) ) ,Wheel*0.5f);
			if (V3_Length(View->Cam.Eye)+V3_Length(V3_Add(View->Cam.Eye,Displace))>1 || Wheel<0)
			View->Cam.Eye=V3_Add(View->Cam.Eye,Displace);

			if (!View->MidClickInWindow) View->BCam=View->Cam;

			bool normalviewport=true;
			glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
			SetCameraView(&View->Cam,4/3.0f,1,mx,my);
			unsigned int *buffer;
			int hits;
			buffer=new unsigned int[4*ActualScene->ObjectNum];
			memset(buffer,0,4*ActualScene->ObjectNum*sizeof(unsigned int));

			glSelectBuffer(4*ActualScene->ObjectNum,buffer);
			glEnable(GL_CULL_FACE);
			glRenderMode(GL_SELECT);
			glInitNames();
			glPushName(0);
			glLoadName(0);
			for (int x=0; x<ActualScene->ObjectNum; x++)
			if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden  && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
			{
				if (!normalviewport && 
					ActualScene->ObjectList[x].Primitive!=13 && 
					ActualScene->ObjectList[x].Primitive!=12 && 
					ActualScene->ObjectList[x].Primitive!=11 && 
					ActualScene->ObjectList[x].Primitive!=aDDict_GRAVITY && 
					ActualScene->ObjectList[x].Primitive!=aDDict_SPHEREDEFLECTOR && 
					ActualScene->ObjectList[x].Primitive!=aDDict_PLANEDEFLECTOR)
				{
					SwitchTo2D();
					glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
					SetCameraView(&View->Cam,4/3.0f,1,mx,my);
					normalviewport=true;
				}
				else
				if (normalviewport)
				{
					SwitchTo2D();
					glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
					SetCameraView(&View->Cam,4/3.0f,10,mx,my);
					normalviewport=false;
				}
				glLoadName(x/*ActualScene->ObjectList[x].ID*/);
				glPushMatrix();
				MultModelViewMatrix(ActualScene->ObjectList[x].ModelView,1);
				if (ActualScene->ObjectList[x].Primitive!=aDDict_CLONE)
				glCallList(ActualScene->ObjectList[x].DisplayList);
				else CloneClick(&ActualScene->ObjectList[x]);
				glPopMatrix();
			}

			glDisable(GL_CULL_FACE);
			hits=glRenderMode(GL_RENDER);
			if (hits) 
			{
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

				CurrentCursor=Crosshair;
				memset(ItemNameString,0,256);
				//if (ActualScene->ObjectList[smallest].ListItem)
				sprintf(ItemNameString,"%s (%d)\0",ActualScene->ObjectList[smallest].ListItem->Name,ActualScene->ObjectList[smallest].ID);
			}
			else 
			{
				CurrentCursor=Arrow;
				memset(ItemNameString,0,256);
			}
			SwitchTo2D();			
		}
		if (ActualScene)
		ActualScene->Camera=ModellerDisplay4->ParentItem->Cam;
	}
};

void TestScaleFactors(VECTOR3 &Min, VECTOR3 &Max, VECTOR3 Pos, MATRIX m)
{
	VECTOR3 ScaleFactors;
	M_Xform3(m,Pos,ScaleFactors);
	Min.x=min(ScaleFactors.x,Min.x); Min.y=min(ScaleFactors.y,Min.y); Min.z=min(ScaleFactors.z,Min.z);
	Max.x=max(ScaleFactors.x,Max.x); Max.y=max(ScaleFactors.y,Max.y); Max.z=max(ScaleFactors.z,Max.z);
}

void CalculateMapTransform(int MapXformType, int Color, MATRIX MXMatrix, int MappedObject)
{
	VECTOR3 ScaleVec;

	if (MapXformType==aDDict_SCALE) //scale
	{
		M_RevScale(MXMatrix,ScaleVec);
		ScaleVec=V3_Sub(ScaleVec,V3_Make(1,1,1));
		for (int x=0; x<3; x++) ScaleVec.a[x]=KillFloat(ScaleVec.a[x],2);
	}

	VECTOR3 AxisVec;
	float Angle;

	if (MapXformType==aDDict_ROTATE) //rotate
	{
		M_RevRotate(MXMatrix,AxisVec,Angle);
		for (int x=0; x<3; x++) AxisVec.a[x]=KillFloat(AxisVec.a[x],2);
		Angle=KillFloat(Angle,2);
	}

	VECTOR3 MoveVec;

	if (MapXformType==aDDict_MOVE) //move
	{
		M_RevXpose(MXMatrix,MoveVec);
		for (int x=0; x<3; x++) MoveVec.a[x]=KillFloat(MoveVec.a[x],2);
	}


	for (int x=0; x<ActualScene->ObjectList[MappedObject].VertexNum; x++)
		if (ActualScene->ObjectList[MappedObject].VertexList[x].Selected)
		{
			float weight;
			if (Color==0) weight=ActualScene->ObjectList[MappedObject].VertexList[x].Weight[0];
			if (Color==1) weight=ActualScene->ObjectList[MappedObject].VertexList[x].Weight[1];
			if (Color==2) weight=ActualScene->ObjectList[MappedObject].VertexList[x].Weight[2];
			if (Color==3) weight=ActualScene->ObjectList[MappedObject].VertexList[x].Weight[3];
			if (Color==4) weight=(ActualScene->ObjectList[MappedObject].VertexList[x].Weight[0]+ActualScene->ObjectList[MappedObject].VertexList[x].Weight[1]+ActualScene->ObjectList[MappedObject].VertexList[x].Weight[2])/3.0f;
			if (MapXformType==aDDict_SCALE)
			{//scale
				VECTOR3 SVec;
				SVec=V3_Mults(ScaleVec,weight);
				ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition.x=ActualScene->ObjectList[MappedObject].VertexList[x].Position.x*(SVec.x+1);
				ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition.y=ActualScene->ObjectList[MappedObject].VertexList[x].Position.y*(SVec.y+1);
				ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition.z=ActualScene->ObjectList[MappedObject].VertexList[x].Position.z*(SVec.z+1);
			}
			if (MapXformType==aDDict_ROTATE)
			{//rotate

				MATRIX m;
				M_Rotate(AxisVec.x,AxisVec.y,AxisVec.z,Angle*weight,m);

				M_Xform3(m,ActualScene->ObjectList[MappedObject].VertexList[x].Position,ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition);

			}
			if (MapXformType==aDDict_MOVE)
			{//move
				VECTOR3 MoveVector=V3_Mults(MoveVec,weight);

				ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition=V3_Add(ActualScene->ObjectList[MappedObject].VertexList[x].Position,MoveVector);
			}

		}
		else ActualScene->ObjectList[MappedObject].VertexList[x].MapTransformedPosition=ActualScene->ObjectList[MappedObject].VertexList[x].Position;
}

void __stdcall GUI_3dRightClick(cINTERFACE* Interface, int ButtonId)
{
	GUIITEM *View=&Interface->ItemList[ButtonId];
	float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
	if (Interface->ItemList[ButtonId].WireFrame)
	{
		if (
			!(rx<=Interface->ItemList[ButtonId].x1+19 && ry<Interface->ItemList[ButtonId].y1+19) &&
			!(rx>=Interface->ItemList[ButtonId].x2-19*4 && ry<Interface->ItemList[ButtonId].y1+19) &&
			!(rx>=Interface->ItemList[ButtonId].x2-19 && ry<Interface->ItemList[ButtonId].y1+19*3))
		{
			if (ActualScene)
			{
				if (MainScroller->ParentItem->SelectedItem==1) //camera transform
				{
					float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
					float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
					ModellerDisplay4->ParentItem->Cam.Target.a[View->i1]=xx*View->i.a[View->i1]+View->Cam.Target.a[View->i1];
					ModellerDisplay4->ParentItem->Cam.Target.a[View->j1]=yy*View->j.a[View->j1]+View->Cam.Target.a[View->j1];
				}
				if (MainScroller->ParentItem->SelectedItem==2) //light transform
				{
					ITEM *a=ActualScene->ItemList->List->Next->Next->SubItems;
					for (int x=0; x<8; x++)
					{
						if (a->Selected && ActualScene->Lights[x].Lit)
						{
							if (ActualScene->Lights[x].Position[3]==1 && ActualScene->Lights[x].Spot_Cutoff!=180) //spotlight
							{
								float xx=((mx-(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)/2.0f-Interface->ItemList[ButtonId].x1))/(float)(Interface->ItemList[ButtonId].x2-Interface->ItemList[ButtonId].x1)*CalcXGLScene(z, View->Cam.Fov, 4/3.0f);
								float yy=(-(my-(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)/2.0f-Interface->ItemList[ButtonId].y1))/(float)(Interface->ItemList[ButtonId].y2-Interface->ItemList[ButtonId].y1)*CalcYGLScene(z, View->Cam.Fov, 4/3.0f);
								ActualScene->Lights[x].Spot_Direction[View->i1]=xx*View->i.a[View->i1]+View->Cam.Eye.a[View->i1]-ActualScene->Lights[x].Position[View->i1];
								ActualScene->Lights[x].Spot_Direction[View->j1]=yy*View->j.a[View->j1]+View->Cam.Eye.a[View->j1]-ActualScene->Lights[x].Position[View->j1];
							}
						}
						a=a->Next;
					}
				}
			}
		}
	}
	else
	if (!TransformingWhenRightClicked && !CammovingWhenRightClicked && ActualScene)
	{		
		bool normalviewport=true;
		glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
		SetCameraView(&View->Cam,4/3.0f,1,mx,my);
		unsigned int *buffer;
		int hits;
		buffer=new unsigned int[4*ActualScene->ObjectNum];
		memset(buffer,0,4*ActualScene->ObjectNum*sizeof(unsigned int));

		glSelectBuffer(4*ActualScene->ObjectNum,buffer);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);
		glLoadName(0);
		int x;
		for (x=0; x<ActualScene->ObjectNum; x++)
		if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden  && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
		{
			if (!normalviewport && 
				ActualScene->ObjectList[x].Primitive!=13 && 
				ActualScene->ObjectList[x].Primitive!=12 && 
				ActualScene->ObjectList[x].Primitive!=11 && 
				ActualScene->ObjectList[x].Primitive!=aDDict_GRAVITY && 
				ActualScene->ObjectList[x].Primitive!=aDDict_SPHEREDEFLECTOR && 
				ActualScene->ObjectList[x].Primitive!=aDDict_PLANEDEFLECTOR)
			{
				SwitchTo2D();
				glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
				SetCameraView(&View->Cam,4/3.0f,1,mx,my);
				normalviewport=true;
			}
			else
			if (normalviewport)
			{
				SwitchTo2D();
				glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
				SetCameraView(&View->Cam,4/3.0f,10,mx,my);
				normalviewport=false;
			}
			glLoadName(x/*ActualScene->ObjectList[x].ID*/);
			glPushMatrix();
			MultModelViewMatrix(ActualScene->ObjectList[x].ModelView,1);

			if (ActualScene->ObjectList[x].Backface) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
			if (ActualScene->ObjectList[x].Backfront) glCullFace(GL_FRONT); else glCullFace(GL_BACK);

			if (ActualScene->ObjectList[x].Primitive!=aDDict_CLONE)
			glCallList(ActualScene->ObjectList[x].DisplayList);
			else
			CloneClick(&ActualScene->ObjectList[x]);
			
			glPopMatrix();
		}

		hits=glRenderMode(GL_RENDER);

		if (hits)
		{
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
			if (RightCtrl) for (x=0; x<ActualScene->ObjectNum; x++) ActualScene->ObjectList[x].ListItem->Selected=false;
				
			ActualScene->ObjectList[smallest].ListItem->Selected=!ActualScene->ObjectList[smallest].ListItem->Selected;
			/*for (x=0; x<ActualScene->ObjectNum; x++)
			if (!ActualScene->ObjectList[x].Deleted && !ActualScene->ObjectList[x].ListItem->Hidden)
			{
				if (RightCtrl)
				{
				 ActualScene->ObjectList[smallest].ListItem->Selected=!ActualScene->ObjectList[smallest].ListItem->Selected;
				}
				else ActualScene->ObjectList[smallest].ListItem->Selected=ActualScene->ObjectList[smallest].ID==smallest;
			}*/
			WaitRightButton=true;
		}
		else
		{
			for (x=0; x<ActualScene->ObjectNum; x++) 
			//if (!ActualScene->ObjectList[x].Deleted  && ActualScene->ObjectList[x].Primitive<aDDict_MAPXFORM)
				ActualScene->ObjectList[x].ListItem->Selected=false;
		}
		
		SwitchTo2D();

		delete buffer;
	}
	glDisable(GL_CULL_FACE);
};


void __stdcall Object_LeftClick(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
	Interface->ClickedItem->Selected=!Interface->ClickedItem->Selected;
	WaitLeftButton=true;
};

void __stdcall Object_RightClick(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;
	for (int x=0; x<ActualScene->ObjectNum; x++) ActualScene->ObjectList[x].ListItem->Selected=false;
	Interface->ClickedItem->Selected=!Interface->ClickedItem->Selected;
	WaitRightButton=true;
};

void __stdcall Object_LLeftClick(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
	Interface->ClickedItem->Selected=!Interface->ClickedItem->Selected;
	WaitLeftButton=true;
};

void __stdcall Object_LRightClick(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
	for (int x=0; x<ActualWorld->ObjectNum; x++) ActualWorld->ObjectList[x].ListItem->Selected=false;
	Interface->ClickedItem->Selected=!Interface->ClickedItem->Selected;
	WaitRightButton=true;
};

bool CheckParentTree(int IDtobefound, WORLD *World, int CurrentPos)
{
	if (World->ObjectList[CurrentPos].ID==IDtobefound) return true;
	for (int x=0; x<World->ObjectNum; x++)
		if (World->ObjectList[x].ParentID==World->ObjectList[CurrentPos].ID)
		{
			bool a=CheckParentTree(IDtobefound,World,x);
			if (a) return true;
		}
	return false;
}

void __stdcall World_LLeftClick(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
	if (LeftCtrl)
	{
		Interface->ClickedItem->Selected=!Interface->ClickedItem->Selected;
		if (Interface->ClickedItem->Selected)
		{
			int c=0;
			for (ITEM *a=ObjectSplines->List;a;a=a->Next)
			{
				if (((WORLDOBJECT*)a->Data)->ID==((WORLDOBJECT*)Interface->ClickedItem->Data)->ID) ObjList->ParentItem->SelectedItem=c;
				c++;
			}
			c=0;
			for (ITEM *a=EmitterSplines->List;a;a=a->Next)
			{
				if (((WORLDOBJECT*)a->Data)->ID==((WORLDOBJECT*)Interface->ClickedItem->Data)->ID) PrtList->ParentItem->SelectedItem=c;
				c++;
			}
			c=0;
			for (ITEM *a=SubWorldSplines->List;a;a=a->Next)
			{
				if (((WORLDOBJECT*)a->Data)->ID==((WORLDOBJECT*)Interface->ClickedItem->Data)->ID) SwrldList->ParentItem->SelectedItem=c;
				c++;
			}
		}
	}
	else
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++) 
		if (ActualWorld->ObjectList[x].ListItem->Selected) 
		{
			if (!CheckParentTree(((WORLDOBJECT*)Interface->ClickedItem->Data)->ID,ActualWorld,x))
			{
				ActualWorld->ObjectList[x].ParentID=((WORLDOBJECT*)Interface->ClickedItem->Data)->ID;
				ActualWorld->ObjectList[x].Parent=ActualWorld->FindObjectByID(ActualWorld->ObjectList[x].ParentID);
			}
			else
			ErrorMessage="YOU CAN'T RECURSIVELY CONNECT AN OBJECT TO ITSELF AGAIN!";
		}
	}
	
	WaitLeftButton=true;
};

void __stdcall World_LRightClick(cINTERFACE* Interface, int ScrollerID)
{
	Interface->ItemList[ScrollerID].SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
	if (RightCtrl)
	{
		for (int x=0; x<ActualWorld->ObjectNum; x++) ActualWorld->ObjectList[x].ListItem->Selected=false;
		Interface->ClickedItem->Selected=!Interface->ClickedItem->Selected;
		int c=0;
		for (ITEM *a=ObjectSplines->List;a;a=a->Next)
		{
			if (((WORLDOBJECT*)a->Data)->ID==((WORLDOBJECT*)Interface->ClickedItem->Data)->ID) ObjList->ParentItem->SelectedItem=c;
			c++;
		}
		c=0;
		for (ITEM *a=EmitterSplines->List;a;a=a->Next)
		{
			if (((WORLDOBJECT*)a->Data)->ID==((WORLDOBJECT*)Interface->ClickedItem->Data)->ID) PrtList->ParentItem->SelectedItem=c;
			c++;
		}
		c=0;
		for (ITEM *a=SubWorldSplines->List;a;a=a->Next)
		{
			if (((WORLDOBJECT*)a->Data)->ID==((WORLDOBJECT*)Interface->ClickedItem->Data)->ID) SwrldList->ParentItem->SelectedItem=c;
			c++;
		}
	}
	else
		for (int x=0; x<ActualWorld->ObjectNum; x++) if (ActualWorld->ObjectList[x].ListItem->Selected) 
		{
			ActualWorld->ObjectList[x].ParentID=-1;
			ActualWorld->ObjectList[x].Parent=NULL;
		}
	WaitRightButton=true;
};


GLuint base;

GLvoid BuildFont(GLvoid)
{
	HFONT	font;
	HFONT	oldfont;

	base = glGenLists(96);

	font = CreateFont(	12,							// Height Of Font
						6,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_NORMAL,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

void glPrint(const char* text, GLuint font)
{
	if (text)
	{
		glPushAttrib(GL_LIST_BIT);
		glListBase(font - 32);
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
		glPopAttrib();
	}
}

void line(int x1, int y1, int x2, int y2)
{
	glBegin(GL_LINES);
	glVertex2i(x1,y1);
	glVertex2i(x2,y2);
	glEnd();
}

void rectangle(int x1, int y1, int x2, int y2)
{
	glBegin(GL_LINES);
	glVertex2i(x1,y1);
	glVertex2i(x2+1,y1);

	glVertex2i(x2,y1);
	glVertex2i(x2,y2);
	
	glVertex2i(x2,y2);
	glVertex2i(x1,y2);
	
	glVertex2i(x1,y2);
	glVertex2i(x1,y1);
	glEnd();
}

void quad(int x1, int y1, int x2, int y2)
{
	glBegin(GL_QUADS);
	glVertex2i(x1,y1);
	glVertex2i(x2,y1);
	glVertex2i(x2,y2);
	glVertex2i(x1,y2);
	//glVertex2i(x1,y1);
	glEnd();
}

void texturedquad(int x1, int y1, int x2, int y2, float u1, float v1, float u2, float v2)
{
	glBegin(GL_QUADS);

	glTexCoord2f(u1,v1);	
	glVertex2i(x1,y1);

	glTexCoord2f(u2,v1);
	glVertex2i(x2+1,y1);

	glTexCoord2f(u2,v2);
	glVertex2i(x2+1,y2+1);

	glTexCoord2f(u1,v2);
	glVertex2i(x1,y2+1);

	glEnd();
}

void DrawButton(int x1, int y1, int x2, int y2, int t1, int t2, bool Pushed, bool Lit, char *Text)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glScissor(x1,YRes-y2-1,max(1,x2-x1+1),max(1,y2-y1+1)+1);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_DEPTH_TEST);
	/*glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);*/
	

	if (!Pushed && !Lit) glColor4f(buttonblue);
	else
	{
		if (Pushed) glColor4f(1,1,1,1);
		else glColor4f(highblue);
	}
	quad(x1+1,y1,x2,y2-1);

	if (Pushed) glColor4f(darkblue); else glColor4f(lightblue);
	line(x1,y1,x2,y1);
	line(x1,y2,x1,y1);
	if (Pushed) glColor4f(lightblue); else glColor4f(darkblue);
	line(x2,y1+1,x2,y2);
	line(x1,y2,x2+1,y2);


	if (Pushed) glColor4f(texton); else glColor4f(textoff);
	glRasterPos2i(x1+t1,y1+t2);
	glPrint(Text,base);

	glColor4f(1,1,1,1);
	glDisable(GL_SCISSOR_TEST);

	//glPopAttrib();
}

void DrawSkinnedButton(int x1, int y1, SKINRECTANGLE SkinRect, bool Pushed, bool Lit, char *Text)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);*/
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D,Skin[SkinRect.SkinID-1].Handle);

	if (!Pushed && !Lit) texturedquad(x1,y1,x1+SkinRect.XRes,y1+SkinRect.YRes,SkinRect.X2,SkinRect.Y1,SkinRect.X3,SkinRect.Y2);
	else
	{
		if (Pushed) texturedquad(x1,y1,x1+SkinRect.XRes,y1+SkinRect.YRes,SkinRect.X1,SkinRect.Y1,SkinRect.X2,SkinRect.Y2);
		else texturedquad(x1,y1,x1+SkinRect.XRes,y1+SkinRect.YRes,SkinRect.X3,SkinRect.Y1,SkinRect.X4,SkinRect.Y2);
	}
	glDisable(GL_TEXTURE_2D);
	//glPopAttrib();
}


void DrawTexture(int x1, int y1, int x2, int y2, bool Highlight, GLuint *Texture)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glScissor(x1,YRes-y2-1,max(1,x2-x1+1),max(1,y2-y1+1)+1);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);
	//glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(false);

	if (!Texture || !glIsTexture(*Texture))
	{
		glColor4f(0,0,0,0);
		quad(x1+1,y1,x2,y2-1);
	}
	else
	{		
		glColor4f(0.5,0.5,0.5,0.5);
		glBindTexture(GL_TEXTURE_2D,TransparencyBackground);
		glEnable(GL_TEXTURE_2D);
		texturedquad(x1+1,y1,x2,y2-1,0,0,10,10);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glColor4f(1,1,1,1);

		glBindTexture(GL_TEXTURE_2D,*Texture);
		texturedquad(x1+1,y1,x2,y2-1,0,0,1,1);
		
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D); 
	}
	if (!Highlight) glColor4f(highblue); else 
	{
		glColor4f(1,1,1,1);
		glDepthMask(true);
	}
	rectangle(x1,y1,x2,y2);

	glDisable(GL_SCISSOR_TEST);
	glColor4f(1,1,1,1);
	glDepthMask(true);
	//glPopAttrib();
}

void DrawTexture(int x1, int y1, int x2, int y2, float u1, float v1, float u2, float v2, bool Highlight, GLuint *Texture)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glScissor(x1,YRes-y2-1,max(1,x2-x1+1),max(1,y2-y1+1)+1);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);
	//glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(false);

	if (!Texture || !glIsTexture(*Texture))
	{
		glColor4f(0,0,0,0);
		quad(x1+1,y1,x2,y2-1);
	}
	else
	{		
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor4f(1,1,1,1);
		glBindTexture(GL_TEXTURE_2D,*Texture);
		texturedquad(x1+1,y1,x2,y2-1,u1,v1,u2,v2);
		glDisable(GL_TEXTURE_2D);
	}
	if (!Highlight) glColor4f(highblue); else 
	{
		glColor4f(1,1,1,1);
		glDepthMask(true);
	}
	rectangle(x1,y1,x2,y2);

	glDisable(GL_SCISSOR_TEST);
	glColor4f(1,1,1,1);
	glDepthMask(true);
	//glPopAttrib();
}


void DrawProgressBar(int x1, int y1, int Size, int Color, float Position, char *Text, float Value, bool Int)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);*/
	glBindTexture(GL_TEXTURE_2D,Skin[1].Handle);

	glColor4f(1,1,1,1);
	float X1,Y1,X2,Y2;

	switch (Color)
	{
	case GUI_RED:
		{
			X1=131;
			Y1=180+19;
			break;
		}
	case GUI_GREEN:
		{
			X1=131;
			Y1=218+19;
			break;
		}
	case GUI_BLUE:
		{
			X1=9;
			Y1=218+19;
			break;
		}
	case GUI_NONE:
		{
			X1=9;
			Y1=180+19;
			break;
		}
	}

	X2=X1+113;
	Y2=Y1+19;

	texturedquad(x1,y1,x1+Size,y1+BarLeft.YRes,X1/256.0f,Y1/256.0f,X2/256.0f,Y2/256.0f);

	X2=X1+113.0f*Position;
	Y1-=19;
	Y2-=19;

	texturedquad(x1,y1,(int)(x1+Size*Position),y1+BarLeft.YRes,X1/256.0f,Y1/256.0f,X2/256.0f,Y2/256.0f);

	glDisable(GL_TEXTURE_2D);
	
	glColor4f(0,0,0,0);

	glRasterPos2i(x1+6,y1+14);
	char *buffer=new char[strlen(Text)+100];
	if (Int) sprintf(buffer,"%s%i\0",Text,(int)Value);
	else sprintf(buffer,"%s%.3f\0",Text,Value);

	glPrint(buffer,base);

	glColor4f(1,1,1,1);

	glRasterPos2i(x1+5,y1+13);
	glPrint(buffer,base);

	delete buffer;

	//glPopAttrib();
}

//#ifndef Public_Release
void PreviewSuperShape()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	SUPERSHAPE s=SuperShapeData;
	glColor4f(1,1,1,1);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);	
	for (int x=0; x<s.Xres; x++)
	{
		float phi=Linear((float)s.Rangex1,(float)s.Rangex2,x/(float)(s.Xres))/180.0f*3.1415f;
		float phi2=Linear((float)s.Rangex1,(float)s.Rangex2,(x+1)/(float)(s.Xres))/180.0f*3.1415f;
	
		float r0=0;
		if (!s.Sphere) r0=s.Trad0;

		float r1a=SuperShapeRad(s.SuperShape1.mint+s.SuperShape1.mfloat/255.0f,s.SuperShape1.n1,s.SuperShape1.n2,s.SuperShape1.n3,s.SuperShape1.a,s.SuperShape1.b,phi)+r0;
		float r1b=SuperShapeRad(s.SuperShape1.mint+s.SuperShape1.mfloat/255.0f,s.SuperShape1.n1,s.SuperShape1.n2,s.SuperShape1.n3,s.SuperShape1.a,s.SuperShape1.b,phi2)+r0;

		if (s.RadFunc)
		{
			float r1=1,r2=1;
			if (s.RadFunc==1) 
			{
				r1=s.Rada*phi;
				r2=s.Rada*phi2;
			}
			if (s.RadFunc==2) 
			{
				r1=(float)(s.Rada*exp(s.Radb*phi));
				r2=(float)(s.Rada*exp(s.Radb*phi2));
			}
			r1a*=r1;
			r1b*=r2;
		}

		float r1=s.Verta*phi;
		float r2=s.Verta*phi2;
		
		for (int y=0; y<s.Yres; y++)
		{
			float ro=Linear((float)s.Rangey1,(float)s.Rangey2,y/(float)(s.Yres))/180.0f*3.1415f;
			float ro2=Linear((float)s.Rangey1,(float)s.Rangey2,(y+1)/(float)(s.Yres))/180.0f*3.1415f;

			float r2a=SuperShapeRad(s.SuperShape2.mint+s.SuperShape2.mfloat/255.0f,s.SuperShape2.n1,s.SuperShape2.n2,s.SuperShape2.n3,s.SuperShape2.a,s.SuperShape2.b,ro);
			float r2b=SuperShapeRad(s.SuperShape2.mint+s.SuperShape2.mfloat/255.0f,s.SuperShape2.n1,s.SuperShape2.n2,s.SuperShape2.n3,s.SuperShape2.a,s.SuperShape2.b,ro2);

			VECTOR3 pos1,pos2,pos3,pos4;
			if (s.Sphere)
			{
				pos1=V3_Make(r1a*cos(phi )*r2a*cos(ro ),r1a*sin(phi )*r2a*cos(ro ),r2a*sin(ro ));
				pos2=V3_Make(r1a*cos(phi )*r2b*cos(ro2),r1a*sin(phi )*r2b*cos(ro2),r2b*sin(ro2));
				pos3=V3_Make(r1b*cos(phi2)*r2a*cos(ro ),r1b*sin(phi2)*r2a*cos(ro ),r2a*sin(ro ));
				pos4=V3_Make(r1b*cos(phi2)*r2b*cos(ro2),r1b*sin(phi2)*r2b*cos(ro2),r2b*sin(ro2));
			}
			else
			{
				pos1=V3_Make(cos(phi )*(r1a+r2a*cos(ro )),sin(phi )*(r1a+r2a*cos(ro )),r2a*sin(ro ));
				pos2=V3_Make(cos(phi )*(r1a+r2b*cos(ro2)),sin(phi )*(r1a+r2b*cos(ro2)),r2b*sin(ro2));
				pos3=V3_Make(cos(phi2)*(r1b+r2a*cos(ro )),sin(phi2)*(r1b+r2a*cos(ro )),r2a*sin(ro ));
				pos4=V3_Make(cos(phi2)*(r1b+r2b*cos(ro2)),sin(phi2)*(r1b+r2b*cos(ro2)),r2b*sin(ro2));
			}

			if (s.VertFunc)
			{
				pos1.z*=r1;
				pos2.z*=r1;
				pos4.z*=r2;
			}

			VECTOR3 n1,n2;

			n1=V3_Cross(V3_Sub(pos2,pos1),V3_Sub(pos2,pos4));
			n2=V3_Cross(V3_Sub(pos2,pos3),V3_Sub(pos2,pos4));

			glNormal3f(n1.x,n1.y,n1.z);

			glVertex3f(pos1.x,pos1.y,pos1.z);
			glVertex3f(pos2.x,pos2.y,pos2.z);
			glVertex3f(pos4.x,pos4.y,pos4.z);

			glNormal3f(n2.x,n2.y,n2.z);
			
			glVertex3f(pos1.x,pos1.y,pos1.z);
			glVertex3f(pos3.x,pos3.y,pos3.z);
			glVertex3f(pos4.x,pos4.y,pos4.z);
			
			/*glVertex3f(pos3.x,pos3.y,pos3.z);
			glVertex3f(pos4.x,pos4.y,pos4.z);
			
			glVertex3f(pos4.x,pos4.y,pos4.z);
			glVertex3f(pos1.x,pos1.y,pos1.z);*/

		}
	}
	glEnd();
	glDisable(GL_LIGHTING);
}
//#endif

void Display3dView(GUIITEM *View)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glScissor(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
	glEnable(GL_SCISSOR_TEST);
	glColor4f(0,0,0,0);
	glDisable(GL_DEPTH_TEST);
	quad(View->x1,View->y1,View->x2,View->y2);
	glEnable(GL_DEPTH_TEST);
	glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
	SetCameraView(&View->Cam,4/3.0f,0,mx,my);
	glColor4f(1,1,1,1);

	float z=V3_Length(V3_Sub(View->Cam.Target,View->Cam.Eye));
	float XSize=CalcXGLScene(z, View->Cam.Fov, 4/3.0f)/2.0f;
	float YSize=CalcYGLScene(z, View->Cam.Fov, 4/3.0f)/2.0f;

	if (View->ParentInterface->ItemList[1].Interface->ItemList[5].ButtonPushed)
	{
		if (View->WireFrame)
		{
			glBegin(GL_LINES);

			VECTOR3 v2;

			for (int z=0; z<3; z++)
			{
				float x;
				for (x=(float)((int)(View->Cam.Target.a[View->i1]-XSize-0.5)); x<View->Cam.Target.a[View->i1]+XSize+0.5; x+=0.5)
				{
					int y = abs((int)(x*2)+1) % 2;
					if ((z==1 && y==1 && x!=0) || (z==2 && y==0 && x!=0) || (z==0 && x==0))
					{
						v2.x=(float)(0.1*(y+1.0));
						v2.y=(float)(0.15*(y+1.0));
						v2.z=(float)(0.25*(y+1.0));

						if (x==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.x,v2.y,v2.z,1);
						float a=View->Cam.Target.a[View->j1]-YSize;
						float b=View->Cam.Target.a[View->j1]+YSize;

						glVertex3f(x*View->i.x*View->i.a[View->i1]+a*View->j.x*View->j.a[View->j1],
								   x*View->i.y*View->i.a[View->i1]+a*View->j.y*View->j.a[View->j1],
								   x*View->i.z*View->i.a[View->i1]+a*View->j.z*View->j.a[View->j1]);
						glVertex3f(x*View->i.x*View->i.a[View->i1]+b*View->j.x*View->j.a[View->j1],
								   x*View->i.y*View->i.a[View->i1]+b*View->j.y*View->j.a[View->j1],
								   x*View->i.z*View->i.a[View->i1]+b*View->j.z*View->j.a[View->j1]);
					}
				}

				for (x=(float)((int)(View->Cam.Target.a[View->j1]-YSize-0.5)); x<View->Cam.Target.a[View->j1]+YSize+0.5; x+=0.5)
				{
					int y = abs((int)(x*2)+1) % 2;
					if ((z==1 && y==1 && x!=0) || (z==2 && y==0 && x!=0) || (z==0 && x==0))
					{
						v2.x=(float)(0.1*(y+1.0));
						v2.y=(float)(0.15*(y+1.0));
						v2.z=(float)(0.25*(y+1.0));

						if (x==0) glColor4d(0.3,0.4,0.5,1); else glColor4f(v2.x,v2.y,v2.z,1);
						float a=View->Cam.Target.a[View->i1]-XSize;
						float b=View->Cam.Target.a[View->i1]+XSize;

						glVertex3f(x*View->j.x*View->j.a[View->j1]+a*View->i.x*View->i.a[View->i1],
								   x*View->j.y*View->j.a[View->j1]+a*View->i.y*View->i.a[View->i1],
								   x*View->j.z*View->j.a[View->j1]+a*View->i.z*View->i.a[View->i1]);
						glVertex3f(x*View->j.x*View->j.a[View->j1]+b*View->i.x*View->i.a[View->i1],
								   x*View->j.y*View->j.a[View->j1]+b*View->i.y*View->i.a[View->i1],
								   x*View->j.z*View->j.a[View->j1]+b*View->i.z*View->i.a[View->i1]);
					}
				}
			}

			glEnd();
		}
		else glCallList(View->CamGrid);
	}

	VECTOR3 v;
	v.x=View->i.x*View->i.x+View->j.x*View->j.x;
	v.y=View->i.y*View->i.y+View->j.y*View->j.y;
	v.z=View->i.z*View->i.z+View->j.z*View->j.z;
	if (!View->LayouterDisplay)
	if (View->WireFrame && ActualScene)
		ActualScene->RenderWireFrame(v,false);
	if (!View->WireFrame && ActualScene && !View->LayouterDisplay)
	{
		glDepthFunc(GL_LESS);
		SetCameraView(&View->Cam,4/3.0f,0,mx,my);
		ActualScene->Render(ModellLighting);

		if (LoftCreating)
		{
			Preview->VertexNum=0;
			Preview->PolygonNum=0;
			Preview->EdgeNum=0;
			if (LoftTrack && LoftShape && !LoftTrack->Deleted && !LoftShape->Deleted)
			{
				GeneratePrimitive(Preview,aDDict_LOFT,(int)LoftTrack,(int)LoftShape,0,0,0);
				glColor4f(1,1,1,1);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
				glCallList(Preview->WireDisplayList);
			}
		}

//#ifndef Public_Release
		if (SuperShapeCreating)
		{
			PreviewSuperShape();
		}
//#endif

		if (Maptransforming)
		{
			glPushMatrix();
			MultModelViewMatrix(ActualScene->ObjectList[MappedObject].ModelView,1);
			glBegin(GL_TRIANGLES);
			for (int x=0; x<ActualScene->ObjectList[MappedObject].PolygonNum; x++)
			{
				int v1,v2,v3;
				v1=ActualScene->ObjectList[MappedObject].PolygonList[x].v1;
				v2=ActualScene->ObjectList[MappedObject].PolygonList[x].v2;
				v3=ActualScene->ObjectList[MappedObject].PolygonList[x].v3;

				float weight;
				if (ColorSelector->ItemList[0].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[0];
				if (ColorSelector->ItemList[1].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[1];
				if (ColorSelector->ItemList[2].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[2];
				if (ColorSelector->ItemList[3].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[3];
				if (ColorSelector->ItemList[4].ButtonPushed) weight=(ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[0]+ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[1]+ActualScene->ObjectList[MappedObject].VertexList[v1].Weight[2])/3.0f;

				if (ActualScene->ObjectList[MappedObject].VertexList[v1].Selected)
				glColor4f(weight,weight,0,1);
				else
				glColor4f(0.5,0.5,0.5,1);
				glVertex3f(ActualScene->ObjectList[MappedObject].VertexList[v1].MapTransformedPosition.x,
					       ActualScene->ObjectList[MappedObject].VertexList[v1].MapTransformedPosition.y,
						   ActualScene->ObjectList[MappedObject].VertexList[v1].MapTransformedPosition.z);

				if (ColorSelector->ItemList[0].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[0];
				if (ColorSelector->ItemList[1].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[1];
				if (ColorSelector->ItemList[2].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[2];
				if (ColorSelector->ItemList[3].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[3];
				if (ColorSelector->ItemList[4].ButtonPushed) weight=(ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[0]+ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[1]+ActualScene->ObjectList[MappedObject].VertexList[v2].Weight[2])/3.0f;
				if (ActualScene->ObjectList[MappedObject].VertexList[v2].Selected)
				glColor4f(weight,weight,0,1);
				else
				glColor4f(0.5,0.5,0.5,1);
				glVertex3f(ActualScene->ObjectList[MappedObject].VertexList[v2].MapTransformedPosition.x,
					       ActualScene->ObjectList[MappedObject].VertexList[v2].MapTransformedPosition.y,
						   ActualScene->ObjectList[MappedObject].VertexList[v2].MapTransformedPosition.z);

				if (ColorSelector->ItemList[0].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[0];
				if (ColorSelector->ItemList[1].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[1];
				if (ColorSelector->ItemList[2].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[2];
				if (ColorSelector->ItemList[3].ButtonPushed) weight=ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[3];
				if (ColorSelector->ItemList[4].ButtonPushed) weight=(ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[0]+ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[1]+ActualScene->ObjectList[MappedObject].VertexList[v3].Weight[2])/3.0f;
				if (ActualScene->ObjectList[MappedObject].VertexList[v3].Selected)
				glColor4f(weight,weight,0,1);
				else
				glColor4f(0.5,0.5,0.5,1);
				glVertex3f(ActualScene->ObjectList[MappedObject].VertexList[v3].MapTransformedPosition.x,
					       ActualScene->ObjectList[MappedObject].VertexList[v3].MapTransformedPosition.y,
						   ActualScene->ObjectList[MappedObject].VertexList[v3].MapTransformedPosition.z);
			}
			glEnd();
			glPopMatrix();
		}
		if (PolySelect)
		{

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glViewport(View->x1,YRes-View->y2,max(1,View->x2-View->x1+1),max(1,View->y2-View->y1+1));
			SetCameraView(&View->Cam,4/3.0f,1,mx,my);

			unsigned int *buffer=new unsigned int[4*ActualScene->ObjectList[PolySelectObject].PolygonNum];
			memset(buffer,0,sizeof(int)*4*ActualScene->ObjectList[PolySelectObject].PolygonNum);

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
			int k;
			for (k=1; k<hits; k++)
			{
				if (buffer[k*4+1]<(GLuint)depth)
				{
					smallest=buffer[k*4+3];
					depth=buffer[k*4+1];
				}
			}

			delete buffer;

			glDisable(GL_BLEND);
			glColor4f(1,0,0,0);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glLineWidth(3);
			glDepthFunc(GL_LEQUAL);
			
			glPushMatrix();
			MultModelViewMatrix(ActualScene->ObjectList[PolySelectObject].ModelView,1);
			glCallList(ActualScene->ObjectList[PolySelectObject].WireDisplayList);

			glEnable(GL_BLEND);
			glColor4f(1,0,0,0.5);
			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);
			glBegin(GL_TRIANGLES);

			for (int zi=0; zi<ActualScene->ObjectList[PolySelectObject].PolygonNum; zi++)
			if (ActualScene->ObjectList[PolySelectObject].PolygonList[zi].Selected)
			{
				glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v1].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v1].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v1].Position.z);
				glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v2].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v2].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v2].Position.z);
				glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v3].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v3].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[zi].v3].Position.z);
			}
			
			if (hits || BrushList->ItemList[2].ActiveBranch)
			{
				PVSelect SelectData;
				memset(&SelectData,0,sizeof(SelectData));
				
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

				Obj_DoPolySelection(&ActualScene->ObjectList[PolySelectObject],&SelectData,true);

				//glEnable(GL_BLEND);
				//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);
				//glBegin(GL_TRIANGLES);
				glColor4f(0,0,1,0.5);
				for (k=0; k<ActualScene->ObjectList[PolySelectObject].PolygonNum; k++)
				if (ActualScene->ObjectList[PolySelectObject].PolygonList[k].Highlighted)
				{
					glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v1].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v1].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v1].Position.z);
					glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v2].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v2].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v2].Position.z);
					glVertex3f(ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v3].Position.x,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v3].Position.y,ActualScene->ObjectList[PolySelectObject].VertexList[ActualScene->ObjectList[PolySelectObject].PolygonList[k].v3].Position.z);
				}
				//glEnd();

			}

			glEnd();

			glPopMatrix();

			glPopAttrib();
		}
	}

	int Mask[3]; Mask[0]=1; Mask[1]=1; Mask[2]=1;
	Mask[View->k]=0;

	glDepthFunc(GL_ALWAYS);
	if (View->WireFrame && ActualScene)
	{
		glBegin(GL_LINES);
		glColor4f(1,0,0,1);
		glVertex3f(ActualScene->Camera.Eye.a[0]*v.x,
				   ActualScene->Camera.Eye.a[1]*v.y,
				   ActualScene->Camera.Eye.a[2]*v.z);
		glColor4f(1,1,0,1);
		glVertex3f(ActualScene->Camera.Target.a[0]*v.x,
				   ActualScene->Camera.Target.a[1]*v.y,
				   ActualScene->Camera.Target.a[2]*v.z);
		glEnd();
	}
	glColor4f(1,1,1,1);
	if (View->WireFrame && ActualScene)
		for (int x=0; x<8; x++)
			if (ActualScene->Lights[x].Lit)
			{
				if (ActualScene->Lights[x].Spot_Cutoff==180 && ActualScene->Lights[x].Position[3]==0) //dirlight
				{
					glBegin(GL_LINES);
					glVertex3f(View->Cam.Eye.a[0]*v.x,
							   View->Cam.Eye.a[1]*v.y,
							   View->Cam.Eye.a[2]*v.z);
					glVertex3f((ActualScene->Lights[x].Position[0]+View->Cam.Eye.a[0])*v.x,
							   (ActualScene->Lights[x].Position[1]+View->Cam.Eye.a[1])*v.y,
							   (ActualScene->Lights[x].Position[2]+View->Cam.Eye.a[2])*v.z);
					glEnd();
				}

				if (ActualScene->Lights[x].Spot_Cutoff==180 && ActualScene->Lights[x].Position[3]==1) //dotlight
				{
					glBegin(GL_LINES);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glEnd();
				}

				if (ActualScene->Lights[x].Position[3]==1 && ActualScene->Lights[x].Spot_Cutoff!=180) //spotlight
				{
					glBegin(GL_LINES);
					glVertex3f(ActualScene->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x+View->i.a[0]*XSize/40.0f-View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y+View->i.a[1]*XSize/40.0f-View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z+View->i.a[2]*XSize/40.0f-View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x-View->i.a[0]*XSize/40.0f+View->j.a[0]*YSize/30.0f,
							   ActualScene->Lights[x].Position[1]*v.y-View->i.a[1]*XSize/40.0f+View->j.a[1]*YSize/30.0f,
							   ActualScene->Lights[x].Position[2]*v.z-View->i.a[2]*XSize/40.0f+View->j.a[2]*YSize/30.0f);

					glVertex3f(ActualScene->Lights[x].Position[0]*v.x,
							   ActualScene->Lights[x].Position[1]*v.y,
							   ActualScene->Lights[x].Position[2]*v.z);
					glVertex3f((ActualScene->Lights[x].Position[0]+ActualScene->Lights[x].Spot_Direction[0])*v.x,
							   (ActualScene->Lights[x].Position[1]+ActualScene->Lights[x].Spot_Direction[1])*v.y,
							   (ActualScene->Lights[x].Position[2]+ActualScene->Lights[x].Spot_Direction[2])*v.z);
					glEnd();
				}
			}

	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	if (ActualScene && MainScroller->ParentItem->SelectedItem==3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8)
	{
		if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[1].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
		DisplayRotateTool(View);
		if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[2].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
		DisplayMoveTool(View);
		if (View->ParentInterface->ItemList[1].Interface->ItemList[2].Interface->ItemList[0].ButtonPushed && View->ParentInterface->ItemList[1].Interface->ItemList[1].ButtonPushed)
		DisplayScaleTool(View);
	}
	SwitchTo2D();
	if (View->WireFrame)
	{
		glColor4f(highblue);
		glBegin(GL_LINES);
		glVertex2f(View->x1+7.0f,View->y2-7.0f);
		glVertex2f(View->x1+50.0f,View->y2-7.0f);
		glVertex2f(View->x1+7.0f,View->y2-7.0f);
		glVertex2f(View->x1+7.0f,View->y2-50.0f);
		glEnd();
		glRasterPos2i(View->x1+10,View->y2-43);
		glPrint(DirNames[View->j1],base);
		glRasterPos2i(View->x1+43,View->y2-10);
		glPrint(DirNames[View->i1],base);
	}
	glColor4f(0,0,0,0);
	if (View->WireFrame)
	glRasterPos2i(View->x1+31,View->y1+12);
	else
	glRasterPos2i(View->x1+31+19,View->y1+12);
	glPrint(View->Text,base);
	glColor4f(1,1,1,1);
	if (View->WireFrame)
	glRasterPos2i(View->x1+30,View->y1+11);
	else
	glRasterPos2i(View->x1+30+19,View->y1+11);
	glPrint(View->Text,base);

	if (!View->WireFrame)
	{
		char text[100];
		sprintf(text,"VCOUNT: %.6d, PCOUNT: %.6d, CCOUNT: %.6d\0",VertexCounter,PolyCounter,CallCounter);
		glRasterPos2i(View->x1+5,View->y2-4);
		glPrint(text,base);
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(highblue);
	rectangle(View->x1,View->y1,View->x2,View->y2);
	glColor4f(1,1,1,1);
	glDepthFunc(GL_LESS);
	glDisable(GL_SCISSOR_TEST);
	glAlphaFunc(GL_GEQUAL,0);
	//glPopAttrib();
}

void cINTERFACE::AddGUIItem()
{
	if (ItemCapacity==ItemNum)
	{
		ItemCapacity=(ItemCapacity+10)*2;
		GUIITEM *Buffer=new GUIITEM[ItemCapacity];
		memset(Buffer,0,ItemCapacity*sizeof(GUIITEM));
		memcpy(Buffer,ItemList,ItemNum*sizeof(GUIITEM));
		delete ItemList;
		ItemList=Buffer;
	}

	ItemList[ItemNum].OnWheelAfter=nop;
	ItemList[ItemNum].HotKey=-1;
	ItemList[ItemNum].ParentInterface=this;
	ItemNum++;
}

cINTERFACE *cINTERFACE::AddViewPort(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2)
{
	AddGUIItem();
	ItemList[ItemNum-1].ItemId=GUI_VIEWPORT;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X2;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].ToolTip="MAIN VIEWPORT (DRAG: START; CTRL+DRAG: END; SHIFT+DRAG: BOTH)\0";

	ItemList[ItemNum-1].OnLeftClick=GUI_ViewPortLeftClick;
	ItemList[ItemNum-1].OnRightClick=nop;
	ItemList[ItemNum-1].OnMidClick=GUI_ViewPortMidClick;
	ItemList[ItemNum-1].OnWheel=GUI_ViewPortWheel;
	ItemList[ItemNum-1].OnWheelAfter=nop;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::Add3dView(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, char *Text, bool WireFrame, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();
	ItemList[ItemNum-1].ItemId=GUI_3DVIEW;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X2;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y2;

	ItemList[ItemNum-1].t1=10;
	ItemList[ItemNum-1].t2=16;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);

	ItemList[ItemNum-1].OnLeftClick=GUI_3dLeftClick;
	ItemList[ItemNum-1].OnRightClick=GUI_3dRightClick;
	ItemList[ItemNum-1].OnMidClick=GUI_MiddleClickFollowCursor;
	ItemList[ItemNum-1].OnWheel=GUI_3dViewCheck;
	ItemList[ItemNum-1].WireFrame=WireFrame;

	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	int I=ItemNum-1;

	cINTERFACE *SubInterface=this->AddSubInterface();
	if (WireFrame)
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+1,ITEM_RECT_Y1,FullScreenButton,"SWITCH BIG / SMALL WINDOW",GUI_ChangeBigSmallWindow,nop,nop,nop);
	else
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+1,ITEM_RECT_Y1,FullScreenButton,"SWITCH BIG / SMALL WINDOW ( ` )",GUI_ChangeBigSmallWindow,nop,nop,nop,'`');
	//if (WireFrame)
	{
		SubInterface->AddSkinnedButton(ITEM_RECT_X2-19,ITEM_RECT_Y1,UberToolButton,"TURN OBJECT TRANSFORMATION TOOL ON / OFF ( SPACE )",GUI_ButtonSwitch,nop,nop,nop,' ');
		SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
		cINTERFACE *I=SubInterface->AddSubInterface();
		I->AddSkinnedButton(ITEM_RECT_X2-19*2,ITEM_RECT_Y1,ScaleButton,"SCALE OBJECT ( r )",GUI_ButtonSelect,nop,nop,nop,'r');
		I->AddSkinnedButton(ITEM_RECT_X2-19*3,ITEM_RECT_Y1,RotateButton,"ROTATE OBJECT ( e )",GUI_ButtonSelect,nop,nop,nop,'e');
		I->AddSkinnedButton(ITEM_RECT_X2-19*4,ITEM_RECT_Y1,MoveButton,"MOVE OBJECT ( w )",GUI_ButtonSelect,nop,nop,nop,'w');
		if (WireFrame)
		I->AddSkinnedButton(ITEM_RECT_X2-19*5,ITEM_RECT_Y1,SkewButton,"SKEW OBJECT ( q )",GUI_ButtonSelect,nop,nop,nop,'q');
		I->ItemList[2].ButtonPushed=true;
		I->SetActiveBranch(2);
		SubInterface->AddSkinnedButton(ITEM_RECT_X2-19,ITEM_RECT_Y1+19,ObjSpaceButton,"OBJECT SPACE TRANSFORMATION ON / OFF",GUI_ButtonSwitch,nop,nop,nop);
		SubInterface->AddSkinnedButton(ITEM_RECT_X2-19,ITEM_RECT_Y1+19*2,CenterButton,"TRANSFORM AROUND OBJECT CENTER / PIVOT",GUI_ButtonSwitch,nop,nop,nop);
		//SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
	}

	SubInterface->AddSkinnedButton(ITEM_RECT_X1+1,ITEM_RECT_Y1+19,GridButton,"TURN GRID ON/OFF ( g )",GUI_ButtonSwitch,nop,nop,nop,'g');
	SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
	if (!WireFrame)
	{
		SubInterface->AddSkinnedButton(ITEM_RECT_X1+20,ITEM_RECT_Y1,ParticlesButton,"TURN PARTICLES ON/OFF ( p )",GUI_ButtonSwitch,nop,nop,nop,'p');
		SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
	}

	return ItemList[I].Interface;
}

cINTERFACE *cINTERFACE::AddL3dView(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, char *Text, bool WireFrame, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();
	ItemList[ItemNum-1].ItemId=GUI_L3DVIEW;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X2;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y2;

	ItemList[ItemNum-1].t1=10;
	ItemList[ItemNum-1].t2=16;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);

	ItemList[ItemNum-1].OnLeftClick=GUI_L3dLeftClick;
	ItemList[ItemNum-1].OnRightClick=GUI_L3dRightClick;
	ItemList[ItemNum-1].OnMidClick=GUI_LMiddleClickFollowCursor;
	ItemList[ItemNum-1].OnWheel=GUI_L3dViewCheck;
	ItemList[ItemNum-1].WireFrame=WireFrame;

	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	int I=ItemNum-1;

	cINTERFACE *SubInterface=this->AddSubInterface();
	if (WireFrame)
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+1,ITEM_RECT_Y1,FullScreenButton,"SWITCH BIG / SMALL WINDOW",GUI_ChangeBigSmallWindow,nop,nop,nop);
	else
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+1,ITEM_RECT_Y1,FullScreenButton,"SWITCH BIG / SMALL WINDOW ( ` )",GUI_ChangeBigSmallWindow,nop,nop,nop,'`');
	//if (WireFrame)
	{
		SubInterface->AddSkinnedButton(ITEM_RECT_X2-19,ITEM_RECT_Y1,UberToolButton,"TURN OBJECT TRANSFORMATION TOOL ON / OFF ( SPACE )",GUI_ButtonSwitch,nop,nop,nop,' ');
		SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
		cINTERFACE *I=SubInterface->AddSubInterface();
		I->AddSkinnedButton(ITEM_RECT_X2-19*2,ITEM_RECT_Y1,ScaleButton,"SCALE OBJECT ( e )",GUI_ButtonSelect,nop,nop,nop,'e');
		I->AddSkinnedButton(ITEM_RECT_X2-19*3,ITEM_RECT_Y1,RotateButton,"ROTATE OBJECT ( w )",GUI_ButtonSelect,nop,nop,nop,'w');
		I->AddSkinnedButton(ITEM_RECT_X2-19*4,ITEM_RECT_Y1,MoveButton,"MOVE OBJECT ( q )",GUI_ButtonSelect,nop,nop,nop,'q');
		I->ItemList[2].ButtonPushed=true;
		I->SetActiveBranch(2);
		SubInterface->AddSkinnedButton(ITEM_RECT_X2-19,ITEM_RECT_Y1+19,ObjSpaceButton,"OBJECT SPACE TRANSFORMATION ON / OFF",GUI_ButtonSwitch,nop,nop,nop);
		SubInterface->AddSkinnedButton(ITEM_RECT_X2-19,ITEM_RECT_Y1+19*2,CenterButton,"TRANSFORM AROUND OBJECT CENTER / PIVOT",GUI_ButtonSwitch,nop,nop,nop);
		//SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
	}

	SubInterface->AddSkinnedButton(ITEM_RECT_X1+1,ITEM_RECT_Y1+19,GridButton,"TURN GRID ON/OFF ( g )",GUI_ButtonSwitch,nop,nop,nop,'g');
	SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
	if (!WireFrame)
	{
		SubInterface->AddSkinnedButton(ITEM_RECT_X1+20,ITEM_RECT_Y1,ParticlesButton,"TURN PARTICLES ON/OFF ( p )",GUI_ButtonSwitch,nop,nop,nop,'p');
		SubInterface->ItemList[SubInterface->ItemNum-1].ButtonPushed=true;
	}

	return ItemList[I].Interface;
}


cINTERFACE *cINTERFACE::AddButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, int TEXT_X, int TEXT_Y, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_BUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].t1=TEXT_X;
	ItemList[ItemNum-1].t2=TEXT_Y;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, int TEXT_X, int TEXT_Y, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, int HotKey)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_BUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].t1=TEXT_X;
	ItemList[ItemNum-1].t2=TEXT_Y;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	ItemList[ItemNum-1].HotKey=HotKey;

	return ItemList[ItemNum-1].Interface;
}


cINTERFACE *cINTERFACE::AddButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, int TEXT_X, int TEXT_Y, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, void **data)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_BUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].t1=TEXT_X;
	ItemList[ItemNum-1].t2=TEXT_Y;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	*data=&ItemList[ItemNum-1].ButtonPushed;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddVerticalBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int *Position, int *ItemNumber)
{
	AddGUIItem();

	ItemList[ItemNum-1].ItemId=GUI_VERTICALBAR;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+16;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+Size;
	ItemList[ItemNum-1].OnLeftClick=GUI_PushWheel;
	ItemList[ItemNum-1].OnRightClick=nop;
	ItemList[ItemNum-1].OnMidClick=nop;
	ItemList[ItemNum-1].OnWheel=GUI_ScrollWheel;
	ItemList[ItemNum-1].SkinRect=ScrollButton;
	ItemList[ItemNum-1].data=Position;
	ItemList[ItemNum-1].data2=ItemNumber;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddItemList(int ITEM_RECT_X1, int ITEM_RECT_Y1, int XSize, int DisplayedNum, ITEMLIST *List, char *Text)
{
	AddGUIItem();

	ItemList[ItemNum-1].ItemId=GUI_ITEMLIST;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+XSize;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+11*DisplayedNum+3;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].OnLeftClick=GUI_OnScrollLeftClick;
	ItemList[ItemNum-1].OnRightClick=GUI_OnScrollRightClick;
	ItemList[ItemNum-1].OnMidClick=GUI_OnScrollMiddleClick;
	ItemList[ItemNum-1].OnWheel=GUI_ListWheel;
	ItemList[ItemNum-1].data=List;
	ItemList[ItemNum-1].BarSize=DisplayedNum;
	int i=ItemNum-1;

	AddSkinnedButton(ITEM_RECT_X1+XSize+1,ITEM_RECT_Y1,UpButton,"SCROLL UP",GUI_DecScrollBarStart,nop,nop,nop);
	AddSkinnedButton(ITEM_RECT_X1+XSize+1,ITEM_RECT_Y1+11*DisplayedNum-13,DownButton,"SCROLL DOWN",GUI_IncScrollBarStart,nop,nop,nop);
	AddVerticalBar(ITEM_RECT_X1+XSize+1,ITEM_RECT_Y1+16,11*DisplayedNum-29,&(ItemList[i].StartItem),&(ItemList[i].ItemNum));

	ItemList[ItemNum-1].BarSize=DisplayedNum;

	ItemList[i].Interface=new cINTERFACE;
	memset(ItemList[i].Interface,0,sizeof(cINTERFACE));
	ItemList[i].Interface->ParentItem=&(ItemList[i]);
	return ItemList[i].Interface;
}

cINTERFACE *cINTERFACE::AddByteProgressBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();

	ItemList[ItemNum-1].ItemId=GUI_BYTEPROGRESSBAR;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+Size;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].Color=Color;
	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].iMin=Min;
	ItemList[ItemNum-1].iMax=Max;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	if (Data)
	*(unsigned char*)Data=Default;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddFloatProgressBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Default, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();

	ItemList[ItemNum-1].ItemId=GUI_FLOATPROGRESSBAR;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+Size;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].Color=Color;
	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].fMin=Min;
	ItemList[ItemNum-1].fMax=Max;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	if (Data)
	*(float*)Data=Default;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddIntProgressBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();

	ItemList[ItemNum-1].ItemId=GUI_INTPROGRESSBAR;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+Size;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].Color=Color;
	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].iMin=Min;
	ItemList[ItemNum-1].iMax=Max;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	if (Data)
	*(int*)Data=Default;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddIntBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecIntBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;
	SubInterface->AddIntProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,Data,Min,Max,Default,Text,ToolTip,GUI_ChangeIntBarValue,nop,nop,GUI_ChangeIntBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncIntBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddIntBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip, GUICALLBACK Controll)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+BarLeft.XRes*2+Size-2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].OnWheelAfter=Controll;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecIntBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;
	SubInterface->AddIntProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,Data,Min,Max,Default,Text,ToolTip,GUI_ChangeIntBarValue,nop,nop,GUI_ChangeIntBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncIntBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddIntBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, int *Data, int Min, int Max, int Default, char *Text, char *ToolTip, GUICALLBACK Controll, GUICALLBACK ControllAfter)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+BarLeft.XRes*2+Size-2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].OnWheel=ControllAfter;
	ItemList[ItemNum-1].OnWheelAfter=Controll;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecIntBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;
	SubInterface->AddIntProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,Data,Min,Max,Default,Text,ToolTip,GUI_ChangeIntBarValue,nop,nop,GUI_ChangeIntBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncIntBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddByteBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecByteBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;
	SubInterface->AddByteProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,(unsigned char*)Data,Min,Max,Default,Text,ToolTip,GUI_ChangeByteBarValue,nop,nop,GUI_ChangeByteBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncByteBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddByteBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip, GUICALLBACK Controll)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+BarLeft.XRes*2+Size-2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].OnWheelAfter=Controll;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecByteBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;
	SubInterface->AddByteProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,(unsigned char*)Data,Min,Max,Default,Text,ToolTip,GUI_ChangeByteBarValue,nop,nop,GUI_ChangeByteBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncByteBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddByteBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, unsigned char *Data, unsigned char Min, unsigned char Max, unsigned char Default, char *Text, char *ToolTip, GUICALLBACK Controll, GUICALLBACK ControllAfter)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+BarLeft.XRes*2+Size-2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].OnWheelAfter=Controll;
	ItemList[ItemNum-1].OnWheel=ControllAfter;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecByteBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;
	SubInterface->AddByteProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,(unsigned char*)Data,Min,Max,Default,Text,ToolTip,GUI_ChangeByteBarValue,nop,nop,GUI_ChangeByteBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncByteBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].iMax=Max;

	return SubInterface;
}


cINTERFACE *cINTERFACE::AddFloatBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Skip, float Default, char *Text, char *ToolTip)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecFloatBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMax=Max;
	SubInterface->ItemList[SubInterface->ItemNum-1].fSkip=Skip;
	SubInterface->AddFloatProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,(float*)Data,Min,Max,Default,Text,ToolTip,GUI_ChangeFloatBarValue,nop,nop,GUI_ChangeFloatBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncFloatBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMax=Max;
	SubInterface->ItemList[SubInterface->ItemNum-1].fSkip=Skip;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddFloatBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Skip, float Default, char *Text, char *ToolTip, GUICALLBACK Controll)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+BarLeft.XRes*2+Size-2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].OnWheelAfter=Controll;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecFloatBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMax=Max;
	SubInterface->ItemList[SubInterface->ItemNum-1].fSkip=Skip;
	SubInterface->AddFloatProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,(float*)Data,Min,Max,Default,Text,ToolTip,GUI_ChangeFloatBarValue,nop,nop,GUI_ChangeFloatBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncFloatBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMax=Max;
	SubInterface->ItemList[SubInterface->ItemNum-1].fSkip=Skip;

	return SubInterface;
}

cINTERFACE *cINTERFACE::AddFloatBar(int ITEM_RECT_X1, int ITEM_RECT_Y1, int Size, int Color, float *Data, float Min, float Max, float Skip, float Default, char *Text, char *ToolTip, GUICALLBACK Controll, GUICALLBACK ControllAfter)
{
	cINTERFACE *SubInterface=this->AddSubInterface();

	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+BarLeft.XRes*2+Size-2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+BarLeft.YRes;
	ItemList[ItemNum-1].OnWheelAfter=Controll;
	ItemList[ItemNum-1].OnWheel=ControllAfter;

	SubInterface->AddSkinnedButton(ITEM_RECT_X1,ITEM_RECT_Y1,BarLeft,"DECREASE VALUE",GUI_DecFloatBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMax=Max;
	SubInterface->ItemList[SubInterface->ItemNum-1].fSkip=Skip;
	SubInterface->AddFloatProgressBar(ITEM_RECT_X1+BarLeft.XRes+1,ITEM_RECT_Y1,Size-2,Color,(float*)Data,Min,Max,Default,Text,ToolTip,GUI_ChangeFloatBarValue,nop,nop,GUI_ChangeFloatBarValueWheel);
	SubInterface->AddSkinnedButton(ITEM_RECT_X1+Size+BarLeft.XRes,ITEM_RECT_Y1,BarRight,"INCREASE VALUE",GUI_IncFloatBarValue,nop,nop,nop);
	SubInterface->ItemList[SubInterface->ItemNum-1].data=Data;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMin=Min;
	SubInterface->ItemList[SubInterface->ItemNum-1].fMax=Max;
	SubInterface->ItemList[SubInterface->ItemNum-1].fSkip=Skip;

	return SubInterface;
}


cINTERFACE *cINTERFACE::AddSkinnedButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_SKINNEDBUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].SkinRect=ButtonSkin;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddSkinnedButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, void **data)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_SKINNEDBUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].SkinRect=ButtonSkin;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	*data=&ItemList[ItemNum-1].ButtonPushed;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddSkinnedButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin, char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel, int HotKey)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_SKINNEDBUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].SkinRect=ButtonSkin;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	ItemList[ItemNum-1].HotKey=HotKey;

	return ItemList[ItemNum-1].Interface;
}


cINTERFACE *cINTERFACE::AddRadioButton(int ITEM_RECT_X1, int ITEM_RECT_Y1, char *Text,char *ToolTip, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();
	SKINRECTANGLE ButtonSkin=RadioButton;
	
	ItemList[ItemNum-1].ItemId=GUI_RADIOBUTTON;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X1+ButtonSkin.XRes;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y1+ButtonSkin.YRes;
	ItemList[ItemNum-1].SkinRect=ButtonSkin;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].ToolTip=new char[strlen(ToolTip)+1];
	sprintf(ItemList[ItemNum-1].ToolTip,"%s\0",ToolTip);
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddText(int ITEM_RECT_X1, int ITEM_RECT_Y1, char *Text)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_TEXT;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].Text=new char[strlen(Text)+1];
	sprintf(ItemList[ItemNum-1].Text,"%s\0",Text);
	ItemList[ItemNum-1].OnLeftClick=nop;
	ItemList[ItemNum-1].OnRightClick=nop;
	ItemList[ItemNum-1].OnMidClick=nop;
	ItemList[ItemNum-1].OnWheel=nop;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddIntDisplay(int ITEM_RECT_X1, int ITEM_RECT_Y1, int *Data)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_INTDISPLAY;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].data=Data;
	ItemList[ItemNum-1].OnLeftClick=nop;
	ItemList[ItemNum-1].OnRightClick=nop;
	ItemList[ItemNum-1].OnMidClick=nop;
	ItemList[ItemNum-1].OnWheel=nop;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	return ItemList[ItemNum-1].Interface;
}


cINTERFACE *cINTERFACE::AddImage(int ITEM_RECT_X1, int ITEM_RECT_Y1, SKINRECTANGLE ButtonSkin)
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_IMAGE;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=-2;
	ItemList[ItemNum-1].y2=-2;
	ItemList[ItemNum-1].SkinRect=ButtonSkin;
	ItemList[ItemNum-1].ToolTip=ToolTip;
	ItemList[ItemNum-1].OnLeftClick=nop;
	ItemList[ItemNum-1].OnRightClick=nop;
	ItemList[ItemNum-1].OnMidClick=nop;
	ItemList[ItemNum-1].OnWheel=nop;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddTextureDisplay(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2, GLuint *Texture, GUICALLBACK OnLeftClick, GUICALLBACK OnRightClick, GUICALLBACK OnMidClick, GUICALLBACK OnWheel)
{
	AddGUIItem();

	ItemList[ItemNum-1].ItemId=GUI_TEXTUREDISPLAY;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].ox1=ITEM_RECT_X1;
	ItemList[ItemNum-1].oy1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].ox2=ITEM_RECT_X2;
	ItemList[ItemNum-1].oy2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].OnLeftClick=OnLeftClick;
	ItemList[ItemNum-1].OnRightClick=OnRightClick;
	ItemList[ItemNum-1].OnMidClick=OnMidClick;
	ItemList[ItemNum-1].OnWheel=OnWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	ItemList[ItemNum-1].Texture=Texture;

	return ItemList[ItemNum-1].Interface;

}

cINTERFACE *cINTERFACE::AddSubInterface()
{
	AddGUIItem();
	
	ItemList[ItemNum-1].ItemId=GUI_SUBINTERFACE;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];
	ItemList[ItemNum-1].OnLeftClick=nop;
	ItemList[ItemNum-1].OnRightClick=nop;
	ItemList[ItemNum-1].OnMidClick=nop;
	ItemList[ItemNum-1].OnWheel=nop;
	ItemList[ItemNum-1].ActiveBranch=true;

	return ItemList[ItemNum-1].Interface;
}

cINTERFACE *cINTERFACE::AddTimeliner(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2)
{
	AddGUIItem();
	int i=ItemNum-1;

	ItemList[ItemNum-1].ItemId=GUI_TIMELINER;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2*10+ITEM_RECT_Y1;
	ItemList[ItemNum-1].BarSize=ITEM_RECT_Y2;
	ItemList[ItemNum-1].OnLeftClick=GUI_TimelinerLeftClick;
	ItemList[ItemNum-1].OnRightClick=GUI_TimelinerRightClick;
	ItemList[ItemNum-1].OnMidClick=GUI_TimelinerMidClick;
	ItemList[ItemNum-1].OnWheel=GUI_TimelinerWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	ItemList[ItemNum-1].ItemNum=256;
	ItemList[ItemNum-1].StartItem=0;
	ItemList[ItemNum-1].ToolTip="MAIN TIMELINE";
	ItemList[ItemNum-1].FrameNum=60000;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	return ItemList[i].Interface;
}

cINTERFACE *cINTERFACE::AddSplineEditor(int ITEM_RECT_X1, int ITEM_RECT_Y1, int ITEM_RECT_X2, int ITEM_RECT_Y2)
{
	AddGUIItem();
	int i=ItemNum-1;

	ItemList[ItemNum-1].ItemId=GUI_SPLINEEDITOR;
	ItemList[ItemNum-1].InterfaceItemId=ItemNum-1;
	ItemList[ItemNum-1].x1=ITEM_RECT_X1;
	ItemList[ItemNum-1].y1=ITEM_RECT_Y1;
	ItemList[ItemNum-1].x2=ITEM_RECT_X2;
	ItemList[ItemNum-1].y2=ITEM_RECT_Y2;
	ItemList[ItemNum-1].BarSize=ITEM_RECT_Y2;
	ItemList[ItemNum-1].OnLeftClick=GUI_SplineEditorLeftClick;
	ItemList[ItemNum-1].OnRightClick=GUI_SplineEditorRightClick;
	ItemList[ItemNum-1].OnMidClick=GUI_SplineEditorMidClick;
	ItemList[ItemNum-1].OnWheel=GUI_SplineEditorWheel;
	ItemList[ItemNum-1].Interface=new cINTERFACE;
	ItemList[ItemNum-1].ItemNum=256;
	ItemList[ItemNum-1].StartItem=0;
	ItemList[ItemNum-1].ToolTip="SPLINE EDITOR";
	ItemList[ItemNum-1].FrameNum=60000;
	memset(ItemList[ItemNum-1].Interface,0,sizeof(cINTERFACE));
	ItemList[ItemNum-1].Interface->ParentItem=&ItemList[ItemNum-1];

	return ItemList[i].Interface;
}

void cINTERFACE::DrawGUI()
{
	int x;
	for (x=0; x<ItemNum; x++)
	{
		switch (ItemList[x].ItemId)
		{
		case GUI_BUTTON:
			{
				DrawButton(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2,ItemList[x].t1,ItemList[x].t2,ItemList[x].LeftClickInWindow || ItemList[x].ButtonPushed || ItemList[x].Reading,ItemList[x].MouseInWindow,ItemList[x].Text);
				break;
			}
		case GUI_SKINNEDBUTTON:
			{
				DrawSkinnedButton(ItemList[x].x1,ItemList[x].y1,ItemList[x].SkinRect,ItemList[x].LeftClickInWindow || ItemList[x].ButtonPushed,ItemList[x].MouseInWindow,ItemList[x].Text);
				break;
			}
		case GUI_TEXTUREDISPLAY:
			{
				if (!ItemList[x].LayouterDisplay)
				{
					DrawTexture(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2,ItemList[x].ButtonPushed,ItemList[x].Texture);
					if (MainGUI.ItemList[2].ButtonPushed)
					{
						line((int)(ItemList[x].x1+(ItemList[x].x2-ItemList[x].x1)*ItemList[x].Offx),ItemList[x].y1,(int)(ItemList[x].x1+(ItemList[x].x2-ItemList[x].x1)*ItemList[x].Offx),ItemList[x].y2);
						line(ItemList[x].x1,(int)(ItemList[x].y1+(ItemList[x].y2-ItemList[x].y1)*ItemList[x].Offy),ItemList[x].x2,(int)(ItemList[x].y1+(ItemList[x].y2-ItemList[x].y1)*ItemList[x].Offy));
					}
				}
				else
				{
					DrawTexture(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2,
								ItemList[x].tx1,ItemList[x].ty1,ItemList[x].tx2,ItemList[x].ty2,
								ItemList[x].ButtonPushed,ItemList[x].Texture);
				}
				break;
			}
		case GUI_IMAGE:
			{
				//glPushAttrib(GL_ALL_ATTRIB_BITS);
				
				glEnable(GL_TEXTURE_2D);
				/*glDisable(GL_BLEND);
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);*/
				glBindTexture(GL_TEXTURE_2D,Skin[ItemList[x].SkinRect.SkinID-1].Handle);

				texturedquad(ItemList[x].x1,ItemList[x].y1,ItemList[x].x1+ItemList[x].SkinRect.XRes,ItemList[x].y1+ItemList[x].SkinRect.YRes,ItemList[x].SkinRect.X1,ItemList[x].SkinRect.Y1,ItemList[x].SkinRect.X2,ItemList[x].SkinRect.Y2);
				
				glDisable(GL_TEXTURE_2D);
				//glPopAttrib();
				break;
			}
		case GUI_INTPROGRESSBAR:
			{			
				DrawProgressBar(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2-ItemList[x].x1,ItemList[x].Color,(float)(*((int*)ItemList[x].data)-ItemList[x].iMin)/(float)(ItemList[x].iMax-ItemList[x].iMin),ItemList[x].Text,(float)*((int*)ItemList[x].data),true);
				break;
			}
		case GUI_BYTEPROGRESSBAR:
			{			
				DrawProgressBar(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2-ItemList[x].x1,ItemList[x].Color,(float)(*((unsigned char*)ItemList[x].data)-ItemList[x].iMin)/(float)(ItemList[x].iMax-ItemList[x].iMin),ItemList[x].Text,(float)*((unsigned char*)ItemList[x].data),true);
				break;
			}
		case GUI_FLOATPROGRESSBAR:
			{			
				DrawProgressBar(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2-ItemList[x].x1,ItemList[x].Color,(float)(*((float*)ItemList[x].data)-ItemList[x].fMin)/(float)(ItemList[x].fMax-ItemList[x].fMin),ItemList[x].Text,(float)*((float*)ItemList[x].data),false);
				break;
			}
		case GUI_ITEMLIST:
			{
				//glPushAttrib(GL_ALL_ATTRIB_BITS);
				//glDisable(GL_TEXTURE_2D);
				//glDisable(GL_BLEND);
				//glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);
				glScissor(ItemList[x].x1,YRes-ItemList[x].y2-1,max(1,ItemList[x].x2-ItemList[x].x1+1),max(1,ItemList[x].y2-ItemList[x].y1+1)+1);
				glEnable(GL_SCISSOR_TEST);
				glColor4f(highblue);
				rectangle(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2-1);
				int xpos=0;
				if (ItemList[x].data)
				{					
					ITEM *Buffer;
					for (int y=0; y<ItemList[x].BarSize;y++)
					{						
						Level=0;
						Buffer=ReturnItem((ITEMLIST*)ItemList[x].data,y+ItemList[x].StartItem);
						if (Buffer)
						{

							if (y+ItemList[x].StartItem==ItemList[x].SelectedItem)
							{
								glColor4f(darkblue);
								quad(ItemList[x].x1+1,ItemList[x].y1+11*y,ItemList[x].x2,ItemList[x].y1+12+11*y);
							}

							if (!Buffer->Hidden)
							{
								if (Buffer->Selected) glColor4f(lightblue);
								else
									glColor4f(highblue);
							}
							else
							if (Buffer->Selected) glColor4f(0,1,0,1);
							else
								glColor4f(0,0.5,0,1);
							
							if (Buffer->SubItemNum)
							{
								glRasterPos2i(ItemList[x].x1+Level*10+4,ItemList[x].y1+9+11*y);
								if (Buffer->SubOpened) glPrint("-",base); else glPrint("+",base);
								glRasterPos2i(ItemList[x].x1+11+Level*10,ItemList[x].y1+9+11*y);		
							}
							else						
								glRasterPos2i(ItemList[x].x1+5+Level*10,ItemList[x].y1+9+11*y);
							glPrint(Buffer->Name,base);
						}
					}
				}
				glDisable(GL_SCISSOR_TEST);
				glColor4f(1,1,1,1);
				//glPopAttrib();
				break;
			}
		case GUI_VERTICALBAR:
			{
				/*glPushAttrib(GL_ALL_ATTRIB_BITS);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);*/

				glColor4f(40.0/256.0,52.0/256.0,71.0/256.0,1);
				quad(ItemList[x].x1+5,ItemList[x].y1,ItemList[x].x1+12,ItemList[x].y2);

				glColor4f(1,1,1,1);
				
				int a=*((int*)ItemList[x].data),b=*((int*)ItemList[x].data2);
				float r2=(float)a/((float)(b-ItemList[x].BarSize));
				int res=(int)((float)((ItemList[x].y2-ItemList[x].y1)-16)*r2);

				DrawSkinnedButton(ItemList[x].x1,ItemList[x].y1+res,ItemList[x].SkinRect,LeftClickInWindow(ItemList[x].x1,ItemList[x].y1+res,ItemList[x].x2,ItemList[x].y1+res+16) || ItemList[x].ButtonPushed,ItemList[x].MouseInWindow && MouseInWindow(ItemList[x].x1,ItemList[x].y1+res,ItemList[x].x2,ItemList[x].y1+res+16),ItemList[x].Text);

				//glPopAttrib();

				break;
			}
		case GUI_3DVIEW:
			{
				Display3dView(&ItemList[x]);
				break;
			}
		case GUI_RADIOBUTTON:
			{
				DrawSkinnedButton(ItemList[x].x1,ItemList[x].y1,ItemList[x].SkinRect,ItemList[x].LeftClickInWindow || ItemList[x].ButtonPushed,ItemList[x].MouseInWindow,ItemList[x].Text);
				glRasterPos2i(ItemList[x].x1+2+14,ItemList[x].y1+10);
				glPrint(ItemList[x].Text,base);				
				break;
			}
		case GUI_TEXT:
			{
				glRasterPos2i(ItemList[x].x1,ItemList[x].y1);
				glPrint(ItemList[x].Text,base);				
				break;
			}
		case GUI_VIEWPORT:
			{
				DisplayViewPort(&ItemList[x]);
				break;
			}
		case GUI_TIMELINER:
			{
				DisplayTimeliner(&ItemList[x]);
				break;
			}
		case GUI_L3DVIEW:
			{
				DisplayL3DView(&ItemList[x]);
				break;
			}
		case GUI_SPLINEEDITOR:
			{
				DisplaySplineEditor(&ItemList[x]);
				break;
			}
		case GUI_INTDISPLAY:
			{
				glRasterPos2i(ItemList[x].x1,ItemList[x].y1);
				char num[20];
				sprintf(num,"%d\0",*(int*)ItemList[x].data);
				glPrint(num,base);
				break;
			}
		}
	}
	for (x=0; x<ItemNum; x++) if (ItemList[x].ActiveBranch && ItemList[x].Interface) ItemList[x].Interface->DrawGUI();
}

void cINTERFACE::CheckGUI()
{
	int x;
	for (x=0; x<ItemNum; x++)
	{
		ItemList[x].MouseInWindow=MouseInWindow(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2) && !ReadLine;
		ItemList[x].LeftClickInWindow=LeftClickInWindow(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2) && !ReadLine;
		ItemList[x].RightClickInWindow=RightClickInWindow(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2) && !ReadLine;
		ItemList[x].MidClickInWindow=MidClickInWindow(ItemList[x].x1,ItemList[x].y1,ItemList[x].x2,ItemList[x].y2) && !ReadLine;
		if (ItemList[x].LeftClickInWindow || (ItemList[x].HotKey!=-1 && Keys[ItemList[x].HotKey] && !CommandLineReading)) 
		{
			ItemList[x].OnLeftClick(this,x);
			//if (ItemList[x].HotKey!=-1 && Keys[ItemList[x].HotKey]) Keys[ItemList[x].HotKey]=false;
		}
		if (ItemList[x].RightClickInWindow) ItemList[x].OnRightClick(this,x);
		if (ItemList[x].MidClickInWindow) ItemList[x].OnMidClick(this,x);
		if ((ItemList[x].MouseInWindow || ItemList[x].LeftClickInWindow) && ItemList[x].ToolTip) 
		{
			ToolTip=ItemList[x].ToolTip;
			if (!ToolTip) ToolTip="";
		}
		if (!ReadLine || ItemList[x].Reading) ItemList[x].OnWheel(this,x);
	}
	for (x=0; x<ItemNum; x++) if (ItemList[x].ActiveBranch && ItemList[x].Interface) ItemList[x].Interface->CheckGUI();
	for (x=0; x<ItemNum; x++)
	{
		if (!ReadLine || ItemList[x].Reading) ItemList[x].OnWheelAfter(this,x);
	}
}
