//#include "glew.h"
#include "3dEngine.h"
#include "GUIInterface.h"
#include "ObjectGenerators.h"
#include "float.h"
#include "Layouter.h"

int PolyCounter,VertexCounter,ObjMemCounter,CallCounter;

void SelectPolyNeighbours(OBJECT *o, bool *Buffer, int *Level, int Poly, int Radius, int LevelNum)
{
	if (Poly>=0 && Poly<o->PolygonNum)
	{
		Buffer[Poly]=true;
		Level[Poly]=LevelNum;
		if (Radius)
		{
			int 
			p=o->EdgeList[o->PolygonList[Poly].e[0]].p1; if (p==Poly) p=o->EdgeList[o->PolygonList[Poly].e[0]].p2;
			if (Level[p]>LevelNum+1)
			SelectPolyNeighbours(o,Buffer,Level,p,Radius-1,LevelNum+1);
			p=o->EdgeList[o->PolygonList[Poly].e[1]].p1; if (p==Poly) p=o->EdgeList[o->PolygonList[Poly].e[1]].p2;
			if (Level[p]>LevelNum+1)
			SelectPolyNeighbours(o,Buffer,Level,p,Radius-1,LevelNum+1);
			p=o->EdgeList[o->PolygonList[Poly].e[2]].p1; if (p==Poly) p=o->EdgeList[o->PolygonList[Poly].e[2]].p2;
			if (Level[p]>LevelNum+1)
			SelectPolyNeighbours(o,Buffer,Level,p,Radius-1,LevelNum+1);
		}
	}
}

void SelectVertexNeighbours(OBJECT *o, bool *Buffer, int *Level, int Vertex, int Radius, int LevelNum)
{
	if (Vertex>=0 && Vertex<o->VertexNum)
	{
		Level[Vertex]=LevelNum;
		if (Radius)
		for (int i=0; i<o->EdgeNum; i++)
		{
			if (o->EdgeList[i].v1==Vertex && Level[o->EdgeList[i].v2]>LevelNum+1)
				SelectVertexNeighbours(o,Buffer,Level,o->EdgeList[i].v2,Radius-1,LevelNum+1);
			else
			if (o->EdgeList[i].v2==Vertex && Level[o->EdgeList[i].v1]>LevelNum+1)
				SelectVertexNeighbours(o,Buffer,Level,o->EdgeList[i].v1,Radius-1,LevelNum+1);
		}
	}
}

void Obj_DoPolySelection(OBJECT *o,PVSelect *SelectData, bool HighLight)
{
	bool *Buffer=new bool[o->PolygonNum];
	int *Level=new int[o->PolygonNum];
	memset(Buffer,0,o->PolygonNum);
	for (int x=0; x<o->PolygonNum; x++) Level[x]=65536;


	int *Level2=new int[o->VertexNum];
	for (int x=0; x<o->VertexNum; x++) Level2[x]=6553600;

	switch (SelectData->Brush)
	{
	case 0: {   //vertexselect brush
				//Param1: Kivalasztott Poly ID
				//Param2: Sugar
				/*SelectVertexNeighbours(o,Buffer,Level2,SelectData->Param1,SelectData->Param2,0);
				for (int x=0; x<o->PolygonNum; x++)
				{
					Buffer[x]=(Level2[o->PolygonList[x].v1]<=SelectData->Param2) || 
						      (Level2[o->PolygonList[x].v2]<=SelectData->Param2) || 
							  (Level2[o->PolygonList[x].v3]<=SelectData->Param2);
				}*/
			}
		break;
	case 1: {
				//polyselect brush
				//Param1: Kivalasztott Poly ID
				//Param2: Sugar
				SelectPolyNeighbours(o,Buffer,Level,SelectData->Param1,SelectData->Param2,0);
			}
		break;
	case 2: {
				//random brush
				//Param1: RMax
				//Param2: RSeed
				//Param3: Count
				srand(SelectData->Param2);
				for (int x=0; x<SelectData->Param3; x++)
				{
					int j=(int)((rand()/(float)RAND_MAX)*SelectData->Param1);
					if (j>=0 && j<o->PolygonNum)
						Buffer[j]=true;
				}
			}
		break;
	case 3: //mapped brush
		break;
	}

	if (HighLight)
	for (int x=0; x<o->PolygonNum; x++) o->PolygonList[x].Highlighted=Buffer[x];
	else
	{
		for (int x=0; x<o->PolygonNum; x++)
		{
			switch (SelectData->Operator)
			{
			case 0: //add (or)
				o->PolygonList[x].Selected=o->PolygonList[x].Selected || Buffer[x];
				break;
			case 1: //sub
				if (Buffer[x]) o->PolygonList[x].Selected=false;
				break;
			case 2: //xor (not)
				o->PolygonList[x].Selected=o->PolygonList[x].Selected ^ Buffer[x];
				break;
			}
		}
	}
	delete Level;
	delete Buffer;
	delete Level2;
}

void SelectObjPolys(OBJECT *o, ITEMLIST *i, int num)
{
	for (int x=0; x<o->PolygonNum; x++) 
	{
		o->PolygonList[x].Highlighted=false;
		o->PolygonList[x].Selected=false;
	}
	if (num==-1)
		for (ITEM *t=i->List;t;t=t->Next)
		{
			Obj_DoPolySelection(o,(PVSelect*)t->Data,false);
		}
	else
	{
		ITEM *t=i->List;
		for (int a=0; a<=num && t; a++)
		{
			Obj_DoPolySelection(o,(PVSelect*)t->Data,false);
			t=t->Next;
		}
	}
}

void SelectObjVertices(OBJECT *o,ITEMLIST *i, int num, bool Border)
{
	SelectObjPolys(o,i,num);
	for (int x=0; x<o->VertexNum; x++) 
	{
		o->VertexList[x].Selected=false;
		o->VertexList[x].SelectCount=0;
		o->VertexList[x].PolyCount=0;
	}
	if (Border)
	{
		for (int x=0; x<o->PolygonNum; x++)
			if (o->PolygonList[x].Selected)
			{
				o->VertexList[o->PolygonList[x].v1].Selected=true;
				o->VertexList[o->PolygonList[x].v2].Selected=true;
				o->VertexList[o->PolygonList[x].v3].Selected=true;
			}
	}
	else
	{
		for (int x=0; x<o->PolygonNum; x++)
		{
			if (o->PolygonList[x].Selected)
			{
				o->VertexList[o->PolygonList[x].v1].SelectCount++;
				o->VertexList[o->PolygonList[x].v2].SelectCount++;
				o->VertexList[o->PolygonList[x].v3].SelectCount++;
			}
			o->VertexList[o->PolygonList[x].v1].PolyCount++;
			o->VertexList[o->PolygonList[x].v2].PolyCount++;
			o->VertexList[o->PolygonList[x].v3].PolyCount++;
		}
		for (int x=0; x<o->VertexNum; x++) 
			o->VertexList[x].Selected=o->VertexList[x].PolyCount==o->VertexList[x].SelectCount;
	}
}



RGBA GetTexPixel(MATERIAL *m, float u, float v)
{
	if (m && m->ImageData.Data && m->Size)
	{
		int x=((int)(u*m->Size))%m->Size;
		while (x<0) x+=m->Size;
		int y=((int)(v*m->Size))%m->Size;
		while (y<0) y+=m->Size;
		if (m->ImageData.Data && m->Size!=0xcdcdcdcd && m->Size>0)
		return m->ImageData.Data[y*m->Size+x];
	}
	ErrorMessage="BAD MATERIAL REFERENCE! (MAPXFORM)";
	RGBA a;
	a.dw=0;
	return a;
}

void CalculateObjectVertexWeights(OBJECT *o, MATERIAL *m, int XTile, int YTile, bool XSwap, bool YSwap, bool Swap, float XOffset, float YOffset)
{
	int OldXTile,OldYTile;
	bool OldXSwap,OldYSwap,OldSwap;
	float OldXOffset,OldYOffset;
	OldXTile=o->Xtile;
	OldYTile=o->Ytile;
	OldXSwap=o->XSwap;
	OldYSwap=o->YSwap;
	OldSwap=o->Swap;
	OldXOffset=o->OffsetX;
	OldYOffset=o->OffsetY;
	o->Xtile=XTile;
	o->Ytile=YTile;
	o->XSwap=XSwap;
	o->YSwap=YSwap;
	o->Swap=Swap;
	o->OffsetX=XOffset;
	o->OffsetY=YOffset;

	o->CalculateTextureCoordinates();

	for (int x=0; x<o->VertexNum; x++)
	{
		for (int z=0; z<4; z++)
		o->VertexList[x].Weight[z]=0;
		o->VertexList[x].EdgeNum=0;
	}

	RGBA Col;

	for (int x=0; x<o->PolygonNum; x++)
	{
		Col=GetTexPixel(m,o->PolygonList[x].ct[0].x,o->PolygonList[x].ct[0].y);
		int z;
		for (z=0; z<4; z++)
		o->VertexList[o->PolygonList[x].v1].Weight[z]+=Col.c[z]/255.0f;
		o->VertexList[o->PolygonList[x].v1].EdgeNum++;

		Col=GetTexPixel(m,o->PolygonList[x].ct[1].x,o->PolygonList[x].ct[1].y);
		for (z=0; z<4; z++)
		o->VertexList[o->PolygonList[x].v2].Weight[z]+=Col.c[z]/255.0f;
		o->VertexList[o->PolygonList[x].v2].EdgeNum++;
		
		Col=GetTexPixel(m,o->PolygonList[x].ct[2].x,o->PolygonList[x].ct[2].y);
		for (z=0; z<4; z++)
		o->VertexList[o->PolygonList[x].v3].Weight[z]+=Col.c[z]/255.0f;
		o->VertexList[o->PolygonList[x].v3].EdgeNum++;
	}

	for (int x=0; x<o->VertexNum; x++)
	{
		for (int z=0; z<4; z++)
		o->VertexList[x].Weight[z]/=(float)o->VertexList[x].EdgeNum;
	}

	o->Xtile=OldXTile;
	o->Ytile=OldYTile;
	o->XSwap=OldXSwap;
	o->YSwap=OldYSwap;
	o->Swap=OldSwap;
	o->OffsetX=OldXOffset;
	o->OffsetY=OldYOffset;
	o->CalculateTextureCoordinates();
}

void SetCameraView(CAMERA *Cam, float AspectRatio, int pick,int x1, int y1)
{
	int i[4];
	glGetIntegerv(GL_VIEWPORT,i);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (pick) gluPickMatrix(x1,768-y1,pick,pick,i);
	gluPerspective(Cam->Fov,AspectRatio,0.1,2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	VECTOR3 dir=V3_Normalize(V3_Sub(Cam->Target,Cam->Eye));
	MATRIX m;
	M_Rotate(dir.x,dir.y,dir.z,(float)Cam->Roll*360.0f/255.0f*(float)radtheta,m);
	VECTOR3 u;
	M_Xformd(m,Cam->Up,u);
	gluLookAt(Cam->Eye.x,Cam->Eye.y,Cam->Eye.z,
			  Cam->Target.x,Cam->Target.y,Cam->Target.z,
			  u.x,u.y,u.z);
	Cam->i=V3_Normalize(V3_Cross(V3_Sub(Cam->Target,Cam->Eye),u));
	Cam->j=V3_Normalize(V3_Cross(V3_Sub(Cam->Target,Cam->Eye),Cam->i));
}

void SetModelViewMatrix(MATRIX m)
{
	float matrix[16];
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			matrix[x+y*4]=m[x][y];
	glLoadMatrixf(matrix);
}

void MultModelViewMatrix(MATRIX m, bool kill)
{
	float matrix[16];
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
		{
			if (kill)
			matrix[x+y*4]=KillFloat(m[x][y],2);
			else
			matrix[x+y*4]=m[x][y];
		}
	glMultMatrixf(matrix);
}

OBJECT *SCENE::FindObjectByID(int ID)
{
	for (int x=0; x<ObjectNum; x++)
		if (ObjectList[x].ID==ID) return &ObjectList[x];
	return NULL;
}

WORLDOBJECT *WORLD::FindObjectByID(int ID)
{
	for (int x=0; x<ObjectNum; x++)
		if (ObjectList[x].ID==ID) return &ObjectList[x];
	return NULL;
}

void SCENE::AddObject()
{
	if (ObjectCapacity==ObjectNum)
	{
		OBJECT *Buffer=new OBJECT[(ObjectCapacity+10)*2];
		memset(Buffer,0,sizeof(OBJECT)*(ObjectCapacity+10)*2);
		memcpy(Buffer,ObjectList,sizeof(OBJECT)*ObjectCapacity);

		for (ITEM *List=ArcList->List; List; List=List->Next)
		{
			for (int x=0; x<ObjectNum; x++) 
			{
				if (Buffer[x].ID==((OBJECT*)(List->Data))->ID) { List->Data=(void*)&Buffer[x]; }
				if (LoftTrack)
				if (Buffer[x].ID==LoftTrack->ID) { LoftTrack=&Buffer[x]; }
				if (LoftShape)
				if (Buffer[x].ID==LoftShape->ID) { LoftShape=&Buffer[x]; }
			}
		}


		memset(ObjectList,0,sizeof(OBJECT)*ObjectCapacity);
		delete[] ObjectList;
		ObjectList=Buffer;
		ObjectCapacity=(ObjectCapacity+10)*2;
		for (int x=0; x<ObjectNum; x++)
		{
			ObjectList[x].ListItem->Data=&ObjectList[x];
			if (ObjectList[x].Primitive==aDDict_CLONE)
			{
				for (int z=0; z<ObjectList[x].Param1; z++)
				{
					((int*)ObjectList[x].Param3)[z]=(int)(FindObjectByID(((int*)ObjectList[x].Param2)[z]));
				}
			}
		}
	}
	M_Identity(ObjectList[ObjectNum].ModelView);
	M_Identity(ObjectList[ObjectNum].TransformBuffer);
	M_Identity(ObjectList[ObjectNum].Inverted);
	M_Identity(ObjectList[ObjectNum].IT);
	//ObjectList[ObjectNum].EmitterData=new EMITTERDATA;
	//memset(ObjectList[ObjectNum].EmitterData,0,sizeof(EMITTERDATA));
	/*ObjectList[ObjectNum].EmitterData->Head=true;
	ObjectList[ObjectNum].EmitterData->TailLength=5;
	ObjectList[ObjectNum].EmitterData->TailLength2=2;*/
	ObjectList[ObjectNum].ListItem=ItemList->AddItem("",&ObjectList[ObjectNum],Object_LeftClick,GUI_Rename,Object_RightClick);
	ObjectList[ObjectNum].DisplayList=glGenLists(1);
	ObjectList[ObjectNum].WireDisplayList=glGenLists(1);
	ObjectList[ObjectNum].SRCBlend=GL_ZERO;
	ObjectList[ObjectNum].DSTBlend=GL_ONE;
	ObjectList[ObjectNum].ZMask=true;
	ObjectList[ObjectNum].Backface=true;
	ObjectList[ObjectNum].Backfront=true;
	ObjectList[ObjectNum].AA=false;
	ObjectList[ObjectNum].Xtile=ObjectList[ObjectNum].Ytile=1;
	ObjectList[ObjectNum].Color.dw=0xffffffff;
	if (ObjectNum)
	ObjectList[ObjectNum].ID=ObjectList[ObjectNum-1].ID+1;
	else
	ObjectList[ObjectNum].ID=0;
	ObjectNum++;
}

void SCENE::SetupLighting()
{
	glEnable(GL_LIGHTING);
	for (int x=0; x<8; x++)
	if (Lights[x].Lit)
	{
		glEnable(Lights[x].Identifier);
		glLightfv(Lights[x].Identifier,GL_AMBIENT,Lights[x].Ambient);
		glLightfv(Lights[x].Identifier,GL_DIFFUSE,Lights[x].Color);
		glLightfv(Lights[x].Identifier,GL_SPECULAR,Lights[x].Color);

			float Dir[4];
			for (int a=0; a<4; Dir[a]=Lights[x].Position[a],a++);
			if (Lights[x].Position[3]==0)
			{
				float l=sqrt((Lights[x].Position[0]*Lights[x].Position[0])+(Lights[x].Position[1]*Lights[x].Position[1])+(Lights[x].Position[2]*Lights[x].Position[2]));
				Lights[x].Position[0]/=l;
				Lights[x].Position[1]/=l;
				Lights[x].Position[2]/=l;
			}
			
		glLightfv(Lights[x].Identifier,GL_POSITION,Dir);
		glLightfv(Lights[x].Identifier,GL_SPOT_DIRECTION,Lights[x].Spot_Direction);
		glLightfv(Lights[x].Identifier,GL_SPOT_EXPONENT,&Lights[x].Spot_Exponent);
		glLightfv(Lights[x].Identifier,GL_SPOT_CUTOFF,&Lights[x].Spot_Cutoff);
		glLightfv(Lights[x].Identifier,GL_CONSTANT_ATTENUATION,&Lights[x].Constant_Attenuation);
		glLightfv(Lights[x].Identifier,GL_LINEAR_ATTENUATION,&Lights[x].Linear_Attenuation);
		glLightfv(Lights[x].Identifier,GL_QUADRATIC_ATTENUATION,&Lights[x].Quadratic_Attenuation);
	}
	else
		glDisable(Lights[x].Identifier);
}

void Sphere()
{
	if (!MainGUI.ItemList[0].ButtonPushed)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINE_STRIP);
		for (int x=0; x<=24; x++)
			glVertex3f(0.5f*sin(x*360/24.0f*(float)radtheta),0.5f*cos(x*360/24.0f*(float)radtheta),0);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (int x=0; x<=24; x++)
			glVertex3f(0.5f*sin(x*360/24.0f*(float)radtheta),0,0.5f*cos(x*360/24.0f*(float)radtheta));
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (int x=0; x<=24; x++)
			glVertex3f(0,0.5f*sin(x*360/24.0f*(float)radtheta),0.5f*cos(x*360/24.0f*(float)radtheta));
		glEnd();
		glPopAttrib();
	}
}

void Plane()
{
	if (!MainGUI.ItemList[0].ButtonPushed)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);
		glVertex3d(-0.75,0,-0.75);
		glVertex3d(0.75,0,-0.75);
		glVertex3d(0.75,0,0.75);
		glVertex3d(-0.75,0,0.75);
		glVertex3d(-0.75,0,-0.75);
		glEnd();
		glPopAttrib();
	}
}

