#include "FileIO.h"
#include "IntroEditorEffects.h"
#include <stdio.h>
#include "vectors.h"

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

/*void loadandscene(SCENE &scn, const char *filename)
{
	FILE *f;
	f=fopen(filename,"rb");
	if (f)
	{
		fseek(f,4,SEEK_SET);
		short int objnum;
		fread(&objnum,2,1,f);
		andobj *objs=new andobj[objnum];
		fseek(f,10,SEEK_SET);
		short int objpacknum;
		fread(&objpacknum,2,1,f);
		short int indexes[16];
		fread(indexes,2,16,f);
		andobjdata *objpackets=new andobjdata[objpacknum];
		fread(objpackets,4,objpacknum,f);
		int on=0;
		for (int x=0; x<objpacknum; x++)
		{
			for (unsigned int z=0; z<=objpackets[x].ismetles; z++)
			{
				objs[on].group=objpackets[x];
				on++;
			}
		}
		unsigned char tort;

		unsigned char aaa[27];

		fseek(f,indexes[0],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (unsigned int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].x=tort/256.0f;	on++;}}
		fseek(f,indexes[1],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (unsigned int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].y=tort/256.0f;	on++;}}
		fseek(f,indexes[2],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (unsigned int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].z=tort/256.0f;	on++;}}
		fseek(f,indexes[7],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (unsigned int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].x+=(signed char)tort;on++;}}
		fseek(f,indexes[8],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (unsigned int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].y+=(signed char)tort;on++;}}
		fseek(f,indexes[9],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (unsigned int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].z+=(signed char)tort;on++;}}

		fseek(f,indexes[3],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (unsigned int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].xs=tort/256.0f; on++;}}
		fseek(f,indexes[4],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (unsigned int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].ys=tort/256.0f; on++;}}
		fseek(f,indexes[5],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (unsigned int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].zs=tort/256.0f; on++;}}
		fseek(f,indexes[10],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (unsigned int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].xs+=(signed char)tort; on++;}}
		fseek(f,indexes[11],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (unsigned int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].ys+=(signed char)tort; on++;}}
		fseek(f,indexes[12],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (unsigned int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].zs+=(signed char)tort; on++;}}
		
		fseek(f,indexes[14],SEEK_SET);
		for (x=0; x<objnum; x++) fread(&objs[x].rot,2,1,f); 

		for (x=0; x<objnum; x++)
		{
			scn.AddObject();
			GeneratePrimitive(&scn.ObjectList[scn.ObjectNum-1],aDDict_CYLINDER,objs[x].group.felbontas+3,2,1,0,0);
			scn.ObjectList[scn.ObjectNum-1].ListItem->Name="CYLINDER";

			MATRIX m1,m2,m3,m;
			M_XPose(0,0.5,0,m2);
			M_Rotate(0,1,0,45.0*radtheta,m);
			M_Scale((float)sqrt(2.0),1,(float)sqrt(2.0),m1);
			M_Mult(m,m2,m);
			M_Mult(m1,m,m);
			
			M_Scale(objs[x].xs,objs[x].zs,objs[x].ys,m1);
			M_Mult(m1,m,m1);

			MATRIX r;

			M_Rotate(0,0,1,objs[x].rot.z/32.0f*360.0f*radtheta,m3);
			
			M_Rotate(0,-1,0,objs[x].rot.y/32.0f*360.0f*radtheta,r);

			M_Mult(r,m3,m3);

			M_Rotate(1,0,0,objs[x].rot.x/32.0f*360.0f*radtheta,r);
			M_Mult(r,m3,m3);

			M_XPose(-objs[x].z,objs[x].y,-objs[x].x,m2);
			M_Mult(m3,m1,m1);
			M_Mult(m2,m1,m1);
			M_Scale(5,-5,5,m);
			M_Mult(m,m1,m1);

			memcpy(scn.ObjectList[scn.ObjectNum-1].ModelView,m1,sizeof(MATRIX));
			memcpy(scn.ObjectList[scn.ObjectNum-1].TransformBuffer,m1,sizeof(MATRIX));

			//obj_transform(scn.lastobject,m1);
		}

	}
}*/

