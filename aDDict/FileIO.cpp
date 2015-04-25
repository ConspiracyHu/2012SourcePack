#include "FileIO.h"

unsigned long masks[5];
char *sss2 = new char[256];

int numberoftextures;
int aa=0;

void writefloat(FILE *f, float s, byte bytenum)
{
	/*s=o->xformmatrix[x][y];
	w=*(unsigned long*)(&s);
	w=(w & masks[scn.floatbytes]) >> ((4-scn.floatbytes)*8);
	fwrite(&w,scn.floatbytes,1,f);*/

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

byte texnum;

void savetextures(FILE *f) //save textures & materials
{
     byte texnum;
	 texnum=texturenum(texturelist);
	 fwrite(&texnum,sizeof(texnum),1,f);
	 int aa = 0;
	 tTexture *list=texturelist;
	 for (aa=1; aa<=texnum; aa++)
	 {
		 byte b=list->number;
		 fwrite(&b,1,1,f);
		 b=list->commandnum;
		 fwrite(&b,1,1,f);
		 for (x=0;x<b;x++)
		 {
			 fwrite(&(list->commands[x]),1,9,f);
			 if (list->commands[x].c.commandnumber==DD_text)
			 {
				 byte bb=strlen(list->texts[list->commands[x].command[8]].text);
				 fwrite(&bb,1,1,f);
				 fwrite(list->texts[list->commands[x].command[8]].text,bb,1,f);
			 }				 
		 }
		 list=list->next;
	 }
	 texnum=materialnum(materiallist);
	 fwrite(&texnum,sizeof(texnum),1,f);
	 material *matlist = materiallist;
	 for (aa=1; aa<=texnum; aa++)
	 {
		 byte b=matlist->texture;
		 fwrite(&b,1,1,f);
		 b=matlist->layer;
		 fwrite(&b,1,1,f);
		 b=matlist->alpha;
		 fwrite(&b,1,1,f);
		 b=matlist->alphalayer;
		 fwrite(&b,1,1,f);
		 b=matlist->alphamode;
		 fwrite(&b,1,1,f);
		 b=matlist->number;
		 fwrite(&b,1,1,f);

		 matlist=matlist->next;
	 }
}

void saveobjects(scene &scn, FILE *f)
{
	short int objnumber;
	byte texnum;
	object *o;
	tminimalobjdata objdata;
	objnumber=objnum(scn.objects);
	fwrite(&objnumber,sizeof(objnumber),1,f);

	masks[0]=0x00000000;
	masks[1]=0xff000000;
	masks[2]=0xffff0000;
	masks[3]=0xffffff00;
	masks[4]=0xffffffff;
	
	o=scn.objects;
	while (o!=NULL)
	{
		memset(&objdata,0,sizeof(objdata));
		objdata.primitive=o->primitive;                // PRIMITIV
		objdata.normalsinverted=o->objdata.normalsinverted; //NORMALOK INVERTALASA
		objdata.notlit=o->objdata.notlit;              // FENY HATASA
		objdata.textured=o->objdata.textured;          // textura materialt ki kell irni
		if (o->objdata.color[0]!=255) objdata.red=1;   // piros nem default, ki kell irni
		if (o->objdata.color[1]!=255) objdata.green=1; // zold nem default, ki kell irni
		if (o->objdata.color[2]!=255) objdata.blue=1;  // kek nem default, ki kell irni
		if (o->objdata.color[3]!=255) objdata.alpha=1; // alfa nem default, ki kell irni
		objdata.alphamap1set=o->objdata.alphamapped;   // alpha materialt ki kell irni
		objdata.alphachannel=o->objdata.chnnl1;        // ALPHA CHANNEL
		objdata.material2set=o->objdata.envmapped;     // envmap materialt ki kell irni
		if (o->objdata.texx!=1) objdata.texxset=1;     //textura x huzas nem default, ki kell irni
		if (o->objdata.texy!=1) objdata.texyset=1;     //textura y huzas nem default, ki kell irni
		if (o->objdata.texox!=0) objdata.texoffxset=1; //textura x offset nem default, ki kell irni
		if (o->objdata.texoy!=0) objdata.texoffyset=1; //textura y offset nem default, ki kell irni
		objdata.shading=o->objdata.shading;            //SHADING
		objdata.mapping=o->objdata.mapping;            //TEXTURE MAPPING
		objdata.inverttexx=o->objdata.inverttexx;
		objdata.inverttexy=o->objdata.inverttexy;
		objdata.swaptexturexy=o->objdata.swaptexturexy;

		fwrite(&objdata,sizeof(tminimalobjdata),1,f);
		objnumber=o->objdata.number;
		fwrite(&objnumber,2,1,f);
		fwrite(&o->parent,2,1,f);

		if (objdata.textured || objdata.primitive==dd3d_map) //textura material kiirasa
		{	texnum=o->objdata.material1;
		    fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.red) //piros kiirasa
		{	texnum=o->objdata.color[0];
			fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.green) //zold kiirasa
		{	texnum=o->objdata.color[1];
			fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.blue) //kek kiirasa
		{	texnum=o->objdata.color[2];
			fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.alpha) //alfa kiirasa
		{	texnum=o->objdata.color[3];
			fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.alphamap1set) //alpha material kiirasa
		{	texnum=o->objdata.alphamap1;
		    fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.material2set) //envmap material kiirasa
		{	texnum=o->objdata.material2;
		    fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.texxset || objdata.primitive==dd3d_map) //textura x huzas kiirasa
		{   texnum=o->objdata.texx;
		    fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.texyset || objdata.primitive==dd3d_map) //textura y huzas kiirasa
		{   texnum=o->objdata.texy;
		    fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.texoffxset || objdata.primitive==dd3d_map) //textura x offset kiirasa
		{   texnum=o->objdata.texox;
		    fwrite(&texnum,sizeof(texnum),1,f);}
		if (objdata.texoffyset || objdata.primitive==dd3d_map) //textura y offset kiirasa
		{   texnum=o->objdata.texoy;
		    fwrite(&texnum,sizeof(texnum),1,f);}

		switch (o->primitive)
		{
		case dd3d_grid:
		case dd3d_hasab:
		case dd3d_cone:
		case dd3d_sphere: {
			texnum=o->params[1];
			fwrite(&texnum,sizeof(texnum),1,f);
			texnum=o->params[2];
			fwrite(&texnum,sizeof(texnum),1,f);
			break;	
						  }
		case dd3d_arc: {
			texnum=o->params[1];
			fwrite(&texnum,sizeof(texnum),1,f);
			objnumber=o->params[2];
			fwrite(&objnumber,sizeof(objnumber),1,f);
			break;
					   }
		case dd3d_line: {
			texnum=o->params[1];
			fwrite(&texnum,sizeof(texnum),1,f);
			break;
						}
		case dd3d_loft: {
			objnumber=o->params[1];
			fwrite(&objnumber,sizeof(objnumber),1,f);
			objnumber=o->params[2];
			fwrite(&objnumber,sizeof(objnumber),1,f);
			break;
						}
		case dd3d_map:{
			//fwrite(&o->objdata,sizeof(o->objdata),1,f);
			texnum=o->objdata.material2;
			fwrite(&texnum,sizeof(texnum),1,f);
			switch (texnum)
			{
			case 0:
				{
					vector3 xpse;
					m_revxpose(o->xformmatrix,xpse);
					writefloat(f,xpse.b.x,scn.floatbytes);
					writefloat(f,xpse.b.y,scn.floatbytes);
					writefloat(f,xpse.b.z,scn.floatbytes);
					break;
				}
			case 1:
				{
					vector3 xpse;
					m_revscale(o->xformmatrix,xpse);
					writefloat(f,xpse.b.x,scn.floatbytes);
					writefloat(f,xpse.b.y,scn.floatbytes);
					writefloat(f,xpse.b.z,scn.floatbytes);
					break;
				}
			case 2:
				{
					vector3 xpse;
					float phi;
					m_revrotate(o->xformmatrix,xpse,phi);
					writefloat(f,xpse.b.x,scn.floatbytes);
					writefloat(f,xpse.b.y,scn.floatbytes);
					writefloat(f,xpse.b.z,scn.floatbytes);
					writefloat(f,phi,scn.floatbytes);
					break;
				}
			}
					  }
		case dd3d_blur:
		case dd3d_linearsubdivision:
		case dd3d_butterfly: {
			if (o->primitive!=dd3d_map)
			{
				texnum=o->params[1];
				fwrite(&texnum,sizeof(texnum),1,f);
			}
			SELECTION *s=(SELECTION*)o->params[2];
			objnumber=selsize(s);
			fwrite(&objnumber,sizeof(objnumber),1,f);
			while (s!=NULL)
			{
				objnumber=s->selected;
				fwrite(&objnumber,sizeof(objnumber),1,f);
				s=s->next;
			}
			break;
							 }

		case dd3d_boolean: {
			texnum=o->params[1];
			fwrite(&texnum,sizeof(texnum),1,f);
			objnumber=o->params[2];
			fwrite(&objnumber,sizeof(objnumber),1,f);
			objnumber=o->params[3];
			fwrite(&objnumber,sizeof(objnumber),1,f);
			for (x=0;x<=2;x++)
				for (y=0;y<=3;y++)
					writefloat(f,o->xformmatrix[x][y],scn.floatbytes);
			break;
						   }
		}
		
		switch (o->primitive)
		{
		case dd3d_hasab: {
			texnum=o->params[3];
			fwrite(&texnum,sizeof(texnum),1,f);
			break;								  
						 }
		}

		switch (o->primitive)
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

			char *s2s = new char[256];
			for (x=0;x<=2;x++)
				for (y=0;y<=3;y++)
				{
					writefloat(f,o->xformmatrix[x][y],scn.floatbytes);
				}
				if (o->primitive==dd3d_clone)
				{
					SELECTION *s=(SELECTION*)o->params[1];
					objnumber=selsize(s);
					fwrite(&objnumber,sizeof(objnumber),1,f);
					while (s!=NULL)
					{
						objnumber=s->selected;
						fwrite(&objnumber,sizeof(objnumber),1,f);
						s=s->next;
					}
				}
				break;
						}
		}

		o=o->next;
	}
}

void SaveMinimalScene(scene &scn, tTexture *textures, const char *filename)
{
 FILE *f;
 f=fopen(filename,"w+b");
 byte texnum;
 if (f==NULL) return;
 else
 {
	 scn.floatbytes=3;
	 texnum=scn.floatbytes;
	 fwrite(&texnum,1,1,f);
	 savetextures(f);
	 if (scn.fog)
	 {
		 texnum=1;
		 fwrite(&texnum,1,1,f);
		 texnum=(int)(scn.fogcol[0]*255);
		 fwrite(&texnum,1,1,f);
		 texnum=(int)(scn.fogcol[1]*255);
		 fwrite(&texnum,1,1,f);
		 texnum=(int)(scn.fogcol[2]*255);
		 fwrite(&texnum,1,1,f);
		 writefloat(f,scn.fogdensity,scn.floatbytes);
	 }
	 else
	 {
		 texnum=0;
		 fwrite(&texnum,1,1,f);
	 }
	 texnum=0;
	 for (int x=0; x<8; x++) if (scn.lights[x].turnedon) texnum++;
	 fwrite(&texnum,1,1,f); //light number
	 byte buf;
	 for (x=0; x<8; x++)
		 if (scn.lights[x].turnedon)
		 {
			 buf=x;
			 fwrite(&buf,1,1,f); //light id
			 buf=1; //dotlight
			 if (scn.lights[x].spot_cutoff!=180) buf=2;//spotlight
			 if (scn.lights[x].position[3]==0) buf=0; //dir light
			 fwrite(&buf,1,1,f); //light type

			 buf=(int)(scn.lights[x].ambient[0]*255);
			 fwrite(&buf,1,1,f);
			 buf=(int)(scn.lights[x].ambient[1]*255);
			 fwrite(&buf,1,1,f);
			 buf=(int)(scn.lights[x].ambient[2]*255);
			 fwrite(&buf,1,1,f);
			 buf=(int)(scn.lights[x].color[0]*255);
			 fwrite(&buf,1,1,f);
			 buf=(int)(scn.lights[x].color[1]*255);
			 fwrite(&buf,1,1,f);
			 buf=(int)(scn.lights[x].color[2]*255);
			 fwrite(&buf,1,1,f);
			 writefloat(f,scn.lights[x].position[0],scn.floatbytes);
			 writefloat(f,scn.lights[x].position[1],scn.floatbytes);
			 writefloat(f,scn.lights[x].position[2],scn.floatbytes);
			 if (scn.lights[x].position[3]!=0)
			 {
				 writefloat(f,scn.lights[x].c_att,scn.floatbytes);
				 writefloat(f,scn.lights[x].l_att,scn.floatbytes);
				 writefloat(f,scn.lights[x].q_att,scn.floatbytes);
			 }
			 if (scn.lights[x].spot_cutoff!=180)
			 {
				 writefloat(f,scn.lights[x].spot_direction[0],scn.floatbytes);
				 writefloat(f,scn.lights[x].spot_direction[1],scn.floatbytes);
				 writefloat(f,scn.lights[x].spot_direction[2],scn.floatbytes);
				 writefloat(f,scn.lights[x].spot_exponent,scn.floatbytes);
				 writefloat(f,scn.lights[x].spot_cutoff,scn.floatbytes);
			 }


		 }

	 saveobjects(scn,f);
 }
 fclose(f);
}