void Cylinder()
{
	if (!MainGUI.ItemList[0].ButtonPushed)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);
		for (int x=0; x<=24; x++)
			glVertex3f(0.5f*sin(x*360/24.0f*(float)radtheta),0.5,0.5f*cos(x*360/24.0f*(float)radtheta));
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (int x=0; x<=24; x++)
			glVertex3f(0.5f*sin(x*360/24.0f*(float)radtheta),-0.5,0.5f*cos(x*360/24.0f*(float)radtheta));
		glEnd();
		glBegin(GL_LINES);
		for (int x=0; x<8; x++)
		{
			glVertex3f(0.5f*sin(x*360/8.0f*(float)radtheta),-0.5,0.5f*cos(x*360/8.0f*(float)radtheta));
			glVertex3f(0.5f*sin(x*360/8.0f*(float)radtheta),0.5,0.5f*cos(x*360/8.0f*(float)radtheta));
		}
		glEnd();
		glPopAttrib();
	}
}

void Cube()
{
	if (!MainGUI.ItemList[0].ButtonPushed)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);
		glVertex3f(-0.5f,-0.5f,-0.5f);
		glVertex3f(0.5f,-0.5f,-0.5f);
		glVertex3f(0.5f,0.5f,-0.5f);
		glVertex3f(-0.5f,0.5f,-0.5f);
		glVertex3f(-0.5f,-0.5f,-0.5f);
		glVertex3f(-0.5f,-0.5f,0.5f);
		glVertex3f(0.5f,-0.5f,0.5f);
		glVertex3f(0.5f,0.5f,0.5f);
		glVertex3f(-0.5f,0.5f,0.5f);
		glVertex3f(-0.5f,-0.5f,0.5f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(-0.5f,0.5f,-0.5f);
		glVertex3f(-0.5f,0.5f,0.5f);
		glVertex3f(0.5f,0.5f,-0.5f);
		glVertex3f(0.5f,0.5f,0.5f);
		glVertex3f(0.5f,-0.5f,-0.5f);
		glVertex3f(0.5f,-0.5f,0.5f);
		glEnd();
		glPopAttrib();
	}
}

void CloneWireframe(OBJECT *o)
{
	if (o && !o->Deleted)
	{
		for (int x=0; x<o->Param1; x++)
		{
			OBJECT *o2=((OBJECT*)((int*)o->Param3)[x]);
			if (!o2->Deleted)
			{
				glPushMatrix();
				MultModelViewMatrix(o2->ModelView,1);
				if (o2->Primitive!=aDDict_CLONE)
				{
					glCallList(o2->WireDisplayList);
					CallCounter++;
				}
				else
				CloneWireframe(o2);
				glPopMatrix();
			}
		}
	}
}

void SCENE::RenderWireFrame(VECTOR3 v, bool Layouter)
{
	MATRIX m;
	M_Scale(v.x,v.y,v.z,m);

	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	if (!Layouter) glColor4f(highblue);
	glPushMatrix();
	MultModelViewMatrix(m,1);
	int x;
	for (x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted && !ObjectList[x].ListItem->Hidden)
	{
		if (!ObjectList[x].ListItem->Selected || Layouter)
		{
			glPushMatrix();
			//MultModelViewMatrix(m);
			MultModelViewMatrix(ObjectList[x].ModelView,1);
			//if (ObjectList[x].Primitive!=13 && ObjectList[x].Primitive!=12 && ObjectList[x].Primitive!=11 && ObjectList[x].Primitive!=aDDict_GRAVITY && ObjectList[x].Primitive!=aDDict_SPHEREDEFLECTOR && ObjectList[x].Primitive!=aDDict_PLANEDEFLECTOR)
			if (ObjectList[x].Primitive!=aDDict_CLONE)
			{
				glCallList(ObjectList[x].WireDisplayList);
				CallCounter++;
			}
			else
			CloneWireframe(&ObjectList[x]);
			glPopMatrix();
		}
	}
	if (!Layouter)
	{
		glColor4f(lightblue);
		for (x=0; x<ObjectNum; x++)
		if (!ObjectList[x].Deleted && !ObjectList[x].ListItem->Hidden)
		{
			if (ObjectList[x].ListItem->Selected)
			{
				glPushMatrix();
				//MultModelViewMatrix(m);
				MultModelViewMatrix(ObjectList[x].ModelView,1);
				//if (ObjectList[x].Primitive!=13 && ObjectList[x].Primitive!=12 && ObjectList[x].Primitive!=11 && ObjectList[x].Primitive!=aDDict_GRAVITY && ObjectList[x].Primitive!=aDDict_SPHEREDEFLECTOR && ObjectList[x].Primitive!=aDDict_PLANEDEFLECTOR)
				if (ObjectList[x].Primitive!=aDDict_CLONE)
				{
					glCallList(ObjectList[x].WireDisplayList);
					CallCounter++;
				}
				else
				CloneWireframe(&ObjectList[x]);
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
	if (!Layouter) glColor4f(1,1,1,1);
	//glPopAttrib();
}

void OBJECT::Render(bool Zbuffer,RGBA SceneColor, bool ColorDiscard)
{
	if (this && !ListItem->Hidden && !Deleted)
	{
		if (Backface) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (Backfront) glCullFace(GL_FRONT); else glCullFace(GL_BACK);

		if (AA)
		{
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
		}
		else
		{
			glDisable(GL_POLYGON_SMOOTH);
		}

		if (Primitive!=aDDict_CLONE && Primitive<aDDict_MAPXFORM)
		{
			glAlphaFunc(GL_GEQUAL,AEpsilon/255.0f);
			if (ZMask==Zbuffer)
			{
				float c[4];
				c[0]=c[1]=c[2]=c[3]=1;

				glBlendFunc(DSTBlend,SRCBlend);
				if (!Material) Textured=false;
				if (Textured)
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,Material->ImageData.TextureHandle);
				}
				else glDisable(GL_TEXTURE_2D);
				glPushMatrix();
				MultModelViewMatrix(ModelView,1);

				if (!ColorDiscard)
				{
					c[0]=Color.r/255.0f;
					c[1]=Color.g/255.0f;
					c[2]=Color.b/255.0f;
					c[3]=Color.a/255.0f;
				}
				else
				{
					c[0]=SceneColor.r/255.0f;
					c[1]=SceneColor.g/255.0f;
					c[2]=SceneColor.b/255.0f;
					c[3]=SceneColor.a/255.0f;
				}
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,c);
				glColor4fv(c);
				
				PolyCounter+=PolygonNum;
				VertexCounter+=VertexNum;
				if (!Wireframe)
				{
					glCallList(DisplayList);
					CallCounter++;
				}
				else
				{	
					glPushAttrib(GL_ALL_ATTRIB_BITS);
					glDisable(GL_LIGHTING);
					glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
					glEnable(GL_LINE_SMOOTH);
					glCallList(WireDisplayList);
					CallCounter++;
					glPopAttrib();
				}
				if (!EnvMap) EnvMapped=false;
				if (EnvMapped)
				{
					//glPushAttrib(GL_ALL_ATTRIB_BITS);
					//glEnable(GL_BLEND);
					glBlendFunc(GL_ONE,GL_ONE);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,EnvMap->ImageData.TextureHandle);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
					glDepthFunc(GL_LEQUAL);
					glEnable(GL_TEXTURE_GEN_S);
					glEnable(GL_TEXTURE_GEN_T);
					glCallList(DisplayList);
					CallCounter++;
					glDisable(GL_TEXTURE_GEN_S);
					glDisable(GL_TEXTURE_GEN_T);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
					//glPopAttrib();
				}

				//normalok:

				/*glColor4f(0,1,0,1);
				glDisable(GL_BLEND);
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_TEXTURE_2D);
				glBlendFunc(GL_ONE,GL_ZERO);
				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				for (int x=0; x<VertexNum; x++)
				{
					glVertex3f(VertexList[x].Position.x,VertexList[x].Position.y,VertexList[x].Position.z);
					glVertex3f(VertexList[x].Position.x+VertexList[x].Normal.x/20.0f,
						       VertexList[x].Position.y+VertexList[x].Normal.y/20.0f,
							   VertexList[x].Position.z+VertexList[x].Normal.z/20.0f);
				}
				glEnd();*/

				glPopMatrix();
			}
		}
		else
		if (Primitive==aDDict_CLONE)
		{
			glPushMatrix();
			MultModelViewMatrix(ModelView,1);
			for (int x=0; x<Param1; x++)
			{
				((OBJECT*)((int*)Param3)[x])->Render(Zbuffer,SceneColor,ColorDiscard);
			}
			glPopMatrix();
		}

	}
}

void SCENE::Render(bool Lighting)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,1/256.0f);

	float c[4];
	c[0]=c[1]=c[2]=c[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,c);
	glColor4f(1,1,1,1);

	if (Lighting)
	SetupLighting();
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	//glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	int x;
	for (x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted && (ObjectList[x].ZMask || ObjectList[x].Primitive==aDDict_CLONE) && !(Maptransforming && MappedObject==x)  && ObjectList[x].Primitive<aDDict_MAPXFORM)
	{
		RGBA Color;
		Color.dw=0xffffffff;
		ObjectList[x].Render(true,Color,ColorDiscard);
		if (ObjectList[x].ListItem->Selected && Lighting && ObjectList[x].Primitive<aDDict_MAPXFORM)
		{
			glPushMatrix();
			MultModelViewMatrix(ObjectList[x].ModelView,1);
			//glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
			glColor4f(0,0,1,1);
			glDisable(GL_LIGHTING);
			glDisable(GL_LINE_SMOOTH);
			glBegin(GL_LINE_STRIP);			
			glVertex3d(-0.51,-0.51,0.51);
			glVertex3d( 0.51,-0.51,0.51);
			glVertex3d( 0.51, 0.51,0.51);
			glVertex3d(-0.51, 0.51,0.51);
			glVertex3d(-0.51,-0.51,0.51);

			glVertex3d(-0.51,-0.51,-0.51);
			glVertex3d( 0.51,-0.51,-0.51);
			glVertex3d( 0.51, 0.51,-0.51);
			glVertex3d(-0.51, 0.51,-0.51);
			glVertex3d(-0.51,-0.51,-0.51);
			glEnd();
			glBegin(GL_LINES);
			glVertex3d( 0.51,-0.51,0.51);
			glVertex3d( 0.51,-0.51,-0.51);
			glVertex3d( 0.51, 0.51,0.51);
			glVertex3d( 0.51, 0.51,-0.51);
			glVertex3d(-0.51, 0.51,0.51);
			glVertex3d(-0.51, 0.51,-0.51);
			glEnd();
			//glPopAttrib();
			glPopMatrix();
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
		}
	}

	glDepthMask(false);

	for (x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted && (!ObjectList[x].ZMask || ObjectList[x].Primitive==aDDict_CLONE) && !(Maptransforming && MappedObject==x)  && ObjectList[x].Primitive<aDDict_MAPXFORM)
	{
		RGBA Color;
		Color.dw=0xffffffff;
		ObjectList[x].Render(false,Color,ColorDiscard);
		if (ObjectList[x].ListItem->Selected)
		{
			glPushMatrix();
			MultModelViewMatrix(ObjectList[x].ModelView,1);
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
			glColor4f(0,0,1,1);
			glDisable(GL_LIGHTING);
			glDisable(GL_LINE_SMOOTH);
			glBegin(GL_LINE_STRIP);			
			glVertex3d(-0.51,-0.51,0.51);
			glVertex3d( 0.51,-0.51,0.51);
			glVertex3d( 0.51, 0.51,0.51);
			glVertex3d(-0.51, 0.51,0.51);
			glVertex3d(-0.51,-0.51,0.51);

			glVertex3d(-0.51,-0.51,-0.51);
			glVertex3d( 0.51,-0.51,-0.51);
			glVertex3d( 0.51, 0.51,-0.51);
			glVertex3d(-0.51, 0.51,-0.51);
			glVertex3d(-0.51,-0.51,-0.51);
			glEnd();
			glBegin(GL_LINES);
			glVertex3d( 0.51,-0.51,0.51);
			glVertex3d( 0.51,-0.51,-0.51);
			glVertex3d( 0.51, 0.51,0.51);
			glVertex3d( 0.51, 0.51,-0.51);
			glVertex3d(-0.51, 0.51,0.51);
			glVertex3d(-0.51, 0.51,-0.51);
			glEnd();
			glPopAttrib();
			glPopMatrix();
		}
	}

	glDepthMask(true);
	glPopAttrib();
}

void SCENE::Render(bool Lighting, bool ZMask, RGBA Color)
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	float c[4];
	c[0]=c[1]=c[2]=c[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,c);
	glColor4f(1,1,1,1);

	if (Lighting)
	SetupLighting();
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	//glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthMask(true);

	if (ZMask)
	for (int x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted && (ObjectList[x].ZMask || ObjectList[x].Primitive==aDDict_CLONE))
	{
		ObjectList[x].Render(true,Color,ColorDiscard);
		if (ObjectList[x].ListItem->Selected && Lighting)
		{
			glPushMatrix();
			MultModelViewMatrix(ObjectList[x].ModelView,1);
			glPopMatrix();
		}
	}

	glDepthMask(false);

	if (!ZMask)
	for (int x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted && (!ObjectList[x].ZMask || ObjectList[x].Primitive==aDDict_CLONE))
	{
		ObjectList[x].Render(false,Color,ColorDiscard);
		if (ObjectList[x].ListItem->Selected)
		{
			glPushMatrix();
			MultModelViewMatrix(ObjectList[x].ModelView,1);
			glPopMatrix();
		}
	}

	glDepthMask(true);
	//glPopAttrib();
}


void OBJECT::AddVertex(float x, float y, float z, float u, float v)
{
	if (VertexCapacity==VertexNum)
	{
		VERTEX *Buffer=new VERTEX[(VertexCapacity+10)*2];
		memset(Buffer,0,sizeof(VERTEX)*(VertexCapacity+10)*2);
		memcpy(Buffer,VertexList,sizeof(VERTEX)*VertexCapacity);
		delete VertexList;
		VertexList=Buffer;
		VertexCapacity=(VertexCapacity+10)*2;
	}

	VertexList[VertexNum].Position.x=x;
	VertexList[VertexNum].Position.y=y;
	VertexList[VertexNum].Position.z=z;
	//VertexList[VertexNum].PrimitivePosition=VertexList[VertexNum].MapTransformedPosition=VertexList[VertexNum].Position;

	VertexList[VertexNum].TextureCoordinate.u=u;
	VertexList[VertexNum].TextureCoordinate.v=v;
	VertexList[VertexNum].CurrentTextureCoordinate=VertexList[VertexNum].TextureCoordinate;

	VertexList[VertexNum].EdgeCapacity=6;
	VertexList[VertexNum].EdgeNum=0;
	VertexList[VertexNum].EdgeList=new int[6];

	VertexNum++;
}

void OBJECT::AddPolygon(int x, int y, int z, bool e1, bool e2, bool e3)
{
	if (PolygonCapacity==PolygonNum)
	{
		POLYGON *Buffer=new POLYGON[(PolygonCapacity+10)*2];
		memset(Buffer,0,sizeof(POLYGON)*(PolygonCapacity+10)*2);
		memcpy(Buffer,PolygonList,sizeof(POLYGON)*PolygonCapacity);
		delete PolygonList;
		PolygonList=Buffer;
		PolygonCapacity=(PolygonCapacity+10)*2;
	}
	PolygonList[PolygonNum].v1=x;
	PolygonList[PolygonNum].v2=y;
	PolygonList[PolygonNum].v3=z;
	PolygonList[PolygonNum].t[0]=VertexList[x].TextureCoordinate;
	PolygonList[PolygonNum].t[1]=VertexList[y].TextureCoordinate;
	PolygonList[PolygonNum].t[2]=VertexList[z].TextureCoordinate;
	PolygonList[PolygonNum].ct[0]=PolygonList[PolygonNum].t[0];
	PolygonList[PolygonNum].ct[1]=PolygonList[PolygonNum].t[1];
	PolygonList[PolygonNum].ct[2]=PolygonList[PolygonNum].t[2];
	VECTOR3 xy=V3_Sub(VertexList[y].Position,VertexList[x].Position),
		    xz=V3_Sub(VertexList[z].Position,VertexList[x].Position);
	PolygonList[PolygonNum].Normal=PolygonList[PolygonNum].CurrentNormal=V3_Cross(xz,xy);	
	PolygonNum++;
	PolygonList[PolygonNum-1].e[0]=AddEdge(x,y,PolygonNum-1,e1);
	PolygonList[PolygonNum-1].e[1]=AddEdge(x,z,PolygonNum-1,e2);
	PolygonList[PolygonNum-1].e[2]=AddEdge(y,z,PolygonNum-1,e3);
}

void OBJECT::AddPolygon(int x, int y, int z, char Shading, bool e1, bool e2, bool e3)
{
	AddPolygon(x,y,z,e1,e2,e3);
	PolygonList[PolygonNum-1].Shading=Shading;
	PolygonList[PolygonNum-1].CurrentShading=Shading;
}

void OBJECT::AddPolygon(int x, int y, int z, char Shading, float t1x, float t1y,  float t2x, float t2y,  float t3x, float t3y, bool e1, bool e2, bool e3)
{
	AddPolygon(x,y,z,Shading,e1,e2,e3);
	PolygonList[PolygonNum-1].t[0].x=t1x;
	PolygonList[PolygonNum-1].t[0].y=t1y;
	PolygonList[PolygonNum-1].t[1].x=t2x;
	PolygonList[PolygonNum-1].t[1].y=t2y;
	PolygonList[PolygonNum-1].t[2].x=t3x;
	PolygonList[PolygonNum-1].t[2].y=t3y;
	PolygonList[PolygonNum-1].ct[0]=PolygonList[PolygonNum-1].t[0];
	PolygonList[PolygonNum-1].ct[1]=PolygonList[PolygonNum-1].t[1];
	PolygonList[PolygonNum-1].ct[2]=PolygonList[PolygonNum-1].t[2];
}

void OBJECT::AddPolygon(int x, int y, int z, float t1x, float t1y,  float t2x, float t2y,  float t3x, float t3y, bool e1, bool e2, bool e3)
{
	AddPolygon(x,y,z,e1,e2,e3);
	PolygonList[PolygonNum-1].t[0].x=t1x;
	PolygonList[PolygonNum-1].t[0].y=t1y;
	PolygonList[PolygonNum-1].t[1].x=t2x;
	PolygonList[PolygonNum-1].t[1].y=t2y;
	PolygonList[PolygonNum-1].t[2].x=t3x;
	PolygonList[PolygonNum-1].t[2].y=t3y;
	PolygonList[PolygonNum-1].ct[0]=PolygonList[PolygonNum-1].t[0];
	PolygonList[PolygonNum-1].ct[1]=PolygonList[PolygonNum-1].t[1];
	PolygonList[PolygonNum-1].ct[2]=PolygonList[PolygonNum-1].t[2];
}

