#ifndef __FileIO__
#define __FileIO__

#include "aDDict.h"
#include "engine.h"
#include "stdlib.h"

#define aDDict_Texture   84+88*256+84*256*256+82*256*256*256
#define aDDict_Material  77+84*256+82*256*256+76*256*256*256
#define aDDict_Scene     83+67*256+78*256*256+69*256*256*256
#define aDDict_Selection 83+69*256+76*256*256+46*256*256*256
#define aDDict_Lights    76+73*256+71*256*256+84*256*256*256
#define aDDict_Fog       70+79*256+71*256*256
#define aDDict_Object    79+66*256+74*256*256
#define aDDict_Object2   '2JBO'
#define aDDict_EOF       69+79*256+70*256*256+46*256*256*256
#define aDDict_CameraAni 67+65*256+78*256*256+73*256*256*256
#define aDDict_CameraKey 67+75*256+69*256*256+89*256*256*256
#define aDDict_Event     69+86*256+78*256*256+84*256*256*256
#define aDDict_Event2    0x324e5645
#define aDDict_Anim      0x4d494e41
#define aDDict_AnimKey   0x59454b41

struct andobjdata
{
	unsigned nemtommi  : 1, //FIX 0
			 felbontas : 4, //FIX
             a:1,b:1,c : 1,
             
             curvenum  : 2, // lehet kevesebb is (biztos ;)
             y3        : 1, // lehet kevesebb is (biztos ;)
             y4        : 1, // lehet kevesebb is (biztos ;)
             y5        : 1, // lehet kevesebb is (biztos ;)
             y6        : 1, // lehet kevesebb is (biztos ;)
             y7        : 1, // lehet kevesebb is (biztos ;)
             y8        : 1, // lehet kevesebb is (biztos ;)

             ismetles  : 8, // FIX
             nemtommi2 : 8; // FIX = 0
};

struct rotdata
{
	unsigned x :5,
		   y :5,
           z :5;
};

struct andobj
{
	andobjdata group;
	float x,y,z,xs,ys,zs;
	rotdata rot;
};

struct chunk
{
	int ID;
	int size;
};

struct camfield
{
	unsigned fovwritten : 1,
             rollwritten: 1,
             eyexwritten : 1,
             eyeywritten : 1,
             eyezwritten : 1,
             targetxwritten: 1,
             targetywritten: 1,
             targetzwritten: 1;
};

struct objfield
{
	unsigned posx : 1,
             posy : 1,
             posz : 1,
             rotx : 1,
             roty : 1,
             rotz : 1,
             rota : 1,
             strx : 1,
             stry : 1,
             strz : 1,
             colr : 1,
             colg : 1,
             colb : 1,
             cola : 1;
};


bool loadaDDictscene(scene *scn, tTexture *textures, char *filename, bool objectload,bool selectionload,bool textureload,bool lightload,bool cameraload,bool environmentload);
bool saveaDDictscene(scene &scn, tTexture *textures, const char *filename);
void SaveMinimalScene(scene &scn, tTexture *textures, const char *filename);

void saveminimalproject(const char *filename, byte floatbytes);

void SaveProject(const char *filename);
void LoadProject(const char *filename);
void loadandscene(scene &scn, const char *filename);

#endif