void loadandscene(SCENE &scn, const char *filename)
{
	FILE *f;
	f=fopen(filename,"rb");
	fseek(f,0,SEEK_END);
	int fsize=ftell(f);
	unsigned char *data=new unsigned char[fsize];
	fseek(f,0,SEEK_SET);
	fread(data,fsize,1,f);
	fclose(f);

	unsigned char *d=data;
	d+=4;

	short objnum = (short)*d;
	d+=6;
	short objpacknum= (short)*d;
	d+=2;

	unsigned char* XPositionFine=data+(short)*d; d+=2;
	unsigned char* YPositionFine=data+(short)*d; d+=2;
	unsigned char* ZPositionFine=data+(short)*d; d+=2;
	unsigned char* XPackSizeFine=data+(short)*d; d+=2;
	unsigned char* YPackSizeFine=data+(short)*d; d+=2;
	unsigned char* ZPackSizeFine=data+(short)*d; d+=2;
	unsigned char* ChanferSizePackData=data+(short)*d; d+=2;
	unsigned char* XPositionRough=data+(short)*d; d+=2;
	unsigned char* YPositionRough=data+(short)*d; d+=2;
	unsigned char* ZPositionRough=data+(short)*d; d+=2;
	unsigned char* XPackSizeRough=data+(short)*d; d+=2;
	unsigned char* YPackSizeRough=data+(short)*d; d+=2;
	unsigned char* ZPackSizeRough=data+(short)*d; d+=2;
	unsigned char* ScalePackData=data+(short)*d; d+=2;
	unsigned char* ObjectRotationData=data+(short)*d; d+=4;
	unsigned char* ObjGroupData=d;




	delete data;

	WaitLeftButtonRelease();
}

struct AnimHead
{
	char Name[20];
	int ID;
	int _30;
	int FrameCounter;
	int FilePos;
};

struct AnimIndexData
{
	int Count;
	int Fpos;
};

struct loddata
{
	int objcount;
	int wtf[2];
	int filepos;
};

#define READ(t) *(t*)d; d+=sizeof(t)

