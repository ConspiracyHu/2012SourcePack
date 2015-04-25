#include "aDDict.h"
#define fullscr false

//inner truth

	char *s2s = new char[256];
/*	sprintf(ss,"%d",texgenmenu);
	MessageBox( 0, ss, "HelloWorld", MB_OK );*/


bool aCid=false;
int music_len,music_pos;

unsigned int memopen(char *name) { return 1; }

void memclose(unsigned int handle) {}

int memread(void *buffer, int size, unsigned int handle)
{
	if (music_pos + size >= music_len) size = music_len - music_pos;

	memcpy(buffer, (char *)music_data+music_pos, size);
	music_pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	if (mode == SEEK_SET) 		 music_pos = pos;
	else if (mode == SEEK_CUR) music_pos += pos;
	else if (mode == SEEK_END) music_pos = music_len + pos;
	if (music_pos > music_len) music_pos = music_len;
}

int memtell(unsigned int handle) { return music_pos; }

char lastfilename[256];

GLuint cubetex,mainmenutex,texbutton1,texbutton2,texbutton3,texts1;
char musfilename[40];
char *music_data;
bool musicloaded=false;
FMUSIC_MODULE *mod;

bool fulls = fullscr;
scene *scenelist=newscene();
scene *lastscene=scenelist;

char tooltip[256];
char defaulttooltip[256];
material *materiallist=NULL,*lastmaterial=NULL;

GLuint texlayers[4],prelayers[4];
GLuint pretextures[6][8];
GLuint background;
int mainmenu = 0;
int texgenmenu=0;
int texsubmenu[6];
texturecommand texdata[6][8];
int x,y,z = 0;
bool waitleftbutton=false;
bool waitrightbutton=false;
texture generatedtexture,buffertexture;
bool randomed = false;
tfilelist *atglist=NULL;
tfilelist *scnlist=NULL,*prjlist=NULL,*xmlist=NULL,*synclist=NULL;
tTexture *actualtexture=NULL,*texturelist=new tTexture[1],*lasttexture;
int atgbarpos=0;
int atgselected=0;
int selbarpos=0;
int selselected=0;
int scnbarpos=0;
int scnselected=0;
tfilelist *pf=NULL;
char *st=new char[256];
FILE *f;
byte b;
byte k;
texturecommand cmd;
int commandselected=0;
int commandpos=0;
char *commandnames[256];
scene *actualscene=scenelist;
int modellermenu=0;
bool xformmenu=true;
bool createmenu=true;
int transformation=0;
bool centertransform=false;
bool transform=false;
object *o;
int selectedtexture=0, texturestart=0;
unsigned int mattexture;

int selectstart=0;
int lofttrack=0,loftmesh=0;
int loftstart=0,loftcounter=0,tracknumber=0,trackstart=0,trackcounter=0;
int modellsubmenu[8];

int matstart=0,matselected=0;

byte spherex=12,spherey=12;
byte gridx=10,gridy=10;
byte tubex=12,tubey=12;
bool tubetop=1;
byte conex=12;
bool conetop=1;
byte arcx=12;
int arcy=360;
byte linex=10;

byte *backraw;

int objnumber;

bool backdraw=false;
char *cursorstring = new char[256];
bool scntexturesave=true, scncamerasave=false, scnselectionsave=true, scnlightsave=false, scnobjectsave=true, scnenvironmentsave=false;


