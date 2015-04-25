#include "IntroEditorEffects.h"
#include "glext.h"
#include "writer.h"

#ifdef INCLUDE_EVENT_CAVE
#include "Cubes.h"
#endif

CAMERA CamBuffer;
VECTOR3 EyeShake,TargetShake;

PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
RENDERTEXTURESLOT RenderTextures[6];

/*PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;*/

#ifdef INCLUDE_EVENT_FUNKENDSCROLL
CWriter *EndScrollText;
#endif

int TXR,TYR;

#define depth 100
GLuint Lists[depth]; //CAVE DEPTH
int LastTime;
int time;


#ifdef INCLUDE_EVENT_MRCEFFECT
void InitMrcEffect(float u1, float v1, float u2, float v2);
#endif

void InitIntroEditorEffects() 
{
	char * ext = (char*)glGetString(GL_EXTENSIONS);
	if (!strstr(ext,"GL_ARB_multitexture") /*|| !strstr(ext,"GL_ARB_vertex_buffer_object")*/) 
	{
		MessageBox( 0, "Required extensions not supported. IBJ...", "Error!", MB_OK );
		exit(0);
	}
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	if (!glMultiTexCoord2fARB) exit(0);
	
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	if (!glActiveTextureARB) exit(0);

	/*glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	if (!glGenBuffersARB) exit(0);
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	if (!glBindBufferARB) exit(0);
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	if (!glBufferDataARB) exit(0);
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	if (!glDeleteBuffersARB) exit(0);*/

	for (int x=0; x<6; x++)
	{
		glGenTextures(1,&RenderTextures[x].TexImage);
		glBindTexture(GL_TEXTURE_2D,RenderTextures[x].TexImage);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
	TXR=TYR=256;
	while (TXR<XRes) TXR<<=1;
	while (TYR<YRes) TYR<<=1;

#ifdef INCLUDE_EVENT_FUNKENDSCROLL
	EndScrollText=new CWriter(); 
#endif


#ifdef INCLUDE_EVENT_CAVE
	glDisable(GL_TEXTURE_2D);
	for (int a=0; a<depth; a++) Lists[a]=glGenLists(1);
	time=0;
	LastTime=timeGetTime();
	
#endif
	
#ifdef INCLUDE_EVENT_MRCEFFECT
	InitMrcEffect( 0, 1.0f-600.0f/1024.0f, 800.0f/1024.0f, 1);
#endif
	glDisable(GL_TEXTURE_2D);
}

#ifdef INCLUDE_EVENT_NOEFFECT
void __stdcall NoEffect(DEFAULTEVENTDATA *DEData, void *EffectData) 
{
	Done=true;
}
#endif

#ifdef INCLUDE_EVENT_2DLAYER
void __stdcall Layer2d(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	LAYER2DTEMPLATE d=*(LAYER2DTEMPLATE*)EffectData;
	if (d.Textured && d.Texture) 
	{
		glBindTexture(GL_TEXTURE_2D,d.Texture->ImageData.TextureHandle);
		glEnable(GL_TEXTURE_2D);
	}
	else glDisable(GL_TEXTURE_2D);
	glColor4ub((unsigned char)LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos),
		       (unsigned char)LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos),
			   (unsigned char)LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos),
			   (unsigned char)LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos));
	glEnable(GL_BLEND);
	glBlendFunc(d.SrcBlend,d.DstBlend);
	glBegin(GL_QUADS);

	glTexCoord2i(0,0);
	glVertex2i(DEData->x1,DEData->y1);
	glTexCoord2i(1,0);
	glVertex2i(DEData->x2,DEData->y1);
	glTexCoord2i(1,1);
	glVertex2i(DEData->x2,DEData->y2);
	glTexCoord2i(0,1);
	glVertex2i(DEData->x1,DEData->y2);

	glEnd();
}
#endif

#ifdef INCLUDE_EVENT_PARTICLECALC
void __stdcall CalcParticle(DEFAULTEVENTDATA *DEData, void *EffectData)
{
#ifdef INCLUDE_PARTICLE_ENGINE
	PARTICLETEMPLATE d=*(PARTICLETEMPLATE*)EffectData;
	if (d.World)
	{
		float TimePos=LinearInterpolate(d.AnimStart,d.AnimEnd,DEData->TimePos);
		if (d.World->AnimNum > d.AnimID)
		for (int x=0; x<d.World->ObjectNum; x++)
		/*if (d.World->ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
			d.World->ObjectList[x].Primitive==aDDict_SPHEREEMITTER ||
			d.World->ObjectList[x].Primitive==aDDict_CYLINDEREMITTER ||
			d.World->ObjectList[x].Primitive==aDDict_SPHEREDEFLECTOR ||
			d.World->ObjectList[x].Primitive==aDDict_PLANEDEFLECTOR ||
			d.World->ObjectList[x].Primitive==aDDict_GRAVITY)*/
		{
			DEFAULTOBJECTSPLINES *s=&d.World->ObjectList[x].Animations[d.AnimID];
			if (s->Posx->KeyNum) d.World->ObjectList[x].PosData.Pos.x=s->Posx->GetInterpolatedValue(TimePos);
			if (s->Posy->KeyNum) d.World->ObjectList[x].PosData.Pos.y=s->Posy->GetInterpolatedValue(TimePos);
			if (s->Posz->KeyNum) d.World->ObjectList[x].PosData.Pos.z=s->Posz->GetInterpolatedValue(TimePos);
			if (s->Sclx->KeyNum) d.World->ObjectList[x].PosData.Scale.x=s->Sclx->GetInterpolatedValue(TimePos);
			if (s->Scly->KeyNum) d.World->ObjectList[x].PosData.Scale.y=s->Scly->GetInterpolatedValue(TimePos);
			if (s->Sclz->KeyNum) d.World->ObjectList[x].PosData.Scale.z=s->Sclz->GetInterpolatedValue(TimePos);
			if (s->Prtfrme)
			if (s->Prtfrme->KeyNum) d.World->ObjectList[x].EmitterData->ParticlesPerFrame=s->Prtfrme->GetInterpolatedValue(TimePos);
			if (s->Rotx->KeyNum)
			{
				Spline_Slerp(s->Rotx,
					         s->Roty,
							 s->Rotz,
							 s->Rotw,
							 &d.World->ObjectList[x].PosData.Quaternion.s,
							 &d.World->ObjectList[x].PosData.Quaternion.v.x,
							 &d.World->ObjectList[x].PosData.Quaternion.v.y,
							 &d.World->ObjectList[x].PosData.Quaternion.v.z,TimePos);
			}
		}

		if (!DEData->OnScreenInLastFrame)
		{
			for (int x=0; x<d.World->ObjectNum; x++)
				if (d.World->ObjectList[x].EmitterData) d.World->ObjectList[x].EmitterData->LastFrameChecked=DEData->CurrentFrame;
		}

		d.World->CalculateObjectMatrices();
		d.World->CalculateParticles(DEData->CurrentFrame);
	}/**/
#endif
}
#endif

#ifdef INCLUDE_EVENT_CLEAR
void __stdcall Clear(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	CLEARTEMPLATE d=*(CLEARTEMPLATE*)EffectData;
	glClearColor(d.Col.r/255.0f,d.Col.g/255.0f,d.Col.b/255.0f,d.Col.a/255.0f);
	if (d.Screen && !d.Zbuffer) glClear(GL_COLOR_BUFFER_BIT);
	if (!d.Screen && d.Zbuffer) glClear(GL_DEPTH_BUFFER_BIT);
	if (d.Screen && d.Zbuffer) glClear(0x4100);
}
#endif

