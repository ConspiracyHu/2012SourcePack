#include "DDgui.h"
#include "gombok.h"
#include "mainmenu.h"
#include "textbutton1.h"
//#include "texts.h"
#include "gombok2.h"
#include "gombok3.h"

void line(float x1, float y1, float x2,float y2)
{
	glBegin(GL_LINES);
	 glVertex2f(x1,y1);
	 glVertex2f(x2,y2);
	glEnd();
}

void line(int x1, int y1, int x2,int y2)
{
	glBegin(GL_LINES);
	 glVertex2f((float)x1,(float)y1);
	 glVertex2f((float)x2,(float)y2);
	glEnd();
}

float linear_(float a, float b, float t)
{
	return a+(b-a)*t;
}

float linear_(int a, int b, float t)
{
	return (float)a+((float)b-(float)a)*min(1,max(0,t));
}

void specline(int x1, int y1, int x2,int y2, int time)
{
	float len=(float)(sqrt((float)(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
	glBegin(GL_LINES);
	 for (float x=-20; x<=len+20; x=x+20)
	 {
		 float l1=(x+(int)((float)time/20.0f)%20)/len;
		 if (l1<0) l1=0; if (l1>1) l1=1;
		 float l2=(x+10.0f+(int)((float)time/20.0f)%20)/len;
		 if (l2<0) l2=0; if (l2>1) l2=1;
		 float xx1,yy1,xx2,yy2;
		 xx1=linear_(x1,x2,l1);
		 xx2=linear_(x1,x2,l2);
		 yy1=linear_(y1,y2,l1);
		 yy2=linear_(y1,y2,l2);
		 glVertex2f((float)xx1,(float)yy1);
		 glVertex2f((float)xx2,(float)yy2);
	 }
	glEnd();
}

void specline(float x1, float y1, float x2,float y2, int time)
{
	float len=(float)(sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
	glBegin(GL_LINES);
	 for (float x=0; x<=len; x=x+20)
	 {
		 float l1=(x+(int)((float)time/10.0f)%20)/len;
		 float l2=(x+10.0f+(int)((float)time/10.0f)%20)/len;
		 float xx1,yy1,xx2,yy2;
		 xx1=linear_(x1,x2,l1);
		 xx2=linear_(x1,x2,l2);
		 yy1=linear_(y1,y2,l1);
		 yy2=linear_(y1,y2,l2);
		 glVertex2f((float)xx1,(float)yy1);
		 glVertex2f((float)xx2,(float)yy2);
	 }
	glEnd();
}

void rectangle(float x1, float y1, float x2,float y2)
{
	line(x1,y1,x2,y1);
	line(x2,y1,x2,y2);
	line(x2,y2,x1,y2);
	line(x1,y2,x1,y1);
}

void rectangle(int x1, int y1, int x2,int y2)
{
	line(x1,y1,x2,y1);
	line(x2,y1,x2,y2);
	line(x2,y2,x1,y2);
	line(x1,y2,x1,y1);
}

void specrectangle(int x1, int y1, int x2,int y2, int time)
{
	specline(x1,y1,x2,y1,time);
	specline(x2,y1,x2,y2,time);
	specline(x2,y2,x1,y2,time);
	specline(x1,y2,x1,y1,time);
}

/*void specrectangle(float x1, float y1, float x2,float y2, int time)
{
	specline(x1,y1,x2,y1,time);
	specline(x2,y1,x2,y2,time);
	specline(x2,y2,x1,y2,time);
	specline(x1,y2,x1,y1,time);
}*/


void quad(float x1, float y1, float x2,float y2)
{
	glBegin(GL_QUADS);
	 glVertex2f(x1,y1);
	 glVertex2f(x2,y1);
	 glVertex2f(x2,y2);
	 glVertex2f(x1,y2);
	 //glVertex2f(x1,y1);
	glEnd();
}

void quad(int x1, int y1, int x2,int y2)
{
	glBegin(GL_QUADS);
	 glVertex2f((float)x1,(float)y1);
	 glVertex2f((float)x2,(float)y1);
	 glVertex2f((float)x2,(float)y2);
	 glVertex2f((float)x1,(float)y2);
	 //glVertex2f((float)x1,(float)y1);
	glEnd();
}

int button(int x1, int y1, int x2, int y2,char* text,int tx,int ty, GLuint font, bool lit, byte mode)
{
	int res = 0;
	if (mode==DD_Draw)
	{
		
		bool lit2 = lit;
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glScissor(x1,600-y2,max(1,x2-x1+1),max(1,y2-y1+1));
		glEnable(GL_SCISSOR_TEST);
		if (leftclickinwindow(x1,y1,x2,y2)) res=DDgui_LeftClick;
		
		if (res==DDgui_LeftClick) lit2=true;
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		
		if (!lit2) glColor4f(col); else glColor4f(col2);
		line(x1,y2,x1,y1);
		line(x1,y1,x2,y1);
		if (lit2)  glColor4f(col); else glColor4f(col2);
		line(x2,y1,x2,y2);
		line(x2,y2,x1,y2);
		
		if (!lit2) glColor4f(col3); else glColor4f(col4);
		if (mouseinwindow(x1,y1,x2,y2) && !leftbutton && !lit2) glColor4f(buttonhighlighted);
		glDepthMask(false);
		quad(x1,y1,x2,y2);
		glDepthMask(true);
		
		//if (mouseinwindow(x1,y1,x2,y2)) glColor4f(buttontextlit); else 
		glColor4f(buttontextcol);
		if (lit2) glColor4f(buttontexton);
		
		glDisable(GL_DEPTH_TEST);
		glRasterPos2i(tx+x1,ty+y1);
		//glPrintCenter(text,font,x1,y1,x2,y2);
		glPrint(text,font);
		glPopAttrib();
		return 0;
	}
	if ((mode==DD_Check) || (mode==DD_AfterCheck))
	{
		if (leftclickinwindow(x1,y1,x2,y2)) res=DDgui_LeftClick;
		if (rightclickinwindow(x1,y1,x2,y2)) res=DDgui_RightClick;
		if (midclickinwindow(x1,y1,x2,y2)) res=DDgui_MidClick;
		return res;
	}
	return 0;
}

void ValueBar(int x1,int y1,int x2,int y2,int mi,int ma,int *value,char *s,int s1,int s2, GLuint font, byte mode)
{
 if (mode==DD_Draw)
 {
 int aa,bb;
 glPushAttrib(GL_ALL_ATTRIB_BITS);
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BLEND);
 glDisable(GL_LIGHTING);
 glColor4f(buttontextcol);
 rectangle(x1,y1,x2,y2);
 if (*value<mi) *value=mi;
 if (*value>ma) *value=ma;
 aa=(int)((float)x1+((float)x2-(float)x1)/((float)ma-(float)mi)*((float)(*value)-(float)mi));
 bb=max(x1+2,aa);

 glColor4f(col);
 line(x1+1,y2-1,x1+1,y1+1);
 line(x1+1,y1+1,bb-1,y1+1);
 glColor4f(col2);
 line(bb-1,y1+1,bb-1,y2-1);
 line(bb-1,y2-1,x1+1,y2-1);

 glDepthMask(false);
 quad(x1,y1,x2,y2);
 glDepthMask(true);
 glColor4f(col3);
 quad(x1,y1,aa,y2);
 glColor4f(1,1,1,1);
 glDisable(GL_DEPTH_TEST);
 glRasterPos2i(x1+s1,y1+s2);
 glPrint(s,font);
 glPopAttrib();
 }
 else
 {
	 if (leftclickinwindow(x1,y1,x2,y2) && mouseinwindow(x1,y1,x2,y2)) *value=(int)((((float)mx-(float)x1)/((float)x2-(float)x1)*((float)ma-(float)mi)+(float)mi)+0.5);
	 if (mode==DD_Check) if (mouseinwindow(x1,y1,x2,y2)) *value+=wheel;
 }
}

void ValueBar(int x1,int y1,int x2,int y2,int mi,int ma,byte *value,char *s,int s1,int s2, GLuint font, byte mode)
{
 if (mode==DD_Draw)
 {
 int aa,bb;
 glPushAttrib(GL_ALL_ATTRIB_BITS);
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BLEND);
 glDisable(GL_LIGHTING);
 glColor4f(buttontextcol);
 rectangle(x1,y1,x2,y2);
 if (*value<mi) *value=mi;
 if (*value>ma) *value=ma;
 aa=(int)((float)x1+((float)x2-(float)x1)/((float)ma-(float)mi)*((float)(*value)-(float)mi));
 bb=max(x1+2,aa);

 glColor4f(col);
 line(x1+1,y2-1,x1+1,y1+1);
 line(x1+1,y1+1,bb-1,y1+1);
 glColor4f(col2);
 line(bb-1,y1+1,bb-1,y2-1);
 line(bb-1,y2-1,x1+1,y2-1);

 glDepthMask(false);
 quad(x1,y1,x2,y2);
 glDepthMask(true);
 glColor4f(col3);
 quad(x1,y1,aa,y2);
 glColor4f(1,1,1,1);
 glDisable(GL_DEPTH_TEST);
 glRasterPos2i(x1+s1,y1+s2);
 glPrint(s,font);
 glPopAttrib();
 }
 else
 {
	 if (leftclickinwindow(x1,y1,x2,y2) && mouseinwindow(x1,y1,x2,y2)) *value=(byte)((((float)mx-(float)x1)/((float)x2-(float)x1)*((float)ma-(float)mi)+(float)mi)+0.5);
 }
}


void ValueBar(int x1,int y1,int x2,int y2,float mi,float ma,float *value,char *s,int s1,int s2, GLuint font, byte mode)
{
 if (mode==DD_Draw)
 {
 int aa,bb;
 glPushAttrib(GL_ALL_ATTRIB_BITS);
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BLEND);
 glDisable(GL_LIGHTING);
 glColor4f(buttontextcol);
 rectangle(x1,y1,x2,y2);
 if (*value<mi) *value=mi;
 if (*value>ma) *value=ma;
 aa=(int)((float)x1+((float)x2-(float)x1)/((float)ma-(float)mi)*((float)(*value)-(float)mi));
 bb=max(x1+2,aa);

 glColor4f(col);
 line(x1+1,y2-1,x1+1,y1+1);
 line(x1+1,y1+1,bb-1,y1+1);
 glColor4f(col2);
 line(bb-1,y1+1,bb-1,y2-1);
 line(bb-1,y2-1,x1+1,y2-1);

 glDepthMask(false);
 quad(x1,y1,x2,y2);
 glDepthMask(true);
 glColor4f(col3);
 quad(x1,y1,aa,y2);
 glColor4f(1,1,1,1);
 glDisable(GL_DEPTH_TEST);
 glRasterPos2i(x1+s1,y1+s2);
 glPrint(s,font);
 glPopAttrib();
 }
 else
 {
	 if (leftclickinwindow(x1,y1,x2,y2) && mouseinwindow(x1,y1,x2,y2)) *value=(float)((((float)mx-(float)x1)/((float)x2-(float)x1)*((float)ma-(float)mi)+(float)mi));
 }
}

void ValueBar(int x1,int y1,int color,float mi,float ma,float skipper,float *value,char *s,int s1,int s2, GLuint font, byte mode)
{
 if (mode==DD_Draw)
 {
	 if (*value<mi) *value=mi;
	 if (*value>ma) *value=ma;
	 int aa;
	 aa=(int)((float)(x1+9)+(113.0f)/((float)ma-(float)mi)*((float)(*value)-(float)mi));
	 bool leftpushed=leftclickinwindow(x1,y1,x1+8,y1+19);
	 bool rightpushed=leftclickinwindow(x1+123,y1,x1+131,y1+19);
	 bool leftlit=mouseinwindow(x1,y1,x1+8,y1+19);
	 bool rightlit=mouseinwindow(x1+123,y1,x1+131,y1+19);
	 glPushAttrib(GL_ALL_ATTRIB_BITS);
	 glEnable(GL_TEXTURE_2D);
	 glDisable(GL_BLEND);
	 glDisable(GL_LIGHTING);
	 glDisable(GL_DEPTH_TEST);
	 glBindTexture(GL_TEXTURE_2D,texbutton1);
	 glColor4f(1,1,1,1);

	 glBegin(GL_QUADS);

	 glTexCoord2d(0.0/256.0,180.0/256.0);
	 if (leftlit) glTexCoord2d(0.0/256.0,218.0/256.0);
	 if (leftpushed) glTexCoord2d(0.0/256.0,199.0/256.0);
	 glVertex2i(x1,y1);

	 glTexCoord2d(9.0/256.0,180.0/256.0);
	 if (leftlit) glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (leftpushed) glTexCoord2d(9.0/256.0,199.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (leftlit) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (leftpushed) glTexCoord2d(9.0/256.0,218.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glTexCoord2d(0.0/256.0,199.0/256.0);
	 if (leftlit) glTexCoord2d(0.0/256.0,237.0/256.0);
	 if (leftpushed) glTexCoord2d(0.0/256.0,218.0/256.0);
	 glVertex2i(x1,y1+19);

	 glTexCoord2d(122.0/256.0,180.0/256.0);
	 if (rightlit) glTexCoord2d(122.0/256.0,218.0/256.0);
	 if (rightpushed) glTexCoord2d(122.0/256.0,199.0/256.0);
	 glVertex2i(x1+122,y1);

	 glTexCoord2d(131/256.0,180.0/256.0);
	 if (rightlit) glTexCoord2d(131/256.0,218.0/256.0);
	 if (rightpushed) glTexCoord2d(131/256.0,199.0/256.0);
	 glVertex2i(x1+131,y1);

	 glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (rightlit) glTexCoord2d(131.0/256.0,237.0/256.0);
	 if (rightpushed) glTexCoord2d(131.0/256.0,218.0/256.0);
	 glVertex2i(x1+131,y1+19);

	 glTexCoord2d(122.0/256.0,199.0/256.0);
	 if (rightlit) glTexCoord2d(122.0/256.0,237.0/256.0);
	 if (rightpushed) glTexCoord2d(122.0/256.0,218.0/256.0);
	 glVertex2i(x1+122,y1+19);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,237.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d(122.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(122.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,237.0/256.0);
	 glVertex2i(x1+122,y1);

	 glTexCoord2d(122.0/256.0,218.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,218.0/256.0);
	 if (color==3) glTexCoord2d(122.0/256.0,256.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,256.0/256.0);
	 glVertex2i(x1+122,y1+19);

	 glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,218.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,256.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,256.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glTexCoord2d(9.0/256.0,180.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,180.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,218.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d((float)(aa-x1)/256.0,180.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,180.0/256.0);
	 if (color==3) glTexCoord2d((float)(aa-x1)/256.0,218.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,218.0/256.0);
	 glVertex2i(aa,y1);

	 glTexCoord2d((float)(aa-x1)/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d((float)(aa-x1)/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,237.0/256.0);
	 glVertex2i(aa,y1+19);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,237.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glEnd();

	 glDisable(GL_TEXTURE_2D);
	 glRasterPos2i(x1+s1,y1+s2);
	 glPrint(s,font);
	 glPopAttrib();
 }
 else
	if (mode!=DD_AfterCheck)
 {
	 if (leftclickinwindow(x1+9,y1,x1+122,y1+19) && mouseinwindow(x1+9,y1,x1+122,y1+19)) *value=(float)((((float)mx-(float)(x1+9))/(113.0f)*(ma-mi)+mi));
	 if (leftclickinwindow(x1,y1,x1+8,y1+19)) {*value=*value-skipper; waitleftbutton=true;}
	 if (leftclickinwindow(x1+123,y1,x1+131,y1+19)) {*value=*value+skipper; waitleftbutton=true;}
	 if (mode==DD_Check) if (mouseinwindow(x1,y1,x1+131,y1+19)) *value+=wheel*skipper;
	 if (*value<mi) *value=mi;
	 if (*value>ma) *value=ma;
 }
}

void ValueBar(int x1,int y1,int color,byte mi,byte ma,byte *value,char *s,int s1,int s2, GLuint font, byte mode)
{
 if (mode==DD_Draw)
 {
	 if (*value<mi) *value=mi;
	 if (*value>ma) *value=ma;
	 int aa;
	 aa=(int)((float)(x1+9)+(113.0f)/((float)ma-(float)mi)*((float)(*value)-(float)mi));
	 bool leftpushed=leftclickinwindow(x1,y1,x1+8,y1+19);
	 bool rightpushed=leftclickinwindow(x1+123,y1,x1+131,y1+19);
	 bool leftlit=mouseinwindow(x1,y1,x1+8,y1+19);
	 bool rightlit=mouseinwindow(x1+123,y1,x1+131,y1+19);
	 glPushAttrib(GL_ALL_ATTRIB_BITS);
	 glEnable(GL_TEXTURE_2D);
	 glDisable(GL_BLEND);
	 glDisable(GL_LIGHTING);
	 glDisable(GL_DEPTH_TEST);
	 glBindTexture(GL_TEXTURE_2D,texbutton1);
	 glColor4f(1,1,1,1);

	 glBegin(GL_QUADS);

	 glTexCoord2d(0.0/256.0,180.0/256.0);
	 if (leftlit) glTexCoord2d(0.0/256.0,218.0/256.0);
	 if (leftpushed) glTexCoord2d(0.0/256.0,199.0/256.0);
	 glVertex2i(x1,y1);

	 glTexCoord2d(9.0/256.0,180.0/256.0);
	 if (leftlit) glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (leftpushed) glTexCoord2d(9.0/256.0,199.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (leftlit) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (leftpushed) glTexCoord2d(9.0/256.0,218.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glTexCoord2d(0.0/256.0,199.0/256.0);
	 if (leftlit) glTexCoord2d(0.0/256.0,237.0/256.0);
	 if (leftpushed) glTexCoord2d(0.0/256.0,218.0/256.0);
	 glVertex2i(x1,y1+19);

	 glTexCoord2d(122.0/256.0,180.0/256.0);
	 if (rightlit) glTexCoord2d(122.0/256.0,218.0/256.0);
	 if (rightpushed) glTexCoord2d(122.0/256.0,199.0/256.0);
	 glVertex2i(x1+122,y1);

	 glTexCoord2d(131/256.0,180.0/256.0);
	 if (rightlit) glTexCoord2d(131/256.0,218.0/256.0);
	 if (rightpushed) glTexCoord2d(131/256.0,199.0/256.0);
	 glVertex2i(x1+131,y1);

	 glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (rightlit) glTexCoord2d(131.0/256.0,237.0/256.0);
	 if (rightpushed) glTexCoord2d(131.0/256.0,218.0/256.0);
	 glVertex2i(x1+131,y1+19);

	 glTexCoord2d(122.0/256.0,199.0/256.0);
	 if (rightlit) glTexCoord2d(122.0/256.0,237.0/256.0);
	 if (rightpushed) glTexCoord2d(122.0/256.0,218.0/256.0);
	 glVertex2i(x1+122,y1+19);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,237.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d(122.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(122.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,237.0/256.0);
	 glVertex2i(x1+122,y1);

	 glTexCoord2d(122.0/256.0,218.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,218.0/256.0);
	 if (color==3) glTexCoord2d(122.0/256.0,256.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,256.0/256.0);
	 glVertex2i(x1+122,y1+19);

	 glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,218.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,256.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,256.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glTexCoord2d(9.0/256.0,180.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,180.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,218.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d((float)(aa-x1)/256.0,180.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,180.0/256.0);
	 if (color==3) glTexCoord2d((float)(aa-x1)/256.0,218.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,218.0/256.0);
	 glVertex2i(aa,y1);

	 glTexCoord2d((float)(aa-x1)/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d((float)(aa-x1)/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,237.0/256.0);
	 glVertex2i(aa,y1+19);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,237.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glEnd();

	 glDisable(GL_TEXTURE_2D);
	 glRasterPos2i(x1+s1,y1+s2);
	 glPrint(s,font);
	 glPopAttrib();
 }
 else
	if (mode!=DD_AfterCheck)
 {
	 if (leftclickinwindow(x1+9,y1,x1+122,y1+19) && mouseinwindow(x1+9,y1,x1+122,y1+19)) *value=(byte)((((float)mx-(float)(x1+9))/(113.0f)*(float)(ma-mi)+(float)mi+0.5));
	 if (leftclickinwindow(x1,y1,x1+8,y1+19)) {*value=*value-1; waitleftbutton=true;}
	 if (leftclickinwindow(x1+123,y1,x1+131,y1+19)) {*value=*value+1; waitleftbutton=true;}
	 if (mode==DD_Check) if (mouseinwindow(x1,y1,x1+131,y1+19)) *value+=wheel;
	 if (*value<mi) *value=mi;
	 if (*value>ma) *value=ma;
 }
}

void ValueBar(int x1,int y1,int color,int mi,int ma,int *value,char *s,int s1,int s2, GLuint font, byte mode)
{
 if (mode==DD_Draw)
 {
	 if (*value<mi) *value=mi;
	 if (*value>ma) *value=ma;
	 int aa;
	 aa=(int)((float)(x1+9)+(113.0f)/((float)ma-(float)mi)*((float)(*value)-(float)mi));
	 bool leftpushed=leftclickinwindow(x1,y1,x1+8,y1+19);
	 bool rightpushed=leftclickinwindow(x1+123,y1,x1+131,y1+19);
	 bool leftlit=mouseinwindow(x1,y1,x1+8,y1+19);
	 bool rightlit=mouseinwindow(x1+123,y1,x1+131,y1+19);
	 glPushAttrib(GL_ALL_ATTRIB_BITS);
	 glEnable(GL_TEXTURE_2D);
	 glDisable(GL_BLEND);
	 glDisable(GL_LIGHTING);
	 glDisable(GL_DEPTH_TEST);
	 glBindTexture(GL_TEXTURE_2D,texbutton1);
	 glColor4f(1,1,1,1);

	 glBegin(GL_QUADS);

	 glTexCoord2d(0.0/256.0,180.0/256.0);
	 if (leftlit) glTexCoord2d(0.0/256.0,218.0/256.0);
	 if (leftpushed) glTexCoord2d(0.0/256.0,199.0/256.0);
	 glVertex2i(x1,y1);

	 glTexCoord2d(9.0/256.0,180.0/256.0);
	 if (leftlit) glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (leftpushed) glTexCoord2d(9.0/256.0,199.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (leftlit) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (leftpushed) glTexCoord2d(9.0/256.0,218.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glTexCoord2d(0.0/256.0,199.0/256.0);
	 if (leftlit) glTexCoord2d(0.0/256.0,237.0/256.0);
	 if (leftpushed) glTexCoord2d(0.0/256.0,218.0/256.0);
	 glVertex2i(x1,y1+19);

	 glTexCoord2d(122.0/256.0,180.0/256.0);
	 if (rightlit) glTexCoord2d(122.0/256.0,218.0/256.0);
	 if (rightpushed) glTexCoord2d(122.0/256.0,199.0/256.0);
	 glVertex2i(x1+122,y1);

	 glTexCoord2d(131/256.0,180.0/256.0);
	 if (rightlit) glTexCoord2d(131/256.0,218.0/256.0);
	 if (rightpushed) glTexCoord2d(131/256.0,199.0/256.0);
	 glVertex2i(x1+131,y1);

	 glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (rightlit) glTexCoord2d(131.0/256.0,237.0/256.0);
	 if (rightpushed) glTexCoord2d(131.0/256.0,218.0/256.0);
	 glVertex2i(x1+131,y1+19);

	 glTexCoord2d(122.0/256.0,199.0/256.0);
	 if (rightlit) glTexCoord2d(122.0/256.0,237.0/256.0);
	 if (rightpushed) glTexCoord2d(122.0/256.0,218.0/256.0);
	 glVertex2i(x1+122,y1+19);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,237.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d(122.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(122.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,237.0/256.0);
	 glVertex2i(x1+122,y1);

	 glTexCoord2d(122.0/256.0,218.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,218.0/256.0);
	 if (color==3) glTexCoord2d(122.0/256.0,256.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,256.0/256.0);
	 glVertex2i(x1+122,y1+19);

	 glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,218.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,256.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,256.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glTexCoord2d(9.0/256.0,180.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,180.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,218.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,218.0/256.0);
	 glVertex2i(x1+9,y1);

	 glTexCoord2d((float)(aa-x1)/256.0,180.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,180.0/256.0);
	 if (color==3) glTexCoord2d((float)(aa-x1)/256.0,218.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,218.0/256.0);
	 glVertex2i(aa,y1);

	 glTexCoord2d((float)(aa-x1)/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(243.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d((float)(aa-x1)/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(243.0/256.0,237.0/256.0);
	 glVertex2i(aa,y1+19);

	 glTexCoord2d(9.0/256.0,199.0/256.0);
	 if (color==1) glTexCoord2d(131.0/256.0,199.0/256.0);
	 if (color==3) glTexCoord2d(9.0/256.0,237.0/256.0);
	 if (color==2) glTexCoord2d(131.0/256.0,237.0/256.0);
	 glVertex2i(x1+9,y1+19);

	 glEnd();

	 glDisable(GL_TEXTURE_2D);
	 glRasterPos2i(x1+s1,y1+s2);
	 glPrint(s,font);
	 glPopAttrib();
 }
 else
	if (mode!=DD_AfterCheck)
 {
	 if (leftclickinwindow(x1+9,y1,x1+122,y1+19) && mouseinwindow(x1+9,y1,x1+122,y1+19)) *value=(int)((((float)mx-(float)(x1+9))/(113.0f)*(float)(ma-mi)+(float)mi+0.5));
	 if (leftclickinwindow(x1,y1,x1+8,y1+19)) {*value=*value-1; waitleftbutton=true;}
	 if (leftclickinwindow(x1+123,y1,x1+131,y1+19)) {*value=*value+1; waitleftbutton=true;}
	 if (mode==DD_Check) if (mouseinwindow(x1,y1,x1+131,y1+19)) *value+=wheel;
	 if (*value<mi) *value=mi;
	 if (*value>ma) *value=ma;
 }
}


char* readline(const char* windowheader,int x,int y,GLuint font,const char* def)
{
	bool donereading = false;
	char *str = new(char [256]);
	char s2[256];
	keyboardbuffer=NULL;
	//strcpy(str,def);
	memset(str,0,256);
	memcpy(str,def,strlen(def));
	tkey *ll;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_BLEND);
	glColor4f(1,1,1,1);
	glEnable(GL_DEPTH_TEST);

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
		SwapBuffers(hDC);
	}

	glDisable(GL_TEXTURE_2D);

	do
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)  { done=true; donereading=true;}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (active)
		{
			SwapBuffers(hDC);
			glClear(0x100);
			glColor4f(buttontextlit);
			rectangle(50,280,750,320);
			glColor4f(0,0,0,1);
			quad(50,280,750,320);
			glColor4f(buttontextlit);
			glClear(0x100);
			glRasterPos2i(50+x,293+y);
			glPrint(windowheader,font);
			glColor4f(1,1,1,1);
			while (keyboardbuffer!=NULL)
			{
				if ((keyboardbuffer->key!=VK_BACK) && (keyboardbuffer->key!=VK_RETURN) && (keyboardbuffer->key!=VK_ESCAPE) && (strlen(str)<76))
				{
					str[strlen(str)]=keyboardbuffer->key;
				}
				else
				{
					switch (keyboardbuffer->key)
					{
						case VK_ESCAPE  : 
							{
								memset(str,0,256);
								keys[VK_ESCAPE]=false;
								break;
							}
						case VK_BACK : 
							{
								str[strlen(str)-1]=0;
								break;
							}
						case VK_RETURN : 
							{
								str[strlen(str)]='\0';
								donereading=true;
								break;
							}
					}
				}

				ll=keyboardbuffer;
				keyboardbuffer=keyboardbuffer->next;
				delete(ll);

			}
			glRasterPos2i(55,310);
			sprintf(s2,"%s_",str);
			glPrint(s2,base);
		}
	}
	while (!donereading);
	glEnable(GL_TEXTURE_2D);
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
		SwapBuffers(hDC);
	}
	glClear(0x100);
	glPopAttrib();
	return str;
}

void scroller(int x1,int y1,int x2,int y2,int buttonsize,int scrollsize,int itemnumber,int size,int &pos, int mode)
{
	int a1,b1,a2,b2;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	/*if (mode==DD_Draw)
	{
		glDepthMask(false);
		glColor4f(col2);
		quad(x1,y1,x2,y2);
		glDepthMask(true);
	}*/

	//int button(int x1, int y1, int x2, int y2,GLuint texture, double xs1, double ys1,double xs2, double ys2, bool lit, byte mode)
	//if (button(x1,y1,x2,y1+buttonsize,"",0,0,base,false,mode) == DDgui_LeftClick) {pos--; Sleep(30);}
	//if (button(x1,y2-buttonsize,x2,y2,"",0,0,base,false,mode) == DDgui_LeftClick) {pos++; Sleep(30);}

	glColor4f(40.0/256.0,52.0/256.0,71.0/256.0,1);
	quad(x1+5,y1,x1+12,y2);
	glColor4f(1,1,1,1);
	if (button(x1+1,y1,x2+1,y1+buttonsize,cubetex,0.0,176.0/256.0,16.0/256.0,191.0/256.0,false,mode) == DDgui_LeftClick) {pos--; Sleep(30);}
	if (button(x1+1,y2-buttonsize,x2+1,y2,cubetex,0.0,208.0/256.0,16.0/256.0,223.0/256.0,false,mode) == DDgui_LeftClick) {pos++; Sleep(30);}

	a1=x1; a2=x2;
	b1=(int)( ((y2-buttonsize-1-scrollsize)-(y1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +y1+buttonsize+1);
	if (itemnumber==size) b1=y1+buttonsize+1;
	b2=b1+scrollsize;

	if (mode==DD_Check)
	{
		if (leftclickinwindow(x1,y1+buttonsize+1,x2,y2-buttonsize-1) && mouseinwindow(x1-buttonsize*2,y1+buttonsize+1,x2+buttonsize*2,y2-buttonsize-1))
		{
			if (my<b1) { pos-=scrollsize/2; Sleep(30); }
			if (my>b1+buttonsize) { pos+=scrollsize/2; Sleep(30); }
			if (ly>=b1 && ly<=b1+buttonsize)
			{				
				pos=(int)(( ( (float)my-((float)y1+(float)buttonsize+(float)scrollsize / 2.0+1.0) ) / ( ((float)y2-(float)buttonsize-(float)scrollsize / 2.0-1.0)-((float)y1+(float)buttonsize+ (float)scrollsize / 2.0+1.0) ) * ((float)itemnumber-(float)size))+0.5);
				a1=x1; a2=x2;
				b1=(int)( ((y2-buttonsize-1-scrollsize)-(y1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +y1+buttonsize+1);
				if (itemnumber==size) b1=y1+buttonsize+1;
				ly=b1+buttonsize/2;
			}
		}
	}
	
	if (pos>itemnumber-size) pos=itemnumber-size;

	if (pos<0) pos=0;

	a1=x1; a2=x2;
	b1=(int)( ((y2-buttonsize-1-scrollsize)-(y1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +y1+buttonsize+1);
	if (itemnumber==size) b1=y1+buttonsize+1;
	b2=b1+scrollsize;

	if (mode==DD_Draw)
	{
		if (itemnumber>size)
		button(a1+1,b1+1,a2+1,b2+1,cubetex,0,192.0/256.0,16.0/256.0,208.0/256.0,leftclickinwindow(x1,y1+buttonsize+1,x2,y2-buttonsize-1) && mouseinwindow(x1-buttonsize*2,y1+buttonsize+1,x2+buttonsize*2,y2-buttonsize-1),mode);

		/*glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,cubetex);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0,
		glVertex2i(a1,b1);
		glVertex2i(a2,b1);
		glVertex2i(a2,b2);
		glVertex2i(a1,b2);
		glEnd();*/


		/*glColor4f(col);
		line(a1,b2,a1,b1);
		line(a1,b1,a2,b1);
		glColor4f(col2);
		line(a2,b1,a2,b2);
		line(a2,b2,a1,b2);

		glColor4f(col3);
		glDepthMask(false);
		quad(a1,b1,a2,b2);
		glDepthMask(true);*/
	}
	glPopAttrib();
}

int button(int x1, int y1, int x2, int y2,GLuint texture, double xs1, double ys1,double xs2, double ys2, bool lit, byte mode)
{
	int res = 0;
	if (mode==DD_Draw)
	{
		
		bool lit2 = lit;
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (leftclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_LeftClick;
		
		if (res==DDgui_LeftClick) lit2=true;

		double u1,v1,u2,v2,us;
		u1=xs1;
		u2=xs2;
		us=u2-u1;
		v1=ys1;
		v2=ys2;
		if (!lit) {u1+=us; u2+=us;}
		if (mouseinwindow(x1,y1,x2-1,y2-1) && !leftbutton && !lit) {u1+=us; u2+=us;}
		if (lit2) {u1=xs1; u2=xs2;}
	
		glColor4f(1,1,1,1);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture);
		glBegin(GL_QUADS);
		glTexCoord2d(u1,v1);
		glVertex2i(x1,y1);
		glTexCoord2d(u2,v1);
		glVertex2i(x2,y1);
		glTexCoord2d(u2,v2);
		glVertex2i(x2,y2);
		glTexCoord2d(u1,v2);
		glVertex2i(x1,y2);
		glEnd();

		glPopAttrib();
		return 0;
	}
	if ((mode==DD_Check) || (mode==DD_AfterCheck))
	{
		if (leftclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_LeftClick;
		if (rightclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_RightClick;
		if (midclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_MidClick;
		return res;
	}
	return 0;
}

void initGUI()
{
	glGenTextures(1,&cubetex);
	glBindTexture(GL_TEXTURE_2D,cubetex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,raw_gombok);

	glGenTextures(1,&mainmenutex);
	glBindTexture(GL_TEXTURE_2D,mainmenutex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,512,128,0,GL_RGB,GL_UNSIGNED_BYTE,raw_mainmenu);

	glGenTextures(1,&texbutton1);
	glBindTexture(GL_TEXTURE_2D,texbutton1);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,raw_textbutton1);

	glGenTextures(1,&texbutton2);
	glBindTexture(GL_TEXTURE_2D,texbutton2);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,raw_gombok2);

/*	glGenTextures(1,&texts1);
	glBindTexture(GL_TEXTURE_2D,texts1);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,raw_texts);*/

	glGenTextures(1,&texbutton3);
	glBindTexture(GL_TEXTURE_2D,texbutton3);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,raw_gombok3);

}

int RadioButton(int x1, int y1, bool &lit, char *text, int mode)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,cubetex);
	if (mode==DD_Draw)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(171.0/256.0,137.0/256.0);
		if (lit) glTexCoord2f(185.0/256.0,137.0/256.0);
		glVertex2i(x1,y1);

		glTexCoord2f((171.0+14.0)/256.0,137.0/256.0);
		if (lit) glTexCoord2f((185.0+14.0)/256.0,137.0/256.0);
		glVertex2i(x1+14,y1);

		glTexCoord2f((171.0+14.0)/256.0,(137.0+14.0)/256.0);
		if (lit) glTexCoord2f((185.0+14.0)/256.0,(137.0+14.0)/256.0);
		glVertex2i(x1+14,y1+14);

		glTexCoord2f(171.0/256.0,(137.0+14.0)/256.0);
		if (lit) glTexCoord2f(185.0/256.0,(137.0+14.0)/256.0);
		glVertex2i(x1,y1+14);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glColor4f(buttontextlit);
	glRasterPos2i(x1+17,y1+10);
	glPrint(text,base2);
	if (mode==DD_Check)
	{
		if (leftclickinwindow(x1,y1,x1+14,y1+14)) { lit=!lit; waitleftbutton=true;}		
	}
	glPopAttrib();

	return 0;
}

int hbutton(int x1, int y1, int x2, int y2,GLuint texture, double xs1, double ys1,double xs2, double ys2, bool lit, byte mode)
{
	int res = 0;
	if (mode==DD_Draw)
	{
		
		bool lit2 = lit;
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (leftclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_LeftClick;
		
		if (res==DDgui_LeftClick) lit2=true;

		double u1,v1,u2,v2,us;
		u1=xs1;
		u2=xs2;
		us=u2-u1;
		v1=ys1;
		v2=ys2;
		if (!lit) {u1+=us; u2+=us;}
		if (mouseinwindow(x1,y1,x2-1,y2-1) && !lit) {u1+=us; u2+=us;}
		if (lit2) {u1=xs1; u2=xs2;}
	
		glColor4f(1,1,1,1);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture);
		glBegin(GL_QUADS);
		glTexCoord2d(u1,v1);
		glVertex2i(x1,y2);
		glTexCoord2d(u2,v1);
		glVertex2i(x1,y1);
		glTexCoord2d(u2,v2);
		glVertex2i(x2,y1);
		glTexCoord2d(u1,v2);
		glVertex2i(x2,y2);
		glEnd();

		glPopAttrib();
		return 0;
	}
	if ((mode==DD_Check) || (mode==DD_AfterCheck))
	{
		if (leftclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_LeftClick;
		if (rightclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_RightClick;
		if (midclickinwindow(x1,y1,x2-1,y2-1)) res=DDgui_MidClick;
		return res;
	}
	return 0;
}


void hscroller(int x1,int y1,int x2,int y2,int buttonsize,int scrollsize,int itemnumber,int size,int &pos, int mode)
{
	int a1,b1,a2,b2;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	/*if (mode==DD_Draw)
	{
		glDepthMask(false);
		glColor4f(col2);
		quad(y1,x1,y2,x2);
		glDepthMask(true);
	}*/

	//int button(int y1, int x1, int y2, int x2,GLuint texture, double xs1, double ys1,double xs2, double ys2, bool lit, byte mode)
	//if (button(y1,x1,y2,x1+buttonsize,"",0,0,base,false,mode) == DDgui_LeftClick) {pos--; Sleep(30);}
	//if (button(y1,x2-buttonsize,y2,x2,"",0,0,base,false,mode) == DDgui_LeftClick) {pos++; Sleep(30);}

	glColor4f(40.0/256.0,52.0/256.0,71.0/256.0,1);
	quad(x1,y1+5,x2,y1+12);
	glColor4f(1,1,1,1);
	if (hbutton(x1,y1+1,x1+buttonsize,y2+1,cubetex,0.0,176.0/256.0,16.0/256.0,191.0/256.0,false,mode) == DDgui_LeftClick) {pos--; Sleep(30);}
	if (hbutton(x2-buttonsize,y1+1,x2,y2+1,cubetex,0.0,208.0/256.0,16.0/256.0,223.0/256.0,false,mode) == DDgui_LeftClick) {pos++; Sleep(30);}

	a1=y1; a2=y2;
	b1=(int)( ((x2-buttonsize-1-scrollsize)-(x1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +x1+buttonsize+1);
	if (itemnumber==size) b1=x1+buttonsize+1;
	b2=b1+scrollsize;

	if (mode==DD_Check)
	{
		if (leftclickinwindow(x1+buttonsize+1,y1,x2-buttonsize-1,y2) && mouseinwindow(x1+buttonsize+1,y1-buttonsize*2,x2-buttonsize-1,y2+buttonsize*2))
		{
			if (mx<b1) { pos-=scrollsize/2; Sleep(30); 
						 int ba1=(int)( ((x2-buttonsize-1-scrollsize)-(x1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +x1+buttonsize+1);
						 if (mx>ba1) pos+=scrollsize/2;
						}
			if (mx>b1+buttonsize) { pos+=scrollsize/2; Sleep(30); 
						 int ba1=(int)( ((x2-buttonsize-1-scrollsize)-(x1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +x1+buttonsize+1);
						 if (mx<ba1+buttonsize) pos-=scrollsize/2;
						}

			//if (mx<b1) { pos-=scrollsize/2; Sleep(30); }
			//if (mx>b1+buttonsize) { pos+=scrollsize/2; Sleep(30); }

			if (lx>=b1 && lx<=b1+buttonsize)
			{				
				pos=(int)(( ( (float)mx-((float)x1+(float)buttonsize+(float)scrollsize / 2.0+1.0) ) / ( ((float)x2-(float)buttonsize-(float)scrollsize / 2.0-1.0)-((float)x1+(float)buttonsize+ (float)scrollsize / 2.0+1.0) ) * ((float)itemnumber-(float)size))+0.5);
				a1=y1; a2=y2;
				b1=(int)( ((x2-buttonsize-1-scrollsize)-(x1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +x1+buttonsize+1);
				if (itemnumber==size) b1=x1+buttonsize+1;
				lx=b1+buttonsize/2;
			}
		}
	}
	
	if (pos>itemnumber-size) pos=itemnumber-size;

	if (pos<0) pos=0;

	a1=y1; a2=y2;
	b1=(int)( ((x2-buttonsize-1-scrollsize)-(x1+buttonsize+1)) * ((float)pos/((float)itemnumber-(float)size)) +x1+buttonsize+1);
	if (itemnumber==size) b1=x1+buttonsize+1;
	b2=b1+scrollsize;

	if (mode==DD_Draw)
	{
		if (itemnumber>size)
		hbutton(b1+1,a1+1,b2+1,a2+1,cubetex,0,192.0/256.0,16.0/256.0,208.0/256.0,leftclickinwindow(x1+buttonsize+1,y1,x2-buttonsize-1,y2) && mouseinwindow(x1+buttonsize+1,y1-buttonsize*2,x2-buttonsize-1,y2+buttonsize*2),mode);

		/*glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,cubetex);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0,
		glVertey2i(a1,b1);
		glVertey2i(a2,b1);
		glVertey2i(a2,b2);
		glVertey2i(a1,b2);
		glEnd();*/


		/*glColor4f(col);
		line(a1,b2,a1,b1);
		line(a1,b1,a2,b1);
		glColor4f(col2);
		line(a2,b1,a2,b2);
		line(a2,b2,a1,b2);

		glColor4f(col3);
		glDepthMask(false);
		quad(a1,b1,a2,b2);
		glDepthMask(true);*/
	}
	glPopAttrib();
}