void AddVertexEdge(OBJECT *o,int v, int e)
{
	if (o->VertexList[v].EdgeCapacity==o->VertexList[v].EdgeNum)
	{
		int *Buffer=new int[o->VertexList[v].EdgeCapacity*2];
		memcpy(Buffer,o->VertexList[v].EdgeList,o->VertexList[v].EdgeCapacity*sizeof(int));
		delete[] o->VertexList[v].EdgeList;
		o->VertexList[v].EdgeList=Buffer;
		o->VertexList[v].EdgeCapacity*=2;
	}
	o->VertexList[v].EdgeList[o->VertexList[v].EdgeNum]=e;
	o->VertexList[v].EdgeNum++;
}

int OBJECT::AddEdge(int a, int b, int p, bool InWireframe)
{
	if (EdgeCapacity==EdgeNum)
	{
		EDGE *Buffer=new EDGE[(EdgeCapacity+10)*2];
		memset(Buffer,0,sizeof(EDGE)*(EdgeCapacity+10)*2);
		memcpy(Buffer,EdgeList,sizeof(EDGE)*EdgeCapacity);
		delete[] EdgeList;
		EdgeList=Buffer;
		EdgeCapacity=(EdgeCapacity+10)*2;
	}
	int EID;
	EDGE Buffer;
	Buffer.v1=min(a,b);
	Buffer.v2=max(a,b);
	Buffer.p1=p;
	Buffer.p2=-1;
	Buffer.InWireframe=InWireframe;
	bool Found=false;
	for (int x=EdgeNum-1; x>=0 && !Found; x--)
		if (EdgeList[x].v1==Buffer.v1 && EdgeList[x].v2==Buffer.v2) 
		{
			EdgeList[x].p2=p;
			EdgeList[x].InWireframe|=InWireframe;
			Found=true;
			EID=x;
		}
	if (!Found)
	{
		EdgeList[EdgeNum]=Buffer;
		EID=EdgeNum;
		AddVertexEdge(this,a,EdgeNum);
		AddVertexEdge(this,b,EdgeNum);
		EdgeNum++;
	}
	return EID;
}

void OBJECT::CalculateTextureCoordinates()
{
	for (int x=0; x<PolygonNum; x++)
	{
		float xmul=1,ymul=1;
		if (XSwap) xmul=-1;
		if (YSwap) ymul=-1;

		if (!Swap)
		{
			PolygonList[x].ct[0].x=(PolygonList[x].t[0].x*Xtile+((unsigned char)(OffsetX*255))/255.0f)*xmul;
			PolygonList[x].ct[0].y=(PolygonList[x].t[0].y*Ytile+((unsigned char)(OffsetY*255))/255.0f)*ymul;

			PolygonList[x].ct[1].x=(PolygonList[x].t[1].x*Xtile+((unsigned char)(OffsetX*255))/255.0f)*xmul;
			PolygonList[x].ct[1].y=(PolygonList[x].t[1].y*Ytile+((unsigned char)(OffsetY*255))/255.0f)*ymul;

			PolygonList[x].ct[2].x=(PolygonList[x].t[2].x*Xtile+((unsigned char)(OffsetX*255))/255.0f)*xmul;
			PolygonList[x].ct[2].y=(PolygonList[x].t[2].y*Ytile+((unsigned char)(OffsetY*255))/255.0f)*ymul;
		}
		else
		{
			PolygonList[x].ct[0].y=(PolygonList[x].t[0].x*Xtile+((unsigned char)(OffsetX*255))/255.0f)*xmul;
			PolygonList[x].ct[0].x=(PolygonList[x].t[0].y*Ytile+((unsigned char)(OffsetY*255))/255.0f)*ymul;

			PolygonList[x].ct[1].y=(PolygonList[x].t[1].x*Xtile+((unsigned char)(OffsetX*255))/255.0f)*xmul;
			PolygonList[x].ct[1].x=(PolygonList[x].t[1].y*Ytile+((unsigned char)(OffsetY*255))/255.0f)*ymul;

			PolygonList[x].ct[2].y=(PolygonList[x].t[2].x*Xtile+((unsigned char)(OffsetX*255))/255.0f)*xmul;
			PolygonList[x].ct[2].x=(PolygonList[x].t[2].y*Ytile+((unsigned char)(OffsetY*255))/255.0f)*ymul;
		}

	}
}

void OBJECT::CalculateNormals()
{
	int x;
	for (x=0; x<VertexNum; x++)	VertexList[x].Normal=V3_Make(0,0,0);
	for (x=0; x<PolygonNum; x++)
	{
		PolygonList[x].Normal=V3_Normalize(
							  V3_Cross(V3_Sub(VertexList[PolygonList[x].v3].Position,VertexList[PolygonList[x].v1].Position),
									   V3_Sub(VertexList[PolygonList[x].v2].Position,VertexList[PolygonList[x].v1].Position))
									 );

		if (NormalsInverted) 
			
			PolygonList[x].CurrentNormal=V3_Mults(PolygonList[x].Normal,-1); else 
			PolygonList[x].CurrentNormal=PolygonList[x].Normal;

		VertexList[PolygonList[x].v1].Normal=V3_Add(VertexList[PolygonList[x].v1].Normal,PolygonList[x].CurrentNormal);
		VertexList[PolygonList[x].v2].Normal=V3_Add(VertexList[PolygonList[x].v2].Normal,PolygonList[x].CurrentNormal);
		VertexList[PolygonList[x].v3].Normal=V3_Add(VertexList[PolygonList[x].v3].Normal,PolygonList[x].CurrentNormal);
	}
	for (x=0; x<VertexNum; x++)	VertexList[x].CurrentNormal=V3_Normalize(VertexList[x].Normal);
}

void OBJECT::CreateDisplayList()
{
	if (glIsList(WireDisplayList)) glDeleteLists(WireDisplayList,1);
	glNewList(WireDisplayList,GL_COMPILE);

	glBegin(GL_LINES);
	int x;
	for (x=0; x<EdgeNum; x++) if (EdgeList[x].InWireframe)
	{
		glVertex3f(VertexList[EdgeList[x].v1].Position.x,VertexList[EdgeList[x].v1].Position.y,VertexList[EdgeList[x].v1].Position.z);
		glVertex3f(VertexList[EdgeList[x].v2].Position.x,VertexList[EdgeList[x].v2].Position.y,VertexList[EdgeList[x].v2].Position.z);
	}
	glEnd();

	glEndList();

	if (glIsList(DisplayList)) glDeleteLists(DisplayList,1);
	glNewList(DisplayList,GL_COMPILE);

	if (PerVertexColor) glEnable(GL_COLOR_MATERIAL);

	glBegin(GL_TRIANGLES);

	for (x=0; x<PolygonNum; x++)
	{
		if (PerVertexColor) glColor3fv(VertexList[PolygonList[x].v1].Color);
		glNormal3f(PolygonList[x].CurrentNormal.x,PolygonList[x].CurrentNormal.y,PolygonList[x].CurrentNormal.z);
		if (PolygonList[x].CurrentShading==aDDict_GOURAUDSHADE) glNormal3f(VertexList[PolygonList[x].v1].CurrentNormal.x,VertexList[PolygonList[x].v1].CurrentNormal.y,VertexList[PolygonList[x].v1].CurrentNormal.z);
		glTexCoord2f(PolygonList[x].ct[0].x,PolygonList[x].ct[0].y);
		glVertex3f(VertexList[PolygonList[x].v1].Position.x,VertexList[PolygonList[x].v1].Position.y,VertexList[PolygonList[x].v1].Position.z);
		
		if (PerVertexColor) glColor3fv(VertexList[PolygonList[x].v2].Color);
		
		if (PolygonList[x].CurrentShading==aDDict_GOURAUDSHADE) glNormal3f(VertexList[PolygonList[x].v2].CurrentNormal.x,VertexList[PolygonList[x].v2].CurrentNormal.y,VertexList[PolygonList[x].v2].CurrentNormal.z);
		glTexCoord2f(PolygonList[x].ct[1].x,PolygonList[x].ct[1].y);
		glVertex3f(VertexList[PolygonList[x].v2].Position.x,VertexList[PolygonList[x].v2].Position.y,VertexList[PolygonList[x].v2].Position.z);

		if (PerVertexColor) glColor3fv(VertexList[PolygonList[x].v3].Color);
		if (PolygonList[x].CurrentShading==aDDict_GOURAUDSHADE) glNormal3f(VertexList[PolygonList[x].v3].CurrentNormal.x,VertexList[PolygonList[x].v3].CurrentNormal.y,VertexList[PolygonList[x].v3].CurrentNormal.z);
		glTexCoord2f(PolygonList[x].ct[2].x,PolygonList[x].ct[2].y);		
		glVertex3f(VertexList[PolygonList[x].v3].Position.x,VertexList[PolygonList[x].v3].Position.y,VertexList[PolygonList[x].v3].Position.z);
	}

	glEnd();

	glEndList();
}