void init()
{
	memset(lastfilename,0,256);
	memset(musfilename,0,40);
	initGUI();
	sprintf(scenelist->name,"Default");
	memset(tooltip,0,256);
	memset(defaulttooltip,0,256);
	sprintf(defaulttooltip,"a.D.D.i.c.t. (c) BoyC 2002");
	actualtexture=texturelist;
	lasttexture=texturelist;
	memset(texturelist,0,sizeof(tTexture));
	texturelist->next=NULL;
	sprintf(texturelist->name,"Default");
	memset(cursorstring,0,256);
	for (x=0;x<=7;x++) modellsubmenu[x]=0;
	init3dengine();
	InitModellerGUI();
	BuildFont();
	BuildFont2();
	BuildFontSmall();
	inittextureengine();
	generatedtexture.init();
	buffertexture.init();
	memset(pretextures,0,sizeof(pretextures));
	memset(texdata,0,sizeof(texdata));
	memset(texsubmenu,0,sizeof(texsubmenu));
	texdata[0][3].command[3]=4;
	texdata[0][5].command[3]=14;
	texdata[0][5].command[4]=100;
	texdata[0][6].command[3]=30;
	texdata[1][1].command[2]=128;
	texdata[2][4].command[3]=128;
	texdata[3][2].command[2]=128;
	texdata[3][4].command[4]=255;
	texdata[3][6].c.param1=255;
	texdata[3][6].c.param3=255;
	texdata[3][6].c.param5=255;
	glGenTextures(1,&mattexture);
	glBindTexture(GL_TEXTURE_2D,mattexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[0]);
	for (x=0;x<4;x++)
	{
		glGenTextures(1,&(prelayers[x]));
		glBindTexture(GL_TEXTURE_2D,prelayers[x]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[x]);
	}
	for (x=0;x<4;x++)
	{
		glGenTextures(1,&(texlayers[x]));
		glBindTexture(GL_TEXTURE_2D,texlayers[x]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[x]);
	}
	for (x=0;x<4;x++)
	{
		glGenTextures(1,&alayers[x]);
		glBindTexture(GL_TEXTURE_2D,alayers[x]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,generatedtexture.layers[x]);
	}
	glGenTextures(1,&background);
	glBindTexture(GL_TEXTURE_2D, background);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	commandnames[DD_fractal]     ="Fractal    ";
	commandnames[DD_plasma]      ="Plasma     ";
	commandnames[DD_cells]       ="Cells      ";
	commandnames[DD_envmap]      ="Envmap     ";
	commandnames[DD_subplasm]    ="Subplasma  ";
	commandnames[DD_clear]       ="Clear      ";
	commandnames[DD_text]        ="Text       ";
	commandnames[DD_sindist]     ="SinDist    ";
	commandnames[DD_offset]      ="Offset     ";
	commandnames[DD_twirl]       ="Twirl      ";
	commandnames[DD_blur]        ="Blur       ";
	commandnames[DD_map]         ="MapDist    ";
	commandnames[DD_dirblur]     ="DirBlur    ";
	commandnames[DD_xchng]       ="Xchange    ";
	commandnames[DD_copy]        ="Copy       ";
	commandnames[DD_mix]         ="Mix        ";
	commandnames[DD_mul]         ="Mul        ";
	commandnames[DD_add]         ="Add        ";
	commandnames[DD_max]         ="Max        ";
	commandnames[DD_contrast]    ="Contrast   ";
	commandnames[DD_invert]      ="Invert     ";
	commandnames[DD_shade]       ="Shade      ";
	commandnames[DD_bright]      ="Brighten   ";
	commandnames[DD_sincol]      ="Color Sine ";
	commandnames[DD_scale]       ="Scale      ";
	commandnames[DD_hsv]         ="HSV        ";
	commandnames[DD_colorize]    ="Colorize   ";
	commandnames[DD_mixmap]      ="MixMap     ";
	commandnames[DD_emboss]      ="Emboss     ";
	commandnames[DD_stored]      ="Stored     ";

	initkeyframergui();
	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
	FSOUND_Init(44100,32);
}

void menu(byte mode)
{
	if (button(12,1,12+107,1+23,mainmenutex,0,0,107.0/512.0,23.0/128.0,mainmenu==0,mode)			== DDgui_LeftClick) mainmenu=0;
	if (button(119,1,183,1+23,mainmenutex,0,23.0/128.0,63.0/512.0,46.0/128.0,mainmenu==1,mode)== DDgui_LeftClick) mainmenu=1;
	if (button(183,1,261,1+23,mainmenutex,0,46.0/128.0,78.0/512.0,69.0/128.0,mainmenu==2,mode)		== DDgui_LeftClick) 
	{
		if (materiallist!=NULL)
		{
			material *pt=materiallist;
			for (x=1;x<=matselected;x++) pt=pt->next;
			mattexture=pt->handle;
		}
		mainmenu=2;
	}
	if (button(261,1,345,1+23,mainmenutex,0,69.0/128.0,84.0/512.0,92.0/128.0,mainmenu==3,mode)		== DDgui_LeftClick) mainmenu=3;
	if (button(345,1,396,1+23,mainmenutex,0,92.0/128.0,51.0/512.0,115.0/128.0,mainmenu==4,mode)			== DDgui_LeftClick) { mainmenu=4; prjlist=findfile(projectmask);}

	switch (mainmenu) {
	case 0 : {introeditor(mode); break;}
	case 1 : {TextureGeneratorGUI(mode); break;}
	case 2 : {ModellerGUI(mode); break;}
	case 3 : {keyframergui(mode); break;}
	case 4 : {filemenu(mode); break;}
	}
}