#ifdef INCLUDE_EVENT_RENDERTOTEXTURE
void __stdcall R2Txt(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	R2TXTTEMPLATE d=*(R2TXTTEMPLATE*)EffectData;
	RenderTextures[d.RenderTexture].x1=(DEData->x1)/(float)(TXR-1);
	RenderTextures[d.RenderTexture].x2=(DEData->x2+0.2f)/(float)(TXR-1);
	RenderTextures[d.RenderTexture].y1=(DEData->y2+1.5f)/(float)(TYR-1);
	RenderTextures[d.RenderTexture].y2=(DEData->y1+1)/(float)(TYR-1);
	glBindTexture(GL_TEXTURE_2D,RenderTextures[d.RenderTexture].TexImage);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,0,0,TXR,TYR,0);
}
#endif

#ifdef INCLUDE_EVENT_FEEDBACK
void __stdcall Feedback(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	FEEDBACKTEMPLATE d=*(FEEDBACKTEMPLATE*)EffectData;
	glColor4ub((unsigned char)LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos),
			  (unsigned char)LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos),
			  (unsigned char)LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos),
			  (unsigned char)LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos));
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(d.SrcBlend,d.DstBlend);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	RENDERTEXTURESLOT &r = RenderTextures[d.RenderTexture];
	glBindTexture(GL_TEXTURE_2D,r.TexImage);

	float zoom = d.LayerZoom/255.0f/100.0f*(float)TXR/(float)TYR;
	float xd=XRes/800.0f*1024.0f/(float)TXR;
	float yd=YRes/600.0f*1024.0f/(float)TXR;

	float asp=(DEData->x2-DEData->x1)/(float)(DEData->y2-DEData->y1) 
		       //DEData->Aspect
			   *(float)TYR/(float)TXR;

	for (int i=0; i<d.LayerNum; i++) {
		glPushMatrix();
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(r.x1+i*zoom*asp*xd,r.y1-i*zoom*yd); glVertex2f((float)DEData->x1,(float)DEData->y1); 
			glTexCoord2f(r.x1+i*zoom*asp*xd,r.y2+i*zoom*yd); glVertex2f((float)DEData->x1,(float)DEData->y2);
			glTexCoord2f(r.x2-i*zoom*asp*xd,r.y1-i*zoom*yd); glVertex2f((float)DEData->x2,(float)DEData->y1);
			glTexCoord2f(r.x2-i*zoom*asp*xd,r.y2+i*zoom*yd); glVertex2f((float)DEData->x2,(float)DEData->y2);
		glEnd();
		glPopMatrix();
	 	if (i!=d.LayerNum) glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,0,0,TXR,TYR,0);
	}/**/
}
#endif

#ifdef INCLUDE_EVENT_BLUR
void __stdcall Blur(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	BLURFXTEMPLATE d=*(BLURFXTEMPLATE*)EffectData;
	glColor4ub((unsigned char)LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos),
			   (unsigned char)LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos),
			   (unsigned char)LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos),
			   (unsigned char)(LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos)/2.0f));
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(d.SrcBlend,d.DstBlend);
	glDisable(GL_LIGHTING);

	RENDERTEXTURESLOT &r = RenderTextures[d.RenderTexture];
	RenderTextures[d.RenderTexture].x1=(DEData->x1)/(float)(TXR-1);
	RenderTextures[d.RenderTexture].x2=(DEData->x2+0.2f)/(float)(TXR-1);
	RenderTextures[d.RenderTexture].y1=(DEData->y2+1.5f)/(float)(TYR-1);
	RenderTextures[d.RenderTexture].y2=(DEData->y1+1)/(float)(TYR-1);

	//WriteDebug("ez fagy -> %08X",(int)glActiveTextureARB);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,r.TexImage);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,r.TexImage);

	float half[]={0.5,0.5,0.5,0.5};
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_INTERPOLATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_CONSTANT_EXT);
//	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_ALPHA);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, half);

	float zoom = d.LayerZoom/255.0f/100.0f;//*(float)TXR/(float)TYR;
	float xd=XRes/800.0f*1024.0f/(float)TXR;
	float yd=YRes/600.0f*1024.0f/(float)TXR;

	for (int i=0; i<d.LayerNum; i++) {
		glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,0,0,TXR,TYR,0);
		glPushMatrix();
		glBegin(GL_QUADS);

			float zm=zoom*i;

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x1,r.y1+zm*yd);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x1,r.y1-zm*yd);
			glVertex2f((float)DEData->x1,(float)DEData->y1); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x1,r.y2+zm*yd);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x1,r.y2-zm*yd);
			glVertex2f((float)DEData->x1,(float)DEData->y2); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x2,r.y2+zm*yd);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x2,r.y2-zm*yd);
			glVertex2f((float)DEData->x2,(float)DEData->y2); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x2,r.y1+zm*yd);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x2,r.y1-zm*yd);
			glVertex2f((float)DEData->x2,(float)DEData->y1); 
		glEnd();

		glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,0,0,TXR,TYR,0);

		glBegin(GL_QUADS);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x1+zm*xd,r.y1);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x1-zm*xd,r.y1);
			glVertex2f((float)DEData->x1,(float)DEData->y1); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x1+zm*xd,r.y2);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x1-zm*xd,r.y2);
			glVertex2f((float)DEData->x1,(float)DEData->y2); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x2+zm*xd,r.y2);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x2-zm*xd,r.y2);
			glVertex2f((float)DEData->x2,(float)DEData->y2); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x2+zm*xd,r.y1);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x2-zm*xd,r.y1);
			glVertex2f((float)DEData->x2,(float)DEData->y1); 

		glEnd();
		glPopMatrix();
	}


}
#endif

#ifdef INCLUDE_EVENT_3DSCENE

int FindCamByID(WORLD *w, int CamID)
{
	for (int x=0; x<w->CamNum; x++) if (w->CamAnims[x].CamID==CamID) return x;
	return -1;
}

int FindAnimByID(WORLD *w, int CamID)
{
	for (int x=0; x<w->AnimNum; x++) if (w->Animations[x].AnimID==CamID) return x;
	return -1;
}

