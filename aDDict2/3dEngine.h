#ifndef __3DENGINE__
#define __3DENGINE__

#include "memtrack.h"

#include "Vectors.h"
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "GUIItemList.h"
#include "Texgen2.h"

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
#define aDDict_SPHERESLOWER 9

extern int LastTime; // cave last time

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

class SCENE;

struct CAMERA
{
	VECTOR3 Eye,Target,Up;
	float Roll,Fov;
	VECTOR3 i,j;
	VECTOR3 Normal;
	float d;
};

struct EDGE
{
	int v1,v2; //vertices on the ends of edge
	int p1,p2; //polygons on the side of edge
	bool selected; //needed variable for calculations
	bool InWireframe; //Displayed in wireframe view
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

	float Color[3],NewColor[3];
};

struct POLYGON
{
	int v1,v2,v3; //vertices
	int e[3]; //edges
	VECTOR2 t[3],ct[3];

	GLuint Material;
	VECTOR3 Normal,CurrentNormal;
	unsigned char Shading,CurrentShading;

	bool Selected,Highlighted;
};

struct PARTICLE
{
	VECTOR3 Position,LastPos,DisplayPos,DisplayPos2;
	VECTOR3 Speed;
	int TailCnt,TailCnt2;
	int Age,StartAge;
	bool Aging;
	bool Active;
	float Color1[4],Color2[4];
	VECTOR3 *Tail;
	float Rotation,RotChaos;
	float Size;
};

struct EMITTERDATA
{
	int DefaultAge,AgeChaos;
	float ParticlesPerFrame;
	float ParticleNumBuffer;
	float d,Speed,SpeedChaos,DirChaos;
	VECTOR3 n,Dir;

	PARTICLE *Particles;
	int MaxParticles;

	int LastFrameChecked;	

	bool Head,Tail,ObjectHead,TailInfo;
	unsigned char TailLength,TailLength2;
	SCENE *ObjectHeadScene;
	int ObjectHeadSceneID;
	GLuint HeadMaterial;
	float Color1[5], Color2[5];
	float Param1, Param2;
	bool Repeller;
	float Size;

	float CamFade,Rotspeed,RotChaos;

	bool RandRotate,FixedUp;
	// rotate speed
	// VECTOR3 StartDir;

};

struct RAYINTERSECTION
{
	bool hit;
	void *o;
	int Polygon;
	VECTOR3 Multipliers;
};

struct VISIBILITY
{
	int va,vb;
	int oa,ob;
};

class OBJECT
{
public:
	int ID;
	unsigned char Primitive;
	int Param1,Param2,Param3,Param4,Param5;
	bool CullBuff;

	VERTEX *VertexList;
	int VertexNum,VertexCapacity;

	POLYGON *PolygonList;
	int PolygonNum,PolygonCapacity;

	EDGE *EdgeList;
	int EdgeNum,EdgeCapacity;

	ITEM *ListItem,*ArcListItem;
	GLuint DisplayList;
	GLuint WireDisplayList;
	GLuint SRCBlend,DSTBlend;
	bool Wireframe;
	int Shading;

	bool Deleted;
	bool PerVertexColor;

	MATRIX ModelView,TransformBuffer,Inverted,IT;
	//EMITTERDATA *EmitterData;

	void AddVertex(float x, float y, float z, float u, float v);
	void AddPolygonEdgeTest(int x, int y, int z, bool AddEdges);
	void AddPolygon(int x, int y, int z, bool e1, bool e2, bool e3);
	void AddPolygon(int x, int y, int z, char Shading, bool e1, bool e2, bool e3);
	void AddPolygon(int x, int y, int z, float t1x, float t1y,  float t2x, float t2y,  float t3x, float t3y, bool e1, bool e2, bool e3);
	void AddPolygon(int x, int y, int z, char Shading, float t1x, float t1y,  float t2x, float t2y,  float t3x, float t3y, bool e1, bool e2, bool e3);
	int AddEdge(int a, int b, int p, bool InWireframe);
	void CalculateTextureCoordinates();
	void CalculateNormals();
	void CreateDisplayList();

	void Render(bool Zbuffer,RGBA Color,bool ColorDiscard);
	void RenderWireframe();

