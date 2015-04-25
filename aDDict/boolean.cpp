/*#define CROSS(dest,v1,v2)                      \
              dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
              dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
              dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; dest[1]=v1[1]-v2[1]; dest[2]=v1[2]-v2[2]; 

#define SET(dest,src) dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; 

#define ADD(dest,v1,v2) dest[0]=v1[0]+v2[0]; dest[1]=v1[1]+v2[1]; dest[2]=v1[2]+v2[2]; 

#define MULT(dest,v,factor) dest[0]=factor*v[0]; dest[1]=factor*v[1]; dest[2]=factor*v[2];

void isect2(float VTX0[3],float VTX1[3],float VTX2[3],float VV0,float VV1,float VV2,
	    float D0,float D1,float D2,float *isect0,float *isect1,float isectpoint0[3],float isectpoint1[3]) 
{
  float tmp=D0/(D0-D1);          
  float diff[3];
  *isect0=VV0+(VV1-VV0)*tmp;         
  SUB(diff,VTX1,VTX0);              
  MULT(diff,diff,tmp);               
  ADD(isectpoint0,diff,VTX0);        
  tmp=D0/(D0-D2);                    
  *isect1=VV0+(VV2-VV0)*tmp;          
  SUB(diff,VTX2,VTX0);                   
  MULT(diff,diff,tmp);                 
  ADD(isectpoint1,VTX0,diff);          
}

int compute_intervals_isectline(float VERT0[3],float VERT1[3],float VERT2[3],
				       float VV0,float VV1,float VV2,float D0,float D1,float D2,
				       float D0D1,float D0D2,float *isect0,float *isect1,
				       float isectpoint0[3],float isectpoint1[3])
{
  if(D0D1>0.0f)                                        
  {                                                    
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);
  } 
  else if(D0D2>0.0f)                                   
    {                                                   
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1);
  }                                                  
  else if(D1*D2>0.0f || D0!=0.0f)   
  {                                   
    isect2(VERT0,VERT1,VERT2,VV0,VV1,VV2,D0,D1,D2,isect0,isect1,isectpoint0,isectpoint1);   
  }                                                  
  else if(D1!=0.0f)                                  
  {                                               
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1); 
  }                                         
  else if(D2!=0.0f)                                  
  {                                                   
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);     
  }                                                 
  else                                               
  {                                                   
    return 1;
  }
  return 0;
}

#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return 1;                      \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return 1;                      \
    }                                                 \
  }                                

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}

#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  float a,b,c,d0,d1,d2;                     \
  a=U1[i1]-U0[i1];                          \
  b=-(U1[i0]-U0[i0]);                       \
  c=-a*U0[i0]-b*U0[i1];                     \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b=-(U2[i0]-U1[i0]);                       \
  c=-a*U1[i0]-b*U1[i1];                     \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b=-(U0[i0]-U2[i0]);                       \
  c=-a*U2[i0]-b*U2[i1];                     \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return 1;                 \
  }                                         \
}

int coplanar_tri_tri(float N[3],float V0[3],float V1[3],float V2[3],
                     float U0[3],float U1[3],float U2[3])
{
   float A[3];
   short i0,i1;
   A[0]=(float)(fabs(N[0]));
   A[1]=(float)(fabs(N[1]));
   A[2]=(float)(fabs(N[2]));
   if(A[0]>A[1])
   {
      if(A[0]>A[2])  
      {
          i0=1;
          i1=2;
      }
      else
      {
          i0=0;
          i1=1;
      }
   }
   else
   {
      if(A[2]>A[1])
      {
          i0=0;
          i1=1;                                           
      }
      else
      {
          i0=0;
          i1=2;
      }
    }               
                
    EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2);
    EDGE_AGAINST_TRI_EDGES(V1,V2,U0,U1,U2);
    EDGE_AGAINST_TRI_EDGES(V2,V0,U0,U1,U2);
                
    POINT_IN_TRI(V0,U0,U1,U2);
    POINT_IN_TRI(U0,V0,V1,V2);

    return 0;
}

#define SORT2(a,b,smallest)       \
             if(a>b)       \
             {             \
               float c;    \
               c=a;        \
               a=b;        \
               b=c;        \
               smallest=1; \
             }             \
             else smallest=0;

int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3],
				                     float U0[3],float U1[3],float U2[3],int *coplanar,
				                     float isectpt1[3],float isectpt2[3])
{
  float E1[3],E2[3];
  float N1[3],N2[3],d1,d2;
  float du0,du1,du2,dv0,dv1,dv2;
  float D[3];
  float isect1[2], isect2[2];
  float isectpointA1[3],isectpointA2[3];
  float isectpointB1[3],isectpointB2[3];
  float du0du1,du0du2,dv0dv1,dv0dv2;
  short index;
  float vp0,vp1,vp2;
  float up0,up1,up2;
  float b,c,max;
  int smallest1,smallest2;
  
  SUB(E1,V1,V0);
  SUB(E2,V2,V0);
  CROSS(N1,E1,E2);
  d1=-DOT(N1,V0);

  du0=DOT(N1,U0)+d1;
  du1=DOT(N1,U1)+d1;
  du2=DOT(N1,U2)+d1;

#if USE_EPSILON_TEST==TRUE
  if(fabs(du0)<EPSILON) du0=0.0;
  if(fabs(du1)<EPSILON) du1=0.0;
  if(fabs(du2)<EPSILON) du2=0.0;
#endif
  du0du1=du0*du1;
  du0du2=du0*du2;

    return 0;

  SUB(E1,U1,U0);
  SUB(E2,U2,U0);
  CROSS(N2,E1,E2);
  d2=-DOT(N2,U0);

  dv0=DOT(N2,V0)+d2;
  dv1=DOT(N2,V1)+d2;
  dv2=DOT(N2,V2)+d2;

#if USE_EPSILON_TEST==TRUE
  if(fabs(dv0)<EPSILON) dv0=0.0;
  if(fabs(dv1)<EPSILON) dv1=0.0;
  if(fabs(dv2)<EPSILON) dv2=0.0;
#endif

  dv0dv1=dv0*dv1;
  dv0dv2=dv0*dv2;
        
  if(dv0dv1>0.0f && dv0dv2>0.0f)
    return 0;

  CROSS(D,N1,N2);

  max=(float)(fabs(D[0]));
  index=0;
  b=(float)(fabs(D[1]));
  c=(float)(fabs(D[2]));
  if(b>max) max=b,index=1;
  if(c>max) max=c,index=2;

  vp0=V0[index];
  vp1=V1[index];
  vp2=V2[index];
  
  up0=U0[index];
  up1=U1[index];
  up2=U2[index];

  *coplanar=compute_intervals_isectline(V0,V1,V2,vp0,vp1,vp2,dv0,dv1,dv2,
				       dv0dv1,dv0dv2,&isect1[0],&isect1[1],isectpointA1,isectpointA2);
  if(*coplanar) return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);     


  compute_intervals_isectline(U0,U1,U2,up0,up1,up2,du0,du1,du2,
			      du0du1,du0du2,&isect2[0],&isect2[1],isectpointB1,isectpointB2);

  SORT2(isect1[0],isect1[1],smallest1);
  SORT2(isect2[0],isect2[1],smallest2);

  if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;


  if(isect2[0]<isect1[0])
  {
    if(smallest1==0) { SET(isectpt1,isectpointA1); }
    else { SET(isectpt1,isectpointA2); }

    if(isect2[1]<isect1[1])
    {
      if(smallest2==0) { SET(isectpt2,isectpointB2); }
      else { SET(isectpt2,isectpointB1); }
    }
    else
    {
      if(smallest1==0) { SET(isectpt2,isectpointA2); }
      else { SET(isectpt2,isectpointA1); }
    }
  }
  else
  {
    if(smallest2==0) { SET(isectpt1,isectpointB1); }
    else { SET(isectpt1,isectpointB2); }

    if(isect2[1]>isect1[1])
    {
      if(smallest1==0) { SET(isectpt2,isectpointA2); }
      else { SET(isectpt2,isectpointA1); }      
    }
    else
    {
      if(smallest2==0) { SET(isectpt2,isectpointB2); }
      else { SET(isectpt2,isectpointB1); } 
    }
  }
  return 1;
}


struct pathedge{
	vector3 a,b;
	bool inpath;
	bool circleend;
	bool circlestart;
	bool pathend;
	bool pathstart;
	int sidepathentry;
	int sidepathentryend;
	bool beenhere;
	float mu;
};

struct pathedgelist{ 
	int pathedgeroom;
	int pathedgenum;
	int pathnum;
	pathedge *list;
};

void addpathedge(pathedgelist &list, pathedge elem)
{
	if (list.pathedgeroom==list.pathedgenum)
	{
		pathedge *buffer = new pathedge[(list.pathedgeroom+10)*2];
		memset(buffer,0,sizeof(pathedge)*(list.pathedgeroom+10)*2);
		memcpy(buffer,list.list,sizeof(pathedge)*list.pathedgeroom);
		pathedge *b=list.list;
		list.list=buffer;
		delete[] b;
		list.pathedgeroom=(list.pathedgeroom+10)*2;
	}
	list.list[list.pathedgenum]=elem;
	list.pathedgenum++;	
}

void swapedges(pathedge *list, int a, int b)
{
	pathedge buf;
	buf=list[a];
	list[a]=list[b];
	list[b]=buf;
}

int searchedgestartvertex(pathedgelist list, vector3 a, int match)
{
	int result=-1;
	float eps=0.00001f;

	for (int x=0; x<list.pathedgenum; x++)
	if (x!=match && fabs(list.list[x].a.b.x-a.b.x)<=eps && 
		              fabs(list.list[x].a.b.y-a.b.y)<=eps && 
									fabs(list.list[x].a.b.z-a.b.z)<=eps) return x;
	return result;
}

int searchedgeendvertex(pathedgelist list, vector3 a, int match)
{
	int result=-1;
	float eps=0.00001f;

	for (int x=0; x<list.pathedgenum; x++)
	if (x!=match && fabs(list.list[x].b.b.x-a.b.x)<=eps && 
									fabs(list.list[x].b.b.y-a.b.y)<=eps && 
									fabs(list.list[x].b.b.z-a.b.z)<=eps) return x;
	return result;
}

int searchnext(pathedgelist list, int curr)
{
	int result=-1;
	int nexta=searchedgestartvertex(list,list.list[curr].b,curr);
	int nextb=searchedgeendvertex(list,list.list[curr].b,curr);
	if (nexta!=-1 && nextb==-1) result=nexta;
	if (nexta==-1 && nextb!=-1) 
	{
		vector3 b=list.list[nextb].a;
		list.list[nextb].a=list.list[nextb].b;
		list.list[nextb].b=b;
		result=nextb;
	}
	return result;
}

int searchprev(pathedgelist list, int curr)
{
	int result=-1;
	int nexta=searchedgestartvertex(list,list.list[curr].a,curr);
	int nextb=searchedgeendvertex(list,list.list[curr].a,curr);
	if (nexta!=-1 && nextb==-1)
	{
		vector3 b=list.list[nexta].a;
		list.list[nexta].a=list.list[nexta].b;
		list.list[nexta].b=b;
		result=nexta;
	}
	if (nexta==-1 && nextb!=-1) result=nextb;
	return result;
}


void arrangepaths(pathedgelist &list)
{
	char *ss = new char[256];

	for (int x=0; x<list.pathedgenum; x++)
		if (!list.list[x].inpath)
		{
			int prevnext=x;
			int prev=searchprev(list,x);
			while (!(prev==x || prev==-1)) { prevnext=prev; prev=searchprev(list,prev);}
			if (prev==-1)
			{//nem kort talaltunk
				swapedges(list.list,x,prevnext); //megkerestuk a szakasz elso elemet, es az aktualissal lecsereltuk
				list.list[x].inpath=true;
				list.list[x].pathstart=true;
				int next=searchnext(list,x);
				int y=x+1;
				while (next!=-1)
				{
					swapedges(list.list,y,next);
					list.list[y].inpath=true;
					next=searchnext(list,y++);
				}
				list.list[y-1].pathend=true;
			}
			else
			{//kort talaltunk
				list.list[x].circlestart=true;
				list.list[x].inpath=true;
				int next=searchnext(list,x);
				int y=x+1;
				while (next!=x)
				{
					swapedges(list.list,y,next);
					list.list[y].inpath=true;
					next=searchnext(list,y++);
				}
				list.list[y-1].circleend=true;
			}
		}
}

typedef void (__stdcall *GluTessCallbackType)();
void __stdcall EdgeFlagDataCB(GLboolean flag,void* lpContext){}

int polyindexcntr=0;
int polyindices[3];
object *objtoadd=NULL;
int polyshade=dd3d_flatshade;
int basepoly=0;
polygon poly;

int searchpoly(object *o1, int a, int b, int c)
{
	int v1,v2,v3;

	for (int x=0; x<o1->polynum; x++)
	{
		v1=o1->polygonlist[x].v1;
		v2=o1->polygonlist[x].v1;
		v3=o1->polygonlist[x].v1;
		if ((a==v1 && b==v2 && c==v3) || 
		    (a==v1 && b==v3 && c==v2) || 
			(a==v2 && b==v1 && c==v3) || 
			(a==v2 && b==v3 && c==v1) || 
			(a==v3 && b==v1 && c==v2) || 
			(a==v3 && b==v2 && c==v1)) return x;
	}

	return -1;
}

void __stdcall VertexCB(int vertex_data, object * polydata)
{
	polyindices[polyindexcntr++]=vertex_data;

	if (polyindexcntr==3)
	{
		polyindexcntr=0;
		if (searchpoly(objtoadd,polyindices[0],polyindices[1],polyindices[2])==-1) addpolygon(objtoadd,polyindices[0],polyindices[1],polyindices[2],polyshade);
		objtoadd->polygonlist[objtoadd->polynum-1]=poly;
		objtoadd->polygonlist[objtoadd->polynum-1].v1=polyindices[0];
		objtoadd->polygonlist[objtoadd->polynum-1].v2=polyindices[1];
		objtoadd->polygonlist[objtoadd->polynum-1].v3=polyindices[2];
	}
}

void __stdcall BeginCB(GLenum type)
{
	polyindexcntr=0;
}

void __stdcall EndCB(GLenum type)
{
	objtoadd->polygonlist[basepoly]=objtoadd->polygonlist[objtoadd->polynum-1];
	objtoadd->polynum--;
}


int searchvertexindex(object *o1, vector3 v)
{
	int i=-1;
	float eps=0.0001f;
	for (int x=0; x<o1->vertexnum; x++)
	{
		if (fabs(o1->vertexlist[x].d.b.x-v.b.x)<eps &&
			fabs(o1->vertexlist[x].d.b.y-v.b.y)<eps &&
			fabs(o1->vertexlist[x].d.b.z-v.b.z)<eps) return x;
	}
	return i;
}

bool online(vector3 a, vector3 b, vector3 p, float &mu)
{
	mu=-1;
	float eps=0.000001f;
	bool result=false;
	vector3 ab=egyseg(vect_kivonas(b,a));
	vector3 ap=egyseg(vect_kivonas(p,a));
	float dp=(float)(fabs(dotproduct(ab,ap)));
	if (dp>=1.0f-eps) result=true;
	if (!result) return false;

	float mx=0,my=0,mz=0;
	vector3 n=vect_kivonas(b,a);
	vector3 pma=vect_kivonas(p,a);

	if (fabs(n.b.x)>=eps) mx=pma.b.x/n.b.x;
	if (fabs(n.b.y)>=eps) my=pma.b.y/n.b.y;
	if (fabs(n.b.z)>=eps) mz=pma.b.z/n.b.z;

	     if (fabs(n.b.x)>=eps) mu=mx;
	else if (fabs(n.b.y)>=eps) mu=my;
	else if (fabs(n.b.z)>=eps) mu=mz;

	return result;
}

void cutpolysides(pathedgelist &polypaths, pathedgelist &polyedgelist, vector3 a, vector3 b, vector3 c)
{ 
	float mu;
	pathedge elem;
	memset(&elem,0,sizeof(pathedge));
	elem.a=a; elem.sidepathentry=-1;
	addpathedge(polyedgelist,elem);
	elem.a=b; elem.mu=1;
	addpathedge(polyedgelist,elem);
	elem.a=c; elem.mu=2;
	addpathedge(polyedgelist,elem);
	for (int x=0; x<polypaths.pathedgenum; x++)
	{
		vector3 p;
		if (polypaths.list[x].pathstart)
		{
			p=polypaths.list[x].a;
			elem.a=p;
			
			     if (online(a,b,p,mu)) elem.mu=mu;
			else if (online(b,c,p,mu)) elem.mu=mu+1.0f;
			else if (online(c,a,p,mu)) elem.mu=mu+2.0f;

			elem.sidepathentry=x;
			addpathedge(polyedgelist,elem);
		}
		if (polypaths.list[x].pathend)
		{
			p=polypaths.list[x].b;
			elem.a=p;
			
			     if (online(a,b,p,mu)) elem.mu=mu;
			else if (online(b,c,p,mu)) elem.mu=mu+1.0f;
			else if (online(c,a,p,mu)) elem.mu=mu+2.0f;

			elem.sidepathentry=x;
			addpathedge(polyedgelist,elem);
		}
	}
	float smallest;
	int smallestindex;
	for (x=0; x<polyedgelist.pathedgenum; x++)
	{
		smallest=10;
		smallestindex=-1;
		for (int y=x; y<polyedgelist.pathedgenum; y++)
			if (polyedgelist.list[y].mu<smallest)
			{
				smallest=polyedgelist.list[y].mu;
				smallestindex=y;
			}
		swapedges(polyedgelist.list,x,smallestindex);	
		if (polyedgelist.list[x].sidepathentry!=-1)
		{
			if (polypaths.list[polyedgelist.list[x].sidepathentry].pathstart)
			polypaths.list[polyedgelist.list[x].sidepathentry].sidepathentry=x;
			if (polypaths.list[polyedgelist.list[x].sidepathentry].pathend)
			polypaths.list[polyedgelist.list[x].sidepathentry].sidepathentryend=x;
		}
	}
}

void addtessvertex(object *o, GLUtesselator *tess, vector3 vert)
{
	double vx[3];
	vx[0]=vert.b.x;
	vx[1]=vert.b.y;
	vx[2]=vert.b.z;
	int v=searchvertexindex(objtoadd,vert);
	if (v==-1) {addvertex(objtoadd,vert.b.x,vert.b.y,vert.b.z); v=objtoadd->vertexnum-1;}
	gluTessVertex(tess,reinterpret_cast<double*>(vx),reinterpret_cast<void*>(v));
}

int gothrupath(object *o, GLUtesselator *tess,pathedgelist &polypaths,pathedgelist &polyedgelist, int bee)
{
	int result=-1;
	int start=polyedgelist.list[bee].sidepathentry;
	
	if (polypaths.list[start].pathstart && !polypaths.list[start].pathend)
	{
		int z=start;
		while (!polypaths.list[z].pathend) addtessvertex(o,tess,polypaths.list[z++].b);
		result=polypaths.list[z].sidepathentryend;
	}
	if (polypaths.list[start].pathend && !polypaths.list[start].pathstart)
	{
		int z=start;
		while (!polypaths.list[z].pathstart) addtessvertex(o,tess,polypaths.list[z--].a);
		result=polypaths.list[z].sidepathentry;
	}

	if (polypaths.list[start].pathend && polypaths.list[start].pathstart)
	{
		int a=searchedgestartvertex(polyedgelist, polypaths.list[start].a,-1);
		int b=searchedgestartvertex(polyedgelist, polypaths.list[start].b,-1);

		if (bee==a) result=b;
		if (bee==b) result=a;
	}

	return result;
}

void goaround(object *o,GLUtesselator *tess,pathedgelist &polyedgelist,pathedgelist &polypaths,int start)
{
	
	addtessvertex(o,tess,polyedgelist.list[start].a);
	polyedgelist.list[start].beenhere=true;

	int z=start+1;
	while (z!=start)
	{		
		polyedgelist.list[z].beenhere=true;
		addtessvertex(o,tess,polyedgelist.list[z].a);

		if (polyedgelist.list[z].sidepathentry!=-1) //ha nem sarkon vagyunk eppen
		{ 
			z=gothrupath(o,tess,polypaths,polyedgelist,z);
			polyedgelist.list[z].beenhere=true;
			addtessvertex(o,tess,polyedgelist.list[z].a);
		}

		if (z!=start) z=(z+1)%polyedgelist.pathedgenum; //ha sarkon vagyunk akkor siman tovabbmegyunk
	}
	
}

void cutobject(object *o1,object *o1add,pathedgelist *obj1paths,pathedgelist *obj1abc, int o1pnum, int strt)
{
	objtoadd=o1add;
	for (int x=0; x<o1pnum;x++)
	{
		GLUtesselator* tess = gluNewTess();
		gluTessCallback(tess,GLU_TESS_BEGIN,reinterpret_cast<GluTessCallbackType>(BeginCB));
		//gluTessCallback(tess,GLU_TESS_END,reinterpret_cast<GluTessCallbackType>(EndCB));
		gluTessCallback(tess,GLU_TESS_EDGE_FLAG_DATA,reinterpret_cast<GluTessCallbackType>(EdgeFlagDataCB));
		gluTessCallback(tess,GLU_TESS_VERTEX_DATA,reinterpret_cast<GluTessCallbackType>(VertexCB));

		basepoly=x;
		poly=o1->polygonlist[x];

		gluTessBeginPolygon(tess,NULL);

		int start=strt;
		if (strt==1)
		{
			if (obj1abc[x].list[start].sidepathentry==-1) start=2;
			if (obj1abc[x].list[start].sidepathentry==-1) start=3;
		}

		//a kulso elek a metszo utakkal
		for (int y=start; y<obj1abc[x].pathedgenum; y+=2)
		{
			if (!obj1abc[x].list[y].beenhere && (obj1abc[x].list[y].sidepathentry!=-1 || y==start))
			{
				gluTessBeginContour(tess);
				goaround(o1,tess,obj1abc[x],obj1paths[x],y);
				gluTessEndContour(tess);
			}
		}

		//most jonnek a korok
		for (y=0; y<obj1paths[x].pathedgenum; y++)
		{
			if (obj1paths[x].list[y].circlestart)
			{
				gluTessBeginContour(tess);
				while (!obj1paths[x].list[y].circleend)
				{
					addtessvertex(o1,tess,obj1paths[x].list[y].a);
					y++;
				}
				addtessvertex(o1,tess,obj1paths[x].list[y].a);
				gluTessEndContour(tess);
			}
		}

		gluTessEndPolygon(tess);

		gluDeleteTess(tess);
	}

}

int rayIntersectsTriangle(vector3 p, vector3 d, 
			vector3 v0, vector3 v1, vector3 v2, float &t) 
{

	vector3 e1,e2,h,s,q;
	float a,f,u,v;
	
	e1=vect_kivonas(v1,v0);
	e2=vect_kivonas(v2,v0);
	h=cross(d,e2);
	a = dotproduct(e1,h);
	
	if (a > -0.00001 && a < 0.00001) return(false);
	
	f = 1/a;
	s=vect_kivonas(p,v0);
	u = f * (dotproduct(s,h));
	
	if (u < 0.0 || u > 1.0)	return(false);
	
	q=cross(s,e1);
	v = f * dotproduct(d,q);
	if (v < 0.0 || u + v > 1.0)	return(false);

    t = dotproduct(e2, q) * f;

	return(true);
}

void checkpolys(object *dummy, object *o)
{
	generatenormals(dummy);
	float t;

	for (int x=0; x<dummy->polynum; x++)
	{
		vector3 polycenter,raydir;
		polycenter= vect_osszeadas(
					vect_osszeadas(dummy->vertexlist[dummy->polygonlist[x].v1].d,
					               dummy->vertexlist[dummy->polygonlist[x].v2].d),
								   dummy->vertexlist[dummy->polygonlist[x].v3].d);
		polycenter.b.x/=3;
		polycenter.b.y/=3;
		polycenter.b.z/=3;
		raydir=egyseg(dummy->polygonlist[x].defaultnormal);
		int isectcounter=0;
	
		for (int y=0; y<o->polynum; y++)
		{
			if (rayIntersectsTriangle(polycenter,raydir,
				o->vertexlist[o->polygonlist[y].v1].d,
				o->vertexlist[o->polygonlist[y].v2].d,
				o->vertexlist[o->polygonlist[y].v3].d,t))
			{
				//vector3 ap=vect_kivonas(o->vertexlist[o->polygonlist[y].v3].d,polycenter);
				if (t>=0) isectcounter++;
			}
		}

		dummy->polygonlist[x].insideotherobject=isectcounter % 2;
	}

}

void movetooutput(object *output, object *in, int shared, int invertnormals)
{
	for (int x=0; x<in->polynum; x++)
	{
		if (( shared &&  in->polygonlist[x].insideotherobject) ||
		    (!shared && !in->polygonlist[x].insideotherobject))
		{
			int a,b,c;
			vector3 av=in->vertexlist[in->polygonlist[x].v1].d,
				    bv=in->vertexlist[in->polygonlist[x].v2].d,
					cv=in->vertexlist[in->polygonlist[x].v3].d;
			a=searchvertexindex(output,av);
			b=searchvertexindex(output,bv);
			c=searchvertexindex(output,cv);
			if (a==-1) {addvertex(output,av.b.x,av.b.y,av.b.z); a=output->vertexnum-1;}
			if (b==-1) {addvertex(output,bv.b.x,bv.b.y,bv.b.z); b=output->vertexnum-1;}
			if (c==-1) {addvertex(output,cv.b.x,cv.b.y,cv.b.z); c=output->vertexnum-1;}
			addpolygon(output,a,b,c,in->polygonlist[x].shading);
			output->polygonlist[output->polynum-1]=in->polygonlist[x];
			output->polygonlist[output->polynum-1].v1=a;
			output->polygonlist[output->polynum-1].v2=b;
			output->polygonlist[output->polynum-1].v3=c;
			if (invertnormals) output->polygonlist[output->polynum-1].normalinverted=!output->polygonlist[output->polynum-1].normalinverted;
		}
	}
}


void obj_boolean2(object *o1, object *o2, int function)
{
	object *dummy1=newobject(),*dummy2=newobject();
	object *output=newobject();

	matrix m;
	memcpy(m,o1->xformmatrix,sizeof(matrix));
	m_invert(m);

	for (int z=0; z<o2->vertexnum; z++)
	{
		o2->vertexlist[z].base=o2->vertexlist[z].d;
		m_xformd(m,o2->vertexlist[z].generated,o2->vertexlist[z].d);
	}

	int o1pnum=o1->polynum, 
		o2pnum=o2->polynum;
	pathedgelist *obj1paths = new pathedgelist[o1->polynum],
		         *obj2paths = new pathedgelist[o2->polynum];
	memset(obj1paths,0,sizeof(pathedgelist)*o1->polynum);
	memset(obj2paths,0,sizeof(pathedgelist)*o2->polynum);
	
	pathedgelist *obj1abc = new pathedgelist[o1->polynum],
		         *obj2abc = new pathedgelist[o2->polynum];
	memset(obj1abc,0,sizeof(pathedgelist)*o1->polynum);
	memset(obj2abc,0,sizeof(pathedgelist)*o2->polynum);
	//ide mennek majd a polykhoz tartozo vagasgorbek meg a polygonok szelein levo szakaszok
	
	int coplanar=1;
	for (int x=0; x<o1pnum;x++)
		for (int y=0; y<o2pnum;y++)
		{
			coplanar=1;
			pathedge section;
			memset(&section,0,sizeof(pathedge));
			if (tri_tri_intersect_with_isectline(o1->vertexlist[o1->polygonlist[x].v1].d.a,o1->vertexlist[o1->polygonlist[x].v2].d.a,o1->vertexlist[o1->polygonlist[x].v3].d.a,
				                                 o2->vertexlist[o2->polygonlist[y].v1].d.a,o2->vertexlist[o2->polygonlist[y].v2].d.a,o2->vertexlist[o2->polygonlist[y].v3].d.a,
				&coplanar,
				section.a.a,section.b.a))
			{
				if (!coplanar)
				{
					//section-ban o1 x.polyjanak es o2 y.polyjanak metszet szakasza,
					//a ket poly erintkezik
					float eps=0.00001f;
					if (!(fabs(section.a.b.x-section.b.b.x)<=eps &&
						  fabs(section.a.b.y-section.b.b.y)<=eps &&
						  fabs(section.a.b.z-section.b.b.z)<=eps))
					{
						addpathedge(obj1paths[x],section);
						addpathedge(obj2paths[y],section);
					}
				}
			}
		}

	obj_transform(o2,o2->xformmatrix);

	for (x=0; x<o1pnum;x++) arrangepaths(obj1paths[x]);
	for (x=0; x<o2pnum;x++) arrangepaths(obj2paths[x]);

	//polyoldalak vagasanak szamitasa
	for (x=0; x<o1pnum;x++)	cutpolysides(obj1paths[x],obj1abc[x],o1->vertexlist[o1->polygonlist[x].v1].d,o1->vertexlist[o1->polygonlist[x].v2].d,o1->vertexlist[o1->polygonlist[x].v3].d);
	for (x=0; x<o2pnum;x++)	cutpolysides(obj2paths[x],obj2abc[x],o2->vertexlist[o2->polygonlist[x].v1].d,o2->vertexlist[o2->polygonlist[x].v2].d,o2->vertexlist[o2->polygonlist[x].v3].d);

	//polyvagas kovetkezik GLU RULEZ, legalabb valamit ne en csinaljak ;-)
	cutobject(o1,dummy1,obj1paths,obj1abc,o1pnum,0);
	cutobject(o2,dummy2,obj2paths,obj2abc,o2pnum,0);
	for (x=0; x<o1pnum;x++)	for (int y=0; y<obj1abc[x].pathedgenum;y++)	obj1abc[x].list[y].beenhere=false;
	for (x=0; x<o2pnum;x++)	for (int y=0; y<obj2abc[x].pathedgenum;y++)	obj2abc[x].list[y].beenhere=false;
	cutobject(o1,dummy1,obj1paths,obj1abc,o1pnum,1);
	cutobject(o2,dummy2,obj2paths,obj2abc,o2pnum,1);

	//polyvagas vege, lecc boolean

	checkpolys(dummy1,o2);
	checkpolys(dummy2,o1);

	switch (function)
	{
	case 0:  //add
		{ 
			movetooutput(output,dummy1,false,false);
			movetooutput(output,dummy2,false,false);
			break;
		}
	case 1:  //subtract
		{ 
			movetooutput(output,dummy1,false,false);
			movetooutput(output,dummy2,true,true);
			break;
		}
	case 2:  //intersect
		{ 
			movetooutput(output,dummy1,true,false);
			movetooutput(output,dummy2,true,false);
			break;
		}
	}

	//eeeeeeeeees keeeeeeeeeeeeeeeeeeeesz :))))))))

	delete[] o1->vertexlist;
	delete[] o1->polygonlist;

	o1->vlistsize=output->vlistsize;
	o1->vertexnum=output->vertexnum;
	o1->plistsize=output->plistsize;
	o1->polynum=output->polynum;
	o1->vertexlist=output->vertexlist;
	o1->polygonlist=output->polygonlist;
	generatenormals(o1);
	//smoothgroups(o1);
	//generatenormals(o1);
	obj_transform(o1,o1->xformmatrix);

	for (z=0; z<o2->vertexnum; z++)	o2->vertexlist[z].d=o2->vertexlist[z].base;
	obj_transform(o2,o2->xformmatrix);
}
/***************************************************************************/

