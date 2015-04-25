#ifndef __introengine__
#define __introengine__

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "vectors.h"
#include "track.h"
#include "texgen.h"
#include "introplayer.h"

#define aDDict_default             0
#define aDDict_flatshade           1
#define aDDict_gouraudshade        2

#define layer2d        1
#define layer3d        2
#define cleargl        3
#define rendertotext   4
#define feedback       5
#define grideffect     6

struct scrshot
{
	GLuint texture;
	float x1,y1,x2,y2;
};

struct camera {

    int number;
    vector3 eye,target,up;
    float fov;
	float roll;

	CTrack *eyex,*eyey,*eyez,*trgx,*trgy,*trgz,*fovt,*rollt;

    camera *next;
};

struct tminimalobjdata
{
	unsigned primitive       :8,
             normalsinverted :1,
             notlit          :1,
             textured        :1,
             red             :1,
             green           :1,
             blue            :1,
             alpha           :1,
             alphamap1set    :1,
             alphachannel    :2,
             material2set    :1,
             texxset         :1,
             texyset         :1,
             texoffxset      :1,
             texoffyset      :1,
             shading         :2,
             mapping         :3,
             swaptexturexy   :1,
             inverttexx      :1,
			 inverttexy      :1;
}; //31 bit jol :)

struct selection {
	short int selected;
	selection *next;
};

struct vertex {
	vector3 position,d,base;
	vector3 normal,defaultnormal;
	vector2 dt,t;
};

struct polygon {
	int v1,v2,v3;
	vector3 normal,defaultnormal;
	vector4 color;
	vector2 t1,t2,t3;
	vector2 d1,d2,d3;
	int shading;
	int insideotherobject;
	bool normalinverted;
	GLuint texturehandle,envmaphandle;
};

struct orientation {
	vector3 position;
	vector3 rotaxis;
	float rotangle;
	vector3 stretch;
	float color[4];
};

struct objanim {
	int number;

	CTrack *posx,*posy,*posz,*rotx,*roty,*rotz,*rota,*strx,*stry,*strz,*colr,*colg,*colb,*cola;

	objanim *next;
};

struct object {

	tminimalobjdata data;
	int number;
	int parent;

	unsigned char texture,envmap;
	float color[4];
	unsigned char texxscale,texyscale,texxoffset,texyoffset;
	int params[4];

	matrix xformmatrix,currentmatrix,buffermatrix;
	float glmatrix[16];

	vertex *vertices;
	int vertexcapacity,vertexnum;

	polygon *polygons;
	orientation orient;
	objanim *anims;
	int polygoncapacity,polygonnum;

	GLuint solidlist,translist,envlist;
};

struct light{
	GLint identifier;
	bool turnedon;
	GLfloat ambient[4];
	GLfloat color[4];
	GLfloat position[4];
	GLfloat spot_direction[4];
	GLfloat spot_exponent;
	GLfloat spot_cutoff;
	GLfloat c_att,l_att,q_att;
};

struct scene {
	object *objects;

	int number;

	light lights[8];
	
	unsigned char fog;
	float fogcol[3];
	float fogdensity;

	int objectcapacity,objectnum;

	camera *cameras;

	scene *next;
};

struct cpolygon
{
	polygon data;
	vector3 a,b,c;
	cpolygon *next;
};

struct bspnode
{
	vector3 planevertex,planenormal;

	cpolygon *polylist;

	bspnode *front,*back;
};

struct event
{
	int eventtype,startframe,endframe,pass;

	int startrectx1,startrectx2,startrecty1,startrecty2;
	int endrectx1,endrectx2,endrecty1,endrecty2;

	float startcol[4],endcol[4];

	GLuint blendfunc1,blendfunc2;

	int textured,texture;
	int sceneid,camid,animid;
	int camstart,camend,animstart,animend;
	int clearscreen,clearzbuffer;

	int effect;
	int param1;
	float param4;
	float gridstart,gridend;

	// calculated variables:

	GLuint mattexture;

	scene *iscene;
	camera *icam;
	int ianim;
	int ianimlength;

	event *next;
};

extern texture *texturelist;
extern material *materiallist;
extern scene *scenelist;
extern rgba maptexture[256][256];
extern event *eventlist, *lastevent;
extern scrshot shots[20];
extern bool calculatehierarchy[50];

extern texture *ptexturelist;
extern material *pmateriallist;
extern scene *pscenelist;
extern event *peventlist, *plastevent;


void addvertex(object *o, float x, float y, float z);
void addpolygon(object *o, int v1, int v2, int v3, int shading);
void addobject(scene &s);

object newobject();
scene *newscene();

object *searchobjectbynumber(object *list, int listsize, int id);

int selsize(selection *scn);
material *findmaterial(int num);
texture *findtexture(int num);
scene *findscene(int num);
camera *findcam(scene *s,int num);

void obj_transform(object *obj, matrix m);
void renderscene(scene *Scene);
void setlighting(scene *scn);
void setcameraview(camera c, float aspectratio);
void initintroengine();
void generateprojectlists();
void killprojectlists();

#endif