int countcameraanims(cameraanimation *list)
{
	int i=0;
	for (cameraanimation *l=list;l;l=l->next) i++;
	return i;
}

void savecameras(scene &scn, FILE *f, byte floatbytes)
{
	byte texnum=countcameraanims(scn.cameraanims);
	fwrite(&texnum,1,1,f);
	for (cameraanimation *l=scn.cameraanims;l;l=l->next)
	{
		sortkeyframes(l);
		texnum=l->number;
		fwrite(&texnum,1,1,f);
		texnum=l->keyframenum;
		fwrite(&texnum,1,1,f);
		if (l->keyframenum)
		{
			keyframe ca=l->keyframes[0];
			camerakeyframe c=*(camerakeyframe*)ca.data;

			unsigned short framenum=ca.frame;
			fwrite(&framenum,2,1,f);

			texnum=(byte)(c.fov);
			fwrite(&texnum,1,1,f);

			short sh=(short)c.roll;
			fwrite(&sh,2,1,f);

			float fl=c.eye.b.x;
			writefloat(f,fl,floatbytes);
			fl=c.eye.b.y;
			writefloat(f,fl,floatbytes);
			fl=c.eye.b.z;
			writefloat(f,fl,floatbytes);

			fl=c.target.b.x;
			writefloat(f,fl,floatbytes);
			fl=c.target.b.y;
			writefloat(f,fl,floatbytes);
			fl=c.target.b.z;
			writefloat(f,fl,floatbytes);

			for (int x=1; x<l->keyframenum; x++)
			{
				keyframe ca=l->keyframes[x];
				keyframe co=l->keyframes[x-1];
				camerakeyframe c=*(camerakeyframe*)ca.data;
				camerakeyframe o=*(camerakeyframe*)co.data;
				camfield cf;
				memset(&cf,0,sizeof(cf));
				if ((byte)c.fov!=(byte)o.fov) cf.fovwritten=true;
				if ((short)c.roll!=(short)o.roll) cf.rollwritten=true;
				if (c.eye.b.x!=o.eye.b.x) cf.eyexwritten=true;
				if (c.eye.b.y!=o.eye.b.y) cf.eyeywritten=true;
				if (c.eye.b.z!=o.eye.b.z) cf.eyezwritten=true;
				if (c.target.b.x!=o.target.b.x) cf.targetxwritten=true;
				if (c.target.b.y!=o.target.b.y) cf.targetywritten=true;
				if (c.target.b.z!=o.target.b.z) cf.targetzwritten=true;

				unsigned short framenum=ca.frame;
				fwrite(&framenum,2,1,f);

				fwrite(&cf,1,1,f);

				texnum=(byte)(c.fov);
				if (cf.fovwritten) fwrite(&texnum,1,1,f);

				short sh=(short)c.roll;
				if (cf.rollwritten) fwrite(&sh,2,1,f);

				float fl=c.eye.b.x;
				if (cf.eyexwritten) writefloat(f,fl,floatbytes);
				fl=c.eye.b.y;
				if (cf.eyeywritten) writefloat(f,fl,floatbytes);
				fl=c.eye.b.z;
				if (cf.eyezwritten) writefloat(f,fl,floatbytes);

				fl=c.target.b.x;
				if (cf.targetxwritten) writefloat(f,fl,floatbytes);
				fl=c.target.b.y;
				if (cf.targetywritten) writefloat(f,fl,floatbytes);
				fl=c.target.b.z;
				if (cf.targetzwritten) writefloat(f,fl,floatbytes);
			}
		}
	}
}

int countanims(sceneobjanim *list)
{
	int i=0;
	for (sceneobjanim *l=list;l;l=l->next) i++;
	return i;
}

void saveanims(scene &scn, FILE *f, byte floatbytes)
{
	byte texnum=countanims(scn.anims);
	fwrite(&texnum,1,1,f);
	for (sceneobjanim *an=scn.anims;an;an=an->next)
	{
		byte texnum=an->number;
		fwrite(&texnum,1,1,f);
		for (object *o=scn.objects;o;o=o->next)	if (!o->effect && !o->track)
		{
			objectanimation *oa=findobjanim(o,an->number);
			sortkeyframes(oa);
			byte texnum=oa->keyframenum;
			fwrite(&texnum,1,1,f);
			if (oa->keyframenum)
			{
				keyframe ca=oa->keyframes[0];
				objkeyframedata c=*(objkeyframedata*)ca.data;

				unsigned short framenum=ca.frame;
				fwrite(&framenum,sizeof(framenum),1,f);
				
				float fl=c.position.b.x;
				writefloat(f,fl,floatbytes);
				fl=c.position.b.y;
				writefloat(f,fl,floatbytes);
				fl=c.position.b.z;
				writefloat(f,fl,floatbytes);

				fl=c.rotaxis.b.x;
				writefloat(f,fl,floatbytes);
				fl=c.rotaxis.b.y;
				writefloat(f,fl,floatbytes);
				fl=c.rotaxis.b.z;
				writefloat(f,fl,floatbytes);

				short ra=c.rotangle;
				fwrite(&ra,sizeof(short),1,f);

				fl=c.stretch.b.x;
				writefloat(f,fl,floatbytes);
				fl=c.stretch.b.y;
				writefloat(f,fl,floatbytes);
				fl=c.stretch.b.z;
				writefloat(f,fl,floatbytes);

				byte b=(byte)(c.color[0]);
				fwrite(&b,1,1,f);
				b=(byte)(c.color[1]);
				fwrite(&b,1,1,f);
				b=(byte)(c.color[2]);
				fwrite(&b,1,1,f);
				b=(byte)(c.color[3]);
				fwrite(&b,1,1,f);

				for (int a=1; a<oa->keyframenum; a++)
				{
					keyframe ca=oa->keyframes[a];
					keyframe co=oa->keyframes[a-1];
					objkeyframedata c=*(objkeyframedata*)ca.data;
					objkeyframedata o=*(objkeyframedata*)co.data;
					objfield of;
					memset(&of,0,sizeof(of));

					of.posx=c.position.b.x!=o.position.b.x;
					of.posy=c.position.b.y!=o.position.b.y;
					of.posz=c.position.b.z!=o.position.b.z;
					of.rotx=c.rotaxis.b.x!=o.rotaxis.b.x;
					of.roty=c.rotaxis.b.y!=o.rotaxis.b.y;
					of.rotz=c.rotaxis.b.z!=o.rotaxis.b.z;
					of.rota=c.rotangle!=o.rotangle;
					of.strx=c.stretch.b.x!=o.stretch.b.x;
					of.stry=c.stretch.b.y!=o.stretch.b.y;
					of.strz=c.stretch.b.z!=o.stretch.b.z;
					of.colr=(byte)(c.color[0])!=(byte)(o.color[0]);
					of.colg=(byte)(c.color[1])!=(byte)(o.color[1]);
					of.colb=(byte)(c.color[2])!=(byte)(o.color[2]);
					of.cola=(byte)(c.color[3])!=(byte)(o.color[3]);

					unsigned short framenum=ca.frame;
					fwrite(&framenum,sizeof(framenum),1,f);

					fwrite(&of,2,1,f);
					
					float fl=c.position.b.x;
					if (of.posx) writefloat(f,fl,floatbytes);
					fl=c.position.b.y;
					if (of.posy) writefloat(f,fl,floatbytes);
					fl=c.position.b.z;
					if (of.posz) writefloat(f,fl,floatbytes);

					fl=c.rotaxis.b.x;
					if (of.rotx) writefloat(f,fl,floatbytes);
					fl=c.rotaxis.b.y;
					if (of.roty) writefloat(f,fl,floatbytes);
					fl=c.rotaxis.b.z;
					if (of.rotz) writefloat(f,fl,floatbytes);

					short ra=c.rotangle;
					if (of.rota) fwrite(&ra,sizeof(short),1,f);

					fl=c.stretch.b.x;
					if (of.strx) writefloat(f,fl,floatbytes);
					fl=c.stretch.b.y;
					if (of.stry) writefloat(f,fl,floatbytes);
					fl=c.stretch.b.z;
					if (of.strz) writefloat(f,fl,floatbytes);

					byte b=(byte)(c.color[0]);
					if (of.colr) fwrite(&b,1,1,f);
					b=(byte)(c.color[1]);
					if (of.colg) fwrite(&b,1,1,f);
					b=(byte)(c.color[2]);
					if (of.colb) fwrite(&b,1,1,f);
					b=(byte)(c.color[3]);
					if (of.cola) fwrite(&b,1,1,f);
				}
			}			
		}
	}
}



void saveevents(byte floatbytes, FILE *f)
{
	sortevents();
	short i=0;
	event *e;
	for (e=eventlist;e;e=e->next) i++;
	fwrite(&i,2,1,f);
	for (e=eventlist;e;e=e->next)
	{
		byte b=e->eventtype;
		fwrite(&b,1,1,f);
		short s=e->startframe;
		fwrite(&s,2,1,f);
		s=e->endframe;
		fwrite(&s,2,1,f);
		b=e->pass;
		fwrite(&b,1,1,f);
		if (e->eventtype==layer2d || 
			e->eventtype==layer3d || 
			e->eventtype==rendertotext || 
			e->eventtype==feedback || 
			e->eventtype==grideffect)
		{
			s=e->startrectx1; fwrite(&s,2,1,f);
			s=e->startrecty1; fwrite(&s,2,1,f);
			s=e->startrectx2; fwrite(&s,2,1,f);
			s=e->startrecty2; fwrite(&s,2,1,f);

			s=e->endrectx1; fwrite(&s,2,1,f);
			s=e->endrecty1; fwrite(&s,2,1,f);
			s=e->endrectx2; fwrite(&s,2,1,f);
			s=e->endrecty2; fwrite(&s,2,1,f);
		}
		if (e->eventtype==layer2d || 
			e->eventtype==feedback || 
			e->eventtype==grideffect)
		{
			b=(byte)(e->startcol[0]*255.0); fwrite(&b,1,1,f);
			b=(byte)(e->startcol[1]*255.0); fwrite(&b,1,1,f);
			b=(byte)(e->startcol[2]*255.0); fwrite(&b,1,1,f);
			b=(byte)(e->startcol[3]*255.0); fwrite(&b,1,1,f);

			b=(byte)(e->endcol[0]*255.0); fwrite(&b,1,1,f);
			b=(byte)(e->endcol[1]*255.0); fwrite(&b,1,1,f);
			b=(byte)(e->endcol[2]*255.0); fwrite(&b,1,1,f);
			b=(byte)(e->endcol[3]*255.0); fwrite(&b,1,1,f);

			switch (e->blendfunc1)
			{
			case GL_ZERO				: b=0; break;
			case GL_ONE					: b=1; break;
			case GL_SRC_COLOR			: b=2; break;
			case GL_ONE_MINUS_SRC_COLOR	: b=3; break;
			case GL_SRC_ALPHA			: b=4; break;
			case GL_ONE_MINUS_SRC_ALPHA	: b=5; break;
			case GL_DST_ALPHA			: b=6; break;
			case GL_ONE_MINUS_DST_ALPHA	: b=7; break;
			case GL_DST_COLOR			: b=8; break;
			case GL_ONE_MINUS_DST_COLOR	: b=9; break;
			case GL_SRC_ALPHA_SATURATE	: b=10; break;
			}
			fwrite(&b,1,1,f);

			switch (e->blendfunc2)
			{
			case GL_ZERO				: b=0; break;
			case GL_ONE					: b=1; break;
			case GL_SRC_COLOR			: b=2; break;
			case GL_ONE_MINUS_SRC_COLOR	: b=3; break;
			case GL_SRC_ALPHA			: b=4; break;
			case GL_ONE_MINUS_SRC_ALPHA	: b=5; break;
			case GL_DST_ALPHA			: b=6; break;
			case GL_ONE_MINUS_DST_ALPHA	: b=7; break;
			case GL_DST_COLOR			: b=8; break;
			case GL_ONE_MINUS_DST_COLOR	: b=9; break;
			case GL_SRC_ALPHA_SATURATE	: b=10; break;
			}
			fwrite(&b,1,1,f);
		}

		switch (e->eventtype)
		{
		case layer2d:
			{
				b=e->textured;
				fwrite(&b,1,1,f);
				b=e->texture;
				fwrite(&b,1,1,f);
				break;
			}
		case layer3d:
			{
				b=e->sceneid; fwrite(&b,1,1,f);
				b=e->cameraid; fwrite(&b,1,1,f);
				b=e->animid; fwrite(&b,1,1,f);
				s=(int)(e->camerastart*(float)e->icam->length); fwrite(&s,2,1,f);
				s=(int)(e->cameraend*(float)e->icam->length); fwrite(&s,2,1,f);
				s=(int)(e->animstart*(float)e->ianim->length); fwrite(&s,2,1,f);
				s=(int)(e->animend*(float)e->ianim->length); fwrite(&s,2,1,f);
				break;
			}
		case cleargl:
			{
				b=e->clearscreen;
				fwrite(&b,1,1,f);
				b=e->clearzbuffer;
				fwrite(&b,1,1,f);
				break;
			}
		case rendertotext:
			{
				b=e->texture;
				fwrite(&b,1,1,f);
				break;
			}
		case feedback:
			{
				b=e->texture; fwrite(&b,1,1,f);
				b=e->param1; fwrite(&b,1,1,f);
				writefloat(f,e->param4,floatbytes);
				break;
			}
		case grideffect:
			{
				b=e->texture; fwrite(&b,1,1,f);
				b=e->effect; fwrite(&b,1,1,f);
				writefloat(f,e->animstart,floatbytes);
				writefloat(f,e->animend,floatbytes);
				break;
			}
		}
	}
}