	void MeshSmooth(bool Linear);
	void MeshBlur();
	void SortVertexEdges();
	void TessellateEdges(int *, bool);
	void BuildNewFaces(int *, bool);

	bool Textured;
	MATERIAL *Material;
	bool EnvMapped;
	MATERIAL *EnvMap;

	bool NormalsInverted;
	unsigned char Xtile,Ytile;
	bool XSwap,YSwap,Swap;
	float OffsetX,OffsetY;
	RGBA Color;
	bool ZMask,Backface,Backfront,AA;
	unsigned char AEpsilon;
	char MapXformType;
	char MapXformColor;
	int TargetObjectID;
	bool ReCalc;

	int RayCount;
	RAYINTERSECTION *RayIntersections;

	OBJECT()
	{
		memset(this,0,sizeof(OBJECT));
		VertexList=new VERTEX[10]; VertexCapacity=10;
		PolygonList=new POLYGON[10]; PolygonCapacity=10;
		EdgeList=new EDGE[10]; EdgeCapacity=10;
		Xtile=Ytile=1;
	}

	~OBJECT()
	{
		delete[] VertexList;
		delete[] PolygonList;
		delete[] EdgeList;
		memset(this,0,sizeof(OBJECT));
		//delete EmitterData->Particles;
	}
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

#define aDDict_NONE 0
#define aDDict_LINEAR 1
#define aDDict_HERMITE 2
#define aDDict_BEZIER 3
#define aDDict_CATMULL 4
#define aDDict_SINE 5
#define aDDict_SAW 6
#define aDDict_SQUARE 7
#define aDDict_BSPLINE 8

struct Key
{
	float Value;
	float Time;
	float an,bn;
};

class SPLINE
{
public:
	int KeyNum;
	int KeyCapacity;
	Key *Keys;

	bool Loop;
	float LoopStart,LoopEnd;
	int Interpolation;

	SPLINE()
	{
		memset(this,0,sizeof(SPLINE));
		KeyCapacity=10;
		Keys=new Key[10];		
		memset(Keys,0,sizeof(Key)*10);
	}

	void AddKey(float Value, float Time);
	float GetInterpolatedValue(float Time);
	int Arrange(int);	
	void InitVectors();
	float GetVector(unsigned char sel, int n);
	void ChangeInterpolation(int i);
};

struct DEFAULTOBJECTSPLINES
{
//public:
	SPLINE *Posx,*Posy,*Posz;
	SPLINE *Rotx,*Roty,*Rotz,*Rotw;
	SPLINE *Sclx,*Scly,*Sclz;
	SPLINE *AnimID; //Sub-World animacio ID
	SPLINE *AnimTime; //Sub-Worldokhoz
	SPLINE *Red,*Green,*Blue,*Alpha,*Size,*Prtfrme;
	/*DEFAULTOBJECTSPLINES()
	{
		memset(this,0,sizeof(DEFAULTOBJECTSPLINES));
		Posx=new SPLINE(); Posx->Interpolation=aDDict_HERMITE;
		Posy=new SPLINE(); Posy->Interpolation=aDDict_HERMITE;
		Posz=new SPLINE(); Posz->Interpolation=aDDict_HERMITE;
		Rotx=new SPLINE();
		Roty=new SPLINE();
		Rotz=new SPLINE();
		Rotw=new SPLINE();
		Sclx=new SPLINE(); Sclx->Interpolation=aDDict_HERMITE;
		Scly=new SPLINE(); Scly->Interpolation=aDDict_HERMITE;
		Sclz=new SPLINE(); Sclz->Interpolation=aDDict_HERMITE;
		Anim=new SPLINE();
		Red=new SPLINE();  Red->Interpolation=aDDict_LINEAR;
		Green=new SPLINE(); Green->Interpolation=aDDict_LINEAR;
		Blue=new SPLINE(); Blue->Interpolation=aDDict_LINEAR;
		Alpha=new SPLINE(); Alpha->Interpolation=aDDict_LINEAR;
		Size=new SPLINE(); Size->Interpolation=aDDict_LINEAR;
		Prtfrme=new SPLINE(); Prtfrme->Interpolation=aDDict_LINEAR;
	}*/
};

struct LIGHTSPLINES
{
	SPLINE *Lit;
	SPLINE *Red,*Green,*Blue;
	SPLINE *ARed,*AGreen,*ABlue;
	SPLINE *Posx,*Posy,*Posz;
	SPLINE *Trgx,*Trgy,*Trgz;
};

struct CAMSPLINES
{
	int CamID;
	char *Name;
	SPLINE *Eyex,*Eyey,*Eyez;
	SPLINE *Trgx,*Trgy,*Trgz;
	SPLINE *Fov,*Roll;
	bool Deleted;
	bool Used;
};

struct ANIMDATA
{
	int AnimID;
	char *Name;
	LIGHTSPLINES *Lights;
	bool Deleted;
	ITEM *ListItem;
	bool Used;
};

struct ANIMPOS
{
	int AnimID;
	float AnimPos;
};

struct WORLDOBJECT
{
	int ID;
	int ParentID;
	void *Data;
	int Primitive;
	ITEM *ListItem;
	MATRIX ModelView,TransformBuffer,Inverted,IT,Hierarchy;
	//RST Position;
	bool Deleted;
	EMITTERDATA *EmitterData;
	GLuint SRCBlend,DSTBlend;
	bool Textured,ZMask;
	MATERIAL *Material;
	DEFAULTOBJECTSPLINES *Animations;
	int AnimNum;
	int AnimCapacity;
	RST PosData,PosDataBuffer,StoredPos;
	ANIMPOS APosData;
	RGBA Color;
	int TargetWorldID;
	WORLDOBJECT *Parent;
	unsigned char AEpsilon;
};

struct RENDERLISTARRAYOBJECT
{
	OBJECT *Object;
	WORLDOBJECT *WorldObject;
	bool Zwrite;
	bool EnvMapped;
	GLuint Blend1,Blend2;
	int TextureID,TextureSlot;
	MATRIX *m;