void __stdcall Display3d(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	DISPLAY3DTEMPLATE d=*(DISPLAY3DTEMPLATE*)EffectData;
	if (d.World)
	{
		float CamPos,TimePos;
		CamPos=LinearInterpolate(d.CamStart,d.CamEnd,DEData->TimePos);
		TimePos=LinearInterpolate(d.AnimStart,d.AnimEnd,DEData->TimePos);

		CAMERA Cam=CamBuffer;
		if (d.World->CamNum)
		{
			int cid=FindCamByID(d.World,d.CamID);
			CAMSPLINES c=d.World->CamAnims[cid];
			if (c.Eyex)
			{
				if (c.Eyex->KeyNum)
				{
					Cam.Fov=45;
					Cam.Up.x=0;
					Cam.Up.y=1;
					Cam.Up.z=0;

					Cam.Eye.x=c.Eyex->GetInterpolatedValue(CamPos);
					Cam.Eye.y=c.Eyey->GetInterpolatedValue(CamPos);
					Cam.Eye.z=c.Eyez->GetInterpolatedValue(CamPos);
					Cam.Target.x=c.Trgx->GetInterpolatedValue(CamPos);
					Cam.Target.y=c.Trgy->GetInterpolatedValue(CamPos);
					Cam.Target.z=c.Trgz->GetInterpolatedValue(CamPos);

					Cam.Eye=V3_Add(Cam.Eye,EyeShake);
					Cam.Target=V3_Add(Cam.Target,TargetShake);

					if (c.Fov->KeyNum)
					Cam.Fov=c.Fov->GetInterpolatedValue(CamPos);
					Cam.Roll=c.Roll->GetInterpolatedValue(CamPos);
					if (!d.DontSaveCam)
					CamBuffer=Cam;
				}
			}
		}

		if (d.World->AnimNum)
		for (int x=0; x<d.World->ObjectNum; x++)
		{
			DEFAULTOBJECTSPLINES *s=&d.World->ObjectList[x].Animations[FindAnimByID(d.World,d.AnimID)];
			if (s->Posx->KeyNum) d.World->ObjectList[x].PosData.Pos.x=s->Posx->GetInterpolatedValue(TimePos);
			if (s->Posy->KeyNum) d.World->ObjectList[x].PosData.Pos.y=s->Posy->GetInterpolatedValue(TimePos);
			if (s->Posz->KeyNum) d.World->ObjectList[x].PosData.Pos.z=s->Posz->GetInterpolatedValue(TimePos);
			if (s->Sclx->KeyNum) d.World->ObjectList[x].PosData.Scale.x=s->Sclx->GetInterpolatedValue(TimePos);
			if (s->Scly->KeyNum) d.World->ObjectList[x].PosData.Scale.y=s->Scly->GetInterpolatedValue(TimePos);
			if (s->Sclz->KeyNum) d.World->ObjectList[x].PosData.Scale.z=s->Sclz->GetInterpolatedValue(TimePos);
			if (s->Rotx->KeyNum)
			{
				Spline_Slerp(s->Rotx,
					         s->Roty,
							 s->Rotz,
							 s->Rotw,
							 &d.World->ObjectList[x].PosData.Quaternion.s,
							 &d.World->ObjectList[x].PosData.Quaternion.v.x,
							 &d.World->ObjectList[x].PosData.Quaternion.v.y,
							 &d.World->ObjectList[x].PosData.Quaternion.v.z,TimePos);
			}

			if (s->AnimID->KeyNum) d.World->ObjectList[x].APosData.AnimID=(int)s->AnimID->GetInterpolatedValue(TimePos);
			if (s->AnimTime->KeyNum) d.World->ObjectList[x].APosData.AnimPos=s->AnimTime->GetInterpolatedValue(TimePos);
			
			if (s->Red->KeyNum)   d.World->ObjectList[x].Color.r=(unsigned char)min(255,max(0,s->Red->GetInterpolatedValue(TimePos)));
			if (s->Green->KeyNum) d.World->ObjectList[x].Color.g=(unsigned char)min(255,max(0,s->Green->GetInterpolatedValue(TimePos)));
			if (s->Blue->KeyNum)  d.World->ObjectList[x].Color.b=(unsigned char)min(255,max(0,s->Blue->GetInterpolatedValue(TimePos)));
			if (s->Alpha->KeyNum) d.World->ObjectList[x].Color.a=(unsigned char)min(255,max(0,s->Alpha->GetInterpolatedValue(TimePos)));
		}


		d.World->CalculateObjectMatrices();
		glLoadIdentity();
		glViewport(DEData->x1,YRes-DEData->y2,DEData->x2-DEData->x1+1,DEData->y2-DEData->y1);
		glScissor(DEData->x1,YRes-DEData->y2,DEData->x2-DEData->x1+1,DEData->y2-DEData->y1);
		glEnable(GL_SCISSOR_TEST);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		SetCameraView(&Cam, DEData->Aspect);// (DEData->x2-DEData->x1)/(float)(DEData->y2-DEData->y1));
		//d.World->CalculateParticles();
		d.World->Render(false,false);
#ifdef INCLUDE_PARTICLE_ENGINE
		MATRIX m;
		M_Identity(m);
		d.World->RenderParticleTree(&Cam,d.AnimID,&m);
#endif
		//SwitchTo2D();*/
	}
}
#endif

#ifdef INCLUDE_EVENT_PARTICLERESET
void __stdcall ParticleReset(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	PARTICLERESETTEMPLATE d=*(PARTICLERESETTEMPLATE*)EffectData;
	if (d.World)
	{
		for (int x=0; x<d.World->ObjectNum; x++)
		if (d.World->ObjectList[x].EmitterData && d.World->ObjectList[x].EmitterData->Particles)
			for (int z=0; z<d.World->ObjectList[x].EmitterData->MaxParticles; z++)
			{
				d.World->ObjectList[x].EmitterData->Particles[z].Active=false;
			}

	}
}
#endif

#ifdef INCLUDE_EVENT_CONTRAST
void __stdcall Contrast(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	CONTRASTFXTEMPLATE d=*(CONTRASTFXTEMPLATE*)EffectData;
	glColor4ub((unsigned char)LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos),
			  (unsigned char)LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos),
			  (unsigned char)LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos),
			  (unsigned char)LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos));
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(d.SrcBlend,d.DstBlend);
	glDisable(GL_LIGHTING);

	RENDERTEXTURESLOT &r = RenderTextures[d.RenderTexture];
	RenderTextures[d.RenderTexture].x1=(DEData->x1)/(float)(TXR-1);
	RenderTextures[d.RenderTexture].x2=(DEData->x2+0.2f)/(float)(TXR-1);
	RenderTextures[d.RenderTexture].y1=(DEData->y2+1.5f)/(float)(TYR-1);
	RenderTextures[d.RenderTexture].y2=(DEData->y1+1)/(float)(TYR-1);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,r.TexImage);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,r.TexImage);

	for (int i=0; i<d.MulAmount+d.AddAmount; i++) {
		glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,0,0,TXR,TYR,0);
		glPushMatrix();

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (i<d.MulAmount)?GL_MODULATE:GL_ADD);
		glBegin(GL_QUADS);

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x1,r.y1);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x1,r.y1);
			glVertex2f((float)DEData->x1,(float)DEData->y1); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x1,r.y2);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x1,r.y2);
			glVertex2f((float)DEData->x1,(float)DEData->y2); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x2,r.y2);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x2,r.y2);
			glVertex2f((float)DEData->x2,(float)DEData->y2); 

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,r.x2,r.y1);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,r.x2,r.y1);
			glVertex2f((float)DEData->x2,(float)DEData->y1); 

		glEnd();

		glPopMatrix();
	}


}
#endif

#ifdef INCLUDE_EVENT_FUNKENDSCROLL
extern "C" char scrolltext[];

void __stdcall EndScroll(DEFAULTEVENTDATA *DEData, void *EffectData)
{
//	glEnable(GL_TEXTURE_2D);
	glScissor(1,1,XRes-2,YRes-2);
	glEnable(GL_SCISSOR_TEST);
	EndScrollText->Print(
		(float)DEData->x1,
		(float)DEData->y1,
		(float)DEData->x2,
		(float)DEData->y2,
		500.0f*XRes/800.0f,scrolltext);
}
#endif

