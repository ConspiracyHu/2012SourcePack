#include "MinimalExport.h"
#include "IntroEditorEffects.h"
#include "GuiInterface.h"

#ifndef Public_Release

unsigned long masks[5];

bool *UsedTextures,*UsedMaterials;
bool *UsedScenes;
bool *UsedWorlds;

void CheckWorld(WORLD *w)
{
	UsedWorlds[w->ID]=true;
	w->Used=true;
	for (int x=0; x<w->ObjectNum; x++)
	{
		if (w->ObjectList[x].Primitive==aDDict_SPHEREEMITTER || w->ObjectList[x].Primitive==aDDict_CUBEEMITTER || w->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER)
			if (w->ObjectList[x].Textured) 
			{
				UsedTextures[w->ObjectList[x].Material->TextureID]=true;
				UsedMaterials[w->ObjectList[x].Material->TextureID*8+w->ObjectList[x].Material->SlotNumber]=true;
			}
		if (w->ObjectList[x].Primitive==aDDict_SCENE)
		{
			SCENE *s=((SCENE*)w->ObjectList[x].Data);
			UsedScenes[s->ID]=true;
			s->Used=true;
			for (int z=0; z<s->ObjectNum; z++)
			{
				if (s->ObjectList[z].Primitive==aDDict_MAPXFORM || s->ObjectList[z].Textured)
				{
					UsedTextures[s->ObjectList[z].Material->TextureID]=true;
					UsedMaterials[s->ObjectList[z].Material->TextureID*8+s->ObjectList[z].Material->SlotNumber]=true;
				}
				if (s->ObjectList[z].EnvMapped) 
				{
					UsedTextures[s->ObjectList[z].EnvMap->TextureID]=true;
					UsedMaterials[s->ObjectList[z].EnvMap->TextureID*8+s->ObjectList[z].EnvMap->SlotNumber]=true;
				}
			}
		}				
		if (w->ObjectList[x].Primitive==aDDict_SUBSCENE) 
		{
			for (int z=0; z<w->AnimNum; z++)
			{
				if (w->Animations[z].Used)
				{
					for (int k=0; k<w->ObjectList[x].Animations[z].AnimID->KeyNum; k++)
					{
						((WORLD*)w->ObjectList[x].Data)->Animations[k].Used=true;
					}
				}
			}
			CheckWorld((WORLD*)w->ObjectList[x].Data);
		}
	}
}

void AnalyzeProject(bool analyze)
{
	SCENE *s;
	WORLD *w;
	TEXTURE *t;
	int TexNum=0,SceneNum=0,WorldNum=0;
	for (t=TextureList;t;t=t->Next) TexNum=max(TexNum,t->Id);
	for (s=Scenes;s;s=s->Next) SceneNum=max(SceneNum,s->ID);
	for (w=Worlds;w;w=w->Next) WorldNum=max(WorldNum,w->ID);

	TexNum++;
	SceneNum++;
	WorldNum++;

	UsedTextures=new bool[TexNum]; memset(UsedTextures,0,TexNum);
	UsedMaterials=new bool[TexNum*8]; memset(UsedMaterials,0,TexNum*8);
	UsedScenes=new bool[SceneNum]; memset(UsedScenes,0,SceneNum);
	UsedWorlds=new bool[WorldNum]; memset(UsedWorlds,0,WorldNum);

	for (s=Scenes; s; s=s->Next)	s->Used=false;
	for (w=Worlds; w; w=w->Next)
	{
		w->Used=false;
		int x;
		for (x=0; x<w->AnimNum; x++) w->Animations[x].Used=false;
		for (x=0; x<w->CamNum; x++)	w->CamAnims[x].Used=false;
	}

	if (analyze)
	{
		EVENT *e;
		for (e=EventList; e; e=e->Next)
		{
			if (e->EventType==7) 
			{
				WORLD *w=((DISPLAY3DTEMPLATE*)e->EffectData)->World;
				DISPLAY3DTEMPLATE *d=(DISPLAY3DTEMPLATE*)e->EffectData;
				w->CamAnims[d->CamID].Used=true;
				w->Animations[d->AnimID].Used=true;
			}
		}

		for (e=EventList; e; e=e->Next)
		{		
			if (e->EventType==7) 
			{
				WORLD *w=((DISPLAY3DTEMPLATE*)e->EffectData)->World;
				DISPLAY3DTEMPLATE *d=(DISPLAY3DTEMPLATE*)e->EffectData;
				CheckWorld(w);
			}

			if (e->EventType==2) 
			{
				WORLD *w=((PARTICLETEMPLATE*)e->EffectData)->World;
				PARTICLETEMPLATE *d=(PARTICLETEMPLATE*)e->EffectData;

				w->CamAnims[d->CamID].Used=true;
				w->Animations[d->AnimID].Used=true;
				CheckWorld(w);
			}

			if (e->EventType==1)
			{
				LAYER2DTEMPLATE *d=(LAYER2DTEMPLATE*)e->EffectData;
				if (d->Textured) 
				{
					UsedTextures[d->Texture->TextureID]=true;				
					UsedMaterials[d->Texture->TextureID*8+d->Texture->SlotNumber]=true;
				}
			}

			if (e->EventType==12)
			{
				GRIDFILLERTEMPLATE *d=(GRIDFILLERTEMPLATE*)e->EffectData;
				if (d->Textured) 
				{
					UsedTextures[d->Texture->TextureID]=true;				
					UsedMaterials[d->Texture->TextureID*8+d->Texture->SlotNumber]=true;
				}
			}

			if (e->EventType==13)
			{
				CAVETEMPLATE *d=(CAVETEMPLATE*)e->EffectData;
				if (d->Textured) 
				{
					UsedTextures[d->Texture->TextureID]=true;				
					UsedMaterials[d->Texture->TextureID*8+d->Texture->SlotNumber]=true;
				}
			}
		
		}
	}
	else
	{
		memset(UsedTextures,1,TexNum);
		memset(UsedMaterials,1,TexNum*8);
		memset(UsedScenes,1,SceneNum);
		memset(UsedWorlds,1,WorldNum);
	}
}

