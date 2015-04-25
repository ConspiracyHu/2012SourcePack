#include "ParticleEngine.h"
#include "3dEngine.h"
#include "includelist.h"
#ifdef INCLUDE_PARTICLE_ENGINE

GLuint *ParticleIndexBuffer;
PARTICLEVECTOR *ParticleVertices;
int MaxParticleNum;

void InitParticleIndexBuffer()
{
	//ez feltolti a particle index buffert, ami megadja hogy a vertexbufferben hogy vannak a
	//negyszogek, meg beallitja a particlek texturakoordinatait a bufferbe

	ParticleIndexBuffer=new GLuint[MaxParticleNum*4];
	int x;
	for (x=0; x<MaxParticleNum*4; x++) ParticleIndexBuffer[x]=x;
	ParticleVertices=new PARTICLEVECTOR[MaxParticleNum*4];
	memset(ParticleVertices,0,MaxParticleNum*4*sizeof(PARTICLEVECTOR));
	for (x=0; x<MaxParticleNum; x++) 
	{
		ParticleVertices[x*4+0].u=1;
		ParticleVertices[x*4+3].u=1;
		ParticleVertices[x*4+0].v=1;
		ParticleVertices[x*4+1].v=1;
	}
}

void WORLD::CalculateParticles(int CurrentFrame)
{
	//ezt nem kell piszkalnod, ha belenyulsz akkor csak optim miatt ;)
	int x;
	for (x=0; x<ObjectNum; x++)
	{
		switch (ObjectList[x].Primitive)
		{
		case aDDict_PLANEDEFLECTOR:
#ifdef INCLUDE_WRL_PLANEDEFLECTOR
			{
			VECTOR3 n=V3_Make(0,1,0);
			memcpy(ObjectList[x].Inverted,ObjectList[x].ModelView,16*4);
			M_Invert(ObjectList[x].Inverted);
			memcpy(ObjectList[x].IT,ObjectList[x].Inverted,16*4);
			M_Transpose(ObjectList[x].IT);
			M_Xformd(ObjectList[x].IT,n,n);
			ObjectList[x].EmitterData->n=n=V3_Normalize(n);
			ObjectList[x].EmitterData->d=ObjectList[x].ModelView[0][3]*n.x+ObjectList[x].ModelView[1][3]*n.y+ObjectList[x].ModelView[2][3]*n.z;
			}
#endif
			break;
		case aDDict_SPHEREDEFLECTOR:
#ifdef INCLUDE_WRL_SPHEREDEFLECTOR
			memcpy(ObjectList[x].Inverted,ObjectList[x].ModelView,16*4);
			M_Invert(ObjectList[x].Inverted);
			memcpy(ObjectList[x].IT,ObjectList[x].Inverted,16*4);
			M_Transpose(ObjectList[x].IT);
#endif
			break;
		}
	}
	for (x=0; x<ObjectNum; x++)
	{
		switch (ObjectList[x].Primitive)
		{
		case aDDict_CUBEEMITTER:
		case aDDict_SPHEREEMITTER:
		case aDDict_CYLINDEREMITTER:
			if (ObjectList[x].EmitterData->LastFrameChecked>CurrentFrame)
			{
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
					P=FindObjectByID(ParentID);
					M_Mult(P->ModelView,Parent,Parent);
					ParentID=P->ParentID;
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
						ObjectList[x].EmitterData->Particles[Particle].StartAge=ObjectList[x].EmitterData->Particles[Particle].Age=ObjectList[x].EmitterData->DefaultAge+(int)((Random())*ObjectList[x].EmitterData->AgeChaos)+(int)!ObjectList[x].EmitterData->Particles[Particle].Aging;
						ObjectList[x].EmitterData->Particles[Particle].Rotation=0;
						if (ObjectList[x].EmitterData->RandRotate)
						ObjectList[x].EmitterData->Particles[Particle].Rotation=(Random()+0.5f)*360.0f;
						ObjectList[x].EmitterData->Particles[Particle].RotChaos=(Random()+0.5f)*(float)ObjectList[x].EmitterData->RotChaos;
						
						ObjectList[x].EmitterData->Particles[Particle].Size=ObjectList[x].EmitterData->Size*(1+(rand()/(float)RAND_MAX*2-1)*ObjectList[x].EmitterData->Color2[4]/255.0f);

						for (int cc=0; cc<4; cc++)
						{
							if (cc!=3 || ObjectList[x].EmitterData->Color1[3])
							ObjectList[x].EmitterData->Particles[Particle].Color1[cc]=(unsigned char)max(0,min(255,(ObjectList[x].EmitterData->Color1[cc]+Random()*2*ObjectList[x].EmitterData->Color1[4]/255.0f)));
							else ObjectList[x].EmitterData->Particles[Particle].Color1[3]=0;

							if (cc!=3 || ObjectList[x].EmitterData->Color2[3])
							ObjectList[x].EmitterData->Particles[Particle].Color2[cc]=(unsigned char)max(0,min(255,(ObjectList[x].EmitterData->Color2[cc])));//+Random()*2*ObjectList[x].EmitterData->Color2[4]/255.0f)));
							else ObjectList[x].EmitterData->Particles[Particle].Color2[3]=0;
						}
						
						VECTOR3 Normal;

						ObjectList[x].EmitterData->Particles[Particle].Position=V3_Make(Random(),Random(),Random());
						//ObjectList[x].EmitterData->Particles[Particle].Speed=V3_Make(0,0,0);

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
						
						ObjectList[x].EmitterData->Particles[Particle].TailCnt=0;

						if (ObjectList[x].Primitive!=aDDict_CUBEEMITTER)
						{
							if (ObjectList[x].Primitive==aDDict_CYLINDEREMITTER) ObjectList[x].EmitterData->Particles[Particle].Position.y=0;
							Normal=V3_Mults(V3_Normalize(ObjectList[x].EmitterData->Particles[Particle].Position),0.5f);
							ObjectList[x].EmitterData->Particles[Particle].Position.x=Normal.x*((Random()+0.5f)*(1-ObjectList[x].EmitterData->Param1)+ObjectList[x].EmitterData->Param1);
							ObjectList[x].EmitterData->Particles[Particle].Position.z=Normal.z*((Random()+0.5f)*(1-ObjectList[x].EmitterData->Param1)+ObjectList[x].EmitterData->Param1);
						}
						if (ObjectList[x].Primitive==aDDict_CYLINDEREMITTER) ObjectList[x].EmitterData->Particles[Particle].Position.y=(Random()+0.5f)-0.5f;
						if (ObjectList[x].Primitive==aDDict_SPHEREEMITTER) ObjectList[x].EmitterData->Particles[Particle].Position.y=Normal.y*((Random()+0.5f)*(1-ObjectList[x].EmitterData->Param1)+ObjectList[x].EmitterData->Param1);
						
						//M_Xformd(ObjectList[x].ModelView,ObjectList[x].EmitterData->Particles[Particle].Position,ObjectList[x].EmitterData->Particles[Particle].Position);
						M_Xformd(Parent,ObjectList[x].EmitterData->Particles[Particle].Position,ObjectList[x].EmitterData->Particles[Particle].Position);

						if (ObjectList[x].ParentID>=0)
						{
							if (ObjectList[x].Parent)
							M_Xformd(((WORLDOBJECT*)ObjectList[x].Parent)->ModelView,ObjectList[x].EmitterData->Particles[Particle].Position,ObjectList[x].EmitterData->Particles[Particle].Position);
						}

						if (ObjectList[x].EmitterData->Tail)
						for (int a=0; a<256; a++)
						ObjectList[x].EmitterData->Particles[Particle].Tail[a]=ObjectList[x].EmitterData->Particles[Particle].Position;
					}

					for (int z=0; z<ObjectList[x].EmitterData->MaxParticles; z++)
					{
						if (ObjectList[x].EmitterData->Particles[z].Active)
						{
							int a;
							for (a=0; a<ObjectNum; a++)
							{
								switch (ObjectList[a].Primitive)
								{
								case aDDict_GRAVITY:
#ifdef INCLUDE_WRL_GRAVITY
									float ForcePower;
									VECTOR3 ForceDir;
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
#endif
									break;
								}
							}
							for (a=0; a<ObjectNum; a++)
							{
								switch (ObjectList[a].Primitive)
								{
								case aDDict_PLANEDEFLECTOR:
#ifdef INCLUDE_WRL_PLANEDEFLECTOR
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
#endif
									break;
								case aDDict_SPHEREDEFLECTOR:
#ifdef INCLUDE_WRL_SPHEREDEFLECTOR

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

										float b4ac=(float)sqrt(b*b-4*_a*(cc-0.25f));
										float t;

										t=(-b+b4ac)/(2*_a);
										if (t>=1) t=(-b-b4ac)/(2*_a);

										VECTOR3 p,d;
										d=p=V3_Add(Pos,V3_Mults(Spd,t)); // utkozesi pont,normal iranya
										M_Xformd(ObjectList[a].IT,d,d);
										M_Xformd(ObjectList[a].ModelView,p,p);
										d=V3_Normalize(d); //ellipszoid feluleti normal

										ObjectList[x].EmitterData->Particles[z].Speed=V3_Mults(V3_Add(ObjectList[x].EmitterData->Particles[z].Speed,V3_Mults(d,-2.0f*V3_Dot(d,ObjectList[x].EmitterData->Particles[z].Speed))),ObjectList[a].EmitterData->Param1);
										ObjectList[x].EmitterData->Particles[z].Position=V3_Add(p,V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,t-1.0f));									

										if (ObjectList[x].EmitterData->Particles[z].Tail)
										{
											ObjectList[x].EmitterData->Particles[z].TailCnt=(ObjectList[x].EmitterData->Particles[z].TailCnt+1)%((ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2);
											ObjectList[x].EmitterData->Particles[z].Tail[ObjectList[x].EmitterData->Particles[z].TailCnt]=p;
											ObjectList[x].EmitterData->Particles[z].TailCnt2=(ObjectList[x].EmitterData->Particles[z].TailCnt2+1)%ObjectList[x].EmitterData->TailLength2;
										}
									}
#endif
									break;
								}
							}
							ObjectList[x].EmitterData->Particles[z].LastPos=ObjectList[x].EmitterData->Particles[z].Position;
							ObjectList[x].EmitterData->Particles[z].Position=V3_Add(ObjectList[x].EmitterData->Particles[z].Position,V3_Mults(ObjectList[x].EmitterData->Particles[z].Speed,1));
							ObjectList[x].EmitterData->Particles[z].DisplayPos.x=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.x,ObjectList[x].EmitterData->Particles[z].Position.x,t);
							ObjectList[x].EmitterData->Particles[z].DisplayPos.y=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.y,ObjectList[x].EmitterData->Particles[z].Position.y,t);
							ObjectList[x].EmitterData->Particles[z].DisplayPos.z=LinearInterpolate(ObjectList[x].EmitterData->Particles[z].LastPos.z,ObjectList[x].EmitterData->Particles[z].Position.z,t);
							ObjectList[x].EmitterData->Particles[z].Rotation+=ObjectList[x].EmitterData->Rotspeed+ObjectList[x].EmitterData->Particles[z].RotChaos;
							if (ObjectList[x].EmitterData->Tail)
							{
								ObjectList[x].EmitterData->Particles[z].TailCnt=(ObjectList[x].EmitterData->Particles[z].TailCnt+1)%((ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2);
								ObjectList[x].EmitterData->Particles[z].Tail[ObjectList[x].EmitterData->Particles[z].TailCnt]=ObjectList[x].EmitterData->Particles[z].Position;
								ObjectList[x].EmitterData->Particles[z].TailCnt2=(ObjectList[x].EmitterData->Particles[z].TailCnt2+1)%ObjectList[x].EmitterData->TailLength2;
							}

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
	}/**/
}

float SetParticleColor(WORLDOBJECT *o, int Particle, int ActualAnim, PARTICLEVECTOR *k, CAMERA *c, float Distance)
{
	//aktualis particle szin lekerese elet, animacio es kamerapozicio (camfadehez) alapjan
	//nem kell piszkalni

	unsigned char Dest=255;
	if (o->EmitterData->CamFade!=0)
	Dest=(unsigned char)(min(1,Distance/o->EmitterData->CamFade)*255.0f);

	float a=1-o->EmitterData->Particles[Particle].Age/(float)o->EmitterData->Particles[Particle].StartAge;
	unsigned char col[4];
	if (!o->AnimNum || !o->Animations[ActualAnim].Red->KeyNum)
	col[0]=(unsigned char)LinearInterpolate(o->EmitterData->Particles[Particle].Color1[0],o->EmitterData->Particles[Particle].Color2[0],a);
	else
	col[0]=(unsigned char)min(255,max(0,o->Animations[ActualAnim].Red->GetInterpolatedValue(a)));

	if (!o->AnimNum || !o->Animations[ActualAnim].Green->KeyNum)
	col[1]=(unsigned char)LinearInterpolate(o->EmitterData->Particles[Particle].Color1[1],o->EmitterData->Particles[Particle].Color2[1],a);
	else
	col[1]=(unsigned char)min(255,max(0,o->Animations[ActualAnim].Green->GetInterpolatedValue(a)));
	
	if (!o->AnimNum || !o->Animations[ActualAnim].Blue->KeyNum)
	col[2]=(unsigned char)LinearInterpolate(o->EmitterData->Particles[Particle].Color1[2],o->EmitterData->Particles[Particle].Color2[2],a);
	else
	col[2]=(unsigned char)min(255,max(0,o->Animations[ActualAnim].Blue->GetInterpolatedValue(a)));
	
	if (!o->AnimNum || !o->Animations[ActualAnim].Alpha->KeyNum)
	col[3]=(unsigned char)LinearInterpolate(o->EmitterData->Particles[Particle].Color1[3],o->EmitterData->Particles[Particle].Color2[3],a);
	else
	col[3]=(unsigned char)min(255,max(0,o->Animations[ActualAnim].Alpha->GetInterpolatedValue(a)));

	float Size;
	if (!o->AnimNum || !o->Animations[ActualAnim].Size->KeyNum) Size=o->EmitterData->Particles[Particle].Size;
	else
	Size=o->Animations[ActualAnim].Size->GetInterpolatedValue(a);

	k->col.r=col[0]*Dest/255;
	k->col.g=col[1]*Dest/255;
	k->col.b=col[2]*Dest/255;
	k->col.a=col[3]*Dest/255;
	return Size;
}

void MultModelViewMatrix(MATRIX m, bool kill)
{
	float matrix[16];
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
		{
			matrix[x+y*4]=m[x][y];
		}
	glMultMatrixf(matrix);
}


void WORLD::RenderParticles(CAMERA *Cam, int ActualAnim, MATRIX *WorldData)
{
	//ez rendereli a particleket

	Cam->Normal=V3_Sub(Cam->Target,Cam->Eye);
	Cam->d=Cam->Eye.x*Cam->Normal.x+Cam->Eye.y*Cam->Normal.y+Cam->Eye.z*Cam->Normal.z;


	//pushattrib BADBADBAD
	glPushAttrib(GL_ALL_ATTRIB_BITS);

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

	//ez igazabol nem is kene ide
	glPointSize(5);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);


	//glDepthMask(false);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glColor4ub(255,255,255,255);

	for (int x=0; x<ObjectNum; x++)
	{
		if (ObjectList[x].Primitive==aDDict_CUBEEMITTER || 
			ObjectList[x].Primitive==aDDict_CYLINDEREMITTER || 
			ObjectList[x].Primitive==aDDict_SPHEREEMITTER)
		{
			//ha emittert talaltunk

			//beallitja az emitterhez tartozo zmaskot
			glDepthMask(ObjectList[x].ZMask);
			//es alpha epsilont
			glAlphaFunc(GL_GEQUAL,ObjectList[x].AEpsilon/255.0f);
			//es blendmodot
			glBlendFunc(ObjectList[x].DSTBlend,ObjectList[x].SRCBlend);

			//ha van farka a particleknek, akkor azt rendereli
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
							col[0]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[0],ObjectList[x].EmitterData->Particles[y].Color2[0],i)/255.0f;
							else
							col[0]=ObjectList[x].Animations[ActualAnim].Red->GetInterpolatedValue(i)/255.0f;

							if (!ObjectList[x].Animations[ActualAnim].Green->KeyNum)
							col[1]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[1],ObjectList[x].EmitterData->Particles[y].Color2[1],i)/255.0f;
							else
							col[1]=ObjectList[x].Animations[ActualAnim].Green->GetInterpolatedValue(i)/255.0f;

							if (!ObjectList[x].Animations[ActualAnim].Blue->KeyNum)
							col[2]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[2],ObjectList[x].EmitterData->Particles[y].Color2[2],i)/255.0f;
							else
							col[2]=ObjectList[x].Animations[ActualAnim].Blue->GetInterpolatedValue(i)/255.0f;

							if (!ObjectList[x].Animations[ActualAnim].Alpha->KeyNum)
							col[3]=LinearInterpolate(ObjectList[x].EmitterData->Particles[y].Color1[3],ObjectList[x].EmitterData->Particles[y].Color2[3],i)/255.0f;
							else
							col[3]=ObjectList[x].Animations[ActualAnim].Alpha->GetInterpolatedValue(i)/255.0f;

							//ez egy particle farka
							glBegin(GL_LINE_STRIP);
							for (int a=0;a<ObjectList[x].EmitterData->TailLength;a++)
							{
								float c=(1-a/(float)ObjectList[x].EmitterData->TailLength);
								glColor4f(c*col[0],c*col[1],c*col[2],c*col[3]);
								int k=(ObjectList[x].EmitterData->Particles[y].TailCnt-a*ObjectList[x].EmitterData->TailLength2);
								k=k%((ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2);
								if (k<0) k+=(ObjectList[x].EmitterData->TailLength)*ObjectList[x].EmitterData->TailLength2;

								VECTOR3 t=ObjectList[x].EmitterData->Particles[y].Tail[k];
								glVertex3f(t.x,t.y,t.z);
							}
							glEnd();
					}
					glPopMatrix();
			}

			//ha van a particleknek fejuk
			if (ObjectList[x].EmitterData->Head)
			{
				//ha van textura akkor nem pottyoket rajzolunk, a nem texturazott nincs supportolva itt
				//(mi a francnak ;)
				if (ObjectList[x].Textured)
				{

					glPushMatrix();
					MultModelViewMatrix(*WorldData,0);
					//megszamolja hany elo particle van
					int ActiveCount=0;
					for (int y=0; y<ObjectList[x].EmitterData->MaxParticles; y++)
						if (ObjectList[x].EmitterData->Particles[y].Active) ActiveCount++;

					VECTOR3 CamNormal=V3_Normalize(V3_Sub(Cam->Target,Cam->Eye));

					PARTICLESORTINFO *Sort;
					//ha van elo particle
					if (ActiveCount)
					{

						int Count=0;
						Sort=new PARTICLESORTINFO[ActiveCount];
						//ez minden particlere kiszamolja a kamerasikhoz viszonyitott tavolsagat
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

						//sort tavolsag szerint
						for (int z=0; z<ActiveCount; z++)
						{
							float GreatestValue=Sort[z].Distance;
							int GreatestID=z;
							
							for (int y=z; y<ActiveCount; y++)
								if (Sort[y].Distance>GreatestValue)
								{
									GreatestValue=Sort[y].Distance;
									GreatestID=y;
								}

							PARTICLESORTINFO Buffer=Sort[z];
							Sort[z]=Sort[GreatestID];
							Sort[GreatestID]=Buffer;

						}

						//kell textura
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D,ObjectList[x].Material->ImageData.TextureHandle);
						
						int ActiveParticles=0;
						//ez tolti fel a vertexbuffert, igazabol innen veheted majd te is az ertekeket
						for (y=0; y<ActiveCount; y++)
						{
							float Size=SetParticleColor(&ObjectList[x],Sort[y].ParticleID,ActualAnim,&ParticleVertices[ActiveParticles*4],Cam,Sort[y].Distance);
							for (int k=0; k<3; k++)
								ParticleVertices[ActiveParticles*4+k+1].col=ParticleVertices[ActiveParticles*4].col;

							MATRIX m;
							M_Rotate(Cam->Normal.x,Cam->Normal.y,Cam->Normal.z,ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].Rotation/180.0f*3.1415f,m);

							VECTOR3 i,j;
							i=V3_Mults(Cam->i,Size);
							if (!ObjectList[x].EmitterData->FixedUp)
							j=V3_Mults(Cam->j,Size);
							else j=V3_Make(0,Size,0);

							VECTOR3 i1,j1;
							M_Xform3(m,i,i1);
							M_Xform3(m,j,j1);

							ParticleVertices[ActiveParticles*4  ].Pos=V3_Add(V3_Add(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2,i1),j1);
							ParticleVertices[ActiveParticles*4+1].Pos=V3_Add(V3_Sub(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2,i1),j1);
							ParticleVertices[ActiveParticles*4+2].Pos=V3_Sub(V3_Sub(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2,i1),j1);
							ParticleVertices[ActiveParticles*4+3].Pos=V3_Sub(V3_Add(ObjectList[x].EmitterData->Particles[Sort[y].ParticleID].DisplayPos2,i1),j1);

							ActiveParticles++;
						}


						//render
						glInterleavedArrays(GL_T2F_C4UB_V3F,0,ParticleVertices);
						glDrawElements(GL_QUADS,ActiveParticles*4,GL_UNSIGNED_INT,ParticleIndexBuffer);
						glIsEnabled(GL_TEXTURE_2D);

						delete [] Sort;
					}

					glPopMatrix();

				}
			}
		}
	}
	glPopAttrib();
}

