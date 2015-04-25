#include "introinput.h"

unsigned char readbyte(unsigned char **file)
{
	unsigned char b=**file;
	*file+=1;
	return b;
}

short int readword(unsigned char **file)
{
	short int b;
	memcpy(&b,*file,2);
	*file+=2;			
	return b;
}

float readfloat(unsigned char **file,unsigned char floatbytes)
{
	unsigned long int w=0;
	memcpy(&w,*file,floatbytes);
	*file+=floatbytes;
	w=w << ((4-floatbytes)*8);
	return *(float*) &w;
}

void readmatrix(unsigned char **file, unsigned char floatbytes,matrix &m)
{
	m_identity(m);
	for (int x=0;x<=2;x++)
		for (int y=0;y<=3;y++)
			m[x][y]=readfloat(file,floatbytes);
}

selection *readselection(unsigned char **file)
{
	selection *list=NULL;
	int selsize=readword(file);
	for (int x=0; x<selsize; x++)
	{
		selection *buffer=new selection;
		buffer->next=list;
		list=buffer;
		buffer->selected=readword(file);
	}
	return list;
}

void creatematerial(material *m)
{
	texture *ntexture=findtexture(m->texture);
	texture *alphatexture=findtexture(m->alphatexture);
	for (int x=0; x<256; x++)
		for (int y=0; y<256; y++)
		{
			m->image[x][y]=ntexture->layers[m->layer][x][y];
			m->image[x][y].a=255;
		}
	glGenTextures(1,&m->handle);
	glBindTexture(GL_TEXTURE_2D,m->handle);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,m->image);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA8,256,256,GL_RGBA,GL_UNSIGNED_BYTE,m->image);
}

void setkeydata(KEY *key, int frame, float data)
{
	key->data=data;
	key->frame=frame;
}

