/////// Advanced mesh generators - mapxform, meshsmooth, meshblur

#include "AdvancedMesh.h"
#include "includelist.h"
#include "3dEngine.h"

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
	int x;
	for (x=0; x<o->PolygonNum; x++) Level[x]=65536;


	int *Level2=new int[o->VertexNum];
	for (x=0; x<o->VertexNum; x++) Level2[x]=6553600;

	switch (SelectData->Brush)
	{
	case 0: {   //vertexselect brush
				//Param1: Kivalasztott Poly ID
				//Param2: Sugar
				/*SelectVertexNeighbours(o,Buffer,Level2,SelectData->Param1,SelectData->Param2,0);
				for (int x=0; x<o->PolygonNum; x++)
				{
					Buffer[x]=(Level2[o->PolygonList[x].v[0]]<=SelectData->Param2) || 
						      (Level2[o->PolygonList[x].v[1]]<=SelectData->Param2) || 
							  (Level2[o->PolygonList[x].v[2]]<=SelectData->Param2);
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
					int j=(int)(((Random()+0.5f))*SelectData->Param1);
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

void SelectObjPolys(OBJECT *o, PVSelect *i, int BrushNum, int num)
{
	for (int x=0; x<o->PolygonNum; x++) 
	{
		o->PolygonList[x].Highlighted=false;
		o->PolygonList[x].Selected=false;
	}
	if (num==-1)
		for (int x=0; x<BrushNum; x++)
		{
			Obj_DoPolySelection(o,&i[x],false);
		}
	else
	{
		for (int a=0; a<=num; a++)
		{
			Obj_DoPolySelection(o,&i[a],false);
		}
	}
}

void SelectObjVertices(OBJECT *o,PVSelect *i,int BrushNum, int num, bool Border)
{
	SelectObjPolys(o,i,BrushNum, num);
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
				o->VertexList[o->PolygonList[x].v[0]].Selected=true;
				o->VertexList[o->PolygonList[x].v[1]].Selected=true;
				o->VertexList[o->PolygonList[x].v[2]].Selected=true;
			}
	}
	else
	{
		int x;
		for (x=0; x<o->PolygonNum; x++)
		{
			if (o->PolygonList[x].Selected)
			{
				o->VertexList[o->PolygonList[x].v[0]].SelectCount++;
				o->VertexList[o->PolygonList[x].v[1]].SelectCount++;
				o->VertexList[o->PolygonList[x].v[2]].SelectCount++;
			}
			o->VertexList[o->PolygonList[x].v[0]].PolyCount++;
			o->VertexList[o->PolygonList[x].v[1]].PolyCount++;
			o->VertexList[o->PolygonList[x].v[2]].PolyCount++;
		}
		for (x=0; x<o->VertexNum; x++) 
			o->VertexList[x].Selected=o->VertexList[x].PolyCount==o->VertexList[x].SelectCount;
	}
}

#ifdef INCLUDE_OBJ_MAPXFORM

RGBA GetTexPixel(MATERIAL *m, float u, float v)
{
	int x=((int)(u*m->Size))%m->Size;
	while (x<0) x+=m->Size;
	int y=((int)(v*m->Size))%m->Size;
	while (y<0) y+=m->Size;
	return m->ImageData.Data[y*m->Size+x];
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

	int x;
	for (x=0; x<o->VertexNum; x++)
	{
		for (int z=0; z<4; z++)
		o->VertexList[x].Weight[z]=0;
		o->VertexList[x].EdgeNum=0;
	}

	RGBA Col;

	for (x=0; x<o->PolygonNum; x++)
	{
		Col=GetTexPixel(m,o->PolygonList[x].ct[0].x,o->PolygonList[x].ct[0].y);
		int z;
		for (z=0; z<4; z++)
		o->VertexList[o->PolygonList[x].v[0]].Weight[z]+=Col.c[z]/255.0f;
		o->VertexList[o->PolygonList[x].v[0]].EdgeNum++;

		Col=GetTexPixel(m,o->PolygonList[x].ct[1].x,o->PolygonList[x].ct[1].y);
		for (z=0; z<4; z++)
		o->VertexList[o->PolygonList[x].v[1]].Weight[z]+=Col.c[z]/255.0f;
		o->VertexList[o->PolygonList[x].v[1]].EdgeNum++;
		
		Col=GetTexPixel(m,o->PolygonList[x].ct[2].x,o->PolygonList[x].ct[2].y);
		for (z=0; z<4; z++)
		o->VertexList[o->PolygonList[x].v[2]].Weight[z]+=Col.c[z]/255.0f;
		o->VertexList[o->PolygonList[x].v[2]].EdgeNum++;
	}

	for (x=0; x<o->VertexNum; x++)
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

#endif

float vertBlendingWeights[257][256];

int GetEdgeVertex(OBJECT *o, int BadVertex, int EdgeID)
{
	if (o->EdgeList[EdgeID].v1==BadVertex) return o->EdgeList[EdgeID].v2; else return o->EdgeList[EdgeID].v1;
	return -1;
}

#ifdef INCLUDE_OBJ_BUTTERFLYSMOOTH
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
      vertBlendingWeights[K][j] = (float)((0.25f + cos(2*3.1415f*j/(float)K) + 0.5f*cos(4*3.1415f*j/(float)K)) / (float)K);
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
#endif

#ifdef INCLUDE_MESHSMOOTH
void OBJECT::TessellateEdges(int *EdgeVertMap, bool Linear)
{
	for (int x=EdgeNum-1; x>=0; x--)
	{
		VECTOR3 NewVert;
#ifdef INCLUDE_OBJ_BUTTERFLYSMOOTH
		if (!Linear) NewVert=ButterflyTessellation(this,x);
#ifdef INCLUDE_OBJ_LINEARSMOOTH
		else NewVert=V3_Mults(V3_Add(VertexList[EdgeList[x].v1].Position,VertexList[EdgeList[x].v2].Position),0.5f);
#endif
#else
		NewVert=V3_Mults(V3_Add(VertexList[EdgeList[x].v1].Position,VertexList[EdgeList[x].v2].Position),0.5f);
#endif

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

		a=PolygonList[x].v[0];
		b=PolygonList[x].v[1];
		c=PolygonList[x].v[2];
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
		PolygonList[x].v[0]=d;
		PolygonList[x].v[1]=f;
		PolygonList[x].v[2]=e;
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

#ifdef INCLUDE_OBJ_BUTTERFLYSMOOTH
	if (!Linear)
	{
		for (int x=0; x<VertexNum; x++) VertexList[x].MapTransformedPosition=VertexList[x].Position;
		CalculateNormals();

		SortVertexEdges();
	}
#endif

	TessellateEdges(EdgeVertMap, Linear);
	BuildNewFaces(EdgeVertMap, Linear);

	delete EdgeVertMap;

	CalculateTextureCoordinates();
}
#endif

//#ifdef INCLUDE_OBJ_MESHBLUR
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
}
//#endif
