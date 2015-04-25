#pragma once

#include "Vectors.h"
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "Texgen.h"
#include "includelist.h"
#include "Splines.h"
#include "ParticleEngine.h"

#define aDDict_DEFAULTSHADE 0
#define aDDict_FLATSHADE 1
#define aDDict_GOURAUDSHADE 2

#define aDDict_SCALE 0
#define aDDict_ROTATE 1
#define aDDict_MOVE 2

#define aDDict_DUMMY 0
#define aDDict_SCENE 1
#define aDDict_SUBSCENE 2
#define aDDict_SPHEREEMITTER 3
#define aDDict_CUBEEMITTER 4
#define aDDict_CYLINDEREMITTER 5
#define aDDict_PLANEDEFLECTOR 6
#define aDDict_SPHEREDEFLECTOR 7
#define aDDict_GRAVITY 8

#define radtheta 3.1415f/180.0f

struct SUPERSHAPEARC
{
	unsigned char mint,mfloat;
	float n1,n2,n3,a,b;
};

struct SUPERSHAPE
{
	SUPERSHAPEARC SuperShape1,SuperShape2;
	unsigned char Sphere; //sphere or toroid?
	float Trad0,TradPhi; //toroid data
	unsigned char RadFunc; //radial function
	float Rada,Radb; //radial function variables
	unsigned char VertFunc; //vertical function
	float Verta; //vertical function variable
	int Rangex1,Rangex2,Rangey1,Rangey2; //range variables
	unsigned char Xres,Yres; //resolution
};

struct CAMERA
{
	VECTOR3 Eye,Target,Up;
	float Roll,Fov;
	VECTOR3 i,j,Normal;
	float d;
};

struct EDGE
{
	int v1,v2; //vertices on the ends of edge
	int p1,p2; //polygons on the side of edge
	bool selected; //needed variable for calculations
	bool InWireframe;
};

struct VERTEX
{
	VECTOR3 Position,MapTransformedPosition;
	VECTOR3 Normal,CurrentNormal;
	VECTOR2 TextureCoordinate,CurrentTextureCoordinate;

	int *EdgeList; //connected edges
	int EdgeNum,EdgeCapacity;
	float Weight[4];
	bool Selected;
	int SelectCount;
	int PolyCount;
};

struct POLYGON
{
	int v[3]; //vertices
	int e[3]; //edges
	VECTOR2 t[3],ct[3];

	GLuint Material;
	VECTOR3 Normal,CurrentNormal;
	unsigned char Shading,CurrentShading;

	bool Selected,Highlighted;
};

class OBJECT
{
public:
	int ID;
	unsigned char Primitive;
	int Param1,Param2,Param3,Param4,Param5;

	VERTEX *VertexList;
	int VertexNum,VertexCapacity;

	POLYGON *PolygonList;
	int PolygonNum,PolygonCapacity;

	EDGE *EdgeList;
	int EdgeNum,EdgeCapacity;

	GLuint DisplayList;
	GLuint WireDisplayList;
	GLuint SRCBlend,DSTBlend;
	bool Wireframe;
	int Shading;

	MATRIX ModelView,TransformBuffer,Inverted,IT;

	int NewVertexNum;
	VECTOR3 *NewVertices;
	VECTOR3 *NewNormals;
	VECTOR2 *NewTexCoords;
	int *NewIndices;

	void OptimizeForVBO();
	void AddVertex(float x, float y, float z, float u, float v);
	void AddPolygon(int x, int y, int z, bool e1, bool e2, bool e3);
	void AddPolygon(int x, int y, int z, char Shading, bool e1, bool e2, bool e3);
	void AddPolygon(int x, int y, int z, float t1x, float t1y,  float t2x, float t2y,  float t3x, float t3y, bool e1, bool e2, bool e3);
	void AddPolygon(int x, int y, int z, char Shading, float t1x, float t1y,  float t2x, float t2y,  float t3x, float t3y, bool e1, bool e2, bool e3);
	int AddEdge(int a, int b, int p, bool InWireframe);
	void CalculateTextureCoordinates();
	void CalculateNormals();

#ifdef INCLUDE_MESHSMOOTH
	void MeshSmooth(bool Linear);
	void SortVertexEdges();
	void TessellateEdges(int *, bool);
	void BuildNewFaces(int *, bool);
#endif
#ifdef INCLUDE_OBJ_MESHBLUR
	void MeshBlur();
#endif

	bool Textured;
	MATERIAL *Material;
	bool EnvMapped;
	MATERIAL *EnvMap;

	bool NormalsInverted;
	unsigned char Xtile,Ytile;
	bool XSwap,YSwap,Swap;
	float OffsetX,OffsetY;
	RGBA Color;
	bool ZMask,Backface,Backfront;
	unsigned char AEpsilon;
	char MapXformType;
	char MapXformColor;
	int TargetObjectID;
};

struct LIGHT
{
	GLint Identifier;
	bool Lit;
	GLfloat Ambient[4];
	GLfloat Color[4];
	GLfloat Position[4];
	GLfloat Spot_Direction[4];
	GLfloat Spot_Exponent;
	GLfloat Spot_Cutoff;
	GLfloat Constant_Attenuation,Linear_Attenuation,Quadratic_Attenuation;
	bool CastShadow;
};

struct WORLDOBJECT;