void WORLD::CalculateParticles()
{
	srand(timeGetTime());
	int CurrentFrame=timeGetTime();
	int x;
	for (x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted)
	{
		int a,b;
		switch (ObjectList[x].Primitive)
		{
		case aDDict_PLANEDEFLECTOR:
			{
			VECTOR3 n=V3_Make(0,1,0);
			memcpy(ObjectList[x].Inverted,ObjectList[x].ModelView,16*4);
			M_Invert(ObjectList[x].Inverted);
			for (a=0; a<4; a++)
				for (b=0; b<4; b++)
					ObjectList[x].IT[a][b]=ObjectList[x].Inverted[b][a];
			M_Xformd(ObjectList[x].IT,n,n);
			ObjectList[x].EmitterData->n=n=V3_Normalize(n);
			ObjectList[x].EmitterData->d=ObjectList[x].ModelView[0][3]*n.x+ObjectList[x].ModelView[1][3]*n.y+ObjectList[x].ModelView[2][3]*n.z;

			break;
			}
		case aDDict_SPHEREDEFLECTOR:
			{
			WORLDOBJECT *p=ObjectList[x].Parent;
			memcpy(ObjectList[x].Hierarchy,ObjectList[x].ModelView,16*4);
			while (p)
			{
				M_Mult(p->ModelView,ObjectList[x].Hierarchy,ObjectList[x].Hierarchy);
				p=p->Parent;
			}

			memcpy(ObjectList[x].Inverted,&ObjectList[x].Hierarchy,16*4);
			M_Invert(ObjectList[x].Inverted);
			for (a=0; a<4; a++)
				for (b=0; b<4; b++)
					ObjectList[x].IT[a][b]=ObjectList[x].Inverted[b][a];
			break;
			}
		case aDDict_SPHERESLOWER:
			{
			WORLDOBJECT *p=ObjectList[x].Parent;
			memcpy(ObjectList[x].Hierarchy,ObjectList[x].ModelView,16*4);
			while (p)
			{
				M_Mult(p->ModelView,ObjectList[x].Hierarchy,ObjectList[x].Hierarchy);
				p=p->Parent;
			}

			memcpy(ObjectList[x].Inverted,&ObjectList[x].Hierarchy,16*4);
			M_Invert(ObjectList[x].Inverted);
			for (a=0; a<4; a++)
				for (b=0; b<4; b++)
					ObjectList[x].IT[a][b]=ObjectList[x].Inverted[b][a];
			break;
			}
		}
	}
	for (x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted)
	{
		switch (ObjectList[x].Primitive)
		{
		case aDDict_CUBEEMITTER:
		case aDDict_SPHEREEMITTER:
		case aDDict_CYLINDEREMITTER:
			if (ObjectList[x].EmitterData->LastFrameChecked>CurrentFrame)
			{
				/*MATRIX Parent;
				M_Identity(Parent);
				int ParentID=ObjectList[x].ID;
				WORLDOBJECT *P;
				while (ParentID!=-1)
				{
					if (FindObjectByID(ParentID))
					{
						P=FindObjectByID(ParentID);
						M_Mult(P->ModelView,Parent,Parent);
						ParentID=P->ParentID;
					}
					else
					{
						P->ParentID=ParentID=-1;
					}
				}*/
				float t=min(1,(CurrentFrame-(ObjectList[x].EmitterData->LastFrameChecked-25))/25.0f);
				for (int z=0; z<ObjectList[x].EmitterData->MaxParticles; z++)
				{
					ObjectList[x].EmitterData->Particles[z].DisplayPos.x=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.x,ObjectList[x].EmitterData->Particles[z].Position.x,t);
					ObjectList[x].EmitterData->Particles[z].DisplayPos.y=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.y,ObjectList[x].EmitterData->Particles[z].Position.y,t);
					ObjectList[x].EmitterData->Particles[z].DisplayPos.z=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.z,ObjectList[x].EmitterData->Particles[z].Position.z,t);
				}
			}
			else
			{

				MATRIX Parent;
				M_Identity(Parent);
				int ParentID=ObjectList[x].ID;
				WORLDOBJECT *P;
				while (ParentID!=-1)
				{
					if (FindObjectByID(ParentID))
					{
						P=FindObjectByID(ParentID);
						M_Mult(P->ModelView,Parent,Parent);
						ParentID=P->ParentID;
					}
					else
					{
						P->ParentID=ParentID=-1;
					}
				}

				VECTOR3 Dir=V3_Make(0,0,0);
				
				if (ObjectList[x].EmitterData->Dir.x!=0 || ObjectList[x].EmitterData->Dir.y!=0 || ObjectList[x].EmitterData->Dir.z!=0)
				Dir=V3_Normalize(ObjectList[x].EmitterData->Dir);

				int Frame;
				for (Frame=ObjectList[x].EmitterData->LastFrameChecked; Frame<=CurrentFrame; Frame+=25)
				{
					float t=min(1,(CurrentFrame-Frame)/25.0f);
					if (ObjectList[x].EmitterData->DefaultAge)
					ObjectList[x].EmitterData->ParticleNumBuffer+=max(0,ObjectList[x].EmitterData->ParticlesPerFrame);
					for (; ObjectList[x].EmitterData->ParticleNumBuffer>=1; ObjectList[x].EmitterData->ParticleNumBuffer-=1)
					{
						float minage=1; int Particle=0;
						for (int z=0; z<ObjectList[x].EmitterData->MaxParticles; z++)
						{
							if (ObjectList[x].EmitterData->Particles[z].Age/(float)ObjectList[x].EmitterData->Particles[z].StartAge<minage) {
								minage=ObjectList[x].EmitterData->Particles[z].Age/(float)ObjectList[x].EmitterData->Particles[z].StartAge;
								Particle=z;
							}
							if (!ObjectList[x].EmitterData->Particles[z].Active) 
							{
								Particle=z;
								z=ObjectList[x].EmitterData->MaxParticles;
							}
						}
						ObjectList[x].EmitterData->Particles[Particle].Active=true;
						ObjectList[x].EmitterData->Particles[Particle].Aging=ObjectList[x].EmitterData->DefaultAge!=0;
						ObjectList[x].EmitterData->Particles[Particle].StartAge=ObjectList[x].EmitterData->Particles[Particle].Age=ObjectList[x].EmitterData->DefaultAge+(int)(((rand()/(float)RAND_MAX)-0.5f)*ObjectList[x].EmitterData->AgeChaos)+(int)!ObjectList[x].EmitterData->Particles[Particle].Aging;
						ObjectList[x].EmitterData->Particles[Particle].Color1[0]=ObjectList[x].EmitterData->Color1[0]+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color1[4];
						ObjectList[x].EmitterData->Particles[Particle].Color1[1]=ObjectList[x].EmitterData->Color1[1]+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color1[4];
						ObjectList[x].EmitterData->Particles[Particle].Color1[2]=ObjectList[x].EmitterData->Color1[2]+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color1[4];
						ObjectList[x].EmitterData->Particles[Particle].Rotation=0;
						if (ObjectList[x].EmitterData->RandRotate)
						ObjectList[x].EmitterData->Particles[Particle].Rotation=rand()/(float)RAND_MAX*360.0f;
						ObjectList[x].EmitterData->Particles[Particle].RotChaos=rand()/(float)RAND_MAX*(float)ObjectList[x].EmitterData->RotChaos;

						ObjectList[x].EmitterData->Particles[Particle].Size=ObjectList[x].EmitterData->Size*(1+(rand()/(float)RAND_MAX*2-1)*ObjectList[x].EmitterData->Color2[4]);
						
						if (ObjectList[x].EmitterData->Color1[3]!=0)
						ObjectList[x].EmitterData->Particles[Particle].Color1[3]=ObjectList[x].EmitterData->Color1[3]+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color1[4];
						else ObjectList[x].EmitterData->Particles[Particle].Color1[3]=0;
						
						ObjectList[x].EmitterData->Particles[Particle].Color2[0]=ObjectList[x].EmitterData->Color2[0];//+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color2[4];
						ObjectList[x].EmitterData->Particles[Particle].Color2[1]=ObjectList[x].EmitterData->Color2[1];//+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color2[4];
						ObjectList[x].EmitterData->Particles[Particle].Color2[2]=ObjectList[x].EmitterData->Color2[2];//+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color2[4];
						
						if (ObjectList[x].EmitterData->Color2[3]!=0)
						ObjectList[x].EmitterData->Particles[Particle].Color2[3]=ObjectList[x].EmitterData->Color2[3];//+(rand()/float(RAND_MAX)-0.5f)*2*ObjectList[x].EmitterData->Color2[4];
						else ObjectList[x].EmitterData->Particles[Particle].Color2[3]=0;

						VECTOR3 Normal;

						ObjectList[x].EmitterData->Particles[Particle].Position=V3_Make((rand()/(float)RAND_MAX)-0.5f,(rand()/(float)RAND_MAX)-0.5f,(rand()/(float)RAND_MAX)-0.5f);

						VECTOR3 Helper;
						Helper.x=rand()/(float)RAND_MAX-0.5f;
						Helper.y=rand()/(float)RAND_MAX-0.5f;
						Helper.z=rand()/(float)RAND_MAX-0.5f;
						Helper=V3_Normalize(Helper);
						Helper=V3_Cross(Helper,Dir);

						float phi=(rand()/(float)RAND_MAX-0.5f)*ObjectList[x].EmitterData->DirChaos*radtheta/2.0f;

						MATRIX r;
						M_Rotate(Helper.x,Helper.y,Helper.z,phi,r);

						M_Xform3(r,Dir,Dir);

						ObjectList[x].EmitterData->Particles[Particle].Speed=V3_Mults(Dir,((rand()/(float)RAND_MAX)-0.5f)*ObjectList[x].EmitterData->SpeedChaos+ObjectList[x].EmitterData->Speed);
						//ObjectList[x].EmitterData->Particles[Particle].Speed=V3_Make(1,1,1);
						ObjectList[x].EmitterData->Particles[Particle].TailCnt=0;

						if (ObjectList[x].Primitive!=aDDict_CUBEEMITTER)
						{
							if (ObjectList[x].Primitive==aDDict_CYLINDEREMITTER) ObjectList[x].EmitterData->Particles[Particle].Position.y=0;
							Normal=V3_Mults(V3_Normalize(ObjectList[x].EmitterData->Particles[Particle].Position),0.5f);
							ObjectList[x].EmitterData->Particles[Particle].Position.x=Normal.x*(rand()/(float)RAND_MAX*(1-ObjectList[x].EmitterData->Param1)+ObjectList[x].EmitterData->Param1);
							ObjectList[x].EmitterData->Particles[Particle].Position.z=Normal.z*(rand()/(float)RAND_MAX*(1-ObjectList[x].EmitterData->Param1)+ObjectList[x].EmitterData->Param1);
						}
						if (ObjectList[x].Primitive==aDDict_CYLINDEREMITTER) ObjectList[x].EmitterData->Particles[Particle].Position.y=rand()/(float)RAND_MAX-0.5f;
						if (ObjectList[x].Primitive==aDDict_SPHEREEMITTER) ObjectList[x].EmitterData->Particles[Particle].Position.y=Normal.y*(rand()/(float)RAND_MAX*(1-ObjectList[x].EmitterData->Param1)+ObjectList[x].EmitterData->Param1);
						
						//M_Xformd(ObjectList[x].ModelView,ObjectList[x].EmitterData->Particles[Particle].Position,ObjectList[x].EmitterData->Particles[Particle].Position);
						M_Xformd(Parent,ObjectList[x].EmitterData->Particles[Particle].Position,ObjectList[x].EmitterData->Particles[Particle].Position);

						/*if (ObjectList[x].ParentID>=0)
						{
							if (ObjectList[x].Parent)
							M_Xformd(((WORLDOBJECT*)ObjectList[x].Parent)->ModelView,ObjectList[x].EmitterData->Particles[Particle].Position,ObjectList[x].EmitterData->Particles[Particle].Position);
						}*/

						if (ObjectList[x].EmitterData->TailInfo)
						for (int a=0; a<256; a++)
						ObjectList[x].EmitterData->Particles[Particle].Tail[a]=ObjectList[x].EmitterData->Particles[Particle].Position;
					}

					for (int z=0; z<ObjectList[x].EmitterData->MaxParticles; z++)
					{
						/*if (_isnan(ObjectList[x].EmitterData->Particles[z].Speed.x) || 
							_isnan(ObjectList[x].EmitterData->Particles[z].Speed.y) || 
							_isnan(ObjectList[x].EmitterData->Particles[z].Speed.z) || 
							_isnan(ObjectList[x].EmitterData->Particles[z].Position.x) || 
							_isnan(ObjectList[x].EmitterData->Particles[z].Position.y) || 
							_isnan(ObjectList[x].EmitterData->Particles[z].Position.z)) ObjectList[x].EmitterData->Particles[z].Active=false;*/
						if (ObjectList[x].EmitterData->Particles[z].Active)
						{
							int a;
							for (a=0; a<ObjectNum; a++)
							if (!ObjectList[a].Deleted)
							{
								float ForcePower;
								VECTOR3 ForceDir;
								switch (ObjectList[a].Primitive)
								{
								case aDDict_GRAVITY:
									if (ObjectList[a].EmitterData->Param2!=1)
									{
										ForceDir=V3_Sub(V3_Make(ObjectList[a].ModelView[0][3],ObjectList[a].ModelView[1][3],ObjectList[a].ModelView[2][3]),ObjectList[x].EmitterData->Particles[z].Position);
										ForcePower=V3_Length(ForceDir);
										ForcePower=ObjectList[a].EmitterData->Param1/(ForcePower*ForcePower*ForcePower)*0.01f;
									}
									else
									{
										if (ObjectList[a].ModelView[0][3]!=0 || ObjectList[a].ModelView[1][3] || ObjectList[a].ModelView[2][3])
										{
											ForceDir=V3_Normalize(V3_Make(ObjectList[a].ModelView[0][3],ObjectList[a].ModelView[1][3],ObjectList[a].ModelView[2][3]));
											ForcePower=ObjectList[a].EmitterData->Param1*0.01f;
										}
										else ForceDir=V3_Make(0,0,0);
									}
									ObjectList[x].EmitterData->Particles[z].Speed=V3_Add(ObjectList[x].EmitterData->Particles[z].Speed,V3_Mults(ForceDir,ForcePower));
									break;
								}
							}
							for (a=0; a<ObjectNum; a++)
							if (!ObjectList[a].Deleted)
							{
								switch (ObjectList[a].Primitive)
								{
								case aDDict_SPHERESLOWER:
									{

									VECTOR3 Dir2=V3_Make(0,0,0);
									
									if (ObjectList[a].EmitterData->Dir.x!=0 || ObjectList[a].EmitterData->Dir.y!=0 || ObjectList[a].EmitterData->Dir.z!=0)
									Dir2=V3_Normalize(ObjectList[a].EmitterData->Dir);
									VECTOR3 Pos,Spd;

									M_Xformd(ObjectList[a].Inverted,ObjectList[x].EmitterData->Particles[z].Position,Pos);
									M_Xform3(ObjectList[a].Inverted,ObjectList[x].EmitterData->Particles[z].Speed,Spd);

									float cc=V3_Length(Pos);
									
									float ddd=(1-cc/0.5f)/100.0f;
									if (cc<=0.5f) ObjectList[x].EmitterData->Particles[z].Speed=V3_Add(V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,ObjectList[a].EmitterData->Param1),V3_Mults(Dir2,ObjectList[a].EmitterData->Param2*ddd));
									}
									break;
								case aDDict_PLANEDEFLECTOR:
									float a_,_b;
									a_=V3_Dot(ObjectList[a].EmitterData->n,ObjectList[x].EmitterData->Particles[z].Position)-ObjectList[a].EmitterData->d;
									_b=V3_Dot(ObjectList[a].EmitterData->n,V3_Add(ObjectList[x].EmitterData->Particles[z].Position,ObjectList[x].EmitterData->Particles[z].Speed))-ObjectList[a].EmitterData->d;
									if ((a_<0)!=(_b<0))
									{
										float t=-a_/V3_Dot(ObjectList[a].EmitterData->n,ObjectList[x].EmitterData->Particles[z].Speed);
										VECTOR3 p=V3_Add(ObjectList[x].EmitterData->Particles[z].Position,V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,t));
										ObjectList[x].EmitterData->Particles[z].Speed=V3_Mults(V3_Add(ObjectList[x].EmitterData->Particles[z].Speed,V3_Mults(ObjectList[a].EmitterData->n,-2.0f*V3_Dot(ObjectList[a].EmitterData->n,ObjectList[x].EmitterData->Particles[z].Speed))),ObjectList[a].EmitterData->Param1);
										ObjectList[x].EmitterData->Particles[z].Position=V3_Add(p,V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,t-1.0f));
									}
									break;
								case aDDict_SPHEREDEFLECTOR:

									VECTOR3 Pos,Spd;

									M_Xformd(ObjectList[a].Inverted,ObjectList[x].EmitterData->Particles[z].Position,Pos);
									M_Xform3(ObjectList[a].Inverted,ObjectList[x].EmitterData->Particles[z].Speed,Spd);

									float cc=V3_Dot(Pos,Pos);
									
									VECTOR3 _a=V3_Add(Pos,Spd);
									float d2=V3_Dot(_a,_a);

									if ((cc<=0.25)!=(d2<=0.25))
									{									
										float _a,b;
										_a=V3_Dot(Spd,Spd);
										b=2*V3_Dot(Pos,Spd);

										float b4ac=sqrt(b*b-4*_a*(cc-0.25f));
										float t;

										t=(-b+b4ac)/(2*_a);
										if (t>=1) t=(-b-b4ac)/(2*_a);

										VECTOR3 p,d;
										d=p=V3_Add(Pos,V3_Mults(Spd,t)); // utkozesi pont,normal iranya
										M_Xformd(ObjectList[a].IT,d,d);
										M_Xformd(ObjectList[a].Hierarchy,p,p);
										d=V3_Normalize(d); //ellipszoid feluleti normal

										ObjectList[x].EmitterData->Particles[z].Speed=V3_Mults(V3_Add(ObjectList[x].EmitterData->Particles[z].Speed,V3_Mults(d,-2.0f*V3_Dot(d,ObjectList[x].EmitterData->Particles[z].Speed))),ObjectList[a].EmitterData->Param1);
										ObjectList[x].EmitterData->Particles[z].Position=V3_Add(p,V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,t-1.0f));									

										ObjectList[x].EmitterData->Particles[z].TailCnt=(ObjectList[x].EmitterData->Particles[z].TailCnt+1)%((ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2);
										if (ObjectList[x].EmitterData->TailInfo)
										ObjectList[x].EmitterData->Particles[z].Tail[ObjectList[x].EmitterData->Particles[z].TailCnt]=p;
										ObjectList[x].EmitterData->Particles[z].TailCnt2=(ObjectList[x].EmitterData->Particles[z].TailCnt2+1)%ObjectList[x].EmitterData->TailLength2;
									}
									break;
								}
							}
							ObjectList[x].EmitterData->Particles[z].LastPos=ObjectList[x].EmitterData->Particles[z].Position;
							ObjectList[x].EmitterData->Particles[z].Position=V3_Add(ObjectList[x].EmitterData->Particles[z].Position,V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,1));
							ObjectList[x].EmitterData->Particles[z].DisplayPos.x=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.x,ObjectList[x].EmitterData->Particles[z].Position.x,t);
							ObjectList[x].EmitterData->Particles[z].DisplayPos.y=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.y,ObjectList[x].EmitterData->Particles[z].Position.y,t);
							ObjectList[x].EmitterData->Particles[z].DisplayPos.z=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.z,ObjectList[x].EmitterData->Particles[z].Position.z,t);
							ObjectList[x].EmitterData->Particles[z].TailCnt=(ObjectList[x].EmitterData->Particles[z].TailCnt+1)%((ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2);
							if (ObjectList[x].EmitterData->TailInfo)
							ObjectList[x].EmitterData->Particles[z].Tail[ObjectList[x].EmitterData->Particles[z].TailCnt]=ObjectList[x].EmitterData->Particles[z].Position;
							ObjectList[x].EmitterData->Particles[z].TailCnt2=(ObjectList[x].EmitterData->Particles[z].TailCnt2+1)%ObjectList[x].EmitterData->TailLength2;
							ObjectList[x].EmitterData->Particles[z].Rotation+=ObjectList[x].EmitterData->Rotspeed+ObjectList[x].EmitterData->Particles[z].RotChaos;

							if (ObjectList[x].EmitterData->Particles[z].Aging) 
							{
								ObjectList[x].EmitterData->Particles[z].Age--;
								if (ObjectList[x].EmitterData->Particles[z].Age<=0) ObjectList[x].EmitterData->Particles[z].Active=false;
							}
						}
					}
				}
				ObjectList[x].EmitterData->LastFrameChecked=Frame;
			}
		}
	}
}

void WORLD::SetupLighting()
{
	if (!Lighting) glDisable(GL_LIGHTING);
	else
	{
		glEnable(GL_LIGHTING);
		for (int x=0; x<8; x++)
		if (Lights[x].Lit)
		{
			glEnable(Lights[x].Identifier);
			glLightfv(Lights[x].Identifier,GL_AMBIENT,Lights[x].Ambient);
			glLightfv(Lights[x].Identifier,GL_DIFFUSE,Lights[x].Color);
			glLightfv(Lights[x].Identifier,GL_SPECULAR,Lights[x].Color);

			float Dir[4];
			for (int a=0; a<4; Dir[a]=Lights[x].Position[a],a++);
			if (Dir[3]==0)
			{
				float l=sqrt((Dir[0]*Dir[0])+(Dir[1]*Dir[1])+(Dir[2]*Dir[2]));
				Dir[0]/=l;
				Dir[1]/=l;
				Dir[2]/=l;
			}
			
			glLightfv(Lights[x].Identifier,GL_POSITION,Dir);
			glLightfv(Lights[x].Identifier,GL_SPOT_DIRECTION,Lights[x].Spot_Direction);
			glLightfv(Lights[x].Identifier,GL_SPOT_EXPONENT,&Lights[x].Spot_Exponent);
			glLightfv(Lights[x].Identifier,GL_SPOT_CUTOFF,&Lights[x].Spot_Cutoff);
			glLightfv(Lights[x].Identifier,GL_CONSTANT_ATTENUATION,&Lights[x].Constant_Attenuation);
			glLightfv(Lights[x].Identifier,GL_LINEAR_ATTENUATION,&Lights[x].Linear_Attenuation);
			glLightfv(Lights[x].Identifier,GL_QUADRATIC_ATTENUATION,&Lights[x].Quadratic_Attenuation);
		}
		else
			glDisable(Lights[x].Identifier);
	}
}

void WORLD::AddObject()
{
	if (ObjectCapacity==ObjectNum)
	{
		WORLDOBJECT *Buffer=new WORLDOBJECT[(ObjectCapacity+10)*2];
		memset(Buffer,0,sizeof(WORLDOBJECT)*(ObjectCapacity+10)*2);
		memcpy(Buffer,ObjectList,sizeof(WORLDOBJECT)*ObjectCapacity);
		memset(ObjectList,0,sizeof(WORLDOBJECT)*ObjectCapacity);
		delete[] ObjectList;
		ObjectList=Buffer;
		ObjectCapacity=(ObjectCapacity+10)*2;
		for (int x=0; x<ObjectNum; x++) ObjectList[x].ListItem->Data=&ObjectList[x];
	}
	ObjectList[ObjectNum].EmitterData=new EMITTERDATA;
	memset(ObjectList[ObjectNum].EmitterData,0,sizeof(EMITTERDATA));
	ObjectList[ObjectNum].EmitterData->Head=true;
	ObjectList[ObjectNum].EmitterData->TailLength=5;
	ObjectList[ObjectNum].EmitterData->TailLength2=2;
	ObjectList[ObjectNum].ListItem=ItemList->AddItem("",&ObjectList[ObjectNum],World_LLeftClick,GUI_Rename,World_LRightClick);
	if (ObjectNum)
	ObjectList[ObjectNum].ID=ObjectList[ObjectNum-1].ID+1;
	else
	ObjectList[ObjectNum].ID=0;
	ObjectList[ObjectNum].ParentID=-1;
	M_Identity(ObjectList[ObjectNum].ModelView);
	M_Identity(ObjectList[ObjectNum].IT);
	M_Identity(ObjectList[ObjectNum].Inverted);
	M_Identity(ObjectList[ObjectNum].TransformBuffer);
	ObjectList[ObjectNum].Animations=new DEFAULTOBJECTSPLINES[AnimCapacity];
	for (int x=0; x<AnimCapacity; x++)
	{
		ObjectList[ObjectNum].Animations[x].Posx=new SPLINE(); ObjectList[ObjectNum].Animations[x].Posx->Interpolation=aDDict_CATMULL;
		ObjectList[ObjectNum].Animations[x].Posy=new SPLINE(); ObjectList[ObjectNum].Animations[x].Posy->Interpolation=aDDict_CATMULL;
		ObjectList[ObjectNum].Animations[x].Posz=new SPLINE(); ObjectList[ObjectNum].Animations[x].Posz->Interpolation=aDDict_CATMULL;
		ObjectList[ObjectNum].Animations[x].Rotx=new SPLINE();
		ObjectList[ObjectNum].Animations[x].Roty=new SPLINE();
		ObjectList[ObjectNum].Animations[x].Rotz=new SPLINE();
		ObjectList[ObjectNum].Animations[x].Rotw=new SPLINE();
		ObjectList[ObjectNum].Animations[x].Sclx=new SPLINE(); ObjectList[ObjectNum].Animations[x].Sclx->Interpolation=aDDict_CATMULL;
		ObjectList[ObjectNum].Animations[x].Scly=new SPLINE(); ObjectList[ObjectNum].Animations[x].Scly->Interpolation=aDDict_CATMULL;
		ObjectList[ObjectNum].Animations[x].Sclz=new SPLINE(); ObjectList[ObjectNum].Animations[x].Sclz->Interpolation=aDDict_CATMULL;
		ObjectList[ObjectNum].Animations[x].AnimTime=new SPLINE(); ObjectList[ObjectNum].Animations[x].AnimTime->Interpolation=aDDict_LINEAR;
		ObjectList[ObjectNum].Animations[x].AnimID=new SPLINE();
		ObjectList[ObjectNum].Animations[x].Red=new SPLINE();  ObjectList[ObjectNum].Animations[x].Red->Interpolation=aDDict_LINEAR;
		ObjectList[ObjectNum].Animations[x].Green=new SPLINE(); ObjectList[ObjectNum].Animations[x].Green->Interpolation=aDDict_LINEAR;
		ObjectList[ObjectNum].Animations[x].Blue=new SPLINE(); ObjectList[ObjectNum].Animations[x].Blue->Interpolation=aDDict_LINEAR;
		ObjectList[ObjectNum].Animations[x].Alpha=new SPLINE(); ObjectList[ObjectNum].Animations[x].Alpha->Interpolation=aDDict_LINEAR;
		ObjectList[ObjectNum].Animations[x].Size=new SPLINE(); ObjectList[ObjectNum].Animations[x].Size->Interpolation=aDDict_LINEAR;
		ObjectList[ObjectNum].Animations[x].Prtfrme=new SPLINE(); ObjectList[ObjectNum].Animations[x].Prtfrme->Interpolation=aDDict_LINEAR;
	}
	ObjectList[ObjectNum].AnimCapacity=AnimCapacity;
	ObjectList[ObjectNum].AnimNum=AnimNum;
	ObjectList[ObjectNum].PosData.Scale=V3_Make(1,1,1);
	ObjectList[ObjectNum].PosData.Quaternion.s=1;
	ObjectList[ObjectNum].PosDataBuffer.Quaternion.s=1;
	ObjectList[ObjectNum].PosDataBuffer.Scale=V3_Make(1,1,1);
	ObjectList[ObjectNum].Color.dw=0xffffffff;

	ObjectNum++;
}