void writefloat(FILE *f, float s, unsigned char bytenum)
{
	masks[0]=0x00000000;
	masks[1]=0xff000000;
	masks[2]=0xffff0000;
	masks[3]=0xffffff00;
	masks[4]=0xffffffff;
	unsigned long w;
	w=*(unsigned long*)(&s);
	w=(w & masks[bytenum]) >> ((4-bytenum)*8);
	fwrite(&w,bytenum,1,f);
}

void writematrix(FILE *f, MATRIX m)
{
	for (int y=0; y<4; y++)
		for (int x=0; x<3; x++)
			writefloat(f,m[x][y],2);
}

void writematrix(FILE *f, MATRIX m, bool orient, bool pos)
{
	if (orient)
		for (int y=0; y<3; y++)
			for (int x=0; x<3; x++)
				writefloat(f,m[x][y],2);
	if (pos)
	 for (int x=0; x<3; x++)
	  writefloat(f,m[x][3],2);
}

void ExportHeader(FILE *f)
{
}

void ExportTextures(FILE *f)
{
	unsigned short Count=0;
	TEXTURE *t;
	for (t=TextureList; t; t=t->Next) { if (UsedTextures[t->Id]) Count++; }
	fwrite(&Count,1,1,f);
	for (t=TextureList; t; t=t->Next) if (UsedTextures[t->Id])
	{
		//fwrite(&t->XRes,2,1,f);
		char z=0;
		switch (t->XRes)
		{
		case 32: z=0; break;
		case 64: z=1; break;
		case 128: z=2; break;
		case 256: z=3; break;
		case 512: z=4; break;
		case 1024: z=5; break;
		}
		fwrite(&z,1,1,f);

		bool *Map=new bool[t->CommandNumber];
		bool Found[8];
		memset(Map,0,t->CommandNumber);
		memset(Found,0,8);
		int i;
		for (i=t->CommandNumber-1; i>=0; i--)
			if (t->Commands[i].Filter==TEXGEN_MakeMaterial)
			{
				if (Found[((MAKEMATERIALTEMPLATE*)t->Commands[i].data)->MaterialSlot]) Map[i]=true;
				Found[((MAKEMATERIALTEMPLATE*)t->Commands[i].data)->MaterialSlot]=true;
			}
		int Counter=0;
		for (i=0; i<t->CommandNumber; i++) if (!Map[i]) Counter++;

		//fwrite(&t->CommandNumber,1,1,f);
		fwrite(&Counter,1,1,f);
		fwrite(&t->Id,1,1,f);


		for (i=0; i<t->CommandNumber; i++) 
			if (!Map[i])
		{
			COMMAND * c = t->Commands+i; 

			fwrite(&c->Filter,1,1,f);
			fwrite(&c->Layer,1,1,f);

			unsigned char OpMask=0;
			OpMask |= c->OperatorMask[0]<<0;
			OpMask |= c->OperatorMask[1]<<1;
			OpMask |= c->OperatorMask[2]<<2;
			OpMask |= c->OperatorMask[3]<<3;
			OpMask |= (c->Operator&0x0f) << 4;
			fwrite(&OpMask,1,1,f);

			if (c->Filter==TEXGEN_Text) {
				TEXTTEMPLATE * tt = (TEXTTEMPLATE*)c->data;
				fwrite(c->data,7,1,f);
				fputs(tt->Text,f);
				fputc(0,f);
			}
			else
			if (c->Filter==TEXGEN_Jpeg)
			{
				JPEGTEMPLATE *tt=(JPEGTEMPLATE*)c->data;
				fwrite(&tt->JPEGDataSize,4,1,f);
				fwrite(tt->JPEGData,tt->JPEGDataSize,1,f);
			}
			else 
			if (c->Filter==TEXGEN_MakeMaterial) 
			{
				fwrite(c->data,1,1,f);

				bool Used=false;
				{
				for (SCENE *s=Scenes; s; s=s->Next)
					for (int k=0; k<s->ObjectNum; k++)
					if (s->ObjectList[k].Primitive==aDDict_MAPXFORM)
					{
						if (s->ObjectList[k].Material->TextureID==t->Id && 
							s->ObjectList[k].Material->SlotNumber==((MAKEMATERIALTEMPLATE*)(c->data))->MaterialSlot) Used=true;
					}
				}
				fwrite(&Used,1,1,f);

				//////////////////////////////////////////////////////////////////////////
				// ez irja ki az xbox demohoz a raw kepet.
				//fwrite(t->Layers[c->Layer].Data,t->XRes*t->XRes*4,1,f);

			}
			else fwrite(c->data,c->DataSize,1,f);

		}
		
	}
}

void ExportPolySelections(FILE *f)
{
	unsigned short Count=0;
	ITEM *i;
	for (i=PolySelections->List; i; i=i->Next) Count++;
	fwrite(&Count,1,1,f);
	for (i=PolySelections->List; i; i=i->Next)
	{
		ITEMLIST *j=(ITEMLIST*)i->Data;
		Count=0;
		ITEM *k;
		for (k=j->List;k;k=k->Next) Count++;
		fwrite(&Count,1,1,f);
		for (k=j->List;k;k=k->Next)
		{
			PVSelect *p=(PVSelect*)k->Data;
			fwrite(&p->Brush,1,1,f);
			fwrite(&p->Operator,1,1,f);
			fwrite(&p->Param1,2,1,f);
			fwrite(&p->Param2,2,1,f);
			fwrite(&p->Param3,2,1,f);
		}
	}
}

void ExportSplineFloat(FILE *f, SPLINE *s)
{
	fwrite(&s->KeyNum,1,1,f);
	fwrite(&s->Interpolation,1,1,f);
	for (int x=0; x<s->KeyNum; x++)
	{
		writefloat(f,s->Keys[x].Time,2);
		writefloat(f,s->Keys[x].Value,2);
	}
}

void ExportSplineColor(FILE *f, SPLINE *s)
{
	fwrite(&s->KeyNum,1,1,f);
	fwrite(&s->Interpolation,1,1,f);
	for (int x=0; x<s->KeyNum; x++)
	{
		writefloat(f,s->Keys[x].Time,2);
		if (s->Interpolation<aDDict_SINE)
		{
			float a=min(255,max(0,s->Keys[x].Value*16.0f));
			writefloat(f,a,2);
		}
		else
		{
			writefloat(f,s->Keys[x].Value*16.0f,2);
		}
	}
}