int countscenes()
{
	int i=0;
	for(scene *s=scenelist;s;s=s->next) i++;
	return i;
}

void saveminimalproject(const char *filename, byte floatbytes)
{
	FILE *f=fopen(filename,"w+b");
	if (f==NULL) return;
	byte texnum;
	
	texnum=floatbytes;
	fwrite(&texnum,1,1,f);
	savetextures(f);

	texnum=countscenes();
	fwrite(&texnum,1,1,f);

	for (scene *s=scenelist; s; s=s->next)
	{
		texnum=s->number;
		fwrite(&texnum,1,1,f);

		s->floatbytes=floatbytes;
		if (s->fog)
		{
			texnum=1;
			fwrite(&texnum,1,1,f);
			texnum=(int)(s->fogcol[0]*255);
			fwrite(&texnum,1,1,f);
			texnum=(int)(s->fogcol[1]*255);
			fwrite(&texnum,1,1,f);
			texnum=(int)(s->fogcol[2]*255);
			fwrite(&texnum,1,1,f);
			writefloat(f,s->fogdensity,s->floatbytes);
		}
		else
		{
			texnum=0;
			fwrite(&texnum,1,1,f);
		}
		texnum=0;
		for (int x=0; x<8; x++) if (s->lights[x].turnedon) texnum++;
		fwrite(&texnum,1,1,f); //light number
		byte buf;
		for (x=0; x<8; x++)
			if (s->lights[x].turnedon)
			{
				buf=x;
				fwrite(&buf,1,1,f); //light id
				buf=1; //dotlight
				if (s->lights[x].spot_cutoff!=180) buf=2;//spotlight
				if (s->lights[x].position[3]==0) buf=0; //dir light
				fwrite(&buf,1,1,f); //light type

				buf=(int)(s->lights[x].ambient[0]*255);
				fwrite(&buf,1,1,f);
				buf=(int)(s->lights[x].ambient[1]*255);
				fwrite(&buf,1,1,f);
				buf=(int)(s->lights[x].ambient[2]*255);
				fwrite(&buf,1,1,f);
				buf=(int)(s->lights[x].color[0]*255);
				fwrite(&buf,1,1,f);
				buf=(int)(s->lights[x].color[1]*255);
				fwrite(&buf,1,1,f);
				buf=(int)(s->lights[x].color[2]*255);
				fwrite(&buf,1,1,f);
				writefloat(f,s->lights[x].position[0],s->floatbytes);
				writefloat(f,s->lights[x].position[1],s->floatbytes);
				writefloat(f,s->lights[x].position[2],s->floatbytes);
				if (s->lights[x].position[3]!=0)
				{
					writefloat(f,s->lights[x].c_att,s->floatbytes);
					writefloat(f,s->lights[x].l_att,s->floatbytes);
					writefloat(f,s->lights[x].q_att,s->floatbytes);
				}
				if (s->lights[x].spot_cutoff!=180)
				{
					writefloat(f,s->lights[x].spot_direction[0],s->floatbytes);
					writefloat(f,s->lights[x].spot_direction[1],s->floatbytes);
					writefloat(f,s->lights[x].spot_direction[2],s->floatbytes);
					writefloat(f,s->lights[x].spot_exponent,s->floatbytes);
					writefloat(f,s->lights[x].spot_cutoff,s->floatbytes);
				}


			}
		saveobjects(*s,f);
		savecameras(*s,f,floatbytes);
		saveanims(*s,f,floatbytes);
	}
	saveevents(floatbytes,f);
	fclose(f);
}

int ffwrite(const void *data, size_t a, size_t b, FILE **ff, bool write)
{
	if (write) fwrite(data,a,b,*ff);
	return (a*b);
}

int WriteTextureChunkData(bool write, FILE **f, tTexture *tex)
{
	int counter=0;

	byte b=tex->number;
	counter+=ffwrite(&b,1,1,f,write);
	b=tex->commandnum;
	counter+=ffwrite(&b,1,1,f,write);
	for (x=0;x<b;x++)
	{
	 counter+=ffwrite(&(tex->commands[x]),1,9,f,write);
	 if (tex->commands[x].c.commandnumber==DD_text)
	 {
		 byte bb=strlen(tex->texts[tex->commands[x].command[8]].text);
		 counter+=ffwrite(&bb,1,1,f,write);
		 counter+=ffwrite(tex->texts[tex->commands[x].command[8]].text,bb,1,f,write);
	 }
	}
	counter+=ffwrite(tex->name,20,1,f,write);
	
	return counter;
}

void SaveTextureChunks(FILE **ff)
{
	FILE *f=*ff;
	tTexture *t=texturelist;
	chunk Chunk;
	while (t!=NULL)
	{
		Chunk.ID=aDDict_Texture;
		Chunk.size=WriteTextureChunkData(false,&f,t);
		fwrite(&Chunk,sizeof(Chunk),1,f);
		WriteTextureChunkData(true,&f,t);
		t=t->next;
	}
}

int WriteMaterialChunkData(bool write, FILE **f, material *mat)
{
	int counter=0;
	counter+=ffwrite(mat->name,20,1,f,write);

	byte b=mat->texture;
	counter+=ffwrite(&b,1,1,f,write);
	b=mat->layer;
	counter+=ffwrite(&b,1,1,f,write);
	b=mat->alpha;
	counter+=ffwrite(&b,1,1,f,write);
	b=mat->alphalayer;
	counter+=ffwrite(&b,1,1,f,write);
	b=mat->alphamode;
	counter+=ffwrite(&b,1,1,f,write);
	b=mat->number;
	counter+=ffwrite(&b,1,1,f,write);

	return counter;
}

void SaveMaterialChunks(FILE **ff)
{
	FILE *f=*ff;
	material *m=materiallist;
	chunk Chunk;
	while (m!=NULL)
	{
		Chunk.ID=aDDict_Material;
		Chunk.size=WriteMaterialChunkData(false,&f,m);
		fwrite(&Chunk,sizeof(Chunk),1,f);
		WriteMaterialChunkData(true,&f,m);
		m=m->next;
	}
}

int WriteSceneSelectionChunkData(bool write, FILE **f, selectionlist *se)
{
	int counter=0;
	short int objnumber;

	counter+=ffwrite(se->name,20,1,f,write);
	objnumber=selsize(se->selection);
	counter+=ffwrite(&objnumber,sizeof(objnumber),1,f,write);
	SELECTION *sel2=se->selection;
	while (sel2!=NULL)
	{
	 objnumber=sel2->selected;
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,f,write);
	 sel2=sel2->next;
	}
	
	return counter;
}

int SaveSceneSelectionChunks(FILE **ff, scene *s, bool write)
{
	int counter =0;
	FILE *f=*ff;
	chunk Chunk;
	selectionlist *se=s->selections;
	while (se!=NULL)
	{
		Chunk.ID=aDDict_Selection;
		Chunk.size=WriteSceneSelectionChunkData(false,ff,se);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneSelectionChunkData(write,ff,se);
		se=se->next;
	}
	return counter;
}

int SaveSceneLightChunks(FILE **ff, scene *s, bool write)
{
	int counter =0;

	FILE *f=*ff;
	chunk Chunk;
	Chunk.ID=aDDict_Lights;
	Chunk.size=ffwrite(s->lights,sizeof(light),8,ff,false);
	counter+=Chunk.size;
	counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
	counter+=ffwrite(s->lights,sizeof(light),8,ff,write);
	
	return counter;
}

int SaveSceneFogChunk(FILE **ff, scene *s, bool write)
{
	int counter =0;

	if (s->fog)
	{
		chunk Chunk;
		Chunk.ID=aDDict_Fog;
		Chunk.size=sizeof(float)*8;
		
		ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		counter+=ffwrite(s->fogcol,sizeof(float),4,ff,write);
		counter+=ffwrite(&s->fogmode,sizeof(float),1,ff,write);
		counter+=ffwrite(&s->fogstart,sizeof(float),1,ff,write);
		counter+=ffwrite(&s->fogend,sizeof(float),1,ff,write);
		counter+=ffwrite(&s->fogdensity,sizeof(float),1,ff,write);
	}

	return counter;
}

int WriteSceneObjectChunkData(bool write, FILE **ff, object *o)
{
	int counter=0;
	short int objnumber;
	unsigned char texnum;
	float s;
	unsigned long int w;

	char cc[40];
	memset(cc,0,40);
	
	texnum=strlen(o->name);
	memcpy(cc,o->name,texnum);
	counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	counter+=ffwrite(cc,40,1,ff,write);
	texnum=o->primitive;
	counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	switch (o->primitive)
	{
	case dd3d_grid:
	case dd3d_hasab:
	case dd3d_cone:
	case dd3d_sphere: {
	 texnum=o->params[1];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 texnum=o->params[2];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 break;
				   }
	case dd3d_arc: {
	 texnum=o->params[1];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 objnumber=o->params[2];
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
	 break;
				}
	case dd3d_line: {
	 texnum=o->params[1];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 break;
				 }
	case dd3d_loft: {
	 objnumber=o->params[1];
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
	 objnumber=o->params[2];
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
	 break;
				 }
	case dd3d_map:{
	 counter+=ffwrite(&o->objdata,sizeof(o->objdata),1,ff,write);
	 texnum=o->objdata.material2;
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 switch (texnum)
	 {
	 case 0:
		 {
			 vector3 xpse;
			 m_revxpose(o->xformmatrix,xpse);
			 counter+=ffwrite(&xpse,sizeof(vector3),1,ff,write);
			 break;
		 }
	 case 1:
		 {
			 vector3 xpse;
			 m_revscale(o->xformmatrix,xpse);
			 counter+=ffwrite(&xpse,sizeof(vector3),1,ff,write);
			 break;
		 }
	 case 2:
		 {
			 vector3 xpse;
			 float phi;
			 m_revrotate(o->xformmatrix,xpse,phi);
			 counter+=ffwrite(&xpse,sizeof(vector3),1,ff,write);
			 counter+=ffwrite(&phi,sizeof(phi),1,ff,write);
			 break;
		 }
	 }
			   }
	case dd3d_blur:
	case dd3d_linearsubdivision:
	case dd3d_butterfly: {
	 texnum=o->params[1];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 SELECTION *s=(SELECTION*)o->params[2];
	 objnumber=selsize(s);
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
	 while (s!=NULL)
	 {
		 objnumber=s->selected;
		 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
		 s=s->next;
	 }
	 break;
					  }

	case dd3d_boolean: {
	 texnum=o->params[1];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 objnumber=o->params[2];
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
	 objnumber=o->params[3];
	 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
	 for (x=0;x<=2;x++)
		 for (y=0;y<=3;y++)
		 {
			 s=o->xformmatrix[x][y];
			 w=*(unsigned long*)(&s);
			 w=(w & masks[4]) >> ((4-4)*8);
			 counter+=ffwrite(&w,4,1,ff,write);
		 }
	 break;
					}
	}
	switch (o->primitive)
	{
	case dd3d_hasab: {
	 texnum=o->params[3];
	 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
	 break;								  
				  }
	}

	switch (o->primitive)
	{
	case dd3d_box:
	case dd3d_sphere:
	case dd3d_hasab:
	case dd3d_dodecaeder:
	case dd3d_icosaeder:
	case dd3d_cone:
	case dd3d_arc:
	case dd3d_loft:
	case dd3d_line:
	case dd3d_grid:
	case dd3d_freetrack:
	case dd3d_freeobj:
	case dd3d_clone:{

	 for (x=0;x<=2;x++)
		 for (y=0;y<=3;y++)
		 {
			 s=o->xformmatrix[x][y];
			 w=*(unsigned long*)(&s);
			 w=(w & masks[4]) >> ((4-4)*8);
			 counter+=ffwrite(&w,4,1,ff,write);
		 }
		 counter+=ffwrite(&o->objdata,sizeof(tobjdata),1,ff,write);
		 texnum=byte(o->selected);
		 counter+=ffwrite(&texnum,sizeof(texnum),1,ff,write);
		 if (o->primitive==dd3d_clone)
		 {
			 SELECTION *s=(SELECTION*)o->params[1];
			 objnumber=selsize(s);
			 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
			 while (s!=NULL)
			 {
				 objnumber=s->selected;
				 counter+=ffwrite(&objnumber,sizeof(objnumber),1,ff,write);
				 s=s->next;
			 }
		 }
		 break;
				 }
	}

	return counter;
}