void AddScene(char *CommandLine)
{
	SCENE *Buffer=new SCENE();
	Buffer->Name=new char[strlen(CommandLine)+1];
	memset(Buffer->Name,0,strlen(CommandLine)+1);
	memcpy(Buffer->Name,CommandLine,strlen(CommandLine));

	if (!Scenes)
	{
		LastScene=Scenes=Buffer;
		Buffer->ID=0;
	}
	else
	{
		SceneScroller->ParentItem->SelectedItem++;
		Buffer->ID=LastScene->ID+1;
		LastScene->Next=Buffer;
		LastScene=Buffer;
	}

	ActualScene=LastScene;
	LastScene->ItemList=new ITEMLIST();
	ITEM *SceneData=LastScene->ItemList->AddItem("SCENE DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(CommandLine)+51];
	memset(NameString,0,strlen(CommandLine)+51);
	_snprintf(NameString,strlen(CommandLine)+51,"%s",CommandLine);
	SceneData->AddSubItem(NameString,NULL,nop,nop,nop);
	NameString=new char[10];
	memset(NameString,0,10);
	_snprintf(NameString,10,"ID: %d",LastScene->ID);
	SceneData->AddSubItem(NameString,NULL,nop,nop,nop);

	LastScene->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
	ITEM *LightList=LastScene->ItemList->AddItem("LIGHTS",NULL,nop,nop,nop);
	LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
	LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
	LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
	LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
	LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
	LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
	LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
	LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
	
	LastScene->ItemList->AddItem("-------OBJECTS-------",NULL,Scroller_SelectItem,nop,nop);

	char *NewLine=new char[strlen(CommandLine)+51];
	memset(NewLine,0,strlen(CommandLine)+51);
	_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s",LastScene->ID,CommandLine);
	SceneListScroller->AddItem(NewLine,LastScene,Scroller_SelectOnlyThisScene,nop,nop);
	SceneScroller->ParentItem->ItemNum++;
	AddModellInterface->ParentItem->ItemNum++;
	ActualScene->Camera=ModellerDisplay4->ParentItem->Cam;
	ActualScene->ItemList=new ITEMLIST();
	ActualScene->ArcList=new ITEMLIST();

	SetEnvironmentToActualScene();
}

struct objheader
{
	int children;

	int counter1,counter2;
	char name[20];

	int stuff;

	int parent;

	int stuff2[3];
	int vertexcnt;
	int polycnt;

	int stuff3[2];

	float x,y,z;

	float d,e,f;
	float a,b;

	int vertexlistposition;
	int polygonlistposition;

	float stuff4[4];
};

struct vx
{
	VECTOR3 Pos;
	VECTOR3 Normal;
	int Col;
	VECTOR2 Tex;
};

struct pl
{
	int Mat;
	VECTOR3 Normal;
	int b;
	int v[3];
	int c;
};

void addworld(char *CommandLine)
{
	WORLD *Buffer=new WORLD();
	Buffer->Name=new char[strlen(CommandLine)+1];
	memset(Buffer->Name,0,strlen(CommandLine)+1);
	memcpy(Buffer->Name,CommandLine,strlen(CommandLine));

	if (!Worlds)
	{
		LastWorld=Worlds=Buffer;
		Buffer->ID=0;
	}
	else
	{
		WorldScroller->ParentItem->SelectedItem++;
		Buffer->ID=LastWorld->ID+1;
		LastWorld->Next=Buffer;
		LastWorld=Buffer;
	}

	ActualWorld=LastWorld;
	LastWorld->ItemList=new ITEMLIST();
	ITEM *WorldData=LastWorld->ItemList->AddItem("WORLD DATA",NULL,nop,SubOpenClose,nop);
	char *NameString=new char[strlen(CommandLine)+51];
	memset(NameString,0,strlen(CommandLine)+51);
	_snprintf(NameString,strlen(CommandLine)+51,"%s",CommandLine);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);
	NameString=new char[10];
	memset(NameString,0,10);
	_snprintf(NameString,10,"ID: %d",LastWorld->ID);
	WorldData->AddSubItem(NameString,NULL,nop,nop,nop);

	LastWorld->ItemList->AddItem("CAMERA",NULL,Scroller_SelectItem,nop,nop);
	ITEM *LightList=LastWorld->ItemList->AddItem("LIGHTS",NULL,Scroller_SelectItem,SubOpenClose,nop);
	LightList->AddSubItem("LIGHT 1",NULL,Light1,nop,nop);
	LightList->AddSubItem("LIGHT 2",NULL,Light2,nop,nop);
	LightList->AddSubItem("LIGHT 3",NULL,Light3,nop,nop);
	LightList->AddSubItem("LIGHT 4",NULL,Light4,nop,nop);
	LightList->AddSubItem("LIGHT 5",NULL,Light5,nop,nop);
	LightList->AddSubItem("LIGHT 6",NULL,Light6,nop,nop);
	LightList->AddSubItem("LIGHT 7",NULL,Light7,nop,nop);
	LightList->AddSubItem("LIGHT 8",NULL,Light8,nop,nop);
	
	LastWorld->ItemList->AddItem("--------SCENES-------",NULL,Scroller_SelectItem,nop,nop);

	char *NewLine=new char[strlen(CommandLine)+51];
	memset(NewLine,0,strlen(CommandLine)+51);
	_snprintf(NewLine,strlen(CommandLine)+51,"%d: %s",LastWorld->ID,CommandLine);
	WorldListScroller->AddItem(NewLine,LastWorld,Scroller_SelectOnlyThisWorld,nop,nop);
	WorldScroller->ParentItem->ItemNum++;
	memset(&LayouterDisplay4->ParentItem->Cam,0,sizeof(CAMERA));
	LayouterDisplay4->ParentItem->Cam.Fov=45;
	LayouterDisplay4->ParentItem->Cam.Up.y=1;
	LayouterDisplay4->ParentItem->Cam.Eye.z=10;
	LayouterDisplay4->ParentItem->Cam.Eye.y=7;
	ActualWorld->Camera=LayouterDisplay4->ParentItem->Cam;
	SetEnvironmentToActualWorld();
}

struct TSET
{
	float a,b,c;
};

typedef float QTSET[6];

