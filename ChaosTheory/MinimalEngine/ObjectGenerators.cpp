#include "ObjectGenerators.h"
#include "math.h"

#ifdef INCLUDE_OBJ_SUPERSHAPE
float SuperShapeRad(float m,float n1,float n2,float n3,float a, float b, float phi)
{
   double t1,t2;
   //double a=1,b=1;

   t1 = cos(m * phi / 4) / a;
   t1 = fabs(t1);
   t1 = pow((double)t1,(double)n2);

   t2 = sin(m * phi / 4) / b;
   t2 = fabs(t2);
   t2 = pow((double)t2,(double)n3);

   return (float)pow((double)(t1+t2),(double)(-1/n1));
}
#endif


int SCENE::FindObjectIndex(int ID)
{
	for (int x=0; x<ObjectNum; x++)
		if (ObjectList[x].ID==ID) return x;
	return -1;
}

OBJECT *SCENE::FindObjectByID(int ID)
{
	int x=FindObjectIndex(ID);
	if (x==-1) return NULL;
	else return &ObjectList[x];
}

WORLDOBJECT *WORLD::FindObjectByID(int ID)
{
	for (int x=0; x<ObjectNum; x++)
		if (ObjectList[x].ID==ID) return &ObjectList[x];
	return NULL;
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

void OBJECT::AddVertex(float x, float y, float z, float u, float v)
{
	if (VertexCapacity==VertexNum)
	{
		VERTEX *Buffer=new VERTEX[(VertexCapacity+10)*2];
		memset(Buffer,0,sizeof(VERTEX)*(VertexCapacity+10)*2);
		if (VertexNum)
		{
			memcpy(Buffer,VertexList,sizeof(VERTEX)*VertexCapacity);
			delete VertexList;
		}
		VertexList=Buffer;
		VertexCapacity=(VertexCapacity+10)*2;
	}

	VertexList[VertexNum].Position.x=x;
	VertexList[VertexNum].Position.y=y;
	VertexList[VertexNum].Position.z=z;
	VertexList[VertexNum].MapTransformedPosition=VertexList[VertexNum].Position;

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
	PolygonList[PolygonNum].v[0]=x;
	PolygonList[PolygonNum].v[1]=y;
	PolygonList[PolygonNum].v[2]=z;
	PolygonList[PolygonNum].t[0]=VertexList[x].TextureCoordinate;
	PolygonList[PolygonNum].t[1]=VertexList[y].TextureCoordinate;
	PolygonList[PolygonNum].t[2]=VertexList[z].TextureCoordinate;
	PolygonList[PolygonNum].ct[0]=PolygonList[PolygonNum].t[0];
	PolygonList[PolygonNum].ct[1]=PolygonList[PolygonNum].t[1];
	PolygonList[PolygonNum].ct[2]=PolygonList[PolygonNum].t[2];
	VECTOR3 xy=V3_Sub(VertexList[y].Position,VertexList[x].Position),
		    xz=V3_Sub(VertexList[z].Position,VertexList[x].Position);
	PolygonList[PolygonNum].Normal=PolygonList[PolygonNum].CurrentNormal=V3_Cross(xz,xy);	
	PolygonList[PolygonNum].e[0]=AddEdge(x,y,PolygonNum,e1);	
	PolygonList[PolygonNum].e[1]=AddEdge(x,z,PolygonNum,e2);
	PolygonList[PolygonNum].e[2]=AddEdge(y,z,PolygonNum,e3);
	PolygonNum++;
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
	//for (int x=0; x<EdgeNum && !Found; x++)
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

		for (int t=0; t<3; t++)
		{
			if (!Swap)
			{
				PolygonList[x].ct[t].x=(PolygonList[x].t[t].x*Xtile+OffsetX)*xmul;
				PolygonList[x].ct[t].y=(PolygonList[x].t[t].y*Ytile+OffsetY)*ymul;
			}
			else
			{
				PolygonList[x].ct[t].y=(PolygonList[x].t[t].x*Xtile+OffsetX)*xmul;
				PolygonList[x].ct[t].x=(PolygonList[x].t[t].y*Ytile+OffsetY)*ymul;
			}
		}
	}
}