void plane_imp_triangle_int_add_3d ( float x1, float y1, float z1, 
  float x2, float y2, float z2, float dist1, float dist2, 
  int *num_int, float x[], float y[], float z[] )
{
  float alpha;

  if ( fabs(dist1) <= 0.00001 ) {
    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;
  }
  else if ( fabs(dist2) <= 0.00001 ) {
    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;
  }
  else if ( dist1 * dist2 < 0.0 ) {
    alpha = dist2 / ( dist2 - dist1 );
    x[*num_int] = alpha * x1 + ( 1.0f - alpha ) * x2;
    y[*num_int] = alpha * y1 + ( 1.0f - alpha ) * y2;
    z[*num_int] = alpha * z1 + ( 1.0f - alpha ) * z2;
    *num_int = *num_int + 1;
  }

  return;
}

void plane_imp_triangle_int_3d ( float x1, float y1, float z1, float x2, 
  float y2, float z2, float x3, float y3, float z3, float a, float b, 
  float c, float d, int *num_int, float x[], float y[], float z[], float sides[], bool &split )

{
  split=false;
  float dist1;
  float dist2;
  float dist3;

  *num_int = 0;
  dist1 = a * x1 + b * y1 + c * z1 + d;
  dist2 = a * x2 + b * y2 + c * z2 + d;
  dist3 = a * x3 + b * y3 + c * z3 + d;
  if ( fabs(dist1) <= 0.00001 ) {

    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;

  }

  if ( fabs(dist2) <= 0.00001 ) {

    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;

  }

  if ( fabs(dist3) <= 0.00001 ) {

    x[*num_int] = x3;
    y[*num_int] = y3;
    z[*num_int] = z3;
    *num_int = *num_int + 1;

  }
  if ( *num_int >= 2 ) {
    return;
  }
  if ( *num_int == 1 ) {

    if ( fabs(dist1) <= 0.00001 ) {

      plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
        dist2, dist3, num_int, x, y, z );

    }
    else if ( fabs(dist2) <= 0.00001 ) {

      plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
        dist1, dist3, num_int, x, y, z );

    }
    else if ( fabs(dist3) <= 0.00001 ) {

      plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
        dist1, dist2, num_int, x, y, z );

    }

    return;

  }
  split=true;
  if ( dist1 * dist2 < 0.0 && dist1 * dist3 < 0.0 ) {

    sides[0]=1; //ab
    plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
      dist1, dist2, num_int, x, y, z );

	sides[1]=3; //ac
    plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
      dist1, dist3, num_int, x, y, z );

  }
  else if ( dist2 * dist1 < 0.0 && dist2 * dist3 < 0.0 ) {

	sides[0]=1; //ab
    plane_imp_triangle_int_add_3d ( x2, y2, z2, x1, y1, z1, 
      dist2, dist1, num_int, x, y, z );

	sides[1]=2; //bc
    plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
      dist2, dist3, num_int, x, y, z );

  }
  else if ( dist3 * dist1 < 0.0 && dist3 * dist2 < 0.0 ) {

	sides[0]=3; //ac
    plane_imp_triangle_int_add_3d ( x3, y3, z3, x1, y1, z1, 
      dist3, dist1, num_int, x, y, z );

	sides[1]=2; //bc
    plane_imp_triangle_int_add_3d ( x3, y3, z3, x2, y2, z2, 
      dist3, dist2, num_int, x, y, z );

  }

  return;
}

