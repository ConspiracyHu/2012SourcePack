#ifndef __IntroEngine__
#define __IntroEngine__

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "texgen.h"
#include "vectors.h"
#include "introwindow.h"

#define radtheta                 0.017453292f

#define dd3d_box                 1
#define dd3d_sphere              3
#define dd3d_dodecaeder          4
#define dd3d_hasab               6
#define dd3d_cone                7
#define dd3d_icosaeder           8
#define dd3d_arc                 9
#define dd3d_loft                10
#define dd3d_line                11
#define dd3d_grid                12
#define dd3d_freetrack           13
#define dd3d_freeobj             14
#define dd3d_clone               15

#define dd3d_butterfly           100
#define dd3d_linearsubdivision   101
#define dd3d_boolean             102
#define dd3d_blur                103
#define dd3d_map                 104

#define dd3d_default             0
#define dd3d_flatshade           1
#define dd3d_gouraudshade        2

#define dd3d_defaultmap          0
#define dd3d_spheremap           1
#define dd3d_planemap            2
#define dd3d_tubemap             3

struct edge {
	int v1,v2;
};

struct tTexture {
	rgba			layers[4][256][256];
	texturecommand	commands[100];
	byte			commandnum;
	string			texts[100];
	byte            number;
	tTexture		*next;
};

struct material {
	byte texture;
	byte layer;
	byte alpha;
	byte alphalayer;
	byte alphamode;
	GLuint handle;
	byte number;
	material *next;
};

struct SELECTION {
	short int selected;
	SELECTION *next;
};

struct selectionlist {
	SELECTION *selection;
	selectionlist *next;
};

struct camera {
    int number;
    vector3 eye,target,up;
    float fov;

    camera *next;
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

struct vertex {

	vector3 d;
	vector3 generated;
	vector3 v;
	vector3 transformed;
	vector3 defaultnormal;
	vector3 generatednormal;
	vector3 normal;
	vector3 transformednormal;

	vector2 t;

	int vedgenum;
	int vedgelist[20];
};

struct polygon {

	int v1,v2,v3;
	vector2 t1,t2,t3;
	vector4 c1,c2,c3;
	vector3 defaultnormal;
	vector3 generatednormal;
	vector3 normal;
	vector3 transformednormal;
	int shading;
	GLuint materialhandle;

	int pedgelist[3];
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

struct tobjdata {
	bool normalsinverted;
	byte texx,texy;
	byte texox,texoy;
	byte color[4];
	byte shading;
	bool notlit;
	byte material1;
	byte alphamap1,chnnl1;
	byte material2;
	bool alphamapped,envmapped;
	bool textured;
	byte mapping;
    bool swaptexturexy;
    bool inverttexx;
	bool inverttexy;
	bool buffer;
	unsigned short int number;
};


struct object
{
	tminimalobjdata objheader;
	byte primitive;
	tobjdata objdata;
	int params[6];
	matrix xformmatrix;
	bool track;

	vector3 center;

	int vertexnum;
	int vlistsize;
	vertex *vertexlist;

	int polynum;
	int plistsize;
	polygon *polygonlist;

	int edgenum;
	int elistsize;
	edge *edgelist;

	object *next;
};

struct scene
{
	int number;

	camera *cameras;
	selectionlist *selections;
	selectionlist *lastselection;

	object *objects;
	object *lastobject;

	light lights[8];

	unsigned char floatbytes;

	scene *next;
};

extern vector3 origo;
void init3dengine();
void setcameraview(camera c, float aspectratio);

void addvertex(object *obj, double x, double y, double z, double nx, double ny, double nz);
vertex  *searchvertex(vertex *vlist, int num);
void     addpolygon(object *obj, int v1, int v2, int v3, float nx, float ny, float nz, int shading);
polygon *searchpolygon(polygon *vlist, int num);
object  *searchobjectbynumber(object *vlist, int num);
int countvertices(vertex *pf);
int countpolygons(polygon *pf);
int objnum(object *pf);

object  *newobject();
scene  *newscene();
int selnum(selectionlist *scn);
int selsize(SELECTION *scn);

void scene_addobject(scene *scn);
void scene_render(scene &scn);

void butterflysubdivision(object *obj);

void obj_transform(object *obj, matrix m);
void obj_createprimitive(object *obj, int primitive, int param1, int param2, int param3, int param4, int param5);
void scene_renderwireframe(scene &scn);

#endif