#include "filemenu.h"

int fscnbarpos,fscnselected;

material *findmat(int id)
{
	for (material *m=materiallist;m;m=m->next) if (m->number==id) return m;
	return NULL;
}

void markusedmat()
{
	material *a;
	for (material *m=materiallist;m;m=m->next) m->used=false;
	for (scene *s=scenelist;s;s=s->next)
		for (object *o=s->objects;o;o=o->next)
			if (o->objdata.textured || o->primitive==dd3d_map || o->objdata.envmapped)
			{				
				if (o->objdata.textured || o->primitive==dd3d_map)
				{
					a=findmat(o->objdata.material1);
					if (a) a->used=true;
				}
				if (o->objdata.envmapped)
				{
					a=findmat(o->objdata.material2);
					if (a) a->used=true;
				}
			}
	for (event *e=eventlist;e;e=e->next)
	{
		if (e->eventtype == layer2d && e->textured)
		{
			a=findmat(e->texture);
			if (a) a->used=true;
		}
	}
}

void deleteunmarkedmat()
{
	material *prev=NULL;
	for (material *m=materiallist;m;m=m->next)
	{
		if (!m->used)
		{
			if (prev!=NULL)
			{
				prev->next=m->next;
			}
			else materiallist=m->next;
		}
		else prev=m;
	}
}

tTexture *findtex(int id)
{
	for (tTexture *t=texturelist;t;t=t->next) if (t->number==id) return t;
	return NULL;
}

void markusedtex()
{
	for (tTexture *t=texturelist;t;t=t->next) t->used=false;
	for (material *m=materiallist;m;m=m->next)
	{
		tTexture *t=findtex(m->texture);
		t->used=true;
		//t=findtex(m->alpha);
		//t->used=true;
		//if (m->alpha!=-1)
	}
}

void deleteunmarkedtex()
{
	tTexture *prev=NULL;
	for (tTexture *m=texturelist;m;m=m->next)
	{
		if (!m->used)
		{
			if (prev!=NULL)
			{
				prev->next=m->next;
			}
			else texturelist=m->next;
		}
		else prev=m;
	}
}

void markmultiplemat()
{
	for (material *m1=materiallist;m1;m1=m1->next)
	{
		for (material *m=materiallist;m;m=m->next) m->used=true;
		for (material *m2=materiallist;m2;m2=m2->next)
		if (m1!=m2)
		{
			tTexture *t1=findtex(m1->texture);
			tTexture *t2=findtex(m2->texture);
			bool same=true;
			for (int x=0; x<256; x++)
				for (int y=0; y<256; y++)
				{
					if (t1->layers[m1->layer][x][y].a.r!=t2->layers[m2->layer][x][y].a.r ||
						t1->layers[m1->layer][x][y].a.g!=t2->layers[m2->layer][x][y].a.g ||
						t1->layers[m1->layer][x][y].a.b!=t2->layers[m2->layer][x][y].a.b ||
						t1->layers[m1->layer][x][y].a.a!=t2->layers[m2->layer][x][y].a.a) same=false;
				}
				if (same)
				{
					m2->used=false;
					m1->used=true;
					for (scene *s=scenelist;s;s=s->next)
						for (object *o=s->objects;o;o=o->next)
						{
							if (o->objdata.material1==m2->number) o->objdata.material1=m1->number;
							if (o->objdata.material2==m2->number) o->objdata.material2=m1->number;
						}
						for (event *e=eventlist;e;e=e->next)
						{
							if (e->eventtype == layer2d && e->textured && e->texture==m2->number)
							{
								e->texture=m1->number;
							}
						}
				}		
		}
		deleteunmarkedmat();
	}
}

void optimizeproject()
{
	markusedmat();
	deleteunmarkedmat();
	markusedtex();
	deleteunmarkedtex();
	markmultiplemat();
	deleteunmarkedmat();
	markusedtex();
	deleteunmarkedtex();
}

//void 