#ifdef INCLUDE_EVENT_CRYSTALIZE
void __stdcall Crystalize(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	CRYSTALIZEFXTEMPLATE d=*(CRYSTALIZEFXTEMPLATE*)EffectData;
	GLUquadricObj* Cone=gluNewQuadric();

	srand(d.RandSeed);
	RENDERTEXTURESLOT &r = RenderTextures[d.RenderTexture];
	glBindTexture(GL_TEXTURE_2D,r.TexImage);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glColor4ub(255,255,255,255);
	glEnable(GL_BLEND);
	glBlendFunc(d.SrcBlend,d.DstBlend);

	glLoadIdentity();
	glViewport(DEData->x1,YRes-DEData->y2,DEData->x2-DEData->x1+1,DEData->y2-DEData->y1);
	glScissor(DEData->x1,YRes-DEData->y2,DEData->x2-DEData->x1+1,DEData->y2-DEData->y1);
	glEnable(GL_SCISSOR_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int XSize=DEData->x2-DEData->x1;
	int YSize=DEData->y2-DEData->y1;

	float Time=LinearInterpolate(d.aStart,d.aEnd,DEData->TimePos)/255.0f;
	float CrystalSize=LinearInterpolate(d.sCrystalSize,d.eCrystalSize,Time);

	for (int x=0; x<d.CrystalNum; x++)
	{
		float sxp,syp;
		//sxp=(rand()/(float)RAND_MAX*(XSize-1)*2)+1;
		//syp=(rand()/(float)RAND_MAX*(YSize-1)*2)+1;

		sxp=rand()/(float)RAND_MAX;
		syp=rand()/(float)RAND_MAX;

		float exp=-1,eyp=-1;
		//exp=sxp+(rand()/(float)RAND_MAX-0.5f)*2*d.AnimChaos*XSize;
		//eyp=syp+(rand()/(float)RAND_MAX-0.5f)*2*d.AnimChaos*YSize;

		/*exp=sxp+(rand()/(float)RAND_MAX-0.5f)*2*d.AnimChaos+10;
		exp-=(int)exp;
		eyp=syp+(rand()/(float)RAND_MAX-0.5f)*2*d.AnimChaos+10;
		eyp-=(int)eyp;*/

		while (!(exp>=0 && exp<=1))
		exp=sxp+(rand()/(float)RAND_MAX-0.5f)*2*d.AnimChaos;

		while (!(eyp>=0 && eyp<=1))
		eyp=syp+(rand()/(float)RAND_MAX-0.5f)*2*d.AnimChaos;

		float xp,yp;
		xp=LinearInterpolate(sxp,exp,Time)*(XSize-1)*2+1;
		yp=LinearInterpolate(syp,eyp,Time)*(YSize-1)*2+1;

		glTexCoord2f((r.x2-r.x1)*xp/XSize/2.0f+r.x1,r.y2-(r.y2-r.y1)*yp/YSize/2.0f);
		glPushMatrix();
		glTranslatef(xp/XSize-1,yp/YSize-1,0);
		gluCylinder(Cone,0,CrystalSize,1,d.Detail,1);
		glPopMatrix();
	}

	gluDeleteQuadric(Cone);
	srand(timeGetTime());
}
#endif

#ifdef INCLUDE_EVENT_GRIDFILLER

/*int GridTriangles[40*40*4];

struct GRIDVECTOR
{
	VECTOR2 T2F;
	VECTOR3 N3F;
	VECTOR3 V3F;
};

GRIDVECTOR Grid[41*41];

void InitGrid()
{
	for (int x=0; x<41; x++)
		for (int y=0; y<41; y++)
			Grid[y*41+x].V3F=V3_Make(x*20.0f,74+y*11.25f,0);
	for (x=0; x<40; x++)
		for (int y=0; y<40; y++)
		{
			GridTriangles[(y*40+x)*4]=y*41+x;
			GridTriangles[(y*40+x)*4+1]=y*41+x+1;
			GridTriangles[(y*40+x)*4+2]=(y+1)*41+x+1;
			GridTriangles[(y*40+x)*4+3]=(y+1)*41+x;
		}
}

void __stdcall Gridfiller(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	glDisable(GL_DEPTH_TEST);
	GRIDFILLERTEMPLATE d=*(GRIDFILLERTEMPLATE*)EffectData;
	float a[4];

	for (int y=0; y<41; y++)
		for (int x=0; x<41; x++)
		{
			for (int z=0; z<4; z++)
				a[z]=(float)sin( (x*d.XSin[z]/100.0f + y*d.YSin[z]/100.0f + DEData->TimePos*d.WSin[z]*d.Repeat) * 3.1415 * d.ZSin[z]);

			Grid[y*41+x].T2F.u=a[0]-a[1];
			Grid[y*41+x].T2F.v=a[2]-a[3];
			Grid[y*41+x].N3F=V3_Normalize(V3_Make(-Grid[y*41+x].T2F.u,-Grid[y*41+x].T2F.v,-1));
		}

	if (d.Texture && d.Textured)
	{
		glBindTexture(GL_TEXTURE_2D,d.Texture->ImageData.TextureHandle);
		glEnable(GL_TEXTURE_2D);
	}

	glColor4ub((unsigned char)LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos),
		       (unsigned char)LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos),
			   (unsigned char)LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos),
			   (unsigned char)LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos));
	glEnable(GL_BLEND);
	glBlendFunc(d.SrcBlend,d.DstBlend);

	if (d.Shade) 
	{
		glEnable(GL_LIGHTING);
		for (int x=0; x<8; x++) glDisable(GL_LIGHT0+x);
		glEnable(GL_LIGHT0);

		float f[4];
		
		f[0]=f[1]=f[2]=f[3]=0;
		glLightfv(GL_LIGHT0,GL_AMBIENT,f);
		f[0]=f[1]=f[2]=f[3]=1;
		glLightfv(GL_LIGHT0,GL_DIFFUSE,f);
		glLightfv(GL_LIGHT0,GL_SPECULAR,f);
		glLightfv(GL_LIGHT0,GL_POSITION,f);
	}

	glInterleavedArrays(GL_T2F_N3F_V3F,0,Grid);
	glDrawElements(GL_QUADS,40*40*4,GL_UNSIGNED_INT,GridTriangles);
}*/

void __stdcall Gridfiller(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	glDisable(GL_DEPTH_TEST);
	GRIDFILLERTEMPLATE d=*(GRIDFILLERTEMPLATE*)EffectData;
	float a[4];

	VECTOR2 Grid[41][41];
	VECTOR3 Norm[41][41];

	int x;
	for (x=0; x<41; x++)
		for (int y=0; y<41; y++)
		{
			for (int z=0; z<4; z++)
				a[z]=(float)sin( (x*d.XSin[z]/100.0f + y*d.YSin[z]/100.0f + DEData->TimePos*d.WSin[z]*d.Repeat) * 3.1415 * d.ZSin[z]);

			Grid[x][y].u=a[0]-a[1];
			Grid[x][y].v=a[2]-a[3];
			Norm[x][y]=V3_Normalize(V3_Make(-Grid[x][y].u,-Grid[x][y].v,-1));
		}

	if (d.Texture && d.Textured)
	{
		glBindTexture(GL_TEXTURE_2D,d.Texture->ImageData.TextureHandle);
		glEnable(GL_TEXTURE_2D);
	}

	float XStep,YStep;
	XStep=(DEData->x2-DEData->x1)/40.0f;
	YStep=(DEData->y2-DEData->y1)/40.0f;

	glColor4f(LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos)/255.0f,
		      LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos)/255.0f,
			  LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos)/255.0f,
			  LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos)/255.0f);
	glEnable(GL_BLEND);
	glBlendFunc(d.SrcBlend,d.DstBlend);

	if (d.Shade) 
	{
		glEnable(GL_LIGHTING);
		for (int x=0; x<8; x++) glDisable(GL_LIGHT0+x);
		glEnable(GL_LIGHT0);

		float f[4];
		
		f[0]=f[1]=f[2]=f[3]=0;
		glLightfv(GL_LIGHT0,GL_AMBIENT,f);
		f[0]=f[1]=f[2]=f[3]=1;
		glLightfv(GL_LIGHT0,GL_DIFFUSE,f);
		glLightfv(GL_LIGHT0,GL_SPECULAR,f);
		glLightfv(GL_LIGHT0,GL_POSITION,f);
	}

	glBegin(GL_QUADS);
	for (    x=0; x<40; x++)
		for (int y=0; y<40; y++)
		{
			
			glNormal3fv(Norm[x][y].a);
			glTexCoord2f(Grid[x][y].u,Grid[x][y].v);
			glVertex2f(DEData->x1+XStep*x,DEData->y1+YStep*y);

			glNormal3fv(Norm[x+1][y].a);
			glTexCoord2f(Grid[x+1][y].u,Grid[x+1][y].v);
			glVertex2f(DEData->x1+XStep*(x+1),DEData->y1+YStep*y);
			
			glNormal3fv(Norm[x+1][y+1].a);
			glTexCoord2f(Grid[x+1][y+1].u,Grid[x+1][y+1].v);
			glVertex2f(DEData->x1+XStep*(x+1),DEData->y1+YStep*(y+1));
			
			glNormal3fv(Norm[x][y+1].a);
			glTexCoord2f(Grid[x][y+1].u,Grid[x][y+1].v);
			glVertex2f(DEData->x1+XStep*x,DEData->y1+YStep*(y+1));

		}
	glEnd();
}