void ExportSplineByte(FILE *f, SPLINE *s)
{
	fwrite(&s->KeyNum,1,1,f);
	fwrite(&s->Interpolation,1,1,f);
	for (int x=0; x<s->KeyNum; x++)
	{
		writefloat(f,s->Keys[x].Time,2);
		unsigned char a=(int)(max(0,min(255,s->Keys[x].Value)));
		fwrite(&a,1,1,f);
	}
}


void ExportScenes(FILE *f)
{
	unsigned short Count=0;
	SCENE *s;
	for (s=Scenes; s; s=s->Next) if (UsedScenes[s->ID]) Count++;
	fwrite(&Count,1,1,f);
	for (s=Scenes; s; s=s->Next) if (UsedScenes[s->ID]) 
	{
		fwrite(&s->ID,1,1,f);
		fwrite(&s->ColorDiscard,1,1,f);
		fwrite(&s->ObjectNum,2,1,f);
		for (int a=0; a<s->ObjectNum; a++)
		{
			unsigned char c=s->ObjectList[a].Primitive;
			fwrite(&c,1,1,f);

			if (c==aDDict_STORED)
			{
				OBJECT *o=&s->ObjectList[a];
				fwrite(&o->VertexNum,2,1,f);
				int x;
				for (x=0; x<o->VertexNum; x++)
				{
					writefloat(f,o->VertexList[x].Position.x,2);
					writefloat(f,o->VertexList[x].Position.y,2);
					writefloat(f,o->VertexList[x].Position.z,2);
					char z=(char)(o->VertexList[x].CurrentNormal.x*127); fwrite(&z,1,1,f);
					z=(char)(o->VertexList[x].CurrentNormal.y*127); fwrite(&z,1,1,f);
					z=(char)(o->VertexList[x].CurrentNormal.z*127); fwrite(&z,1,1,f);
					unsigned char t=(unsigned char)(o->VertexList[x].TextureCoordinate.x*256); fwrite(&t,1,1,f);
					              t=(unsigned char)(o->VertexList[x].TextureCoordinate.y*256); fwrite(&t,1,1,f);
				}
				fwrite(&o->Textured,1,1,f);
				fwrite(&o->PolygonNum,2,1,f);
				for (x=0; x<o->PolygonNum; x++)
				{
					fwrite(&o->PolygonList[x].v1,2,1,f);
					fwrite(&o->PolygonList[x].v2,2,1,f);
					fwrite(&o->PolygonList[x].v3,2,1,f);
					if (o->Textured)
					{
					unsigned char t=(unsigned char)(o->PolygonList[x].ct[0].x*256); fwrite(&t,1,1,f);
					              t=(unsigned char)(o->PolygonList[x].ct[0].y*256); fwrite(&t,1,1,f);
					              t=(unsigned char)(o->PolygonList[x].ct[1].x*256); fwrite(&t,1,1,f);
					              t=(unsigned char)(o->PolygonList[x].ct[1].y*256); fwrite(&t,1,1,f);
					              t=(unsigned char)(o->PolygonList[x].ct[2].x*256); fwrite(&t,1,1,f);
					              t=(unsigned char)(o->PolygonList[x].ct[2].y*256); fwrite(&t,1,1,f);
					}
				}
			}

			int ObjectNum=a;

			//OBJECT o=s->ObjectList[a];

			OBJECTDATA od;
			od.Wireframe=s->ObjectList[ObjectNum].Wireframe;
			od.Textured=s->ObjectList[ObjectNum].Textured;
			od.Envmapped=s->ObjectList[ObjectNum].EnvMapped;
			od.NormalsInverted=s->ObjectList[ObjectNum].NormalsInverted;
			od.XTile=s->ObjectList[ObjectNum].Xtile!=1;
			od.YTile=s->ObjectList[ObjectNum].Ytile!=1;
			od.XSwap=s->ObjectList[ObjectNum].XSwap;
			od.YSwap=s->ObjectList[ObjectNum].YSwap;
			od.Swap=s->ObjectList[ObjectNum].Swap;
			od.Offx=s->ObjectList[ObjectNum].OffsetX!=0;
			od.Offy=s->ObjectList[ObjectNum].OffsetY!=0;
			od.Red=s->ObjectList[ObjectNum].Color.c[0]!=255;
			od.Green=s->ObjectList[ObjectNum].Color.c[1]!=255;
			od.Blue=s->ObjectList[ObjectNum].Color.c[2]!=255;
			od.Alpha=s->ObjectList[ObjectNum].Color.c[3]!=255;
			od.Zmask=s->ObjectList[ObjectNum].ZMask;
			od.Shading=s->ObjectList[ObjectNum].Shading;
			if (s->ObjectList[ObjectNum].Textured || s->ObjectList[ObjectNum].Primitive==aDDict_MAPXFORM)  od.TexSlot=s->ObjectList[ObjectNum].Material->SlotNumber;
			if (s->ObjectList[ObjectNum].EnvMapped) od.EnvSlot=s->ObjectList[ObjectNum].EnvMap->SlotNumber;
			od.AEpsilon=s->ObjectList[ObjectNum].AEpsilon;
			switch (s->ObjectList[ObjectNum].DSTBlend)
			{
				case GL_ZERO:                od.SRCBlend=0; break;
				case GL_ONE:                 od.SRCBlend=1; break;
				case GL_DST_COLOR:           od.SRCBlend=2; break;
				case GL_ONE_MINUS_DST_COLOR: od.SRCBlend=3; break;
				case GL_DST_ALPHA:           od.SRCBlend=4; break;
				case GL_ONE_MINUS_DST_ALPHA: od.SRCBlend=5; break;
				case GL_SRC_ALPHA:           od.SRCBlend=6; break;
				case GL_ONE_MINUS_SRC_ALPHA: od.SRCBlend=7; break;
				case GL_SRC_ALPHA_SATURATE:  od.SRCBlend=8; break;
			}
			switch (s->ObjectList[ObjectNum].SRCBlend)
			{
				case GL_ZERO:                od.DSTBlend=0; break;
				case GL_ONE:                 od.DSTBlend=1; break;
				case GL_SRC_COLOR:           od.DSTBlend=2; break;
				case GL_ONE_MINUS_SRC_COLOR: od.DSTBlend=3; break;
				case GL_DST_ALPHA:           od.DSTBlend=4; break;
				case GL_ONE_MINUS_DST_ALPHA: od.DSTBlend=5; break;
				case GL_SRC_ALPHA:           od.DSTBlend=6; break;
				case GL_ONE_MINUS_SRC_ALPHA: od.DSTBlend=7; break;
			}
			
			OBJECTDATA2 od2;
			od2.BackFace=s->ObjectList[ObjectNum].Backface;
			od2.BackFront=s->ObjectList[ObjectNum].Backfront;

			od2.Orientation=false;
			od2.Position=false;
			for (int xx=0; xx<3; xx++)
			{
				for (int yy=0; yy<3; yy++)
				{
					float a=0; if (yy==xx) a=1;
					if (s->ObjectList[ObjectNum].ModelView[xx][yy]!=a) od2.Orientation=true;
				}
				if (s->ObjectList[ObjectNum].ModelView[xx][3]!=0) od2.Position=true;
			}

			switch (s->ObjectList[ObjectNum].Primitive)
			{
				case aDDict_MAPXFORM:
					{
						fwrite(&od,4,1,f);
						if (od.XTile) fwrite(&s->ObjectList[ObjectNum].Xtile,1,1,f);
						if (od.YTile) fwrite(&s->ObjectList[ObjectNum].Ytile,1,1,f);
						if (od.Offx)
						{
							unsigned char k=(unsigned char)(s->ObjectList[ObjectNum].OffsetX*255);
							fwrite(&k,1,1,f);
						}
						if (od.Offy)
						{
							unsigned char k=(unsigned char)(s->ObjectList[ObjectNum].OffsetY*255);
							fwrite(&k,1,1,f);
						}
						fwrite(&s->ObjectList[ObjectNum].TargetObjectID,2,1,f);
						fwrite(&s->ObjectList[ObjectNum].MapXformType,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].MapXformColor,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].Material->TextureID,1,1,f);

						fwrite(&s->ObjectList[ObjectNum].Param1,1,1,f);
						char k=s->ObjectList[ObjectNum].Param2;
						fwrite(&k,1,1,f);
						k=s->ObjectList[ObjectNum].Param3;
						fwrite(&k,1,1,f);

						switch (s->ObjectList[ObjectNum].MapXformType)
						{
						case aDDict_SCALE:
							{
								VECTOR3 ScaleVec;
								M_RevScale(s->ObjectList[ObjectNum].ModelView,ScaleVec);
								writefloat(f,ScaleVec.x-1,2);
								writefloat(f,ScaleVec.y-1,2);
								writefloat(f,ScaleVec.z-1,2);
								break;
							}
						case aDDict_ROTATE:
							{
								VECTOR3 AxisVec;
								float Angle;
								M_RevRotate(s->ObjectList[ObjectNum].ModelView,AxisVec,Angle);
								writefloat(f,AxisVec.x,2);
								writefloat(f,AxisVec.y,2);
								writefloat(f,AxisVec.z,2);
								writefloat(f,Angle,2);
								break;
							}
						case aDDict_MOVE:
							{
								VECTOR3 MoveVec;
								M_RevXpose(s->ObjectList[ObjectNum].ModelView,MoveVec);
								writefloat(f,MoveVec.x,2);
								writefloat(f,MoveVec.y,2);
								writefloat(f,MoveVec.z,2);
								break;
							}
						}

						break;
					}
				case aDDict_CLONE:
					{
						int i=s->ObjectList[ObjectNum].ID;
						fwrite(&i,2,1,f);
						fwrite(&od2,1,1,f);
						writematrix(f,s->ObjectList[ObjectNum].ModelView,od2.Orientation,od2.Position);
						fwrite(&s->ObjectList[ObjectNum].Param1,1,1,f);
						for (int x=0; x<s->ObjectList[ObjectNum].Param1; x++)
							fwrite( &((int*)s->ObjectList[ObjectNum].Param2)[x],2,1,f);
						break;
					}
				case aDDict_BUTTERFLYSMOOTH:
				case aDDict_LINEARSMOOTH:
				case aDDict_MESHBLUR:
					{
						int i=s->ObjectList[ObjectNum].TargetObjectID;
						fwrite(&i,2,1,f);
						i=s->ObjectList[ObjectNum].Param2;
						fwrite(&i,1,1,f);
					}
					break;
				default:
					//if (s->ObjectList[ObjectNum].Primitive==aDDict_SUPERSHAPE) fclose(f);
					int i=s->ObjectList[ObjectNum].ID;
					fwrite(&i,2,1,f);
					fwrite(&od,4,1,f);
					fwrite(&od2,1,1,f);
					fwrite(&s->ObjectList[ObjectNum].AEpsilon,1,1,f);
					writematrix(f,s->ObjectList[ObjectNum].ModelView,od2.Orientation,od2.Position);
					if (od.XTile) fwrite(&s->ObjectList[ObjectNum].Xtile,1,1,f);
					if (od.YTile) fwrite(&s->ObjectList[ObjectNum].Ytile,1,1,f);
					if (od.Offx)
					{
						unsigned char k=(unsigned char)(s->ObjectList[ObjectNum].OffsetX*255);
						fwrite(&k,1,1,f);
					}
					if (od.Offy)
					{
						unsigned char k=(unsigned char)(s->ObjectList[ObjectNum].OffsetY*255);
						fwrite(&k,1,1,f);
					}
					if (od.Red) fwrite(&s->ObjectList[ObjectNum].Color.r,1,1,f);
					if (od.Green) fwrite(&s->ObjectList[ObjectNum].Color.g,1,1,f);
					if (od.Blue) fwrite(&s->ObjectList[ObjectNum].Color.b,1,1,f);
					if (od.Alpha) fwrite(&s->ObjectList[ObjectNum].Color.a,1,1,f);

					if (s->ObjectList[ObjectNum].Textured) fwrite(&s->ObjectList[ObjectNum].Material->TextureID,1,1,f);
					if (s->ObjectList[ObjectNum].EnvMapped) fwrite(&s->ObjectList[ObjectNum].EnvMap->TextureID,1,1,f);

					switch (s->ObjectList[ObjectNum].Primitive)
					{
					case aDDict_SPHERE:
					case aDDict_PLANE:
					case aDDict_CIRCLE:
						fwrite(&s->ObjectList[ObjectNum].Param1,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param2,1,1,f);
						break;
					case aDDict_LINE:
						fwrite(&s->ObjectList[ObjectNum].Param1,1,1,f);
						break;
					case aDDict_CONE:
					case aDDict_CYLINDER:
						fwrite(&s->ObjectList[ObjectNum].Param1,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param2,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param3,1,1,f);
						break;
					case aDDict_ARC:
						fwrite(&s->ObjectList[ObjectNum].Param1,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param2,1,2,f);
						break;
					case aDDict_LOFT:
						fwrite(&s->ObjectList[ObjectNum].Param1,1,2,f);
						fwrite(&s->ObjectList[ObjectNum].Param2,1,2,f);
						break;
					case aDDict_FUR:
						fwrite(&s->ObjectList[ObjectNum].Param1,2,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param2,1,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param3,2,1,f);
						fwrite(&s->ObjectList[ObjectNum].Param4,2,1,f);
						break;
					case aDDict_SUPERSHAPE:						
						SUPERSHAPE k=*(SUPERSHAPE*)(s->ObjectList[ObjectNum].Param1);
						fwrite(&k.SuperShape1.mfloat,1,1,f);
						fwrite(&k.SuperShape1.mint,1,1,f);
						writefloat(f,k.SuperShape1.n1,2);
						writefloat(f,k.SuperShape1.n2,2);
						writefloat(f,k.SuperShape1.n3,2);
						writefloat(f,k.SuperShape1.a,2);
						writefloat(f,k.SuperShape1.b,2);

						fwrite(&k.SuperShape2.mfloat,1,1,f);
						fwrite(&k.SuperShape2.mint,1,1,f);
						writefloat(f,k.SuperShape2.n1,2);
						writefloat(f,k.SuperShape2.n2,2);
						writefloat(f,k.SuperShape2.n3,2);
						writefloat(f,k.SuperShape2.a,2);
						writefloat(f,k.SuperShape2.b,2);

						fwrite(&k.Sphere,1,1,f);
						writefloat(f,k.Trad0,2);
						writefloat(f,k.TradPhi,2);
						fwrite(&k.RadFunc,1,1,f);
						writefloat(f,k.Rada,2);
						writefloat(f,k.Radb,2);
						fwrite(&k.VertFunc,1,1,f);
						writefloat(f,k.Verta,2);
						fwrite(&k.Rangex1,1,2,f);
						fwrite(&k.Rangey1,1,2,f);
						fwrite(&k.Rangex2,1,2,f);
						fwrite(&k.Rangey2,1,2,f);

						fwrite(&k.Xres,1,1,f);
						fwrite(&k.Yres,1,1,f);
						//fclose(f);

						break;
					}


			}
		}
	}
}