int checkplanetri(vector3 normal, vector3 planevert, vector3 a, vector3 b, vector3 c)
{
	float planedist=-normal.b.x*planevert.b.x-normal.b.y*planevert.b.y-normal.b.z*planevert.b.z;
	float x[3],y[3],z[3],sides[3];
	int num_int=0;
	bool split;
	plane_imp_triangle_int_3d ( a.b.x, a.b.y, a.b.z, 
		                        b.b.x, b.b.y, b.b.z, 
								c.b.x, c.b.y, c.b.z,  
								normal.b.x, normal.b.y, normal.b.z, planedist,
								&num_int, x, y, z, sides, split);
	if (num_int==3) return tri_onplane;
	if (num_int==2 && split) return tri_split;
	float ta;
	vector3 s;
	s=vect_osszeadas(vect_osszeadas(a,b),c);
	s.b.x/=3.0f;
	s.b.y/=3.0f;
	s.b.z/=3.0f;
	ta=normal.b.x*s.b.x+normal.b.y*s.b.y+normal.b.z*s.b.z+planedist;
	if (ta<0) return tri_back;
	return tri_front;
}

float vectorvsplanecheck(vector3 normal, vector3 planevert, vector3 dot)
{
	vector3 v=vect_kivonas(dot,planevert);
	return normal.b.x*v.b.x+normal.b.y*v.b.y+normal.b.z*v.b.z;
}

