#ifndef __CommonGUIFunctions__
#define __CommonGUIFunctions__

#include "memtrack.h"

#include "GUIInterface.h"
#include "FindFile.h"

void __stdcall Scroller_SelectItem(cINTERFACE* ScrollerInterface, int ScrollerID);
void __stdcall SubOpenClose(cINTERFACE* ScrollerInterface, int ScrollerID);
ITEM *ScrollerFindItem(ITEMLIST *List, int Number);
void __stdcall Scroller_SelectOnlyThis(cINTERFACE* ScrollerInterface, int ScrollerID);
void SetupFileList(ITEMLIST *List, char *FileMask);
void __stdcall GUI_CheckByteSwitch(cINTERFACE* Interface, int ButtonId);

#endif