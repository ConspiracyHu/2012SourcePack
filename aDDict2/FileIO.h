#pragma once

#include "memtrack.h"

#include "addict2.h"
#include "GUIinterface.h"
#include "CommonGUIFunctions.h"
#include "MinimalExport.h"

struct ChunkHeader
{
	int ID;
	int Size;
};

extern float UndoStatus2;

void BuildFileMenuGui(cINTERFACE *FileMenu);
void __stdcall GUI_LoadZoom3Scene(cINTERFACE* Interface, int ButtonId);
void __stdcall SaveScene(cINTERFACE* Interface, int ButtonId);
void __stdcall ImportScene(cINTERFACE* Interface, int ButtonId);
void loadsttascene(SCENE &scn, const char *filename);
void loadwdo(SCENE &scn, const char *filename);