/*int checkplanetri(vector3 normal, vector3 planevert, vector3 a, vector3 b, vector3 c)
{
	float ac,bc,cc;
	ac=vectorvsplanecheck(normal,planevert,a);
	bc=vectorvsplanecheck(normal,planevert,b);
	cc=vectorvsplanecheck(normal,planevert,c);

	if (fabs(ac)<0.00001 && fabs(bc)<0.00001 && fabs(cc)<0.00001) return tri_onplane;
	if ((ac>=0 || fabs(ac)<0.00001) && 
		(bc>=0 || fabs(bc)<0.00001) && 
		(cc>=0 || fabs(cc)<0.00001)) return tri_front;
	if ((ac<=0 || fabs(ac)<0.00001) && 
		(bc<=0 || fabs(bc)<0.00001) && 
		(cc<=0 || fabs(cc)<0.00001)) return tri_back;
	return tri_split;
}*/

float gett(vector3 a, vector3 b, vector3 p)
{
	float d1=v3_len(vect_kivonas(b,a));
	float d2=v3_len(vect_kivonas(p,a));
	return d2/d1;
}

vector2 gettexc(vector2 a, vector2 b, float t)
{
	vector2 ret;
	ret.b.x=(b.b.x-a.b.x)*t+a.b.x;
	ret.b.y=(b.b.y-a.b.y)*t+a.b.y;
	return ret;
}

