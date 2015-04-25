#include "3dEngine.h"
#include "ObjectGenerators.h"
#include "float.h"

void MultModelViewMatrix(MATRIX m)
{
	//igen, ez az aktualis opengl matrixot beszorozza m-el. jol.

	float matrix[16];
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			matrix[x+y*4]=m[x][y];
	glMultMatrixf(matrix);
}

void SetCameraView(CAMERA *Cam, float AspectRatio)
{
	//kamerabeallitas :)

	MATRIX m;
	VECTOR3 u,dir;

	dir=V3_Normalize(V3_Sub(Cam->Target,Cam->Eye));
	M_Rotate(dir.x,dir.y,dir.z,(float)Cam->Roll*360.0f/255.0f*(float)radtheta,m);
	M_Xformd(m,Cam->Up,u); //itt keszul a rotated up vector (a rollhoz)

	Cam->i=V3_Normalize(V3_Cross(dir,u));
	Cam->j=V3_Normalize(V3_Cross(dir,Cam->i)); //kamera sikjaban egysegvektorok (billboardinghoz)

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(Cam->Fov,AspectRatio,0.1,2000.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(Cam->Eye.x,Cam->Eye.y,Cam->Eye.z,
			  Cam->Target.x,Cam->Target.y,Cam->Target.z,
			  u.x,u.y,u.z);

}

void WORLD::SetupLighting()
{
	//na ez belovi a fenyeket, biztos van ilyen a d3dben is, az opengl doksi leirja h mikor
	//directional, mikor spot, mikor omni stb, nemelyik ertektol fugg
	glEnable(GL_LIGHTING);
	for (int x=0; x<8; x++)
	if (Lights[x].Lit)
	{
		glEnable(Lights[x].Identifier);
		glLightfv(Lights[x].Identifier,GL_AMBIENT,Lights[x].Ambient);
		glLightfv(Lights[x].Identifier,GL_DIFFUSE,Lights[x].Color);
		glLightfv(Lights[x].Identifier,GL_SPECULAR,Lights[x].Color);
		glLightfv(Lights[x].Identifier,GL_POSITION,Lights[x].Position);
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

void WORLD::CalculateObjectMatrices()
{
	//kiszamolja az aktualis matrixokat a rot scale pos adatokbol
	for (int x=0; x<ObjectNum; x++) RST2Matrix(&ObjectList[x].PosData,ObjectList[x].ModelView);
}

void WORLD::SetToAnimPos(ANIMPOS *a)
{
	//szamolja az animaciot. d3d szempontbol lenyegtelen
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

			if (s->Red->KeyNum) ObjectList[x].Color.r=(int)min(255,max(0,s->Red->GetInterpolatedValue(TimePos)));
			if (s->Green->KeyNum) ObjectList[x].Color.g=(int)min(255,max(0,s->Green->GetInterpolatedValue(TimePos)));
			if (s->Blue->KeyNum) ObjectList[x].Color.b=(int)min(255,max(0,s->Blue->GetInterpolatedValue(TimePos)));
			if (s->Alpha->KeyNum) ObjectList[x].Color.a=(int)min(255,max(0,s->Alpha->GetInterpolatedValue(TimePos)));

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

void DrawChildren(WORLD *w, int Parent, bool ZMask)
{
	//object hierarchy render
	for (int x=0; x<w->ObjectNum; x++)
	if (w->ObjectList[x].ParentID==Parent)
	{		
		glPushMatrix();
 		MultModelViewMatrix(w->ObjectList[x].ModelView); //bemegyunk object space-be
		switch (w->ObjectList[x].Primitive)
		{
		case aDDict_SCENE:
			{
				if (((SCENE*)w->ObjectList[x].Data)->ColorDiscard) //ha animalni akarjuk az object szinet
					glColor4ub(w->ObjectList[x].Color.c[0],w->ObjectList[x].Color.c[1],w->ObjectList[x].Color.c[2],w->ObjectList[x].Color.c[3]);

				//elso korben a zmaskosak aztan a nem zmaskosak renderelodnek
				if (ZMask) glCallList(((SCENE*)w->ObjectList[x].Data)->ZmaskList);
					else glCallList(((SCENE*)w->ObjectList[x].Data)->NoZmaskList);
				glDisable(GL_TEXTURE_GEN_S); //envmapszamolas kikapcs
				glDisable(GL_TEXTURE_GEN_T);
				break;
			}
		case aDDict_SUBSCENE:
			//animacio szamitas, nem fontos d3dszempontbol
			((WORLD*)w->ObjectList[x].Data)->SetToAnimPos(&w->ObjectList[x].APosData);
			//render
			((WORLD*)w->ObjectList[x].Data)->Render(true,ZMask);
			break;
		}
		DrawChildren(w,w->ObjectList[x].ID,ZMask);
		glPopMatrix();
	}
}

void WORLD::Render(bool SubWorld, bool Zmask)
{
	//pushattrib EVIL, kill it
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (!SubWorld)
	{
		if (Lighting) 
		SetupLighting(); //fenyezes beallitasa
		else 
			glDisable(GL_LIGHTING);
		if (Fog) //kod beallitasa
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

		DrawChildren(this,-1,true); //zmaskos render
		glDepthMask(false);
		DrawChildren(this,-1,false); //zmask nelkuli render
	}
	else DrawChildren(this,-1,Zmask); //ha subworld, akkor megy lejjebb a hierarchyn, a zmask beallitas az amit fentrol kapott, igy csak jot renderel

	//popattrib atikiller, badbadbad
	glPopAttrib();
}

int CountClones(SCENE *Scene, int CloneID)
{
	//osszeszamolja hogy valojaban hany object lesz a klonokbol
	int ObjNum=0;

	OBJECT *Clone=&Scene->ObjectList[CloneID];

	for (int x=0; x<Clone->Param1; x++)
	{
		OBJECT *o=((OBJECT*)((int*)Clone->Param3)[x]);
		if (o->Primitive<100 && o->Primitive!=11) 
		{
			ObjNum++;
			if (o->EnvMapped) ObjNum++;
		}
		if (o->Primitive==11) ObjNum+=CountClones(Scene,Scene->FindObjectIndex(((int*)Clone->Param2)[x]));
	}

	return ObjNum;
}

int SCENE::CountObjects()
{
	//osszeszamolja hogy valojaban hany objectet renderelunk (egyes klonozott peldanyok included)
	int ObjNum=0;

	for (int x=0; x<ObjectNum; x++)
	{
		if (ObjectList[x].Primitive<100 && ObjectList[x].Primitive!=11 /*clone*/) 
		{
			ObjNum++;
			if (ObjectList[x].EnvMapped) ObjNum++;
		}
		if (ObjectList[x].Primitive==11 /*clone*/) ObjNum+=CountClones(this,x);
	}

	return ObjNum;
}

/************************************************************************************

	Na. A precalc ugy mukodik hogy letrejon egy tomb "object dummy"kbol, amiben
	pontosan annyi object lesz amennyit tenyleg renderelni kell (klonok inkluzive).
	Ebbe a tombbe bekerulnek az igazi renderelendo objectekre a hivatkozasok, es a
	valos poziciojuk egy matrix formajaban. Ezutan ez a tomb sortolodik attributumok
	alapjan (texturak, zmask, ilyesmi), es az igy sortolt cuccbol egy jobban
	optimalizalt display list keszul.

	Nagyon fontos h az envmapok kulon objectkent kerulnek bele a dummy listaba, es
	azok is sortolodnak


************************************************************************************/


void SCENE::CalculateDummy(OBJECT *Object, int &DummyNum, MATRIX *Position)
{
	//feltolt egy slotot a dummy tombben
	ObjDummies[DummyNum].OriginalObj=Object;
	ObjDummies[DummyNum].OriginalWorldObj=NULL;

	memcpy(ObjDummies[DummyNum].ModelView,Position,sizeof(MATRIX));

	DummyNum++;

	if (Object->EnvMapped)
	{
		ObjDummies[DummyNum].OriginalObj=Object;
		ObjDummies[DummyNum].OriginalWorldObj=NULL;
		ObjDummies[DummyNum].Envmap=true;
		ObjDummies[DummyNum].List=ObjDummies[DummyNum-1].List;
		memcpy(ObjDummies[DummyNum].ModelView,Position,sizeof(MATRIX));
		DummyNum++;
	}
}

void SetupClones(SCENE *Scene, int CloneID, int &FilledDummy, MATRIX *ParentMatrix)
{
	//lekezeli a klonok slotjait a dummy tombben
	OBJECT *Clone=&Scene->ObjectList[CloneID];

	MATRIX New;

	for (int x=0; x<Clone->Param1; x++)
	{
		OBJECT *o=((OBJECT*)((int*)Clone->Param3)[x]);
		M_Mult(*ParentMatrix,o->ModelView,New);

		if (o->Primitive<100 && o->Primitive!=11) Scene->CalculateDummy(&(Scene->ObjectList[Scene->FindObjectIndex(((int*)Clone->Param2)[x])]),FilledDummy,&New);
		
		if (o->Primitive==11) SetupClones(Scene,Scene->FindObjectIndex(((int*)Clone->Param2)[x]),FilledDummy,&New);
	}
}

int DummyCompare(const void * elem1, const void * elem2)
{   //ez a dummy attributum sort compare cucc
	OBJDUMMY *a=(OBJDUMMY*)elem1;
	OBJDUMMY *b=(OBJDUMMY*)elem2;

	if (a->OriginalObj->ZMask && !b->OriginalObj->ZMask) return -1;
	if (!a->OriginalObj->ZMask && b->OriginalObj->ZMask) return 1; //zmask elore

	/*int at=a->OriginalWorldObj->ID-b->OriginalWorldObj->ID;
	if (at!=0) return at; //worldobjectek szerint is sort*/

	if (a->Envmap && !b->Envmap) return 1;
	if (!a->Envmap && b->Envmap) return -1; //nem envmap elore

	if (!a->OriginalObj->Material) a->OriginalObj->Textured=false;
	if (!b->OriginalObj->Material) b->OriginalObj->Textured=false;
	if (a->OriginalObj->Textured && !b->OriginalObj->Textured) return 1;
	if (!a->OriginalObj->Textured && b->OriginalObj->Textured) return -1; //nem texturazott elore

	if (a->OriginalObj->Textured) //csak akkor kell textura szerint sort ha van textura ;)
	{
		int at=a->OriginalObj->Material->TextureID*8+a->OriginalObj->Material->SlotNumber;
		int bt=b->OriginalObj->Material->TextureID*8+b->OriginalObj->Material->SlotNumber;

		at-=bt;
		if (at!=0) return at;
	}

	if (a->OriginalObj->Backface) //csak akkor kell backface szerint ha be van kapcsolva
	{
		return a->OriginalObj->Backfront==b->OriginalObj->Backfront;
	}

	/*at=a->OriginalObj->SRCBlend*10000+a->OriginalObj->DSTBlend;
	int bt=b->OriginalObj->SRCBlend*10000+b->OriginalObj->DSTBlend;

	at-=bt;
	if (at!=0) return at;*/

	/*int at=a->OriginalObj->ID-b->OriginalObj->ID;
	if (at!=0) return at; //objectek szerint is sort*/

	return 0;
}

void SCENE::SetupDummies()
{
	bool Lighting=true;
	//na ez lovi be a scene display listjeit

	ObjDummyCount=CountObjects();
	ObjDummies=new OBJDUMMY[ObjDummyCount];
	memset(ObjDummies,0,sizeof(OBJDUMMY)*ObjDummyCount); //dummy tomb

	int FilledDummy=0;

	int x;
	for (x=0; x<ObjectNum; x++) //dummy tomb feltoltese
	{
		if (ObjectList[x].Primitive<100 && ObjectList[x].Primitive!=11 /*clone*/) CalculateDummy(&ObjectList[x],FilledDummy,&ObjectList[x].ModelView);
		if (ObjectList[x].Primitive==11 /*clone*/) SetupClones(this,x,FilledDummy,&ObjectList[x].ModelView);
	}

	qsort(ObjDummies,ObjDummyCount,sizeof(OBJDUMMY),DummyCompare); //attributumsort

	glNewList(ZmaskList,GL_COMPILE); //ket lista keszul, egy a zmaskos objecteknek, egy a nem zmaskosoknak
									 //ez a zmaskos, a nem zmaskos a valtaskor kap erteket
	glBegin(GL_TRIANGLES);
	unsigned char c[4],o[4];
	for (x=0; x<4; x++) c[x]=o[x]=2;

	for (int z=0; z<ObjDummyCount; z++) //a fo loop
	{
		//glErrorCheck();
		OBJECT *LastObject=NULL;
		if (z) LastObject=ObjDummies[z-1].OriginalObj;  //ez az elozo object, referencianak h melyik
														//attributum valtozott


		OBJECT *Object=ObjDummies[z].OriginalObj;		//az aktualis object

		//MATRIX Position;
		//memcpy(&Position,ObjDummies[z].ModelView,sizeof(MATRIX)); //az object pozicioja
		/*MATRIX InvTransp;
		memcpy(&InvTransp,&Position,sizeof(MATRIX));
		M_Invert(InvTransp);
		M_Transpose(InvTransp);*/
		
		//eltranszformaljuk az osszes vertexet a vegleges pozicioba, es kiszamoljuk a normalokat,
		//erre azert van szukseg mert igy jok lesznek a normalok es a nonuniform scalezettek nem bugzanak be
		for (x=0; x<Object->VertexNum; x++)
		{
				Object->VertexList[x].MapTransformedPosition=Object->VertexList[x].Position;
				M_Xformd(ObjDummies[z].ModelView,Object->VertexList[x].MapTransformedPosition,Object->VertexList[x].MapTransformedPosition);
		}
		Object->CalculateNormals();

		//ha szinvaltozas van
		bool NewCol=!LastObject;
		if (!ColorDiscard && LastObject) for (int a=0; a<4; a++) if (Object->Color.c[a]!=LastObject->Color.c[a]) NewCol=true;

		bool Cut;

		//ha elertunk a zmask valtashoz
		bool ZmaskChange=!Object->ZMask && (!LastObject || LastObject->ZMask);
		NewCol=ZmaskChange || NewCol;
		//ha envmap objecthez ertunk
		bool EnvmapTurnOn=((LastObject && !ObjDummies[z-1].Envmap) || !LastObject) && ObjDummies[z].Envmap;
		//ha wireframe valtas van
		//bool WireframeChange=!LastObject || ZmaskChange || EnvmapTurnOn || (Object->Wireframe != LastObject->Wireframe);
		//ha kikapcsolodott a textura
		bool TextureDisable=(!Object->Textured && ((LastObject && LastObject->Textured) || !LastObject));// || (WireframeChange && Object->Wireframe && !ObjDummies[z].Envmap);
		//ha valtozott a textura
		bool TextureChange=Object->Textured && (ZmaskChange || (!LastObject || !LastObject->Textured) || (LastObject && LastObject->Material->TextureID*8+LastObject->Material->SlotNumber!=Object->Material->TextureID*8+Object->Material->SlotNumber));// || (WireframeChange && !Object->Wireframe && Object->Textured));
		//ha valtozott a blendmode
		bool BlendChange=!LastObject || LastObject->SRCBlend!=Object->SRCBlend || LastObject->DSTBlend!=Object->DSTBlend;
		//ha valtozott az envmap
		bool EnvmapChange=Object->EnvMap && LastObject && ObjDummies[z-1].Envmap && (LastObject->EnvMap->TextureID*8+LastObject->EnvMap->SlotNumber!=Object->EnvMap->TextureID*8+Object->EnvMap->SlotNumber);
		//ha valtozott a backface culling ki be kapcsoltsaga
		bool BackChange=!LastObject || ZmaskChange || (Object->Backface != LastObject->Backface);
		//ha valtozott a backface culling iranya
		bool BackFrontChange=!LastObject || ZmaskChange || (Object->Backface && Object->Backfront!=LastObject->Backfront);
		//ha valtozott az alpha epsilon
		bool AepsilonChange=!LastObject || ZmaskChange || (Object->AEpsilon != LastObject->AEpsilon);

		//ha meg kell szakitani modvaltas miatt a glbegin() glend()-et
		Cut=ZmaskChange || 
			(!ObjDummies[z].Envmap && (TextureDisable || TextureChange || BlendChange) ) ||
			(ObjDummies[z].Envmap && (EnvmapTurnOn || EnvmapChange)) ||
			(NewCol && !ColorDiscard) ||
			BackChange ||
			BackFrontChange ||
			AepsilonChange;
			//||WireframeChange;

		//ha meg kell szakitani, akkor megszakitjuk :)
		if (Cut) glEnd();

		//zmask valtas, innen a nem zmaskos objectek jonnek (kulon listaba mert egy world tobb scenebol
		//allhat, es eloszor a zmaskos scene listakat kell renderelni, utana a nem zmaskosakat)
		if (ZmaskChange)
		{
			glEndList();
			glNewList(NoZmaskList,GL_COMPILE);
		}

		//alpha epsilon valtas
		if (AepsilonChange)
		{
			glAlphaFunc(GL_GEQUAL,Object->AEpsilon/255.0f);
		}

		//szinvaltas
		if (!ColorDiscard && NewCol) glColor4ub(Object->Color.c[0],Object->Color.c[1],Object->Color.c[2],Object->Color.c[3]);

		//backface cull ki/bekapcs
		if (BackChange)
			if (Object->Backface) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		//backface cull reverse
		if (BackFrontChange)
			if (Object->Backfront) glCullFace(GL_FRONT); else glCullFace(GL_BACK);


		//ha nem envmap az object
		if (!ObjDummies[z].Envmap)
		{
			//textura kikapcs
			if (TextureDisable) glDisable(GL_TEXTURE_2D);

			//texturavaltas
			if (TextureChange)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,Object->Material->ImageData.TextureHandle);
			}
			//blendmodevaltas
			if (BlendChange) glBlendFunc(Object->DSTBlend,Object->SRCBlend);
		}
		else
		//ha envmap az object
		{
			//envmap bekapcs
			if (EnvmapTurnOn)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,Object->EnvMap->ImageData.TextureHandle);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glBlendFunc(GL_ONE,GL_ONE);
			}
			//envmap textura valtas
			if (EnvmapChange) glBindTexture(GL_TEXTURE_2D,Object->EnvMap->ImageData.TextureHandle);				
		}

		/*if (WireframeChange)
		{
			if (!Object->Wireframe || ObjDummies[z].Envmap) 
			{
				if (Lighting)
				glEnable(GL_LIGHTING);
			}
			else 
			{
				Lighting=glIsEnabled(GL_LIGHTING)!=0;
				glDisable(GL_LIGHTING);
			}
		}*/

		//ha vagas volt, ujra kell glbegin
		if (Cut) 
			if (!Object->Wireframe || ObjDummies[z].Envmap)
			glBegin(GL_TRIANGLES);
			else
			glBegin(GL_LINES);

		if (!Object->Wireframe || ObjDummies[z].Envmap)
		{
			for (x=0; x<Object->PolygonNum; x++)
			{
				if (Object->PolygonList[x].CurrentShading!=aDDict_GOURAUDSHADE) 
					glNormal3f(Object->PolygonList[x].CurrentNormal.x,Object->PolygonList[x].CurrentNormal.y,Object->PolygonList[x].CurrentNormal.z);
				
				//vertex1

				if (Object->PolygonList[x].CurrentShading==aDDict_GOURAUDSHADE) 
					glNormal3f(Object->VertexList[Object->PolygonList[x].v[0]].CurrentNormal.x,Object->VertexList[Object->PolygonList[x].v[0]].CurrentNormal.y,Object->VertexList[Object->PolygonList[x].v[0]].CurrentNormal.z);
				if (Object->Textured) glTexCoord2f(Object->PolygonList[x].ct[0].x,Object->PolygonList[x].ct[0].y);
				glVertex3f(Object->VertexList[Object->PolygonList[x].v[0]].MapTransformedPosition.x,Object->VertexList[Object->PolygonList[x].v[0]].MapTransformedPosition.y,Object->VertexList[Object->PolygonList[x].v[0]].MapTransformedPosition.z);

				//vertex2
				
				if (Object->PolygonList[x].CurrentShading==aDDict_GOURAUDSHADE) 
					glNormal3f(Object->VertexList[Object->PolygonList[x].v[1]].CurrentNormal.x,Object->VertexList[Object->PolygonList[x].v[1]].CurrentNormal.y,Object->VertexList[Object->PolygonList[x].v[1]].CurrentNormal.z);
				if (Object->Textured) glTexCoord2f(Object->PolygonList[x].ct[1].x,Object->PolygonList[x].ct[1].y);			
				glVertex3f(Object->VertexList[Object->PolygonList[x].v[1]].MapTransformedPosition.x,Object->VertexList[Object->PolygonList[x].v[1]].MapTransformedPosition.y,Object->VertexList[Object->PolygonList[x].v[1]].MapTransformedPosition.z);

				//vertex3
				
				if (Object->PolygonList[x].CurrentShading==aDDict_GOURAUDSHADE) 
					glNormal3f(Object->VertexList[Object->PolygonList[x].v[2]].CurrentNormal.x,Object->VertexList[Object->PolygonList[x].v[2]].CurrentNormal.y,Object->VertexList[Object->PolygonList[x].v[2]].CurrentNormal.z);
				if (Object->Textured) glTexCoord2f(Object->PolygonList[x].ct[2].x,Object->PolygonList[x].ct[2].y);
				glVertex3f(Object->VertexList[Object->PolygonList[x].v[2]].MapTransformedPosition.x,Object->VertexList[Object->PolygonList[x].v[2]].MapTransformedPosition.y,Object->VertexList[Object->PolygonList[x].v[2]].MapTransformedPosition.z);
			}

			/*if (ObjDummies[z].Envmap)
			{
				glEnd();
				
				glColor4f(0,1,0,1);
				glDisable(GL_BLEND);
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_TEXTURE_2D);
				glBlendFunc(GL_ONE,GL_ZERO);
				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				for (x=0; x<Object->VertexNum; x++)
				{
					glVertex3f(Object->VertexList[x].MapTransformedPosition.x,Object->VertexList[x].MapTransformedPosition.y,Object->VertexList[x].MapTransformedPosition.z);
					glVertex3f(Object->VertexList[x].MapTransformedPosition.x+Object->VertexList[x].CurrentNormal.x/5.0f,
							   Object->VertexList[x].MapTransformedPosition.y+Object->VertexList[x].CurrentNormal.y/5.0f,
							   Object->VertexList[x].MapTransformedPosition.z+Object->VertexList[x].CurrentNormal.z/5.0f);
				}
				glEnd();
			}


			glBegin(GL_TRIANGLES);*/

		}
		else
		{
			glEnd();
			glPushAttrib(GL_LIGHTING_BIT);
			glPushAttrib(GL_TEXTURE_BIT);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINES);
			for (x=0; x<Object->EdgeNum; x++)
				if (Object->EdgeList[x].InWireframe)
				{
					glVertex3f(Object->VertexList[Object->EdgeList[x].v1].MapTransformedPosition.x,Object->VertexList[Object->EdgeList[x].v1].MapTransformedPosition.y,Object->VertexList[Object->EdgeList[x].v1].MapTransformedPosition.z);
					glVertex3f(Object->VertexList[Object->EdgeList[x].v2].MapTransformedPosition.x,Object->VertexList[Object->EdgeList[x].v2].MapTransformedPosition.y,Object->VertexList[Object->EdgeList[x].v2].MapTransformedPosition.z);
				}
			glEnd();

			glPopAttrib();
			glPopAttrib();
			glBegin(GL_LINES);
		}

	}

	//ha elertunk minden vegere, lezarjuk a stuffot
	glEnd();
	glEndList();
}