	//csak addictba
	bool Clone;
	OBJECT *CloneParent;
};

class RENDERLISTARRAY
{
public:
	int ItemNum;
	RENDERLISTARRAYOBJECT *List;
	int Pos;

	void SortByBlendMode();
	void SortByTexture();
	void SortByEnvMap();
	void SortByZwrite();
	void Sort()
	{
		SortByBlendMode();
		SortByTexture();
		SortByEnvMap();
		SortByZwrite();
	}

	void AddObject(OBJECT *o, WORLDOBJECT *w);

	RENDERLISTARRAY()
	{
		memset(this,0,sizeof(RENDERLISTARRAY));
		ItemNum=0;
	}

	~RENDERLISTARRAY()
	{
		delete List;
	}
};

class SCENE
{
public:
	int ID;

	char *Name;

	OBJECT *ObjectList;
	int ObjectNum,ObjectCapacity;

	ITEMLIST *ItemList,*ArcList;
	SCENE *Next;

	LIGHT Lights[8];

	CAMERA Camera;
	RGBA Fog;
	bool ColorDiscard;

	bool Used;

	RENDERLISTARRAY RLA;

	void CreateRLA();

	void AddObject();
	void SetupLighting();
	void RenderWireFrame(VECTOR3 v, bool);
	void Render(bool Lighting);
	void Render(bool Lighting, bool ZMask, RGBA Color);
	OBJECT *FindObjectByID(int ID);

	SCENE()
	{
		memset(this,0,sizeof(SCENE));
		ObjectList=new OBJECT[10]; ObjectNum=0; ObjectCapacity=10;
		memset(ObjectList,0,10*sizeof(OBJECT));
		memset(Lights,0,sizeof(LIGHT)*8);
		for (int x=0; x<8; x++) 
		{
			Lights[x].Identifier=GL_LIGHT0+x;
			Lights[x].Constant_Attenuation=1.0f;
			Lights[x].Spot_Cutoff=180.0f;
		}
		Lights[0].Lit=true;
		Lights[0].Position[0]=-1.0f;
		Lights[0].Position[1]=1.0f;
		Lights[0].Position[2]=1.0f;
		Lights[0].Color[0]=1.0f;
		Lights[0].Color[1]=1.0f;
		Lights[0].Color[2]=1.0f;
		Lights[1].Lit=true;
		Lights[1].Position[0]=1.0f;
		Lights[1].Position[1]=-1.0f;
		Lights[1].Position[2]=-1.0f;
		Lights[1].Color[0]=0.3f;
		Lights[1].Color[1]=0.3f;
		Lights[1].Color[2]=0.3f;
	}