void WORLD::AddAnimation(char *Name)
{
	if (AnimCapacity==AnimNum)
	{
		ANIMDATA *Buffer=new ANIMDATA[(AnimCapacity+10)*2];
		memset(Buffer,0,sizeof(ANIMDATA)*(AnimCapacity+10)*2);
		memcpy(Buffer,Animations,sizeof(ANIMDATA)*AnimCapacity);

		for (int y=0; y<AnimNum; y++) Buffer[y].ListItem->Data=&Buffer[y];

		memset(Animations,0,sizeof(ANIMDATA)*AnimCapacity);
		delete Animations;
		Animations=Buffer;

		for (int x=0; x<ObjectNum; x++)
		{
			DEFAULTOBJECTSPLINES *Buffer=new DEFAULTOBJECTSPLINES[(AnimCapacity+10)*2];
			memset(Buffer,0,sizeof(DEFAULTOBJECTSPLINES)*(AnimCapacity+10)*2);

			memcpy(Buffer,ObjectList[x].Animations,sizeof(DEFAULTOBJECTSPLINES)*AnimCapacity);
			memset(ObjectList[x].Animations,0,sizeof(DEFAULTOBJECTSPLINES)*AnimCapacity);
			delete ObjectList[x].Animations;
			ObjectList[x].Animations=Buffer;
			ObjectList[x].AnimCapacity=(AnimCapacity+10)*2;
		}
		
		AnimCapacity=(AnimCapacity+10)*2;

		//ErrorMessage="ANIMATION LIMIT (10) REACHED. GO BASH BOYC AROUND TO FIX THIS.";
		//objectekben is meg kell novelni az animbuffer meretet!
	}
	if (AnimNum==0)
	Animations[AnimNum].AnimID=0; else Animations[AnimNum].AnimID=Animations[AnimNum-1].AnimID+1;
	Animations[AnimNum].Lights=new LIGHTSPLINES[8];
	Animations[AnimNum].Name=new char[strlen(Name)+1];
	memset(Animations[AnimNum].Name,0,strlen(Name)+1);
	memcpy(Animations[AnimNum].Name,Name,strlen(Name));
	
	memset(Animations[AnimNum].Lights,0,sizeof(LIGHTSPLINES)*8);
	int x;
	for (x=0; x<8; x++)
	{
		Animations[AnimNum].Lights[x].ARed=new SPLINE();   Animations[AnimNum].Lights[x].ARed->Interpolation=aDDict_LINEAR;
		Animations[AnimNum].Lights[x].AGreen=new SPLINE(); Animations[AnimNum].Lights[x].AGreen->Interpolation=aDDict_LINEAR;
		Animations[AnimNum].Lights[x].ABlue=new SPLINE();  Animations[AnimNum].Lights[x].ABlue->Interpolation=aDDict_LINEAR;
		Animations[AnimNum].Lights[x].Red=new SPLINE();   Animations[AnimNum].Lights[x].Red->Interpolation=aDDict_LINEAR;
		Animations[AnimNum].Lights[x].Green=new SPLINE(); Animations[AnimNum].Lights[x].Green->Interpolation=aDDict_LINEAR;
		Animations[AnimNum].Lights[x].Blue=new SPLINE();  Animations[AnimNum].Lights[x].Blue->Interpolation=aDDict_LINEAR;
		Animations[AnimNum].Lights[x].Lit=new SPLINE();
	}
	for (x=0; x<ObjectNum; x++)
	{

		ObjectList[x].Animations[AnimNum].Posx=new SPLINE(); ObjectList[x].Animations[AnimNum].Posx->Interpolation=aDDict_CATMULL;
		ObjectList[x].Animations[AnimNum].Posy=new SPLINE(); ObjectList[x].Animations[AnimNum].Posy->Interpolation=aDDict_CATMULL;
		ObjectList[x].Animations[AnimNum].Posz=new SPLINE(); ObjectList[x].Animations[AnimNum].Posz->Interpolation=aDDict_CATMULL;
		ObjectList[x].Animations[AnimNum].Rotx=new SPLINE();
		ObjectList[x].Animations[AnimNum].Roty=new SPLINE();
		ObjectList[x].Animations[AnimNum].Rotz=new SPLINE();
		ObjectList[x].Animations[AnimNum].Rotw=new SPLINE();
		ObjectList[x].Animations[AnimNum].Sclx=new SPLINE(); ObjectList[x].Animations[AnimNum].Sclx->Interpolation=aDDict_CATMULL;
		ObjectList[x].Animations[AnimNum].Scly=new SPLINE(); ObjectList[x].Animations[AnimNum].Scly->Interpolation=aDDict_CATMULL;
		ObjectList[x].Animations[AnimNum].Sclz=new SPLINE(); ObjectList[x].Animations[AnimNum].Sclz->Interpolation=aDDict_CATMULL;
		ObjectList[x].Animations[AnimNum].AnimTime=new SPLINE(); ObjectList[x].Animations[AnimNum].AnimTime->Interpolation=aDDict_LINEAR;
		ObjectList[x].Animations[AnimNum].AnimID=new SPLINE();
		ObjectList[x].Animations[AnimNum].Red=new SPLINE();  ObjectList[x].Animations[AnimNum].Red->Interpolation=aDDict_LINEAR;
		ObjectList[x].Animations[AnimNum].Green=new SPLINE(); ObjectList[x].Animations[AnimNum].Green->Interpolation=aDDict_LINEAR;
		ObjectList[x].Animations[AnimNum].Blue=new SPLINE(); ObjectList[x].Animations[AnimNum].Blue->Interpolation=aDDict_LINEAR;
		ObjectList[x].Animations[AnimNum].Alpha=new SPLINE(); ObjectList[x].Animations[AnimNum].Alpha->Interpolation=aDDict_LINEAR;
		ObjectList[x].Animations[AnimNum].Size=new SPLINE(); ObjectList[x].Animations[AnimNum].Size->Interpolation=aDDict_LINEAR;
		ObjectList[x].Animations[AnimNum].Prtfrme=new SPLINE(); ObjectList[x].Animations[AnimNum].Prtfrme->Interpolation=aDDict_LINEAR;
		
		//ObjectList[x].Animations[AnimNum].AnimID=Animations[AnimNum].AnimID;
		ObjectList[x].AnimNum++;
	}
	AnimNum++;
}

void WORLD::AddCamera(char *Name)
{
	if (CamCapacity==CamNum)
	{
		CAMSPLINES *Buffer=new CAMSPLINES[(CamCapacity+10)*2];
		memset(Buffer,0,sizeof(CAMSPLINES)*(CamCapacity+10)*2);
		memcpy(Buffer,CamAnims,sizeof(CAMSPLINES)*CamCapacity);

		memset(CamAnims,0,sizeof(CAMSPLINES)*CamCapacity);
		delete CamAnims;
		CamAnims=Buffer;

		ITEM *c=CameraList->List;
		for (int x=0; x<CamNum; x++)
		{
			c->Data=&(CamAnims[x]);
			c=c->Next;
		}

		CamCapacity=(CamCapacity+10)*2;
	}

	if (CamNum==0)
	CamAnims[CamNum].CamID=0; else CamAnims[CamNum].CamID=CamAnims[CamNum-1].CamID+1;
	CamAnims[CamNum].Name=new char[strlen(Name)+1];
	memset(CamAnims[CamNum].Name,0,strlen(Name)+1);
	memcpy(CamAnims[CamNum].Name,Name,strlen(Name));
	CamNum++;
}

void DrawWireChildren(WORLD *w, int Parent, bool SubWorld)
{
	glPushMatrix();
	VECTOR3 v=V3_Make(1,1,1);
	for (int x=0; x<w->ObjectNum; x++)
	if (w->ObjectList[x].ParentID==Parent && !w->ObjectList[x].Deleted)
	{		
		glPushMatrix();
 		MultModelViewMatrix(w->ObjectList[x].ModelView,0);
		switch (w->ObjectList[x].Primitive)
		{
		case aDDict_SCENE:
			if (!SubWorld)
			if (!w->ObjectList[x].ListItem->Selected) glColor4f(highblue); else glColor4f(lightblue);
			((SCENE*)w->ObjectList[x].Data)->RenderWireFrame(v,true);
			break;
		case aDDict_SUBSCENE:
			if (!SubWorld)
			if (!w->ObjectList[x].ListItem->Selected) glColor4f(highblue); else glColor4f(lightblue);
			((WORLD*)w->ObjectList[x].Data)->SetToAnimPos(&w->ObjectList[x].APosData);
			((WORLD*)w->ObjectList[x].Data)->RenderWireFrame(v,true);
			break;
		case aDDict_SPHEREEMITTER:
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(0,1,0,1);
			Sphere();
			glColor4f(1,0,1,1);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3fv(w->ObjectList[x].EmitterData->Dir.a);
			glEnd();
			glPopAttrib();
			break;
		case aDDict_CUBEEMITTER:
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(0,1,0,1);
			Cube();
			glColor4f(1,0,1,1);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3fv(w->ObjectList[x].EmitterData->Dir.a);
			glEnd();
			glPopAttrib();
			break;
		case aDDict_CYLINDEREMITTER:
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(0,1,0,1);
			Cylinder();
			glColor4f(1,0,1,1);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3fv(w->ObjectList[x].EmitterData->Dir.a);
			glEnd();
			glPopAttrib();
			break;
		case aDDict_SPHEREDEFLECTOR:
			glColor4f(1,0,0,1);
			Sphere();
			break;
		case aDDict_SPHERESLOWER:
			glColor4f(1,0,1,1);
			Sphere();
			glColor4f(1,0,1,1);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3fv(w->ObjectList[x].EmitterData->Dir.a);
			glEnd();
			break;
		case aDDict_PLANEDEFLECTOR:
			glColor4f(1,0,0,1);
			Plane();
			break;
		case aDDict_GRAVITY:
			if (!MainGUI.ItemList[0].ButtonPushed)
			{
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				if (w->ObjectList[x].EmitterData->Param1>0)
				glColor4f(0.5f,0,0,1);
				else
				glColor4f(0,0,0.5f,1);
				glPointSize(15);
				glEnable(GL_BLEND);
				glEnable(GL_POINT_SMOOTH);
				glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
				glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
				glDisable(GL_LIGHTING);
				glBegin(GL_POINTS);
				glVertex3f(0,0,0);
				glEnd();
				glPopAttrib();
			}
			break;
		}
		DrawWireChildren(w,w->ObjectList[x].ID,SubWorld);
		glPopMatrix();
	}
	glPopMatrix();
}

void WORLD::RenderWireFrame(VECTOR3 v, bool SubWorld)
{
	MATRIX m;
	M_Scale(v.x,v.y,v.z,m);

	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	if (!SubWorld)
	glColor4f(highblue);
	MultModelViewMatrix(m,0);
	DrawWireChildren(this,-1,SubWorld);
	glColor4f(1,1,1,1);
	glDisable(GL_CULL_FACE);
	//glPopAttrib();
}

void DrawChildren(WORLD *w, int Parent, bool ZMask)
{
	for (int x=0; x<w->ObjectNum; x++)
	if (w->ObjectList[x].ParentID==Parent && !w->ObjectList[x].Deleted)
	{		
		glPushMatrix();
 		MultModelViewMatrix(w->ObjectList[x].ModelView,0);
		switch (w->ObjectList[x].Primitive)
		{
		case aDDict_SCENE:
			((SCENE*)w->ObjectList[x].Data)->Render(false,ZMask,w->ObjectList[x].Color);
			break;
		case aDDict_SUBSCENE:
			((WORLD*)w->ObjectList[x].Data)->SetToAnimPos(&w->ObjectList[x].APosData);
			((WORLD*)w->ObjectList[x].Data)->Render(true,ZMask);
			break;
		case aDDict_SPHEREEMITTER:
			glColor4f(0,1,0,1);
			Sphere();
			break;
		case aDDict_DUMMY:
			glColor4f(0,0,1,1);
			Sphere();
			break;
		case aDDict_CUBEEMITTER:
			glColor4f(0,1,0,1);
			Cube();
			break;
		case aDDict_CYLINDEREMITTER:
			glColor4f(0,1,0,1);
			Cylinder();
			break;
		case aDDict_SPHEREDEFLECTOR:
			glColor4f(1,0,0,1);
			Sphere();
			break;
		case aDDict_SPHERESLOWER:
			glColor4f(1,0,1,1);
			Sphere();
			break;
		case aDDict_PLANEDEFLECTOR:
			glColor4f(1,0,0,1);
			Plane();
			break;
		case aDDict_GRAVITY:
			if (!MainGUI.ItemList[0].ButtonPushed)
			{
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				if (w->ObjectList[x].EmitterData->Param1>0)
				glColor4f(0.5f,0,0,1);
				else
				glColor4f(0,0,0.5f,1);
				glPointSize(15);
				glEnable(GL_BLEND);
				glEnable(GL_POINT_SMOOTH);
				glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
				glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
				glDisable(GL_LIGHTING);
				glBegin(GL_POINTS);
				glVertex3f(0,0,0);
				glEnd();
				glPopAttrib();
			}
			break;
		}
		DrawChildren(w,w->ObjectList[x].ID,ZMask);
		glPopMatrix();
	}
}

void WORLD::Render(bool SubWorld, bool Zmask)
{
	WriteDebug("World %s render start\n",Name);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,1/256.0f);

	if (!SubWorld)
	{
		SetupLighting();
		if (Fog)
		{
			glEnable(GL_FOG);
			float Col[4];
			Col[0]=FogCol[0]/255.0f;
			Col[1]=FogCol[1]/255.0f;
			Col[2]=FogCol[2]/255.0f;
			Col[3]=1;
			glFogi(GL_FOG_MODE,GL_LINEAR);
			glFogfv(GL_FOG_COLOR,Col);
			glFogf(GL_FOG_START,FogStart);
			glFogf(GL_FOG_END,FogEnd);
		}
	}
	if (!SubWorld)
	{
		DrawChildren(this,-1,true);
		DrawChildren(this,-1,false);
	}
	else
		DrawChildren(this,-1,Zmask);
	glPopAttrib();
}

void WORLD::SetToAnimPos(ANIMPOS *a)
{
	if (a->AnimID<AnimNum && a->AnimID>=0)
	{
		float TimePos=a->AnimPos;
		if (a->AnimPos-(float)floor(a->AnimPos) != 0)
		TimePos=a->AnimPos-(float)floor(a->AnimPos);
		for (int x=0; x<ObjectNum; x++)
		{
			DEFAULTOBJECTSPLINES *s=&ObjectList[x].Animations[a->AnimID];
			if (s->Posx->KeyNum) ObjectList[x].PosData.Pos.x=s->Posx->GetInterpolatedValue(TimePos);
			if (s->Posy->KeyNum) ObjectList[x].PosData.Pos.y=s->Posy->GetInterpolatedValue(TimePos);
			if (s->Posz->KeyNum) ObjectList[x].PosData.Pos.z=s->Posz->GetInterpolatedValue(TimePos);
			if (s->Sclx->KeyNum) ObjectList[x].PosData.Scale.x=s->Sclx->GetInterpolatedValue(TimePos);
			if (s->Scly->KeyNum) ObjectList[x].PosData.Scale.y=s->Scly->GetInterpolatedValue(TimePos);
			if (s->Sclz->KeyNum) ObjectList[x].PosData.Scale.z=s->Sclz->GetInterpolatedValue(TimePos);

			if (s->Red->KeyNum) ObjectList[x].Color.r=(int)min(255,max(0,s->Red->GetInterpolatedValue(TimePos)*16));
			if (s->Green->KeyNum) ObjectList[x].Color.g=(int)min(255,max(0,s->Green->GetInterpolatedValue(TimePos)*16));
			if (s->Blue->KeyNum) ObjectList[x].Color.b=(int)min(255,max(0,s->Blue->GetInterpolatedValue(TimePos)*16));
			if (s->Alpha->KeyNum) ObjectList[x].Color.a=(int)min(255,max(0,s->Alpha->GetInterpolatedValue(TimePos)*16));

			if (s->AnimID->KeyNum) ObjectList[x].APosData.AnimID=(int)s->AnimID->GetInterpolatedValue(TimePos);
			if (s->AnimTime->KeyNum) ObjectList[x].APosData.AnimPos=s->AnimTime->GetInterpolatedValue(TimePos);

			if (s->Rotx->KeyNum)
			{
				Spline_Slerp(s->Rotx,
					         s->Roty,
							 s->Rotz,
							 s->Rotw,
							 &ObjectList[x].PosData.Quaternion.s,
							 &ObjectList[x].PosData.Quaternion.v.x,
							 &ObjectList[x].PosData.Quaternion.v.y,
							 &ObjectList[x].PosData.Quaternion.v.z,TimePos);
			}
		}
		CalculateObjectMatrices();
	}
}

struct PARTICLESORTINFO
{
	float Distance;
	int ParticleID;
};