void ExportWorlds(FILE *f)
{
	unsigned short Count=0;
	WORLD *w;
	for (w=Worlds; w; w=w->Next) if (UsedWorlds[w->ID]) Count++;
	fwrite(&Count,1,1,f);
	for (w=Worlds; w; w=w->Next) if (UsedWorlds[w->ID])
	{
		fwrite(&w->ID,2,1,f);
		fwrite(&w->Fog,1,1,f);
		if (w->Fog) 
		{		
			fwrite(w->FogCol,4,1,f);	
			writefloat(f,w->FogStart,2);
			writefloat(f,w->FogEnd,2);
		}
		fwrite(&w->Lighting,1,1,f);
		if (w->Lighting)
		{
			unsigned char LightNum=0;
			int x;
			for (x=0; x<8; x++) if (w->Lights[x].Lit) LightNum++;
			fwrite(&LightNum,1,1,f);
			for (x=0; x<8; x++) 
				if (w->Lights[x].Lit)
				{
					fwrite(&x,1,1,f); //light id
					unsigned char LightType=1; //dotlight
					if (w->Lights[x].Spot_Cutoff!=180) LightType=2;//Spotlight
					if (w->Lights[x].Position[3]==0) LightType=0; //Dir light
					fwrite(&LightType,1,1,f);

					int buf=(int)(w->Lights[x].Ambient[0]*255);
					fwrite(&buf,1,1,f);
					buf=(int)(w->Lights[x].Ambient[1]*255);
					fwrite(&buf,1,1,f);
					buf=(int)(w->Lights[x].Ambient[2]*255);
					fwrite(&buf,1,1,f);
					buf=(int)(w->Lights[x].Color[0]*255);
					fwrite(&buf,1,1,f);
					buf=(int)(w->Lights[x].Color[1]*255);
					fwrite(&buf,1,1,f);
					buf=(int)(w->Lights[x].Color[2]*255);
					fwrite(&buf,1,1,f);
					writefloat(f,w->Lights[x].Position[0],2);
					writefloat(f,w->Lights[x].Position[1],2);
					writefloat(f,w->Lights[x].Position[2],2);
					writefloat(f,w->Lights[x].Position[3],2);
					if (w->Lights[x].Position[3]!=0)
					{
						writefloat(f,w->Lights[x].Constant_Attenuation,2);
						writefloat(f,w->Lights[x].Linear_Attenuation,2);
						writefloat(f,w->Lights[x].Quadratic_Attenuation,2);
					}
					writefloat(f,w->Lights[x].Spot_Cutoff,2);
					if (w->Lights[x].Spot_Cutoff!=180)
					{
						writefloat(f,w->Lights[x].Spot_Direction[0],2);
						writefloat(f,w->Lights[x].Spot_Direction[1],2);
						writefloat(f,w->Lights[x].Spot_Direction[2],2);
						writefloat(f,w->Lights[x].Spot_Exponent,2);
					}
				}
		}

		int cnum=0;
		int x;
		for (x=0; x<w->CamNum; x++) if (w->CamAnims[x].Used) cnum++;

		fwrite(&cnum,1,1,f);
		fwrite(&w->CamNum,1,1,f);
		for (x=0; x<w->CamNum; x++)
			if (w->CamAnims[x].Used)
		{
			fwrite(&w->CamAnims[x].CamID,1,1,f);
			ExportSplineFloat(f,w->CamAnims[x].Eyex);
			ExportSplineFloat(f,w->CamAnims[x].Eyey);
			ExportSplineFloat(f,w->CamAnims[x].Eyez);
			ExportSplineFloat(f,w->CamAnims[x].Trgx);
			ExportSplineFloat(f,w->CamAnims[x].Trgy);
			ExportSplineFloat(f,w->CamAnims[x].Trgz);
			ExportSplineFloat(f,w->CamAnims[x].Roll);
			ExportSplineFloat(f,w->CamAnims[x].Fov);
		}

		cnum=0;
		for (x=0; x<w->AnimNum; x++) if (w->Animations[x].Used) cnum++;

		fwrite(&cnum,1,1,f);
		fwrite(&w->AnimNum,1,1,f);

		//anim header

		fwrite(&w->ObjectNum,2,1,f);
		for (x=0; x<w->ObjectNum; x++)
		{
			WORLDOBJECT o=w->ObjectList[x];
			fwrite(&o.ID,2,1,f);
			short int Parent=o.ParentID;
			fwrite(&Parent,2,1,f);
			fwrite(&o.Primitive,1,1,f);
		
			if (o.Data)
			{
				if (o.Primitive!=aDDict_SUBSCENE)
				fwrite(&((SCENE*)o.Data)->ID,1,1,f);
				else fwrite(&((WORLD*)o.Data)->ID,1,1,f);
			}
			else
			{
				char a=-1;
				fwrite(&a,1,1,f);
			}

			WORLDOBJECTDATA od;

			switch (o.DSTBlend)
			{
				case GL_ZERO:                od.SRCBlend=0; break;
				case GL_ONE:                 od.SRCBlend=1; break;
				case GL_DST_COLOR:           od.SRCBlend=2; break;
				case GL_ONE_MINUS_DST_COLOR: od.SRCBlend=3; break;
				case GL_DST_ALPHA:           od.SRCBlend=4; break;
				case GL_ONE_MINUS_DST_ALPHA: od.SRCBlend=5; break;
				case GL_SRC_ALPHA:           od.SRCBlend=6; break;
				case GL_ONE_MINUS_SRC_ALPHA: od.SRCBlend=7; break;
				case GL_SRC_ALPHA_SATURATE:  od.SRCBlend=8; break;
			}
			switch (o.SRCBlend)
			{
				case GL_ZERO:                od.DSTBlend=0; break;
				case GL_ONE:                 od.DSTBlend=1; break;
				case GL_SRC_COLOR:           od.DSTBlend=2; break;
				case GL_ONE_MINUS_SRC_COLOR: od.DSTBlend=3; break;
				case GL_DST_ALPHA:           od.DSTBlend=4; break;
				case GL_ONE_MINUS_DST_ALPHA: od.DSTBlend=5; break;
				case GL_SRC_ALPHA:           od.DSTBlend=6; break;
				case GL_ONE_MINUS_SRC_ALPHA: od.DSTBlend=7; break;
			}

			od.Posx=o.PosData.Pos.x!=0;
			od.Posy=o.PosData.Pos.y!=0;
			od.Posz=o.PosData.Pos.z!=0;
			od.Sclx=o.PosData.Scale.x!=1;
			od.Scly=o.PosData.Scale.y!=1;
			od.Sclz=o.PosData.Scale.z!=1;
			od.Quat=o.PosData.Quaternion.s!=1;
			od.Textured=o.Textured;
			if (o.Textured) od.TexSlot=o.Material->SlotNumber;
			od.EmitterData=o.EmitterData==0;
			if (o.EmitterData)
			{
				od.Tail=o.EmitterData->Tail;
				od.Head=o.EmitterData->Head;
				od.ObjectHead=o.EmitterData->ObjectHead;
				od.TailRes1=o.EmitterData->TailLength-1;
				od.TailRes2=o.EmitterData->TailLength2-1;
			}

			fwrite(&od,4,1,f);
			
			if (o.Textured) fwrite(&o.Material->TextureID,1,1,f);
			if (od.Posx) writefloat(f,o.PosData.Pos.x,2);
			if (od.Posy) writefloat(f,o.PosData.Pos.y,2);
			if (od.Posz) writefloat(f,o.PosData.Pos.z,2);
			if (od.Sclx) writefloat(f,o.PosData.Scale.x,2);
			if (od.Scly) writefloat(f,o.PosData.Scale.y,2);
			if (od.Sclz) writefloat(f,o.PosData.Scale.z,2);
			if (od.Quat)
			{
				writefloat(f,o.PosData.Quaternion.s,2);
				writefloat(f,o.PosData.Quaternion.v.x,2);
				writefloat(f,o.PosData.Quaternion.v.y,2);
				writefloat(f,o.PosData.Quaternion.v.z,2);
			}

			switch (o.Primitive)
			{
			case aDDict_SPHEREEMITTER:
			case aDDict_CUBEEMITTER:
			case aDDict_CYLINDEREMITTER:
				{
					fwrite(&o.EmitterData->DefaultAge,2,1,f);
					fwrite(&o.EmitterData->AgeChaos,2,1,f);
					writefloat(f,o.EmitterData->ParticlesPerFrame,2);
					fwrite(&o.EmitterData->MaxParticles,2,1,f);
					fwrite(&o.EmitterData->FixedUp,1,1,f);
					fwrite(&o.ZMask,1,1,f);
					fwrite(&o.AEpsilon,1,1,f);
					//if (od.ObjectHead) 
					int d;
					for (d=0; d<5; d++)
					{
						unsigned char c=(unsigned char)(o.EmitterData->Color1[d]*255);
						fwrite(&c,1,1,f);
					}
					for (d=0; d<5; d++)
					{
						unsigned char c=(unsigned char)(o.EmitterData->Color2[d]*255);
						fwrite(&c,1,1,f);
					}
					writefloat(f,o.EmitterData->Param1,2);
					writefloat(f,o.EmitterData->Param2,2);
					writefloat(f,o.EmitterData->Size,2);

					writefloat(f,o.EmitterData->Rotspeed,2);
					writefloat(f,o.EmitterData->RotChaos,2);
					writefloat(f,o.EmitterData->CamFade,2);

					writefloat(f,o.EmitterData->Speed,2);
					writefloat(f,o.EmitterData->SpeedChaos,2);
					writefloat(f,o.EmitterData->DirChaos,2);
					writefloat(f,o.EmitterData->Dir.x,2);
					writefloat(f,o.EmitterData->Dir.y,2);
					writefloat(f,o.EmitterData->Dir.z,2);
					fwrite(&o.EmitterData->RandRotate,1,1,f);

					break;
				}
			case aDDict_GRAVITY:
			case aDDict_PLANEDEFLECTOR:
			case aDDict_SPHEREDEFLECTOR:
			case aDDict_SPHERESLOWER:
					writefloat(f,o.EmitterData->Param1,2);
					writefloat(f,o.EmitterData->Param2,2);
				break;				
			}

			for (int y=0; y<w->AnimNum; y++)
				if (w->Animations[y].Used)
			{
				//fwrite(w->Animations[y].AnimID,1,1,f);
				//fwrite(&y,1,1,f);
				switch (o.Primitive)
				{
				case aDDict_SPHEREEMITTER:
				case aDDict_CUBEEMITTER:
				case aDDict_CYLINDEREMITTER:
					ExportSplineFloat(f,o.Animations[y].Prtfrme);
					ExportSplineFloat(f,o.Animations[y].Size);
				default:
					ExportSplineColor(f,o.Animations[y].Red);
					ExportSplineColor(f,o.Animations[y].Green);
					ExportSplineColor(f,o.Animations[y].Blue);
					ExportSplineColor(f,o.Animations[y].Alpha);
					ExportSplineFloat(f,o.Animations[y].Posx);
					ExportSplineFloat(f,o.Animations[y].Posy);
					ExportSplineFloat(f,o.Animations[y].Posz);
					ExportSplineFloat(f,o.Animations[y].Sclx);
					ExportSplineFloat(f,o.Animations[y].Scly);
					ExportSplineFloat(f,o.Animations[y].Sclz);
					ExportSplineFloat(f,o.Animations[y].Rotx);
					ExportSplineFloat(f,o.Animations[y].Roty);
					ExportSplineFloat(f,o.Animations[y].Rotz);
					ExportSplineFloat(f,o.Animations[y].Rotw);
					ExportSplineByte(f,o.Animations[y].AnimID);
					ExportSplineFloat(f,o.Animations[y].AnimTime);
				}
			}

		}
		for (int y=0; y<w->AnimNum; y++)
			if (w->Animations[y].Used)
				fwrite(&w->Animations[y].AnimID,1,1,f);

	}
}

