#pragma once
#include "WindowHandler.h"
#include "TexGen.h"
#include "3DEngine.h"
#include "ObjectGenerators.h"
#include "IntroEditorEffects.h"
#include "EventHandler.h"

extern bool DisplayPrecalcAnim;

extern int pEventNum;
extern MATERIAL *pMaterialList;
extern SCENE *pSceneList;
extern WORLD *pWorldList;
extern EVENT *pEventList;
extern PSelect *pPolySelectList;

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

#pragma pack(1)

struct OBJECTDATA2
{
	unsigned BackFace :1,
             BackFront : 1,
			 Orientation:1,
             Position:1;
}; //4 bit

#pragma pack()

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

extern int SceneNum;
extern int PolySelNum;
extern int WorldNum;

extern GLuint SRCBlends[];
extern GLuint DSTBlends[];

extern int eventsizes[];
void InitTextureBuffer();

int LoadDataFile(void **Data, MATERIAL **MaterialList, PSelect **PolySelections, SCENE **SceneList, WORLD **WorldList, EVENT **EventList, int LOD);
//void ImportTextures(void **f, MATERIAL **MaterialList);