void WORLD::RenderParticles(CAMERA *Cam, int ActualAnim, MATRIX *WorldData)
{
	Cam->Normal=V3_Sub(Cam->Target,Cam->Eye);
	Cam->d=Cam->Eye.x*Cam->Normal.x+Cam->Eye.y*Cam->Normal.y+Cam->Eye.z*Cam->Normal.z;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (Fog)
	{
		glEnable(GL_FOG);
		float Col[4];
		Col[0]=FogCol[0]/255.0f;
		Col[1]=FogCol[1]/255.0f;
		Col[2]=FogCol[2]/255.0f;
		Col[3]=1;
		glFogi(GL_FOG_MODE,GL_LINEAR);
		glFogfv(GL_FOG_COLOR,Col);
		glFogf(GL_FOG_START,FogStart);
		glFogf(GL_FOG_END,FogEnd);
	}
	glPointSize(5);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	//glDepthMask(false);

	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1,1,1,1);
	for (int x=0; x<ObjectNum; x++)
	if (!ObjectList[x].Deleted && !ObjectList[x].ListItem->Hidden)
	{
		if (ObjectList[x].Primitive==aDDict_CUBEEMITTER || 
			ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || 
			ObjectList[x].Primitive==aDDict_SPHEREEMITTER)
		{
			glDepthMask(ObjectList[x].ZMask);
			glAlphaFunc(GL_GEQUAL,ObjectList[x].AEpsilon/255.0f);
			glBlendFunc(ObjectList[x].DSTBlend,ObjectList[x].SRCBlend);
			if (ObjectList[x].EmitterData->Tail)
			{
				glPushMatrix();
				MultModelViewMatrix(*WorldData,0);
				glDisable(GL_TEXTURE_2D);
				for (int y=0; y<ObjectList[x].EmitterData->MaxParticles; y++)
					if (ObjectList[x].EmitterData->Particles[y].Active)
						{
							float col[4];
							float i=1-ObjectList[x].EmitterData->Particles[y].Age/(float)ObjectList[x].EmitterData->Particles[y].StartAge;
							
							if (!ObjectList[x].Animations[ActualAnim].Red->KeyNum)
							col[0]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[0],ObjectList[x].EmitterData->Particles[y].Color2[0],i);
							else
							col[0]=ObjectList[x].Animations[ActualAnim].Red->GetInterpolatedValue(i)/16.0f;

							if (!ObjectList[x].Animations[ActualAnim].Green->KeyNum)
							col[1]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[1],ObjectList[x].EmitterData->Particles[y].Color2[1],i);
							else
							col[1]=ObjectList[x].Animations[ActualAnim].Green->GetInterpolatedValue(i)/16.0f;
							
							if (!ObjectList[x].Animations[ActualAnim].Blue->KeyNum)
							col[2]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[2],ObjectList[x].EmitterData->Particles[y].Color2[2],i);
							else
							col[2]=ObjectList[x].Animations[ActualAnim].Blue->GetInterpolatedValue(i)/16.0f;
							
							if (!ObjectList[x].Animations[ActualAnim].Alpha->KeyNum)
							col[3]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[3],ObjectList[x].EmitterData->Particles[y].Color2[3],i);
							else
							col[3]=ObjectList[x].Animations[ActualAnim].Alpha->GetInterpolatedValue(i)/16.0f;

							glBegin(GL_LINE_STRIP);
							for (int a=0;a<ObjectList[x].EmitterData->TailLength;a++)
							{
								VertexCounter++;
								glColor4f((1-a/(float)ObjectList[x].EmitterData->TailLength)*col[0],
										  (1-a/(float)ObjectList[x].EmitterData->TailLength)*col[1],
										  (1-a/(float)ObjectList[x].EmitterData->TailLength)*col[2],
										  (1-a/(float)ObjectList[x].EmitterData->TailLength)*col[3]);
								int k=(ObjectList[x].EmitterData->Particles[y].TailCnt-a*ObjectList[x].EmitterData->TailLength2);
								k=k%((ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2);
								if (k<0) k+=(ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2;
								if (ObjectList[x].EmitterData->TailInfo)
								glVertex3f(ObjectList[x].EmitterData->Particles[y].Tail[k].x,
										   ObjectList[x].EmitterData->Particles[y].Tail[k].y,
										   ObjectList[x].EmitterData->Particles[y].Tail[k].z);
							}
							glEnd();
					}
				glPopMatrix();
			}
			if (!ObjectList[x].Textured)
			{
				glPushMatrix();
				MultModelViewMatrix(*WorldData,0);
				glDisable(GL_TEXTURE_2D);
				if (ObjectList[x].EmitterData->Head)
				{
					glBegin(GL_POINTS);
					int y;
					for (y=0; y<ObjectList[x].EmitterData->MaxParticles; y++)
						if (ObjectList[x].EmitterData->Particles[y].Active)
						{
							VertexCounter++;
							float a=1-ObjectList[x].EmitterData->Particles[y].Age/(float)ObjectList[x].EmitterData->Particles[y].StartAge;
							float col[4];
							if (!ObjectList[x].Animations[ActualAnim].Red->KeyNum)
							col[0]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[0],ObjectList[x].EmitterData->Particles[y].Color2[0],a);
							else
							col[0]=ObjectList[x].Animations[ActualAnim].Red->GetInterpolatedValue(a)/16.0f;

							if (!ObjectList[x].Animations[ActualAnim].Green->KeyNum)
							col[1]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[1],ObjectList[x].EmitterData->Particles[y].Color2[1],a);
							else
							col[1]=ObjectList[x].Animations[ActualAnim].Green->GetInterpolatedValue(a)/16.0f;
							
							if (!ObjectList[x].Animations[ActualAnim].Blue->KeyNum)
							col[2]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[2],ObjectList[x].EmitterData->Particles[y].Color2[2],a);
							else
							col[2]=ObjectList[x].Animations[ActualAnim].Blue->GetInterpolatedValue(a)/16.0f;
							
							if (!ObjectList[x].Animations[ActualAnim].Alpha->KeyNum)
							col[3]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[3],ObjectList[x].EmitterData->Particles[y].Color2[3],a);
							else
							col[3]=ObjectList[x].Animations[ActualAnim].Alpha->GetInterpolatedValue(a)/16.0f;

							float Dest=1;
							if (ObjectList[x].EmitterData->CamFade!=0)
							Dest=min(1,V3_Length(V3_Sub(ObjectList[x].EmitterData->Particles[y].DisplayPos,Cam->Eye))/ObjectList[x].EmitterData->CamFade);

							col[0]*=Dest;
							col[1]*=Dest;
							col[2]*=Dest;
							col[3]*=Dest;

							glColor4fv(col);
							glVertex3f(ObjectList[x].EmitterData->Particles[y].DisplayPos.x,
								ObjectList[x].EmitterData->Particles[y].DisplayPos.y,
								ObjectList[x].EmitterData->Particles[y].DisplayPos.z);
						}
					glEnd();
				}
				glPopMatrix();
			}
			else
			{
				if (ObjectList[x].EmitterData->Head)
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,ObjectList[x].Material->ImageData.TextureHandle);
					VECTOR3 v,v1,v2,v3,v4;
					v=V3_Make(0,0,0);
					VECTOR3 i,j;
					i=V3_Mults(Cam->i,1/*ObjectList[x].EmitterData->Size*/);
					if (!ObjectList[x].EmitterData->FixedUp)
					j=V3_Mults(Cam->j,1/*ObjectList[x].EmitterData->Size*/);
					else j=V3_Make(0,1,0);
					v1=V3_Add(V3_Add(v,i),j);
					v2=V3_Add(V3_Sub(v,i),j);
					v3=V3_Sub(V3_Sub(v,i),j);
					v4=V3_Sub(V3_Add(v,i),j);
					GLuint Particle=glGenLists(1);
					glNewList(Particle,GL_COMPILE);
					glBegin(GL_TRIANGLE_STRIP);
					glTexCoord2i(1,1);
					glVertex3f(v1.x,v1.y,v1.z);
					glTexCoord2i(0,1);
					glVertex3f(v2.x,v2.y,v2.z);
					glTexCoord2i(1,0);
					glVertex3f(v4.x,v4.y,v4.z);
					glTexCoord2i(0,0);
					glVertex3f(v3.x,v3.y,v3.z);
					glEnd();
					glEndList();

					int ActiveCount=0;
					int y;
					for (y=0; y<ObjectList[x].EmitterData->MaxParticles; y++)
						if (ObjectList[x].EmitterData->Particles[y].Active) ActiveCount++;

					VECTOR3 CamNormal=V3_Normalize(V3_Sub(Cam->Target,Cam->Eye));

					PARTICLESORTINFO *Sort;
					if (ActiveCount)
					{
						int Count=0;
						Sort=new PARTICLESORTINFO[ActiveCount];
						int y;
						for (y=0; y<ObjectList[x].EmitterData->MaxParticles; y++)
							if (ObjectList[x].EmitterData->Particles[y].Active)
							{
								Sort[Count].ParticleID=y;

								M_Xformd(*WorldData,ObjectList[x].EmitterData->Particles[y].DisplayPos,ObjectList[x].EmitterData->Particles[y].DisplayPos2);
								
								VECTOR3 PointCam=V3_Sub(ObjectList[x].EmitterData->Particles[y].DisplayPos2,Cam->Eye);
								Sort[Count].Distance=V3_Dot(CamNormal,PointCam);

								Count++;
							}

						for (int x=0; x<ActiveCount; x++)
						{
							float GreatestValue=Sort[x].Distance;
							int GreatestID=x;
							
							for (int y=x; y<ActiveCount; y++)
								if (Sort[y].Distance>GreatestValue)
								{
									GreatestValue=Sort[y].Distance;
									GreatestID=y;
								}

							PARTICLESORTINFO Buffer=Sort[x];
							Sort[x]=Sort[GreatestID];
							Sort[GreatestID]=Buffer;

						}
					}

					//for (y=0; y<ObjectList[x].EmitterData->MaxParticles; y++)
						//if (ObjectList[x].EmitterData->Particles[y].Active)
						for (y=0; y<ActiveCount; y++)
						{
							VertexCounter+=4;
							PolyCounter+=2;
							float a=1-ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Age/(float)ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].StartAge;

							float Size;
							if (!ObjectList[x].Animations[ActualAnim].Size->KeyNum) Size=ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Size;
							else
							Size=ObjectList[x].Animations[ActualAnim].Size->GetInterpolatedValue(a);

							float col[4];
							if (!ObjectList[x].Animations[ActualAnim].Red->KeyNum)
							col[0]=LinearInterpolate(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color1[0],ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color2[0],a);
							else
							col[0]=ObjectList[x].Animations[ActualAnim].Red->GetInterpolatedValue(a)/16.0f;

							if (!ObjectList[x].Animations[ActualAnim].Green->KeyNum)
							col[1]=LinearInterpolate(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color1[1],ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color2[1],a);
							else
							col[1]=ObjectList[x].Animations[ActualAnim].Green->GetInterpolatedValue(a)/16.0f;
							
							if (!ObjectList[x].Animations[ActualAnim].Blue->KeyNum)
							col[2]=LinearInterpolate(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color1[2],ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color2[2],a);
							else
							col[2]=ObjectList[x].Animations[ActualAnim].Blue->GetInterpolatedValue(a)/16.0f;
							
							if (!ObjectList[x].Animations[ActualAnim].Alpha->KeyNum)
							col[3]=LinearInterpolate(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color1[3],ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Color2[3],a);
							else
							col[3]=ObjectList[x].Animations[ActualAnim].Alpha->GetInterpolatedValue(a)/16.0f;

							float Dest=1;
							if (ObjectList[x].EmitterData->CamFade!=0)
								Dest=min(1, Sort[y].Distance/ObjectList[x].EmitterData->CamFade);

							col[0]*=Dest;
							col[1]*=Dest;
							col[2]*=Dest;
							col[3]*=Dest;
							
							glColor4fv(col);

							glPushMatrix();
							glTranslatef(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2.x,ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2.y,ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2.z);
							glScalef(Size,Size,Size);
							glRotatef(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Rotation,Cam->Normal.x,Cam->Normal.y,Cam->Normal.z);
							glCallList(Particle);
							CallCounter++;
							glPopMatrix();
						}

					if (ActiveCount) delete Sort;
					glDeleteLists(Particle,1);
				}
			}
		}
	}
	glPopAttrib();
}

void WORLD::RenderParticleTree(CAMERA *Cam, int ActualAnim, MATRIX *WorldData)
{
	RenderParticles(Cam,max(ActualAnim,0),WorldData);
	for (int x=0; x<ObjectNum; x++)
		if (ObjectList[x].Primitive==aDDict_SUBSCENE && !ObjectList[x].Deleted) 
		{

			MATRIX Parent;
			M_Identity(Parent);
			int ParentID=ObjectList[x].ID;
			WORLDOBJECT *P;
			while (ParentID!=-1)
			{
				if (FindObjectByID(ParentID))
				{
					P=FindObjectByID(ParentID);
					M_Mult(P->ModelView,Parent,Parent);
					ParentID=P->ParentID;
				}
				else
				{
					P->ParentID=ParentID=-1;
				}
			}
			
			MATRIX w;
			memcpy(&w,WorldData,sizeof(MATRIX));			

			M_Mult(Parent,w,w);
			((WORLD*)ObjectList[x].Data)->RenderParticleTree(Cam,max(ActualAnim,0),&w);
		}
}

int SPLINE::Arrange(int DraggedValue)
{
	if (Interpolation<aDDict_SINE)
	{
		float Smallest;
		int SmallestIndex;
		Key Buffer;
		int DragValue=DraggedValue;
		for (int x=0; x<KeyNum; x++)
		{
			Smallest=Keys[x].Time;
			SmallestIndex=x;
			for (int y=x+1; y<KeyNum; y++)
			{
				if (Keys[y].Time<Smallest)
				{
					Smallest=Keys[y].Time;
					SmallestIndex=y;
				}
			}
			if (x==DragValue) DragValue=SmallestIndex;
			else if (SmallestIndex==DragValue) DragValue=x;
			Buffer=Keys[x];
			Keys[x]=Keys[SmallestIndex];
			Keys[SmallestIndex]=Buffer;
		}
		return DragValue;
	}
	else return DraggedValue;
}

void SPLINE::AddKey(float Value,float Time)
{
	if (Interpolation<aDDict_SINE || KeyNum<3)
	{
		if (KeyNum==KeyCapacity)
		{
			Key *Buffer=new Key[(KeyCapacity+10)*2];
			memset(Buffer,0,sizeof(Key)*(KeyCapacity+10)*2);
			memcpy(Buffer,Keys,sizeof(Key)*KeyCapacity);
			delete[] Keys;
			Keys=Buffer;
			KeyCapacity=(KeyCapacity+10)*2;
		}

		Keys[KeyNum].Time=KillFloat(Time,2);
		Keys[KeyNum].Value=KillFloat(Value,2);
		KeyNum++;
		Arrange(0);
	}
}

float hermite(float p1,float p2,float r1,float r2,float t) {
  return p1*( 2.0f*(t*t*t)-3.0f*(t*t)+1.0f)+
         r1*(     (t*t*t)-2.0f*(t*t)+t  )+
         p2*(-2.0f*(t*t*t)+3.0f*(t*t)    )+
         r2*(     (t*t*t)-    (t*t)    );
}

float bezier(float p1,float p2,float r1,float r2,float t) {
	float t3=t*t*t;
	float t2=t*t;
	
	float cx = 3.0f * (r1 - p1);
	float bx = 3.0f * (r2 - r1) - cx;
	float ax = p2 - p1 - cx - bx;

	return ax*t3 + bx*t2 + cx*t + p1;
}

float catmull(float v0,float v1,float v2,float v3,float t) {
   //float xx = t / distance;
   float P = (v3 - v2) - (v0 - v1);
   float Q = (v0 - v1) - P;
   float R =  v2 - v0;
   float r=((P*t*t*t) + (Q*t*t) + (R*t) + v1);
   return r;
}

float catmull2(float v0,float v1,float v2,float v3,float t) {
	float P= -v0 + 3*v1 - 3*v2 + v3;
	float Q=2*v0 - 5*v1 + 4*v2 - v3;
	float R= -v0 + v2;
	float S= 2*v1;
	float r=((P*t*t*t) + (Q*t*t) + (R*t) + S)*0.5f;
	return r;
}

void  SPLINE::InitVectors()
{
	int i;
	if (KeyNum<=1) return;
	for (i=0; i<KeyNum; i++) 
	{
		Keys[i].an=GetVector(0,i);
		Keys[i].bn=GetVector(1,i);
	}
}

#define TENS 0.0f
#define LOOP 0
#define BIAS 0.0f
#define CONT 0.0f

float SPLINE::GetVector(unsigned char sel, int n) 
{

	Key   kn_1, kn, kn1;
	float pn_1, pn, pn1;

	kn = Keys[n];
	pn = kn.Value;

	if (sel == 2) return pn;

	if (n == 0) 
	{
		//first key
		kn1 = Keys[1];
		pn1 = kn1.Value;

		if (KeyNum == 2) 
		{
			//2 Keys
			return (float)((pn1 - pn)*(1.0 - TENS));
		};
		if (!LOOP) 
		{
			//first key, no loop
			return (float)(((pn1 - pn)*1.5 - GetVector(0,1)*0.5)*(1.0 - TENS));
		} 
		else 
		{
			//first key, loop
			kn_1 = Keys[KeyNum-2];
		};
	}
	
	else 
	if (n == KeyNum-1) 
	{
		//last key
		kn_1 = Keys[n-1];
		pn_1 = kn_1.Value;

		if (KeyNum == 2) 
		{
			//2 Keys
			return (float)((pn - pn_1)*(1.0 - TENS));
		};
		if (!LOOP) 
		{
			//last key, no loop
			return (float)(((pn - pn_1)*1.5 - GetVector(1,n-1)*0.5)*(1.0 - TENS));
		} 
		else 
		{
			//last key, loop
			kn1 = Keys[1];
		};
	} 
	else 
	{
		//middle Keys
		kn_1= Keys[n-1];
		kn1 = Keys[n+1];
	};
	pn_1= kn_1.Value;
	pn1 = kn1 .Value;

	float f = (sel == 0) ? 0.5f : -0.5f;

	float g1 = (pn  - pn_1)*(1.0f + BIAS);
	float g2 = (pn1 - pn  )*(1.0f - BIAS);

	return (g1 + (g2-g1)*(0.5f + f*CONT))*(1.0f - TENS);
}

