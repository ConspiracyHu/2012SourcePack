#include "ObjectGenerators.h"
#include "aDDictMath.h"
#include "GuiInterface.h"

//#ifndef Public_Release
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

   return (float)pow((float)(t1+t2),(float)(-1.0f/n1));
}
//#endif


void GeneratePrimitive(OBJECT *Object,int PrimitiveID,int Param1, int Param2, int Param3, int Param4, int Param5)
{
	int x,y;
	int x1,x2,y1,y2;
	VECTOR3 v;
	if (PrimitiveID!=aDDict_STORED)
	{
		Object->VertexNum=0;
		Object->PolygonNum=0;
	}
	Object->Param1=Param1;
	Object->Param2=Param2;
	Object->Param3=Param3;
	Object->Param4=Param4;
	Object->Param5=Param5;
	//M_Identity(Object->ModelView);
	//M_Identity(Object->TransformBuffer);
	Object->Primitive=PrimitiveID;
	switch (PrimitiveID)
	{
	case aDDict_BOX:
		Object->AddVertex(-0.5,-0.5,-0.5,  0,  0); //0
		Object->AddVertex( 0.5,-0.5,-0.5,  1,  0); //1
		Object->AddVertex( 0.5, 0.5,-0.5,  1,  1); //2
		Object->AddVertex(-0.5, 0.5,-0.5,  0,  1); //3
		Object->AddVertex( 0.0, 0.0,-0.5,0.5,0.5); //4 center

		Object->AddVertex( 0.5,-0.5, 0.5,  0,  0); //5
		Object->AddVertex( 0.5, 0.5, 0.5,  0,  1); //6
		Object->AddVertex( 0.5, 0.0, 0.0,0.5,0.5); //7 center

		Object->AddVertex(-0.5, 0.5, 0.5,  1,  0); //8
		Object->AddVertex(-0.5,-0.5, 0.5,  1,  1); //9
		Object->AddVertex( 0.0, 0.0, 0.5,0.5,0.5); //10 center

		Object->AddVertex(-0.5, 0.0, 0.0,0.5,0.5); //11 center
		Object->AddVertex( 0.0,-0.5, 0.0,0.5,0.5); //12 center
		Object->AddVertex( 0.0, 0.5, 0.0,0.5,0.5); //13 center

		Object->AddPolygon(0,1,4,0,0,1,0,0.5,0.5,1,0,0);
		Object->AddPolygon(1,2,4,1,0,1,1,0.5,0.5,1,0,0);
		Object->AddPolygon(2,3,4,1,1,0,1,0.5,0.5,1,0,0);
		Object->AddPolygon(3,0,4,0,1,0,0,0.5,0.5,1,0,0);

		Object->AddPolygon(1,5,7,0,0,1,0,0.5,0.5,1,0,0);
		Object->AddPolygon(5,6,7,1,0,1,1,0.5,0.5,1,0,0);
		Object->AddPolygon(6,2,7,1,1,0,1,0.5,0.5,1,0,0);
		Object->AddPolygon(2,1,7,0,1,0,0,0.5,0.5,1,0,0);

		Object->AddPolygon(5,9,10,0,0,1,0,0.5,0.5,1,0,0);
		Object->AddPolygon(9,8,10,1,0,1,1,0.5,0.5,1,0,0);
		Object->AddPolygon(8,6,10,1,1,0,1,0.5,0.5,1,0,0);
		Object->AddPolygon(6,5,10,0,1,0,0,0.5,0.5,1,0,0);

		Object->AddPolygon(9,0,11,0,0,1,0,0.5,0.5,1,0,0);
		Object->AddPolygon(0,3,11,1,0,1,1,0.5,0.5,1,0,0);
		Object->AddPolygon(3,8,11,1,1,0,1,0.5,0.5,1,0,0);
		Object->AddPolygon(8,9,11,0,1,0,0,0.5,0.5,1,0,0);

		Object->AddPolygon(9,5,12,0,0,1,0,0.5,0.5,1,0,0);
		Object->AddPolygon(5,1,12,1,0,1,1,0.5,0.5,1,0,0);
		Object->AddPolygon(1,0,12,1,1,0,1,0.5,0.5,1,0,0);
		Object->AddPolygon(0,9,12,0,1,0,0,0.5,0.5,1,0,0);

		Object->AddPolygon(3,2,13,0,0,1,0,0.5,0.5,1,0,0);
		Object->AddPolygon(2,6,13,1,0,1,1,0.5,0.5,1,0,0);
		Object->AddPolygon(6,8,13,1,1,0,1,0.5,0.5,1,0,0);
		Object->AddPolygon(8,3,13,0,1,0,0,0.5,0.5,1,0,0);
		break;

	case aDDict_SPHERE:

		Object->AddVertex(0,-0.5,0,0.5,0);
		Object->AddVertex(0,0.5,0,0.5,0);
		float theta,phi;
		for (x=1; x<Param1-1; x++)
		{
			theta=(float)((x/(float)(Param1-1)*180-90)*radtheta);
			for (int y=0; y<Param2; y++)
			{
				phi=y/(float)Param2*360.0f*(float)radtheta;
				v.x=cos(theta)*cos(phi)*0.5f;
				v.y=cos(theta)*sin(phi)*0.5f;
				v.z=sin(theta)*0.5f;
				Object->AddVertex(v.x,v.z,-v.y,x/(float)Param1,y/(float)Param2);
			}
		}

		for (x=0; x<Param1-1; x++)
		{
			x1=(x-1)*Param2;
			x2=x*Param2;
			for (int y=0; y<Param2; y++)
			{
				y1=y+2;
				y2=(y+1)%Param2+2;
				if (x>0 && x<Param1-2)
				{
					Object->AddPolygon(x1+y1,x2+y1,x2+y2,aDDict_GOURAUDSHADE,(x)/(float)(Param1),(y)/(float)Param2,(x+1)/(float)(Param1),y/(float)Param2,(x+1)/(float)(Param1),(y+1)/(float)Param2,1,0,1);
					Object->AddPolygon(x1+y1,x2+y2,x1+y2,aDDict_GOURAUDSHADE,(x)/(float)(Param1),(y)/(float)Param2,(x+1)/(float)(Param1),(y+1)/(float)Param2,x/(float)(Param1),(y+1)/(float)Param2,0,1,1);
				}
				else
				if (x==0) Object->AddPolygon(y2,0,y1,aDDict_GOURAUDSHADE,(1)/(float)(Param1),(y+1)/(float)Param2,0,(y+1)/(float)Param2,(1)/(float)(Param1),(y)/(float)Param2,1,1,1);
				else Object->AddPolygon(x1+y1,1,x1+y2,aDDict_GOURAUDSHADE,(x)/(float)(Param1),(y)/(float)Param2,1,(y+1)/(float)Param2,(x)/(float)(Param1),(y+1)/(float)Param2,1,1,1);
			}
		}

		break;

	case aDDict_CYLINDER:
		Object->AddVertex(0,-0.5,0,0.5,0.5);
		Object->AddVertex(0,0.5,0,0.5,0.5);
		for (y=0; y<Param2+1; y++)
		{
			for (x=0; x<Param1; x++)
			{
				theta=(float)((x/(float)Param1*360.0f)*radtheta);
				v.x=cos(theta)*0.5f;
				v.y=sin(theta)*0.5f;
				v.z=-0.5f+y/(float)Param2;
				Object->AddVertex(v.x,v.z,-v.y,x/(float)Param1,v.z+0.5f);
			}
		}

		for (x=0; x<Param1; x++)
		{
			x1=x;
			x2=(x+1)%Param1;
			if (Param3)
			{
				Object->AddPolygon(0,x1+2,x2+2,0.5f,0.5f,sin(x/(float)(Param1)*360*radtheta)*0.5f+0.5f,cos(x/(float)(Param1)*360*radtheta)*0.5f+0.5f,sin((x+1)/(float)(Param1)*360*radtheta)*0.5f+0.5f,cos((x+1)/(float)(Param1)*360*radtheta)*0.5f+0.5f,1,1,1);
				Object->AddPolygon(x1+Param1*Param2+2,1,x2+Param1*Param2+2,sin(x/(float)(Param1)*360*radtheta)*0.5f+0.5f,cos(x/(float)(Param1)*360*radtheta)*0.5f+0.5f,0.5f,0.5f,sin((x+1)/(float)(Param1)*360*radtheta)*0.5f+0.5f,cos((x+1)/(float)(Param1)*360*radtheta)*0.5f+0.5f,1,1,1);
			}
			for (int y=0; y<Param2; y++)
			{
				y1=y*Param1+2;
				y2=(y+1)*Param1+2;
				Object->AddPolygon(x2+y1,x1+y1,x2+y2,aDDict_GOURAUDSHADE,(x+1)/(float)(Param1),y/(float)Param2,x/(float)(Param1),y/(float)Param2,(x+1)/(float)(Param1),(y+1)/(float)Param2,1,1,0);
				Object->AddPolygon(x2+y2,x1+y1,x1+y2,aDDict_GOURAUDSHADE,(x+1)/(float)(Param1),(y+1)/(float)Param2,x/(float)(Param1),y/(float)Param2,x/(float)(Param1),(y+1)/(float)Param2,0,1,1);
			}
		}


		break;
	case aDDict_CONE:
		Object->AddVertex(0,0.5,0,0.5,0.5);
		Object->AddVertex(0,-0.5,0,0.5,0.5);
		/*for (x=0; x<Param1; x++)
		{
				theta=(float)((x/(float)Param1*360.0f)*radtheta);
				v.x=cos(theta)*0.5f;
				v.y=sin(theta)*0.5f;
				v.z=-0.5f;
				Object->AddVertex(v.x,v.z,-v.y,v.x+0.5f,v.y+0.5f);
		}
		for (x=0; x<Param1; x++)
		{
			Object->AddPolygon(0,(x+1)%Param1+2,x+2,aDDict_GOURAUDSHADE);
			if (Param2)
			Object->AddPolygon(1,x+2,(x+1)%Param1+2);
		}*/

		for (y=1; y<=Param3; y++)
		{
			for (x=0; x<Param1; x++)
			{
				theta=(float)((x/(float)Param1*360.0f)*radtheta);
				v.x=cos(theta)*0.5f*y/(float)Param3;
				v.y=sin(theta)*0.5f*y/(float)Param3;
				v.z=-(-0.5f+y/(float)Param3);
				Object->AddVertex(v.x,v.z,-v.y,v.x+0.5f,v.y+0.5f);
			}
		}

		for (x=0; x<Param1; x++)
		{
			x1=x;
			x2=(x+1)%Param1;

			Object->AddPolygon(x1+2,0,x2+2,aDDict_GOURAUDSHADE,1,1,1);
			
			if (Param2)
				Object->AddPolygon(1,x1+Param1*(Param3-1)+2,x2+Param1*(Param3-1)+2,1,1,1);

			for (int y=0; y<Param3-1; y++)
			{
				y1=y*Param1+2;
				y2=(y+1)*Param1+2;
				Object->AddPolygon(x1+y1,x2+y1,x2+y2,aDDict_GOURAUDSHADE,1,0,1);
				Object->AddPolygon(x1+y1,x2+y2,x1+y2,aDDict_GOURAUDSHADE,0,1,1);
			}
		}
		
		break;
	case aDDict_PLANE:
		for (x=0; x<=Param1; x++)
			for (y=0; y<=Param2; y++)
				Object->AddVertex(x/(float)Param1-0.5f,0,y/(float)Param2-0.5f,x/(float)Param1,y/(float)Param2);
		for (x=0; x<Param1; x++)
		{
			x1=x*(Param2+1);
			x2=(x+1)*(Param2+1);
			for (int y=0; y<Param2; y++)
			{
				y1=y;
				y2=(y+1)%(Param2+1);
				Object->AddPolygon(x1+y1,x2+y1,x2+y2,1,0,1);
				Object->AddPolygon(x1+y1,x2+y2,x1+y2,0,1,1);
			}
		}
		break;
	case aDDict_CIRCLE:
		for (x=0; x<Param1; x++)
		{
				theta=(float)((x/(float)Param1*360.0f)*radtheta);
				v.x=cos(theta);
				v.y=sin(theta);
				v.z=0;
				float p=Param2/255.0f*0.5f;
				Object->AddVertex(v.x*0.5f,v.z,-v.y*0.5f,x/(float)Param1,0);
				Object->AddVertex(v.x*p,v.z,-v.y*p,x/(float)Param1,1);
		}
		for (x=0; x<Param1; x++)
		{
			x1=(x)%Param1*2;
			x2=(x+1)%Param1*2;
			y1=0;
			y2=1;
			Object->AddPolygon(x1+y1,x2+y1,x2+y2,aDDict_FLATSHADE,x/(float)Param1,0,(x+1)/(float)Param1,0,(x+1)/(float)Param1,1,1,0,1);
			Object->AddPolygon(x1+y1,x2+y2,x1+y2,aDDict_FLATSHADE,x/(float)Param1,0,(x+1)/(float)Param1,1,x/(float)Param1,1,0,1,1);
		}
		break;
	case aDDict_LINE:
		for (x=-1; x<=Param1; x++)
		{
			Object->AddVertex(0,0,-0.5f+x/(float)(Param1-1),0,0);
			if (x>0 && x<Param1) 
				Object->AddEdge(x,x+1,-1,1);
		}
		Object->Wireframe=true;
		break;
	case aDDict_ARC:
		for (x=-1; x<=Param1; x++)
		{
			float phase=x/(float)(Param1-1)*(float)Param2*radtheta;
			
			Object->AddVertex(0.5f*cos(phase),0,0.5f*sin(phase),0,0);
			if (x>0 && x<Param1) 
				Object->AddEdge(x,x+1,-1,1);
		}
		Object->Wireframe=true;
		break;

	case aDDict_LOFT: 
		if (Param1 && Param2)
		{
			VECTOR3 Up=V3_Make(0,1,0);

			Object->Backface=false;
			OBJECT *v1,*v2;
			v1=(OBJECT*)Param1;
			v2=(OBJECT*)Param2;

			Object->Param1=v1->ID;
			Object->Param2=v2->ID;

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
						VECTOR3 a2=V3_Normalize(V3_Sub(vc,vb));
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
//#ifndef Public_Release
	case aDDict_FUR:
		{
			srand(115);
			OBJECT *Host=(OBJECT*)Param5;
			for (int x=0; x<Host->PolygonNum; x++)
			{
				VECTOR3 Ha,Hb,Hc;
				Ha=Host->VertexList[Host->PolygonList[x].v1].Position;
				Hb=Host->VertexList[Host->PolygonList[x].v2].Position;
				Hc=Host->VertexList[Host->PolygonList[x].v3].Position;

				MATRIX m;
				memcpy(&m,Host->ModelView,sizeof(MATRIX));
				for (int a=0; a<4; a++) for (int b=0; b<4; b++) m[a][b]=KillFloat(m[a][b],2);

				M_Xformd(m,Ha,Ha);
				M_Xformd(m,Hb,Hb);
				M_Xformd(m,Hc,Hc);

				VECTOR3 Origin=V3_Mults(V3_Add(V3_Add(Ha,Hb),Hc),1/3.0f);
				VECTOR3 i=V3_Normalize(V3_Cross(V3_Sub(Hc,Ha),V3_Sub(Hb,Ha)));

				for (int y=0; y<Param2; y++)
				{
					VECTOR3 Random=V3_Normalize(V3_Make((float)rand(),(float)rand(),(float)rand()));
					VECTOR3 j=V3_Cross(i,Random);
					VECTOR3 k=V3_Mults(V3_Cross(i,j),0.5f);

					VECTOR3 a,b,c,d;
					float r=((rand()/(float)RAND_MAX)-0.5f)*2.0f;
					i=V3_Mults(i,Param3/255.0f);
					k=V3_Mults(k,Param1/255.0f);
					
					a=V3_Add(Origin,k);
					b=V3_Add(V3_Add(Origin,k),i);
					c=V3_Add(V3_Sub(Origin,k),i);
					d=V3_Sub(Origin,k);

					Object->AddVertex(a.x,a.y,a.z,0,0);
					Object->AddVertex(b.x,b.y,b.z,0,1);
					Object->AddVertex(c.x,c.y,c.z,1,1);
					Object->AddVertex(d.x,d.y,d.z,1,0);

					Object->AddPolygon((x*Param2+y)*4,(x*Param2+y)*4+1,(x*Param2+y)*4+2,aDDict_GOURAUDSHADE,1,0,1);
					Object->AddPolygon((x*Param2+y)*4,(x*Param2+y)*4+2,(x*Param2+y)*4+3,aDDict_GOURAUDSHADE,0,1,1);
				}
			}
		}
		break;
	case aDDict_SUPERSHAPE:
		{
			SUPERSHAPE *s=(SUPERSHAPE*)Param1;

			int x;
			for (x=0; x<=s->Xres; x++)
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
					if (s->Sphere) pos=V3_Make(r1a*cos(phi )*r2a*cos(ro ),r1a*sin(phi )*r2a*cos(ro ),r2a*sin(ro ));
					else pos=V3_Make(cos(phi )*(r1a+r2a*cos(ro )),sin(phi )*(r1a+r2a*cos(ro )),r2a*sin(ro ));

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
//#endif
	}
		
		
	//Object->Xtile=Object->Ytile=1;
	//Object->Color.r=Object->Color.g=Object->Color.b=Object->Color.a=255;
	if (PrimitiveID!=aDDict_STORED)
	{
		Object->CalculateNormals();
		Object->CalculateTextureCoordinates();
	}
	Object->CreateDisplayList();
}