void OBJECT::CalculateNormals()
{
	int x;
	for (x=0; x<VertexNum; x++)	VertexList[x].Normal=V3_Make(0,0,0);
	for (x=0; x<PolygonNum; x++)
	{
		PolygonList[x].Normal=//V3_Normalize(
							  V3_Cross(V3_Sub(VertexList[PolygonList[x].v[2]].MapTransformedPosition,VertexList[PolygonList[x].v[0]].MapTransformedPosition),
									   V3_Sub(VertexList[PolygonList[x].v[1]].MapTransformedPosition,VertexList[PolygonList[x].v[0]].MapTransformedPosition))
							;	//	 );

		if (NormalsInverted) PolygonList[x].Normal=V3_Mults(PolygonList[x].Normal,-1);
			
		VertexList[PolygonList[x].v[0]].Normal=V3_Add(VertexList[PolygonList[x].v[0]].Normal,PolygonList[x].Normal);
		VertexList[PolygonList[x].v[1]].Normal=V3_Add(VertexList[PolygonList[x].v[1]].Normal,PolygonList[x].Normal);
		VertexList[PolygonList[x].v[2]].Normal=V3_Add(VertexList[PolygonList[x].v[2]].Normal,PolygonList[x].Normal);
		//M_Xform3(IT,PolygonList[x].Normal,PolygonList[x].CurrentNormal);
		PolygonList[x].CurrentNormal=V3_Normalize(PolygonList[x].Normal);
	}

	for (x=0; x<VertexNum; x++)	
	{
		VertexList[x].CurrentNormal=V3_Normalize(VertexList[x].Normal);
		//M_Xform3(IT,VertexList[x].CurrentNormal,VertexList[x].CurrentNormal);
	}

}

void CalculateMapTransform(int MapXformType, int Color, MATRIX MXMatrix, OBJECT *MappedObject, VECTOR3 MoveVec, VECTOR3 ScaleVec, VECTOR3 AxisVec, float Angle)
{
	for (int x=0; x<MappedObject->VertexNum; x++)
		if (MappedObject->VertexList[x].Selected)
		{
			float weight;
			if (Color==0) weight=MappedObject->VertexList[x].Weight[0];
			if (Color==1) weight=MappedObject->VertexList[x].Weight[1];
			if (Color==2) weight=MappedObject->VertexList[x].Weight[2];
			if (Color==3) weight=MappedObject->VertexList[x].Weight[3];
			if (Color==4) weight=(MappedObject->VertexList[x].Weight[0]+MappedObject->VertexList[x].Weight[1]+MappedObject->VertexList[x].Weight[2])/3.0f;
			if (MapXformType==aDDict_SCALE)
			{//scale
				VECTOR3 SVec;
				SVec=V3_Mults(ScaleVec,weight);
				MappedObject->VertexList[x].MapTransformedPosition.x=MappedObject->VertexList[x].Position.x*(SVec.x+1);
				MappedObject->VertexList[x].MapTransformedPosition.y=MappedObject->VertexList[x].Position.y*(SVec.y+1);
				MappedObject->VertexList[x].MapTransformedPosition.z=MappedObject->VertexList[x].Position.z*(SVec.z+1);
			}
			if (MapXformType==aDDict_ROTATE)
			{//rotate

				MATRIX m;
				M_Rotate(AxisVec.x,AxisVec.y,AxisVec.z,Angle*weight,m);

				M_Xform3(m,MappedObject->VertexList[x].Position,MappedObject->VertexList[x].MapTransformedPosition);

			}
			if (MapXformType==aDDict_MOVE)
			{//move
				VECTOR3 MoveVector=V3_Mults(MoveVec,weight);

				MappedObject->VertexList[x].MapTransformedPosition=V3_Add(MappedObject->VertexList[x].Position,MoveVector);
			}

		}
		else MappedObject->VertexList[x].MapTransformedPosition=MappedObject->VertexList[x].Position;
}

#ifdef INCLUDE_OBJ_BOX
char BoxVertexData[] = {

		-5,-5,-5,  0,  0, //0
		 5,-5,-5, 10,  0, //1
		 5, 5,-5,  1, 10, //2
		-5, 5,-5,  0, 10, //3
		 0, 0,-5,  5,  5, //4 center
		 5,-5, 5,  0,  0, //5
		 5, 5, 5,  0, 10, //6
		 5, 0, 0,  5,  5, //7 center
		-5, 5, 5,  10,  0, //8
		-5,-5, 5,  10,  10, //9
		 0, 0, 5,5,5, //10 center
		-5, 0, 0,5,5, //11 center
		 0,-5, 0,5,5, //12 center
		 0, 5, 0,5,5, //13 center
};