#endif

#ifdef INCLUDE_EVENT_CAVE

#define gridx 40
#define gridy 30
float grids[4][gridx][gridy];

#define tunnels 15

struct tunnel
{
	float x,y;
	float r2,rs,ra,rd;
};

tunnel Tunnels[tunnels];

struct TR
{
	TRIANGLE t[5];
	char i;
};

TR Triangles[gridx][gridy];

struct TS
{
	float xa,ya,xs,ys;
	float bxa,bya,bxs,bys;
	float cxa,cya,cxs,cys;
};

TS ts[tunnels+1];

void InitTunnels(int Rands, float a, float b, float c,float d,float e, float f, float g, float h, float radius)
{
	srand(Rands);
	for (int x=0; x<=tunnels; x++)
	{
		do
		ts[x].xs=rand()/(float)RAND_MAX*a-a/2.0f; //7
		while (fabs(ts[x].xs)<0.1);
		
		do
		ts[x].ys=rand()/(float)RAND_MAX*b-b/2.0f; //7
		while (fabs(ts[x].ys)<0.1);

		do
		ts[x].xa=rand()/(float)RAND_MAX*c-c/2.0f; //0.6
		while (fabs(ts[x].xa)<0.1);
		
		do
		ts[x].ya=rand()/(float)RAND_MAX*d-d/2.0f; //0.6
		while (fabs(ts[x].ya)<0.1);
		
		ts[x].bxs=(rand()/(float)RAND_MAX*e-e/2.0f);
		ts[x].bys=(rand()/(float)RAND_MAX*f-f/2.0f);
		ts[x].bxa=(rand()/(float)RAND_MAX*g-g/2.0f)/2.0f;
		ts[x].bya=(rand()/(float)RAND_MAX*h-h/2.0f)/2.0f;
		Tunnels[x].rd=rand()/(float)RAND_MAX*(gridx*4.0f/3.0f)*radius+5;
		Tunnels[x].rs=rand()/(float)RAND_MAX*7.0f-3.5f;
		Tunnels[x].ra=rand()/(float)RAND_MAX*(gridx*4.0f/3.0f)/2.0f*radius;

	}
	Tunnels[0].rd=70;
	/*ts[0].xs=0.91;
	ts[0].ys=0.67;
	ts[0].xa=-0.17;
	ts[0].ya=0.1;*/
}

float CheckPoint(int x, int y)
{
	if (x==1 || x==gridx-1 || y==1 || y==gridy-1) return -100000;
	float p=0;
	for (int z=0; z<tunnels; z++)
	{
		p+=Tunnels[z].r2/((x-Tunnels[z].x)*(x-Tunnels[z].x)+(y-Tunnels[z].y)*(y-Tunnels[z].y));
	}
	return p;
}

void CalculateGrid(float grid[gridx][gridy], int timer)
{
	for (int a=0; a<tunnels; a++)
	{
		Tunnels[a].x=(float)(gridx*sin(timer*radtheta*ts[a].xs)*ts[a].xa+gridx/2.0f+gridx*cos(timer*radtheta*ts[a].bxs)*ts[a].bxa);
		Tunnels[a].y=(float)(gridy*cos(timer*radtheta*ts[a].ys)*ts[a].ya+gridy/2.0f+gridy*sin(timer*radtheta*ts[a].bys)*ts[a].bya);
		Tunnels[a].r2=(float)(max(5,Tunnels[a].rd+sin(timer*radtheta*Tunnels[a].rs)*Tunnels[a].ra));
	}
	for (int x=0; x<gridx; x++)
		for (int y=0; y<gridy; y++)
			grid[x][y]=CheckPoint(x,y);
}

VECTOR3 CalcNormal(int x,int y,int z)
{
	VECTOR3 buffer;
	buffer.x=(grids[z][x+1][y]-grids[z][x-1][y])/2.0f;
	buffer.y=(grids[z][x][y+1]-grids[z][x][y-1])/2.0f;
	buffer.z=-(grids[z+1][x][y]-grids[z-1][x][y])/2.0f;
	return V3_Normalize(buffer);
}

