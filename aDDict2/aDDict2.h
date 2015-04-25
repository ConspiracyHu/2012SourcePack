#ifndef __aDDict2__
#define __aDDict2__

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

#include "memtrack.h"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "windowhandler.h"
#include "GUIinterface.h"
#include "aDDict2GUI.h"
#include "Texgen2.h"
#include "3dEngine.h"

#define DataDir "Data"
#define ProjectDir "data/projects"
#define ProjectMsk "data/projects/*.dmo"
#define TextureDir "data/textures"
#define TextureMsk "data/textures/*.tex"
#define JpegDir    "data/Images"
#define JpegMsk    "data/Images/*.jpg"
#define SceneDir   "data/scenes"
#define SceneMsk   "data/scenes/*.scn"
#define AseMsk     "data/scenes/*.ase"
#define ObjMsk     "data/scenes/*.obj"
#define PdoMsk     "data/scenes/*.wdo"
#define MusicDir   "data/musics"
#define MusicMsk   "data/musics/*.wav"
#define MusicMsk2  "data/musics/*.mp3"
#define MusicMsk3  "data/musics/*.ogg"
#define MinimalDir "data/minimal"
#define MinimalMsk "data/minimal/*.min"

extern ITEMLIST *TextureListScroller,*SceneListScroller,*ActualSceneItemList;
extern SCENE *Scenes,*LastScene,*ActualScene,*LayouterSelectedScene;
extern int lastwheel;

// COMMANDLINE CHANGABLE VALUES

extern bool LowRezLoad;

#endif