void loadsttascene(SCENE &scn, const char *filename)
{
	TSET *tset=new TSET[6913];
	QTSET *qtset= new QTSET[44768];

	FILE *f;

	f=fopen("qtset.dat","rb+");
	if (f)
	{
		fseek(f,4,SEEK_SET);
		fread(qtset,6*4,44768,f);
		fclose(f);

		f=fopen("tset.dat","rb+");
		if (f)
		{
			fseek(f,4,SEEK_SET);
			fread(tset,12,6913,f);
			fclose(f);


			f=fopen(filename,"rb");
			fseek(f,0,SEEK_END);
			int fsize=ftell(f);
			unsigned char *data=new unsigned char[fsize];
			fseek(f,0,SEEK_SET);
			fread(data,fsize,1,f);
			fclose(f);

			unsigned char *d=data;
			d+=4;
			addworld((char*)d);
			d+=20;
			int AnimCnt=READ(int);
			int LodCnt=READ(int);
			int ObjCnt=READ(int);
			d+=32;

			AnimHead *Anims=(AnimHead*)d;
			AnimHead LastAnim=Anims[AnimCnt-1];

			AnimIndexData *LastIndex=(AnimIndexData*)(LastAnim.FilePos+(ObjCnt-1)*sizeof(AnimIndexData)+data);

			ErrorMessage=new char[100];

			MainLayouterScroller->ParentItem->SelectedItem=3+(int)(ActualWorld->ItemList->List->Next->Next->SubOpened)*8;
			d=((loddata*)(LastIndex->Fpos+LastIndex->Count*4+data))->filepos+data;
			int StartSceneID=0;
			int x;
			for (x=0; x<ObjCnt; x++)
			{
				objheader *o=(objheader*)d;
				d+=sizeof(objheader);
				if (o->name[1])
				{
					AddScene(o->name);
					if (x==0) StartSceneID=ActualScene->ID;					
					ActualWorld->AddObject();
					ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ID=x;
					ActualWorld->ObjectCount++;

					WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
					w->ListItem->Name=new char[20];			
					memcpy(w->ListItem->Name,o->name,20);
					w->Primitive=aDDict_SCENE;
					w->Data=ActualScene;
					w->PosData.Pos.x=o->x;
					w->PosData.Pos.y=o->y;
					w->PosData.Pos.z=o->z;
					if (x) w->ParentID=o->parent;

					MainLayouterScroller->ParentItem->ItemNum++;
					SetObjectSplines();

					ActualScene->AddObject();
					ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_STORED;
					ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="STORED";
					OBJECT *z=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

					unsigned char* p=d;
					d=o->vertexlistposition+data;

					int y;
					for (y=0; y<o->vertexcnt; y++)
					{
						vx v=READ(vx);
						z->AddVertex(v.Pos.x,v.Pos.y,v.Pos.z,v.Tex.u,v.Tex.v);
						z->VertexList[z->VertexNum-1].CurrentNormal=z->VertexList[z->VertexNum-1].Normal=v.Normal;			
					}

					d=o->polygonlistposition+data;

					for (y=0; y<o->polycnt; y++)
					{
						pl p=READ(pl);
						z->AddPolygon(p.v[0],p.v[1],p.v[2],aDDict_GOURAUDSHADE,1,1,1);
						z->PolygonList[z->PolygonNum-1].Normal=p.Normal;
					}
					z->Color.r=255;
					z->Color.g=255;
					z->Color.b=255;
					z->Color.a=255;
					z->Backface=false;
					z->Shading=aDDict_GOURAUDSHADE;

					z->CreateDisplayList();

					d=p;
				}
			}

			for (x=0; x<AnimCnt; x++)
			{
				Anims[x].Name[19]=0;
				ActualWorld->AddAnimation(Anims[x].Name);
				ActualWorld->AnimList->AddItem(Anims[x].Name,&ActualWorld->Animations[ActualWorld->AnimNum-1],GUI_SelectOnlyThisAnim,nop,nop);
				ActualWorld->Animations[ActualWorld->AnimNum-1].ListItem=ActualWorld->AnimList->Last;
				ActualWorld->AnimList->Last->Name=new char[20];
				memcpy(ActualWorld->AnimList->Last->Name,Anims[x].Name,20);
				SetActualWorldAnimListToMainAnimList();

				d=Anims[x].FilePos+data;
				for (int y=0; y<ObjCnt; y++)
				{
					AnimIndexData a1data=READ(AnimIndexData);
					unsigned char *b2=d;
					WORLDOBJECT *o=NULL;
					for (int k=0; k<ActualWorld->ObjectNum; k++) if (ActualWorld->ObjectList[k].ID==y) o=&ActualWorld->ObjectList[k];

					d=a1data.Fpos+data;

					if (o)
					for (int z=0;z<a1data.Count;z++)
					{
						unsigned short qtdata=READ(unsigned short);
						unsigned short tdata=READ(unsigned short);
						float frame=qtset[qtdata][4]/(float)Anims[x].FrameCounter;
						o->Animations[x].Posx->Interpolation=aDDict_LINEAR;
						o->Animations[x].Posx->AddKey(tset[tdata].a,frame);

						o->Animations[x].Posy->Interpolation=aDDict_LINEAR;
						o->Animations[x].Posy->AddKey(tset[tdata].b,frame);

						o->Animations[x].Posz->Interpolation=aDDict_LINEAR;
						o->Animations[x].Posz->AddKey(tset[tdata].c,frame);

						o->Animations[x].Rotx->AddKey(-qtset[qtdata][3],frame);
						o->Animations[x].Roty->AddKey(qtset[qtdata][0],frame);
						o->Animations[x].Rotz->AddKey(qtset[qtdata][1],frame);
						o->Animations[x].Rotw->AddKey(qtset[qtdata][2],frame);

						sprintf(ErrorMessage,"%d anims loaded\0",AnimCnt);
						//ActualWorld->ObjectList[y].Animations
						/*lastanim^.objs^[y-1].keynum:=a1data.ammount;
						lastanim^.objs^[y-1].keys^[z-1].i1:=b4.w[0];
						lastanim^.objs^[y-1].keys^[z-1].i2:=b4.w[1];*/
					}

					if (o)
					Spline_QuaternionGetVectors(o->Animations[x].Rotx,o->Animations[x].Roty,o->Animations[x].Rotz,o->Animations[x].Rotw);

					d=b2;
				}

			}

			SetObjectSplines();

			delete data;
			delete tset;
			delete[] qtset;
			WaitLeftButtonRelease();
		}
	}
}