void CalculateCave(bool Solid, bool Wireframe, int skip)
{
	time+=skip;
	for (int z=0; z<3; z++)	memcpy(grids[z],grids[z+1],gridx*gridy*sizeof(float));
	CalculateGrid(grids[3],time);
	GRIDCELL g;
	
	//glTranslatef(0,0,depth-4);
	
	for (int a=0; a<depth; a++)
	{
		//if (draw && a) glCallList(Lists[a]);
		Lists[a]=Lists[(a+1)%depth];
	}

	glNewList(Lists[depth-1],GL_COMPILE);
	glBegin(GL_TRIANGLES);
	int x;
	for (x=1; x<gridx-2; x++)
		for (int y=1; y<gridy-2; y++)	
		{
			g.p[0].x=(float)x; 	g.p[0].y=(float)y+1;	g.p[0].z=1;	g.val[0]=grids[2][x][y+1];   g.n[0]=CalcNormal(x,y+1,2);			
			g.p[1].x=(float)x+1;	g.p[1].y=(float)y+1;	g.p[1].z=1; g.val[1]=grids[2][x+1][y+1]; g.n[1]=CalcNormal(x+1,y+1,2);
			g.p[2].x=(float)x+1; 	g.p[2].y=(float)y+1; 	g.p[2].z=0;	g.val[2]=grids[1][x+1][y+1]; g.n[2]=CalcNormal(x+1,y+1,1);
			g.p[3].x=(float)x; 	g.p[3].y=(float)y+1; 	g.p[3].z=0; g.val[3]=grids[1][x][y+1];   g.n[3]=CalcNormal(x,y+1,1);
			g.p[4].x=(float)x; 	g.p[4].y=(float)y; 	g.p[4].z=1;	g.val[4]=grids[2][x][y];     g.n[4]=CalcNormal(x,y,2);
			g.p[5].x=(float)x+1;	g.p[5].y=(float)y; 	g.p[5].z=1; g.val[5]=grids[2][x+1][y];   g.n[5]=CalcNormal(x+1,y,2);
			g.p[6].x=(float)x+1; 	g.p[6].y=(float)y;  	g.p[6].z=0;	g.val[6]=grids[1][x+1][y];   g.n[6]=CalcNormal(x+1,y,1);
			g.p[7].x=(float)x; 	g.p[7].y=(float)y;  	g.p[7].z=0; g.val[7]=grids[1][x][y];     g.n[7]=CalcNormal(x,y,1);
			Triangles[x][y].i=Polygonise(g,10,Triangles[x][y].t);
			if (Solid)
			for (int b=0; b<Triangles[x][y].i; b++)
			{			
				for (int c=0; c<3; c++)					
				{
					glNormal3d(Triangles[x][y].t[b].n[c].x,Triangles[x][y].t[b].n[c].y,Triangles[x][y].t[b].n[c].z);
					glTexCoord2f((float)Triangles[x][y].t[b].p[c].x/(float)gridx*3,(float)(Triangles[x][y].t[b].p[c].z+time)/(float)gridx*3);
					glVertex3d(Triangles[x][y].t[b].p[c].x-gridx/2.0f,
						Triangles[x][y].t[b].p[c].y-gridy/2.0,
						(1-Triangles[x][y].t[b].p[c].z-(depth-1)));/**/
				}
			}
		}
	glEnd();

	if (Wireframe)
	{
		glColor4ub(255,255,255,255);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		float wd=0.05f;
		for (x=1; x<gridx-1; x++)
			for (int y=1; y<gridy-1; y++)	
			{
				for (int b=0; b<Triangles[x][y].i; b++)
				{
					glVertex3d(Triangles[x][y].t[b].p[0].x-gridx/2.0+Triangles[x][y].t[b].n[0].x*wd,Triangles[x][y].t[b].p[0].y-gridy/2.0+Triangles[x][y].t[b].n[0].y*wd,(1-Triangles[x][y].t[b].p[0].z-(depth-1))+Triangles[x][y].t[b].n[0].z*wd);
					glVertex3d(Triangles[x][y].t[b].p[1].x-gridx/2.0+Triangles[x][y].t[b].n[1].x*wd,Triangles[x][y].t[b].p[1].y-gridy/2.0+Triangles[x][y].t[b].n[1].y*wd,(1-Triangles[x][y].t[b].p[1].z-(depth-1))+Triangles[x][y].t[b].n[1].z*wd);

					glVertex3d(Triangles[x][y].t[b].p[0].x-gridx/2.0+Triangles[x][y].t[b].n[0].x*wd,Triangles[x][y].t[b].p[0].y-gridy/2.0+Triangles[x][y].t[b].n[0].y*wd,(1-Triangles[x][y].t[b].p[0].z-(depth-1))+Triangles[x][y].t[b].n[0].z*wd);
					glVertex3d(Triangles[x][y].t[b].p[2].x-gridx/2.0+Triangles[x][y].t[b].n[2].x*wd,Triangles[x][y].t[b].p[2].y-gridy/2.0+Triangles[x][y].t[b].n[2].y*wd,(1-Triangles[x][y].t[b].p[2].z-(depth-1))+Triangles[x][y].t[b].n[2].z*wd);

					glVertex3d(Triangles[x][y].t[b].p[1].x-gridx/2.0+Triangles[x][y].t[b].n[1].x*wd,Triangles[x][y].t[b].p[1].y-gridy/2.0+Triangles[x][y].t[b].n[1].y*wd,(1-Triangles[x][y].t[b].p[1].z-(depth-1))+Triangles[x][y].t[b].n[1].z*wd);
					glVertex3d(Triangles[x][y].t[b].p[2].x-gridx/2.0+Triangles[x][y].t[b].n[2].x*wd,Triangles[x][y].t[b].p[2].y-gridy/2.0+Triangles[x][y].t[b].n[2].y*wd,(1-Triangles[x][y].t[b].p[2].z-(depth-1))+Triangles[x][y].t[b].n[2].z*wd);
				}
			}
		glEnd();
		glPopAttrib();
	}
	
	glTranslatef(0,0,-1);
	glEndList();
}

void __stdcall Cave(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	if (!DEData->OnScreenInLastFrame)
	{
		LastTime=DEData->StartFrame*10;
	}
	CAVETEMPLATE dat=*(CAVETEMPLATE*)EffectData;

	if (dat.Textured && dat.Texture)	
	{
		glBindTexture(GL_TEXTURE_2D,dat.Texture->ImageData.TextureHandle);
		glEnable(GL_TEXTURE_2D);
	}
	else
	glDisable(GL_TEXTURE_2D);

	float col[4];
	col[0]=dat.r/255.0f;
	col[1]=dat.g/255.0f;
	col[2]=dat.b/255.0f;
	col[3]=1;
	glFogfv(GL_FOG_COLOR,col);
	glFogf(GL_FOG_DENSITY,0.045f);
	glHint(GL_FOG_HINT,GL_NICEST);
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogf(GL_FOG_START,depth/10.0);
	glFogf(GL_FOG_END,depth-10);
	glEnable(GL_FOG);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glLoadIdentity();
	glViewport(DEData->x1,YRes-DEData->y2,DEData->x2-DEData->x1+1,DEData->y2-DEData->y1);
	glScissor(DEData->x1,YRes-DEData->y2,DEData->x2-DEData->x1+1,DEData->y2-DEData->y1);
	glEnable(GL_SCISSOR_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	int tgt=DEData->CurrentFrame;
	int DestTime=(int)(((tgt-LastTime)/(1000.0f/(float)dat.fps)));
	
	for (int x=0; x<DestTime; x++)
	{
		CalculateCave(dat.Solid,dat.WireFrame,dat.skip);
		LastTime+=(int)((1000.0f/(float)dat.fps));
	}

	CAMERA Camera;
	int m=3;

	float a=(time-depth+m-1)*radtheta*ts[0].xs;
	float b=(time-depth+m-1)*radtheta*ts[0].bxs;
	float c=(time-depth+m-1)*radtheta*ts[0].ys;
	float d=(time-depth+m-1)*radtheta*ts[0].bys;

	Camera.Eye.x=(float)(gridx*sin(a)*ts[0].xa+gridx*cos(b)*ts[0].bxa); //kameramozgas
	Camera.Eye.y=(float)(gridy*cos(c)*ts[0].ya+gridy*sin(d)*ts[0].bya);
	Camera.Eye.z=(float)-m;

	Camera.Target.x=(float)(gridx*sin((time-depth+m)*radtheta*ts[0].xs)*ts[0].xa+gridx*cos((time-depth+m)*radtheta*ts[0].bxs)*ts[0].bxa); //kameramozgas
	Camera.Target.y=(float)(gridy*cos((time-depth+m)*radtheta*ts[0].ys)*ts[0].ya+gridy*sin((time-depth+m)*radtheta*ts[0].bys)*ts[0].bya);
	Camera.Target.z=-170000000;


	float derive=(float)((gridx*ts[0].xa*cos(a)*a-gridx*ts[0].bxa*sin(b)*b-gridy*ts[0].ya*sin(c)*c+gridy*ts[0].bya*cos(d)*d)/(float)time);

	Camera.Up.x=(float)sin(derive);
	Camera.Up.y=(float)cos(derive);
	Camera.Up.z=0;
	Camera.Fov=dat.Fov;
	Camera.Roll=0;
	SetCameraView(&Camera,DEData->Aspect);

	float pos[4];
	pos[0]=1;
	pos[1]=-1;
	pos[2]=1;
	pos[3]=0;
	if (dat.Lighting)
	glEnable(GL_LIGHTING);
	else
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_POSITION,pos);
	
	glColor4ub(255,255,255,255);

	glTranslatef(0,0,depth-4);
	
	//for (int z=0; z<depth; z++)
		//glCallList(Lists[z]);
	//Sleep(5);
	glCallLists(depth,GL_UNSIGNED_INT,Lists);
	glDisable(GL_CULL_FACE);
}
#endif