void SaveEvent(FILE *f,EVENT *e)
{
	fwrite(&e->StartFrame,2,1,f);
	fwrite(&e->EndFrame,2,1,f);
	fwrite(&e->Pass,1,1,f);
	fwrite(&e->Effect->Type,1,1,f);
	fwrite(&e->ex1,2,1,f);
	fwrite(&e->ex2,2,1,f);
	fwrite(&e->ey1,2,1,f);
	fwrite(&e->ey2,2,1,f);
	fwrite(&e->sx1,2,1,f);
	fwrite(&e->sx2,2,1,f);
	fwrite(&e->sy1,2,1,f);
	fwrite(&e->sy2,2,1,f);
	switch (e->Effect->Type)
	{
		case 0: //NO EFFECT
			fwrite(e->EffectData,1,e->Effect->DataSize,f);
		break;
		case 1: //2D LAYER
			{
				fwrite(e->EffectData,1,e->Effect->DataSize-4,f);
				int i=0;
				MATERIAL *m=((LAYER2DTEMPLATE*)e->EffectData)->Texture;
				
				if (m)
				{
					fwrite(&m->TextureID,1,3,f);
					fwrite(&m->SlotNumber,1,1,f);
				}

				else
				fwrite(&i,4,1,f);
			}
		break;
		case 2: //PARTICLE RENDER
			{
				int i=((PARTICLETEMPLATE*)e->EffectData)->World->ID;
				fwrite(&i,1,1,f);
				fwrite(&((PARTICLETEMPLATE*)e->EffectData)->CamID,1,1,f);
				fwrite(&((PARTICLETEMPLATE*)e->EffectData)->AnimID,1,1,f);
				writefloat(f,((PARTICLETEMPLATE*)e->EffectData)->AnimStart,2);
				writefloat(f,((PARTICLETEMPLATE*)e->EffectData)->AnimEnd,2);
			}
		break;
		case 3: //CLEAR
			fwrite(e->EffectData,1,e->Effect->DataSize,f);
		break;
		case 4: //RENDERTEXTURE
			fwrite(e->EffectData,1,e->Effect->DataSize,f);
		break;
		case 5: //FEEDBACK
			fwrite(e->EffectData,1,e->Effect->DataSize,f);
		break;
		case 7: //3D SCENE
			{
				int i=((DISPLAY3DTEMPLATE*)e->EffectData)->World->ID;
				fwrite(&i,1,1,f);
				fwrite(&((DISPLAY3DTEMPLATE*)e->EffectData)->CamID,1,1,f);
				fwrite(&((DISPLAY3DTEMPLATE*)e->EffectData)->AnimID,1,1,f);
				writefloat(f,((DISPLAY3DTEMPLATE*)e->EffectData)->CamStart,2);
				writefloat(f,((DISPLAY3DTEMPLATE*)e->EffectData)->CamEnd,2);
				writefloat(f,((DISPLAY3DTEMPLATE*)e->EffectData)->AnimStart,2);
				writefloat(f,((DISPLAY3DTEMPLATE*)e->EffectData)->AnimEnd,2);
				writefloat(f,((DISPLAY3DTEMPLATE*)e->EffectData)->DontSaveCam,1);
			}
		break;
		case 8: //PARTICLE RESET
			{
				int i=((DISPLAY3DTEMPLATE*)e->EffectData)->World->ID;
				fwrite(&i,1,1,f);
			}
		break;
		case 12: //GRIDFILL
			{
				fwrite(e->EffectData,1,e->Effect->DataSize-4,f);
				int i=0;
				MATERIAL *m=((GRIDFILLERTEMPLATE*)e->EffectData)->Texture;
				
				if (m)
				{
					fwrite(&m->TextureID,1,3,f);
					fwrite(&m->SlotNumber,1,1,f);
				}

				else
				fwrite(&i,4,1,f);
			}
		break;
		case 13: //CAVE
			{
				fwrite(e->EffectData,1,e->Effect->DataSize-4,f);
				int i=0;
				MATERIAL *m=((CAVETEMPLATE*)e->EffectData)->Texture;
				
				if (m)
				{
					fwrite(&m->TextureID,1,3,f);
					fwrite(&m->SlotNumber,1,1,f);
				}

				else
				fwrite(&i,4,1,f);
			}
		break;
		default:
			fwrite(e->EffectData,1,e->Effect->DataSize,f);
	}
}

