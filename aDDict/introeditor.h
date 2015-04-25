#ifndef __introeditor__
#define __introeditor__

#include "windows.h"
#include "gl\gl.h"
#include "gl\glu.h"
#include "addict.h"
#include "ddgui.h"
#include "ddwindow.h"
#include "fmod\minifmod.h"

#define layer2d        1
#define layer3d        2
#define cleargl        3
#define rendertotext   4
#define feedback       5
#define grideffect     6

struct introtimeline
{
	int introlength;
};

struct event
{
	char name[40];
	int startframe,endframe; //
	int eventtype; //
	float startrectx,startrecty,startrectxrez,startrectyrez; //
	float endrectx,endrecty,endrectxrez,endrectyrez; //
	int pass; //
	GLuint blendfunc1,blendfunc2;//
	float startcol[4],endcol[4];//
	bool textured;//
	int texxtile, texytile; //
	int sceneid, cameraid,  animid; //
	float camerastart,  cameraend,   animstart, animend; //
	int texture; //
	bool clearzbuffer;
	bool clearscreen;

	int menu1,menu2;
	int icamselected,icampos,isceneselected,iscenepos,ianimselected,ianimpos;
	scene *iscene;
	cameraanimation *icam;
	sceneobjanim *ianim;
	int matselected, matstart;
	GLuint mattexture;

	int startrectx1,startrecty1,startrectx2,startrecty2; //
	int endrectx1,endrecty1,endrectx2,endrecty2; //

	int effect,param1,param2,param3;
	float param4,param5,param6;

	event *next;
};

extern event *eventlist;

void sortevents();
void initintroeditor();
void introeditor(int mode);

#endif