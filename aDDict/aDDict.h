#ifndef __aDDict__
#define __aDDict__

#define texturedir  "data\\textures\\"
#define texturemask "data\\textures\\*.atg"
#define scenedir    "data\\scenes\\"
#define scenemask   "data\\scenes\\*.a3d"
#define minimaldir  "data\\minimal\\"
#define minimalmask "data\\minimal\\*.min"
#define projectdir  "data\\projects\\"
#define projectmask "data\\projects\\*.64k"
#define musicdir    "data\\musics\\"
#define musicmask   "data\\musics\\*.xm"
#define syncdir    "data\\musics\\"
#define syncmask   "data\\musics\\*.syn"
#define fileheader  "-aDDict-"

#include <windows.h>
#include <math.h>
#include <memory.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "vectors.h"
#include "engine.h"
#include "findfile.h"
#include "DDwindow.h"
#include "TexGen.h"
#include "DDgui.h"
#include "TxGenGUI.h"
#include "ModellGui.h"
#include "FileIO.h"
#include "keyframergui.h"
#include "introeditor.h"
#include "filemenu.h"
#include "fmod\minifmod.h"

extern GLuint cubetex,mainmenutex,texbutton1,texbutton2,texbutton3;//,texts1;
extern char musfilename[40];
extern char *music_data;
extern FMUSIC_MODULE *mod;
extern bool musicloaded;
extern int music_len,music_pos;

extern char lastfilename[256];

extern scene *scenelist;
extern scene *lastscene;

extern char tooltip[256];
extern char defaulttooltip[256];
extern material *materiallist,*lastmaterial;

extern GLuint texlayers[4],prelayers[4];
extern GLuint pretextures[6][8];
extern GLuint background;
extern int mainmenu;
extern int texgenmenu;
extern int texsubmenu[6];
extern texturecommand texdata[6][8];
extern int x,y,z;
extern bool waitleftbutton,waitrightbutton;
extern texture generatedtexture,buffertexture;
extern char *s;
extern bool randomed;
extern tfilelist *atglist;
extern tfilelist *scnlist,*prjlist,*xmlist, *synclist;
extern tTexture *actualtexture,*texturelist,*lasttexture;
extern int atgbarpos;
extern int atgselected;
extern int selbarpos;
extern int selselected;
extern int scnbarpos;
extern int scnselected;
extern tfilelist *pf;
extern char *st;
extern FILE *f;
extern byte b;
extern byte k;
extern texturecommand cmd;
extern int commandselected;
extern int commandpos;
extern char *commandnames[256];
extern scene *actualscene;
extern int modellermenu;
extern bool xformmenu;
extern bool createmenu;
extern int transformation;
extern bool centertransform;
extern int selectstart;
extern int lofttrack,loftmesh;
extern int loftstart,loftcounter,tracknumber,trackstart,trackcounter;
extern object *o;
extern bool transform;
extern int modellsubmenu[8];
extern int selectedtexture, texturestart;


extern byte spherex,spherey;
extern byte gridx,gridy;
extern byte tubex,tubey;
extern bool tubetop;
extern byte conex;
extern bool conetop;
extern byte arcx;
extern int arcy;
extern byte linex;
extern unsigned int mattexture;
extern int matstart,matselected;

extern int objnumber;
extern char *cursorstring;
extern bool backdraw;

extern bool scntexturesave, scncamerasave, scnselectionsave, scnlightsave, scnobjectsave, scnenvironmentsave;
extern bool aCid;


unsigned int memopen(char *name);
void memclose(unsigned int handle);
int memread(void *buffer, int size, unsigned int handle);
void memseek(unsigned int handle, int pos, signed char mode);
int memtell(unsigned int handle);


#endif