void savetextureusefile()
{
	FILE *f=fopen("texture.txt","w+t");
	bool effects[256];
	int used[256];
	memset(used,0,256*4);
	memset(effects,0,256);
	for (tTexture *l=texturelist;l;l=l->next)
	{
		for (int x=0;x<l->commandnum;x++)
		{
			effects[l->commands[x].c.commandnumber]=true;
			used[l->commands[x].c.commandnumber]++;
		}
	}
	for (int x=0; x<256; x++)
	{
		if (effects[x]) fprintf(f,"%d (used %d times)\n",x,used[x]);
	}
	fclose(f);
}

int allpolynum,allvertexnum,allobjectnum,alleffectnum,allscenenum,allrendertextureframenum,alleventnum,rendertexnum;

void getstats(int mode)
{
	if (mode==DD_Check)
	{
		allpolynum=allvertexnum=allobjectnum=alleffectnum=allscenenum=allrendertextureframenum=alleventnum=rendertexnum=0;
		
		for (scene *x=scenelist; x=x->next; x)
		{
			if (x->objects)
			for (object *y=x->objects; y=y->next; y)
			{
				allpolynum+=y->polynum;
				allvertexnum+=y->vertexnum;
				if (y->primitive<100) allobjectnum++;
				else alleffectnum++;
			}
			allscenenum++;
		}
		if (eventlist)
		for (event *e=eventlist; e=e->next; e)
		{
			alleventnum++;
			if (e->eventtype==rendertotext) { allrendertextureframenum+=(e->endframe-e->startframe); rendertexnum++;}
		}
	}
	if (mode==DD_Draw)
	{
		char *s=new char[256];
		memset(s,0,256);
		sprintf(s,"Overall Polycount = %d",allpolynum);
		glRasterPos2i(100,200);
		glPrint(s,base);
		sprintf(s,"Overall Vertexcount = %d",allvertexnum);
		glRasterPos2i(100,220);
		glPrint(s,base);
		sprintf(s,"Overall ObjectCount = %d",allobjectnum);
		glRasterPos2i(100,240);
		glPrint(s,base);
		sprintf(s,"Overall Effectcount = %d",alleffectnum);
		glRasterPos2i(100,260);
		glPrint(s,base);
		sprintf(s,"Overall Scenecount = %d",allscenenum);
		glRasterPos2i(100,280);
		glPrint(s,base);
		sprintf(s,"Overall Eventcount = %d",alleventnum);
		glRasterPos2i(100,300);
		glPrint(s,base);
		sprintf(s,"Overall r2txtframecount = %d",allrendertextureframenum);
		glRasterPos2i(100,320);
		glPrint(s,base);
		sprintf(s,"Overall r2txtcount = %d",rendertexnum);
		glRasterPos2i(100,340);
		glPrint(s,base);
	}
}

