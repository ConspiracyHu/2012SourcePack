#include "Parser.h"
#include "EventHandler.h"
//#include <stdio.h>
//char ss[256];

TEXTURE *BufferTexture;
LAYER *BufferLayer;

bool DisplayPrecalcAnim;
int precalcdelaytimer=0;

int minf=0;
int maxf=0;

int data[5000];
int datacnt=0;

float readfloat(unsigned char **file,unsigned char floatbytes)
{
	unsigned long int w=0;
	memcpy(&w,*file,floatbytes);
	*file+=floatbytes;
	w<<=((4-floatbytes)*8);

	if (((w>>24)&127)!=0)
	{
		bool found=false;
		for (int x=0; x<datacnt; x++)
			if (data[x]==w)
			{
				found=true;
				break;
			}
			if (!found)
			{
				data[datacnt++]=w;
			}

		int k=((int)((w>>24)&127))-63;
		minf=min(minf,k);
		maxf=max(maxf,k);
	}

	return *(float*) &w;
}

void readmatrix(unsigned char **file, unsigned char floatbytes,MATRIX &m, bool orient, bool pos)
{
	M_Identity(m);
	for (int y=0;y<4;y++)
		for (int x=0;x<3;x++)
			if ((orient && y<3) || (pos && y==3))
				m[x][y]=readfloat(file,floatbytes);
}


#define READ(t) *(t*)raw; raw+=sizeof(t)