void cutplanepoly(vector3 normal, vector3 planevert, cpolygon p, cpolygon *&frontlist, cpolygon *&backlist)
{
	cpolygon *buffer;
	float planedist=-normal.b.x*planevert.b.x-normal.b.y*planevert.b.y-normal.b.z*planevert.b.z;
	float xx[3],yy[3],zz[3],sides[3];
	int num_int;
	bool split;
	plane_imp_triangle_int_3d ( p.a.b.x, p.a.b.y, p.a.b.z, 
		                        p.b.b.x, p.b.b.y, p.b.b.z, 
								p.c.b.x, p.c.b.y, p.c.b.z,  
								normal.b.x, normal.b.y, normal.b.z, planedist,
								&num_int, xx, yy, zz, sides, split);

	vector3 i1,i2;
	vector2 i0;
	i0.b.x=0; i0.b.y=0;
	if (num_int==2)
	{
		i1.b.x=xx[0]; i1.b.y=yy[0]; i1.b.z=zz[0];
		i2.b.x=xx[1]; i2.b.y=yy[1]; i2.b.z=zz[1];
		vector2 i1t,i2t;

		if (sides[0]==1 && sides[1]==2) //ab,bc metszet
		{
			i1t=gettexc(p.data.t1,p.data.t2,gett(p.a,p.b,i1));
			i2t=gettexc(p.data.t2,p.data.t3,gett(p.b,p.c,i2));
			//i1t=i0;
			//i2t=i0;

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i1;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t2=i1t;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i2;
			buffer->c=p.c;
			buffer->data=p.data;
			buffer->data.t2=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			
			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=p.b;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			return;
		}
		else
		if (sides[0]==1 && sides[1]==3) //ab,ac metszet
		{
			i1t=gettexc(p.data.t1,p.data.t2,gett(p.a,p.b,i1));
			i2t=gettexc(p.data.t1,p.data.t3,gett(p.a,p.c,i2));

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i1;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t2=i1t;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}

			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=p.c;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			buffer->data.t2=p.data.t3;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			
			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=p.b;
			buffer->c=p.c;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			return;
		}
		else
		if (sides[0]==3 && sides[1]==2) //bc,ac metszet
		{
			i2t=gettexc(p.data.t2,p.data.t3,gett(p.b,p.c,i2));
			i1t=gettexc(p.data.t1,p.data.t3,gett(p.a,p.c,i1));

			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=i2;
			buffer->c=i1;
			buffer->data=p.data;
			buffer->data.t2=i2t;
			buffer->data.t3=i1t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}

			buffer=new cpolygon;
			buffer->a=i1;
			buffer->b=i2;
			buffer->c=p.c;
			buffer->data=p.data;
			buffer->data.t1=i1t;
			buffer->data.t2=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			
			buffer=new cpolygon;
			buffer->a=p.a;
			buffer->b=p.b;
			buffer->c=i2;
			buffer->data=p.data;
			buffer->data.t3=i2t;
			if (checkplanetri(normal,planevert,buffer->a,buffer->b,buffer->c)==tri_front) {buffer->next=frontlist;frontlist=buffer;}else{buffer->next=backlist;	backlist=buffer;}
			return;
		}
	}
}