float SPLINE::GetInterpolatedValue(float Time)
{
	int x=0;

	float amp=1;
	if (Interpolation == aDDict_SINE || Interpolation == aDDict_SAW || Interpolation == aDDict_SQUARE)
	{
		if (Time<Keys[0].Time) amp=LinearInterpolate(0,Keys[0].Value,Time/Keys[0].Time);
		if (Time>Keys[1].Time) amp=LinearInterpolate(Keys[1].Value,0,(Time-Keys[1].Time)/(1-Keys[1].Time));
		if (Time>=Keys[0].Time && Time<=Keys[1].Time) amp=LinearInterpolate(Keys[0].Value,Keys[1].Value,(Time-Keys[0].Time)/(Keys[1].Time-Keys[0].Time));
	}
	else
	{
		if (!KeyNum) return 0;
		if (Time<Keys[0].Time) return GetInterpolatedValue(Keys[0].Time);
		if (Time>Keys[KeyNum-1].Time) return GetInterpolatedValue(Keys[KeyNum-1].Time);
		while (Keys[x].Time<Time && x<KeyNum) x++;
		x--;
		if (x==-1) 
		{
			x=0;
			return Keys[x].Value;
		}
		if (x==KeyNum-1) return Keys[x].Value;
	}

	float t; //a ket kulcs kozotti pozicio
	t=(Time-Keys[x].Time)/(Keys[x+1].Time-Keys[x].Time);

	switch (Interpolation)
	{
	case aDDict_LINEAR:
		return LinearInterpolate((float)Keys[x].Value,(float)Keys[x+1].Value,t);
	case aDDict_HERMITE:
		return hermite(Keys[x].Value,Keys[x+1].Value,Keys[x].an  ,Keys[x+1].bn,  t );
	case aDDict_CATMULL:
		int a,b,c,d;
		a=x-1;
		if (x-1<0) a=0;
		b=x; c=x+1;
		d=x+2;
		if (x+1>=KeyNum-1) d=KeyNum-1;
		/*a=(x-1+KeyNum) % KeyNum;
		b=(x+KeyNum)   % KeyNum;
		c=(x+1+KeyNum) % KeyNum;
		d=(x+2+KeyNum) % KeyNum;*/
		//return BSplineInterpolate(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
		return catmull2(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
		break;
	case aDDict_BEZIER:
		{
			int a,b,c,d;
			a=x-1;
			if (x-1<0) a=0;
			b=x; c=x+1;
			d=x+2;
			if (x+1>=KeyNum-1) d=KeyNum-1;
			/*a=(x-1+KeyNum) % KeyNum;
			b=(x+KeyNum)   % KeyNum;
			c=(x+1+KeyNum) % KeyNum;
			d=(x+2+KeyNum) % KeyNum;*/
			return BSplineInterpolate(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
			//return catmull2(Keys[a].Value,Keys[b].Value,Keys[c].Value,Keys[d].Value,t);
		}
		break;
	/*case aDDict_BEZIER:
		return bezier(Keys[x].Value,Keys[x].Value,Keys[x+1].Value,Keys[x+1].Value,t);
		break;*/
	case aDDict_SINE:
		return amp*sin(Time/Keys[2].Time);
		break;
	case aDDict_SAW:
		float va;
		float t;
		t=Time/Keys[2].Time/3.1415f;
		t=t-(float)floor(t);

		if (t<0.5f) va=4*t-1; else va=1-4*(t-0.5f);

		return amp*va;
		break;
	case aDDict_SQUARE:
		float v=sin(Time/Keys[2].Time);
		v=v/fabs(v);
		return amp*v;
		break;
	}

	//aDDict_NONE
	return Keys[x].Value;
}

void SPLINE::ChangeInterpolation(int i)
{
	int oldi=Interpolation;
	Interpolation = i;
	if ((oldi==aDDict_NONE || oldi==aDDict_LINEAR || oldi==aDDict_CATMULL) 
	 && (i==aDDict_SINE || i==aDDict_SAW || i==aDDict_SQUARE))
	{
		KeyNum=0;
		AddKey(32,0.2f);
		AddKey(32,0.8f);
		AddKey(1,0.03f);
	}

	if ((i==aDDict_NONE || i==aDDict_LINEAR || i==aDDict_CATMULL) 
	 && (oldi==aDDict_SINE || oldi==aDDict_SAW || oldi==aDDict_SQUARE))
	{
		KeyNum=0;
	}
}

void WORLD::CalculateObjectMatrices()
{
	for (int x=0; x<ObjectNum; x++) RST2Matrix(&ObjectList[x].PosData,ObjectList[x].ModelView,0);
}


void GetVector(unsigned char sel, unsigned int n, SPLINE *s, SPLINE *x, SPLINE *y, SPLINE *z, float *a, float *b, float *c, float *d)
{
	Key      kn_1[4], kn[4], kn1[4];
	QUATERNION qn_1, qn, qn1;

	kn[0]=s->Keys[n];
	kn[1]=x->Keys[n];
	kn[2]=y->Keys[n];
	kn[3]=z->Keys[n];
	qn.s=s->Keys[n].Value;
	qn.v.x=x->Keys[n].Value;
	qn.v.y=y->Keys[n].Value;
	qn.v.z=z->Keys[n].Value;

	//  if (sel == 2) return qn;

	if (!n) 
	{

		kn1[0]=s->Keys[1];
		kn1[1]=x->Keys[1];
		kn1[2]=y->Keys[1];
		kn1[3]=z->Keys[1];
		qn1.s=s->Keys[1].Value;
		qn1.v.x=x->Keys[1].Value;
		qn1.v.y=y->Keys[1].Value;
		qn1.v.z=z->Keys[1].Value;

		QUATERNION q=Q_Slerp(qn,qn1,1/3.0f);
		*a=q.s;
		*b=q.v.x;
		*c=q.v.y;
		*d=q.v.z;
		return;
		
	} 
	else if ((signed)n == s->KeyNum-1) 
	{
		kn_1[0]=s->Keys[n-1];
		kn_1[1]=x->Keys[n-1];
		kn_1[2]=y->Keys[n-1];
		kn_1[3]=z->Keys[n-1];
		qn_1.s=s->Keys[n-1].Value;
		qn_1.v.x=x->Keys[n-1].Value;
		qn_1.v.y=y->Keys[n-1].Value;
		qn_1.v.z=z->Keys[n-1].Value;

		QUATERNION q=Q_Slerp(qn,qn_1,1/3.0f);
		*a=q.s;
		*b=q.v.x;
		*c=q.v.y;
		*d=q.v.z;
		return;
	}
	else {
		//middle Keys
		kn_1[0]=s->Keys[n-1];
		kn_1[1]=x->Keys[n-1];
		kn_1[2]=y->Keys[n-1];
		kn_1[3]=z->Keys[n-1];
		kn1[0] =s->Keys[n+1];
		kn1[1] =x->Keys[n+1];
		kn1[2] =y->Keys[n+1];
		kn1[3] =z->Keys[n+1];
	};
	qn_1.s= kn_1[0].Value;
	qn_1.v.x= kn_1[1].Value;
	qn_1.v.y= kn_1[2].Value;
	qn_1.v.z= kn_1[3].Value;
	qn1.s = kn1[0] .Value;
	qn1.v.x= kn1[1] .Value;
	qn1.v.y= kn1[2] .Value;
	qn1.v.z= kn1[3] .Value;

	float f = !sel ? 1.0f : -1.0f;

	QUATERNION g1 = Q_Slerp(qn,qn_1,-1/3.0f);
	QUATERNION g2 = Q_Slerp(qn,qn1,  1/3.0f);
	//  udQuatKeyGetQuat:=udQuatSlerp(qn, udQuatSlerp(g1,g2,0.5+f*0.5*kn.c) ,f*(kn.t-1));

	//  return (g1 + (g2-g1)*(0.5 + f*CONT))*(1.0 - TENS);
	QUATERNION q=Q_Slerp(qn,Q_Slerp(g1,g2,0.5), f*(-1));
	*a=q.s;
	*b=q.v.x;
	*c=q.v.y;
	*d=q.v.z;
}

QUATERNION quat_interp(Key k1[4], Key k2[4], float t) 
{
	QUATERNION k1v,k1b,k2a,k2v;
	k1v.s=k1[0].Value;
	k1v.v.x=k1[1].Value;
	k1v.v.y=k1[2].Value;
	k1v.v.z=k1[3].Value;
	k1b.s=k1[0].bn;
	k1b.v.x=k1[1].bn;
	k1b.v.y=k1[2].bn;
	k1b.v.z=k1[3].bn;

	k2v.s=k2[0].Value;
	k2v.v.x=k2[1].Value;
	k2v.v.y=k2[2].Value;
	k2v.v.z=k2[3].Value;
	k2a.s=k2[0].an;
	k2a.v.x=k2[1].an;
	k2a.v.y=k2[2].an;
	k2a.v.z=k2[3].an;

	QUATERNION q0=Q_Slerp(k1v,k1b,t);
	QUATERNION q1=Q_Slerp(k1b,k2a,t);
	QUATERNION q2=Q_Slerp(k2a,k2v,t);
	
	q0=Q_Slerp(q0,q1,t);
	q1=Q_Slerp(q1,q2,t);
	
	return Q_Slerp(q0,q1,t);
}

void Spline_QuaternionGetVectors(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z)
{
	if (s->KeyNum==1) return;
	for (int i=0; i<s->KeyNum; i++) 
	{
		GetVector(0,i,s,x,y,z,&s->Keys[i].an,&x->Keys[i].an,&y->Keys[i].an,&z->Keys[i].an);
		GetVector(1,i,s,x,y,z,&s->Keys[i].bn,&x->Keys[i].bn,&y->Keys[i].bn,&z->Keys[i].bn);
	}
	
}

void Spline_Slerp(SPLINE *s,SPLINE *x,SPLINE *y,SPLINE *z, float *a, float *b, float *c, float *d, float Time)
{
	int i=0;
	while (s->Keys[i].Time<Time && i<s->KeyNum) i++;
	i--;
	if (i==-1) 
	{
		i=0;
		//return Keys[i].Value;
		*a=s->Keys[i].Value;
		*b=x->Keys[i].Value;
		*c=y->Keys[i].Value;
		*d=z->Keys[i].Value;
		return;
	}
	if (i==s->KeyNum-1) 
	{
		*a=s->Keys[i].Value;
		*b=x->Keys[i].Value;
		*c=y->Keys[i].Value;
		*d=z->Keys[i].Value;
		return;
	}

	float t; //a ket kulcs kozotti pozicio
	t=(Time-s->Keys[i].Time)/(s->Keys[i+1].Time-s->Keys[i].Time);
	Key q1[4],q2[4];
	q1[0]=s->Keys[i];
	q1[1]=x->Keys[i];
	q1[2]=y->Keys[i];
	q1[3]=z->Keys[i];
	q2[0]=s->Keys[i+1];
	q2[1]=x->Keys[i+1];
	q2[2]=y->Keys[i+1];
	q2[3]=z->Keys[i+1];
	QUATERNION q3;
	q3=quat_interp(q1,q2,t);
	*a=q3.s;
	*b=q3.v.x;
	*c=q3.v.y;
	*d=q3.v.z;
}

float vertBlendingWeights[257][256];

void initVertexBlend()
{
  vertBlendingWeights[3][0] = 5.0f / 12.0f;
  vertBlendingWeights[3][1] = -1.0f / 12.0f;
  vertBlendingWeights[3][2] = -1.0f / 12.0f;

  vertBlendingWeights[4][0] = 3.0f / 8.0f;
  vertBlendingWeights[4][1] = 0.0f;
  vertBlendingWeights[4][2] = -1.0f / 8.0f;
  vertBlendingWeights[4][3] = 0.0f;

  for(int K=5; K<=256; K++)
  {
    for (int j=0; j<K; j++)
    {
      vertBlendingWeights[K][j] = (0.25f + cos(2*3.1415f*j/(float)K) + 0.5f*cos(4*3.1415f*j/(float)K)) / (float)K;
    }
  }

  float w = 0;
  vertBlendingWeights[6][0] = (1.0f / 2.0f) - w;
  vertBlendingWeights[6][1] = (1.0f / 16.0f) + w;
  vertBlendingWeights[6][2] = (-1.0f / 16.0f) - w;
  vertBlendingWeights[6][3] = w;
  vertBlendingWeights[6][4] = (-1.0f / 16.0f) - w;
  vertBlendingWeights[6][5] = (1.0f / 16.0f) + w;
}

int GetEdgeVertex(OBJECT *o, int BadVertex, int EdgeID)
{
	if (o->EdgeList[EdgeID].v1==BadVertex) return o->EdgeList[EdgeID].v2; else return o->EdgeList[EdgeID].v1;
	return -1;
}


void OBJECT::SortVertexEdges()
{
	for (int x=0; x<VertexNum; x++)
	{
		if (VertexList[x].EdgeNum>2)
		{
			VECTOR3 vertvector;
			VECTOR3 normal = V3_Normalize(VertexList[x].Normal);
			VECTOR3 vx = VertexList[x].Position;
			
			float *szogek = new float[VertexList[x].EdgeNum];

			int e=GetEdgeVertex(this,x,VertexList[x].EdgeList[0]);
			VECTOR3 refvector=VertexList[e].Position;
			refvector = V3_Sub(refvector,vx);

			VECTOR3 projvec=normal;
			projvec=V3_Mults(projvec,V3_Dot(refvector,normal));
			refvector = V3_Normalize(V3_Sub(refvector,projvec));

			int y;
			for (y=1;y<VertexList[x].EdgeNum;y++)
			{
				e=GetEdgeVertex(this,x,VertexList[x].EdgeList[y]);
				vertvector = V3_Sub(VertexList[e].Position,vx);
				projvec=V3_Mults(normal,V3_Dot(vertvector,normal));
				vertvector = V3_Normalize(V3_Sub(vertvector,projvec));

				float cosTheta = max(-1.0f,min(1.0f,V3_Dot(vertvector,refvector)));
				float angle = (float)acos(cosTheta) * 180.0f / 3.1415f;

				VECTOR3 crossProd=V3_Cross(vertvector,refvector);
				if (V3_Dot(crossProd,normal) < 0) angle = 360 - angle;

				szogek[y]=angle;

			}

			int smallest,b1; 
			float smallestvalue, b2;

			for (int z=0; z<VertexList[x].EdgeNum; z++)
			{
				smallest=z;
				smallestvalue=szogek[z];
				
				for (y=z;y<VertexList[x].EdgeNum;y++)
				{
					if (szogek[y]<=smallestvalue)
					{
						smallest=y;
						smallestvalue=szogek[y];
					}
				}
				
				b2=szogek[smallest];
				szogek[smallest]=szogek[z];
				szogek[z]=b2;

				b1=VertexList[x].EdgeList[smallest];
				VertexList[x].EdgeList[smallest]=VertexList[x].EdgeList[z];
				VertexList[x].EdgeList[z]=b1;
			}

			delete[] szogek;
		}
	}
}

void getContributionToVert(OBJECT *o,int vertNum, int newVertEdge, VECTOR3 *out)
{
  VERTEX vEdge = o->VertexList[vertNum];

  for (int x=0; x<vEdge.EdgeNum; x++)
  {
    int edgeNum = (x+newVertEdge+vEdge.EdgeNum) % vEdge.EdgeNum;
    int otherVert = GetEdgeVertex(o,vertNum,vEdge.EdgeList[edgeNum]);
	*out=V3_Add(*out,V3_Mults(o->VertexList[otherVert].Position,vertBlendingWeights[vEdge.EdgeNum][x]));
  }

  if (vEdge.EdgeNum != 6) *out=V3_Add(*out,V3_Mults(vEdge.Position,0.75f));
}

int FindEdge(OBJECT *o, int v, int e)
{
	for (int x=0; x<o->VertexList[v].EdgeNum; x++) if (o->VertexList[v].EdgeList[x]==e) return x;
	return -1;
}

VECTOR3 ButterflyTessellation(OBJECT *o, int e)
{
    int v0EdgeNum = FindEdge(o,o->EdgeList[e].v1,e);
    int v1EdgeNum = FindEdge(o,o->EdgeList[e].v2,e);

	VECTOR3 Buffer=V3_Make(0,0,0);

	int vert0 = o->EdgeList[e].v1;
	int vert1 = o->EdgeList[e].v2;
	int valence0 = o->VertexList[vert0].EdgeNum;
	int valence1 = o->VertexList[vert1].EdgeNum;

	if (valence0 == 6 && valence1 == 6)
	{
	  getContributionToVert(o, vert0, v0EdgeNum, &Buffer);
	  getContributionToVert(o, vert1, v1EdgeNum, &Buffer);
	}
	else if (valence0 == 6) getContributionToVert(o, vert1, v1EdgeNum, &Buffer);
	else if (valence1 == 6) getContributionToVert(o, vert0, v0EdgeNum, &Buffer);
	else
	{
	  getContributionToVert(o, vert0, v0EdgeNum, &Buffer);
	  getContributionToVert(o, vert1, v1EdgeNum, &Buffer);
	  Buffer=V3_Mults(Buffer,0.5f);
	}

	return Buffer;
}

void OBJECT::TessellateEdges(int *EdgeVertMap, bool Linear)
{
	for (int x=EdgeNum-1; x>=0; x--)
	{
		VECTOR3 NewVert;
		if (!Linear) NewVert=ButterflyTessellation(this,x);
		else NewVert=V3_Mults(V3_Add(VertexList[EdgeList[x].v1].Position,VertexList[EdgeList[x].v2].Position),0.5f);

		VECTOR2 t;
		t.x=(VertexList[EdgeList[x].v1].TextureCoordinate.x+VertexList[EdgeList[x].v1].TextureCoordinate.x)/2.0f;
		t.y=(VertexList[EdgeList[x].v1].TextureCoordinate.y+VertexList[EdgeList[x].v1].TextureCoordinate.y)/2.0f;

		AddVertex(NewVert.x,NewVert.y,NewVert.z,t.x,t.y);
		EdgeVertMap[x]=VertexNum-1;
	}
}

VECTOR2 GetInterpolatedTexCoord(OBJECT *o, int p, int a, int b)
{
	VECTOR2 v;
	v.x=(o->PolygonList[p].t[a].x+o->PolygonList[p].t[b].x)/2.0f;
	v.y=(o->PolygonList[p].t[a].y+o->PolygonList[p].t[b].y)/2.0f;
	return v;
}

void OBJECT::BuildNewFaces(int *EdgeVertMap, bool Linear)
{
	EdgeNum=0;
	int x;
	for (x=0; x<VertexNum; x++) VertexList[x].EdgeNum=0;

	for (x=PolygonNum-1; x>=0; x--)
	{
		int Shading=aDDict_GOURAUDSHADE;
		if (Linear) Shading=PolygonList[x].CurrentShading;
		int a,b,c,d,e,f;

		a=PolygonList[x].v1;
		b=PolygonList[x].v2;
		c=PolygonList[x].v3;
		d=EdgeVertMap[PolygonList[x].e[0]];
		e=EdgeVertMap[PolygonList[x].e[1]];
		f=EdgeVertMap[PolygonList[x].e[2]];

		VECTOR2 at,bt,ct,dt,et,ft;

		at=PolygonList[x].t[0];
		bt=PolygonList[x].t[1];
		ct=PolygonList[x].t[2];
		dt=GetInterpolatedTexCoord(this,x,0,1);
		ft=GetInterpolatedTexCoord(this,x,1,2);
		et=GetInterpolatedTexCoord(this,x,2,0);

		AddPolygon(a,d,e,Shading,at.x,at.y,dt.x,dt.y,et.x,et.y,1,1,1);
		AddPolygon(b,f,d,Shading,bt.x,bt.y,ft.x,ft.y,dt.x,dt.y,1,1,1);
		AddPolygon(c,e,f,Shading,ct.x,ct.y,et.x,et.y,ft.x,ft.y,1,1,1);
		PolygonList[x].CurrentShading=PolygonList[x].Shading=Shading;
		PolygonList[x].v1=d;
		PolygonList[x].v2=f;
		PolygonList[x].v3=e;
		PolygonList[x].t[0]=dt;
		PolygonList[x].t[1]=ft;
		PolygonList[x].t[2]=et;
		PolygonList[x].e[0]=AddEdge(d,f,x,1);
		PolygonList[x].e[1]=AddEdge(d,e,x,1);
		PolygonList[x].e[2]=AddEdge(f,e,x,1);
	}
}


void OBJECT::MeshSmooth(bool Linear)
{
	int *EdgeVertMap = new int[EdgeNum];
	for (int x=0; x<EdgeNum; x++) EdgeVertMap[x]=-1;

	SortVertexEdges();

	TessellateEdges(EdgeVertMap, Linear);
	BuildNewFaces(EdgeVertMap, Linear);

	delete EdgeVertMap;

	CalculateTextureCoordinates();
	CalculateNormals();
	CreateDisplayList();
}

void OBJECT::MeshBlur()
{
	VECTOR3 *NewPosList = new VECTOR3[VertexNum];
	int x;
	for (x=0; x<VertexNum; x++) NewPosList[x]=VertexList[x].Position;
	for (x=0; x<VertexNum; x++)
	{
		VECTOR3 Buffer;

		Buffer=V3_Mults(VertexList[x].Position,2);
		for (int y=0;y<VertexList[x].EdgeNum;y++) Buffer=V3_Add(Buffer,VertexList[GetEdgeVertex(this, x, VertexList[x].EdgeList[y])].Position);

		NewPosList[x]=V3_Mults(Buffer,1.0f/(float)(VertexList[x].EdgeNum+2));
	}
	for (x=0; x<VertexNum; x++) 
		VertexList[x].Position=VertexList[x].Position=NewPosList[x];

	delete NewPosList;
	CalculateNormals();
	CreateDisplayList();
}

/*void meshblur(object *obj)
{
	generateedges(obj);
	vertex *newvlist = new vertex [obj->vertexnum];
	memcpy(newvlist,obj->vertexlist,sizeof(vertex)*obj->vertexnum);
	for (int x=0;x<obj->vertexnum;x++)
	{
		vector3 buffer;
		float dw=2;
		buffer=vect_skalarszorzas(obj->vertexlist[x].d,2);
		for (int y=0;y<newvlist[x].vedgenum;y++)
		{
			float w;
			int vx=getedgevertex(obj, x, newvlist[x].vedgelist[y]);
			w=1;
			dw+=w;
			buffer=vect_skalarszorzas(vect_osszeadas(buffer,obj->vertexlist[vx].d),w);
		}

		buffer=vect_skalarszorzas(buffer,1.0f/dw);
		newvlist[x].d=buffer;
		newvlist[x].base=buffer;
	}
	for (x=0; x<obj->polynum; x++)
	{
		obj->polygonlist[x].shading=dd3d_gouraudshade;
	}
	memcpy(obj->vertexlist,newvlist,sizeof(vertex)*obj->vertexnum);
	generatenormals(obj);
	obj_transform(obj,obj->xformmatrix);
}*/

void RENDERLISTARRAY::AddObject(OBJECT *o, WORLDOBJECT *w)
{
	List[Pos].Object=o;
	List[Pos].WorldObject=w;
	Pos++;
}

int CountCloneItems(SCENE *s, OBJECT *o)
{
	int Count=1;

	for (int x=0; x<o->Param1; x++)
	{
		OBJECT *o2=((OBJECT*)((int*)o->Param3)[x]);
		if (o2->Primitive!=aDDict_CLONE) Count++;
		else Count+=CountCloneItems(s,o2);
	}

	return Count;
}

int CountSceneObjects(SCENE *s)
{
	int Count=0;
	for (int x=0; x<s->ObjectNum; x++)
	{
		if (s->ObjectList[x].Primitive<aDDict_MAPXFORM)
		{
			if (s->ObjectList[x].Primitive==aDDict_CLONE) Count+=CountCloneItems(s,&(s->ObjectList[x]));
			else Count++;
		}

	}
	return Count;
}

void FillCloneScene(SCENE *s, OBJECT *o, RENDERLISTARRAY *RLA, WORLDOBJECT *w, OBJECT *Parent)
{
	RLA->AddObject(o,w);
	RLA->List[RLA->Pos-1].Clone=true;
	RLA->List[RLA->Pos-1].CloneParent=Parent;

	for (int x=0; x<o->Param1; x++)
	{
		OBJECT *o2=((OBJECT*)((int*)o->Param3)[x]);
		if (o2->Primitive!=aDDict_CLONE) 
		{
			RLA->AddObject(o2,w);
			RLA->List[RLA->Pos-1].CloneParent=o;
		}
		else FillCloneScene(s,o2,RLA,w,o);
	}

}

void FillSceneRLA(SCENE *s, RENDERLISTARRAY *RLA, WORLDOBJECT *w)
{
	for (int x=0; x<s->ObjectNum; x++)
	{
		if (s->ObjectList[x].Primitive<aDDict_MAPXFORM)
		{
			if (s->ObjectList[x].Primitive==aDDict_CLONE) FillCloneScene(s,&(s->ObjectList[x]),RLA,w,NULL);
			RLA->AddObject(&(s->ObjectList[x]),w);
		}

	}
}

void SCENE::CreateRLA()
{
	if (RLA.ItemNum && RLA.List) delete RLA.List;
	RLA.ItemNum=CountSceneObjects(this);

	RLA.List=new RENDERLISTARRAYOBJECT[RLA.ItemNum];
	memset(RLA.List,0,sizeof(RENDERLISTARRAYOBJECT)*RLA.ItemNum);

}

int rayIntersectsTriangle(VECTOR3 p, VECTOR3 d, 
			VECTOR3 v0, VECTOR3 v1, VECTOR3 v2, float &t) 
{

	VECTOR3 e1,e2,h,s,q;
	float a,f,u,v;
	
	e1=V3_Sub(v1,v0);
	e2=V3_Sub(v2,v0);
	h=V3_Cross(d,e2);
	a = V3_Dot(e1,h);
	
	if (a > -0.00001 && a < 0.00001) return(false);
	
	f = 1/a;
	s=V3_Sub(p,v0);
	u = f * (V3_Dot(s,h));
	
	if (u < 0.0 || u > 1.0)	return(false);
	
	q=V3_Cross(s,e1);
	v = f * V3_Dot(d,q);
	if (v < 0.0 || u + v > 1.0)	return(false);

    t = V3_Dot(e2, q) * f;

	return(true);
}

bool RayBox(VECTOR3 P, VECTOR3 Dir, VECTOR3 BoxCenter, VECTOR3 BoxDimensions)
{
	VECTOR3 tmin,tmax;

	for (int x=0; x<3; x++)
	{
		float t1=(BoxCenter.a[x]-BoxDimensions.a[x]-P.a[x])/Dir.a[x];
		float t2=(BoxCenter.a[x]+BoxDimensions.a[x]-P.a[x])/Dir.a[x];

		tmin.a[x]=min(t1,t2);
		tmax.a[x]=max(t1,t2);
	}

	float rayMin=max(max(tmin.x,tmin.y),tmin.z);
	float rayMax=min(min(tmax.x,tmax.y),tmax.z);

	return rayMin < rayMax;	
}

VECTOR3 GetTriangleMixValues(VECTOR3 a,VECTOR3 b,VECTOR3 c,VECTOR3 p)
{
	VECTOR3 pma=V3_Sub(p,a);
	VECTOR3 bma=V3_Sub(b,a);
	VECTOR3 cma=V3_Sub(c,a);

	/*VECTOR3 r;
	r.y=(pma.x/bma.x-pma.y/bma.y)/(cma.x/bma.x-cma.y/bma.y);
	r.x=(pma.x-r.y*bma.x)/cma.x;
	r.z=1-r.x-r.y;	

	r.x=min(1,max(0,r.x));
	r.y=min(1,max(0,r.y));
	r.z=min(1,max(0,r.z));

	return r;*/

	VECTOR3 N = V3_Cross (bma,cma);
	float s=V3_Length(N);

	    
	VECTOR3 r;

	r.x = V3_Dot(V3_Cross(pma,cma),N)/s;

	r.y = V3_Dot(V3_Cross(bma,pma),N)/s;
	
	r.z = 1 - (r.x + r.y);

	return r;

}

bool RaySphereIntersect(const VECTOR3 &p, const VECTOR3 &d, float r)
{
	float det,b;
	b = -V3_Dot(p,d);
	det = b*b - V3_Dot(p,p) + r*r;
	if (det<0) return false;
	det= sqrt(det);
	float i1= b - det;
	float i2= b + det;
	// intersecting with ray?
	if(i2<0) return false;
	if(i1<0)i1=0;
	return true;
};

void SCENE::BuildRadiosityVisibility()
{
	VisibilityInfoBuilt=true;
	int VertexCount=0;
	int x;
	for (x=0; x<ObjectNum; x++)
	{
		if (!ObjectList[x].Deleted) VertexCount+=ObjectList[x].VertexNum;
		for (int y=0; y<ObjectList[x].VertexNum; y++)
		{
			for (int z=0; z<3; z++)
			ObjectList[x].VertexList[y].Color[z]=ObjectList[x].VertexList[y].NewColor[z]=0;
			M_Xformd(ObjectList[x].ModelView,ObjectList[x].VertexList[y].Position,ObjectList[x].VertexList[y].MapTransformedPosition);
			M_Xform3(ObjectList[x].ModelView,ObjectList[x].VertexList[y].CurrentNormal,ObjectList[x].VertexList[y].Normal);
			ObjectList[x].VertexList[y].Normal=V3_Normalize(ObjectList[x].VertexList[y].Normal);

		}
		memcpy(ObjectList[x].Inverted,ObjectList[x].ModelView,16*4);
		M_Invert(ObjectList[x].Inverted);
		for (int a=0; a<4; a++)
			for (int b=0; b<4; b++)
				ObjectList[x].IT[a][b]=ObjectList[x].Inverted[b][a];
	}
	VisibilityInfo=new VISIBILITY[(VertexCount*VertexCount)];
	memset(VisibilityInfo,0,sizeof(VISIBILITY)*(VertexCount*VertexCount));
	VertexPairs=0;

	for (x=0; x<ObjectNum; x++)	if (!ObjectList[x].Deleted)
		for (int o=x; o<ObjectNum; o++)	if (!ObjectList[o].Deleted)
			if (o!=x)
			for (int i=0; i<ObjectList[x].VertexNum;i++)
			{
				for (int j=0; j<ObjectList[o].VertexNum; j++) if (x!=o)
				{   //object X vertex I and object O vertex J
					
					VECTOR3 p=ObjectList[x].VertexList[i].MapTransformedPosition;
					VECTOR3 q=ObjectList[o].VertexList[j].MapTransformedPosition;

					VECTOR3 pn=ObjectList[x].VertexList[i].Normal;
					//VECTOR3 qn=ObjectList[x].VertexList[i].Normal;

					if (V3_Dot(pn,V3_Sub(q,p))>=0) //q vertex p elott van
					{
						bool hit=false;

						VECTOR3 d=V3_Normalize(V3_Sub(q,p));
						float t=0;

						if (d.x!=0)
						t=(q.x-p.x)/d.x;
						else if (d.y!=0) t=(q.y-p.y)/d.y;
						else t=(q.z-p.z)/d.z;

						//p-d ray intersection check
						for (int a=0; a<ObjectNum; a++)
						if (!ObjectList[a].Deleted && a!=x && a!=o)
						{
							//boundingbox, kd tree check

							//tri-ray intersection check

							VECTOR3 ObjSpaceP,ObjSpaceD;

							M_Xformd(ObjectList[a].Inverted,p,ObjSpaceP);
							M_Xformd(ObjectList[a].Inverted,q,ObjSpaceD);
							ObjSpaceD=V3_Normalize(V3_Sub(ObjSpaceD,ObjSpaceP));

							switch (ObjectList[a].Primitive)
							{
							case aDDict_BOX:
								if (RayBox(ObjSpaceP,ObjSpaceD,V3_Make(0,0,0),V3_Make(0.5,0.5,0.5))) hit=true;
								break;
							case aDDict_SPHERE:
								if (RaySphereIntersect(ObjSpaceP,ObjSpaceD,0.5f)) hit=true;
								break;
							case aDDict_PLANE:
								if (RayBox(ObjSpaceP,ObjSpaceD,V3_Make(0,0,0),V3_Make(0.5,0.5,0))) hit=true;
								break;
							default:
								for (int z=0; z<ObjectList[a].PolygonNum;z++)
								{
									float k=0;
									if (rayIntersectsTriangle(p,d,ObjectList[a].VertexList[ObjectList[a].PolygonList[z].v1].MapTransformedPosition,
																  ObjectList[a].VertexList[ObjectList[a].PolygonList[z].v2].MapTransformedPosition,
																  ObjectList[a].VertexList[ObjectList[a].PolygonList[z].v3].MapTransformedPosition,k))
									if (k<t-0.0001 && k>0.0001) 
									{
										hit=true;
										break;
									}
								}
							}

							if (hit) break;
						}



						if (!hit)
						{
							//visibility confirmed
							VisibilityInfo[VertexPairs].va=i;
							VisibilityInfo[VertexPairs].vb=j;
							VisibilityInfo[VertexPairs].oa=x;
							VisibilityInfo[VertexPairs].ob=o;
							VertexPairs++;
						}
						else
						{
							//int a=5;
						}
					}					
				}
			}

}

//#pragma comment(lib,"glew32.lib")
//#include "glew.h"

GLuint fbo;
GLuint fbtexture;

void InitRadiosityStuff()
{
	/*GLenum err = glewInit();
	if (GLEW_OK != err) return;
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status!=GL_FRAMEBUFFER_COMPLETE_EXT) return;
	glGenFramebuffersEXT(1,&fbo);
	glGenTextures(1,&fbtexture);
	glBindTexture(GL_TEXTURE_2D,fbtexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,32,32,0,GL_RGB,GL_INT,0);*/
	
}

void SCENE::DoRadiosityPass(bool FirstPass, int RayCount)
{
	/*if (!VisibilityInfoBuilt) BuildRadiosityVisibility();
	for (int x=0; x<ObjectNum; x++) for (int y=0; y<ObjectList[x].VertexNum; y++) for (int z=0; z<3; z++) ObjectList[x].VertexList[y].NewColor[z]=0;

	for (x=0; x<VertexPairs; x++)
	if (!(ObjectList[VisibilityInfo[x].oa].Wireframe && ObjectList[VisibilityInfo[x].ob].Wireframe))
	{
		VERTEX *v1=&ObjectList[VisibilityInfo[x].oa].VertexList[VisibilityInfo[x].va];
		VERTEX *v2=&ObjectList[VisibilityInfo[x].ob].VertexList[VisibilityInfo[x].vb];

		VECTOR3 d1,d2;
		d1=V3_Normalize(V3_Sub(v2->MapTransformedPosition,v1->MapTransformedPosition));
		d2=V3_Mults(d1,-1);
		float m1,m2;
		m1=max(0,min(1,V3_Dot(d1,v1->Normal)));
		m1*=m1*m1*0.5f;
		m2=max(0,min(1,V3_Dot(d2,v2->Normal)));
		m2*=m2*m2*0.5f;


		if (!ObjectList[VisibilityInfo[x].oa].Wireframe)
		if (!ObjectList[VisibilityInfo[x].ob].Wireframe)
		{
			for (int z=0; z<3; z++)
			v1->NewColor[z]+=v2->Color[z]*ObjectList[VisibilityInfo[x].oa].Color.c[z]/255.0f*m1;
		}
		else
		{
			for (int z=0; z<3; z++)
				v1->NewColor[z]+=ObjectList[VisibilityInfo[x].ob].Color.c[z]/255.0f*ObjectList[VisibilityInfo[x].oa].Color.c[z]/255.0f*m1;

		}

		if (!ObjectList[VisibilityInfo[x].ob].Wireframe)
		if (!ObjectList[VisibilityInfo[x].oa].Wireframe)
		{
			for (int z=0; z<3; z++)
			v2->NewColor[z]+=v1->Color[z]*ObjectList[VisibilityInfo[x].ob].Color.c[z]/255.0f*m2;
		}
		else
		{
			for (int z=0; z<3; z++)
				v2->NewColor[z]+=ObjectList[VisibilityInfo[x].ob].Color.c[z]/255.0f*ObjectList[VisibilityInfo[x].oa].Color.c[z]/255.0f*m2;
		}

	}

	for (x=0; x<ObjectNum; x++)
	{
		for (int y=0; y<ObjectList[x].VertexNum; y++)
			for (int z=0; z<3; z++)
			ObjectList[x].VertexList[y].Color[z]=min(1,ObjectList[x].VertexList[y].NewColor[z]);
		ObjectList[x].PerVertexColor=true;
		ObjectList[x].CreateDisplayList();
	}*/

	unsigned char *texbuf;
	texbuf=new unsigned char[32*32*4];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, fbtexture, 0);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,32,32);
	gluPerspective(90,1,0.1,1000);
	RGBA col;
	col.dw=0xffffffff;
	glDisable(GL_LIGHTING);

	int x;
	for (x=0; x<ObjectNum; x++) 
	{
		ObjectList[x].CullBuff=ObjectList[x].Backface;
	}

	FILE *f=fopen("vxdump.txt","w+t");
	FILE *f2=fopen("vxdump.raw","w+b");
	for (x=0; x<ObjectNum; x++)
	{
		ObjectList[x].Backface=false;
		MATRIX m;
		memcpy(m,ObjectList[x].ModelView,16*4);
		M_Invert(m);
		M_Transpose(m);
		for (int y=0; y<ObjectList[x].VertexNum; y++)
		{
			fprintf(f,"O%d V%d ",x,y);
			VECTOR3 e=ObjectList[x].VertexList[y].Position;
			M_Xformd(ObjectList[x].ModelView,e,e);
			VECTOR3 t[5];
			t[0]=V3_Normalize(V3_Mults(ObjectList[x].VertexList[y].Normal,1));
			M_Xformd(m,t[0],t[0]);
			VECTOR3 u=V3_Normalize(V3_Make(rand(),rand(),rand()));
			t[1]=V3_Cross(u,t[0]); t[2]=V3_Mults(t[2],-1);
			t[3]=V3_Cross(t[1],t[0]); t[4]=V3_Mults(t[3],-1);

			//fprintf(f,"(%.2f %.2f %.2f)(%.2f %.2f %.2f)(%.2f %.2f %.2f) | ",e.x,e.y,e.z,t.x,t.y,t.z,u.x,u.y,u.z);

			double r=0,g=0,b=0;
			
			glMatrixMode(GL_MODELVIEW);

			for (int z=0; z<5; z++)
			{
				glLoadIdentity();
				gluLookAt(e.x,e.y,e.z,e.x+t[z].x,e.y+t[z].y,e.z+t[z].z,u.x,u.y,u.z);
				glClear(0x4100);
				Render(false,false,col);
				Render(false,true,col);
				glReadPixels(0,0,32,32,GL_RGBA,GL_UNSIGNED_BYTE,texbuf);
				for (int j=0; j<32*32*4;)
				{
					r+=texbuf[j++];
					g+=texbuf[j++];
					b+=texbuf[j++];
					j++;
				}
				fwrite(texbuf,32*32,4,f2);
			}

			//fprintf(f,"%f %f %f\n",(float)r,float(g),float(b));
			r/=32.0*32.0*5;
			g/=32.0*32.0*5;
			b/=32.0*32.0*5;
			if (ObjectList[x].PerVertexColor)
			{
				ObjectList[x].VertexList[y].NewColor[0]=ObjectList[x].VertexList[y].Color[0]+r/255.0;
				ObjectList[x].VertexList[y].NewColor[1]=ObjectList[x].VertexList[y].Color[1]+g/255.0;
				ObjectList[x].VertexList[y].NewColor[2]=ObjectList[x].VertexList[y].Color[2]+b/255.0;
			}
			else
			{
				ObjectList[x].VertexList[y].NewColor[0]=(ObjectList[x].Color.r+r)/255.0;
				ObjectList[x].VertexList[y].NewColor[1]=(ObjectList[x].Color.g+g)/255.0;
				ObjectList[x].VertexList[y].NewColor[2]=(ObjectList[x].Color.b+b)/255.0;
			}
			//ObjectList[x].VertexList[y].NewColor[3]=1;
			
		}
	}
	fclose(f);
	fclose(f2);

	for (x=0; x<ObjectNum; x++)
	{
		ObjectList[x].Backface=ObjectList[x].CullBuff;
		for (int y=0; y<ObjectList[x].VertexNum; y++)
			for (int z=0; z<3; z++)
			ObjectList[x].VertexList[y].Color[z]=min(1,ObjectList[x].VertexList[y].NewColor[z]);
		ObjectList[x].PerVertexColor=true;
		ObjectList[x].CreateDisplayList();
	}

	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	SwitchTo2D();
	delete[] texbuf;
}

void SCENE::RadiosityCleanup()
{
	for (int x=0; x<ObjectNum; x++) delete[] ObjectList[x].RayIntersections;
}