struct OBJDUMMY
{
	OBJECT *OriginalObj; //material datanak stb
	WORLDOBJECT *OriginalWorldObj; //world-be hogy beazonositsa a hozza tartozo matrixot a rendernel
	GLuint List; //display list
	bool Envmap;
	MATRIX ModelView;
};

class SCENE
{
public:
	int ID;

	bool ColorDiscard;

	OBJECT *ObjectList;
	int ObjectNum,ObjectCapacity;
	GLuint ZmaskList,NoZmaskList;

	//void AddObject();
	OBJECT *FindObjectByID(int ID);
	int FindObjectIndex(int ID);
	int CountObjects();
	void SetupDummies();
	void CalculateDummy(OBJECT *Object, int &DummyNum, MATRIX *Position);

	int ObjDummyCount;
	OBJDUMMY *ObjDummies;
};

class DEFAULTOBJECTSPLINES
{
public:
	SPLINE *Posx,*Posy,*Posz;
	SPLINE *Rotx,*Roty,*Rotz,*Rotw;
	SPLINE *Sclx,*Scly,*Sclz;
	SPLINE *AnimID; //Sub-World animacio ID
	SPLINE *AnimTime; //Sub-Worldokhoz
	SPLINE *Red,*Green,*Blue,*Alpha,*Size,*Prtfrme;
};

struct LIGHTSPLINES
{
	SPLINE *Lit;
	SPLINE *Red,*Green,*Blue;
	SPLINE *ARed,*AGreen,*ABlue;
};

struct CAMSPLINES
{
	int CamID;
	SPLINE *Eyex,*Eyey,*Eyez;
	SPLINE *Trgx,*Trgy,*Trgz;
	SPLINE *Fov,*Roll;
};

struct ANIMDATA
{
	int AnimID;
	LIGHTSPLINES *Lights;
};

struct ANIMPOS
{
	int AnimID;
	float AnimPos;
};

struct EMITTERDATA;

struct WORLDOBJECT
{
	int ID;
	int ParentID;
	void *Data;
	int Primitive;
	MATRIX ModelView,TransformBuffer,Inverted,IT;
	//RST Position;
	EMITTERDATA *EmitterData;
	GLuint SRCBlend,DSTBlend;
	bool Textured,ZMask;
	MATERIAL *Material;
	DEFAULTOBJECTSPLINES *Animations;
	int AnimNum;
	int AnimCapacity;
	RST PosData;
	ANIMPOS APosData;
	RGBA Color;

	WORLDOBJECT *Parent;
	unsigned char AEpsilon;

	int TargetID;
};

class WORLD
{
public:
	int ID;
	WORLDOBJECT *ObjectList;
	int ObjectNum,ObjectCapacity;
	int ObjectCount,ParticleCount,SubWorldCount;

	bool Lighting;
	LIGHT Lights[8];
	CAMERA Camera;

	bool Fog;
	unsigned char FogCol[4];
	float FogStart,FogEnd;

	int AnimNum;
	int AnimCapacity;
	ANIMDATA *Animations;

	CAMSPLINES *CamAnims;
	int CamNum;
	int CamCapacity;

	//void AddObject();
	void SetupLighting();
	//void RenderWireFrame(VECTOR3 v);
	void Render(bool SubWorld, bool ZMask);
#ifdef INCLUDE_PARTICLE_ENGINE
	void RenderParticles(CAMERA *Cam, int ActualAnim, MATRIX *WorldData);
	void RenderParticleTree(CAMERA *Cam, int ActualAnim, MATRIX *WorldData);
	void CalculateParticles(int CurrentFrame);
	//void AddAnimation();
	//void AddCamera();
	void CalculateParticleObjectMatrices();
#endif
	void CalculateObjectMatrices();
	void SetToAnimPos(ANIMPOS *a);
	WORLDOBJECT *FindObjectByID(int ID);

	/*int ObjDummyCount;
	OBJDUMMY *ObjDummies;

	int CountObjects();
	void SetupDummies();*/
};

struct PVSelect
{
	int ID;
	int Brush;
	int Operator;
	int Param1;
	int Param2;
	int Param3;
};

struct PSelect
{
	int BrushNum;
	PVSelect *Brushes;
};

void Obj_DoPolySelection(OBJECT *o,PVSelect *SelectData, bool HighLight);
void SelectObjPolys(OBJECT *o, PVSelect *i, int num);
//void SelectObjVertices(OBJECT *o,PVSelect *i, int num, bool Border);
void SelectObjVertices(OBJECT *o,PVSelect *i,int BrushNum, int num, bool Border);

#ifdef INCLUDE_OBJ_MAPXFORM
void CalculateObjectVertexWeights(OBJECT *o, MATERIAL *m, int XTile, int YTile, bool XSwap, bool YSwap, bool Swap, float XOffset, float YOffset);
#endif

void Spline_Slerp(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z, float *a, float *b, float *c, float *d, float Time);
void Spline_QuaternionGetVectors(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z);

void MultModelViewMatrix(MATRIX m);
void SetCameraView(CAMERA *Cam, float AspectRatio);
void SetModelViewMatrix(MATRIX m);

#ifdef INCLUDE_PARTICLE_ENGINE
extern int MaxParticleNum;
void InitParticleIndexBuffer();
#endif

#ifdef INCLUDE_OBJ_BUTTERFLYSMOOTH
void initVertexBlend();
#endif