bspnode *generatebsptree(cpolygon *inlist)
{
	bspnode *node=new bspnode;
	node->front=NULL;
	node->back=NULL;
	node->polylist=NULL;
	cpolygon *frontlist=NULL;
	cpolygon *backlist=NULL;

	node->planenormal=egyseg(cross(vect_kivonas(inlist->b,inlist->a),
								   vect_kivonas(inlist->c,inlist->a)));
	node->planevertex=inlist->a;

	for (cpolygon *x=inlist; x; x=x->next)
	{
		switch (checkplanetri(node->planenormal,node->planevertex,x->a,x->b,x->c))
		{
		case tri_onplane:
			{
				//MessageBox( 0, "onplane", "HelloWorld", MB_OK );
				cpolygon *buffer=new cpolygon;
				*buffer=*x;
				buffer->next=node->polylist;
				node->polylist=buffer;

				//addpolytolist(&node->polylist,*x);
				break;
			}
		case tri_front:
			{
				//MessageBox( 0, "front", "HelloWorld", MB_OK );
				cpolygon *buffer=new cpolygon;
				*buffer=*x;
				buffer->next=frontlist;
				frontlist=buffer;

				//addpolytolist(&frontlist,*x);
				break;
			}
		case tri_back:
			{
				//MessageBox( 0, "back", "HelloWorld", MB_OK );
				cpolygon *buffer=new cpolygon;
				*buffer=*x;
				buffer->next=backlist;
				backlist=buffer;

				//addpolytolist(&backlist,*x);
				break;
			}
		case tri_split:
			{
				//MessageBox( 0, "split", "HelloWorld", MB_OK );
				cutplanepoly(node->planenormal,node->planevertex,*x,frontlist,backlist);
				break;
			}
		}
	}

	if (frontlist) node->front=generatebsptree(frontlist);
	if (backlist) node->back=generatebsptree(backlist);

	return node;
}

