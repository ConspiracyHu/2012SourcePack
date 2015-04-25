#ifndef __modellergui__
#define __modellergui__

#include "addict.h"
#include "DDwindow.h"
#include "ddgui.h"
#include "engine.h"

struct window3d {
	int xpos,ypos,oxpos,oypos,xsize,ysize;
	int pos;
	int a,b,c;
	int c1,c2,c3;
	char *xn,*yn;
	float camsensitivity;
	camera cam,cam2;
	bool wireframe;
	bool perspective;
	bool grid;
	bool fullscreen;
	bool showhidden;
	char *name;
	char transformation;
	bool centertransform;
	bool objspacetransform;
	objkeyframedata or;
	int odx,ody;
};

extern window3d modellviews[4];
extern int lightselected;
extern GLuint backrawtex;


int materialnum(material *t);
void InitModellerGUI();
void delete_selected();
void ModellerGUI(unsigned char mode);

void deselectall();
float CalcXGLScene(float z, float fov, float aspect);
float CalcYGLScene(float z, float fov, float aspect);
int polycounter(scene *scn);
void displaywindow(window3d &win, unsigned char mode);

int cntscenes();

#endif