void ExportEvents(FILE *f)
{
	unsigned short Count=0;
	int HighestPass=0;
	EVENT *e;
	for (e=EventList; e; e=e->Next) 
	{	
		Count++;
		HighestPass=max(HighestPass,e->Pass);
	}
	fwrite(&Count,1,2,f);
	for (int x=0; x<=HighestPass; x++)
	for (e=EventList; e; e=e->Next)
		if (e->Pass==x) SaveEvent(f,e);
}

void ExportMinimalProject(char *FileName)
{
	AnalyzeProject(true);
	FILE *f=fopen(FileName,"w+b");

	ExportHeader(f);
	ExportTextures(f);
	ExportPolySelections(f);
	ExportScenes(f);
	ExportWorlds(f);
	ExportEvents(f);

	fclose(f);
}

char *TexFilterNames[]=
{
	"Envmap",
	"Plasma",
	"Map",
	"Blur",
	"DirBlur",
	"Text",
	"SubPlasma",
	"FractalPlasma",
	"Colorize",
	"Shade",

	"Brightness",
	"Copy",
	"Cells",
	"Twirl",
	"SineDist",
	"Mix",
	"MixMap",
	"Scale",
	"SineColor",
	"Max",

	"HSV",
	"Emboss",
	"Invert",
	"Glass",
	"Pixelize",
	"Offset",
	"Crystalize",
	"Rectangle",
	"Circle",
	"Contrast",

	"MakeMaterial",
	"Gradient",
	"Rotozoom",
	"ChamferRectangle",
	"Dots",
	"Jpeg",
	"FractalPlasmaOld",

};