cpolygon *csgclippolygon(bspnode *node, cpolygon *poly, int side, bool &cut)
{
	int test=checkplanetri(node->planenormal,node->planevertex,poly->a,poly->b,poly->c);

	switch (test)
	{
	case tri_front:
		{
			if (node->front) return csgclippolygon(node->front,poly,side,cut);
			if (side==tri_front) {cut=false; return poly;}
			cut=true;
			return NULL;			
			break;
		}
	case tri_back:
		{
			if (node->back) return csgclippolygon(node->back,poly,side,cut);
			if (side==tri_back) {cut=false; return poly;}
			cut=true;
			return NULL;
			break;
		}
	case tri_split:
		{
			cpolygon *pFront=NULL, *pBack=NULL, *pRetFront=NULL, *pRetBack=NULL;
			cutplanepoly(node->planenormal, node->planevertex, *poly, pFront, pBack);
			bool pcut=false;
			bool ppcut;

			if (node->front) //front polykat lenyomni a front agon
			{
				for (cpolygon *x=pFront; x; x=x->next)
				{
					cpolygon *lowerpolys=csgclippolygon(node->front,x,side,ppcut);
					if (ppcut) pcut=true;
					for (cpolygon *y=lowerpolys; y; y=y->next)
					{
						cpolygon *buffer=new cpolygon;
						*buffer=*y;
						buffer->next=pRetFront;
						pRetFront=buffer;
					}
				}
			}
			else 
			{
				if (side==tri_front) pRetFront=pFront;
				else pcut=true;
			}

			if (node->back) //Back polykat lenyomni a Back agon
			{
				for (cpolygon *x=pBack; x; x=x->next)
				{
					cpolygon *lowerpolys=csgclippolygon(node->back,x,side,ppcut);
					if (ppcut) pcut=true;
					for (cpolygon *y=lowerpolys; y; y=y->next)
					{
						cpolygon *buffer=new cpolygon;
						*buffer=*y;
						buffer->next=pRetBack;
						pRetBack=buffer;
					}
				}
			}
			else 
			{
				if (side==tri_back) pRetBack=pBack;
				else pcut=true;
			}

			if (!pcut)
			{
				cut=false;
				return poly;
			}
			cut=true;

			cpolygon *rlist=NULL;
			cpolygon *c;
			for (c=pRetFront; c; c=c->next)
			{
				cpolygon *buffer=new cpolygon;
				*buffer=*c;
				buffer->next=rlist;
				rlist=buffer;
			}
			for (c=pRetBack; c; c=c->next)
			{
				cpolygon *buffer=new cpolygon;
				*buffer=*c;
				buffer->next=rlist;
				rlist=buffer;
			}
			return rlist;

			break;
		}
	}

	return poly;
}