int datasizes[] = {
 /* 0 */
#ifdef INCLUDE_TEX_Envmap
 sizeof(ENVMAPTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Plasma
 sizeof(PLASMATEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Map
 sizeof(MAPTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Blur
 sizeof(BLURTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_DirBlur
 sizeof(DIRBLURTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Text
 sizeof(TEXTTEMPLATE)-4,
#else
	 0,
#endif
#ifdef INCLUDE_TEX_SubPlasma
 sizeof(SUBPLASMATEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_FractalPlasma
 sizeof(FRACTALPLASMATEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Colorize
 sizeof(COLORIZETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Shade
 sizeof(SHADETEMPLATE),
#else
	 0,
#endif

 /* 10 */
#ifdef INCLUDE_TEX_Brightness
 sizeof(BRIGHTNESSTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Copy
 sizeof(COPYTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Cells
 sizeof(CELLSTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Twirl
 sizeof(TWIRLTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_SineDist
 sizeof(SINEDISTTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Mix
 sizeof(MIXTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_MixMap
 sizeof(MIXMAPTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Scale
 sizeof(SCALETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_SineColor
 sizeof(SINECOLORTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Max
 sizeof(MAXTEMPLATE),
#else
	 0,
#endif

 /* 20 */
#ifdef INCLUDE_TEX_HSV
 sizeof(HSVTEMPLATE),
#else
	 0,
#endif
 0, //EMBOSS
 0, //INVERT
#ifdef INCLUDE_TEX_Glass
 sizeof(GLASSTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Pixelize
 sizeof(PIXELIZETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Offset
 sizeof(OFFSETTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Crystalize
 sizeof(CRYSTALIZETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Rectangle
 sizeof(RECTANGLETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Circle
 sizeof(CIRCLETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Contrast
 sizeof(CONTRASTTEMPLATE),
#else
	 0,
#endif
 /* 30 */

 1, //MakeMaterial
#ifdef INCLUDE_TEX_Gradient
 sizeof(GRADIENTTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Rotozoom
 sizeof(ROTOZOOMTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_ChamferRectangle
 sizeof(CHAMFERRECTANGLETEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Dots
 sizeof(DOTSTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_Jpeg
 sizeof(JPEGTEMPLATE),
#else
	 0,
#endif
#ifdef INCLUDE_TEX_FractalPlasmaOld
 sizeof(FRACTALPLASMAOLDTEMPLATE),
#else
	 0,
#endif
}; 

void InitTextureBuffer()
{
	BufferTexture=new TEXTURE(1024);
	BufferLayer=new LAYER;
	BufferLayer->Data=new RGBA[1024*1024];
}

int pEventNum;
MATERIAL *pMaterialList;
SCENE *pSceneList;
WORLD *pWorldList;
EVENT *pEventList;
PSelect *pPolySelectList;

#pragma comment(lib,"winmm.lib")
void PrecalcAnim(float t)
{
	if (DisplayPrecalcAnim)
	{
		if (Keys[VK_ESCAPE]) exit(0);
		int tme=timeGetTime();
		if (tme>precalcdelaytimer+100)
		{
			precalcdelaytimer=tme;
			MSG msg;
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
			else
			{
					glClear(0x4100);

					DisplayFrame((int)(t*1000),
								 pMaterialList, 
								 pSceneList, 
								 pWorldList, 
								 pEventList, 
								 pEventNum);

					SwapBuffers(hDC);
					//Sleep(10);
			}
		}
	}
}

void ImportTextures(void **f, MATERIAL **MaterialList, int LOD)
{
	unsigned char * raw = (unsigned char*)*f;
	int NumTextures = READ(unsigned char);
	(*MaterialList)=NULL;

	int TexCounter=0;

	for (int i=0; i<NumTextures; i++) 
	{
		int Size = READ(unsigned char);

		int TextureSize=32;
		int x;
		for (x=0; x<Size; x++) TextureSize<<=1;
		
		//if (LOD!=-1) TextureSize=TextureLODs[72-i*3+(2-LOD)];

		BufferTexture->XRes=BufferTexture->YRes=TextureSize;

		memset(BufferTexture->Layers[0].Data,0,TextureSize*TextureSize*4);
		int a;
		for (a=0; a<TextureSize*TextureSize; a++) BufferTexture->Layers[0].Data[a].a=255;
		for (x=1; x<8; x++)	memcpy(BufferTexture->Layers[x].Data,BufferTexture->Layers[0].Data,TextureSize*TextureSize*4);

		int NumFilters = READ(unsigned char);
		int ID = READ(unsigned char);

		for (int j=0; j<NumFilters; j++) 
		{
			COMMAND c;			
			memset(&c,0,sizeof(COMMAND));
			c.Filter   = READ(unsigned char);
			c.Layer    = READ(unsigned char);
			c.Operator = READ(unsigned char);
			c.OperatorMask[0] = (c.Operator & (1<<0))>0;
			c.OperatorMask[1] = (c.Operator & (1<<1))>0;
			c.OperatorMask[2] = (c.Operator & (1<<2))>0;
			c.OperatorMask[3] = (c.Operator & (1<<3))>0;
			c.Operator >>= 4;

			c.data = new char[datasizes[c.Filter]+5];
			memcpy(c.data,raw,datasizes[c.Filter]);

			#ifdef INCLUDE_TEX_Text
			if (c.Filter==TEXGEN_Text) 
			{
				TEXTTEMPLATE * tt = (TEXTTEMPLATE*)c.data;
				tt->Text = (char*)raw+7;
				 //ADD TEXTURE GENERATION HERE 
				memcpy(BufferLayer->Data,BufferTexture->Layers[c.Layer].Data,TextureSize*TextureSize*4);
				BufferTexture->PerformCommand(BufferLayer->Data,&c);
				BufferTexture->PerformFilter(BufferTexture->Layers[c.Layer].Data,BufferLayer->Data,&c);
				/**/

				raw+=7+strlen(tt->Text)+1;
			} 
			else
            #endif
			#ifdef INCLUDE_TEX_Jpeg
			if (c.Filter==TEXGEN_Jpeg) 
			{
				JPEGTEMPLATE * tt = (JPEGTEMPLATE*)c.data;
				tt->JPEGDataSize=*(int*)raw;
				tt->JPEGData=raw+4;
				BufferTexture->PerformCommand(BufferLayer->Data,&c);
				BufferTexture->PerformFilter(BufferTexture->Layers[c.Layer].Data,BufferLayer->Data,&c);
				raw+=4+tt->JPEGDataSize;
			} 
			else
            #endif
			#ifdef INCLUDE_TEX_MakeMaterial
			if (c.Filter!=TEXGEN_MakeMaterial)
			{
				// ADD TEXTURE GENERATION HERE 
				memcpy(BufferLayer->Data,BufferTexture->Layers[c.Layer].Data,TextureSize*TextureSize*4);
				BufferTexture->PerformCommand(BufferLayer->Data,&c);
				BufferTexture->PerformFilter(BufferTexture->Layers[c.Layer].Data,BufferLayer->Data,&c);
				/**/

				raw+=datasizes[c.Filter];
			}
			else
            #endif
			{
				raw++;

				bool *Used=(bool*)raw;//READ(bool);
				raw++;

				MATERIAL *Buffer=new MATERIAL;
				Buffer->Next=*MaterialList;
				*MaterialList=Buffer;
				Buffer->TextureID=ID;
				Buffer->Size=TextureSize;

				if (*Used)
				{
					Buffer->ImageData.Data=new RGBA[TextureSize*TextureSize];
					memcpy(Buffer->ImageData.Data,BufferTexture->Layers[c.Layer].Data,TextureSize*TextureSize*4);/**/

					/*#define SETTGAW(x) *(unsigned short*)(tgaheader+0x0C)=(x)
					#define SETTGAH(x) *(unsigned short*)(tgaheader+0x0E)=(x)

					unsigned char tgaheader[]={
									0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,
									0x20,0x00
					};

					char str[256];
					sprintf(str,"%04d.tga\0",TexCounter++);

					FILE *f=fopen(str,"w+b");
					SETTGAW(TextureSize);
					SETTGAH(TextureSize);
					fwrite(tgaheader,18,1,f);
					fwrite(Buffer->ImageData.Data,1,TextureSize*TextureSize*4,f);

					fclose(f);/**/
				}

				Buffer->SlotNumber=((MAKEMATERIALTEMPLATE*)c.data)->MaterialSlot;

				glGenTextures(1,&Buffer->ImageData.TextureHandle);
				glBindTexture(GL_TEXTURE_2D,Buffer->ImageData.TextureHandle);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
				gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA8,TextureSize,TextureSize,GL_RGBA,GL_UNSIGNED_BYTE,BufferTexture->Layers[c.Layer].Data);/**/
			}

			//delete c.data;

			PrecalcAnim((i/(float)(NumTextures)+1/(float)(NumTextures)*j/(float)(NumFilters))*0.95f);

			delete c.data;
		}
		//PrecalcAnim((i/(float)(NumTextures))*0.8f);
	}
	*f=raw;
}

MATERIAL *FindMaterial(int TextureID, int SlotNumber, MATERIAL *MaterialList)
{
	for (MATERIAL *m=MaterialList;m;m=m->Next)
		if (m->TextureID==TextureID && m->SlotNumber==SlotNumber) return m;
	return NULL;
}

int ImportPolySelections(void **f, PSelect **PolySelect)
{
	unsigned char * raw = (unsigned char*)*f;
	int PolySelectCount=READ(unsigned char);

	(*PolySelect)=new PSelect[PolySelectCount];
	memset((*PolySelect),0,sizeof(PSelect)*PolySelectCount);

	for (int i=0; i<PolySelectCount; i++)
	{
		(*PolySelect)[i].BrushNum=READ(unsigned char);
		(*PolySelect)[i].Brushes=new PVSelect[(*PolySelect)[i].BrushNum];
		memset((*PolySelect)[i].Brushes,0,sizeof(PVSelect)*(*PolySelect)[i].BrushNum);
		for (int k=0;k<(*PolySelect)[i].BrushNum;k++)
		{
			(*PolySelect)[i].Brushes[k].Brush=READ(unsigned char);
			(*PolySelect)[i].Brushes[k].Operator=READ(unsigned char);
			(*PolySelect)[i].Brushes[k].Param1=READ(unsigned short);
			(*PolySelect)[i].Brushes[k].Param2=READ(unsigned short);
			(*PolySelect)[i].Brushes[k].Param3=READ(unsigned short);
		}
	}
	*f=raw;
	return PolySelectCount;
}

GLuint SRCBlends[]= 
{
	GL_ZERO,
	GL_ONE,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_SRC_ALPHA_SATURATE
};

GLuint DSTBlends[]=
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA
};

int ImportScenes(void **f, SCENE **SceneList, MATERIAL **MaterialList, PSelect **PolySelections, int PolySelNum)
{
	unsigned char * raw = (unsigned char*)*f;
	int SceneNum=READ(unsigned char);

	(*SceneList)=new SCENE[SceneNum];
	memset(*SceneList,0,sizeof(SCENE)*SceneNum);

	for (int z=0; z<SceneNum; z++)
	{
		SCENE *s=&(*SceneList)[z];
		s->ID=READ(unsigned char);
		s->ColorDiscard=READ(bool);
		s->ObjectNum=READ(unsigned short);
		s->ObjectList=new OBJECT[s->ObjectNum];
		s->ZmaskList=glGenLists(1);
		s->NoZmaskList=glGenLists(1);
		memset(s->ObjectList,0,sizeof(OBJECT)*s->ObjectNum);

		for (int a=0; a<s->ObjectNum; a++)
		{
			OBJECT *o=&s->ObjectList[a];
			o->Primitive=READ(unsigned char);
			o->DisplayList=glGenLists(1);
			o->WireDisplayList=glGenLists(1);

#ifdef INCLUDE_OBJ_STORED
			if (o->Primitive==aDDict_STORED)
			{
				int VxNum=READ(short int);
				int l;
				for (l=0; l<VxNum; l++)
				{
					VECTOR3 p;
					p.x=readfloat(&raw,2);
					p.y=readfloat(&raw,2);
					p.z=readfloat(&raw,2);
					VECTOR3 n;
					n.x=READ(char); n.x/=127.0f;
					n.y=READ(char); n.y/=127.0f;
					n.z=READ(char); n.z/=127.0f;
					VECTOR2 t;
					t.x=READ(char); t.x/=127.0f;
					t.y=READ(char); t.y/=127.0f;
					o->AddVertex(p.x,p.y,p.z,t.x,t.y);
					o->VertexList[o->VertexNum-1].Normal=o->VertexList[o->VertexNum-1].CurrentNormal=n;
				}
				bool Textured=READ(bool);
				VxNum=READ(short int);
				for (l=0; l<VxNum; l++)
				{
					short int a,b,c;
					a=READ(short int);
					b=READ(short int);
					c=READ(short int);
					o->AddPolygon(a,b,c,aDDict_GOURAUDSHADE,1,1,1);
					POLYGON *p=&o->PolygonList[o->PolygonNum-1];
					p->ct[0]=p->t[0]=o->VertexList[p->v[0]].TextureCoordinate;
					p->ct[1]=p->t[1]=o->VertexList[p->v[1]].TextureCoordinate;
					p->ct[2]=p->t[2]=o->VertexList[p->v[2]].TextureCoordinate;
					if (Textured)
					{
						p->ct[0].x=READ(unsigned char);
						p->ct[0].y=READ(unsigned char);
						p->ct[1].x=READ(unsigned char);
						p->ct[1].y=READ(unsigned char);
						p->ct[2].x=READ(unsigned char);
						p->ct[2].y=READ(unsigned char);
						p->ct[0].x/=256.0f;
						p->ct[0].y/=256.0f;
						p->ct[1].x/=256.0f;
						p->ct[1].y/=256.0f;
						p->ct[2].x/=256.0f;
						p->ct[2].y/=256.0f;
					}
				}
			}
#endif

			OBJECTDATA od;

			o->Xtile=o->Ytile=1;
			o->OffsetX=o->OffsetY=0;
			o->Color.dw=0xffffffff;
			
			switch (o->Primitive)
			{
				#ifdef INCLUDE_OBJ_MAPXFORM
				case aDDict_MAPXFORM:
					{
						od=READ(OBJECTDATA);

						o->Wireframe=od.Wireframe;
						o->Textured=od.Textured;
						o->EnvMapped=od.Envmapped;
						o->NormalsInverted=od.NormalsInverted;
						o->XSwap=od.XSwap;
						o->YSwap=od.YSwap;
						o->Swap=od.Swap;
						o->ZMask=od.Zmask;
						o->Shading=od.Shading;
						o->AEpsilon=od.AEpsilon;
						o->DSTBlend=SRCBlends[od.SRCBlend];
						o->SRCBlend=DSTBlends[od.DSTBlend];

						if (od.XTile) { o->Xtile=READ(unsigned char); }
						if (od.YTile) { o->Ytile=READ(unsigned char); }
						if (od.Offx) { o->OffsetX=READ(unsigned char); o->OffsetX/=255.0f; }
						if (od.Offy) { o->OffsetY=READ(unsigned char); o->OffsetY/=255.0f; }
						o->TargetObjectID=READ(unsigned short);
						o->MapXformType=READ(unsigned char);
						o->MapXformColor=READ(unsigned char);
						int TextureID=READ(unsigned char);
						o->Material=FindMaterial(TextureID,od.TexSlot,*MaterialList);

						o->Param1=READ(unsigned char);//&o.Param1,1,1,f);						
						int PolyScelectNum=READ(unsigned char);//char k=o.Param2;
						o->Param2=PolyScelectNum;
						o->Param3=READ(char);//k=o.Param3;

						VECTOR3 ScaleVec=V3_Make(0,0,0);
						VECTOR3 AxisVec=V3_Make(0,0,0);
						float Angle=0;
						VECTOR3 MoveVec=V3_Make(0,0,0);

						switch (o->MapXformType)
						{
						case aDDict_SCALE:
							{
								ScaleVec.x=readfloat(&raw,2);
								ScaleVec.y=readfloat(&raw,2);
								ScaleVec.z=readfloat(&raw,2);
								//M_Scale(ScaleVec.x,ScaleVec.y,ScaleVec.z,o->ModelView);
								break;
							}
						case aDDict_ROTATE:
							{
								AxisVec.x=readfloat(&raw,2);
								AxisVec.y=readfloat(&raw,2);
								AxisVec.z=readfloat(&raw,2);
								Angle=readfloat(&raw,2);
								//M_Rotate(AxisVec.x,AxisVec.y,AxisVec.z,Angle,o->ModelView);
								break;
							}
						case aDDict_MOVE:
							{
								MoveVec.x=readfloat(&raw,2);
								MoveVec.y=readfloat(&raw,2);
								MoveVec.z=readfloat(&raw,2);
								//M_XPose(MoveVec.x,MoveVec.y,MoveVec.z,o->ModelView);
								break;
							}
						}

						OBJECT *MappedObject=s->FindObjectByID(o->TargetObjectID);
						CalculateObjectVertexWeights(MappedObject,
													  o->Material,
													  o->Xtile,
													  o->Ytile,
													  o->XSwap,
													  o->YSwap,
													  o->Swap,
													  o->OffsetX,
													  o->OffsetY);

						if (o->Param1)
						{
							SelectObjVertices(MappedObject,((*PolySelections)[PolyScelectNum]).Brushes,((*PolySelections)[PolyScelectNum]).BrushNum,o->Param3,o->Param1!=2);
						}
						else for (int x=0; x<MappedObject->VertexNum; x++) MappedObject->VertexList[x].Selected=true;

						CalculateMapTransform(o->MapXformType,o->MapXformColor,o->ModelView,MappedObject,MoveVec,ScaleVec,AxisVec,Angle);
						for (int x=0; x<MappedObject->VertexNum; x++) MappedObject->VertexList[x].Position=MappedObject->VertexList[x].MapTransformedPosition;

						break;
					}
                #endif
				case 11://aDDict_CLONE:
					{
						o->ID=READ(unsigned short);
						
						OBJECTDATA2 *od2=(OBJECTDATA2*)raw;
						raw++;

						readmatrix(&raw,2,o->ModelView,od2->Orientation,od2->Position);
						o->Param1=READ(unsigned char);
						o->Param2=(int)(new int[o->Param1]);
						o->Param3=(int)(new int[o->Param1]);
						for (int x=0; x<o->Param1; x++)
						{
							((int*)o->Param2)[x]=READ(unsigned short);
							((int*)o->Param3)[x]=(int)s->FindObjectByID(((int*)o->Param2)[x]);
						}
						break;
					}
                #ifdef INCLUDE_OBJ_BUTTERFLYSMOOTH
				case aDDict_BUTTERFLYSMOOTH:
                #endif
                #ifdef INCLUDE_OBJ_LINEARSMOOTH
				case aDDict_LINEARSMOOTH:
                #endif
				#ifdef INCLUDE_OBJ_MESHBLUR
				case aDDict_MESHBLUR:
                #endif
					{
						o->TargetObjectID=READ(short int);
						o->Param2=READ(unsigned char); //repeat count
						OBJECT *o2=s->FindObjectByID(o->TargetObjectID);
						for (int Repeat=0; Repeat<o->Param2; Repeat++)
						{
			    #ifdef INCLUDE_MESHSMOOTH
							if (o->Primitive!=103) o2->MeshSmooth(o->Primitive==101);
				#endif
				#ifdef INCLUDE_OBJ_MESHBLUR
							#ifdef INCLUDE_MESHSMOOTH
							else 
							#endif
								o2->MeshBlur();
                #endif
						}
					}
					break;
				default:
					o->ID=READ(unsigned short);
					od=READ(OBJECTDATA);

					OBJECTDATA2 *od2=(OBJECTDATA2*)raw;
					o->Backface=od2->BackFace;
					o->Backfront=od2->BackFront;
					raw++;

					o->AEpsilon=READ(unsigned char);

					readmatrix(&raw,2,o->ModelView,od2->Orientation,od2->Position);

					o->Wireframe=od.Wireframe;
					o->Textured=od.Textured;
					o->EnvMapped=od.Envmapped;
					o->NormalsInverted=od.NormalsInverted;
					o->XSwap=od.XSwap;
					o->YSwap=od.YSwap;
					o->Swap=od.Swap;
					o->ZMask=od.Zmask;
					o->Shading=od.Shading;
					//o->AEpsilon=od.AEpsilon;
					o->DSTBlend=SRCBlends[od.SRCBlend];
					o->SRCBlend=DSTBlends[od.DSTBlend];

					if (od.XTile) 
					{
						o->Xtile=READ(unsigned char);
					}
					if (od.YTile) 
					{
						o->Ytile=READ(unsigned char);
					}
					if (od.Offx)
					{
						o->OffsetX=READ(unsigned char);
						o->OffsetX/=255.0f;
					}
					if (od.Offy)
					{
						READ(unsigned char);
						o->OffsetY/=255.0f;
					}
					if (od.Red) 
					{
						o->Color.r=READ(unsigned char);
					}
					if (od.Green) 
					{
						o->Color.g=READ(unsigned char);
					}
					if (od.Blue) 
					{
						o->Color.b=READ(unsigned char);
					}
					if (od.Alpha) 
					{
						o->Color.a=READ(unsigned char);
					}

					if (od.Textured) 
					{
						int TextureID=READ(unsigned char);
						o->Material=FindMaterial(TextureID,od.TexSlot,*MaterialList);
					}
					if (od.Envmapped) 
					{
						int EnvMapID=READ(unsigned char);
						o->EnvMap=FindMaterial(EnvMapID,od.EnvSlot,*MaterialList);
					}

					switch (o->Primitive)
					{
#ifdef INCLUDE_OBJ_SPHERE
					case aDDict_SPHERE:
#endif
#ifdef INCLUDE_OBJ_PLANE
					case aDDict_PLANE:
#endif
#ifdef INCLUDE_OBJ_CIRCLE
					case aDDict_CIRCLE:
#endif
						{
							o->Param1=READ(unsigned char);
							o->Param2=READ(unsigned char);
#ifdef INCLUDE_OBJ_CIRCLE
							//if (o->Primitive==aDDict_CIRCLE)
							//o->NormalsInverted=!o->NormalsInverted;
#endif
							break;
						}
#ifdef INCLUDE_OBJ_LINE
					case aDDict_LINE:
						o->Param1=READ(unsigned char);
						break;
#endif
#ifdef INCLUDE_OBJ_CONE
					case aDDict_CONE:
#endif
#ifdef INCLUDE_OBJ_CYLINDER
					case aDDict_CYLINDER:
						o->Param1=READ(unsigned char);
						o->Param2=READ(unsigned char);
						o->Param3=READ(unsigned char);
						break;
#endif
#ifdef INCLUDE_OBJ_ARC
					case aDDict_ARC:
						o->Param1=READ(unsigned char);
						o->Param2=READ(unsigned short);
						break;
#endif
#ifdef INCLUDE_OBJ_LOFT
					case aDDict_LOFT:
						o->Param1=READ(unsigned short);
						o->Param1=(int)s->FindObjectByID(o->Param1);
						o->Param2=READ(unsigned short);
						o->Param2=(int)s->FindObjectByID(o->Param2);
						break;
#endif
#ifdef INCLUDE_OBJ_FUR
					case aDDict_FUR:
						o->Param1=READ(unsigned short);
						o->Param2=READ(unsigned char);
						o->Param3=READ(unsigned short);
						o->Param4=READ(unsigned short);
						o->Param5=(int)s->FindObjectByID(o->Param4);
						break;
#endif
#ifdef INCLUDE_OBJ_SUPERSHAPE
					case aDDict_SUPERSHAPE:
						{
							SUPERSHAPE *s=new SUPERSHAPE;
							o->Param1=(int)s;

							s->SuperShape1.mfloat=READ(unsigned char);
							s->SuperShape1.mint=READ(unsigned char);

							s->SuperShape1.n1=readfloat(&raw,2);
							s->SuperShape1.n2=readfloat(&raw,2);
							s->SuperShape1.n3=readfloat(&raw,2);
							s->SuperShape1.a=readfloat(&raw,2);
							s->SuperShape1.b=readfloat(&raw,2);

							s->SuperShape2.mfloat=READ(unsigned char);
							s->SuperShape2.mint=READ(unsigned char);
							s->SuperShape2.n1=readfloat(&raw,2);
							s->SuperShape2.n2=readfloat(&raw,2);
							s->SuperShape2.n3=readfloat(&raw,2);
							s->SuperShape2.a=readfloat(&raw,2);
							s->SuperShape2.b=readfloat(&raw,2);

							s->Sphere=READ(unsigned char);
							s->Trad0=readfloat(&raw,2);
							s->TradPhi=readfloat(&raw,2);
							s->RadFunc=READ(unsigned char);
							s->Rada=readfloat(&raw,2);
							s->Radb=readfloat(&raw,2);
							s->VertFunc=READ(unsigned char);
							s->Verta=readfloat(&raw,2);
							s->Rangex1=READ(short);
							s->Rangey1=READ(short);
							s->Rangex2=READ(short);
							s->Rangey2=READ(short);

							s->Xres=READ(unsigned char);
							s->Yres=READ(unsigned char);
						}

						break;
#endif
					//case -25472: break;
					}
					GeneratePrimitive(o);
			}
			
			//PrecalcAnim(0.8f+(z/(float)(SceneNum)+1/(float)(SceneNum)*a/(float)(s->ObjectNum))*0.1f);
		}
		s->SetupDummies();
		PrecalcAnim(0.95f+(z/(float)(SceneNum))*0.05f);
	}
	*f=raw;
	return SceneNum;
}

void ImportSplineFloat(unsigned char **f, SPLINE **s)
{
	(*s)=new SPLINE;
	unsigned char * raw = (unsigned char*)*f;
	int KeyNum=READ(unsigned char);
	(*s)->Interpolation=READ(unsigned char);
	(*s)->Keys=new Key[KeyNum+1];
	memset((*s)->Keys,0,sizeof(Key)*(KeyNum+1));
	(*s)->KeyNum=KeyNum;

	for (int x=0; x<KeyNum; x++)
	{
		(*s)->Keys[x].Time=readfloat(&raw,2);
		(*s)->Keys[x].Value=readfloat(&raw,2);
	}
	(*s)->InitVectors();
	*f=raw;
}

void ImportSplineColor(unsigned char **f, SPLINE **s)
{
	(*s)=new SPLINE;
	unsigned char * raw = (unsigned char*)*f;
	int KeyNum=READ(unsigned char);
	(*s)->Interpolation=READ(unsigned char);
	(*s)->Keys=new Key[KeyNum+1];
	memset((*s)->Keys,0,sizeof(Key)*(KeyNum+1));
	(*s)->KeyNum=KeyNum;

	for (int x=0; x<KeyNum; x++)
	{
		(*s)->Keys[x].Time=readfloat(&raw,2);
		(*s)->Keys[x].Value=READ(unsigned char);
	}
	*f=raw;
}

SCENE *FindScene(SCENE *SceneList, int ID, int SceneNum)
{
	for (int x=0; x<SceneNum; x++)
		if (SceneList[x].ID==ID) return &SceneList[x];
	return NULL;
}

WORLD *FindWorld(WORLD *WorldList, int ID, int WorldNum)
{
	for (int x=0; x<WorldNum; x++)
		if (WorldList[x].ID==ID) return &WorldList[x];
	return NULL;
}

int ImportWorlds(void **f, WORLD **WorldList, SCENE **SceneList, MATERIAL **MaterialList, int SceneNum)
{
	unsigned char * raw = (unsigned char*)*f;
	int WorldNum=READ(unsigned char);
#ifdef INCLUDE_PARTICLE_ENGINE
	MaxParticleNum=0;
#endif

	(*WorldList)=new WORLD[WorldNum];
	memset((*WorldList),0,sizeof(WORLD)*WorldNum);

	for (int z=0; z<WorldNum; z++)
	{
		WORLD *w=&(*WorldList)[z];
		w->ID=READ(unsigned short);

		w->Fog=READ(bool);
		if (w->Fog) 
		{
			RGBA fog=READ(RGBA);
			w->FogCol[0]=fog.c[0];
			w->FogCol[1]=fog.c[1];
			w->FogCol[2]=fog.c[2];
			w->FogCol[3]=fog.c[3];
			w->FogStart=readfloat(&raw,2);
			w->FogEnd=readfloat(&raw,2);
		}

		w->Lighting=READ(bool);
		if (w->Lighting)
		{
			unsigned char LightNum=READ(unsigned char);
			for (int x=0; x<LightNum; x++) 
				{
					w->Lights[x].Lit=true;
					w->Lights[x].Identifier=READ(unsigned char);
					w->Lights[x].Identifier+=GL_LIGHT0;
					w->Lights[x].Linear_Attenuation=1;
					int LightType=READ(unsigned char);

					int k;
					for (k=0; k<3; k++)
					{
						int buf=READ(unsigned char);
						w->Lights[x].Ambient[k]=buf/255.0f;
					}
					for (k=0; k<3; k++)
					{
						int buf=READ(unsigned char);
						w->Lights[x].Color[k]=buf/255.0f;
					}
					for (k=0; k<4; k++) w->Lights[x].Position[k]=readfloat(&raw,2);
					if (w->Lights[x].Position[3]!=0)
					{
						w->Lights[x].Constant_Attenuation=readfloat(&raw,2);
						w->Lights[x].Linear_Attenuation=readfloat(&raw,2);
						w->Lights[x].Quadratic_Attenuation=readfloat(&raw,2);
					}
					w->Lights[x].Spot_Cutoff=readfloat(&raw,2);
					if (w->Lights[x].Spot_Cutoff!=180)
					{
						for (k=0; k<3; k++) w->Lights[x].Spot_Direction[k]=readfloat(&raw,2);
						w->Lights[x].Spot_Exponent=readfloat(&raw,2);
					}
				}
		}
		w->CamNum=READ(unsigned char);
		w->CamCapacity=READ(unsigned char);
		w->CamAnims=new CAMSPLINES[w->CamCapacity];
		int x;
		for (x=0; x<w->CamNum; x++)
		{
			int a=w->CamAnims[x].CamID=READ(unsigned char);			
			ImportSplineFloat(&raw,&w->CamAnims[x].Eyex);
			ImportSplineFloat(&raw,&w->CamAnims[x].Eyey);
			ImportSplineFloat(&raw,&w->CamAnims[x].Eyez);
			ImportSplineFloat(&raw,&w->CamAnims[x].Trgx);
			ImportSplineFloat(&raw,&w->CamAnims[x].Trgy);
			ImportSplineFloat(&raw,&w->CamAnims[x].Trgz);
			ImportSplineFloat(&raw,&w->CamAnims[x].Roll);
			ImportSplineFloat(&raw,&w->CamAnims[x].Fov);
		}
		w->AnimNum=READ(unsigned char);//&w->AnimNum,1,1,f);
		w->AnimCapacity=READ(unsigned char);
		w->Animations=new ANIMDATA[w->AnimCapacity];

		//anim header

		w->ObjectNum=READ(unsigned short);
		w->ObjectList=new WORLDOBJECT[w->ObjectNum];
		memset(w->ObjectList,0,sizeof(WORLDOBJECT)*w->ObjectNum);

		for (x=0; x<w->ObjectNum; x++)
		{
			WORLDOBJECT *o=&w->ObjectList[x];
			o->Color.dw=0xffffffff;
			o->ID=READ(unsigned short);
			o->ParentID=READ(signed short);
			o->Parent=w->FindObjectByID(o->ParentID);
			o->Primitive=READ(unsigned char);
		
			int SceneID=READ(signed char); //o->Data-ba kell a scenere pointer
			o->TargetID=SceneID;
			o->Data=FindScene(*SceneList,SceneID,SceneNum);

			WORLDOBJECTDATA od=READ(WORLDOBJECTDATA);
			o->Textured=od.Textured;
			o->PosData.Scale=V3_Make(1,1,1);
			o->PosData.Quaternion.s=1;

			if (od.Textured) 
			{ 
				int Material=READ(unsigned char);
				o->Material=FindMaterial(Material,od.TexSlot,*MaterialList);
			}
			if (od.Posx) { o->PosData.Pos.x=readfloat(&raw,2); }
			if (od.Posy) { o->PosData.Pos.y=readfloat(&raw,2); }
			if (od.Posz) { o->PosData.Pos.z=readfloat(&raw,2); }
			if (od.Sclx) { o->PosData.Scale.x=readfloat(&raw,2); }
			if (od.Scly) { o->PosData.Scale.y=readfloat(&raw,2); }
			if (od.Sclz) { o->PosData.Scale.z=readfloat(&raw,2); }
			if (od.Quat)
			{
				o->PosData.Quaternion.s=readfloat(&raw,2);
				o->PosData.Quaternion.v.x=readfloat(&raw,2);
				o->PosData.Quaternion.v.y=readfloat(&raw,2);
				o->PosData.Quaternion.v.z=readfloat(&raw,2);
			}

#ifdef INCLUDE_PARTICLE_ENGINE
			switch (o->Primitive)
			{
			case aDDict_SPHEREEMITTER:
			case aDDict_CUBEEMITTER:
			case aDDict_CYLINDEREMITTER:
				{
					o->EmitterData=new EMITTERDATA;
					memset(o->EmitterData,0,sizeof(EMITTERDATA));
					o->EmitterData->DefaultAge=READ(unsigned short);
					o->EmitterData->AgeChaos=READ(unsigned short);
					o->EmitterData->ParticlesPerFrame=readfloat(&raw,2);
					o->EmitterData->MaxParticles=READ(unsigned short);
					o->EmitterData->FixedUp=READ(bool);
					o->ZMask=READ(bool);
					o->AEpsilon=READ(unsigned char);
					o->EmitterData->Particles=new PARTICLE[o->EmitterData->MaxParticles];
					o->EmitterData->ParticleNumBuffer=o->EmitterData->ParticlesPerFrame;
					MaxParticleNum=max(MaxParticleNum,o->EmitterData->MaxParticles);

					o->DSTBlend=SRCBlends[od.SRCBlend];
					o->SRCBlend=DSTBlends[od.DSTBlend];
					o->EmitterData->Tail=od.Tail;
					o->EmitterData->Head=od.Head;
					o->EmitterData->TailLength=od.TailRes1+1;
					o->EmitterData->TailLength2=od.TailRes2+1;					

					memset(o->EmitterData->Particles,0,sizeof(PARTICLE)*o->EmitterData->MaxParticles);
					if (od.Tail) for (int x=0; x<o->EmitterData->MaxParticles; x++) 
					{
						o->EmitterData->Particles[x].Tail=new VECTOR3[256];
						memset(o->EmitterData->Particles[x].Tail,0,sizeof(VECTOR3)*256);
					}

					//if (od.ObjectHead) 
					
					int d;
					for (d=0; d<5; d++)
					{
						o->EmitterData->Color1[d]=READ(unsigned char);
					}
					for (d=0; d<5; d++)
					{
						o->EmitterData->Color2[d]=READ(unsigned char);
					}
					o->EmitterData->Param1=readfloat(&raw,2);
					o->EmitterData->Param2=readfloat(&raw,2);
					o->EmitterData->Size=readfloat(&raw,2);
					o->EmitterData->Rotspeed=readfloat(&raw,2);
					o->EmitterData->RotChaos=readfloat(&raw,2);
					o->EmitterData->CamFade=readfloat(&raw,2);

					o->EmitterData->Speed=readfloat(&raw,2);
					o->EmitterData->SpeedChaos=readfloat(&raw,2);
					o->EmitterData->DirChaos=readfloat(&raw,2);
					o->EmitterData->Dir.x=readfloat(&raw,2);
					o->EmitterData->Dir.y=readfloat(&raw,2);
					o->EmitterData->Dir.z=readfloat(&raw,2);
					o->EmitterData->RandRotate=READ(bool);

					break;
				}
			case aDDict_GRAVITY:
			case aDDict_PLANEDEFLECTOR:
			case aDDict_SPHEREDEFLECTOR:
					o->EmitterData=new EMITTERDATA;
					memset(o->EmitterData,0,sizeof(EMITTERDATA));
					o->EmitterData->Param1=readfloat(&raw,2);
					o->EmitterData->Param2=readfloat(&raw,2);
				break;				
			}
#endif

			o->Animations=new DEFAULTOBJECTSPLINES[w->AnimCapacity];
			memset(o->Animations,0,sizeof(DEFAULTOBJECTSPLINES)*w->AnimCapacity);
			o->AnimNum=w->AnimNum;
			o->AnimCapacity=w->AnimCapacity;
			for (int y=0; y<w->AnimNum; y++)
			{
				//int ID=w->Animations[y].AnimID=READ(unsigned char);
				switch (o->Primitive)
				{
				case aDDict_SPHEREEMITTER:
				case aDDict_CUBEEMITTER:
				case aDDict_CYLINDEREMITTER:
					ImportSplineFloat(&raw,&o->Animations[y].Prtfrme);
					ImportSplineFloat(&raw,&o->Animations[y].Size);
				default:
					ImportSplineFloat(&raw,&o->Animations[y].Red);
					ImportSplineFloat(&raw,&o->Animations[y].Green);
					ImportSplineFloat(&raw,&o->Animations[y].Blue);
					ImportSplineFloat(&raw,&o->Animations[y].Alpha);
					ImportSplineFloat(&raw,&o->Animations[y].Posx);
					ImportSplineFloat(&raw,&o->Animations[y].Posy);
					ImportSplineFloat(&raw,&o->Animations[y].Posz);
					ImportSplineFloat(&raw,&o->Animations[y].Sclx);
					ImportSplineFloat(&raw,&o->Animations[y].Scly);
					ImportSplineFloat(&raw,&o->Animations[y].Sclz);
					ImportSplineFloat(&raw,&o->Animations[y].Rotx);
					ImportSplineFloat(&raw,&o->Animations[y].Roty);
					ImportSplineFloat(&raw,&o->Animations[y].Rotz);
					ImportSplineFloat(&raw,&o->Animations[y].Rotw);
					ImportSplineColor(&raw,&o->Animations[y].AnimID);
					ImportSplineFloat(&raw,&o->Animations[y].AnimTime);
					Spline_QuaternionGetVectors(o->Animations[y].Rotx,o->Animations[y].Roty,o->Animations[y].Rotz,o->Animations[y].Rotw);
				}
			}

			//PrecalcAnim(0.9f+(z/(float)(WorldNum)+1/(float)(WorldNum)*x/(float)(w->ObjectNum))*0.1f);
		}

		for (int y=0; y<w->AnimNum; y++) 
		{
			w->Animations[y].AnimID=READ(unsigned char);
		}

#ifdef INCLUDE_PARTICLE_ENGINE
		w->CalculateParticleObjectMatrices();
#endif
		w->CalculateObjectMatrices();
#ifdef INCLUDE_PARTICLE_ENGINE
		w->CalculateParticles(0);
#endif
		//PrecalcAnim(0.975f+(z/(float)(WorldNum))*0.025f);
	}

	for (int y=0; y<WorldNum; y++)
	{
		WORLD *w=&(*WorldList)[y];
		for (int x=0; x<(*WorldList)[y].ObjectNum; x++)
			if (w->ObjectList[x].Primitive==aDDict_SUBSCENE) 
				w->ObjectList[x].Data=FindWorld(*WorldList,w->ObjectList[x].TargetID,WorldNum);
	}

	*f=raw;
	return WorldNum;
}

int eventsizes[] = {
 0,
 sizeof(LAYER2DTEMPLATE),
 sizeof(PARTICLETEMPLATE),
 sizeof(CLEARTEMPLATE),
 sizeof(R2TXTTEMPLATE),
 sizeof(FEEDBACKTEMPLATE),
 sizeof(BLURFXTEMPLATE),
 sizeof(DISPLAY3DTEMPLATE),
 sizeof(PARTICLERESETTEMPLATE),
 sizeof(CONTRASTFXTEMPLATE),
 0,
 sizeof(CRYSTALIZEFXTEMPLATE),
 sizeof(GRIDFILLERTEMPLATE),
 sizeof(CAVETEMPLATE),
 sizeof(SHAKETEMPLATE),
 sizeof(MRCTEMPLATE),
 sizeof(FEEDBACKTEMPLATE),
};

EFFECTCALLBACK EffectProcedures[] = {
#ifdef INCLUDE_EVENT_NOEFFECT
	NoEffect,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_2DLAYER
	Layer2d,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_PARTICLECALC
	CalcParticle,
#else
		0,
#endif	
#ifdef INCLUDE_EVENT_CLEAR
	Clear,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_RENDERTOTEXTURE
	R2Txt,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_FEEDBACK
	Feedback,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_BLUR
	Blur,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_3DSCENE
	Display3d,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_PARTICLERESET
	ParticleReset,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_CONTRAST
	Contrast,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_FUNKENDSCROLL
	EndScroll,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_CRYSTALIZE
	Crystalize,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_GRIDFILLER
	Gridfiller,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_CAVE
	Cave,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_CAMSHAKE
	CamShake,
#else
		0,
#endif
#ifdef INCLUDE_EVENT_MRCEFFECT
	MrcEffekt,
#else
		0,
#endif

#ifdef INCLUDE_EVENT_FEEDBACK2
	Feedback2,
#else
		0,
#endif

};

int ImportEvents(void **f, EVENT **EventList, WORLD **WorldList, SCENE **SceneList, MATERIAL **MaterialList, int WorldNum)
{
	unsigned char * raw = (unsigned char*)*f;
	int EventNum=READ(unsigned short);
	(*EventList)=new EVENT[EventNum];
	memset(*EventList,0,sizeof(EVENT)*EventNum);

	for (int x=0; x<EventNum; x++)
	{
		(*EventList)[x].StartFrame=READ(unsigned short);
		(*EventList)[x].EndFrame=READ(unsigned short);
		(*EventList)[x].Pass=READ(unsigned char);
		(*EventList)[x].EventType=READ(unsigned char);
		(*EventList)[x].Effect=EffectProcedures[(*EventList)[x].EventType];

		(*EventList)[x].ex1=READ(short);
		(*EventList)[x].ex2=READ(short);
		(*EventList)[x].ey1=READ(short);
		(*EventList)[x].ey2=READ(short);
		(*EventList)[x].sx1=READ(short);
		(*EventList)[x].sx2=READ(short);
		(*EventList)[x].sy1=READ(short);
		(*EventList)[x].sy2=READ(short);

		switch ((*EventList)[x].EventType)
		{
			case 1: //GRIDFILLER
				{					
					(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
					memcpy((*EventList)[x].EffectData,raw,eventsizes[(*EventList)[x].EventType]-4);
					raw+=eventsizes[(*EventList)[x].EventType]-4;
					int i=0;
					memcpy(&i,raw,3);
					raw+=3;
					int k=0;
					memcpy(&k,raw,1);
					raw++;
					((LAYER2DTEMPLATE*)(*EventList)[x].EffectData)->Texture=FindMaterial(i,k,*MaterialList);
				}
			break;
			case 2: //PARTICLE RENDER
				{
					(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
					memset((*EventList)[x].EffectData,0,eventsizes[(*EventList)[x].EventType]);
					int WorldID=READ(unsigned char);
					((PARTICLETEMPLATE*)(*EventList)[x].EffectData)->World=FindWorld(*WorldList,WorldID,WorldNum);
					((PARTICLETEMPLATE*)(*EventList)[x].EffectData)->CamID=READ(unsigned char);
					((PARTICLETEMPLATE*)(*EventList)[x].EffectData)->AnimID=READ(unsigned char);
					((PARTICLETEMPLATE*)(*EventList)[x].EffectData)->AnimStart=readfloat(&raw,2);
					((PARTICLETEMPLATE*)(*EventList)[x].EffectData)->AnimEnd=readfloat(&raw,2);
				}
			break;
			case 7: //3D SCENE
				{					
					(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
					memset((*EventList)[x].EffectData,0,eventsizes[(*EventList)[x].EventType]);
					int WorldID=READ(unsigned char);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->World=FindWorld(*WorldList,WorldID,WorldNum);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->CamID=READ(unsigned char);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->AnimID=READ(unsigned char);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->CamStart=readfloat(&raw,2);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->CamEnd=readfloat(&raw,2);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->AnimStart=readfloat(&raw,2);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->AnimEnd=readfloat(&raw,2);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->DontSaveCam=READ(bool);
				}
			break;
			case 8: //PARTICLE RESET
				{					
					(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
					memset((*EventList)[x].EffectData,0,eventsizes[(*EventList)[x].EventType]);
					int WorldID=READ(unsigned char);
					((DISPLAY3DTEMPLATE*)(*EventList)[x].EffectData)->World=FindWorld(*WorldList,WorldID,WorldNum);
				}
			break;
			case 12: //GRIDFILLER
				{					
					(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
					memcpy((*EventList)[x].EffectData,raw,eventsizes[(*EventList)[x].EventType]-4);
					raw+=eventsizes[(*EventList)[x].EventType]-4;
					int i=0;
					memcpy(&i,raw,3);
					raw+=3;
					int k=0;
					memcpy(&k,raw,1);
					raw++;
					((GRIDFILLERTEMPLATE*)(*EventList)[x].EffectData)->Texture=FindMaterial(i,k,*MaterialList);
				}
			break;
#ifdef INCLUDE_EVENT_CAVE 
			case 13: //CAVE
				{					
					(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
					memcpy((*EventList)[x].EffectData,raw,eventsizes[(*EventList)[x].EventType]-4);
					raw+=eventsizes[(*EventList)[x].EventType]-4;
					int i=0;
					memcpy(&i,raw,3);
					raw+=3;
					int k=0;
					memcpy(&k,raw,1);
					raw++;
					((CAVETEMPLATE*)(*EventList)[x].EffectData)->Texture=FindMaterial(i,k,*MaterialList);
					CAVETEMPLATE dat=*(CAVETEMPLATE*)(*EventList)[x].EffectData;
					InitTunnels(dat.RandSeed,dat.Data[0]/25.5f,dat.Data[1]/25.5f,dat.Data[2]/255.0f,dat.Data[3]/255.0f,dat.Data[4]/25.5f,dat.Data[5]/25.5f,dat.Data[6]/255.0f,dat.Data[7]/255.0f,dat.Data[8]/255.0f);
					for (int x=0; x<100; x++) CalculateCave(dat.Solid,dat.WireFrame,dat.skip);
				}
			break;
#endif
			default:
				(*EventList)[x].EffectData=new char[eventsizes[(*EventList)[x].EventType]];
				memcpy((*EventList)[x].EffectData,raw,eventsizes[(*EventList)[x].EventType]);
				raw+=eventsizes[(*EventList)[x].EventType];
		}
	}
	*f=raw;

	return EventNum;
	LastTime=timeGetTime();
}


int SceneNum=0;
int PolySelNum=0;
int WorldNum=0;

int LoadDataFile(void **Data, MATERIAL **MaterialList, PSelect **PolySelections, SCENE **SceneList, WORLD **WorldList, EVENT **EventList, int LOD)
{
	precalcdelaytimer=timeGetTime()-101;
	int EventNum=0;
	ImportTextures(Data,MaterialList,LOD);
	PolySelNum=ImportPolySelections(Data,PolySelections);
	SceneNum=ImportScenes(Data,SceneList,MaterialList,PolySelections,PolySelNum);
	WorldNum=ImportWorlds(Data,WorldList,SceneList,MaterialList,SceneNum);
#ifdef INCLUDE_PARTICLE_ENGINE
	InitParticleIndexBuffer();
#endif
	EventNum=ImportEvents(Data,EventList,WorldList,SceneList,MaterialList,WorldNum);
	return EventNum;
}