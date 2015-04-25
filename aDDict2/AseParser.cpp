#include "AseParser.h"
#include "3dengine.h"
#include <windows.h>
#include <stdio.h>
#include "vectors.h"
#include "modellergui.h"

char Line[2048];

char *ReadLn(FILE *f)
{
	memset(Line,0,2048);
	return fgets(Line,2048,f);
}

void ParseASE(char *FileName)
{
	if (strstr(FileName,".ase\0"))
	{
		int Depth=0;
		bool GeomObject=false;
		bool MeshData=false;
		FILE *f=fopen(FileName,"r+t");
		OBJECT *o=NULL;

		int NumVertex=0;
		int NumFaces=0;
		int NumTVertex=0;
		int NumTFaces=0;

		bool VertexList=false,FaceList=false,TVertList=false,TFaceList=false,Normals=false;
		MainScroller->ParentItem->SelectedItem=3+(int)(ActualScene->ItemList->List->Next->Next->SubOpened)*8;

		VECTOR2 *TList=new VECTOR2[1];
		int TVertCounter=0;

		while (ReadLn(f))
		{
			if (Depth==0 && strstr(Line,"*GEOMOBJECT")) 
			{
				GeomObject=true;
				ActualScene->AddObject();
				o=&(ActualScene->ObjectList[ActualScene->ObjectNum-1]);
				o->Primitive=aDDict_STORED;
				o->ListItem->Name="STORED";
				MainScroller->ParentItem->ItemNum++;
				
			}

			if (Depth==1 && GeomObject)
			{
				if (strstr(Line,"*NODE_NAME"))
				{
					char *start,*end;
					start=strstr(Line,"\"");
					end=strstr(start+1,"\"");
					o->ListItem->Name=new char[end-start+1];
					memset(o->ListItem->Name,0,end-start+1);
					memcpy(o->ListItem->Name,start+1,end-start-1);
				}
				if (strstr(Line,"*MESH")) MeshData=true;
			}

			if (Depth==2 && MeshData)
			{
				if (strstr(Line,"*MESH_NUMVERTEX"))
				{
					char *a=strstr(Line,"*MESH_NUMVERTEX");
					sscanf(a,"*MESH_NUMVERTEX %d",&NumVertex);
				}
				if (strstr(Line,"*MESH_NUMFACES"))
				{
					char *a=strstr(Line,"*MESH_NUMFACES");
					sscanf(a,"*MESH_NUMFACES %d",&NumFaces);
				}
				if (strstr(Line,"*MESH_NUMTVERTEX"))
				{
					char *a=strstr(Line,"*MESH_NUMTVERTEX");
					sscanf(a,"*MESH_NUMTVERTEX %d",&NumTVertex);
					TVertCounter=0;
					delete[] TList;
					TList=new VECTOR2[NumTVertex];
				}
				if (strstr(Line,"*MESH_NUMTVFACES"))
				{
					char *a=strstr(Line,"*MESH_NUMTVFACES");
					sscanf(a,"*MESH_NUMTVFACES %d",&NumTFaces);
				}
				if (strstr(Line,"*MESH_VERTEX_LIST")) VertexList=true;
				if (strstr(Line,"*MESH_FACE_LIST")) FaceList=true;
				if (strstr(Line,"*MESH_TVERTLIST")) TVertList=true;
				if (strstr(Line,"*MESH_TFACELIST")) TFaceList=true;
				if (strstr(Line,"*MESH_NORMALS")) Normals=true;
			}

			if (Depth==3)
			{
				if (VertexList && strstr(Line,"*MESH_VERTEX "))
				{
					int n;
					VECTOR3 v;
					char *b=strstr(Line,"*MESH_VERTEX");
					sscanf(b,"*MESH_VERTEX %d %f %f %f",&n,&v.x,&v.y,&v.z);
					o->AddVertex(-v.x/10.0f,v.z/10.0f,v.y/10.0f,0,0);
				}
				
				if (FaceList && strstr(Line,"*MESH_FACE "))
				{
					int v1,v2,v3;
					char *b=strstr(Line,"A:");
					sscanf(b,"A: %d",&v1);
					b=strstr(Line,"B:");
					sscanf(b,"B: %d",&v2);
					b=strstr(Line,"C:");
					sscanf(b,"C: %d",&v3);
					o->AddPolygon(v1,v2,v3,aDDict_GOURAUDSHADE,1,1,1);
				}
				if (TVertList && strstr(Line,"*MESH_TVERT "))
				{
					int n;
					char *b=strstr(Line,"*MESH_TVERT");
					sscanf(b,"*MESH_TVERT %d %f %f",&n,&(TList[TVertCounter].u),&(TList[TVertCounter].v));
					TList[TVertCounter].v*=-1;
					TVertCounter++;
				}
				if (TFaceList && strstr(Line,"*MESH_TFACE ") && NumTFaces==NumFaces)
				{
					int n,v1,v2,v3;
					char *b=strstr(Line,"*MESH_TFACE");
					sscanf(b,"*MESH_TFACE %d %d %d %d",&n,&v1,&v2,&v3);
					o->PolygonList[n].t[0]=o->PolygonList[n].ct[0]=TList[v1];
					o->VertexList[o->PolygonList[n].v1].CurrentTextureCoordinate=o->VertexList[o->PolygonList[n].v1].TextureCoordinate=TList[v1];
					o->PolygonList[n].t[1]=o->PolygonList[n].ct[1]=TList[v2];
					o->VertexList[o->PolygonList[n].v2].CurrentTextureCoordinate=o->VertexList[o->PolygonList[n].v2].TextureCoordinate=TList[v2];
					o->PolygonList[n].t[2]=o->PolygonList[n].ct[2]=TList[v3];
					o->VertexList[o->PolygonList[n].v3].CurrentTextureCoordinate=o->VertexList[o->PolygonList[n].v3].TextureCoordinate=TList[v3];
				}
				if (Normals && strstr(Line,"*MESH_FACENORMAL"))
				{
					int n;
					VECTOR3 v;
					char *b=strstr(Line,"*MESH_FACENORMAL");
					sscanf(b,"*MESH_FACENORMAL %d %f %f %f",&n,&v.x,&v.y,&v.z);
					o->PolygonList[n].Normal=v;
					o->PolygonList[n].CurrentNormal=v;
				}
				if (Normals && strstr(Line,"*MESH_VERTEXNORMAL"))
				{
					int n;
					VECTOR3 v;
					char *b=strstr(Line,"*MESH_VERTEXNORMAL");
					sscanf(b,"*MESH_VERTEXNORMAL %d %f %f %f",&n,&v.x,&v.y,&v.z);
					o->VertexList[n].CurrentNormal=o->VertexList[n].Normal=v;
				}
			}

			if (strstr(Line,"{")) Depth++;
			if (strstr(Line,"}")) Depth--;
			if (Depth==0) 
			{
				if (GeomObject)
				{
					o->CalculateNormals();
					o->CalculateTextureCoordinates();
					o->CreateDisplayList();
				}
				GeomObject=false;
			}
			if (Depth==1) MeshData=false;
			if (Depth==2) VertexList=FaceList=TVertList=TFaceList=Normals=false;
		}

		o=NULL;
		fclose(f);
	}
	if (strstr(FileName,".obj\0"))
	{
		FILE *f=fopen(FileName,"r+t");

		int Vcount=0,VNcount=0,VTcount=0,FCount=0;
		while (ReadLn(f))
		{
			if (Line[0]=='v')
				switch (Line[1])
				{
					case ' ': Vcount++; break;
					case 'n': VNcount++; break;
					case 't': VTcount++; break;
				}
			if (Line[0]=='f') FCount++;
		}

		fseek(f,0,SEEK_SET);

		int Vpos=0,VNpos=0,VTpos=0,Fpos=0;
		VECTOR3 *Vx,*Vn;
		VECTOR2 *Vt;

		if (Vcount)
		{

			Vx=new VECTOR3[Vcount];
			memset(Vx,0,sizeof(VECTOR3)*Vcount);

			Vn=new VECTOR3[VNcount];
			memset(Vn,0,sizeof(VECTOR3)*VNcount);

			Vt=new VECTOR2[VTcount];
			memset(Vt,0,sizeof(VECTOR2)*VTcount);

			ActualScene->AddObject();
			OBJECT *o=&ActualScene->ObjectList[ActualScene->ObjectNum-1];
			o->ListItem->Name="STORED";
			o->Primitive=aDDict_STORED;
			MainScroller->ParentItem->ItemNum++;


			while (ReadLn(f))
			{
				if (Line[0]=='v')
					switch (Line[1])
					{
						case ' ': sscanf(&Line[2],"%f %f %f",&Vx[Vpos].x,&Vx[Vpos].y,&Vx[Vpos].z); o->AddVertex(Vx[Vpos].x,Vx[Vpos].y,Vx[Vpos].z,0,0); Vpos++; break;
						case 'n': sscanf(&Line[3],"%f %f %f",&Vn[VNpos].x,&Vn[VNpos].y,&Vn[VNpos].z); VNpos++; break;
						case 't': sscanf(&Line[3],"%f %f",&Vt[VTpos].x,&Vt[VTpos].y); VTpos++; break;
					}
				if (Line[0]=='f')
				{
					int Count=0;
					int v1a,v1b,v1c;
					char *Line2=&Line[2];
					if (sscanf(Line2,"%d/%d/%d ",&v1a,&v1b,&v1c)==3)
					{
						int v2a,v2b,v2c,v3a,v3b,v3c;
						Line2=strstr(Line2," ");
						Line2++;

						if (sscanf(Line2,"%d/%d/%d ",&v2a,&v2b,&v2c)==3)
						{
							Line2=strstr(Line2," ");
							Line2++;

							while (sscanf(Line2,"%d/%d/%d ",&v3a,&v3b,&v3c)==3)
							{
								Line2=strstr(Line2," ");

								o->AddPolygon(v1a-1,v3a-1,v2a-1,aDDict_GOURAUDSHADE,Vt[v1b-1].x,Vt[v1b-1].y,Vt[v3b-1].x,Vt[v3b-1].y,Vt[v2b-1].x,Vt[v2b-1].y,1,1,1);

								v2a=v3a;
								v2b=v3b;
								v2c=v3c;

								if (!Line2) break;
								Line2++;
							}

						}
					}

				}
			}

			o->CalculateNormals();
			o->CreateDisplayList();

			delete[] Vx;
			delete[] Vn;
			delete[] Vt;
		}
		
		fclose(f);
	}
}