int WriteSceneObjectChunkData2(bool write, FILE **ff, object *o)
{
	int counter=0;

	counter+=ffwrite(&o->parent,sizeof(o->parent),10,ff,write);

	return counter;
}

int SaveSceneObjectChunks(FILE **ff, scene *s, bool write)
{
	int counter =0;
	FILE *f=*ff;
	chunk Chunk;
	object *o=s->objects;
	while (o!=NULL)
	{
		Chunk.ID=aDDict_Object;
		Chunk.size=WriteSceneObjectChunkData(false,ff,o);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneObjectChunkData(write,ff,o);

		Chunk.ID=aDDict_Object2;
		Chunk.size=WriteSceneObjectChunkData2(false,ff,o);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneObjectChunkData2(write,ff,o);

		o=o->next;
	}
	return counter;
}

int WriteSceneCameraAnimKeyChunkData(bool write, FILE **ff, keyframe k)
{
	int counter=0;
	camerakeyframe ckey=*(camerakeyframe*)k.data;
	counter+=ffwrite(&k.frame,4,1,ff,write);
	counter+=ffwrite(&ckey.eye,sizeof(ckey.eye),1,ff,write);
	counter+=ffwrite(&ckey.target,sizeof(ckey.target),1,ff,write);
	counter+=ffwrite(&ckey.fov,sizeof(ckey.fov),1,ff,write);
	counter+=ffwrite(&ckey.roll,sizeof(ckey.roll),1,ff,write);
	return counter;
}

int WriteSceneCameraAnimChunkData(bool write, FILE **ff, cameraanimation *c)
{
	int counter=0;
	FILE *f=*ff;
	chunk Chunk;
	/*keyframe *k=c->keyframes;
	while (c!=NULL)
	{*/
	counter+=ffwrite(c->name,40,1,ff,write);
	counter+=ffwrite(&c->length,4,1,ff,write);
	counter+=ffwrite(&c->number,4,1,ff,write);
	for (int x=0; x<c->keyframenum; x++)
	{
		Chunk.ID=aDDict_CameraKey;
		Chunk.size=WriteSceneCameraAnimKeyChunkData(false,ff,c->keyframes[x]);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneCameraAnimKeyChunkData(write,ff,c->keyframes[x]);
	}
	return counter;
}

int SaveSceneCameraAnimChunks(FILE **ff, scene *s, bool write)
{
	int counter =0;
	FILE *f=*ff;
	chunk Chunk;
	cameraanimation *c=s->cameraanims;
	while (c!=NULL)
	{
		Chunk.ID=aDDict_CameraAni;
		Chunk.size=48;//WriteSceneCameraAnimChunkData(false,ff,c);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneCameraAnimChunkData(write,ff,c);
		c=c->next;
	}
	return counter;
}

int WriteSceneAnimKeyChunkData(bool write, FILE **ff, keyframe k, unsigned short &objid)
{
	int counter=0;
	objkeyframedata ckey=*(objkeyframedata*)k.data;
	counter+=ffwrite(&k.frame,4,1,ff,write);
	counter+=ffwrite(&objid,sizeof(objid),1,ff,write);

	counter+=ffwrite(&ckey.position,sizeof(ckey.position),1,ff,write);
	counter+=ffwrite(&ckey.rotaxis,sizeof(ckey.rotaxis),1,ff,write);
	counter+=ffwrite(&ckey.stretch,sizeof(ckey.stretch),1,ff,write);
	counter+=ffwrite(&ckey.rotaxis,sizeof(ckey.rotaxis),1,ff,write);
	counter+=ffwrite(ckey.color,sizeof(float),4,ff,write);

	return counter;
}

int WriteSceneAnimChunkData(bool write, FILE **ff, scene *s, sceneobjanim *c)
{
	int counter=0;
	FILE *f=*ff;
	chunk Chunk;
	/*keyframe *k=c->keyframes;
	while (c!=NULL)
	{*/
	counter+=ffwrite(c->name,40,1,ff,write);
	counter+=ffwrite(&c->length,4,1,ff,write);
	counter+=ffwrite(&c->number,4,1,ff,write);

	/*for (int x=0; x<c->keyframenum; x++)
	{
		Chunk.ID=aDDict_AnimKey;
		Chunk.size=WriteSceneCameraAnimKeyChunkData(false,ff,c->keyframes[x]);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneAnimKeyChunkData(write,ff,c->keyframes[x]);
	}*/

	for (object *o=s->objects;o;o=o->next)
	{
		objectanimation *ck=findobjanim(o,c->number);
		for (int x=0; x<ck->keyframenum; x++)
		{
			Chunk.ID=aDDict_AnimKey;
			Chunk.size=WriteSceneAnimKeyChunkData(false,ff,ck->keyframes[x],o->objdata.number);
			counter+=Chunk.size;
			counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
			WriteSceneAnimKeyChunkData(write,ff,ck->keyframes[x],o->objdata.number);
		}
	}


	return counter;
}

int SaveSceneAnimChunks(FILE **ff, scene *s, bool write)
{
	int counter =0;
	FILE *f=*ff;
	chunk Chunk;
	sceneobjanim *c=s->anims;
	while (c!=NULL)
	{
		Chunk.ID=aDDict_Anim;
		Chunk.size=48;//WriteSceneCameraAnimChunkData(false,ff,c);
		counter+=Chunk.size;
		counter+=ffwrite(&Chunk,sizeof(Chunk),1,ff,write);
		WriteSceneAnimChunkData(write,ff,s,c);
		c=c->next;
	}
	return counter;
}


int WriteSceneChunkData(bool write, FILE **f, scene *s)
{
	int counter=0;

	// ide jon majd meg a global scene data
	counter+=ffwrite(s->name,40,1,f,write);
	counter+=ffwrite(&s->number,sizeof(int),1,f,write);
	counter+=ffwrite(&s->floatbytes,1,1,f,write);
	counter+=ffwrite(&s->editview,sizeof(camera),1,f,write);

	// global scene data vege

	counter+=SaveSceneFogChunk(f,s,false);
	SaveSceneFogChunk(f,s,write);
	
	counter+=SaveSceneSelectionChunks(f,s,false);
	SaveSceneSelectionChunks(f,s,write);
	
	counter+=SaveSceneLightChunks(f,s,false);
	SaveSceneLightChunks(f,s,write);

	counter+=SaveSceneObjectChunks(f,s,false);
	SaveSceneObjectChunks(f,s,write);

	counter+=SaveSceneCameraAnimChunks(f,s,false);
	SaveSceneCameraAnimChunks(f,s,write);

	counter+=SaveSceneAnimChunks(f,s,false);
	SaveSceneAnimChunks(f,s,write);

	return counter;
}

void SaveSceneChunks(FILE **ff)
{
	FILE *f=*ff;
	scene *s=scenelist;
	chunk Chunk;
	while (s!=NULL)
	{
		s->floatbytes=4;
		Chunk.ID=aDDict_Scene;
		Chunk.size=45+sizeof(camera);//WriteSceneChunkData(false,&f,s);
		fwrite(&Chunk,sizeof(Chunk),1,f);
		WriteSceneChunkData(true,&f,s);
		s=s->next;
	}
}

int WriteEventChunkData(bool write, FILE **f, event *s)
{
	int counter=0;

	counter+=ffwrite(s->name,40,1,f,write);
	counter+=ffwrite(&s->startframe,sizeof(int),1,f,write);
	counter+=ffwrite(&s->endframe,sizeof(int),1,f,write);
	counter+=ffwrite(&s->pass,sizeof(int),1,f,write);
	counter+=ffwrite(&s->eventtype,sizeof(int),1,f,write);
	counter+=ffwrite(&s->startrectx,sizeof(float),1,f,write);
	counter+=ffwrite(&s->startrecty,sizeof(float),1,f,write);
	counter+=ffwrite(&s->startrectxrez,sizeof(float),1,f,write);
	counter+=ffwrite(&s->startrectyrez,sizeof(float),1,f,write);
	counter+=ffwrite(&s->endrectx,sizeof(float),1,f,write);
	counter+=ffwrite(&s->endrecty,sizeof(float),1,f,write);
	counter+=ffwrite(&s->endrectxrez,sizeof(float),1,f,write);
	counter+=ffwrite(&s->endrectyrez,sizeof(float),1,f,write);

	counter+=ffwrite(&s->blendfunc1,sizeof(GLuint),1,f,write);
	counter+=ffwrite(&s->blendfunc2,sizeof(GLuint),1,f,write);

	counter+=ffwrite(s->startcol,sizeof(float),4,f,write);
	counter+=ffwrite(s->endcol,sizeof(float),4,f,write);

	counter+=ffwrite(&s->textured,1,1,f,write);
	counter+=ffwrite(&s->texxtile,sizeof(int),1,f,write);
	counter+=ffwrite(&s->texytile,sizeof(int),1,f,write);
	counter+=ffwrite(&s->sceneid,sizeof(int),1,f,write);
	counter+=ffwrite(&s->animid,sizeof(int),1,f,write);
	counter+=ffwrite(&s->cameraid,sizeof(int),1,f,write);

	counter+=ffwrite(&s->camerastart,sizeof(float),1,f,write);
	counter+=ffwrite(&s->cameraend,sizeof(float),1,f,write);
	counter+=ffwrite(&s->animstart,sizeof(float),1,f,write);
	counter+=ffwrite(&s->animend,sizeof(float),1,f,write);

	counter+=ffwrite(&s->texture,sizeof(int),1,f,write);

	counter+=ffwrite(&s->clearzbuffer,1,1,f,write);
	counter+=ffwrite(&s->clearscreen,1,1,f,write);

	return counter;
}

int WriteEventChunkData2(bool write, FILE **f, event *s)
{
	int counter=0;
	counter+=ffwrite(&s->startrectx1,sizeof(int),1,f,write);
	counter+=ffwrite(&s->startrecty1,sizeof(int),1,f,write);
	counter+=ffwrite(&s->startrectx2,sizeof(int),1,f,write);
	counter+=ffwrite(&s->startrecty2,sizeof(int),1,f,write);
	counter+=ffwrite(&s->endrectx1,sizeof(int),1,f,write);
	counter+=ffwrite(&s->endrecty1,sizeof(int),1,f,write);
	counter+=ffwrite(&s->endrectx2,sizeof(int),1,f,write);
	counter+=ffwrite(&s->endrecty2,sizeof(int),1,f,write);
	counter+=ffwrite(&s->effect,sizeof(int),1,f,write);
	counter+=ffwrite(&s->param1,sizeof(int),1,f,write);
	counter+=ffwrite(&s->param2,sizeof(int),1,f,write);
	counter+=ffwrite(&s->param3,sizeof(int),1,f,write);
	counter+=ffwrite(&s->param4,sizeof(float),1,f,write);
	counter+=ffwrite(&s->param5,sizeof(float),1,f,write);
	counter+=ffwrite(&s->param6,sizeof(float),1,f,write);
	return counter;
}

