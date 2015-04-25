#pragma once

#include "memtrack.h"
#include "addict2.h"
#include "GuiInterface.h"

#ifndef Public_Release

struct OBJECTDATA
{
	unsigned Wireframe :1,
             NormalsInverted : 1,
			 XSwap     :1,
			 YSwap     :1,
			 Swap      :1,
			 Zmask     :1,
			 AEpsilon  :1,
			 
             SRCBlend  :4,
			 DSTBlend  :3,
             
             Textured  :1,
			 TexSlot   :3,
             Envmapped :1,
             EnvSlot   :3,
			 Shading   :2,

			 XTile     :1,
			 YTile     :1,
			 Offx      :1,
			 Offy      :1,
			 Red       :1,
			 Green     :1,
			 Blue      :1,
			 Alpha     :1;
}; //32 bit

struct OBJECTDATA2
{
	unsigned BackFace :1,
             BackFront:1,
			 Orientation:1,
             Position:1;
}; //4 bit

struct WORLDOBJECTDATA
{
	unsigned Posx :1,
             Posy :1,
			 Posz :1,
			 Sclx :1,
			 Scly :1,
			 Sclz :1,
			 Quat :1,
             EmitterData: 1,

		     //innen emitter data csak

             SRCBlend  :4,
			 DSTBlend  :3,
             
             Textured  :1,
			 TexSlot   :3,
             Tail      :1,
             Head      :1,
			 ObjectHead:1,

			 TailRes1  :4,
			 TailRes2  :4;
}; //31 bit

void ExportMinimalProject(char *FileName);
void CreateIncludeList();

#endif