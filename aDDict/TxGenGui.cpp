#include "TxGenGui.h"

char *s = new char[256];
tTexture *pt;
byte bb;
int matlayer=0;
bool matalpha=false;
int alphalayer=0;
int alphamode=0;
int alphastart=0,alphaselected=0,alphanum=1;
unsigned int alayers[4];

int texturenum(tTexture *t)
{
	int x=0;
	tTexture *tt=t;
	while (tt!=NULL)
	{
		x++;
		tt=tt->next;
	}
	return x;
}

void drawtextures()
{
	glColor4f(1,1,1,1);
	glPushAttrib(GL_ALL_ATTRIB_BITS);	
	glEnable(GL_TEXTURE_2D);
	glDepthMask(false);
	glBindTexture(GL_TEXTURE_2D,texlayers[0]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(1+12,20+13); glTexCoord2f(0,1); glVertex2f(255+12,20+13); glTexCoord2f(1,1); glVertex2f(255+12,275+12); glTexCoord2f(1,0); glVertex2f(1+12,275+12);glEnd();
	glBindTexture(GL_TEXTURE_2D,texlayers[2]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(256+12,20+13); glTexCoord2f(0,1); glVertex2f(511+12,20+13); glTexCoord2f(1,1); glVertex2f(511+12,275+12); glTexCoord2f(1,0); glVertex2f(256+12,275+12);glEnd();
	glBindTexture(GL_TEXTURE_2D,texlayers[1]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(1+12,276+13); glTexCoord2f(0,1); glVertex2f(255+12,276+13); glTexCoord2f(1,1); glVertex2f(255+12,531+12); glTexCoord2f(1,0); glVertex2f(1+12,531+12);glEnd();
	glBindTexture(GL_TEXTURE_2D,texlayers[3]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(256+12,276+13); glTexCoord2f(0,1); glVertex2f(511+12,276+13); glTexCoord2f(1,1); glVertex2f(511+12,531+12); glTexCoord2f(1,0); glVertex2f(256+12,531+12);glEnd();
	glDepthMask(true);
	glPopAttrib();
}

void drawpretextures()
{
	glColor4f(1,1,1,1);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(false);
	glBindTexture(GL_TEXTURE_2D,prelayers[0]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(800-128-10-128,600-128-44-12-128); glTexCoord2f(0,1); glVertex2f(800-128-11,600-128-44-12-128); glTexCoord2f(1,1); glVertex2f(800-128-11,600-128-44-13); glTexCoord2f(1,0); glVertex2f(800-128-10-128,600-128-44-13);glEnd();
	glBindTexture(GL_TEXTURE_2D,prelayers[2]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(800-128-10,600-128-44-12-128); glTexCoord2f(0,1); glVertex2f(800-11,600-128-44-12-128); glTexCoord2f(1,1); glVertex2f(800-11,600-128-44-13); glTexCoord2f(1,0); glVertex2f(800-128-10,600-128-44-13);glEnd();
	glBindTexture(GL_TEXTURE_2D,prelayers[1]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(800-128-10-128,600-128-44-12); glTexCoord2f(0,1); glVertex2f(800-128-11,600-128-44-12); glTexCoord2f(1,1); glVertex2f(800-128-11,600-44-13); glTexCoord2f(1,0); glVertex2f(800-128-10-128,600-44-13);glEnd();
	glBindTexture(GL_TEXTURE_2D,prelayers[3]); glBegin(GL_QUADS); glTexCoord2f(0,0); glVertex2f(800-128-10,600-128-44-12); glTexCoord2f(0,1); glVertex2f(800-11,600-128-44-12); glTexCoord2f(1,1); glVertex2f(800-11,600-44-13); glTexCoord2f(1,0); glVertex2f(800-128-10,600-44-13);glEnd();
	glDepthMask(true);
	glPopAttrib();
}

void drawpreview()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (pretextures[texgenmenu][texsubmenu[texgenmenu]]!=0)
	{
		glDisable(GL_TEXTURE_2D);
		glColor4f(buttontextlit);
		rectangle(800-128-11,600-128-44-12,800-11,600-44-12);
		glColor4f(1,1,1,1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,pretextures[texgenmenu][texsubmenu[texgenmenu]]);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(800-128-11,600-128-44-12);
		glTexCoord2f(0,1);
		glVertex2f(800-11,600-128-44-12);
		glTexCoord2f(1,1);
		glVertex2f(800-11,600-44-12);
		glTexCoord2f(1,0);
		glVertex2f(800-128-11,600-44-12);
		glEnd();
	}
	glPopAttrib();

}

void TextureGeneratorGUI(byte mode)
{
	if (mode==DD_Draw)
	{
		drawtextures();
		drawpreview();
		glColor4f(buttontextlit);
		rectangle(0+12,20+13,255+12,275+13);
		rectangle(255+12,20+13,511+12,275+13);
		rectangle(0+12,275+13,255+12,531+13);
		rectangle(255+12,275+13,511+12,531+13);
		glColor4f(1,1,1,1);
	}


	pt = texturelist;
	for (x=1;x<=selectedtexture;x++) pt=pt->next;
	actualtexture=pt;
	
	if (button(540,20+13,617,40+13,"Create" ,13,13,base,texgenmenu==0,mode) == DDgui_LeftClick) texgenmenu=0;
	if (button(618,20+13,694,40+13,"Distort",8 ,13,base,texgenmenu==1,mode) == DDgui_LeftClick) texgenmenu=1;
	if (button(695,20+13,771,40+13,"Layer"  ,17,13,base,texgenmenu==2,mode) == DDgui_LeftClick) texgenmenu=2;
	if (button(540,41+13,617,61+13,"Color"  ,17,13,base,texgenmenu==3,mode) == DDgui_LeftClick) texgenmenu=3;
	if (button(618,41+13,694,61+13,"Blur"   ,20,13,base,texgenmenu==4,mode) == DDgui_LeftClick) texgenmenu=4;
	if (button(695,41+13,771,61+13,"File"   ,20,13,base,texgenmenu==5,mode) == DDgui_LeftClick) texgenmenu=5;
	
	if (texgenmenu<5)
	{
		
		if (((button(540,490+12,650,510+12,"Preview",25,13,base,false,mode) == DDgui_LeftClick) || randomed) && mode==DD_AfterCheck)
		{
			randomed=false;
			memcpy(buffertexture.texts,generatedtexture.texts,sizeof(generatedtexture.texts));
			for (x=0;x<4;x++) 
				for(y=0;y<256;y++)
					for(z=0;z<256;z++)
						buffertexture.layers[x][y][z]=generatedtexture.layers[x][y][z];
			buffertexture.performcommand(texdata[texgenmenu][texsubmenu[texgenmenu]]);
			waitleftbutton=true;
			if (pretextures[texgenmenu][texsubmenu[texgenmenu]]==NULL)
			{
				glGenTextures(1,&pretextures[texgenmenu][texsubmenu[texgenmenu]]);
				glBindTexture(GL_TEXTURE_2D,pretextures[texgenmenu][texsubmenu[texgenmenu]]);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			}

			glBindTexture(GL_TEXTURE_2D,pretextures[texgenmenu][texsubmenu[texgenmenu]]);
			glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,buffertexture.layers[texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer]);
		}
		if ((button(540,469+12,650,489+12,"Do Effect",17,13,base,false,mode) == DDgui_LeftClick)&& mode==DD_AfterCheck)
		{
         generatedtexture.performcommand(texdata[texgenmenu][texsubmenu[texgenmenu]]);
		 generatedtexture.commands[generatedtexture.commandnum]=texdata[texgenmenu][texsubmenu[texgenmenu]];
		 generatedtexture.commandnum++;

		 waitleftbutton=true;
		 for (x=0;x<4;x++)
		 {
			 glBindTexture(GL_TEXTURE_2D,texlayers[x]);
			 glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[x]);
		 }
		 memcpy(actualtexture->commands,generatedtexture.commands,sizeof(generatedtexture.commands));
		 memcpy(actualtexture->texts,generatedtexture.texts,sizeof(generatedtexture.texts));
		 actualtexture->commandnum=generatedtexture.commandnum;
		 memcpy(actualtexture->layers,generatedtexture.layers,sizeof(generatedtexture.layers));
		}
		if (mode==DD_Draw) {
			if (!((texgenmenu==2,mode) && ((texsubmenu[2]==2,mode) || (texsubmenu[2]==3,mode) || (texsubmenu[2]==4) || (texsubmenu[2]==5))))
			{
				glRasterPos2i(560,153);
				glPrint("Layer:",base);
			}
			else
			{
				glRasterPos2i(542,153);
				glPrint("Layer 1:",base);
			}
		}
		if (button(620,130,640,150,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer==0,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer=0;
		if (button(641,130,661,150,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer==2,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer=2;
		if (button(620,151,640,171,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer==1,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer=1;
		if (button(641,151,661,171,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer==3,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer=3;
		if ((button(540,511+12,650,531+12,"Undo",38,13,base,false,mode) == DDgui_LeftClick)&& mode==DD_AfterCheck)
		{
			if (generatedtexture.commandnum>0)
			{
				SwapBuffers(hDC);
				for (x=0;x<4;x++) memset(generatedtexture.layers[x],0,sizeof(generatedtexture.layers[x]));

				for (y=0;y<4;y++)
				{
					glBindTexture(GL_TEXTURE_2D, texlayers[y]);
					glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
				}
				for (x=0;x<generatedtexture.commandnum-1;x++)
				{
					generatedtexture.performcommand(generatedtexture.commands[x]);
					for (y=0;y<4;y++)
					{
						glBindTexture(GL_TEXTURE_2D, texlayers[y]);
						glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
					}
					for (y=0;y<3;y++)
					{
						glEnable(GL_TEXTURE_2D);
						drawtextures();
						glDisable(GL_TEXTURE_2D);
						glColor4f(buttontextlit);
						rectangle(0+12,540+13,523,560+13);
						glColor4f(col4);
						quad(0+12,540+13,(int)((float)x/(float)(generatedtexture.commandnum-1)*511.0+12.0),560+13);
						glColor4f(1,1,1,1);
						SwapBuffers(hDC);
					}
					
				}				
				generatedtexture.commandnum--;
				glColor4f(buttontextlit);
				rectangle(0+12,540+13,523,560+13);
				glColor4f(col4);
				quad(0+12,540+13,511+12,560+13);
				glColor4f(1,1,1,1);
				SwapBuffers(hDC);
				waitleftbutton=true;
			}
		}
	}
	if (mode!=DD_AfterCheck)
	switch (texgenmenu)
		{
		case 0 : {
			if (button(540,80,617,100, "Envmap" ,13,13,base,texsubmenu[texgenmenu]==0,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=0;
			if (button(618,80,694,100, "Clear"  ,17,13,base,texsubmenu[texgenmenu]==1,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=1;
			if (button(695,80,771,100, "Plasma" ,13,13,base,texsubmenu[texgenmenu]==2,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=2;
			if (button(540,101,597,121,"SubPl"  ,07,13,base,texsubmenu[texgenmenu]==3,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=3;
			if (button(598,101,655,121,"Fractl" ,02,13,base,texsubmenu[texgenmenu]==4,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=4;
			if (button(656,101,713,121,"Cells"  ,06,13,base,texsubmenu[texgenmenu]==5,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=5;
			if (button(714,101,771,121,"Text"   ,11,13,base,texsubmenu[texgenmenu]==6,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=6;
			switch (texsubmenu[texgenmenu])
			{
				case 0 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_envmap;
					if (mode==DD_Draw) {
					glRasterPos2i(560,193);
					glPrint(" Type:",base);
					}

					if (button(620,180,683,200,"Normal",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(684,180,730,200,"Size",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if ((boolean)texdata[texgenmenu][texsubmenu[texgenmenu]].command[2])
					{
						if (mode==DD_Draw) {
							glRasterPos2i(542,223);
							glPrint("Channel:",base);
						}
						if (button(620,210,659,230,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=0;
						if (button(660,210,716,230,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=1;
						if (button(717,210,762,230,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=2;
						if (mode==DD_Draw) sprintf(s,"Size: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
						ValueBar(541,240,796,259,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,90,13,base,mode);
					}
					break;

				}
				case 1 : { texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_clear; break;}
				case 2 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_plasma;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint("Channel:",base);
					}
					if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]=0;
					if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]=1;
					if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]=2;
					if (mode==DD_Draw) sprintf(s,"Number of X-Sines: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
					ValueBar(541,210,796,229,0,10,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,37,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Number of Y-Sines: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
					ValueBar(541,230,796,249,0,10,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,37,13,base,mode);

					if (mode==DD_Draw) sprintf(s,"X Shift: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
					ValueBar(541,260,796,279,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,78,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Y-Shift: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]);
					ValueBar(541,280,796,299,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[5],s,78,13,base,mode);
					break;

				}
				case 3 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_subplasm;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint("Channel:",base);
					}
					if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (mode==DD_Draw) {
					glRasterPos2i(542,223);
					glPrint("   Size:",base);
					}
					if (button(620,210,643,230,"4",7,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==4,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=4;
					if (button(644,210,666,230,"8",7,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==8,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=8;
					if (button(667,210,689,230,"16",2,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==16,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=16;
					if (button(690,210,711,230,"32",2,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==32,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=32;
					if (button(712,210,734,230,"64",2,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==64,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=64;
					if (button(735,210,762,230,"128",1,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==128,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=128;

					if (button(540,448+12,650,468+12,"Randomize",17,13,base,false,mode) == DDgui_LeftClick)
					{
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]=rand();
						randomed=true;
					}

					break;
				}
				case 4 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_fractal;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint("Channel:",base);
					}
					if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick) texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;

					if (button(540,448+12,650,468+12,"Randomize",17,13,base,false,mode) == DDgui_LeftClick)
					{
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]=rand();
						randomed=true;
					}
					break;

				}
				case 5 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_cells;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint("Channel:",base);
					}
					if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;

					if (mode==DD_Draw) sprintf(s,"Number of points: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
					ValueBar(541,210,796,229,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,37,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Power: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
					ValueBar(541,230,796,249,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,86,13,base,mode);

					if (button(540,448+12,650,468+12,"Randomize",17,13,base,false,mode) == DDgui_LeftClick)
					{
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]=rand();
						texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]=rand();
						randomed=true;
					}
					break;
				}
				case 6 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_text;

					if (mode==DD_Draw) sprintf(s,"Size: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
					ValueBar(541,180,796,200,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,86,13,base,mode);

					if (mode==DD_Draw) sprintf(s,"Xpos: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
					ValueBar(541,210,796,229,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,86,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Ypos: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]);
					ValueBar(541,230,796,249,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[5],s,86,13,base,mode);

					if (button(540,260,632,280,"Bold",27,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]!=0,mode) == DDgui_LeftClick) { texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]=!(texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]); waitleftbutton=true; }
					if (button(633,260,725,280,"Italic",15,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]!=0,mode) == DDgui_LeftClick) { texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=!(texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]); waitleftbutton=true; }

					if (button(540,290,632,309,fonts[0],25,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(540,310,632,329,fonts[1],16,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(540,330,632,349,fonts[2],12,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(540,350,632,369,fonts[3],20,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;

					if (button(633,290,785,309,fonts[4],50,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==4,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=4;
					if (button(633,310,785,329,fonts[5],50,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==5,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=5;
					if (button(633,330,785,349,fonts[6],10,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==6,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=6;
					if (button(633,350,785,369,fonts[7],46,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==7,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=7;

					if (mode==DD_Draw) {
					glColor4f(buttontextlit);
					glRasterPos2i(540,390);
					glPrint(generatedtexture.texts[generatedtexture.commandnum].text,basesmall);
					glColor4f(1,1,1,1);
					}

					if ((button(540,448+12,650,468+12,"Enter Text",13,13,base,false,mode) == DDgui_LeftClick) && mode!=DD_AfterCheck)
					{
						char *ss=readline("Enter text",300,0,base,generatedtexture.texts[generatedtexture.commandnum].text);
						for (int x=generatedtexture.commandnum;x<=100;x++) sprintf(generatedtexture.texts[x].text,"%s",ss);
						memcpy(buffertexture.texts,generatedtexture.texts,sizeof(generatedtexture.texts));
						for (x=0;x<4;x++) 
							for(y=0;y<256;y++)
								for(z=0;z<256;z++)
									buffertexture.layers[x][y][z]=generatedtexture.layers[x][y][z];
								buffertexture.performcommand(texdata[texgenmenu][texsubmenu[texgenmenu]]);
								waitleftbutton=true;
								if (pretextures[texgenmenu][texsubmenu[texgenmenu]]==NULL)
								{
									glGenTextures(1,&pretextures[texgenmenu][texsubmenu[texgenmenu]]);
									glBindTexture(GL_TEXTURE_2D,pretextures[texgenmenu][texsubmenu[texgenmenu]]);
									glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
									glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
								}

								glBindTexture(GL_TEXTURE_2D,pretextures[texgenmenu][texsubmenu[texgenmenu]]);
								glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,buffertexture.layers[texdata[texgenmenu][texsubmenu[texgenmenu]].c.layer]);
						glColor4f(buttontextlit);
						glRasterPos2i(540,390);
						glPrint(generatedtexture.texts[generatedtexture.commandnum].text,basesmall);
						glColor4f(1,1,1,1);
					}
					texdata[texgenmenu][texsubmenu[texgenmenu]].command[8]=(byte)generatedtexture.commandnum;
					break;
				}
			
			}
		break;
		}
		case 1 : {
			if (button(540,80,617,100, "Sinus"  ,17,13,base,texsubmenu[texgenmenu]==0,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=0;
			if (button(618,80,694,100, "Twirl"  ,17,13,base,texsubmenu[texgenmenu]==1,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=1;
			if (button(695,80,771,100, "Offset" ,13,13,base,texsubmenu[texgenmenu]==2,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=2;
			if (button(618,101,694,121,"MapDist", 8,13,base,texsubmenu[texgenmenu]==3,mode) == DDgui_LeftClick) texsubmenu[texgenmenu]=3;
			switch (texsubmenu[texgenmenu])
			{
				case 0 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_sindist;
					if (mode==DD_Draw) sprintf(s,"Number of X-Sines: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
					ValueBar(541,180,796,199,0,10,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,37,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Number of Y-Sines: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
					ValueBar(541,200,796,219,0,10,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,37,13,base,mode);

					if (mode==DD_Draw) sprintf(s,"X Amplitude: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
					ValueBar(541,230,796,249,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,65,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Y Amplitude: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]);
					ValueBar(541,250,796,269,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[5],s,65,13,base,mode);

					if (mode==DD_Draw) sprintf(s,"X Shift: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]);
					ValueBar(541,280,796,299,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[6],s,78,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Y-Shift: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]);
					ValueBar(541,300,796,319,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[7],s,78,13,base,mode);
					break;
				}
				case 1 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_twirl;
					if (mode==DD_Draw) sprintf(s,"Amount: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
					ValueBar(541,180,796,199,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,78 ,13,base,mode);
					break;

				}
				case 2 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_offset;
					if (mode==DD_Draw) sprintf(s,"X Shift: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
					ValueBar(541,180,796,199,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,78,13,base,mode);
					if (mode==DD_Draw) sprintf(s,"Y-Shift: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
					ValueBar(541,200,796,219,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,78,13,base,mode);
					break;
				}
				case 3 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_map;
					if (mode==DD_Draw) {
					glRasterPos2i(542,203);
					glPrint("X-Layer:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,243);
					glPrint(" X-Chan:",base);
					}
					if (button(620,230,659,250,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=0;
					if (button(660,230,716,250,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=1;
					if (button(717,230,762,250,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=2;
					if (mode==DD_Draw) sprintf(s,"X Amplitude: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
					ValueBar(541,260,796,279,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,65,13,base,mode);

					if (mode==DD_Draw) {
					glRasterPos2i(542,313);
					glPrint("Y-Layer:",base);
					}
					if (button(620,290,640,310,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=0;
					if (button(641,290,661,310,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=2;
					if (button(620,311,640,331,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=1;
					if (button(641,311,661,331,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[5]=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,353);
					glPrint(" Y-Chan:",base);
					}
					if (button(620,340,659,360,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=0;
					if (button(660,340,716,360,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=1;
					if (button(717,340,762,360,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[6]=2;
					if (mode==DD_Draw) sprintf(s,"Y Amplitude: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[7]);
					ValueBar(541,370,796,389,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[7],s,65,13,base,mode);
					break;

				}
			
			}
		break;
		}
		case 2 : {
			if (button(540,80,597,100, "Mul"     ,15,13,base,texsubmenu[texgenmenu]==0,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=0;
			if (button(598,80,654,100, "Add"     ,15,13,base,texsubmenu[texgenmenu]==1,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=1;
			if (button(655,80,711,100, "Max"     ,15,13,base,texsubmenu[texgenmenu]==2,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=2;
			if (button(712,80,771,100, "MixMap"  ,04,13,base,texsubmenu[texgenmenu]==3,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=3;
			if (button(540,101,597,121, "Mix"    ,14,13,base,texsubmenu[texgenmenu]==4,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=4;
			if (button(598,101,654,121, "Xchg"   ,10,13,base,texsubmenu[texgenmenu]==5,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=5;
			if (button(655,101,711,121, "Shade"  ,07,13,base,texsubmenu[texgenmenu]==6,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=6;
			if (button(712,101,771,121, "Copy"   ,12,13,base,texsubmenu[texgenmenu]==7,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=7;
			switch (texsubmenu[texgenmenu])
			{
				case 0 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_mul;
					if (mode==DD_Draw) {
					glRasterPos2i(542,203);
					glPrint("Data-Lr:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
					break;
				}
				case 1 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_add;
					if (mode==DD_Draw) {
					glRasterPos2i(542,203);
					glPrint("Data-Lr:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
					break;
				}
				case 2 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_max;
					if (mode==DD_Draw) {
					glRasterPos2i(542,203);
					glPrint("Layer 2:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
					break;
				}
				case 3 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_mixmap;
					if (mode==DD_Draw) {
					glRasterPos2i(542,203);
					glPrint("Layer 2:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer1=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,253);
					glPrint("Data-Lr:",base);
					}
					if (button(620,230,640,250,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2=0;
					if (button(641,230,661,250,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2=2;
					if (button(620,251,640,271,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2=1;
					if (button(641,251,661,271,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,293);
					glPrint("Data-Ch:",base);
					}
					if (button(620,280,659,300,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1=0;
					if (button(660,280,716,300,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1=1;
					if (button(717,280,762,300,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1=2;
					break;
				}
				case 4 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_mix;
					if (mode==DD_Draw) {
					glRasterPos2i(542,203);
					glPrint("Layer 2:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
					if (mode==DD_Draw) sprintf(s,"%%: %f",(texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]/255.0*2.0-1.0));
					ValueBar(541,230,796,249,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,97,13,base,mode);
					break;
				}
				case 5 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_xchng;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint(" Chan 1:",base);
					}
					if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (mode==DD_Draw) {
					glRasterPos2i(542,233);
					glPrint("Layer 2:",base);
					}
					if (button(620,210,640,230,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=0;
					if (button(641,210,661,230,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=2;
					if (button(620,231,640,251,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=1;
					if (button(641,231,661,251,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,273);
					glPrint(" Chan 2:",base);
					}
					if (button(620,260,659,280,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=0;
					if (button(660,260,716,280,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=1;
					if (button(717,260,762,280,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=2;
					break;
				}
				case 6 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_shade;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint("ShadeLr:",base);
					}
					if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,243);
					glPrint("ShadeCh:",base);
					}
					if (button(620,230,659,250,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=0;
					if (button(660,230,716,250,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=1;
					if (button(717,230,762,250,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=2;
					break;
				}
				case 7 : {
					texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_copy;
					if (mode==DD_Draw) {
					glRasterPos2i(542,193);
					glPrint("Channel:",base);
					}
					if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
					if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
					if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
					if (mode==DD_Draw) {
					glRasterPos2i(542,233);
					glPrint(" Src Lr:",base);
					}
					if (button(620,210,640,230,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=0;
					if (button(641,210,661,230,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=2;
					if (button(620,231,640,251,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=1;
					if (button(641,231,661,251,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=3;
					if (mode==DD_Draw) {
					glRasterPos2i(542,273);
					glPrint(" Src Ch:",base);
					}
					if (button(620,260,659,280,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=0;
					if (button(660,260,716,280,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=1;
					if (button(717,260,762,280,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]=2;
					break;
				}
        
		}
	  break; 
      }
	  case 3 : {
		  if (button(540,80,597,100,  "Inv"     ,15,13,base,texsubmenu[texgenmenu]==0,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=0;
		  if (button(598,80,654,100,  "Cntrst"  ,02,13,base,texsubmenu[texgenmenu]==1,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=1;
		  if (button(655,80,711,100,  "Bright"  ,02,13,base,texsubmenu[texgenmenu]==2,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=2;
		  if (button(712,80,771,100,  "Sine"    ,12,13,base,texsubmenu[texgenmenu]==3,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=3;
		  if (button(540,101,597,121, "Scale"   ,07,13,base,texsubmenu[texgenmenu]==4,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=4;
		  if (button(598,101,654,121, "HSV"     ,15,13,base,texsubmenu[texgenmenu]==5,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=5;
		  if (button(655,101,711,121, "Color"   ,07,13,base,texsubmenu[texgenmenu]==6,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=6;
		  if (button(712,101,771,121, "Emboss"  ,04,13,base,texsubmenu[texgenmenu]==7,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=7;
		  switch (texsubmenu[texgenmenu])
		  {
		  case 0 : { texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_invert; break;}
			  case 1 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_contrast;
				  if (mode==DD_Draw) sprintf(s,"Amount: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
				  ValueBar(541,180,796,199,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,78 ,13,base,mode);
				  break;
			  }
			  case 2 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_bright;
				  if (mode==DD_Draw) sprintf(s,"Amount: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]-128);
				  ValueBar(541,180,796,199,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,78 ,13,base,mode);
				  break;
			  }
			  case 3 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_sincol;
				  if (mode==DD_Draw) {
				  glRasterPos2i(542,193);
				  glPrint("Channel:",base);
				  }
				  if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
				  if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
				  if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
				  if (mode==DD_Draw) sprintf(s,"Number of Sines: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
				  ValueBar(541,210,796,229,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,45,13,base,mode);
				  break;
			  }
			  case 4 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_scale;
				  if (mode==DD_Draw) {
				  glRasterPos2i(542,193);
				  glPrint("Channel:",base);
				  }
				  if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
				  if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
				  if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
				  if (mode==DD_Draw) sprintf(s,"Min: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
				  ValueBar(541,210,796,229,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,100,13,base,mode);
				  if (mode==DD_Draw) sprintf(s,"Max: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
				  ValueBar(541,230,796,249,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,100,13,base,mode);
				  break;
			  }
			  case 5 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_hsv;
				  if (mode==DD_Draw) sprintf(s,"Hue: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
				  ValueBar(541,180,796,200,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,100,13,base,mode);
				  if (mode==DD_Draw) sprintf(s,"Saturation: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
				  ValueBar(541,210,796,230,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,73,13,base,mode);
				  break;
			  }
			  case 6 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_colorize;
				  if (mode==DD_Draw) {
				  glRasterPos2i(542,193);
				  glPrint("Data-Ch:",base);
				  }
				  if (button(620,180,659,200,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
				  if (button(660,180,716,200,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
				  if (button(717,180,762,200,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
				  if (mode==DD_Draw) sprintf(s,"Red 1: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]);
				  ValueBar(541,210,796,229,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[3],s,95,13,base,mode);
				  if (mode==DD_Draw) sprintf(s,"Green 1: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].c.param2);
				  ValueBar(541,230,796,249,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].c.param2,s,85,13,base,mode);
				  if (mode==DD_Draw) sprintf(s,"Blue 1: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].c.param4);
				  ValueBar(541,250,796,269,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].c.param4,s,91,13,base,mode);

				  if (mode==DD_Draw) {
				  glBegin(GL_QUADS);
				  glColor3f((float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2/255.0),
					        (float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param2/255.0),
							(float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param4/255.0));
				  glVertex2f(541,280);
				  glColor3f((float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1/255.0),
					        (float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param3/255),
							(float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param5/255));
				  glVertex2f(796,280);
				  glVertex2f(796,300);
				  glColor3f((float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.datalayer2/255.0),
					        (float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param2/255),
							(float)((float)texdata[texgenmenu][texsubmenu[texgenmenu]].c.param4/255));
				  glVertex2f(541,300);
				  glEnd();
				  glColor4f(1,1,1,1);
				  }

				  if (mode==DD_Draw) sprintf(s,"Red 2: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1);
				  ValueBar(541,310,796,329,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].c.param1,s,95,13,base,mode);
				  if (mode==DD_Draw) sprintf(s,"Green 2: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].c.param3);
				  ValueBar(541,330,796,349,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].c.param3,s,85,13,base,mode);
				  if (mode==DD_Draw) sprintf(s,"Blue 2: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].c.param5);
				  ValueBar(541,350,796,369,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].c.param5,s,91,13,base,mode);
				  break;

			  }
			  case 7 : { texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_emboss; break;}
		  }
		break;
      }
	  case 4 : {
		  if (button(540+38,80,617+38,100, "Normal" ,13,13,base,texsubmenu[texgenmenu]==0,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=0;
		  if (button(618+38,80,694+38,100, "Dir"  ,25,13,base,texsubmenu[texgenmenu]==1,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=1;
		  switch (texsubmenu[texgenmenu])
		  {
			  case 0 : {
				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_blur;
				  if (mode==DD_Draw) sprintf(s,"Amount: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]);
				  ValueBar(541,180,796,199,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[2],s,78 ,13,base,mode);
				  break;
			  }
			  case 1 : {

				  texdata[texgenmenu][texsubmenu[texgenmenu]].c.commandnumber=DD_dirblur;
				  if (mode==DD_Draw) {
				  glRasterPos2i(542,193);
				  glPrint("Data Lr:",base);
				  }
				  if (button(620,180,640,200,"0",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=0;
				  if (button(641,180,661,200,"2",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=2;
				  if (button(620,201,640,221,"1",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=1;
				  if (button(641,201,661,221,"3",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]==3,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[2]=3;
				  if (mode==DD_Draw) {
				  glRasterPos2i(542,243);
				  glPrint("Data Ch:",base);
				  }
				  if (button(620,230,659,250,"Red",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==0,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=0;
				  if (button(660,230,716,250,"Green",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==1,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=1;
				  if (button(717,230,762,250,"Blue",6,13,base,texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]==2,mode) == DDgui_LeftClick)  texdata[texgenmenu][texsubmenu[texgenmenu]].command[3]=2;
				  if (mode==DD_Draw) sprintf(s,"Distance: %d",texdata[texgenmenu][texsubmenu[texgenmenu]].command[4]);
				  ValueBar(541,260,796,279,0,255,&texdata[texgenmenu][texsubmenu[texgenmenu]].command[4],s,74 ,13,base,mode);
				  break;
			  }
		  }
		  break;
      }
	  case 5 : {
		  if (button(540,80,597,100, "New"   ,16,13,base,texsubmenu[texgenmenu]==0,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=0;
		  if (button(598,80,655,100, "Del"   ,16,13,base,texsubmenu[texgenmenu]==1,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=1;
		  if (button(656,80,713,100, "Clr"   ,16,13,base,texsubmenu[texgenmenu]==2,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=2;
		  if (button(714,80,771,100, "Edit"  ,12,13,base,texsubmenu[texgenmenu]==6,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=6;
		  
		  if (button(540,101,597,121,"Import",2,13,base,texsubmenu[texgenmenu]==3,mode) == DDgui_LeftClick)  { texsubmenu[texgenmenu]=3; atglist=findfile(texturemask); }
		  if (button(598,101,655,121,"Export",3,13,base,texsubmenu[texgenmenu]==4,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=4;
		  if (button(656,101,713,121,"Select",3,13,base,texsubmenu[texgenmenu]==5,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=5;
		  if (button(714,101,771,120,"Matrl.",4,13,base,texsubmenu[texgenmenu]==7,mode) == DDgui_LeftClick)  texsubmenu[texgenmenu]=7;

		  switch (texsubmenu[texgenmenu])
		  {
			  case 0 : {
				  if (button(618,130,694,150,"New",24,13,base,false,mode) == DDgui_LeftClick) 
				  {
					  char *ss=readline("Enter Name",300,0,base,"");
					  if (ss!="")
					  {
						  memcpy(actualtexture->commands,generatedtexture.commands,sizeof(generatedtexture.commands));
						  memcpy(actualtexture->texts,generatedtexture.texts,sizeof(generatedtexture.texts));
						  actualtexture->commandnum=generatedtexture.commandnum;
						  memcpy(actualtexture->layers,generatedtexture.layers,sizeof(generatedtexture.layers));
						  tTexture *newtexture=new tTexture[1];
						  memset(newtexture,0,sizeof(tTexture));
						  sprintf(newtexture->name,"%s",ss);
						  newtexture->next=NULL;
						  newtexture->number=lasttexture->number+1;
						  lasttexture->next=newtexture;
						  lasttexture=newtexture;
						  actualtexture=newtexture;
						  memset(&generatedtexture,0,sizeof(generatedtexture));
						  for (y=0;y<=3;y++)
						  {
							  glBindTexture(GL_TEXTURE_2D, texlayers[y]);
							  glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
						  }
						  selectedtexture=texturenum(texturelist)-1;
					  }
				  }
				  break;
			  }
			  case 1 : {
				  break;
			  }
			  case 2 : {
				  break;
			  }
			  case 3 : {
				  if (mode==DD_Draw)
				  {
					  glColor4f(buttontextlit);
					  rectangle(540,130,655,278);
				  }	  
				  if (mode==DD_Check)
				  if (leftclickinwindow(541,131,654,270) && mouseinwindow(541,131,654,270))  atgselected=min(atgbarpos+(my-131) / 14,filenum(atglist)-1);
				  scroller(656,130,671,278,15,15,filenum(atglist),10,atgbarpos,mode);
				  if (mode==DD_Draw)
				  {
					  pf = atglist;
					  for (x=1;x<=atgbarpos;x++) pf=pf->next;
					  for (x=0;x<=9;x++)
					  {
						  if (pf!=NULL)
						  {
							  if (atgbarpos+x==atgselected) glColor4f(col4); else glColor4f(buttontextlit);
							  glRasterPos2i(543,144+x*14);
							  sprintf(st,"%s",pf->filedata.cFileName);
							  glPrint(st,base,12);
							  pf=pf->next;
						  }
					  }

					  glColor4f(buttontextlit);
					  rectangle(800-128-11,600-128-44-12,800-11,600-44-12);
					  rectangle(800-128-11-128,600-128-44-12-128,800-11-128,600-44-12-128);
					  rectangle(800-128-11-128,600-128-44-12-128,800-11,600-44-12);
					  glColor4f(1,1,1,1);
					  sprintf(s,"%d Textures.",filenum(atglist));
					  glRasterPos2i(683,143);
					  glPrint(s,base);

					  glEnable(GL_TEXTURE_2D);
					  drawpretextures();
					  glDisable(GL_TEXTURE_2D);
				  }

				  if ((button(686,181,771,201,"Import",17,13,base,false,mode) == DDgui_LeftClick))
				  {
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

					  pf = atglist;
					  for (x=1;x<=atgselected;x++) pf=pf->next;
					  sprintf(s,"%s%s",texturedir,pf->filedata.cFileName);
					  f=fopen(s,"r+b");
					  					  
					  fread(s,3,1,f);
					  fread(&b,1,1,f);
					  				  
					  generatedtexture.commandnum=0;
					  memset(generatedtexture.commands,0,sizeof(generatedtexture.commands));
					  for (x=0;x<100;x++)
					  {
						  memset(generatedtexture.texts[x].text,0,256);
					  }
					  memset(generatedtexture.layers,0,sizeof(generatedtexture.layers));
					  
					  for (x=0;x<b;x++)
					  {
						  fread(&cmd,9,1,f);
						  if (cmd.c.commandnumber==DD_text) 
						  {
							  fread(&k,1,1,f);
							  fread(generatedtexture.texts[cmd.command[8]].text,k,1,f);
							  generatedtexture.texts[cmd.command[8]].text[k]='\0';
						  }
						  generatedtexture.commands[generatedtexture.commandnum]=cmd;
						  generatedtexture.commandnum++;
					  }
					  fclose(f);
					  
					  for (y=0;y<=3;y++)
					  {
						  glBindTexture(GL_TEXTURE_2D, texlayers[y]);
						  glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
					  }
					  for (x=0;x<generatedtexture.commandnum;x++)
					  {
						  generatedtexture.performcommand(generatedtexture.commands[x]);
						  for (y=0;y<=3;y++)
						  {
							  glBindTexture(GL_TEXTURE_2D, texlayers[y]);
							  glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
						  }
						  for (y=0;y<=2;y++)
						  {
							  glEnable(GL_TEXTURE_2D);
							  drawtextures();
							  glDisable(GL_TEXTURE_2D);
							  glColor4f(buttontextlit);
							  rectangle(0+12,540+13,523,560+13);
							  glColor4f(col4);
							  quad(0+12,540+13,(int)((float)(x+1)/((float)(generatedtexture.commandnum))*511.0+12.0),560+13);
							  glColor4f(1,1,1,1);
						  }
						  SwapBuffers(hDC);
					  }
					  memcpy(actualtexture->commands,generatedtexture.commands,sizeof(generatedtexture.commands));
					  memcpy(actualtexture->texts,generatedtexture.texts,sizeof(generatedtexture.texts));
					  actualtexture->commandnum=generatedtexture.commandnum;
					  memcpy(actualtexture->layers,generatedtexture.layers,sizeof(generatedtexture.layers));
					  waitleftbutton=true;					  
				  }
				  if (button(686,202,771,222,"Preview",12,13,base,false,mode) == DDgui_LeftClick) 
				  {
					  glColor4f(1,1,1,1);
					  glEnable(GL_TEXTURE_2D);
					  drawtextures();
					  glDisable(GL_TEXTURE_2D);
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

					  pf = atglist;
					  for (x=1;x<=atgselected;x++) pf=pf->next;
					  sprintf(s,"%s%s",texturedir,pf->filedata.cFileName);
					  f=fopen(s,"r+b");
					  fread(s,3,1,f);
					  fread(&b,1,1,f);
					  
					  buffertexture.commandnum=0;
					  memset(buffertexture.commands,0,sizeof(buffertexture.commands));
					  for (x=0;x<100;x++)
					  {
						  memset(buffertexture.texts[x].text,0,256);
					  }
					  memset(buffertexture.layers,0,sizeof(buffertexture.layers));

					  for (x=0;x<b;x++)
					  {
						  fread(&cmd,9,1,f);
						  if (cmd.c.commandnumber==DD_text) 
						  {
							  fread(&k,1,1,f);

							  fread(buffertexture.texts[cmd.command[8]].text,k,1,f);
							  buffertexture.texts[cmd.command[8]].text[k]='\0';
						  }
						  buffertexture.commands[buffertexture.commandnum]=cmd;
						  buffertexture.commandnum++;
					  }
					  fclose(f);

					  for (y=0;y<=3;y++)
					  {
						  glBindTexture(GL_TEXTURE_2D, prelayers[y]);
						  glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,buffertexture.layers[y]);
					  }
					  for (x=0;x<buffertexture.commandnum;x++)
					  {
						  buffertexture.performcommand(buffertexture.commands[x]);
						  for (y=0;y<=3;y++)
						  {
							  glBindTexture(GL_TEXTURE_2D, prelayers[y]);
							  glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,buffertexture.layers[y]);
						  }
						  for (y=0;y<=2;y++)
						  {
							  glEnable(GL_TEXTURE_2D);
							  drawpretextures();
							  glDisable(GL_TEXTURE_2D);
							  glColor4f(buttontextlit);
							  rectangle(0+12,540+13,523,560+13);
							  glColor4f(col4);
							  quad(0+12,540+13,(int)((float)(x+1)/((float)(buffertexture.commandnum))*511.0+12.0),560+13);
							  glColor4f(1,1,1,1);
						  }
						  SwapBuffers(hDC);
					  }
					  waitleftbutton=true;					  

				  }/**/
				  break;

            }
			case 4 : {
				if (button(618,131,694,151,"Export",13,13,base,false,mode) == DDgui_LeftClick) 
				{
					char *ss=readline("Enter Filename (.atg not needed)",210,0,base,"");
					if (ss!="")
					{
						sprintf(s,"%s%s.atg",texturedir,ss);
						f=fopen(s,"w+b");
						fwrite("ATG",3,1,f);
						b=generatedtexture.commandnum;
						fwrite(&b,1,1,f);
						for (x=0;x<b;x++)
						{
							fwrite(&(generatedtexture.commands[x]),1,9,f);

							if (generatedtexture.commands[x].c.commandnumber==DD_text)
							{
								bb=strlen(generatedtexture.texts[generatedtexture.commands[x].command[8]].text);
								fwrite(&bb,1,1,f);
								fwrite(generatedtexture.texts[generatedtexture.commands[x].command[8]].text,bb,1,f);
							}
								
						}
						fclose(f);
					}
				}
				break;
            }
			case 5 : {
				glColor4f(buttontextlit);
				if (mode==DD_Draw)
				rectangle(540,130,655,278);
				if (leftclickinwindow(541,131,654,270) && mouseinwindow(541,131,654,270) && (mode==DD_Check))
				{
					memcpy(actualtexture->commands,generatedtexture.commands,sizeof(generatedtexture.commands));
					memcpy(actualtexture->texts,generatedtexture.texts,sizeof(generatedtexture.texts));
					actualtexture->commandnum=generatedtexture.commandnum;
					memcpy(actualtexture->layers,generatedtexture.layers,sizeof(generatedtexture.layers));
					selectedtexture=min(texturestart+(my-131) / 14,texturenum(texturelist)-1);
					pt = texturelist;
					for (x=1;x<=selectedtexture;x++) pt=pt->next;
					if (pt!=NULL)
					{
						actualtexture=pt;
						memcpy(generatedtexture.commands,actualtexture->commands,sizeof(actualtexture->commands));
						generatedtexture.commandnum=actualtexture->commandnum;
						memcpy(generatedtexture.texts,actualtexture->texts,sizeof(actualtexture->texts));
						memcpy(generatedtexture.layers,actualtexture->layers,sizeof(actualtexture->layers));
						for (y=0;y<=3;y++)
						{
							glBindTexture(GL_TEXTURE_2D, texlayers[y]);
							glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[y]);
						}
					}
				}
				scroller(656,130,671,278,15,15,texturenum(texturelist),10,texturestart,mode);

				pt = texturelist;
				for (x=0;x<=texturestart-1;x++) pt=pt->next;
				for (x=0;x<=9;x++)
				{
					if (pt!=NULL)
					{
						if (texturestart+x==selectedtexture) glColor4f(col4); else glColor4f(buttontextlit);
						glRasterPos2i(543,144+x*14);
						sprintf(st,"%2d:%s",pt->number,pt->name);
						glPrint(st,base,12);
						pt=pt->next;
					}
				}
				glColor4f(1,1,1,1);
				sprintf(st,"%d Textures.",texturenum(texturelist));
				glRasterPos2i(683,143);
				glPrint(st,base);
				break;
					 }/**/
			case 6 : {// edit
				glColor4f(buttontextlit);
				rectangle(540,130,755,278);
				if (mode==DD_Check)
				{
					if (leftclickinwindow(541,131,754,270) && mouseinwindow(541,131,754,270)) commandselected=min((int)((float)commandpos+(float)(my-131) / 14.0),generatedtexture.commandnum-1);
				}
				scroller(756,130,771,278,15,15,generatedtexture.commandnum,10,commandpos,mode);
				for (x=commandpos;x<=commandpos+9;x++)
				if (x<generatedtexture.commandnum)
				{
					if (x==commandselected) glColor4f(col4); else glColor4f(buttontextlit);
					memset(s,0,256);
					sprintf(s,"%2d: %s%d",x,commandnames[generatedtexture.commands[x].c.commandnumber],generatedtexture.commands[x].c.layer);
					glRasterPos2i(544,144+(x-commandpos)*14);
					glPrint(s,base);
				}
				glColor4f(1,1,1,1);
				break;
					 }
			case 7 : {// make material
				if (button(620,130,640,150,"0",6,13,base,matlayer==0,mode) == DDgui_LeftClick) matlayer=0;
				if (button(641,130,661,150,"2",6,13,base,matlayer==2,mode) == DDgui_LeftClick) matlayer=2;
				if (button(620,151,640,171,"1",6,13,base,matlayer==1,mode) == DDgui_LeftClick) matlayer=1;
				if (button(641,151,661,171,"3",6,13,base,matlayer==3,mode) == DDgui_LeftClick) matlayer=3;
				if (button(611,180,700,200,"Set Alpha",5,13,base,matalpha,mode) == DDgui_LeftClick && mode==DD_Check) {matalpha=!matalpha; waitleftbutton=true;}
				if (matalpha)
				{
					if (button(620,130+80,640,150+80,"0",6,13,base,alphalayer==0,mode) == DDgui_LeftClick) alphalayer=0;
					if (button(641,130+80,661,150+80,"2",6,13,base,alphalayer==2,mode) == DDgui_LeftClick) alphalayer=2;
					if (button(620,151+80,640,171+80,"1",6,13,base,alphalayer==1,mode) == DDgui_LeftClick) alphalayer=1;
					if (button(641,151+80,661,171+80,"3",6,13,base,alphalayer==3,mode) == DDgui_LeftClick) alphalayer=3;
					if (button(540,260,590,280,"Red",6,13,base,alphamode==0,mode) == DDgui_LeftClick) alphamode=0;
					if (button(591,260,660,280,"Green",6,13,base,alphamode==1,mode) == DDgui_LeftClick) alphamode=1;
					if (button(661,260,710,280,"Blue",6,13,base,alphamode==2,mode) == DDgui_LeftClick) alphamode=2;
					if (button(711,260,770,280,"Mix",6,13,base,alphamode==3,mode) == DDgui_LeftClick) alphamode=3;
					glColor4f(buttontextlit);
					if (mode==DD_Draw)
						rectangle(540,130+160,655,278+160);
					if (leftclickinwindow(541,131+160,654,270+160) && mouseinwindow(541,131+160,654,270+160) && (mode==DD_Check))
					{
						//memcpy(actualtexture->commands,generatedtexture.commands,sizeof(generatedtexture.commands));
						//memcpy(actualtexture->texts,generatedtexture.texts,sizeof(generatedtexture.texts));
						//actualtexture->commandnum=generatedtexture.commandnum;
						//memcpy(actualtexture->layers,generatedtexture.layers,sizeof(generatedtexture.layers));
						alphaselected=min(alphastart+(my-131-160) / 14,texturenum(texturelist)-1);
						pt = texturelist;
						for (x=1;x<=alphaselected;x++) pt=pt->next;
						if (pt!=NULL)
						{
							//actualtexture=pt;
							//memcpy(generatedtexture.commands,actualtexture->commands,sizeof(actualtexture->commands));
							//generatedtexture.commandnum=actualtexture->commandnum;
							//memcpy(generatedtexture.texts,actualtexture->texts,sizeof(actualtexture->texts));
							//memcpy(generatedtexture.layers,actualtexture->layers,sizeof(actualtexture->layers));
							for (y=0;y<=3;y++)
							{
								glBindTexture(GL_TEXTURE_2D, alayers[y]);
								glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,pt->layers[y]);
							}
						}

					}
					if (mode==DD_Draw)
					{
						glPushAttrib(GL_ALL_ATTRIB_BITS);
						glColor4f(1,1,1,1);
						glBindTexture(GL_TEXTURE_2D, alayers[0]);
						glEnable(GL_TEXTURE_2D);
						glBegin(GL_QUADS);
							glTexCoord2i(0,0);
							glVertex2i(673,290);
							glTexCoord2i(0,1);
							glVertex2i(733,290);
							glTexCoord2i(1,1);
							glVertex2i(733,350);
							glTexCoord2i(1,0);
							glVertex2i(673,350);
						glEnd();
						glBindTexture(GL_TEXTURE_2D, alayers[1]);
						glEnable(GL_TEXTURE_2D);
						glBegin(GL_QUADS);
							glTexCoord2i(0,0);
							glVertex2i(673,290+60);
							glTexCoord2i(0,1);
							glVertex2i(733,290+60);
							glTexCoord2i(1,1);
							glVertex2i(733,350+60);
							glTexCoord2i(1,0);
							glVertex2i(673,350+60);
						glEnd();
						glBindTexture(GL_TEXTURE_2D, alayers[2]);
						glEnable(GL_TEXTURE_2D);
						glBegin(GL_QUADS);
							glTexCoord2i(0,0);
							glVertex2i(673+60,290);
							glTexCoord2i(0,1);
							glVertex2i(733+60,290);
							glTexCoord2i(1,1);
							glVertex2i(733+60,350);
							glTexCoord2i(1,0);
							glVertex2i(673+60,350);
						glEnd();
						glBindTexture(GL_TEXTURE_2D, alayers[3]);
						glEnable(GL_TEXTURE_2D);
						glBegin(GL_QUADS);
							glTexCoord2i(0,0);
							glVertex2i(673+60,290+60);
							glTexCoord2i(0,1);
							glVertex2i(733+60,290+60);
							glTexCoord2i(1,1);
							glVertex2i(733+60,350+60);
							glTexCoord2i(1,0);
							glVertex2i(673+60,350+60);
						glEnd();
						glPopAttrib();
					}
					scroller(656,130+160,671,278+160,15,15,texturenum(texturelist),10,alphastart,mode);

					pt = texturelist;
					for (x=0;x<=alphastart-1;x++) pt=pt->next;
					for (x=0;x<=9;x++)
					{
						if (pt!=NULL)
						{
							if (texturestart+x==alphaselected) glColor4f(col4); else glColor4f(buttontextlit);
							glRasterPos2i(543,144+x*14+160);
							sprintf(st,"%2d:%s",pt->number,pt->name);
							glPrint(st,base,12);
							pt=pt->next;
						}
					}
					glColor4f(1,1,1,1);
				}
				if (button(540,511+12,665,531+12,"Make Material",6,13,base,false,mode) == DDgui_LeftClick)
				{
					material *newmat = new material[1];
					memset(newmat,0,sizeof(newmat));
					newmat->layer=matlayer;
					newmat->texture=actualtexture->number;
					newmat->alpha=alphaselected;
					newmat->alphalayer=alphalayer;
					newmat->alphamode=alphamode;
					sprintf(newmat->name,"%s%d",actualtexture->name,matlayer);
					newmat->next=NULL;
					glGenTextures(1,&newmat->handle);
					glBindTexture(GL_TEXTURE_2D,newmat->handle);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
					rgba layer[256][256];
					memset(layer,0,256*256*4);
					memcpy(layer,generatedtexture.layers[matlayer],256*256*4);
					if (matalpha)
					{
						pt = texturelist;
						for (x=0;x<=alphaselected-1;x++) pt=pt->next;
					}
					for (x=0;x<256;x++)
						for (y=0;y<256;y++)
						{
							if (!matalpha)
							layer[x][y].cols[3]=255;
							else
							{
								switch (alphamode)
								{
								case 0:
								case 1:
								case 2:{
									layer[x][y].cols[3]=pt->layers[alphalayer][x][y].cols[alphamode];
									break;
										}
								case 3:{
									layer[x][y].cols[3]=(pt->layers[alphalayer][x][y].cols[0]+pt->layers[alphalayer][x][y].cols[1]+pt->layers[alphalayer][x][y].cols[2])/3;
									   }
								}
							}
						}
					glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,layer);
					if (lastmaterial==NULL)
					{
						lastmaterial=newmat;
						materiallist=newmat;
						newmat->number=0;
					}
					else
					{
						newmat->number=lastmaterial->number+1;
						lastmaterial->next=newmat;
						lastmaterial=newmat;						
					}
					sprintf(defaulttooltip,"Material Created.");
					waitleftbutton=true;
				}

				break;
					 }
       }
      }
 }
}