void loadprojectfile(unsigned char *file)
{
	unsigned char *f=file;
	scenelist=NULL;
	vector3 xpse;
	float phi;
	char floatbytes=readbyte(&file);
	material *pt;
	texture  *t;

	//////////////// TEXTURE LOAD ////////////////////

	unsigned char texnum=readbyte(&file);
	
	int aa;
	for (aa=1; aa<=texnum; aa++)
	{
		texture *t=new texture;
		inittexture(*t);
		t->ID=readbyte(&file); //texture id
		t->next=texturelist;
		texturelist=t;
		char cmdcnt=readbyte(&file); //command count
		
		for (int x=0;x<cmdcnt;x++)
		{
			texturecommand cmd;
			memcpy(&cmd,file,9);
			file+=9; 
			if (cmd.commandnumber==DD_text)
			{				
				byte bb=readbyte(&file);
				memcpy(t->texts[x].text,file,bb);
				file+=bb; //text
			}
			performcommand(*t,cmd);
			//precalc((float)(aa-1)/(float)(texnum-1)+1.0f/float(texnum)*(float)x/(float)cmdcnt);
			precalc((float)(aa-1)/(float)(texnum)+1.0f/(float)(texnum)*(float)x/(float)cmdcnt);
		}		
	}

	/////////////// MATERIAL LOAD ////////////////////

    texnum=readbyte(&file);

	for (aa=1; aa<=texnum; aa++)
	{
		material *m=new material;
		m->next=materiallist;
		materiallist=m;
		m->texture=readbyte(&file);
		m->layer=readbyte(&file);
		m->alphatexture=readbyte(&file);
		m->alphalayer=readbyte(&file);
		m->alphamode=readbyte(&file);
		m->number=readbyte(&file);
		creatematerial(m);
	}

	byte scenenum=readbyte(&file);

	for (int ar=1; ar<=scenenum; ar++)
	{
		scene *Scene=newscene();
		Scene->number=readbyte(&file);

		/////////////////// ENVIRONMENT LOAD ////////////////////

		Scene->fog=readbyte(&file);
		if (Scene->fog)
		{
			Scene->fogcol[0]=(float)readbyte(&file)/255.0f;
			Scene->fogcol[1]=(float)readbyte(&file)/255.0f;
			Scene->fogcol[2]=(float)readbyte(&file)/255.0f;
			Scene->fogdensity=readfloat(&file,floatbytes);
		}

		int lightnum=readbyte(&file);
		for (int x=0; x<lightnum; x++)
		{
			int ID=readbyte(&file);
			int type=readbyte(&file);
			Scene->lights[x].turnedon=true;
			Scene->lights[x].position[3]=0;
			Scene->lights[x].ambient[0]=(float)readbyte(&file)/255.0f;
			Scene->lights[x].ambient[1]=(float)readbyte(&file)/255.0f;
			Scene->lights[x].ambient[2]=(float)readbyte(&file)/255.0f;
			Scene->lights[x].color[0]=(float)readbyte(&file)/255.0f;
			Scene->lights[x].color[1]=(float)readbyte(&file)/255.0f;
			Scene->lights[x].color[2]=(float)readbyte(&file)/255.0f;
			Scene->lights[x].position[0]=readfloat(&file,floatbytes);
			Scene->lights[x].position[1]=readfloat(&file,floatbytes);
			Scene->lights[x].position[2]=readfloat(&file,floatbytes);
			if (type>=1)
			{			
				Scene->lights[x].position[3]=1;
				Scene->lights[x].c_att=readfloat(&file,floatbytes);
				Scene->lights[x].l_att=readfloat(&file,floatbytes);
				Scene->lights[x].q_att=readfloat(&file,floatbytes);
			}
			if (type==2)
			{
				Scene->lights[x].spot_direction[0]=readfloat(&file,floatbytes);
				Scene->lights[x].spot_direction[1]=readfloat(&file,floatbytes);
				Scene->lights[x].spot_direction[2]=readfloat(&file,floatbytes);
				Scene->lights[x].spot_exponent=readfloat(&file,floatbytes);
				Scene->lights[x].spot_cutoff=readfloat(&file,floatbytes);
			}
			else
				Scene->lights[x].spot_cutoff=180.0;
		}

		/////////////////// OBJECT LOAD /////////////////////

		int objnum=readword(&file);

		for (int aa=1;aa<=objnum;aa++)
		{
			addobject(*Scene);
			tminimalobjdata objdata;
			memcpy(&objdata,file,sizeof(objdata));

			int current=Scene->objectnum-1;

			Scene->objects[current].data=objdata;
			file+=4;
			
			Scene->objects[current].number=readword(&file);
			Scene->objects[current].parent=readword(&file);

			if (objdata.textured || objdata.primitive==aDDict_map)     Scene->objects[current].texture=readbyte(&file); 
			if (objdata.red)          Scene->objects[current].color[0]=(float)readbyte(&file)/255.0f;
			if (objdata.green)        Scene->objects[current].color[1]=(float)readbyte(&file)/255.0f;
			if (objdata.blue)         Scene->objects[current].color[2]=(float)readbyte(&file)/255.0f;
			if (objdata.alpha)        Scene->objects[current].color[3]=(float)readbyte(&file)/255.0f;
			if (objdata.alphamap1set) readbyte(&file); 
			if (objdata.material2set) Scene->objects[current].envmap=readbyte(&file); 
			if (objdata.texxset || objdata.primitive==aDDict_map)      Scene->objects[current].texxscale=readbyte(&file); 
			if (objdata.texyset || objdata.primitive==aDDict_map)      Scene->objects[current].texyscale=readbyte(&file); 
			if (objdata.texoffxset || objdata.primitive==aDDict_map)   Scene->objects[current].texxoffset=readbyte(&file); 
			if (objdata.texoffyset || objdata.primitive==aDDict_map)   Scene->objects[current].texyoffset=readbyte(&file); 

			switch (objdata.primitive)
			{
			case aDDict_grid:
			case aDDict_hasab:
			case aDDict_cone:
			case aDDict_sphere:
				{
					Scene->objects[current].params[0]=readbyte(&file);
					Scene->objects[current].params[1]=readbyte(&file);
					break;
				}
			case aDDict_arc: 
				{
					Scene->objects[current].params[0]=readbyte(&file);
					Scene->objects[current].params[1]=readword(&file);
					break;
				}
			case aDDict_line: 
				{
					Scene->objects[current].params[0]=readbyte(&file);
					break;
				}
			case aDDict_loft: 
				{
					Scene->objects[current].params[0]=readword(&file);
					Scene->objects[current].params[1]=readword(&file);
					Scene->objects[current].params[2]=(int)Scene->objects;
					Scene->objects[current].params[3]=Scene->objectnum;
					break;
				}
			case aDDict_map:
				{
					texnum=readbyte(&file);
					Scene->objects[current].envmap=texnum;
					switch (texnum)
					{
					case 0:
						{
							xpse.x=readfloat(&file,floatbytes);
							xpse.y=readfloat(&file,floatbytes);
							xpse.z=readfloat(&file,floatbytes);
							break;
						}
					case 1:
						{
							xpse.x=readfloat(&file,floatbytes);
							xpse.y=readfloat(&file,floatbytes);
							xpse.z=readfloat(&file,floatbytes);
							break;
						}
					case 2:
						{
							xpse.x=readfloat(&file,floatbytes);
							xpse.y=readfloat(&file,floatbytes);
							xpse.z=readfloat(&file,floatbytes);
							phi=readfloat(&file,floatbytes);
							break;
						}
					}
					pt=findmaterial(Scene->objects[current].texture);
					t=findtexture(pt->texture);
					memcpy(maptexture,t->layers[pt->layer],256*256*4);
				}
			case aDDict_blur:
			case aDDict_linearsubdivision:
			case aDDict_butterfly: 
				{
					if (objdata.primitive!=aDDict_map)
						texnum=readbyte(&file);
					
					Scene->objects[current].params[1]=(int)readselection(&file);
					for (selection *s=(selection*)Scene->objects[current].params[1];s;s=s->next)
					{
						switch (Scene->objects[current].data.primitive)
						{
						case aDDict_map:
							{
								object *o=searchobjectbynumber(Scene->objects,Scene->objectnum,s->selected);
								obj_counttexturecoordinates( o,Scene->objects[current].texxscale,Scene->objects[current].texyscale,
									Scene->objects[current].texxoffset,
									Scene->objects[current].texyoffset,
									!Scene->objects[current].data.swaptexturexy,
									Scene->objects[current].data.inverttexx,
									Scene->objects[current].data.inverttexy);

								switch (Scene->objects[current].envmap)
								{
								case 0:
									{
										matrix s;
										for (int a=0; a<o->vertexnum; a++)
										{
											float p=getmappixel(o->vertices[a].t,Scene->objects[current].data.alphachannel,
												Scene->objects[current].data.normalsinverted);
											m_xpose(p*xpse.x,p*xpse.y,p*xpse.z,s);
											m_xformd(s,o->vertices[a].base,o->vertices[a].d);
										}
										break;
									}
								case 1:
									{
										matrix s;
										for (int a=0; a<o->vertexnum; a++)
										{
											float p=getmappixel(o->vertices[a].t,Scene->objects[current].data.alphachannel,
												Scene->objects[current].data.normalsinverted);
											m_scale(p*(xpse.x-1)+1,p*(xpse.y-1)+1,p*(xpse.z-1)+1,s);
											m_xformd(s,o->vertices[a].base,o->vertices[a].d);
										}									
										break;
									}
								case 2:
									{
										matrix s;
										for (int a=0; a<o->vertexnum; a++)
										{
											float p=getmappixel(o->vertices[a].t,Scene->objects[current].data.alphachannel,
												Scene->objects[current].data.normalsinverted);
											m_rotate(xpse.x,xpse.y,xpse.z,p*phi,s);
											m_xformd(s,o->vertices[a].base,o->vertices[a].d);
										}									
										break;
									}
								}
								for (int x=0; x<o->vertexnum; x++)
								{
									o->vertices[x].base=o->vertices[x].d;
									o->vertices[x].t=o->vertices[x].dt;
								}
								obj_counttexturecoordinates(o, o->texxscale, o->texyscale, o->texxoffset, o->texyoffset, o->data.swaptexturexy, o->data.inverttexx, o->data.inverttexy);
								obj_generatenormals(o);
								obj_transform(o,o->xformmatrix);
								break;
							}
						case aDDict_blur:
						case aDDict_linearsubdivision:
						case aDDict_butterfly: 
							{
								for (selection *s=(selection*)Scene->objects[current].params[1];s;s=s->next)
								{
									object *o=searchobjectbynumber(Scene->objects,Scene->objectnum,s->selected);
									for (int x=0; x<texnum; x++)
									{
										if (objdata.primitive==aDDict_blur) meshblur(o);
										else butterflysubdivision(o,objdata.primitive==aDDict_linearsubdivision);
									}
								}
								break;
							}
						}
					}
					
					break;
				}
			case aDDict_boolean: 
				{
					char function=readbyte(&file);
					int baseobj=readword(&file);
					int brush=readword(&file);
					//object *baseobject=searchobjectbynumber(Scene->objects,Scene->objectnum,baseobj);
					//object *brushobject=searchobjectbynumber(Scene->objects,Scene->objectnum,brush);
					matrix difference;//,m;
					readmatrix(&file, floatbytes, difference);

					//memcpy(m,brushobject->xformmatrix,sizeof(matrix));
					//matrix m2;
					//m_mult(baseobject->xformmatrix,difference,m2);
					//obj_transform(brushobject,m2);
					//obj_boolean(baseobject,brushobject,function);
					//memcpy(brushobject->xformmatrix,m,sizeof(matrix));
					//obj_transform(brushobject,brushobject->xformmatrix);
					break;
				}
			}
			
			if (objdata.primitive==aDDict_hasab) Scene->objects[current].params[2]=readbyte(&file);

			switch (objdata.primitive)
			{
			case aDDict_box:
			case aDDict_icosaeder:
			case aDDict_dodecaeder:
			case aDDict_sphere:
			case aDDict_hasab:
			case aDDict_cone:
			case aDDict_arc:
			case aDDict_loft:
			case aDDict_line:
			case aDDict_grid:
			case aDDict_clone:
				{
					readmatrix(&file, floatbytes, Scene->objects[current].xformmatrix);
					
					if (objdata.primitive==aDDict_clone)
					{
						Scene->objects[current].params[0]=(int)readselection(&file);
						Scene->objects[current].params[1]=(int)Scene->objects;
						Scene->objects[current].params[2]=Scene->objectnum;
					}
					
					obj_createprimitive(&Scene->objects[current],Scene->objects[current].data.primitive,Scene->objects[current].params[0],Scene->objects[current].params[1],Scene->objects[current].params[2],Scene->objects[current].params[3]);
					obj_transform(&Scene->objects[current],Scene->objects[current].xformmatrix);
					if (Scene->objects[current].data.primitive!=aDDict_clone)
					obj_counttexturecoordinates(&Scene->objects[current], Scene->objects[current].texxscale, Scene->objects[current].texyscale, Scene->objects[current].texxoffset, Scene->objects[current].texyoffset, Scene->objects[current].data.swaptexturexy, Scene->objects[current].data.inverttexx, Scene->objects[current].data.inverttexy);
					obj_generatenormals(&Scene->objects[current]);

					for (int x=0; x<Scene->objects[current].polygonnum; x++)
					{

						Scene->objects[current].polygons[x].color.x=Scene->objects[current].color[0];
						Scene->objects[current].polygons[x].color.y=Scene->objects[current].color[1];
						Scene->objects[current].polygons[x].color.z=Scene->objects[current].color[2];
						Scene->objects[current].polygons[x].color.w=Scene->objects[current].color[3];
						if (Scene->objects[current].data.shading!=aDDict_default) Scene->objects[current].polygons[x].shading=Scene->objects[current].data.shading;
					}

					if (Scene->objects[current].data.textured && Scene->objects[current].data.primitive!=aDDict_clone)
					{
						material *m=findmaterial(Scene->objects[current].texture);
						if (m!=NULL)
						for (int x=0; x<Scene->objects[current].polygonnum; x++)
						{
							Scene->objects[current].polygons[x].texturehandle=m->handle;
						}
					}

					if (Scene->objects[current].data.material2set && Scene->objects[current].data.primitive!=aDDict_clone)
					{
						material *m=findmaterial(Scene->objects[current].envmap);
						if (m!=NULL)
						for (int x=0; x<Scene->objects[current].polygonnum; x++)
						{
							Scene->objects[current].polygons[x].envmaphandle=m->handle;
						}
					}
					
					break;
				}
			}
		}

		//////////////////////////// CAMERA LOAD ////////////////////////////////
		//MessageBox( 0, "camload", "HelloWorld", MB_OK );
		
		byte camnum=readbyte(&file);
		for (aa=1; aa<=camnum; aa++)
		{
			camera *c=new camera;
			memset(c,0,sizeof(camera));
			c->next=Scene->cameras;
			Scene->cameras=c;
			c->number=readbyte(&file); //camera ID
			c->up.y=-1;
			byte keyframenum=readbyte(&file);
			if (keyframenum)
			{
				//c->eyex.numkey=keyframenum;
				//c->eyex.keys=new KEY[keyframenum];
				c->eyex=new CTrack(keyframenum);
				c->eyey=new CTrack(keyframenum);
				c->eyez=new CTrack(keyframenum);
				c->trgx=new CTrack(keyframenum);
				c->trgy=new CTrack(keyframenum);
				c->trgz=new CTrack(keyframenum);
				c->fovt=new CTrack(keyframenum);
				c->rollt=new CTrack(keyframenum);

				/*c->eyey.numkey=keyframenum;
				c->eyey.keys=new KEY[keyframenum];
				c->eyez.numkey=keyframenum;
				c->eyez.keys=new KEY[keyframenum];
				c->trgx.numkey=keyframenum;
				c->trgx.keys=new KEY[keyframenum];
				c->trgy.numkey=keyframenum;
				c->trgy.keys=new KEY[keyframenum];
				c->trgz.numkey=keyframenum;
				c->trgz.keys=new KEY[keyframenum];
				c->fovt.numkey=keyframenum;
				c->fovt.keys=new KEY[keyframenum];
				c->rollt.numkey=keyframenum;
				c->rollt.keys=new KEY[keyframenum];*/

				int frame=readword(&file); //frame
				int fov=readbyte(&file); //fov
				int roll=readword(&file); //roll
				float eyex=readfloat(&file,floatbytes); //eyex
				float eyey=readfloat(&file,floatbytes); //eyey
				float eyez=readfloat(&file,floatbytes); //eyez
				float trgx=readfloat(&file,floatbytes); //trgx
				float trgy=readfloat(&file,floatbytes); //trgy
				float trgz=readfloat(&file,floatbytes); //trgz
				setkeydata(c->eyex->keys,frame,eyex);
				setkeydata(c->eyey->keys,frame,eyey);
				setkeydata(c->eyez->keys,frame,eyez);
				setkeydata(c->trgx->keys,frame,trgx);
				setkeydata(c->trgy->keys,frame,trgy);
				setkeydata(c->trgz->keys,frame,trgz);
				setkeydata(c->fovt->keys,frame,(float)fov);
				setkeydata(c->rollt->keys,frame,(float)roll);
				/*c->eyex.keys[0].data=eyex;
				c->eyex.keys[0].frame=frame;
				c->eyey.keys[0].data=eyey;
				c->eyey.keys[0].frame=frame;
				c->eyez.keys[0].data=eyez;
				c->eyez.keys[0].frame=frame;
				c->trgx.keys[0].data=trgx;
				c->trgx.keys[0].frame=frame;
				c->trgy.keys[0].data=trgy;
				c->trgy.keys[0].frame=frame;
				c->trgz.keys[0].data=trgz;
				c->trgz.keys[0].frame=frame;
				c->fovt.keys[0].data=(float)fov;
				c->fovt.keys[0].frame=frame;
				c->rollt.keys[0].data=(float)roll;
				c->rollt.keys[0].frame=frame;*/

				for (int x=1; x<keyframenum; x++)
				{
					frame=readword(&file); //frame

					camfield cf;
					memset(&cf,0,sizeof(cf));
					memcpy(&cf,file,1); //mask
					file+=1;

					if (cf.fovwritten) fov=readbyte(&file);
					if (cf.rollwritten) roll=readword(&file);
					if (cf.eyexwritten) eyex=readfloat(&file,floatbytes);
					if (cf.eyeywritten) eyey=readfloat(&file,floatbytes);
					if (cf.eyezwritten) eyez=readfloat(&file,floatbytes);
					if (cf.targetxwritten) trgx=readfloat(&file,floatbytes);
					if (cf.targetywritten) trgy=readfloat(&file,floatbytes);
					if (cf.targetzwritten) trgz=readfloat(&file,floatbytes);

					/*c->eyex.keys[x].data=eyex;
					c->eyex.keys[x].frame=frame;
					c->eyey.keys[x].data=eyey;
					c->eyey.keys[x].frame=frame;
					c->eyez.keys[x].data=eyez;
					c->eyez.keys[x].frame=frame;
					c->trgx.keys[x].data=trgx;
					c->trgx.keys[x].frame=frame;
					c->trgy.keys[x].data=trgy;
					c->trgy.keys[x].frame=frame;
					c->trgz.keys[x].data=trgz;
					c->trgz.keys[x].frame=frame;
					c->fovt.keys[x].data=(float)fov;
					c->fovt.keys[x].frame=frame;
					c->rollt.keys[x].data=(float)roll;
					c->rollt.keys[x].frame=frame;*/
					
					setkeydata(&c->eyex->keys[x],frame,eyex);
					setkeydata(&c->eyey->keys[x],frame,eyey);
					setkeydata(&c->eyez->keys[x],frame,eyez);
					setkeydata(&c->trgx->keys[x],frame,trgx);
					setkeydata(&c->trgy->keys[x],frame,trgy);
					setkeydata(&c->trgz->keys[x],frame,trgz);
					setkeydata(&c->fovt->keys[x],frame,(float)fov);
					setkeydata(&c->rollt->keys[x],frame,(float)roll);

				}
				c->eyex->InitVectors();
				c->eyey->InitVectors();
				c->eyez->InitVectors();
				c->trgx->InitVectors();
				c->trgy->InitVectors();
				c->trgz->InitVectors();
				c->fovt->InitVectors();
				c->rollt->InitVectors();
			}
		}
		//MessageBox( 0, "objload", "HelloWorld", MB_OK );
		///////////////////////// OBJECT ANIM LOAD /////////////////////////////

		byte animnum=readbyte(&file);
		for (aa=1; aa<=animnum; aa++)
		{
			byte animid=readbyte(&file); //anim ID
			int on;
			for (on=0;on<Scene->objectnum;on++)
			{
				objanim *o=new objanim;
				memset(o,0,sizeof(objanim));
				o->next=Scene->objects[on].anims;
				Scene->objects[on].anims=o;
				o->number=animid;
			}
			for (on=0;on<Scene->objectnum;on++)
				if (Scene->objects[on].data.primitive!=9 && 
					Scene->objects[on].data.primitive!=11 &&
					Scene->objects[on].data.primitive<100)
				{
					byte keyframenum=readbyte(&file);
					Scene->objects[on].anims->posx=new CTrack(keyframenum);
					Scene->objects[on].anims->posy=new CTrack(keyframenum);
					Scene->objects[on].anims->posz=new CTrack(keyframenum);
					Scene->objects[on].anims->rotx=new CTrack(keyframenum);
					Scene->objects[on].anims->roty=new CTrack(keyframenum);
					Scene->objects[on].anims->rotz=new CTrack(keyframenum);
					Scene->objects[on].anims->rota=new CTrack(keyframenum);
					Scene->objects[on].anims->strx=new CTrack(keyframenum);
					Scene->objects[on].anims->stry=new CTrack(keyframenum);
					Scene->objects[on].anims->strz=new CTrack(keyframenum);
					Scene->objects[on].anims->colr=new CTrack(keyframenum);
					Scene->objects[on].anims->colg=new CTrack(keyframenum);
					Scene->objects[on].anims->colb=new CTrack(keyframenum);
					Scene->objects[on].anims->cola=new CTrack(keyframenum);
					if (keyframenum)
					{
						int frame=(unsigned short)readword(&file); //frame

						float posx=readfloat(&file,floatbytes); //posx
						float posy=readfloat(&file,floatbytes); //posy
						float posz=readfloat(&file,floatbytes); //posz

						float rotx=readfloat(&file,floatbytes); //rotx
						float roty=readfloat(&file,floatbytes); //roty
						float rotz=readfloat(&file,floatbytes); //rotz
						int rota=readword(&file); //rota

						float strx=readfloat(&file,floatbytes); //strx
						float stry=readfloat(&file,floatbytes); //stry
						float strz=readfloat(&file,floatbytes); //strz

						float colr=(float)readbyte(&file)/255.0f; //colr
						float colg=(float)readbyte(&file)/255.0f; //colg
						float colb=(float)readbyte(&file)/255.0f; //colb
						float cola=(float)readbyte(&file)/255.0f; //cola

						setkeydata(Scene->objects[on].anims->posx->keys,frame,posx);
						setkeydata(Scene->objects[on].anims->posy->keys,frame,posy);
						setkeydata(Scene->objects[on].anims->posz->keys,frame,posz);
						setkeydata(Scene->objects[on].anims->rotx->keys,frame,rotx);
						setkeydata(Scene->objects[on].anims->roty->keys,frame,roty);
						setkeydata(Scene->objects[on].anims->rotz->keys,frame,rotz);
						setkeydata(Scene->objects[on].anims->rota->keys,frame,(float)rota);
						setkeydata(Scene->objects[on].anims->strx->keys,frame,strx);
						setkeydata(Scene->objects[on].anims->stry->keys,frame,stry);
						setkeydata(Scene->objects[on].anims->strz->keys,frame,strz);
						setkeydata(Scene->objects[on].anims->colr->keys,frame,colr);
						setkeydata(Scene->objects[on].anims->colg->keys,frame,colg);
						setkeydata(Scene->objects[on].anims->colb->keys,frame,colb);
						setkeydata(Scene->objects[on].anims->cola->keys,frame,cola);

						/*Scene->objects[on].anims->posx.keys[0].frame=frame;
						Scene->objects[on].anims->posx.keys[0].data=posx;
						Scene->objects[on].anims->posy.keys[0].frame=frame;
						Scene->objects[on].anims->posy.keys[0].data=posy;
						Scene->objects[on].anims->posz.keys[0].frame=frame;
						Scene->objects[on].anims->posz.keys[0].data=posz;
						Scene->objects[on].anims->rotx.keys[0].frame=frame;
						Scene->objects[on].anims->rotx.keys[0].data=rotx;
						Scene->objects[on].anims->roty.keys[0].frame=frame;
						Scene->objects[on].anims->roty.keys[0].data=roty;
						Scene->objects[on].anims->rotz.keys[0].frame=frame;
						Scene->objects[on].anims->rotz.keys[0].data=rotz;
						Scene->objects[on].anims->rota.keys[0].frame=frame;
						Scene->objects[on].anims->rota.keys[0].data=(float)rota;
						Scene->objects[on].anims->strx.keys[0].frame=frame;
						Scene->objects[on].anims->strx.keys[0].data=strx;
						Scene->objects[on].anims->stry.keys[0].frame=frame;
						Scene->objects[on].anims->stry.keys[0].data=stry;
						Scene->objects[on].anims->strz.keys[0].frame=frame;
						Scene->objects[on].anims->strz.keys[0].data=strz;
						Scene->objects[on].anims->colr.keys[0].frame=frame;
						Scene->objects[on].anims->colr.keys[0].data=colr;
						Scene->objects[on].anims->colg.keys[0].frame=frame;
						Scene->objects[on].anims->colg.keys[0].data=colg;
						Scene->objects[on].anims->colb.keys[0].frame=frame;
						Scene->objects[on].anims->colb.keys[0].data=colb;
						Scene->objects[on].anims->cola.keys[0].frame=frame;
						Scene->objects[on].anims->cola.keys[0].data=cola;*/

						for (int x=1; x<keyframenum; x++)
						{
							frame=readword(&file); //frame

							objfield c;
							memset(&c,0,sizeof(c));
							memcpy(&c,file,2); //mask
							file+=2;

							if (c.posx) posx=readfloat(&file,floatbytes);
							if (c.posy) posy=readfloat(&file,floatbytes);
							if (c.posz) posz=readfloat(&file,floatbytes);

							if (c.rotx) rotx=readfloat(&file,floatbytes);
							if (c.roty) roty=readfloat(&file,floatbytes);
							if (c.rotz) rotz=readfloat(&file,floatbytes);
							if (c.rota) rota=readword(&file);

							if (c.strx) strx=readfloat(&file,floatbytes);
							if (c.stry) stry=readfloat(&file,floatbytes);
							if (c.strz) strz=readfloat(&file,floatbytes);

							if (c.colr) colr=(float)readbyte(&file)/255.f;
							if (c.colg) colg=(float)readbyte(&file)/255.f;
							if (c.colb) colb=(float)readbyte(&file)/255.f;
							if (c.cola) cola=(float)readbyte(&file)/255.f;

							/*Scene->objects[on].anims->posx.keys[x].frame=frame;
							Scene->objects[on].anims->posx.keys[x].data=posx;
							Scene->objects[on].anims->posy.keys[x].frame=frame;
							Scene->objects[on].anims->posy.keys[x].data=posy;
							Scene->objects[on].anims->posz.keys[x].frame=frame;
							Scene->objects[on].anims->posz.keys[x].data=posz;
							Scene->objects[on].anims->rotx.keys[x].frame=frame;
							Scene->objects[on].anims->rotx.keys[x].data=rotx;
							Scene->objects[on].anims->roty.keys[x].frame=frame;
							Scene->objects[on].anims->roty.keys[x].data=roty;
							Scene->objects[on].anims->rotz.keys[x].frame=frame;
							Scene->objects[on].anims->rotz.keys[x].data=rotz;
							Scene->objects[on].anims->rota.keys[x].frame=frame;
							Scene->objects[on].anims->rota.keys[x].data=(float)rota;
							Scene->objects[on].anims->strx.keys[x].frame=frame;
							Scene->objects[on].anims->strx.keys[x].data=strx;
							Scene->objects[on].anims->stry.keys[x].frame=frame;
							Scene->objects[on].anims->stry.keys[x].data=stry;
							Scene->objects[on].anims->strz.keys[x].frame=frame;
							Scene->objects[on].anims->strz.keys[x].data=strz;
							Scene->objects[on].anims->colr.keys[x].frame=frame;
							Scene->objects[on].anims->colr.keys[x].data=colr;
							Scene->objects[on].anims->colg.keys[x].frame=frame;
							Scene->objects[on].anims->colg.keys[x].data=colg;
							Scene->objects[on].anims->colb.keys[x].frame=frame;
							Scene->objects[on].anims->colb.keys[x].data=colb;
							Scene->objects[on].anims->cola.keys[x].frame=frame;
							Scene->objects[on].anims->cola.keys[x].data=cola;*/
						
							setkeydata(&Scene->objects[on].anims->posx->keys[x],frame,posx);
							setkeydata(&Scene->objects[on].anims->posy->keys[x],frame,posy);
							setkeydata(&Scene->objects[on].anims->posz->keys[x],frame,posz);
							setkeydata(&Scene->objects[on].anims->rotx->keys[x],frame,rotx);
							setkeydata(&Scene->objects[on].anims->roty->keys[x],frame,roty);
							setkeydata(&Scene->objects[on].anims->rotz->keys[x],frame,rotz);
							setkeydata(&Scene->objects[on].anims->rota->keys[x],frame,(float)rota);
							setkeydata(&Scene->objects[on].anims->strx->keys[x],frame,strx);
							setkeydata(&Scene->objects[on].anims->stry->keys[x],frame,stry);
							setkeydata(&Scene->objects[on].anims->strz->keys[x],frame,strz);
							setkeydata(&Scene->objects[on].anims->colr->keys[x],frame,colr);
							setkeydata(&Scene->objects[on].anims->colg->keys[x],frame,colg);
							setkeydata(&Scene->objects[on].anims->colb->keys[x],frame,colb);
							setkeydata(&Scene->objects[on].anims->cola->keys[x],frame,cola);
						}

						Scene->objects[on].anims->posx->InitVectors();
						Scene->objects[on].anims->posy->InitVectors();
						Scene->objects[on].anims->posz->InitVectors();
						Scene->objects[on].anims->rotx->InitVectors();
						Scene->objects[on].anims->roty->InitVectors();
						Scene->objects[on].anims->rotz->InitVectors();
						Scene->objects[on].anims->rota->InitVectors();
						Scene->objects[on].anims->strx->InitVectors();
						Scene->objects[on].anims->stry->InitVectors();
						Scene->objects[on].anims->strz->InitVectors();
						Scene->objects[on].anims->colr->InitVectors();
						Scene->objects[on].anims->colg->InitVectors();
						Scene->objects[on].anims->colb->InitVectors();
						Scene->objects[on].anims->cola->InitVectors();
					}
				}
		}

		Scene->next=scenelist;
		scenelist=Scene;
	}

	////////////////////////// EVENT LOAD ///////////////////////////////

	int eventnum=readword(&file);
	for (aa=1; aa<=eventnum; aa++)
	{
		event *e=new event;
		memset(e,0,sizeof(event));
		if (eventlist==NULL)
		{
			eventlist=e;
			lastevent=e;
		}
		else
		{
			lastevent->next=e;
			lastevent=e;
		}

		e->eventtype=readbyte(&file);
		e->startframe=(unsigned short)readword(&file)*10; //startframe
		e->endframe=(unsigned short)readword(&file)*10+9; //endframe
		e->pass=readbyte(&file); //pass
		if (e->eventtype==layer2d || e->eventtype==layer3d || e->eventtype==rendertotext || e->eventtype==feedback || e->eventtype==grideffect)
		{
			e->startrectx1=readword(&file);
			e->startrecty1=readword(&file);
			e->startrectx2=readword(&file);
			e->startrecty2=readword(&file);

			e->endrectx1=readword(&file);
			e->endrecty1=readword(&file);
			e->endrectx2=readword(&file);
			e->endrecty2=readword(&file);
		}
		if (e->eventtype==layer2d || e->eventtype==feedback || e->eventtype==grideffect)
		{
			e->startcol[0]=(float)readbyte(&file)/255.0f;
			e->startcol[1]=(float)readbyte(&file)/255.0f;
			e->startcol[2]=(float)readbyte(&file)/255.0f;
			e->startcol[3]=(float)readbyte(&file)/255.0f;

			e->endcol[0]=(float)readbyte(&file)/255.0f;
			e->endcol[1]=(float)readbyte(&file)/255.0f;
			e->endcol[2]=(float)readbyte(&file)/255.0f;
			e->endcol[3]=(float)readbyte(&file)/255.0f;

			switch (readbyte(&file))
			{
				case  0:e->blendfunc1=GL_ZERO; break;
				case  1:e->blendfunc1=GL_ONE; break;
				case  2:e->blendfunc1=GL_SRC_COLOR; break;
				case  3:e->blendfunc1=GL_ONE_MINUS_SRC_COLOR; break;
				case  4:e->blendfunc1=GL_SRC_ALPHA; break;
				case  5:e->blendfunc1=GL_ONE_MINUS_SRC_ALPHA; break;
				case  6:e->blendfunc1=GL_DST_ALPHA; break;
				case  7:e->blendfunc1=GL_ONE_MINUS_DST_ALPHA; break;
				case  8:e->blendfunc1=GL_DST_COLOR; break;
				case  9:e->blendfunc1=GL_ONE_MINUS_DST_COLOR; break;
				case 10:e->blendfunc1=GL_SRC_ALPHA_SATURATE; break;
			}
			switch (readbyte(&file))
			{
				case  0:e->blendfunc2=GL_ZERO; break;
				case  1:e->blendfunc2=GL_ONE; break;
				case  2:e->blendfunc2=GL_SRC_COLOR; break;
				case  3:e->blendfunc2=GL_ONE_MINUS_SRC_COLOR; break;
				case  4:e->blendfunc2=GL_SRC_ALPHA; break;
				case  5:e->blendfunc2=GL_ONE_MINUS_SRC_ALPHA; break;
				case  6:e->blendfunc2=GL_DST_ALPHA; break;
				case  7:e->blendfunc2=GL_ONE_MINUS_DST_ALPHA; break;
				case  8:e->blendfunc2=GL_DST_COLOR; break;
				case  9:e->blendfunc2=GL_ONE_MINUS_DST_COLOR; break;
				case 10:e->blendfunc2=GL_SRC_ALPHA_SATURATE; break;
			}
		}
		
		switch (e->eventtype)
		{
		case layer2d:
			{
				e->textured=readbyte(&file);
				e->texture=readbyte(&file);
				e->mattexture=findmaterial(e->texture)->handle;
				break;
			}
		case layer3d:
			{
				e->sceneid=readbyte(&file);
				e->camid=readbyte(&file);
				e->animid=readbyte(&file);
				e->camstart=readword(&file);
				e->camend=readword(&file);
				e->animstart=readword(&file);
				e->animend=readword(&file);
				e->iscene=findscene(e->sceneid);
				e->icam=findcam(e->iscene,e->camid);
				e->ianim=e->animid;
				break;
			}
		case cleargl:
			{
				e->clearscreen=readbyte(&file);
				e->clearzbuffer=readbyte(&file);
				break;
			}
		case rendertotext:
			{
				e->texture=readbyte(&file);
				break;
			}
		case feedback:
			{
				e->texture=readbyte(&file);
				e->param1=readbyte(&file);
				e->param4=readfloat(&file,floatbytes);
				break;
			}
		case grideffect:
			{
				e->texture=readbyte(&file);
				e->effect=readbyte(&file);
				e->gridstart=readfloat(&file,floatbytes);
				e->gridend=readfloat(&file,floatbytes);
				break;
			}
		}
	}
}