char *BaseObjectNames[]=
{
	"BOX",
	"SPHERE",
	"CYLINDER",
	"CONE",
	"PLANE",
	"ARC",
	"LINE",
	"LOFT",
	"ICOSAEDER",
	"DODECAEDER",
	"CIRCLE",
	"CLONE",
	"STORED",
	"FUR",
	"SUPERSHAPE",
};

char *AdvObjectNames[]=
{
	"MAPXFORM",
	"LINEARSMOOTH",
	"BUTTERFLYSMOOTH",
	"MESHBLUR",
};

char *WorldNames[]=
{
	"DUMMY",
	"SCENE",
	"SUBSCENE",
	"SPHEREEMITTER",
	"CUBEEMITTER",
	"CYLINDEREMITTER",
	"PLANEDEFLECTOR",
	"SPHEREDEFLECTOR",
	"GRAVITY",
};

char *EventNames[]=
{
	"NOEFFECT",
	"2DLAYER",
	"PARTICLECALC",
	"CLEAR",
	"RENDERTOTEXTURE",
	"FEEDBACK",
	"BLUR",
	"3DSCENE",
	"PARTICLERESET",
	"CONTRAST",
	"FUNKENDSCROLL",
	"CRYSTALIZE",
	"GRIDFILLER",
	"CAVE",
	"CAMSHAKE",
	"MRCEFFECT",
	"FEEDBACK2",
};