#ifdef INCLUDE_EVENT_CAMSHAKE
void __stdcall CamShake(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	SHAKETEMPLATE d=*(SHAKETEMPLATE*)EffectData;
	int LastKey,NextKey;
	float Intensity=LinearInterpolate(d.Start,d.End,DEData->TimePos);
	LastKey=DEData->CurrentFrame-DEData->CurrentFrame%(d.Freq*10);
	NextKey=LastKey+(d.Freq*10);
	
	float Lastint[2][3],Nextint[2][3];
	srand(LastKey);
	int x;
	for (x=0; x<2; x++) 
		for (int y=0; y<3; y++)
			Lastint[x][y]=(rand()/(float)RAND_MAX-0.5f)*2.0f*Intensity;
	
	srand(NextKey);
	for (x=0; x<2; x++) 
		for (int y=0; y<3; y++)
			Nextint[x][y]=(rand()/(float)RAND_MAX-0.5f)*2.0f*Intensity;

	float Time=(DEData->CurrentFrame-LastKey)/(float)(d.Freq*10);

	EyeShake=V3_Mults(V3_Make(d.Eye,d.Eye,d.Eye),0.1f);
	TargetShake=V3_Mults(V3_Make(d.Target,d.Target,d.Target),0.1f);

	for (x=0; x<3; x++) EyeShake.a[x]*=LinearInterpolate(Lastint[0][x],Nextint[0][x],Time);
	for (x=0; x<3; x++) TargetShake.a[x]*=LinearInterpolate(Lastint[1][x],Nextint[1][x],Time);
}
#endif

#ifdef INCLUDE_EVENT_MRCEFFECT
#define GRIDXSIZE 64
#define GRIDYSIZE 48
float sqrr(float n) { return n*n; }
#define pi 3.1415f
double offset[GRIDXSIZE+1][GRIDYSIZE+1][2];	// offset

void InitMrcEffect(float u1, float v1, float u2, float v2) 
{
	int i,j;
	double o,p;
	double reli, relj;

	for (i=0;i<=GRIDXSIZE;i++) {
		for (j=0;j<=GRIDYSIZE;j++) {

			reli=(float)i/GRIDXSIZE;
			relj=(float)j/GRIDYSIZE;
			o=(rand()-16384)/163840.0;
			p=(u2-u1)*reli+u1;
			if ((o+p)<u1) { o=p-u1; }
			if ((o+p)>u2) { o=p-u2; }
			offset[i][j][0]=o;

			o=(rand()-16384)/163840.0;
			p=(v2-v1)*relj+v1;
			if ((o+p)<v1) { o=p-v1; }
			if ((o+p)>v2) { o=p-v2; }
			offset[i][j][1]=o;

			//offset[i][j][1]=0.0;//rand()-16384)/163840.0;
		}
	}
}

void drawgrid(float grid[GRIDXSIZE+1][GRIDYSIZE+1][2], float x1, float y1, float x2, float y2)
{
				for (int i=0;i<GRIDXSIZE;i++) {
					for (int j=0;j<GRIDYSIZE;j++) {
						float _x1=(x2-x1)*((float)i/GRIDXSIZE)+x1;
						float _x2=(x2-x1)*((float)(i+1)/GRIDXSIZE)+x1;
						float _y1=(y2-y1)*((float)j/GRIDYSIZE)+y1;
						float _y2=(y2-y1)*((float)(j+1)/GRIDYSIZE)+y1;
						glBegin(GL_QUADS);
							glTexCoord2f(grid[i  ][j  ][0],grid[i  ][j  ][1]);
							glVertex2f(_x1,_y1);
							glTexCoord2f(grid[i+1][j  ][0],grid[i+1][j  ][1]);
							glVertex2f(_x2,_y1);
							glTexCoord2f(grid[i+1][j+1][0],grid[i+1][j+1][1]);
							glVertex2f(_x2,_y2);
							glTexCoord2f(grid[i  ][j+1][0],grid[i  ][j+1][1]);
							glVertex2f(_x1,_y2);
						glEnd();
					}
				}
}