void mainloop()
{
	while (!done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
		else if (keys[VK_ESCAPE]) done=true;
		else
		{
			memcpy(tooltip,defaulttooltip,256);
			glClear(0x4100);
			switchto2d();

			/*glPushAttrib(GL_ALL_ATTRIB_BITS);			
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glColor4d( 40.0/255.0,
				       75.0/255.0,
					  128.0/255.0,1.0);
			glVertex2i(0,0);
			glVertex2i(800,0);
			glColor4f(backgroundcol);
			glVertex2i(800,600);
			glVertex2i(0,600);
			glEnd();
			glPopAttrib();*/
			glBindTexture(GL_TEXTURE_2D,cubetex);

			/*glEnable(GL_TEXTURE_2D); //145*32 addict logo
			glColor4f(1,1,1,1);
			glBegin(GL_QUADS);
			glTexCoord2d(0,(255.0-31.0)/256.0);
			glVertex2i(800-144,0);
			glTexCoord2d(144.0/256.0,(255.0-31.0)/256.0);
			glVertex2i(800,0);
			glTexCoord2d(144.0/256.0,1.0);
			glVertex2i(800,31);
			glTexCoord2d(0,1.0);
			glVertex2i(800-144,31);
			glEnd();
			glDisable(GL_TEXTURE_2D);*/

			menu(DD_Check);
			if (keys[46] && (mainmenu==2)) { delete_selected(); keys[46]=false; }
			if (keys[65] && (mainmenu==2)) { { o=actualscene->objects; while (o!=NULL)  { if (!o->effect) o->selected=true; o=o->next; } } keys[65]=false; }
			if (keys[78] && (mainmenu==2)) { { o=actualscene->objects; while (o!=NULL)  { o->selected=false; o=o->next; } } keys[78]=false; }
			if (keys[73] && (mainmenu==2)) { { o=actualscene->objects; while (o!=NULL)  { if (!o->effect) o->selected=!o->selected; o=o->next; } } keys[73]=false; }
			if (keys[77] && (mainmenu==2)) { for (int x=0; x<4; x++) modellviews[x].transformation=1; keys[77]=false; }
			if (keys[82] && (mainmenu==2)) { for (int x=0; x<4; x++) modellviews[x].transformation=2; keys[82]=false; }
			if (keys[83] && (mainmenu==2)) { for (int x=0; x<4; x++) modellviews[x].transformation=3; keys[83]=false; }
			if (keys[79] && (mainmenu==2)) { for (int x=0; x<4; x++) modellviews[x].centertransform=!modellviews[x].centertransform; keys[79]=false; }
			//if (keys[79] && (mainmenu==2)) { sprintf(s2s,"eye: %.3f %.3f %.3f\n dest: %.3f %.3f %.3f",modellviews[3].cam2.eye.b.x,modellviews[3].cam2.eye.b.y,modellviews[3].cam2.eye.b.z,modellviews[3].cam2.target.b.x,modellviews[3].cam2.target.b.y,modellviews[3].cam2.target.b.z); MessageBox( 0, s2s, "HelloWorld", MB_OK ); keys[79]=false; }
			menu(DD_Draw);
			
			if ((strlen(cursorstring)!=0)) //&& (leftbutton || rightbutton))
			{

				glColor4f(1,1,1,1);
				if (leftbutton)
				{
					line(lx-5,ly,lx+5,ly);
					line(lx,ly-5,lx,ly+5);
				}
				glDisable(GL_DEPTH_TEST);
				glRasterPos2i(mx+15,my+15);
				glPrint(cursorstring,base2);
				glEnable(GL_DEPTH_TEST);

				memset(cursorstring,0,256);
			}

			glColor4f(buttontextlit);
			/*glRasterPos2i(495,14);
			glPrint(tooltip,base);*/
			glColor4f(1,1,1,1);
			SwapBuffers(hDC);
			menu(DD_AfterCheck);
			if (waitleftbutton) {waitforleftbuttonrelease(); waitleftbutton=false;}
			if (waitrightbutton) {waitforrightbuttonrelease(); waitrightbutton=false;}
			if (keys[VK_ESCAPE]) {done=false; keys[VK_ESCAPE]=false;}

			if (leftbutton || rightbutton || middlebutton) SetCapture(hWnd); else ReleaseCapture();
			wheel=0;
			Sleep(10);
		}

	}
}

void checkini()
{
	FILE *inifile=fopen("addict.ini","rt");
	if (inifile!=NULL)
	{
		int a;
		if (fscanf(inifile,"%d",&a)!=0)
		{
			if (a) fulls=true; else fulls=false;
		}
		if (fscanf(inifile,"%d",&a)==1)
		{
			if (a) aCid=true; else aCid=false;
		}
		fclose(inifile);
	}
	/*FILE *a,*b,*c;
	a=fopen("fmodsux\\dll.txt","rt");
	b=fopen("fmodsux\\mini.txt","rt");
	c=fopen("fmodsux\\dif.txt","w+t");
	for (int x=0; x<106; x++)
	{
		int i,j;
		fscanf(a,"%d",&i); //dll
		fscanf(b,"%d",&j); //mini
		j=j-19;
		fprintf(c,"Dif: %d Mult: %.10f\n",i-j,(float)i/(float)j);
	}*/
}

INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
	checkini();
	DD_CreateWindow("a.D.D.i.c.t. - advanced Digital Dynamite intro creation tool v0.9 (c) BoyC 2002-2003", 800, 600, 32, fulls, LoadIcon(hInstance, MAKEINTRESOURCE(101)));
	glClearColor(backgroundcol);
	glClear(0x4100);
	SwapBuffers(hDC);

	initintroeditor();
	init();
	FILE *inifile=fopen("back.raw","rb");
	if (inifile!=NULL)
	{
		backraw=new byte[196608];
		fread(backraw,256*256,3,inifile);
		backdraw=true;
		glGenTextures(1,&backrawtex);
		glBindTexture(GL_TEXTURE_2D,backrawtex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,backraw);
		fclose(inifile);
	}

	//glClearColor(50.0f/255.0f,65.0f/255.0f,89.0f/255.0f,0);
	mainloop();
	
	KillGLWindow();
	return 0;
}