void WORLD::RenderParticleTree(CAMERA *Cam, int ActualAnim, MATRIX *WorldData)
{
	//ez kell a particle hierarchyhoz (fonix powa)
	RenderParticles(Cam,max(ActualAnim,0),WorldData);
	for (int x=0; x<ObjectNum; x++)
		if (ObjectList[x].Primitive==aDDict_SUBSCENE)
		{
			MATRIX Parent;
			M_Identity(Parent);
			int ParentID=ObjectList[x].ID;
			WORLDOBJECT *P;
			while (ParentID!=-1)
			{
				P=FindObjectByID(ParentID);
				M_Mult(P->ModelView,Parent,Parent);
				ParentID=P->ParentID;
			}
			
			MATRIX w;
			memcpy(&w,WorldData,sizeof(MATRIX));			

			M_Mult(Parent,w,w);
			((WORLD*)ObjectList[x].Data)->RenderParticleTree(Cam,max(ActualAnim,0),&w);
		}
}

void WORLD::CalculateParticleObjectMatrices()
{
	//particle object animaciohoz, a calculateobjectmatrices tesoja
	for (int x=0; x<ObjectNum; x++) 
		if (ObjectList[x].Primitive==aDDict_CUBEEMITTER ||
			ObjectList[x].Primitive==aDDict_SPHEREEMITTER ||
			ObjectList[x].Primitive==aDDict_CYLINDEREMITTER ||
			ObjectList[x].Primitive==aDDict_GRAVITY ||
			ObjectList[x].Primitive==aDDict_SPHEREDEFLECTOR ||
			ObjectList[x].Primitive==aDDict_PLANEDEFLECTOR)
			RST2Matrix(&ObjectList[x].PosData,ObjectList[x].ModelView);
}
#endif