void CreateIncludeList()
{
	bool TextureEffects[256];
	memset(TextureEffects,0,256);

	for (TEXTURE *t=TextureList; t; t=t->Next)
		for (int i=0; i<t->CommandNumber; i++)
			TextureEffects[t->Commands[i].Filter]=true;

	FILE *f=fopen("data/includelist.h","w+t");

	fprintf(f,"#pragma once\n\n");

	int x;
	for (x=0; x<37; x++)
	{
		if (!TextureEffects[x]) fprintf(f,"//");
		fprintf(f,"#define INCLUDE_TEX_%s\n",TexFilterNames[x]);
	}

	fprintf(f,"\n\n");

	bool Objects[256];
	memset(Objects,0,256);
	bool PolySelect=false;

	for (SCENE *s=Scenes; s; s=s->Next)
		for (int i=0; i<s->ObjectNum; i++)
		{
			Objects[s->ObjectList[i].Primitive]=true;
			//if (s->ObjectList[i].Primitive==aDDict_MAPXFORM && s->ObjectList[i].Param2) PolySelect=true;
		}

	for (x=0; x<15; x++)
	{
		if (!Objects[x]) fprintf(f,"//");
		fprintf(f,"#define INCLUDE_OBJ_%s\n",BaseObjectNames[x]);
	}
	fprintf(f,"\n");

	if (Objects[aDDict_LINEARSMOOTH] || Objects[aDDict_BUTTERFLYSMOOTH]) fprintf(f,"#define INCLUDE_MESHSMOOTH\n\n");
	else fprintf(f,"//#define INCLUDE_MESHSMOOTH\n\n");

	for (x=0; x<4; x++)
	{
		if (!Objects[x+100]) fprintf(f,"//");
		fprintf(f,"#define INCLUDE_OBJ_%s\n",AdvObjectNames[x]);
	}

	fprintf(f,"\n\n");
	memset(Objects,0,256);

	for (WORLD *w=Worlds; w; w=w->Next)
		for (int i=0; i<w->ObjectNum; i++)
			Objects[w->ObjectList[i].Primitive]=true;

	if (Objects[aDDict_SPHEREEMITTER] || Objects[aDDict_CUBEEMITTER] || Objects[aDDict_CYLINDEREMITTER]) fprintf(f,"#define INCLUDE_PARTICLE_ENGINE\n\n");
	else fprintf(f,"//#define INCLUDE_PARTICLE_ENGINE\n\n");

	for (x=0; x<9; x++)
	{
		if (!Objects[x]) fprintf(f,"//");
		fprintf(f,"#define INCLUDE_WRL_%s\n",WorldNames[x]);
	}

	fprintf(f,"\n\n");
	memset(Objects,0,256);

	for (EVENT *e=EventList; e; e=e->Next)
		Objects[e->EventType]=true;

	for (x=0; x<17; x++)
	{
		if (!Objects[x]) fprintf(f,"//");
		fprintf(f,"#define INCLUDE_EVENT_%s\n",EventNames[x]);
	}

	fclose(f);
}

#endif