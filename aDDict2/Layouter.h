#pragma once

#include "memtrack.h"

#include "GUIInterface.h"
#include "modellergui.h"
#include "introeditor.h"
#include "effecttemplates.h"

extern WORLD *ActualWorld;
extern WORLD *Worlds,*LastWorld;
extern cINTERFACE *LSplineMenu;

void BuildLayouterGUI(cINTERFACE *Layouter);

extern cINTERFACE *SplineMenuInterface,*CamList,*CameraSplines,*SplineEditorInterface,*MainLayouterScroller,*ObjList,*LightsList,*PrtList,*SwrldList,*SceneSplines,*ParticleSplines,*LightSplines_,*WorldScroller;
extern ITEMLIST *ObjectSplines,*LightSplines,*EmitterSplines,*CamSplines,*SubWorldSplines,*LightList,*MainAnimList;
extern cINTERFACE *SWorldSplines;

extern cINTERFACE *LayouterDisplay1;
extern cINTERFACE *LayouterDisplay2;
extern cINTERFACE *LayouterDisplay3;
extern cINTERFACE *LayouterDisplay4;
extern cINTERFACE *VertToggle,*HorToggle,*AddSWorldInterface;

extern ITEMLIST *WorldListScroller;

extern int ActualAnim;

void __stdcall Scroller_SelectOnlyThisWorld(cINTERFACE* ScrollerInterface, int ScrollerID);
void SetEnvironmentToActualWorld();
void __stdcall GUI_SelectOnlyThisAnim(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall GUI_SelectOnlyThisCam(cINTERFACE* ScrollerInterface, int ScrollerID);

extern cINTERFACE *MatMenu,*VarMatMenu,*ConstMatMenu;

void SetCamSplines();
void SetObjectSplines();
void SetEmitterSplines();
void SetSubWorldSplines();
void SetActualWorldAnimListToMainAnimList();
void SetActualWorldCamListToMainCamList();