struct Header
{
	int Size;                //0x00
	int Buffer;              //0x04
	int StartPositions[6]; 
};

int aaaa=0;

VECTOR3 ReadKaspaVertices(OBJECT *o, unsigned char *d, Header *data)
{
	unsigned char *esi=d;
	unsigned char *adata=((unsigned char*)&(((unsigned char*)data)[data->StartPositions[3]]));

	int framecount=*(int*)(adata+4);
	//data->StartPositions[4]-data->StartPositions[3]

	int datasize=(data->StartPositions[4]-data->StartPositions[3]-framecount*4-12)/(float)framecount;
	adata+=(framecount)*4+12+datasize*(aaaa);//0x908;//((*(int*)(adata+4)))*4+0x0c-8;


	int ecx=0;
	int vcount=0;

	int Intarray[3],var_14,var_4,var_20;
	float flt5,flt6,flt7,flt8;
	VECTOR3 v=V3_Make(0,0,0);

	VECTOR3 n=V3_Make(0,0,0);
	o->PerVertexColor=true;

	RGBA c;
	c.dw=0;
	while ((ecx&0x80000000)==0)
	{
		ecx=*(int*)esi;
		esi+=4;
		printf("DataType: %x| ",ecx&0x0f);

		int ebx=0;

		//--------- switch start

		int s=(ecx&0x0f);

		if (s&1)
		{
			Intarray[0]=*(short*)esi;
			Intarray[1]=*(short*)(esi+2);
			Intarray[2]=*(short*)(esi+4);
			esi+=8;
		}
		if (s&2)
		{
			var_14=*(short*)esi;
			esi+=4;
		}
		if (s&4)
		{
			c.dw=var_4=var_20=*(short*)esi;

			c.b=min(255,(int)((*(unsigned char*)(esi+0))*2));
			c.g=min(255,(int)((*(unsigned char*)(esi+1))*2));
			c.r=min(255,(int)((*(unsigned char*)(esi+2))*2));

			c.a=255;


			//n=V3_Normalize(V3_Make(*(char*)esi,*(char*)(esi+1),*(char*)(esi+2)));

			esi+=4;
		}
		if (s&8)
		{
			flt5=*(float*)esi;
			flt6=*(float*)(esi+4);
			esi+=8;
		}

		if (s%8) 
		{
			ebx+=1;
			if (s%8>1) ebx+=s%2+1;
		}

		//--------- switch end

		if (((unsigned short)ecx&0x10)!=0) 
		{
			flt7=*(float*)esi;
			flt8=*(float*)(esi+4);
			esi+=8;
		}

		if (((unsigned short)ecx&0x40)!=0) 
		{
			Intarray[0]=*(short*)adata;
			Intarray[1]=*(short*)(adata+2);
			Intarray[2]=*(short*)(adata+4);
			adata+=8;

			ebx|=1;
		}

		if (ebx!=1)
		{
		}
		if (ebx!=3)
		{
		}

		if (((unsigned short)s&0x01)!=0) esi+=0;
		if (((unsigned short)ecx&0x20)!=0) 
		{
			ecx=*(int*)esi;
			ecx<<=0x0e;
			esi+=4;
		}

		if (vcount)
		{
			o->AddVertex(Intarray[0]/100.0f-v.x,Intarray[1]/100.0f-v.y,Intarray[2]/100.0f-v.z,0,0);
			//o->VertexList[o->VertexNum-1].CurrentNormal=o->VertexList[o->VertexNum-1].Normal=n;
			o->VertexList[o->VertexNum-1].Color[0]=c.r/255.0f;
			o->VertexList[o->VertexNum-1].Color[1]=c.g/255.0f;
			o->VertexList[o->VertexNum-1].Color[2]=c.b/255.0f;
		}
		//else v=V3_Make(Intarray[0]/100.0f,Intarray[1]/100.0f,Intarray[2]/100.0f);


		vcount++;
		printf("\n");
	}

	printf("Vcount: %d\n",vcount);
	printf("FilePos: %d\n",esi-d+32);
	return v;
}

