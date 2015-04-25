#include "IntroInput.h"
#include <stdio.h>
//#include "vectors.h"
//char *ss = new char[256];

float status=0.0f,oldstatus=0.0f;
texture buffertexture;

float readfloat(unsigned char floatbytes,unsigned char **file)
{
	unsigned long int w=0;
	memcpy(&w,*file,floatbytes);
	*file+=floatbytes;
	w=w << ((4-floatbytes)*8);
	return *(float*) &w;
}

void LoadScene(unsigned char *data, scene **scn, tTexture **texlist, tTexture **lasttex, material **matlist, material **lastmat, int scnnum, LOADER_CALLBACK StatusBar)
{
	float oldstatus=status;
    status=0.0;
	*scn=newscene();
	scene *SCENE = *scn;
	unsigned char *file=data;
	int aa,x,y;
	byte texnum;
	byte lasttexture=0;
	byte lastmaterial=0;

	if ((*lasttex)!=NULL) lasttexture=(*lasttex)->number+1;
	if ((*lastmat)!=NULL) lastmaterial=(*lastmat)->number+1;

	SCENE->floatbytes=file++[0];

	//////////////// TEXTURE LOAD ////////////////////

	texnum=file++[0];

	for (aa=1; aa<=texnum; aa++)
	{
		tTexture *newtex = new tTexture[1];
		memset(newtex,0,sizeof(tTexture));
		newtex->number=file++[0]+lasttexture;
		newtex->commandnum=file++[0];
		
		for (x=0;x<newtex->commandnum;x++)
		{
			memcpy(&(newtex->commands[x]),file,9);
			file+=9;
			if (newtex->commands[x].c.commandnumber==DD_text)
			{				
				byte bb=file++[0];
				memcpy(newtex->texts[newtex->commands[x].command[8]].text,file,bb);
				file+=bb;
			}				 
		}

		newtex->next=NULL;
		if ((*lasttex)==NULL)
		{
			(*texlist)=newtex;
			(*lasttex)=newtex;
		}
		else
		{
			(*lasttex)->next=newtex;
			(*lasttex)=newtex;
		}

		buffertexture.commandnum=0;
		memset(buffertexture.commands,0,sizeof(buffertexture.commands));
		memcpy(buffertexture.texts,(*lasttex)->texts,sizeof((*lasttex)->texts));
		memset(buffertexture.layers,0,sizeof(buffertexture.layers));
		//StatusBar((float)scnnum);
		for (x=0;x<(*lasttex)->commandnum;x++) 
		{
			status=oldstatus+(float)((float)(aa-1)/(float)texnum+((float)x/(float)(*lasttex)->commandnum)/(float)texnum);
			buffertexture.performcommand((*lasttex)->commands[x]);
			StatusBar((float)scnnum);
		}
		memcpy((*lasttex)->layers,buffertexture.layers,sizeof(buffertexture.layers));
		StatusBar((float)scnnum);
	}

	/////////////// MATERIAL LOAD ////////////////////

    texnum=file++[0];

	for (aa=1; aa<=texnum; aa++)
	{
		material *newmat= new material[1];
		memset(newmat,0,sizeof(material));

		newmat->texture=file++[0]+lasttexture;
		newmat->layer=file++[0];
		newmat->alpha=file++[0]+lasttexture;
		newmat->alphalayer=file++[0];
		newmat->alphamode=file++[0];
		newmat->number=file++[0]+lastmaterial;

		if (lastmaterial==NULL)
		{
			(*lastmat)=newmat;
			(*matlist)=newmat;
		}
		else
		{
			(*lastmat)->next=newmat;
			(*lastmat)=newmat;
		}

		tTexture *pt=(*texlist);

		while ((pt!=NULL) && (pt->number!=(*lastmat)->texture)) pt=pt->next;
		rgba layer[256][256];
		memset(layer,0,256*256*4);
		memcpy(layer,pt->layers[(*lastmat)->layer],256*256*4);
		for (x=0;x<256;x++)
			for (y=0;y<256;y++)
			{
				layer[x][y].cols[3]=255;
			}

		glGenTextures(1,&(*lastmat)->handle);
		glBindTexture(GL_TEXTURE_2D,(*lastmat)->handle);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,layer);
	}

	/////////////////// OBJECT LOAD /////////////////////

	short int objnum,objID;
	float s;

	tminimalobjdata objdata;

	memcpy(&objnum,file,2); 
	file+=2;

	for (aa=1;aa<=objnum;aa++)
	{
		scene_addobject(SCENE);
	 
		
		memcpy(&objdata,file,sizeof(objdata));
		file+=sizeof(objdata);

		//SCENE->lastobject->objheader=objdata;

		memcpy(&objID,file,2); 
		file+=2;
		SCENE->lastobject->objdata.number=objID;

		if (objdata.textured)     SCENE->lastobject->objdata.material1 =file++[0]+lastmaterial;
		if (objdata.red)          SCENE->lastobject->objdata.color[0]  =file++[0];
		if (objdata.green)        SCENE->lastobject->objdata.color[1]  =file++[0];
		if (objdata.blue)         SCENE->lastobject->objdata.color[2]  =file++[0];
		if (objdata.alpha)        SCENE->lastobject->objdata.color[3]  =file++[0];
		if (objdata.alphamap1set) SCENE->lastobject->objdata.alphamap1 =file++[0]+lastmaterial;
		if (objdata.material2set) SCENE->lastobject->objdata.material2 =file++[0]+lastmaterial;
		if (objdata.texxset)      SCENE->lastobject->objdata.texx      =file++[0];
		if (objdata.texyset)      SCENE->lastobject->objdata.texy      =file++[0];
		if (objdata.texoffxset)   SCENE->lastobject->objdata.texox     =file++[0];
		if (objdata.texoffyset)   SCENE->lastobject->objdata.texoy     =file++[0];

		SCENE->lastobject->primitive=objdata.primitive;

		switch (SCENE->lastobject->primitive)
		{
		case dd3d_grid:
		case dd3d_hasab:
		case dd3d_cone:
		case dd3d_sphere: {
			SCENE->lastobject->params[1]=file++[0];
			SCENE->lastobject->params[2]=file++[0];
			break;
						  }
		case dd3d_arc: {
			SCENE->lastobject->params[1]=file++[0];
			memcpy(&(SCENE->lastobject->params[2]),file,2);
			file+=2;			
			break;
					   }
		case dd3d_line: {
			SCENE->lastobject->params[1]=file++[0];
			break;
						}
		case dd3d_loft: {
			memcpy(&(SCENE->lastobject->params[1]),file,2);
			file+=2;
			memcpy(&(SCENE->lastobject->params[2]),file,2);
			file+=2;
			SCENE->lastobject->params[3]=(int)(SCENE->objects);
			break;
						}
		case dd3d_map:{
			memcpy(&(SCENE->lastobject->objdata.material2),file,1);
			file++;
			//fread(&SCENE->lastobject->objdata,sizeof(SCENE->lastobject->objdata),1,f);
			SCENE->lastobject->objdata.material1+=lastmaterial;
			switch (SCENE->lastobject->objdata.material2)
			{
			case 0:
				{
					vector3 xpse;
					xpse.b.x=readfloat(SCENE->floatbytes,&file);
					xpse.b.y=readfloat(SCENE->floatbytes,&file);
					xpse.b.z=readfloat(SCENE->floatbytes,&file);
					//fread(&xpse,sizeof(vector3),1,f);
					m_xpose(xpse.b.x,xpse.b.y,xpse.b.z,SCENE->lastobject->xformmatrix);
					break;
				}
			case 1:
				{
					vector3 xpse;
					xpse.b.x=readfloat(SCENE->floatbytes,&file);
					xpse.b.y=readfloat(SCENE->floatbytes,&file);
					xpse.b.z=readfloat(SCENE->floatbytes,&file);
					m_scale(xpse.b.x,xpse.b.y,xpse.b.z,SCENE->lastobject->xformmatrix);
					break;
				}
			case 2:
				{
					vector3 xpse;
					float phi;
					xpse.b.x=readfloat(SCENE->floatbytes,&file);
					xpse.b.y=readfloat(SCENE->floatbytes,&file);
					xpse.b.z=readfloat(SCENE->floatbytes,&file);
					phi=readfloat(SCENE->floatbytes,&file);
					m_rotate(xpse.b.x,xpse.b.y,xpse.b.z,phi,SCENE->lastobject->xformmatrix);
					break;
				}
			}
					  }
		case dd3d_blur:
		case dd3d_linearsubdivision:
		case dd3d_butterfly: {
			if (SCENE->lastobject->primitive!=dd3d_map)
			{
				memcpy(&(SCENE->lastobject->params[1]),file,1);
				file++;
			}
			else SCENE->lastobject->params[1]=1;

			//fread(&texnum,sizeof(texnum),1,f); SCENE->lastobject->params[1]=texnum;
			SELECTION *s=NULL,*n=NULL,*last=NULL;
			short int ab,cd,ef;
			memcpy(&(ab),file,2); file+=2;
			for (cd=1;cd<=ab;cd++)
			{
				memcpy(&(ef),file,2); file+=2;
				n=new SELECTION[1];
				n->next=NULL;
				n->selected=ef;//+startobjnum;
				if (s==NULL)
				{
					last=n;
					s=n;
				}
				else
				{
					last->next=n;
					last=n;
				}
				
			}
			SCENE->lastobject->params[2]=int(s);
			SCENE->lastobject->effect=true;
			n=s;
			SCENE->lastobject->cloneparents=n;
			while (n!=NULL)
			{
				for (cd=0; cd<SCENE->lastobject->params[1]; cd++)
				{
					switch (SCENE->lastobject->primitive)
					{
					case dd3d_linearsubdivision:
					case dd3d_butterfly:{
						object *a=searchobjectbynumber(SCENE->objects,n->selected);
						if (a!=NULL)
						{
							butterflysubdivision(a,SCENE->lastobject->primitive==dd3d_linearsubdivision);
							SELECTION *newsel = new SELECTION;
							newsel->selected=SCENE->lastobject->objdata.number;
							newsel->next=a->effects;
							a->effects=newsel;
						}
						break;
										}
					case dd3d_blur:{
						object *a=searchobjectbynumber(SCENE->objects,n->selected);
						if (a!=NULL)
						{
							meshblur(a);
							SELECTION *newsel = new SELECTION;
							newsel->selected=SCENE->lastobject->objdata.number;
							newsel->next=a->effects;
							a->effects=newsel;
						}
						break;
								   }
					case dd3d_map:{
						object *o=searchobjectbynumber(SCENE->objects,n->selected);
						if (o!=NULL)
						{
							SELECTION *newsel = new SELECTION;
							newsel->selected=SCENE->lastobject->objdata.number;
							newsel->next=o->effects;
							o->effects=newsel;

							obj_counttexturecoordinates(o,SCENE->lastobject->objdata.mapping,
								SCENE->lastobject->objdata.texx,SCENE->lastobject->objdata.texy,
								SCENE->lastobject->objdata.texox,SCENE->lastobject->objdata.texoy,
								SCENE->lastobject->objdata.swaptexturexy,SCENE->lastobject->objdata.inverttexx,
								SCENE->lastobject->objdata.inverttexy);
							
							material *pt=*matlist;
							while ((pt!=NULL) && (pt->number!=SCENE->lastobject->objdata.material1)) pt=pt->next;

							//mattexture=pt->handle;
							tTexture *txlist=*texlist;
							for (x=0;x<pt->texture;x++) txlist=txlist->next;
							memcpy(maptexture,txlist->layers[pt->layer],256*256*4);
							switch (SCENE->lastobject->objdata.material2)
							{
							case 0:
								{
									vector3 xpse;
									m_revxpose(SCENE->lastobject->xformmatrix,xpse);
									matrix s;
									for (int a=0; a<o->vertexnum; a++)
									{
										float p=getmappixel(o->vertexlist[a].t,SCENE->lastobject->objdata.chnnl1,SCENE->lastobject->objdata.normalsinverted);
										m_xpose(p*xpse.b.x,p*xpse.b.y,p*xpse.b.z,s);
										m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
									}									
									break;
								}
							case 1:
								{
									vector3 xpse;
									m_revscale(SCENE->lastobject->xformmatrix,xpse);
									matrix s;
									for (int a=0; a<o->vertexnum; a++)
									{
										float p=getmappixel(o->vertexlist[a].t,SCENE->lastobject->objdata.chnnl1,SCENE->lastobject->objdata.normalsinverted);
										m_scale(p*(xpse.b.x-1)+1,p*(xpse.b.y-1)+1,p*(xpse.b.z-1)+1,s);
										m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
									}									
									break;
								}
							case 2:
								{
									vector3 xpse;
									float phi;
									m_revrotate(SCENE->lastobject->xformmatrix,xpse,phi);
									matrix s;
									for (int a=0; a<o->vertexnum; a++)
									{
										float p=getmappixel(o->vertexlist[a].t,SCENE->lastobject->objdata.chnnl1,SCENE->lastobject->objdata.normalsinverted);
										m_rotate(xpse.b.x,xpse.b.y,xpse.b.z,p*phi,s);
										m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
									}									
									break;
								}
							}
							for (x=0; x<o->vertexnum; x++)
							{
								o->vertexlist[x].base=o->vertexlist[x].d;
								o->vertexlist[x].t=o->vertexlist[x].dt;
							}
							for (x=0; x<o->polynum; x++)
							{
								o->polygonlist[x].t1=o->polygonlist[x].d1;
								o->polygonlist[x].t2=o->polygonlist[x].d2;
								o->polygonlist[x].t3=o->polygonlist[x].d3;
							}
							generatenormals(o);
							obj_transform(o,o->xformmatrix);
						}
						break;
								  }
						 }
					 }
					 n=n->next;
				 }
				 break;
								  }
		case dd3d_boolean: {
			memcpy(&(SCENE->lastobject->params[1]),file,1);	file++;
			//fread(&texnum,sizeof(texnum),1,f); SCENE->lastobject->params[1]=texnum;
			memcpy(&(SCENE->lastobject->params[2]),file,2);	file+=2;
			//fread(&objnumber,sizeof(texnum),2,f); SCENE->lastobject->params[2]=objnumber+startobjnum;
			memcpy(&(SCENE->lastobject->params[2]),file,2);	file+=2;
			//fread(&objnumber,sizeof(texnum),2,f); SCENE->lastobject->params[3]=objnumber+startobjnum;
			m_identity(SCENE->lastobject->xformmatrix);
			for (int x=0;x<=2;x++)
				for (int y=0;y<=3;y++)
				{
					/*w=0;
					fread(&w,SCENE->floatbytes,1,f);
					w=w << ((4-SCENE->floatbytes)*8);
					s = *(float*) &w;*/
					SCENE->lastobject->xformmatrix[x][y]=readfloat(SCENE->floatbytes,&file);;
				}
				matrix m;
				object *a=searchobjectbynumber(SCENE->objects,SCENE->lastobject->params[2]);
				object *b=searchobjectbynumber(SCENE->objects,SCENE->lastobject->params[3]);

				memcpy(m,b->xformmatrix,sizeof(matrix));
				obj_transform(b,SCENE->lastobject->xformmatrix);
				obj_boolean(a,b,SCENE->lastobject->params[1]);
				b->hidden=true;
				SCENE->lastobject->effect=true;
				memcpy(b->xformmatrix,m,sizeof(matrix));
				obj_transform(b,b->xformmatrix);
				break;
						   }
		}
		switch (SCENE->lastobject->primitive)
		{
		case dd3d_hasab: {
			SCENE->lastobject->params[3]=file++[0];
			break;								  
						 }
		}

		switch (SCENE->lastobject->primitive)
		{
		case dd3d_box:
		case dd3d_icosaeder:
		case dd3d_dodecaeder:
		case dd3d_sphere:
		case dd3d_hasab:
		case dd3d_cone:
		case dd3d_arc:
		case dd3d_loft:
		case dd3d_line:
		case dd3d_grid:
		case dd3d_freetrack:
		case dd3d_freeobj:
		case dd3d_clone:{

			m_identity(SCENE->lastobject->xformmatrix);
			for (x=0;x<=2;x++)
				for (y=0;y<=3;y++)
				{
					/*w=0;
					memcpy(&w,file,SCENE->floatbytes);
					file+=SCENE->floatbytes;
					w=w << ((4-SCENE->floatbytes)*8);
					s = *(float*) &w;*/
					SCENE->lastobject->xformmatrix[x][y]=readfloat(SCENE->floatbytes,&file);;
				}
				if (SCENE->lastobject->primitive==dd3d_clone)
				{
					SELECTION *s=NULL,*n=NULL,*last=NULL;
					short int ab,cd,ef;
					memcpy(&ab,file,sizeof(ab));
					file+=sizeof(ab);
					for (cd=1;cd<=ab;cd++)
					{
						memcpy(&ef,file,sizeof(ef));
						file+=sizeof(ef);
						n=new SELECTION[1];
						n->next=NULL;
						n->selected=ef;
						if (s==NULL)
						{
							last=n;
							s=n;
						}
						else
						{
							last->next=n;
							last=n;
						}
						
					}
					SCENE->lastobject->params[1]=(int)s;
					SCENE->lastobject->params[2]=(int)SCENE->objects;
				}
				
				obj_createprimitive(SCENE->lastobject,SCENE->lastobject->primitive,SCENE->lastobject->params[1],SCENE->lastobject->params[2],SCENE->lastobject->params[3],SCENE->lastobject->params[4],SCENE->lastobject->params[5]);				
				//butterflysubdivision(SCENE->lastobject);
				obj_transform(SCENE->lastobject,SCENE->lastobject->xformmatrix);

				SCENE->lastobject->objdata.textured=objdata.textured;
				if ((objdata.textured) && (SCENE->lastobject->primitive!=dd3d_clone))
				{
					material *mlist=(*matlist);
					while ((mlist!=NULL) && (mlist->number!=SCENE->lastobject->objdata.material1)) mlist=mlist->next;
					polygon *p=SCENE->lastobject->polygonlist;
					int xa;
					for (xa=0;xa<SCENE->lastobject->polynum;xa++)
						SCENE->lastobject->polygonlist[xa].materialhandle=mlist->handle;
				}
				break;
						}
		}

	}
	status=oldstatus+1;
}