void filemenu(int mode)
{
	getstats(mode);
	if (button(100,100,170,120,"OPTIMIZE",9,13,base2,false,mode)==DDgui_LeftClick && mode!=DD_AfterCheck) { optimizeproject(); waitleftbutton=true; }
	if (button(100,121,170,141,"TextureO",9,13,base2,false,mode)==DDgui_LeftClick && mode!=DD_AfterCheck) { savetextureusefile(); waitleftbutton=true; }
	if (mode==DD_Draw)
	{
		glColor4f(buttontextlit);
		rectangle(661,120,775,264);
	}	  
	if (mode==DD_Check)
		if (leftclickinwindow(661,121,774,263) && mouseinwindow(661,121,774,260)) fscnselected=min(fscnbarpos+(my-121) / 10,filenum(prjlist)-1);
	if ((mode ==DD_Check) && mouseinwindow(661,121,774,263)) fscnbarpos-=wheel*4;
	scroller(775,120,790,264,15,15,filenum(prjlist),14,fscnbarpos,mode);
	if (mode==DD_Draw)
	{
		pf = prjlist;
		for (x=1;x<=fscnbarpos;x++) pf=pf->next;
		for (x=0;x<=13;x++)
		{
			if (pf!=NULL)
			{
				if (fscnbarpos+x==fscnselected) glColor4f(col4); else glColor4f(buttontextlit);
				glRasterPos2i(665,130+x*10);
				strcpy(st,pf->filedata.cFileName);
				glPrint(st,base2,18);
				pf=pf->next;
			}
		}

		glColor4f(buttontextlit);
		sprintf(s,"%d PROJECTS.",filenum(prjlist));
		glRasterPos2i(683,277);
		glPrint(s,base2);

	}
	glColor4f(1,1,1,1);
	if ((button(686,283,771,299,texbutton1,0,96.0/256.0,85.0/256.0,112.0/256.0,false,mode) == DDgui_LeftClick) && (mode!=DD_AfterCheck))
	{
		pf = prjlist;
		for (x=1;x<=fscnselected;x++) pf=pf->next;
		//sprintf(s,"%s%s",scenedir,pf->filedata.cFileName);
		memset(lastfilename,0,256);
		memcpy(lastfilename,pf->filedata.cFileName,strlen(pf->filedata.cFileName)-4);
		sprintf(s,"%s%s",projectdir,pf->filedata.cFileName);
		//loadaDDictscene(*actualscene,NULL,s,true,true,true,true,true);				
		LoadProject(s);
		modellviews[3].cam=actualscene->editview;
		modellviews[3].cam2=actualscene->editview;
		tTexture *tex=texturelist;
		while ((tex!=NULL) && (tex->number!=selectedtexture)) tex=tex->next;
		memcpy(generatedtexture.commands,tex->commands,sizeof(generatedtexture.commands));
		memcpy(generatedtexture.texts,tex->texts,sizeof(generatedtexture.texts));
		generatedtexture.commandnum=tex->commandnum;
		//memcpy(generatedtexture.layers,tex->layers,sizeof(generatedtexture.layers));
		for (y=0;y<=3;y++)
		{
			glBindTexture(GL_TEXTURE_2D, texlayers[y]);
			glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
		}				
		
		if (materiallist!=NULL)
		{
			matselected=0;
			material *mat=materiallist;
			for (x=1;x<=matselected;x++) mat=mat->next;
			mattexture=mat->handle;
		}				

		waitleftbutton=true;
	}

	if (button(685,300,770,316,texbutton1,0,144.0/256.0,85.0/256.0,160.0/256.0,false,mode) == DDgui_LeftClick) 
	{
		char *ss=readline("Enter Filename (.scn not needed)",210,0,base2,lastfilename);
		if (ss!="")
		{
			//sprintf(s,"%s%s.scn",scenedir,ss);
			//saveaDDictscene(*actualscene,NULL,s,scntexturesave, scncamerasave, scnselectionsave, scnlightsave, scnobjectsave);
			memset(lastfilename,0,256);
			memcpy(lastfilename,ss,strlen(ss));
			sprintf(s,"%s%s.64k",projectdir,ss);
			SaveProject(s);
		}
	}
	if (button(685,317,770,333,texbutton1,0,160.0/256.0,85.0/256.0,176.0/256.0,false,mode) == DDgui_LeftClick) 
	{
		char *ss=readline("Enter Filename (.scn not needed)",210,0,base2,"");
		if (ss!="")
		{
			sprintf(s,"%s%s.m64",minimaldir,ss);
			//SaveMinimalScene(*actualscene,NULL,s);
			saveminimalproject(s,2);
		}
	}
	/*RadioButton(681,341,scntexturesave,"TEXTURES",mode);
	RadioButton(681,355,scncamerasave,"CAMERAS",mode);
	RadioButton(681,369,scnselectionsave,"SELECTIONS",mode);
	RadioButton(681,383,scnlightsave,"LIGHTS",mode);
	RadioButton(681,397,scnobjectsave,"OBJECTS",mode);*/
}