void obj_boolean(object *o1, object *o2, int function)
{
	object *dummy1=newobject(),*dummy2=newobject();
	object *output=newobject();

	matrix m;
	memcpy(m,o1->xformmatrix,sizeof(matrix));
	m_invert(m);

	int z;
	for (z=0; z<o2->vertexnum; z++)
	{
		o2->vertexlist[z].base=o2->vertexlist[z].d;
		m_xformd(m,o2->vertexlist[z].generated,o2->vertexlist[z].d);
	}

	int o1pnum=o1->polynum, 
		o2pnum=o2->polynum;

	cpolygon *p1list=NULL;
	cpolygon *p2list=NULL;

	int x;
	for (x=0; x<o1->polynum; x++)
	{
		cpolygon *buffer=new cpolygon;
		buffer->a=o1->vertexlist[o1->polygonlist[x].v1].d;
		buffer->b=o1->vertexlist[o1->polygonlist[x].v2].d;
		buffer->c=o1->vertexlist[o1->polygonlist[x].v3].d;
		buffer->data=o1->polygonlist[x];
		buffer->next=p1list;
		p1list=buffer;
	}

	for (x=0; x<o2->polynum; x++)
	{
		cpolygon *buffer=new cpolygon;
		buffer->a=o2->vertexlist[o2->polygonlist[x].v1].d;
		buffer->b=o2->vertexlist[o2->polygonlist[x].v2].d;
		buffer->c=o2->vertexlist[o2->polygonlist[x].v3].d;
		buffer->data=o2->polygonlist[x];
		buffer->next=p2list;
		p2list=buffer;
	}

	bspnode *bsp1=generatebsptree(p1list);
	bspnode *bsp2=generatebsptree(p2list);

	//tri_front = tri_inside
	//tri_back  = tri_outside

	//additive : o1 - tri_outside o2 - tri_outside
	//sub      : o1 - tri_outside o2 - tri_inside
	//intersect: o1 - tri_inside  o2 - tri_inside

	int o1func; int o2func;
	bool o1reverse; bool o2reverse;
	switch (function)
	{
	case 0:  //add
		{ 
			o1func=tri_back;
			o2func=tri_back;
			o1reverse=false;
			o2reverse=false;
			break;
		}
	case 1:  //subtract
		{ 
			o1func=tri_back;
			o2func=tri_front;
			o1reverse=false;
			o2reverse=true;
			break;
		}
	case 2:  //intersect
		{ 
			o1func=tri_front;
			o2func=tri_front;
			o1reverse=true;
			o2reverse=true;
			break;
		}
	}

	o1->polynum=0;
	o1->vertexnum=0;

	bool cu;

	cpolygon *a;
	for (a=p1list; a; a=a->next)
	{
		cpolygon c;
		c.a=a->a;
		c.b=a->b;
		c.c=a->c;
		c.data=a->data;
		c.next=NULL;
		cpolygon *r=csgclippolygon(bsp2,&c,o1func,cu);
		for (cpolygon *retlist=r; retlist; retlist=retlist->next)
		{
			addvertex(o1,retlist->a.b.x,retlist->a.b.y,retlist->a.b.z);
			addvertex(o1,retlist->b.b.x,retlist->b.b.y,retlist->b.b.z);
			addvertex(o1,retlist->c.b.x,retlist->c.b.y,retlist->c.b.z);
			addpolygon(o1,0,0,0,0);
			o1->polygonlist[o1->polynum-1]=retlist->data;
			o1->polygonlist[o1->polynum-1].d1=o1->polygonlist[o1->polynum-1].t1;
			o1->polygonlist[o1->polynum-1].d2=o1->polygonlist[o1->polynum-1].t2;
			o1->polygonlist[o1->polynum-1].d3=o1->polygonlist[o1->polynum-1].t3;
			if (!o1reverse)
			{
				o1->polygonlist[o1->polynum-1].v1=o1->vertexnum-3;
				o1->polygonlist[o1->polynum-1].v2=o1->vertexnum-2;
				o1->polygonlist[o1->polynum-1].v3=o1->vertexnum-1;
			}
			else
			{
				o1->polygonlist[o1->polynum-1].v1=o1->vertexnum-2;
				o1->polygonlist[o1->polynum-1].v2=o1->vertexnum-3;
				o1->polygonlist[o1->polynum-1].v3=o1->vertexnum-1;
			}
		}
	}
	for (a=p2list; a; a=a->next)
	{
		cpolygon c;
		c.a=a->a;
		c.b=a->b;
		c.c=a->c;
		c.data=a->data;
		c.next=NULL;
		cpolygon *r=csgclippolygon(bsp1,&c,o2func,cu);
		for (cpolygon *retlist=r; retlist; retlist=retlist->next)
		{
			addvertex(o1,retlist->a.b.x,retlist->a.b.y,retlist->a.b.z);
			addvertex(o1,retlist->b.b.x,retlist->b.b.y,retlist->b.b.z);
			addvertex(o1,retlist->c.b.x,retlist->c.b.y,retlist->c.b.z);
			addpolygon(o1,0,0,0,0);
			o1->polygonlist[o1->polynum-1]=retlist->data;
			o1->polygonlist[o1->polynum-1].d1=o1->polygonlist[o1->polynum-1].t1;
			o1->polygonlist[o1->polynum-1].d2=o1->polygonlist[o1->polynum-1].t2;
			o1->polygonlist[o1->polynum-1].d3=o1->polygonlist[o1->polynum-1].t3;
			if (!o2reverse)
			{
				o1->polygonlist[o1->polynum-1].v1=o1->vertexnum-3;
				o1->polygonlist[o1->polynum-1].v2=o1->vertexnum-2;
				o1->polygonlist[o1->polynum-1].v3=o1->vertexnum-1;
			}
			else
			{
				o1->polygonlist[o1->polynum-1].v1=o1->vertexnum-2;
				o1->polygonlist[o1->polynum-1].v2=o1->vertexnum-3;
				o1->polygonlist[o1->polynum-1].v3=o1->vertexnum-1;
			}
		}
	}

	generatenormals(o1);
	obj_transform(o1,o1->xformmatrix);

	for (z=0; z<o2->vertexnum; z++)	o2->vertexlist[z].d=o2->vertexlist[z].base;
	obj_transform(o2,o2->xformmatrix);
}

