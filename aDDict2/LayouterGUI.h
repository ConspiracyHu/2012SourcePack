#pragma once

#include "memtrack.h"

#include "GUIInterface.h"

void DisplayL3DView(GUIITEM *View);
void DisplaySplineEditor(GUIITEM *View);
void __stdcall GUI_LMiddleClickFollowCursor(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_L3dViewCheck(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_L3dLeftClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_L3dRightClick(cINTERFACE* Interface, int ButtonId);

void __stdcall GUI_SplineEditorLeftClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_SplineEditorRightClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_SplineEditorMidClick(cINTERFACE* Interface, int ButtonId);
void __stdcall GUI_SplineEditorWheel(cINTERFACE* Interface, int ButtonId);

void __stdcall LCheckTransform(cINTERFACE* Interface, int ScrollerID);

extern float PlaybackPos;
extern int Xzoom;