void MakeMrcEffect(int fxtype, float time, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2,
				int i1, int i2, int i3, int i4, float f1, float f2, float f3, float f4) {

	int i,j;			// looper
	float reli,relj;		// normalized loop values
	float reli2,relj2;
	float angle;
	float u,v;
	float _x1,_y1,_x2,_y2;	// temp screen coords
	float deltau=(u2-u1)/GRIDXSIZE;
	float deltav=(v2-v1)/GRIDYSIZE;
	float deltax=(x2-x1)/GRIDXSIZE;
	float deltay=(y2-y1)/GRIDYSIZE;

	float f;				// temp shit

	float grid[GRIDXSIZE+1][GRIDYSIZE+1][2];	// da mighty grid :)

	if (time<0) { time=0; }
	if (time>1.0) { time=1.0; }
	u1+=0.002f;
	u2-=0.002f;
	v1-=0.002f;
	v2+=0.002f;

	switch (fxtype) {
	case 0: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						u=(float)((u2-u1)*reli+u1 + (sin(time*2+reli*20+relj*15)*0.05f + cos(time*2+reli*10+relj*25)*0.05f) * sqrr((float)sin(time*pi*0.5f)));
						if (u<u1) { u=u1; }
						if (u>u2) { u=u2; }
						v=(float)((v2-v1)*relj+v1 + (sin(time*2+reli*18+relj*25)*0.05f + sin(time*2+reli*28+relj*15)*0.05f) * sqrr((float)sin(time*pi*0.5f)));
						if (v>v1) { v=v1; }
						if (v<v2) { v=v2; }
						grid[i][j][0]=u;
						grid[i][j][1]=v;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);


				break;
			}

	case 1: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						f=(float)sqrt(sqrr(reli-0.5f)+sqrr(relj-0.5f))*2.0f;
						f=(float)pow(f,time*3.5f);
						f*=sqrr((float)sin(time*pi));
						grid[i][j][0]=((u2-u1)*reli+u1-0.5f*(u1+u2))*(1-f)+0.5f*(u1+u2);
						grid[i][j][1]=((v2-v1)*relj+v1-0.5f*(v1+v2))*(1-f)+0.5f*(v1+v2);
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 2: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						f=(float)sqrt(sqrr(reli-0.5f)+sqrr(relj-0.5f))*2;
						if (f>0.001f) {
						f=(float)pow(f,1/(4*sqrr((float)sin(time*pi))+1)-1);
						//f=1;
						//f=(1-f)*sqr(sin(time*pi))+f*(1-sqr(sin(time*pi)));
						} else { f=0; }
						grid[i][j][0]=((u2-u1)*reli+u1-0.5f*(u1+u2))*(f)+0.5f*(u1+u2);
						grid[i][j][1]=((v2-v1)*relj+v1-0.5f*(v1+v2))*(f)+0.5f*(v1+v2);
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 3: {
				
				time *=3.6f;
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						u=(float)((u2-u1)*reli+u1 + sqrr((float)sin(time*pi/180.0f*50))*offset[i][j][0]);
						if (u<u1) { u=u1; }
						if (u>u2) { u=u2; }
						v=(float)((v2-v1)*relj+v1 + sqrr((float)sin(time*pi/180.0f*50))*offset[i][j][1]);
						if (v>v1) { v=v1; }
						if (v<v2) { v=v2; }
						grid[i][j][0]=u;
						grid[i][j][1]=v;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 4: {
				time *=3.6f;
				
				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {

						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						grid[i][j][0]=(float)((u2-u1)*reli+u1 + sqrr((float)sin(time*pi/180*50))*offset[i][j][0]);
						grid[i][j][1]=(float)((v2-v1)*relj+v1 + sqrr((float)sin(time*pi/180*50))*offset[i][j][1]);
					}
				}

				for (i=0;i<GRIDXSIZE;i++) {
					for (j=0;j<GRIDYSIZE;j++) {
						_x1=(x2-x1)*((float)i/GRIDXSIZE)+x1;
						_x2=(x2-x1)*((float)(i+1)/GRIDXSIZE)+x1;
						_y1=(y2-y1)*((float)j/GRIDYSIZE)+y1;
						_y2=(y2-y1)*((float)(j+1)/GRIDYSIZE)+y1;
						glBegin(GL_QUADS);
							glTexCoord2f(grid[i][j][0],grid[i][j][1]);
							glVertex2f(_x1,_y1);
							glTexCoord2f(grid[i][j][0]+deltau,grid[i][j][1]);
							glVertex2f(_x2,_y1);
							glTexCoord2f(grid[i][j][0]+deltau,grid[i][j][1]+deltav);
							glVertex2f(_x2,_y2);
							glTexCoord2f(grid[i][j][0],grid[i][j][1]+deltav);
							glVertex2f(_x1,_y2);
						glEnd();
					}
				}

				break;
			}
	case 5: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						reli2=reli-0.5f;
						relj2=relj-0.5f;
						f=(float)sqrt(sqrr(reli2)+sqrr(relj2));

						if (f>0.01) {
							angle=(float)(atan(relj2/reli2));
						} else {
							angle=0;
						}
						if (reli2<0) { angle+=pi; }
						if (f<0.5) {
							if (i1) {
								angle+=25*sqrr(0.5f-f)*sqrr((float)sin(time*pi*0.5f));
							} else {
								angle-=25*sqrr(0.5f-f)*sqrr((float)sin(time*pi*0.5f));
							}
						}

						reli=(float)(f*cos(angle)+0.5f);
						relj=(float)(f*sin(angle)+0.5f);

						grid[i][j][0]=(u2-u1)*reli+u1;
						grid[i][j][1]=(v2-v1)*relj+v1;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	case 6: {
				
				for (i=0;i<=GRIDXSIZE;i++) {
					for (j=0;j<=GRIDYSIZE;j++) {
						reli=(float)i/GRIDXSIZE;
						relj=(float)j/GRIDYSIZE;
						reli2=reli-0.5f;
						relj2=relj-0.5f;
						f=(float)sqrt(sqrr(reli2)+sqrr(relj2));

						if (f>0.01) {
							angle=(float)(atan(relj2/reli2));
						} else {
							angle=0;
						}
						if (reli2<0) { angle-=pi; }

						f-=(float)(sqrr((float)sin(time*pi/180*400+f*10))*0.2 * sqrr((float)sin(time*pi/180*180)));
						

						reli=(float)(f*cos(angle)+0.5f);
						relj=(float)(f*sin(angle)+0.5f);

						grid[i][j][0]=(u2-u1)*reli+u1;
						grid[i][j][1]=(v2-v1)*relj+v1;
					}
				}

				drawgrid(grid,x1,y1,x2,y2);
				break;
			}
	}

}

void __stdcall MrcEffekt(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	MRCTEMPLATE d=*(MRCTEMPLATE*)EffectData;

	glColor4f(LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos)/255.0f,
		      LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos)/255.0f,
			  LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos)/255.0f,
			  LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos)/255.0f);
	glEnable(GL_BLEND);
	glBlendFunc(d.SrcBlend,d.DstBlend);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D,RenderTextures[d.RenderTexture].TexImage);

	MakeMrcEffect(d.EffectID ,LinearInterpolate(d.StartTime,d.EndTime,DEData->TimePos),(float)DEData->x1,(float)DEData->y1,(float)DEData->x2,(float)DEData->y2, 
		RenderTextures[d.RenderTexture].x1, RenderTextures[d.RenderTexture].y1, RenderTextures[d.RenderTexture].x2, RenderTextures[d.RenderTexture].y2,
			0, 0, 0, 0, 0, 0, 0, 0);
}

#endif

#ifdef INCLUDE_EVENT_FEEDBACK2
void __stdcall Feedback2(DEFAULTEVENTDATA *DEData, void *EffectData)
{
	FEEDBACKTEMPLATE d=*(FEEDBACKTEMPLATE*)EffectData;
	glColor4f(LinearInterpolate(d.StartCol.r,d.EndCol.r,DEData->TimePos)/255.0f,
			      LinearInterpolate(d.StartCol.g,d.EndCol.g,DEData->TimePos)/255.0f,
						LinearInterpolate(d.StartCol.b,d.EndCol.b,DEData->TimePos)/255.0f,
						LinearInterpolate(d.StartCol.a,d.EndCol.a,DEData->TimePos)/255.0f);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(d.SrcBlend,d.DstBlend);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	RENDERTEXTURESLOT &r = RenderTextures[d.RenderTexture];
	glBindTexture(GL_TEXTURE_2D,r.TexImage);

	float xc=(r.x1+r.x2)/2.0f;
	float yc=(r.y1+r.y2/*+1.0f/(float)(TYR)*/)/2.0f;
	float xs=(r.x2-r.x1/*-1.0f/(float)(TXR)*/)/2.0f;
	float ys=(r.y2-r.y1)/2.0f;

	glBegin(GL_QUADS);
	for (int i=0; i<d.LayerNum; i++) {
			float zoom = 1+i/(float)d.LayerNum/((float)d.LayerZoom/25.5f);
			glTexCoord2f(xc-xs/zoom,yc-ys/zoom); glVertex2f((float)DEData->x1,(float)DEData->y1); 
			glTexCoord2f(xc-xs/zoom,yc+ys/zoom); glVertex2f((float)DEData->x1,(float)DEData->y2);
			glTexCoord2f(xc+xs/zoom,yc+ys/zoom); glVertex2f((float)DEData->x2,(float)DEData->y2);
			glTexCoord2f(xc+xs/zoom,yc-ys/zoom); glVertex2f((float)DEData->x2,(float)DEData->y1);
	}
		glEnd();
	 	/*if (i!=d.LayerNum) 
		{
			//glBindTexture(GL_TEXTURE_2D,RenderTextures[5].TexImage);
			glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,51,768-650,1024,1024,0);
		}*/
}
#endif