	bool VisibilityInfoBuilt;
	int VertexPairs;
	VISIBILITY *VisibilityInfo;

	void DoRadiosityPass(bool FirstPass, int RayCount);
	void RadiosityCleanup();
	void BuildRadiosityVisibility();

	~SCENE()
	{
		delete ObjectList;
	}
};

class WORLD
{
public:
	int ID;
	WORLDOBJECT *ObjectList;
	int ObjectNum,ObjectCapacity;
	int ObjectCount,ParticleCount,SubWorldCount;
	ITEMLIST *ItemList,*AnimList,*CameraList;
	WORLD *Next;

	LIGHT Lights[8];
	CAMERA Camera;

	int AnimNum;
	int AnimCapacity;
	ANIMDATA *Animations;

	CAMSPLINES *CamAnims;
	int CamNum;
	int CamCapacity;
	bool Fog;
	bool Lighting;
	unsigned char FogCol[3];
	float FogStart,FogEnd;

	char *Name;

	RENDERLISTARRAY *Rla;

	bool Used;

	void CreateRLA();

	void AddObject();
	void SetupLighting();
	void RenderWireFrame(VECTOR3 v, bool SubWorld);
	void Render(bool SubWorld, bool Zmask);
	void RenderParticles(CAMERA *Cam, int ActualAnim, MATRIX *WorldPos);
	void RenderParticleTree(CAMERA *Cam, int ActualAnim, MATRIX *WorldData);
	void CalculateParticles();
	void AddAnimation(char *Name);
	void AddCamera(char *Name);
	void CalculateObjectMatrices();
	void SetToAnimPos(ANIMPOS *a);
	WORLDOBJECT *FindObjectByID(int ID);

	WORLD()
	{
		memset(this,0,sizeof(WORLD));
		ObjectList=new WORLDOBJECT[10];
		memset(ObjectList,0,10*sizeof(WORLDOBJECT));
		ObjectCapacity=10;

		for (int x=0; x<8; x++) 
		{
			Lights[x].Identifier=GL_LIGHT0+x;
			Lights[x].Constant_Attenuation=1.0f;
			Lights[x].Spot_Cutoff=180.0f;
		}

		Lights[0].Lit=true;
		Lights[0].Position[0]=-1.0f;
		Lights[0].Position[1]=1.0f;
		Lights[0].Position[2]=1.0f;
		Lights[0].Color[0]=1.0f;
		Lights[0].Color[1]=1.0f;
		Lights[0].Color[2]=1.0f;
		Lights[1].Lit=true;
		Lights[1].Position[0]=1.0f;
		Lights[1].Position[1]=-1.0f;
		Lights[1].Position[2]=-1.0f;
		Lights[1].Color[0]=0.3f;
		Lights[1].Color[1]=0.3f;
		Lights[1].Color[2]=0.3f;
		AnimList=new ITEMLIST();
		CameraList=new ITEMLIST();
		AnimCapacity=10;
		Animations=new ANIMDATA[10];
		memset(Animations,0,sizeof(ANIMDATA)*10);
		CamCapacity=10;
		CamAnims=new CAMSPLINES[10];
		memset(CamAnims,0,sizeof(CAMSPLINES)*10);
		Lighting=true;

	}
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

extern int PolyCounter,VertexCounter,ObjMemCounter,CallCounter;

void initVertexBlend();

void Obj_DoPolySelection(OBJECT *o,PVSelect *SelectData, bool HighLight);
void SelectObjPolys(OBJECT *o, ITEMLIST *i, int num);
void SelectObjVertices(OBJECT *o,ITEMLIST *i, int num, bool Border);
void CalculateObjectVertexWeights(OBJECT *o, MATERIAL *m, int XTile, int YTile, bool XSwap, bool YSwap, bool Swap, float XOffset, float YOffset);

void Spline_Slerp(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z, float *a, float *b, float *c, float *d, float Time);
void Spline_QuaternionGetVectors(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z);

void MultModelViewMatrix(MATRIX m, bool kill);
void SetCameraView(CAMERA *Cam, float AspectRatio, int pick,int x1, int y1);
void SetModelViewMatrix(MATRIX m);

void Sphere();
void Plane();
void Cylinder();
void Cube();
void InitRadiosityStuff();


#endif