void SaveEventChunks(FILE **ff)
{
	FILE *f=*ff;
	event *s=eventlist;
	chunk Chunk;
	while (s!=NULL)
	{
		Chunk.ID=aDDict_Event;
		Chunk.size=WriteEventChunkData(false,&f,s);
		fwrite(&Chunk,sizeof(Chunk),1,f);
		WriteEventChunkData(true,&f,s);
		
		Chunk.ID=aDDict_Event2;
		Chunk.size=WriteEventChunkData2(false,&f,s);
		fwrite(&Chunk,sizeof(Chunk),1,f);
		WriteEventChunkData2(true,&f,s);

		s=s->next;
	}
}


void SaveProject(const char *filename)
{
	masks[0]=0x00000000;
	masks[1]=0xff000000;
	masks[2]=0xffff0000;
	masks[3]=0xffffff00;
	masks[4]=0xffffffff;
	FILE *f;

	f=fopen(filename,"w+b");
	if (f==NULL) { MessageBox( 0, "Baze narancslé", "mit csúszkálsz?", MB_OK ); return;}
	else
	{
		fwrite(fileheader,8,1,f);
		//write textures
		SaveTextureChunks(&f);
		//write materials
		SaveMaterialChunks(&f);
		//write scenes
		SaveSceneChunks(&f);
		//write intro data
		SaveEventChunks(&f);

	}

	chunk Chunk;
	Chunk.ID=aDDict_EOF;
	Chunk.size=0;
	fwrite(&Chunk,sizeof(chunk),1,f);

	fclose(f);
}

void LoadTextureChunk(FILE **ff, int txnum)
{
	FILE *f=*ff;
	tTexture *newtex = new tTexture[1];
	memset(newtex,0,sizeof(tTexture));
	byte b;
	fread(&b,1,1,f); // texture identifier
	newtex->number=b+txnum;
	fread(&b,1,1,f);
	
	newtex->commandnum=0;
	memset(newtex->commands,0,sizeof(newtex->commands));
	for (x=0;x<100;x++)
	{
		memset(newtex->texts[x].text,0,256);
	}
	memset(newtex->layers,0,sizeof(newtex->layers));
	
	for (x=0;x<b;x++)
		{
		fread(&cmd,9,1,f);
		if (cmd.c.commandnumber==DD_text) 
		{
			fread(&k,1,1,f);
			fread(newtex->texts[cmd.command[8]].text,k,1,f);
			newtex->texts[cmd.command[8]].text[k]='\0';
		}
		newtex->commands[newtex->commandnum]=cmd;
		newtex->commandnum++;
	}
	char *ss=new char[256];
	fread(ss,20,1,f);
	sprintf(newtex->name,"%s",ss);
	newtex->next=NULL;
	if (lasttexture==NULL)
	{
		texturelist=newtex;
		lasttexture=newtex;
	}
	else
	{
		lasttexture->next=newtex;
		lasttexture=newtex;
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);
	glReadBuffer(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D,background);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 1024, 1024, 0);
	glReadBuffer(GL_BACK);

	for (z=0;z<2;z++)
	{
		glClear(0x4100);
		glBegin(GL_QUADS);
		glTexCoord2f(0,600.0/1024.0);
		glVertex2f(0,0);
		glTexCoord2f(800.0/1024.0,600.0/1024.0);
		glVertex2f(800.0,0);
		glTexCoord2f(800.0/1024,0);
		glVertex2f(800.0,600.0);
		glTexCoord2f(0,0);
		glVertex2f(0,600.0);
		glEnd();
		glClear(0x100);
		SwapBuffers(hDC);
	}

	buffertexture.commandnum=0;
	memset(buffertexture.commands,0,sizeof(buffertexture.commands));
	memcpy(buffertexture.texts,lasttexture->texts,sizeof(lasttexture->texts));
	memset(buffertexture.layers,0,sizeof(buffertexture.layers));
	for (x=0;x<lasttexture->commandnum;x++)
	{
		buffertexture.performcommand(lasttexture->commands[x]);
		for (y=0;y<=2;y++)
		{
			//glClear(0x4100);
			glColor4f(buttontextlit);
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			rectangle(661,390,790,400);
			glColor4f(col4);
			//quad(662,391,(int)((float)x/(float)(lasttexture->commandnum-1)*128.0*(float)(aa)/(float)(numberoftextures-1))+662,399);
			quad(662,391,(int)(662+128.0f*(float)aa/(float)numberoftextures+128.0f/(float)numberoftextures*(float)(x+1)/(float)(lasttexture->commandnum)),399);
			glColor4f(1,1,1,1);
		}
		SwapBuffers(hDC);
	}
	aa++;
	memcpy(lasttexture->layers,buffertexture.layers,sizeof(buffertexture.layers));
	glPopAttrib();
}

tTexture *findtexture(int ID)
{
	for (tTexture *t=texturelist;t;t=t->next) if (t->number==ID) return t;
	return NULL;
}

void LoadMaterialChunk(FILE **ff, int txnum, int matnum)
{
	FILE *f=*ff;

	material *newmat = new material [1];
	memset(newmat,0,sizeof(material));
	newmat->next=NULL;
	byte b;
				
	char *ss=new char[256];
	fread(ss,20,1,f);
	sprintf(newmat->name,"%s",ss);

	fread(&b,1,1,f);
	newmat->texture=b+txnum;
	fread(&b,1,1,f);
	newmat->layer=b;
	fread(&b,1,1,f);
	newmat->alpha=b+txnum;
	fread(&b,1,1,f);
	newmat->alphalayer=b;
	fread(&b,1,1,f);
	newmat->alphamode=b;
	fread(&b,1,1,f);
	newmat->number=b+matnum;

	if (lastmaterial==NULL)
	{
		lastmaterial=newmat;
		materiallist=newmat;
	}
	else
	{
		lastmaterial->next=newmat;
		lastmaterial=newmat;
	}
	
	tTexture *pt=texturelist;
	pt=findtexture(lastmaterial->texture);//while ((pt!=NULL) && (pt->number!=lastmaterial->texture)) pt=pt->next;
	rgba layer[256][256];
	memset(layer,0,256*256*4);
	memcpy(layer,pt->layers[lastmaterial->layer],256*256*4);
	for (x=0;x<256;x++)
		for (y=0;y<256;y++)
		{
			layer[x][y].cols[3]=255;
		}
	glGenTextures(1,&lastmaterial->handle);
	glBindTexture(GL_TEXTURE_2D,lastmaterial->handle);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,layer);
}

void LoadSelectionChunk(FILE **ff, int startobjnum)
{
	FILE *f=*ff;
	short int objnumber,on;

	selectionlist *sel=new selectionlist[1];
	memset(sel,0,sizeof(selectionlist));
	sel->next=NULL;
	sel->name=new char[80];
	sel->selection=NULL;
	fread(sel->name,20,1,f);
	fread(&objnumber,sizeof(objnumber),1,f);
	for (z=1; z<=objnumber; z++)
	{
		SELECTION *sel2=new SELECTION[1];
		memset(sel2,0,sizeof(SELECTION));
		fread(&on,sizeof(objnumber),1,f);
		sel2->selected=on+startobjnum;
		sel2->next=sel->selection;
		sel->selection=sel2;
	}
	if (lastscene->selections==NULL)
	{
		lastscene->selections=sel;
		lastscene->lastselection=sel;
	}
	else
	{
		lastscene->lastselection->next=sel;
		lastscene->lastselection=sel;
	}
}

tTexture *findtext(tTexture *list,int id)
{
	for (tTexture *t=list;t;t=t->next) if (t->number==id) return t;
	return NULL;
}