unsigned char BoxPolyData[] = {

			0,1,4,0,0,1,0,
			1,2,4,1,0,1,1,
			2,3,4,1,1,0,1,
			3,0,4,0,1,0,0,

			1,5,7,0,0,1,0,
			5,6,7,1,0,1,1,
			6,2,7,1,1,0,1,
			2,1,7,0,1,0,0,

			5,9,10,0,0,1,0,
			9,8,10,1,0,1,1,
			8,6,10,1,1,0,1,
			6,5,10,0,1,0,0,

			9,0,11,0,0,1,0,
			0,3,11,1,0,1,1,
			3,8,11,1,1,0,1,
			8,9,11,0,1,0,0,

			9,5,12,0,0,1,0,
			5,1,12,1,0,1,1,
			1,0,12,1,1,0,1,
			0,9,12,0,1,0,0,

			3,2,13,0,0,1,0,
			2,6,13,1,0,1,1,
			6,8,13,1,1,0,1,
			8,3,13,0,1,0,0,

};
#endif

void GeneratePrimitive(OBJECT *Object)
{
	int x,y;
	int x1,x2,y1,y2;
	VECTOR3 v;
#ifdef INCLUDE_OBJ_STORED
	if (Object->Primitive!=aDDict_STORED)
	{
		Object->VertexNum=0;
		Object->PolygonNum=0;
	}
#else
	Object->VertexNum=0;
	Object->PolygonNum=0;
#endif
	//M_Identity(Object->ModelView);
	//Object->Primitive=PrimitiveID;
	switch (Object->Primitive)
	{
#ifdef INCLUDE_OBJ_BOX
	case aDDict_BOX:
		{
			for (int x=0; x<14; x++) Object->AddVertex(BoxVertexData[x*5]/10.0f,BoxVertexData[x*5+1]/10.0f,BoxVertexData[x*5+2]/10.0f,BoxVertexData[x*5+3]/10.0f,BoxVertexData[x*5+4]/10.0f);
			for (int x=0; x<24; x++) Object->AddPolygon(BoxPolyData[x*7],BoxPolyData[x*7+1],BoxPolyData[x*7+2],BoxPolyData[x*7+3],BoxPolyData[x*7+4],BoxPolyData[x*7+5],BoxPolyData[x*7+6],0.5f,0.5f,1,0,0);
		}
		break;
#endif
#ifdef INCLUDE_OBJ_SPHERE
	case aDDict_SPHERE:

		Object->AddVertex(0,-0.5,0,0.5,0);
		Object->AddVertex(0,0.5,0,0.5,0);
		float theta,phi;
		for (x=1; x<Object->Param1-1; x++)
		{
			theta=(float)((x/(float)(Object->Param1-1)*180-90)*radtheta);
			for (int y=0; y<Object->Param2; y++)
			{
				phi=y/(float)Object->Param2*360.0f*(float)radtheta;
				v.x=(float)cos(theta)*(float)cos(phi)*0.5f;
				v.y=(float)cos(theta)*(float)sin(phi)*0.5f;
				v.z=(float)sin(theta)*0.5f;
				Object->AddVertex(v.x,v.z,-v.y,x/(float)Object->Param1,y/(float)Object->Param2);
			}
		}

		for (x=0; x<Object->Param1-1; x++)
		{
			x1=(x-1)*Object->Param2;
			x2=x*Object->Param2;
			for (int y=0; y<Object->Param2; y++)
			{
				y1=y+2;
				y2=(y+1)%Object->Param2+2;
				if (x>0 && x<Object->Param1-2)
				{
					Object->AddPolygon(x1+y1,x2+y1,x2+y2,aDDict_GOURAUDSHADE,(x)/(float)(Object->Param1),(y)/(float)Object->Param2,(x+1)/(float)(Object->Param1),y/(float)Object->Param2,(x+1)/(float)(Object->Param1),(y+1)/(float)Object->Param2,1,0,1);
					Object->AddPolygon(x1+y1,x2+y2,x1+y2,aDDict_GOURAUDSHADE,(x)/(float)(Object->Param1),(y)/(float)Object->Param2,(x+1)/(float)(Object->Param1),(y+1)/(float)Object->Param2,x/(float)(Object->Param1),(y+1)/(float)Object->Param2,0,1,1);
				}
				else
				if (x==0) Object->AddPolygon(y2,0,y1,aDDict_GOURAUDSHADE,(1)/(float)(Object->Param1),(y+1)/(float)Object->Param2,0,(y+1)/(float)Object->Param2,(1)/(float)(Object->Param1),(y)/(float)Object->Param2,1,1,1);
				else Object->AddPolygon(x1+y1,1,x1+y2,aDDict_GOURAUDSHADE,(x)/(float)(Object->Param1),(y)/(float)Object->Param2,1,(y+1)/(float)Object->Param2,(x)/(float)(Object->Param1),(y+1)/(float)Object->Param2,1,1,1);
			}
		}

		break;
#endif
#ifdef INCLUDE_OBJ_CYLINDER
	case aDDict_CYLINDER:
		Object->AddVertex(0,-0.5,0,0.5,0.5);
		Object->AddVertex(0,0.5,0,0.5,0.5);
		for (y=0; y<Object->Param2+1; y++)
		{
			for (x=0; x<Object->Param1; x++)
			{
				float theta=(float)((x/(float)Object->Param1*360.0f)*radtheta);
				v.x=(float)cos(theta)*0.5f;
				v.y=(float)sin(theta)*0.5f;
				v.z=-0.5f+y/(float)Object->Param2;
				Object->AddVertex(v.x,v.z,-v.y,x/(float)Object->Param1,v.z+0.5f);
			}
		}

		for (x=0; x<Object->Param1; x++)
		{
			x1=x;
			x2=(x+1)%Object->Param1;
			if (Object->Param3)
			{
				Object->AddPolygon(0,x1+2,x2+2,0.5f,0.5f,(float)sin(x/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,(float)cos(x/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,(float)sin((x+1)/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,(float)cos((x+1)/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,1,1,1);
				Object->AddPolygon(x1+Object->Param1*Object->Param2+2,1,x2+Object->Param1*Object->Param2+2,(float)sin(x/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,(float)cos(x/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,0.5f,0.5f,(float)sin((x+1)/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,(float)cos((x+1)/(float)(Object->Param1)*360*radtheta)*0.5f+0.5f,1,1,1);
			}
			for (int y=0; y<Object->Param2; y++)
			{
				y1=y*Object->Param1+2;
				y2=(y+1)*Object->Param1+2;
				Object->AddPolygon(x2+y1,x1+y1,x2+y2,aDDict_GOURAUDSHADE,(x+1)/(float)(Object->Param1),y/(float)Object->Param2,x/(float)(Object->Param1),y/(float)Object->Param2,(x+1)/(float)(Object->Param1),(y+1)/(float)Object->Param2,1,1,0);
				Object->AddPolygon(x2+y2,x1+y1,x1+y2,aDDict_GOURAUDSHADE,(x+1)/(float)(Object->Param1),(y+1)/(float)Object->Param2,x/(float)(Object->Param1),y/(float)Object->Param2,x/(float)(Object->Param1),(y+1)/(float)Object->Param2,0,1,1);
			}
		}


		break;
#endif
#ifdef INCLUDE_OBJ_CONE
	case aDDict_CONE:
		Object->AddVertex(0,0.5,0,0.5,0.5);
		Object->AddVertex(0,-0.5,0,0.5,0.5);

		for (y=1; y<=Object->Param3; y++)
		{
			for (x=0; x<Object->Param1; x++)
			{
				float theta=(float)((x/(float)Object->Param1*360.0f)*radtheta);
				v.x=(float)cos(theta)*0.5f*y/(float)Object->Param3;
				v.y=(float)sin(theta)*0.5f*y/(float)Object->Param3;
				v.z=-(-0.5f+y/(float)Object->Param3);
				Object->AddVertex(v.x,v.z,-v.y,v.x+0.5f,v.y+0.5f);
			}
		}

		for (x=0; x<Object->Param1; x++)
		{
			x1=x;
			x2=(x+1)%Object->Param1;

			Object->AddPolygon(x1+2,0,x2+2,aDDict_GOURAUDSHADE,1,1,1);
			
			if (Object->Param2)
				Object->AddPolygon(1,x1+Object->Param1*(Object->Param3-1)+2,x2+Object->Param1*(Object->Param3-1)+2,1,1,1);

			for (int y=0; y<Object->Param3-1; y++)
			{
				y1=y*Object->Param1+2;
				y2=(y+1)*Object->Param1+2;
				Object->AddPolygon(x1+y1,x2+y1,x2+y2,aDDict_GOURAUDSHADE,1,0,1);
				Object->AddPolygon(x1+y1,x2+y2,x1+y2,aDDict_GOURAUDSHADE,0,1,1);
			}
		}
		
		break;
#endif
#ifdef INCLUDE_OBJ_PLANE
	case aDDict_PLANE:
		for (x=0; x<=Object->Param1; x++)
			for (y=0; y<=Object->Param2; y++)
				Object->AddVertex(x/(float)Object->Param1-0.5f,0,y/(float)Object->Param2-0.5f,x/(float)Object->Param1,y/(float)Object->Param2);
		for (x=0; x<Object->Param1; x++)
		{
			x1=x*(Object->Param2+1);
			x2=(x+1)*(Object->Param2+1);
			for (int y=0; y<Object->Param2; y++)
			{
				y1=y;
				y2=(y+1)%(Object->Param2+1);
				Object->AddPolygon(x1+y1,x2+y1,x2+y2,1,0,1);
				Object->AddPolygon(x1+y1,x2+y2,x1+y2,0,1,1);
			}
		}
		break;
#endif
#ifdef INCLUDE_OBJ_CIRCLE
	case aDDict_CIRCLE:
		for (x=0; x<Object->Param1; x++)
		{
				theta=(float)((x/(float)Object->Param1*360.0f)*radtheta);
				v.x=(float)cos(theta);
				v.y=(float)sin(theta);
				v.z=0;
				float p=Object->Param2/255.0f*0.5f;
				Object->AddVertex(v.x*0.5f,v.z,-v.y*0.5f,x/(float)Object->Param1,0);
				Object->AddVertex(v.x*p,v.z,-v.y*p,x/(float)Object->Param1,1);
		}
		for (x=0; x<Object->Param1; x++)
		{
			x1=(x)%Object->Param1*2;
			x2=(x+1)%Object->Param1*2;
			y1=0;
			y2=1;
			Object->AddPolygon(x1+y1,x2+y1,x2+y2,aDDict_FLATSHADE,x/(float)Object->Param1,0,(x+1)/(float)Object->Param1,0,(x+1)/(float)Object->Param1,1,1,0,1);
			Object->AddPolygon(x1+y1,x2+y2,x1+y2,aDDict_FLATSHADE,x/(float)Object->Param1,0,(x+1)/(float)Object->Param1,1,x/(float)Object->Param1,1,0,1,1);
		}
		break;
#endif
#ifdef INCLUDE_OBJ_LINE
	case aDDict_LINE:
		for (x=-1; x<=Object->Param1; x++)
		{
			Object->AddVertex(0,0,-0.5f+x/(float)(Object->Param1-1),0,0);
			/*if (x>0 && x<Object->Param1) 
				Object->AddEdge(x,x+1,-1);*/
		}
		break;
#endif
#ifdef INCLUDE_OBJ_ARC
	case aDDict_ARC:
		for (x=-1; x<=Object->Param1; x++)
		{
			float phase=x/(float)(Object->Param1-1)*(float)Object->Param2*radtheta;
			
			Object->AddVertex((float)(0.5f*cos(phase)),0,(float)(0.5f*sin(phase)),0,0);
			/*if (x>0 && x<Object->Param1) 
				Object->AddEdge(x,x+1,-1);*/
		}
		break;
#endif
#ifdef INCLUDE_OBJ_LOFT
	case aDDict_LOFT: 
		{
		VECTOR3 Up=V3_Make(0,1,0);

		Object->Backface=false;
		OBJECT *v1,*v2;
		v1=(OBJECT*)Object->Param1;
		v2=(OBJECT*)Object->Param2;

		if (v1 && v2)
		{
			bool xc=v1->Param2==360,
				 yc=v2->Param2==360;

			int v1v=v1->VertexNum-2;
			int v2v=v2->VertexNum-2;

			if (xc) v1v--;
			if (yc) v2v--;

			int xx=0;
			if ((v1->VertexNum>1) && (v2->VertexNum>1))
			{
				int xa,ya;

				for (xa=1;xa<=v1v;xa++)
				{
					VECTOR3 va,vb,vc;
					va=v1->VertexList[xa-1].Position;
					vb=v1->VertexList[xa].Position;
					vc=v1->VertexList[xa+1].Position;
					M_Xformd(v1->ModelView,va,va);
					M_Xformd(v1->ModelView,vb,vb);
					M_Xformd(v1->ModelView,vc,vc);

					VECTOR3 a1=V3_Normalize(V3_Sub(vb,va));
					VECTOR3 a2=V3_Normalize(V3_Sub(vc,va));
					VECTOR3 dir=V3_Normalize(V3_Add(a1,a2));

					VECTOR3 nx=V3_Cross(Up,dir);
					VECTOR3 ny=V3_Cross(nx,dir);

					int yy=0;
					for (ya=1;ya<=v2v;ya++)
					{
						VECTOR3 yv=v2->VertexList[ya].Position;
						M_Xformd(v2->ModelView,yv,yv);

						VECTOR3 nv=V3_Add(V3_Mults(nx,yv.x),V3_Mults(ny,yv.z));

						nv=V3_Add(nv,vb);

						Object->AddVertex(nv.x,nv.y,nv.z,xx/(float)(v1v-1),yy/(float)(v2v-1));

						yy++;
					}

					xx++;
				}

				if (xc) v1v++;
				if (yc) v2v++;

				for (xx=0;xx<=v2v-2;xx++)
					for (int yy=0;yy<=v1v-2;yy++)
					{
						int xa=xx, ya=yy;
						int x1=xx+1,y1=yy+1;
						int v2vs=v2v;

						if (yc)
						{
							v2vs--;
							xa%=v2v-1;
							x1%=v2v-1;
						}
						if (xc)
						{
							ya%=v1v-1;
							y1%=v1v-1;
						}

						Object->AddPolygon(ya*v2vs+xa,ya*v2vs+x1,y1*v2vs+x1,aDDict_GOURAUDSHADE,xx/((float)v2v-1),yy/((float)v1v-1),(xx+1)/((float)v2v-1),yy/((float)v1v-1),(xx+1)/((float)v2v-1),(yy+1)/((float)v1v-1),1,0,1);
						Object->AddPolygon(ya*v2vs+xa,y1*v2vs+x1,y1*v2vs+xa,aDDict_GOURAUDSHADE,xx/((float)v2v-1),yy/((float)v1v-1),(xx+1)/((float)v2v-1),(yy+1)/((float)v1v-1),xx/((float)v2v-1),(yy+1)/((float)v1v-1),0,1,1);
					}


			}
		}
		}
		break;
#endif
#ifdef INCLUDE_OBJ_FUR
	case aDDict_FUR:
		{
			srand(115); //9,26,30,35,39,46,62,75,79,115,116,126
						//135 volt az utolso
			OBJECT *Host=(OBJECT*)Object->Param5;
			for (int x=0; x<Host->PolygonNum; x++)
			{
				VECTOR3 Ha,Hb,Hc;
				Ha=Host->VertexList[Host->PolygonList[x].v[0]].Position;
				Hb=Host->VertexList[Host->PolygonList[x].v[1]].Position;
				Hc=Host->VertexList[Host->PolygonList[x].v[2]].Position;

				MATRIX m;
				memcpy(&m,Host->ModelView,sizeof(MATRIX));
				//for (int a=0; a<4; a++) for (int b=0; b<4; b++) m[a][b]=KillFloat(m[a][b],2);

				M_Xformd(m,Ha,Ha);
				M_Xformd(m,Hb,Hb);
				M_Xformd(m,Hc,Hc);

				VECTOR3 Origin=V3_Mults(V3_Add(V3_Add(Ha,Hb),Hc),1/3.0f);
				VECTOR3 i=V3_Normalize(V3_Cross(V3_Sub(Hc,Ha),V3_Sub(Hb,Ha)));

				for (int y=0; y<Object->Param2; y++)
				{
					VECTOR3 Random=V3_Normalize(V3_Make((float)rand(),(float)rand(),(float)rand()));
					VECTOR3 j=V3_Cross(i,Random);
					VECTOR3 k=V3_Mults(V3_Cross(i,j),0.5f);

					VECTOR3 a,b,c,d;
					float r=((rand()/(float)RAND_MAX)-0.5f)*2.0f;
					i=V3_Mults(i,Object->Param3/255.0f);
					k=V3_Mults(k,Object->Param1/255.0f);
					
					a=V3_Add(Origin,k);
					b=V3_Add(V3_Add(Origin,k),i);
					c=V3_Add(V3_Sub(Origin,k),i);
					d=V3_Sub(Origin,k);

					Object->AddVertex(a.x,a.y,a.z,0,0);
					Object->AddVertex(b.x,b.y,b.z,0,1);
					Object->AddVertex(c.x,c.y,c.z,1,1);
					Object->AddVertex(d.x,d.y,d.z,1,0);

					Object->AddPolygon((x*Object->Param2+y)*4,(x*Object->Param2+y)*4+1,(x*Object->Param2+y)*4+2,aDDict_GOURAUDSHADE,1,0,1);
					Object->AddPolygon((x*Object->Param2+y)*4,(x*Object->Param2+y)*4+2,(x*Object->Param2+y)*4+3,aDDict_GOURAUDSHADE,0,1,1);
				}
			}
		}
		break;
#endif
#ifdef INCLUDE_OBJ_SUPERSHAPE
	case aDDict_SUPERSHAPE:
		{
			SUPERSHAPE *s=(SUPERSHAPE*)Object->Param1;

			for (int x=0; x<=s->Xres; x++)
			{
				float phi=LinearInterpolate((float)s->Rangex1,(float)s->Rangex2,x/(float)(s->Xres))/180.0f*3.1415f;
			
				float r0=0;
				if (!s->Sphere) r0=s->Trad0;

				float r1a=SuperShapeRad(s->SuperShape1.mint+s->SuperShape1.mfloat/255.0f,s->SuperShape1.n1,s->SuperShape1.n2,s->SuperShape1.n3,s->SuperShape1.a,s->SuperShape1.b,phi)+r0;

				if (s->RadFunc)
				{
					float r1;
					if (s->RadFunc==1) r1=s->Rada*phi;
					if (s->RadFunc==2) r1=(float)(s->Rada*exp(s->Radb*phi));
					r1a*=r1;
				}

				float r1=s->Verta*phi;
				
				for (int y=0; y<=s->Yres; y++)
				{
					float ro=LinearInterpolate((float)s->Rangey1,(float)s->Rangey2,y/(float)(s->Yres))/180.0f*3.1415f;

					float r2a=SuperShapeRad(s->SuperShape2.mint+s->SuperShape2.mfloat/255.0f,s->SuperShape2.n1,s->SuperShape2.n2,s->SuperShape2.n3,s->SuperShape2.a,s->SuperShape2.b,ro);

					VECTOR3 pos;
					if (s->Sphere) pos=V3_Make((float)(r1a*cos(phi )*r2a*cos(ro )),(float)(r1a*sin(phi )*r2a*cos(ro )),(float)(r2a*sin(ro )));
					else pos=V3_Make((float)(cos(phi )*(r1a+r2a*cos(ro ))),(float)(sin(phi )*(r1a+r2a*cos(ro ))),(float)(r2a*sin(ro )));

					if (s->VertFunc) pos.z*=r1;

					Object->AddVertex(pos.x,pos.y,pos.z,x/(float)(s->Xres),y/(float)(s->Yres));

				}
			}

			for (x=0; x<s->Xres; x++)
			{
				for (int y=0; y<s->Yres; y++)
				{
					Object->AddPolygon(x*(s->Yres+1)+y,(x+1)*(s->Yres+1)+y+1,(x+1)*(s->Yres+1)+y,aDDict_GOURAUDSHADE,0,1,1);
					Object->AddPolygon(x*(s->Yres+1)+y,(x)*(s->Yres+1)+y+1,(x+1)*(s->Yres+1)+y+1,aDDict_GOURAUDSHADE,1,0,1);
				}
			}

		}
		break;
#endif
	case -25472:
	default: break;
	}
		
#ifdef INCLUDE_OBJ_STORED
	if (Object->Primitive!=aDDict_STORED)
	{
		Object->CalculateTextureCoordinates();
	}
#else
	Object->CalculateTextureCoordinates();
#endif

	for (x=0; x<Object->PolygonNum; x++)
	{
		if (Object->Shading!=aDDict_DEFAULTSHADE) Object->PolygonList[x].CurrentShading=Object->Shading;
	}


}