void loadwdo(SCENE &scn,char const *filename)
{
	FILE *f;
	f=fopen(filename,"rb");
	fseek(f,0,SEEK_END);
	int fsize=ftell(f);
	unsigned char *data=new unsigned char[fsize];
	fseek(f,0,SEEK_SET);
	fread(data,fsize,1,f);
	fclose(f);

	Header *h;

	//addworld((char*)filename);
	//AddScene((char*)filename);

	do
	{
		h=(Header*)data;

		//AddScene("OBJ");

		ActualScene->AddObject();
		ActualScene->ObjectList[ActualScene->ObjectNum-1].Primitive=aDDict_STORED;
		ActualScene->ObjectList[ActualScene->ObjectNum-1].ListItem->Name="STORED";

		OBJECT *z=&ActualScene->ObjectList[ActualScene->ObjectNum-1];

		/*ActualWorld->AddObject();
		//ActualWorld->ObjectList[ActualWorld->ObjectNum-1].ID=x;
		ActualWorld->ObjectCount++;

		WORLDOBJECT *w=&ActualWorld->ObjectList[ActualWorld->ObjectNum-1];
		w->ListItem->Name=new char[20];			
		memcpy(w->ListItem->Name,"OBJ\0                 ",20);
		w->Primitive=aDDict_SCENE;
		w->Data=ActualScene;
		/*w->PosData.Pos.x=o->x;
		w->PosData.Pos.y=o->y;
		w->PosData.Pos.z=o->z;*/

		

		VECTOR3 v=ReadKaspaVertices(z, data+32, (Header*)data);

		/*w->PosData.Pos.x=v.x;
		w->PosData.Pos.y=v.y;
		w->PosData.Pos.z=v.z;*/
		
		int pcount=*((int*)&data[h->StartPositions[4]]);

		//z->AddVertex(0,0,0,0,0);

		/*for (int x=h->StartPositions[4]+4;x<h->Size;x+=4) // WIREFRAME
		{
			z->AddPolygon(min(z->VertexNum-1,*((short*)&data[x])),min(z->VertexNum-1,*((short*)&data[x+2])),min(z->VertexNum-1,*((short*)&data[x+2])));
		}*/

		for (int x=h->StartPositions[0];x<h->StartPositions[1];x+=6) // POLYDATA
		{
			int a,b,c;
			a=*((short*)&data[x]);
			if (a&0x8000) x+=10;
			a=*((short*)&data[x]);

			if (a&0x4000)
			{
				x+=10;
				a=*((short*)&data[x]);
			}

			b=*((short*)&data[x+2]);
			c=*((short*)&data[x+4]);

			if (x<h->StartPositions[1])
			z->AddPolygon(a,b,c,aDDict_GOURAUDSHADE,1,1,1);

		}

		z->CalculateNormals();
		z->CreateDisplayList();

		data+=h->Size;
	} while ((h->Size&0xff000000)==0);

	aaaa++;
	WaitLeftButtonRelease();

}

void __stdcall GUI_LoadZoom3Scene(cINTERFACE* Interface, int ButtonId)
{
	ITEM *Selected=ReturnItem(ModellFileList,*(int*)Interface->ItemList[ButtonId].data);
	char* NewLine=new char[strlen(Selected->Name)+strlen(SceneDir)+10];
	memset(NewLine,0,strlen(Selected->Name)+strlen(SceneDir)+10);
	sprintf(NewLine,"%s\\%s\0",SceneDir,Selected->Name);
	//if (ActualScene)
	{
		//loadsttascene(*ActualScene,NewLine);
		loadwdo(*ActualScene,NewLine);
	}
}