void LoadObjectChunk(scene *scn,FILE **ff, int starttex, int matnum, int startobjnum)
{
	FILE *f=*ff;
	scene_addobject(scn);
	byte texnum;
	short int objnumber;
	unsigned long int w;
	float s;
	char sb[256];
	
	memset(sb,0,256);
	fread(sb,41,1,f);
	memcpy(scn->lastobject->name,&(sb[1]),256);
	fread(&scn->lastobject->primitive,1,1,f);
	//char ss[256];
	//sprintf(ss,"%d - %s",ftell(f),&Chunk.ID);
	//MessageBox( 0, sb, "HelloWorld", MB_OK );
	switch (scn->lastobject->primitive)
	{
	case dd3d_grid:
	case dd3d_hasab:
	case dd3d_cone:
	case dd3d_sphere: {
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[1]=texnum;
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[2]=texnum;
		break;
					  }
	case dd3d_arc: {
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[1]=texnum;
		fread(&objnumber,sizeof(objnumber),1,f); scn->lastobject->params[2]=objnumber;
		break;
				   }
	case dd3d_line: {
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[1]=texnum;
		break;
					}
	case dd3d_loft: {
		fread(&objnumber,sizeof(objnumber),1,f); scn->lastobject->params[1]=objnumber+startobjnum;
		fread(&objnumber,sizeof(objnumber),1,f); scn->lastobject->params[2]=objnumber+startobjnum;
		scn->lastobject->params[3]=(int)(scn->objects);
		break;
					}
	case dd3d_map:{
		fread(&scn->lastobject->objdata,sizeof(scn->lastobject->objdata),1,f);
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->objdata.material2=texnum;
		scn->lastobject->objdata.material1+=matnum;
		switch (texnum)
		{
		case 0:
			{
				vector3 xpse;
				fread(&xpse,sizeof(vector3),1,f);
				m_xpose(xpse.b.x,xpse.b.y,xpse.b.z,scn->lastobject->xformmatrix);
				break;
			}
		case 1:
			{
				vector3 xpse;
				fread(&xpse,sizeof(vector3),1,f);
				m_scale(xpse.b.x,xpse.b.y,xpse.b.z,scn->lastobject->xformmatrix);
				break;
			}
		case 2:
			{
				vector3 xpse;
				float phi;
				fread(&xpse,sizeof(vector3),1,f);
				fread(&phi,sizeof(phi),1,f);
				m_rotate(xpse.b.x,xpse.b.y,xpse.b.z,phi,scn->lastobject->xformmatrix);
				break;
			}
		}
				  }
	case dd3d_blur:
	case dd3d_linearsubdivision:
	case dd3d_butterfly: {
		scn->lastobject->objdata.number+=startobjnum;
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[1]=texnum;
		SELECTION *s=NULL,*n=NULL,*last=NULL;
		short int ab,cd,ef;
		fread(&ab,sizeof(ab),1,f);
		for (cd=1;cd<=ab;cd++)
		{
			fread(&ef,sizeof(ef),1,f);
			n=new SELECTION[1];
			n->next=NULL;
			n->selected=ef+startobjnum;
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
		scn->lastobject->params[2]=int(s);
		scn->lastobject->effect=true;
		n=s;
		scn->lastobject->cloneparents=n;
		while (n!=NULL)
		{
			for (cd=0; cd<scn->lastobject->params[1]; cd++)
			{
				switch (scn->lastobject->primitive)
				{
				case dd3d_linearsubdivision:
				case dd3d_butterfly:{
					object *a=searchobjectbynumber(scn->objects,n->selected);
					if (a!=NULL)
					{
						butterflysubdivision(a,scn->lastobject->primitive==dd3d_linearsubdivision);
						SELECTION *newsel = new SELECTION;
						newsel->selected=scn->lastobject->objdata.number;
						newsel->next=a->effects;
						a->effects=newsel;
					}
					break;
									}
				case dd3d_blur:{
					object *a=searchobjectbynumber(scn->objects,n->selected);
					if (a!=NULL)
					{
						meshblur(a);
						SELECTION *newsel = new SELECTION;
						newsel->selected=scn->lastobject->objdata.number;
						newsel->next=a->effects;
						a->effects=newsel;
					}
					break;
							   }
				case dd3d_map:{
					object *o=searchobjectbynumber(scn->objects,n->selected);
					if (o!=NULL)
					{
						SELECTION *newsel = new SELECTION;
						newsel->selected=scn->lastobject->objdata.number;
						newsel->next=o->effects;
						o->effects=newsel;

						obj_counttexturecoordinates(o,scn->lastobject->objdata.mapping,
							scn->lastobject->objdata.texx,scn->lastobject->objdata.texy,
							scn->lastobject->objdata.texox,
							scn->lastobject->objdata.texoy,																	 								 
							scn->lastobject->objdata.swaptexturexy,
							scn->lastobject->objdata.inverttexx,																									 
							scn->lastobject->objdata.inverttexy);
						
						material *pt=materiallist;
						while ((pt!=NULL) && (pt->number!=scn->lastobject->objdata.material1)) pt=pt->next;

						if (pt)
						mattexture=pt->handle;
						tTexture *txlist=texturelist;
						//for (x=0;x<pt->texture;x++) txlist=txlist->next;
						txlist=findtext(texturelist,pt->texture);
						memcpy(maptexture,txlist->layers[pt->layer],256*256*4);
						switch (scn->lastobject->objdata.material2)
						{
						case 0:
							{
								vector3 xpse;
								m_revxpose(scn->lastobject->xformmatrix,xpse);
								matrix s;
								for (int a=0; a<o->vertexnum; a++)
								{
									float p=getmappixel(o->vertexlist[a].t,scn->lastobject->objdata.chnnl1,scn->lastobject->objdata.normalsinverted);
									m_xpose(p*xpse.b.x,p*xpse.b.y,p*xpse.b.z,s);
									m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
								}									
								break;
							}
						case 1:
							{
								vector3 xpse;
								m_revscale(scn->lastobject->xformmatrix,xpse);
								matrix s;
								for (int a=0; a<o->vertexnum; a++)
								{
									float p=getmappixel(o->vertexlist[a].t,scn->lastobject->objdata.chnnl1,scn->lastobject->objdata.normalsinverted);
									m_scale(p*(xpse.b.x-1)+1,p*(xpse.b.y-1)+1,p*(xpse.b.z-1)+1,s);
									m_xformd(s,o->vertexlist[a].base,o->vertexlist[a].d);
								}									
								break;
							}
						case 2:
							{
								vector3 xpse;
								float phi;
								m_revrotate(scn->lastobject->xformmatrix,xpse,phi);
								matrix s;
								for (int a=0; a<o->vertexnum; a++)
								{
									float p=getmappixel(o->vertexlist[a].t,scn->lastobject->objdata.chnnl1,scn->lastobject->objdata.normalsinverted);
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
						obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);
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
			fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[1]=texnum;
			fread(&objnumber,sizeof(texnum),2,f); scn->lastobject->params[2]=objnumber+startobjnum;
			fread(&objnumber,sizeof(texnum),2,f); scn->lastobject->params[3]=objnumber+startobjnum;
			m_identity(scn->lastobject->xformmatrix);
			for (int x=0;x<=2;x++)
				for (int y=0;y<=3;y++)
				{
					w=0;
					fread(&w,scn->floatbytes,1,f);
					w=w << ((4-scn->floatbytes)*8);
					s = *(float*) &w;
					scn->lastobject->xformmatrix[x][y]=s;
				}
				matrix m;
				object *a=searchobjectbynumber(scn->objects,scn->lastobject->params[2]);
				object *b=searchobjectbynumber(scn->objects,scn->lastobject->params[3]);

				SELECTION *newsel = new SELECTION;
				newsel->selected=scn->lastobject->objdata.number;
				newsel->next=a->effects;
				a->effects=newsel;


				newsel = new SELECTION;
				newsel->selected=a->objdata.number;
				newsel->next=b->effects;
				b->effects=newsel;

				newsel = new SELECTION;
				newsel->selected=a->objdata.number;
				newsel->next=scn->lastobject->cloneparents;
				scn->lastobject->cloneparents=newsel;

				newsel = new SELECTION;
				newsel->selected=b->objdata.number;
				newsel->next=a->cloneparents;
				a->cloneparents=newsel;


				memcpy(m,b->xformmatrix,sizeof(matrix));
				matrix m2;
				m_mult(a->xformmatrix,scn->lastobject->xformmatrix,m2);
				obj_transform(b,m2);
				obj_boolean(a,b,scn->lastobject->params[1]);
				b->hidden=true;
				scn->lastobject->effect=true;
				memcpy(b->xformmatrix,m,sizeof(matrix));
				obj_transform(b,b->xformmatrix);
				break;
						   }
			}
	switch (scn->lastobject->primitive)
	{
	case dd3d_hasab: {
		fread(&texnum,sizeof(texnum),1,f); scn->lastobject->params[3]=texnum;
		break;
					 }
	}

	switch (scn->lastobject->primitive)
	{
	case dd3d_box:
	case dd3d_sphere:
	case dd3d_dodecaeder:
	case dd3d_icosaeder:
	case dd3d_hasab:
	case dd3d_cone:
	case dd3d_arc:
	case dd3d_loft:
	case dd3d_line:
	case dd3d_grid:
	case dd3d_freetrack:
	case dd3d_freeobj:
	case dd3d_clone:{
		m_identity(scn->lastobject->xformmatrix);
		for (x=0;x<=2;x++)
			for (y=0;y<=3;y++)
			{
				w=0;
				fread(&w,scn->floatbytes,1,f);
				w=w << ((4-scn->floatbytes)*8);
				s = *(float*) &w;
				scn->lastobject->xformmatrix[x][y]=s;
			}
			fread(&scn->lastobject->objdata,sizeof(tobjdata),1,f);
			scn->lastobject->objdata.number+=startobjnum;					
			fread(&scn->lastobject->selected,1,1,f);
			//memset(sb,0,256);
			//fread(sb,41,1,f);
			if (scn->lastobject->primitive==dd3d_clone)
			{
				SELECTION *s=NULL,*n=NULL,*last=NULL;
				short int ab,cd,ef;
				fread(&ab,sizeof(ab),1,f);
				for (cd=1;cd<=ab;cd++)
				{
					fread(&ef,sizeof(ef),1,f);
					n=new SELECTION[1];
					n->next=NULL;
					n->selected=ef+startobjnum;
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
				scn->lastobject->params[1]=(int)s;
				scn->lastobject->params[2]=(int)scn->objects;
			}
			obj_createprimitive(scn->lastobject,scn->lastobject->primitive,scn->lastobject->params[1],scn->lastobject->params[2],scn->lastobject->params[3],scn->lastobject->params[4],scn->lastobject->params[5]);
			obj_transform(scn->lastobject,scn->lastobject->xformmatrix);
			scn->lastobject->objdata.material1+=matnum;
			scn->lastobject->objdata.material2+=matnum;
			memcpy(scn->lastobject->name,&(sb[1]),256);
			if ((scn->lastobject->objdata.textured) && (scn->lastobject->primitive!=dd3d_clone))
			{
				material *mlist=materiallist;
				while ((mlist!=NULL) && (mlist->number!=scn->lastobject->objdata.material1)) mlist=mlist->next;
				polygon *p=scn->lastobject->polygonlist;
				int xa;
				if (mlist!=NULL)
				for (xa=0;xa<scn->lastobject->polynum;xa++)
				{
					scn->lastobject->polygonlist[xa].materialhandle=mlist->handle;
				}
			}
			if ((scn->lastobject->objdata.envmapped) && (scn->lastobject->primitive!=dd3d_clone))
			{
				material *mlist=materiallist;
				while ((mlist!=NULL) && (mlist->number!=scn->lastobject->objdata.material2)) mlist=mlist->next;
				polygon *p=scn->lastobject->polygonlist;
				int xa;
				if (mlist!=NULL)
				for (xa=0;xa<scn->lastobject->polynum;xa++)
					scn->lastobject->polygonlist[xa].envmaphandle=mlist->handle;
			}
			for (int xaa=0; xaa<4; xaa++)
			{
				scn->lastobject->colbuf[xaa]=scn->lastobject->objdata.color[xaa];
				scn->lastobject->color[xaa]=scn->lastobject->objdata.color[xaa];
				scn->lastobject->keyframercol[xaa]=scn->lastobject->objdata.color[xaa];
			}
			break;
					}
	}
	obj_counttexturecoordinates(scn->lastobject,0,scn->lastobject->objdata.texx,scn->lastobject->objdata.texy,scn->lastobject->objdata.texox,scn->lastobject->objdata.texoy,scn->lastobject->objdata.swaptexturexy,scn->lastobject->objdata.inverttexx,scn->lastobject->objdata.inverttexy);
}

void LoadObjectChunk2(scene *scn,FILE **ff, int starttex, int matnum, int startobjnum)
{
	FILE *f=*ff;

	fread(&scn->lastobject->parent,sizeof(scn->lastobject->parent),1,f);
	scn->lastobject->parent+=startobjnum;
	int *buffer=new int[10];
	fread(buffer,sizeof(scn->lastobject->parent),9,f);
}

cameraanimation *findcamera(cameraanimation *list, int id)
{
	for (cameraanimation *c=list;c;c=c->next) if (c->number==id) return c;
	return NULL;
}

GLuint findmaterial(int id)
{
	for (material *l=materiallist;l;l=l->next) if (l->number==id) return l->handle;
	return 0;
}

int findcaminlist(int id,cameraanimation *list)
{
	int i=0;
	for (cameraanimation *c=list;c;c=c->next) {if (c->number==id) return i; else i++;}
	return 0;
}

int findsceneinlist(int id,scene *list)
{
	int i=0;
	for (scene *c=list;c;c=c->next) {if (c->number==id) return i; else i++;}
	return 0;
}

int findmaterialinlist(int id,material *list)
{
	int i=0;
	for (material *c=list;c;c=c->next) {if (c->number==id) return i; else i++;}
	return 0;
}

sceneobjanim *findanim(sceneobjanim *s,int id)
{
	for (sceneobjanim *k=s;k;k=k->next) if (k->number==id) return k;
	return NULL;
}

int findaniminlist(int id,sceneobjanim *list)
{
	int i=0;
	for (sceneobjanim *c=list;c;c=c->next) {if (c->number==id) return i; else i++;}
	return 0;
}


void LoadEventChunk(FILE **ff)
{
	FILE *f=*ff;
	event *buf=new event;
	memset(buf,0,sizeof(buf));

	fread(buf->name,40,1,f);
	fread(&buf->startframe,sizeof(int),1,f);
	fread(&buf->endframe,sizeof(int),1,f);
	fread(&buf->pass,sizeof(int),1,f);
	fread(&buf->eventtype,sizeof(int),1,f);
	fread(&buf->startrectx,sizeof(float),1,f);
	fread(&buf->startrecty,sizeof(float),1,f);
	fread(&buf->startrectxrez,sizeof(float),1,f);
	fread(&buf->startrectyrez,sizeof(float),1,f);
	fread(&buf->endrectx,sizeof(float),1,f);
	fread(&buf->endrecty,sizeof(float),1,f);
	fread(&buf->endrectxrez,sizeof(float),1,f);
	fread(&buf->endrectyrez,sizeof(float),1,f);

	fread(&buf->blendfunc1,sizeof(GLuint),1,f);
	fread(&buf->blendfunc2,sizeof(GLuint),1,f);

	fread(buf->startcol,sizeof(float),4,f);
	fread(buf->endcol,sizeof(float),4,f);

	fread(&buf->textured,1,1,f);
	fread(&buf->texxtile,sizeof(int),1,f);
	fread(&buf->texytile,sizeof(int),1,f);
	fread(&buf->sceneid,sizeof(int),1,f);
	fread(&buf->animid,sizeof(int),1,f);
	fread(&buf->cameraid,sizeof(int),1,f);

	fread(&buf->camerastart,sizeof(float),1,f);
	fread(&buf->cameraend,sizeof(float),1,f);
	fread(&buf->animstart,sizeof(float),1,f);
	fread(&buf->animend,sizeof(float),1,f);

	fread(&buf->texture,sizeof(int),1,f);

	fread(&buf->clearzbuffer,1,1,f);
	fread(&buf->clearscreen,1,1,f);

	bool found=false;
	int cnt=0;
	for (event *e=eventlist; e; e=e->next) { if (e->pass==buf->pass) found=true; cnt++;}
	if (!found) passcounter++;

	buf->next=eventlist;
	eventlist=buf;

	int aa,bb;
	aa=(int)(floor((buf->startrectx-buf->startrectxrez/2.0)*639.0));
	bb=(int)(floor((buf->startrectx+buf->startrectxrez/2.0)*639.0))+1;
	buf->startrectx1=min(aa,bb);
	buf->startrectx2=max(aa,bb);
	aa=(int)(floor((buf->startrecty-buf->startrectyrez/2.0)*479.0));
	bb=(int)(floor((buf->startrecty+buf->startrectyrez/2.0)*479.0))+1;
	buf->startrecty1=min(aa,bb);
	buf->startrecty2=max(aa,bb);

	aa=(int)(floor((buf->endrectx-buf->endrectxrez/2.0)*639.0));
	bb=(int)(floor((buf->endrectx+buf->endrectxrez/2.0)*639.0))+1;
	buf->endrectx1=min(aa,bb);
	buf->endrectx2=max(aa,bb);
	aa=(int)(floor((buf->endrecty-buf->endrectyrez/2.0)*479.0));
	bb=(int)(floor((buf->endrecty+buf->endrectyrez/2.0)*479.0))+1;
	buf->endrecty1=min(aa,bb);
	buf->endrecty2=max(aa,bb);

	buf->iscene=findscene(scenelist,buf->sceneid);
	buf->isceneselected=findsceneinlist(buf->sceneid,scenelist);
	buf->icam=findcamera(buf->iscene->cameraanims,buf->cameraid);
	buf->icamselected=findcaminlist(buf->cameraid,buf->iscene->cameraanims);
	buf->mattexture=findmaterial(buf->texture);
	buf->matselected=findmaterialinlist(buf->texture,materiallist);
	buf->ianim=findanim(buf->iscene->anims,buf->animid);
	buf->ianimselected=findaniminlist(buf->animid,buf->iscene->anims);
}

void LoadEventChunk2(FILE **ff)
{
	FILE *f=*ff;
	fread(&eventlist->startrectx1,sizeof(int),1,f);
	fread(&eventlist->startrecty1,sizeof(int),1,f);
	fread(&eventlist->startrectx2,sizeof(int),1,f);
	fread(&eventlist->startrecty2,sizeof(int),1,f);
	fread(&eventlist->endrectx1,sizeof(int),1,f);
	fread(&eventlist->endrecty1,sizeof(int),1,f);
	fread(&eventlist->endrectx2,sizeof(int),1,f);
	fread(&eventlist->endrecty2,sizeof(int),1,f);
	fread(&eventlist->effect,sizeof(int),1,f);
	fread(&eventlist->param1,sizeof(int),1,f);
	fread(&eventlist->param2,sizeof(int),1,f);
	fread(&eventlist->param3,sizeof(int),1,f);
	fread(&eventlist->param4,sizeof(float),1,f);
	fread(&eventlist->param5,sizeof(float),1,f);
	fread(&eventlist->param6,sizeof(float),1,f);
}

void processchunk(chunk Chunk, FILE **ff)
{
	FILE *f=*ff;
	//sprintf(s,"%d - %4s",ftell(f),(char *)(&Chunk.ID));
	//MessageBox( 0,  s, "Baze narancslé", MB_OK );
	switch (Chunk.ID)
	{
	case aDDict_Texture:
		{
			LoadTextureChunk(ff,0);
			break;
		}
	case aDDict_Material:
		{
			LoadMaterialChunk(ff,0,0);
			break;
		}
	case aDDict_Scene:
		{
			scene *n = newscene();
			n->editview=perspcam;
			n->cameraanims=NULL;
			n->lastcamanim=NULL;
			char c[40];
			fread(c,1,40,*ff);
			strcpy(n->name,c);
			fread(&n->number,4,1,*ff);
			fread(&n->floatbytes,1,1,*ff);
			fread(&n->editview,sizeof(camera),1,*ff);
			n->floatbytes=4;
			if (scenelist==NULL)
			{
				scenelist=n;
				lastscene=n;
			}
			else
			{
				lastscene->next=n;
				lastscene=n;
			}
			break;
		}
	case aDDict_Selection:
		{
			LoadSelectionChunk(ff,0);
			break;
		}
	case aDDict_Lights:
		{
			fread(lastscene->lights,sizeof(light),8,f);
			break;
		}
	case aDDict_Fog:
		{
			lastscene->fog=true;
			fread(lastscene->fogcol,sizeof(float),4,f);
			fread(&lastscene->fogmode,sizeof(float),1,f);
			fread(&lastscene->fogstart,sizeof(float),1,f);
			fread(&lastscene->fogend,sizeof(float),1,f);
			fread(&lastscene->fogdensity,sizeof(float),1,f);
			break;
		}
	case aDDict_Object:
		{
			LoadObjectChunk(lastscene,ff,0,0,0);
			break;
		}
	case aDDict_Object2:
		{
			LoadObjectChunk2(lastscene,ff,0,0,0);
			break;
		}
	case aDDict_CameraAni:
		{
			cameraanimation *c=new cameraanimation;
			c->next=NULL;
			c->length=1000;
			c->number=0;
			c->keyframecapacity=10;
			c->keyframenum=0;
			c->keyframes= new keyframe[10];
			fread(c->name,40,1,f);
			fread(&c->length,4,1,f);
			fread(&c->number,4,1,f);
			if (lastscene->cameraanims==NULL)
			{
				lastscene->cameraanims=c;
				lastscene->lastcamanim=c;
			}
			else
			{
				lastscene->lastcamanim->next=c;
				lastscene->lastcamanim=c;
			}
			break;
		}
	case aDDict_CameraKey:
		{
			addkeyframe(lastscene->lastcamanim);
			keyframe *k=&lastscene->lastcamanim->keyframes[lastscene->lastcamanim->keyframenum-1];
			fread(&k->frame,4,1,f);
			camerakeyframe *ckey=new camerakeyframe;
			fread(&ckey->eye,sizeof(ckey->eye),1,f);
			fread(&ckey->target,sizeof(ckey->target),1,f);
			fread(&ckey->fov,sizeof(ckey->fov),1,f);
			fread(&ckey->roll,sizeof(ckey->roll),1,f);
			k->data=ckey;
			allocatecameratracks(lastscene->lastcamanim);
			calculatecameraanim(lastscene->lastcamanim);
			getcamerastate(lastscene->lastcamanim, actualcamera, 0);
			break;
		}
	case aDDict_Event:
		{
			LoadEventChunk(ff);
			break;
		}
	case aDDict_Event2:
		{
			LoadEventChunk2(ff);
			break;
		}
	case aDDict_Anim:
		{
				sceneobjanim *c=new sceneobjanim;
				c->next=NULL;
				c->length=1000;
				c->number=0;
				//c->keyframecapacity=10;
				//c->keyframenum=0;
				//c->keyframes= new keyframe[10];
				fread(c->name,40,1,f);
				fread(&c->length,4,1,f);
				fread(&c->number,4,1,f);
				//c->number+=camnumber;
				if (c->number!=0)
				{
					scene *scn=lastscene;
					if (scn->anims==NULL)
					{
						scn->anims=c;
						scn->lastanim=c;				
					}
					else
					{					
						scn->lastanim->next=c;
						scn->lastanim=c;
					}
					for (object *o=scn->objects;o;o=o->next)
					{
						objectanimation *n=new objectanimation;
						n->number=c->number;
						n->keyframecapacity=10;
						n->keyframenum=0;
						n->keyframes= new keyframe[10];
						n->length=scn->lastanim->length;
						n->next=NULL;
						if (o->actualanim==NULL)
						{
							o->anims=n;
							o->lastanim=n;
							o->actualanim=o->anims;
						}
						else
						{
							o->lastanim->next=n;
							o->lastanim=n;
						}
					}
				}
				else
				{
					lastscene->lastanim->length=c->length;
				}
				break;
		}
	case aDDict_AnimKey:
		{
			int frame;
			unsigned short objid;
			fread(&frame,4,1,f);
			fread(&objid,sizeof(objid),1,f);

			object *o=searchobjectbynumber(lastscene->objects,objid);

			addkeyframe(o->lastanim);

			keyframe *k=&o->lastanim->keyframes[o->lastanim->keyframenum-1];
			k->frame=frame;

			objkeyframedata *ckey=new objkeyframedata;

			fread(&ckey->position,sizeof(ckey->position),1,f);
			fread(&ckey->rotaxis,sizeof(ckey->rotaxis),1,f);
			fread(&ckey->stretch,sizeof(ckey->stretch),1,f);
			fread(&ckey->rotaxis,sizeof(ckey->rotaxis),1,f);
			fread(ckey->color,sizeof(float),4,f);
			
			k->data=ckey;
			allocateobjecttracks(o->lastanim);
			calculateobjectanim(o->lastanim);
			getobjectstate(o->lastanim, *o, 0);
			break;
		}
	case aDDict_EOF:
		{
			break;
		}
	}
}

void LoadProject(const char *filename)
{
	passcounter=0;
	aa=0;
	scenelist=NULL;
	materiallist=NULL;
	texturelist=NULL;
	lastmaterial=NULL;
	lasttexture=NULL;
	eventlist=NULL;
	lastscene=scenelist;
	
	FILE *f;
	char head[8];
	chunk Chunk;
	int currentpos;

	f=fopen(filename,"rb");

	if (f==NULL) { MessageBox( 0, "Baze narancslé", "mit csúszkálsz?", MB_OK ); return;}
	else
	{
		fseek(f,0,SEEK_SET);
		fread(head,8,1,f);
		Chunk.ID=1;
		numberoftextures=0;

		while (Chunk.ID!=aDDict_EOF) 
		{
			fread(&Chunk,sizeof(Chunk),1,f);
			currentpos=ftell(f);
			if (Chunk.ID==aDDict_Texture) numberoftextures++;
			fseek(f,currentpos+Chunk.size,SEEK_SET);
		}

		fseek(f,0,SEEK_SET);
		fread(head,8,1,f);
		Chunk.ID=1;

		while (Chunk.ID!=aDDict_EOF)
		{
			fread(&Chunk,sizeof(Chunk),1,f);
			currentpos=ftell(f);
			processchunk(Chunk,&f);
			fseek(f,currentpos+Chunk.size,SEEK_SET);
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
	actualscene=scenelist;
	scene *buffer=scenelist;
	while (buffer!=NULL)
	{
		if (buffer->cameraanims==NULL)
		{
			buffer->cameraanims=new cameraanimation;
			sprintf(buffer->cameraanims->name,"Default");
			buffer->cameraanims->next=NULL;
			buffer->cameraanims->length=1000;
			buffer->cameraanims->number=0;
			buffer->cameraanims->keyframecapacity=10;
			buffer->cameraanims->keyframenum=0;
			buffer->cameraanims->keyframes= new keyframe[10];
			buffer->lastcamanim=buffer->cameraanims;
			allocatecameratracks(buffer->lastcamanim);
		}
		for (object *o=buffer->objects;o!=NULL;o=o->next)
		{
			obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);

			if (o->objdata.shading!=dd3d_default)
				for (int x=0; x<o->polynum; x++) o->polygonlist[x].shading=o->objdata.shading;
		}
		buffer=buffer->next;
	}
	actualcameraanim=actualscene->cameraanims;

	fclose(f);
}

int countmaterials()
{
	int i=0;
	for (material *m=materiallist;m;m=m->next) i=m->number;
	return i+1;
}

int counttextures()
{
	int i=0;
	for (tTexture *t=texturelist;t;t=t->next) i=t->number;
	return i+1;
}

int countobjects(scene scn)
{
	int i=0;
	for (object *o=scn.objects;o;o=o->next) i=o->objdata.number;
	return i+1;
}

int countcameras(scene scn)
{
	int i=0;
	for (cameraanimation *o=scn.cameraanims;o;o=o->next) i=o->number;
	return i+1;
}


void processchunk2(scene *scn,chunk Chunk, FILE **ff,int texnum, int matnum, int objnum, int camnumber, bool objectload,bool selectionload,bool textureload,bool lightload,bool cameraload,bool environmentload)
{
	FILE *f=*ff;
	//sprintf(s,"%d - %4s",ftell(f),(char *)(&Chunk.ID));
	//MessageBox( 0,  s, "Baze narancslé", MB_OK );
	switch (Chunk.ID)
	{
	case aDDict_Texture:
		{
			if (textureload)
			LoadTextureChunk(ff,texnum);
			break;
		}
	case aDDict_Material:
		{
			if (textureload)
			LoadMaterialChunk(ff,texnum,matnum);
			break;
		}
	case aDDict_Scene:
		{
			/*scene *n = newscene();
			n->editview=perspcam;
			n->cameraanims=NULL;
			n->lastcamanim=NULL;
			char c[40];
			fread(c,1,40,*ff);
			strcpy(n->name,c);
			fread(&n->number,4,1,*ff);
			fread(&n->floatbytes,1,1,*ff);
			fread(&n->editview,sizeof(camera),1,*ff);
			n->floatbytes=4;
			if (scenelist==NULL)
			{
				scenelist=n;
				lastscene=n;
			}
			else
			{
				scn.next=n;
				lastscene=n;
			}*/
			break;
		}
	case aDDict_Selection:
		{
			if (selectionload)
			LoadSelectionChunk(ff,objnum);
			break;
		}
	case aDDict_Lights:
		{
			if (lightload)
			fread(scn->lights,sizeof(light),8,f);
			break;
		}
	case aDDict_Fog:
		{
			if (environmentload)
			{
				scn->fog=true;
				fread(scn->fogcol,sizeof(float),4,f);
				fread(&scn->fogmode,sizeof(float),1,f);
				fread(&scn->fogstart,sizeof(float),1,f);
				fread(&scn->fogend,sizeof(float),1,f);
				fread(&scn->fogdensity,sizeof(float),1,f);
			}
			break;
		}
	case aDDict_Object:
		{
			if (objectload)
			LoadObjectChunk(scn,ff,texnum,matnum,objnum);
			break;
		}
	case aDDict_Object2:
		{
			if (objectload)
			 LoadObjectChunk2(scn,ff,texnum,matnum,objnum);
			break;
		}
	case aDDict_CameraAni:
		{
			if (cameraload)
			{
				cameraanimation *c=new cameraanimation;
				c->next=NULL;
				c->length=1000;
				c->number=0;
				c->keyframecapacity=10;
				c->keyframenum=0;
				c->keyframes= new keyframe[10];
				fread(c->name,40,1,f);
				fread(&c->length,4,1,f);
				fread(&c->number,4,1,f);
				c->number+=camnumber;
				if (scn->cameraanims==NULL)
				{
					scn->cameraanims=c;
					scn->lastcamanim=c;
				}
				else
				{
					scn->lastcamanim->next=c;
					scn->lastcamanim=c;
				}
			}
			break;
		}
	case aDDict_CameraKey:
		{
			if (cameraload)
			{
				addkeyframe(scn->lastcamanim);
				keyframe *k=&scn->lastcamanim->keyframes[scn->lastcamanim->keyframenum-1];
				fread(&k->frame,4,1,f);
				camerakeyframe *ckey=new camerakeyframe;
				fread(&ckey->eye,sizeof(ckey->eye),1,f);
				fread(&ckey->target,sizeof(ckey->target),1,f);
				fread(&ckey->fov,sizeof(ckey->fov),1,f);
				fread(&ckey->roll,sizeof(ckey->roll),1,f);
				k->data=ckey;
				allocatecameratracks(scn->lastcamanim);
				calculatecameraanim(scn->lastcamanim);
				getcamerastate(scn->lastcamanim, actualcamera, 0);
			}
			break;
		}
	case aDDict_Anim:
		{
				sceneobjanim *c=new sceneobjanim;
				c->next=NULL;
				c->length=1000;
				c->number=0;
				//c->keyframecapacity=10;
				//c->keyframenum=0;
				//c->keyframes= new keyframe[10];
				fread(c->name,40,1,f);
				fread(&c->length,4,1,f);
				fread(&c->number,4,1,f);
				//c->number+=camnumber;
				if (scn->anims==NULL)
				{
					scn->anims=c;
					scn->lastanim=c;
				}
				else
				{
					scn->lastanim->next=c;
					scn->lastanim=c;
				}
				for (object *o=scn->objects;o;o=o->next)
				{
					objectanimation *n=new objectanimation;
					n->number=c->number;
					n->keyframecapacity=10;
					n->keyframenum=0;
					n->keyframes= new keyframe[10];
					n->next=NULL;
					if (o->actualanim==NULL)
					{
						o->anims=n;
						o->lastanim=n;
					}
					else
					{
						o->lastanim->next=n;
						o->lastanim=n;
					}
				}
			break;
		}
	case aDDict_AnimKey:
		{
			int frame;
			unsigned short objid;
			fread(&frame,4,1,f);
			fread(&objid,sizeof(objid),1,f);

			object *o=searchobjectbynumber(scn->objects,objid);

			addkeyframe(o->lastanim);

			keyframe *k=&o->lastanim->keyframes[o->lastanim->keyframenum-1];
			k->frame=frame;

			objkeyframedata *ckey=new objkeyframedata;

			fread(&ckey->position,sizeof(ckey->position),1,f);
			fread(&ckey->rotaxis,sizeof(ckey->rotaxis),1,f);
			fread(&ckey->stretch,sizeof(ckey->stretch),1,f);
			fread(&ckey->rotaxis,sizeof(ckey->rotaxis),1,f);
			fread(ckey->color,sizeof(float),4,f);
			
			k->data=ckey;
			allocateobjecttracks(o->lastanim);
			calculateobjectanim(o->lastanim);
			getobjectstate(o->lastanim, *o, 0);

			break;
		}
	case aDDict_EOF:
		{
			break;
		}
	}
}


bool loadaDDictscene(scene *scn, tTexture *textures, char *filename, bool objectload,bool selectionload,bool textureload,bool lightload,bool cameraload,bool environmentload)
{
	aa=0;
	FILE *f;
	char head[8];
	chunk Chunk;
	int currentpos;
	int startmat=countmaterials(),starttex=counttextures(),startobj=countobjects(*scn),startcam=countcameras(*scn);

	scn->floatbytes=4;

	f=fopen(filename,"rb");
	if (f==NULL) { MessageBox( 0, "Baze narancslé", "mit csúszkálsz?", MB_OK ); return false;}
	else
	{
		fseek(f,0,SEEK_SET);
		fread(head,8,1,f);
		Chunk.ID=1;
		numberoftextures=0;

		while (Chunk.ID!=aDDict_EOF) 
		{
			fread(&Chunk,sizeof(Chunk),1,f);
			currentpos=ftell(f);
			if (Chunk.ID==aDDict_Texture) numberoftextures++;
			fseek(f,currentpos+Chunk.size,SEEK_SET);
		}

		fseek(f,0,SEEK_SET);
		fread(head,8,1,f);
		Chunk.ID=1;

		while (Chunk.ID!=aDDict_EOF)
		{
			fread(&Chunk,sizeof(Chunk),1,f);
			currentpos=ftell(f);
			processchunk2(scn,Chunk,&f,starttex,startmat,startobj,startcam,objectload,selectionload,textureload,lightload,cameraload,environmentload);
			fseek(f,currentpos+Chunk.size,SEEK_SET);
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
		}
	}
	actualscene=scenelist;
	scene *buffer=scenelist;
	while (buffer!=NULL)
	{
		if (buffer->cameraanims==NULL)
		{
			buffer->cameraanims=new cameraanimation;
			sprintf(buffer->cameraanims->name,"Default");
			buffer->cameraanims->next=NULL;
			buffer->cameraanims->length=1000;
			buffer->cameraanims->number=0;
			buffer->cameraanims->keyframecapacity=10;
			buffer->cameraanims->keyframenum=0;
			buffer->cameraanims->keyframes= new keyframe[10];
			buffer->lastcamanim=buffer->cameraanims;
			allocatecameratracks(buffer->lastcamanim);
		}
		for (object *o=buffer->objects;o!=NULL;o=o->next)
		{
			obj_counttexturecoordinates(o,0,o->objdata.texx,o->objdata.texy,o->objdata.texox,o->objdata.texoy,o->objdata.swaptexturexy,o->objdata.inverttexx,o->objdata.inverttexy);

			if (o->objdata.shading!=dd3d_default)
				for (int x=0; x<o->polynum; x++) o->polygonlist[x].shading=o->objdata.shading;
		}
		buffer=buffer->next;
	}
	actualcameraanim=actualscene->cameraanims;

	fclose(f);
	return true;
}

bool saveaDDictscene(scene &scn, tTexture *textures, const char *filename)
{
	masks[0]=0x00000000;
	masks[1]=0xff000000;
	masks[2]=0xffff0000;
	masks[3]=0xffffff00;
	masks[4]=0xffffffff;
	FILE *f;

	f=fopen(filename,"w+b");
	if (f==NULL) { MessageBox( 0, "Baze narancslé", "mit csúszkálsz?", MB_OK ); return false;}
	else
	{
		fwrite(fileheader,8,1,f);
		//write textures
		if (!(counttextures()==1 && texturelist->commandnum==0))
		SaveTextureChunks(&f);
		//write materials
		if (materiallist!=NULL)
		SaveMaterialChunks(&f);
		//write scenes
		/*
		while (s!=NULL)
		{*/
		chunk Chunk;
		scene *s=&scn;
		s->floatbytes=4;
		Chunk.ID=aDDict_Scene;
		Chunk.size=45+sizeof(camera);//WriteSceneChunkData(false,&f,s);
		fwrite(&Chunk,sizeof(Chunk),1,f);
		WriteSceneChunkData(true,&f,s);
		/*	s=s->next;
		}*/

	}

	chunk Chunk;
	Chunk.ID=aDDict_EOF;
	Chunk.size=0;
	fwrite(&Chunk,sizeof(chunk),1,f);

	fclose(f);
	return true;
}

void loadandscene(scene &scn, const char *filename)
{
	FILE *f;
	f=fopen(filename,"rb");
	if (f)
	{
		fseek(f,4,SEEK_SET);
		short int objnum;
		fread(&objnum,2,1,f);
		andobj *objs=new andobj[objnum];
		fseek(f,10,SEEK_SET);
		short int objpacknum;
		fread(&objpacknum,2,1,f);
		short int indexes[16];
		fread(indexes,2,16,f);
		andobjdata *objpackets=new andobjdata[objpacknum];
		fread(objpackets,4,objpacknum,f);
		int on=0;
		for (int x=0; x<objpacknum; x++)
		{
			for (int z=0; z<=objpackets[x].ismetles; z++)
			{
				objs[on].group=objpackets[x];
				on++;
			}
		}
		unsigned char tort;

		unsigned char aaa[27];

		fseek(f,indexes[0],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].x=tort/256.0f;	on++;}}
		fseek(f,indexes[1],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].y=tort/256.0f;	on++;}}
		fseek(f,indexes[2],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].z=tort/256.0f;	on++;}}
		fseek(f,indexes[7],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].x+=(signed char)tort;on++;}}
		fseek(f,indexes[8],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].y+=(signed char)tort;on++;}}
		fseek(f,indexes[9],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ if (objpackets[x].curvenum) fread(aaa,1,27,f); for (int y=0; y<=objpackets[x].ismetles; y++){fread(&tort,1,1,f);	objs[on].z+=(signed char)tort;on++;}}

		fseek(f,indexes[3],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].xs=tort/256.0f; on++;}}
		fseek(f,indexes[4],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].ys=tort/256.0f; on++;}}
		fseek(f,indexes[5],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].zs=tort/256.0f; on++;}}
		fseek(f,indexes[10],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].xs+=(signed char)tort; on++;}}
		fseek(f,indexes[11],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].ys+=(signed char)tort; on++;}}
		fseek(f,indexes[12],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].zs+=(signed char)tort; on++;}}
		/*fseek(f,indexes[13],SEEK_SET); on=0;
		for (x=0; x<objpacknum; x++){ fread(&tort,1,1,f); for (int z=0; z<=objpackets[x].ismetles; z++){
				objs[on].xs+=(signed char)tort; 
				objs[on].ys+=(signed char)tort; 
				objs[on].zs+=(signed char)tort; 
				on++;}}*/
		
		fseek(f,indexes[14],SEEK_SET);
		for (x=0; x<objnum; x++) fread(&objs[x].rot,2,1,f); 

		for (x=0; x<objnum; x++)
		{
			scene_addobject(&scn);
			obj_createprimitive(scn.lastobject,dd3d_hasab,objs[x].group.felbontas+3,2,1,0,0);

			matrix m1,m2,m3,m;
			m_xpose(0,0.5,0,m2);
			m_rotate(0,1,0,45.0*radtheta,m);
			m_scale(sqrt(2.0),1,sqrt(2.0),m1);
			m_mult(m,m2,m);
			m_mult(m1,m,m);
			
			m_scale(objs[x].xs,objs[x].zs,objs[x].ys,m1);
			m_mult(m1,m,m1);

			matrix r,mx;

			m_rotate(0,0,1,objs[x].rot.z/32.0*360.0*radtheta,m3);
			
			m_rotate(0,-1,0,objs[x].rot.y/32.0*360.0*radtheta,r);

			m_mult(r,m3,m3);

			m_rotate(1,0,0,objs[x].rot.x/32.0*360.0*radtheta,r);
			m_mult(r,m3,m3);

			m_xpose(-objs[x].z,objs[x].y,-objs[x].x,m2);
			m_mult(m3,m1,m1);
			m_mult(m2,m1,m1);
			m_scale(5,-5,5,m);
			m_mult(m,m1,m1);

			obj_transform(scn.lastobject,m1);
		}

	}
}