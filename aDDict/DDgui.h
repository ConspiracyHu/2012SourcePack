#ifndef __DDGUI__
#define __DDGUI__

#include "aDDict.h"
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include "DDwindow.h"

#define DDgui_LeftClick                   1000
#define DDgui_RightClick                  1001
#define DDgui_MidClick                    1002
#define DDgui_ReSize                      2000
#define DDgui_ToggleWire                  2001

//#define backgroundcol (float)0.07843,(float)0.14705,(float)0.25,   (float)1.0
#define backgroundcol 50.0f/255.0f,65.0f/255.0f,89.0f/255.0f,0
/*#define col           (float)0.31372,(float)0.58823,(float)1.0,    (float)1.0
#define col2          (float)0.10457,(float)0.19607,(float)0.33333,(float)1.0
#define col3          (float)0.15686,(float)0.29411,(float)0.5,    (float)1.0
#define col4          (float)0.20915,(float)0.39215,(float)0.66666,(float)1.0
#define buttontextcol (float)0.24132,(float)0.45248,(float)0.76923,(float)1.0*/
//#define buttontextlit (float)0.34509,(float)0.64705,(float)1.0,    (float)1.0
#define buttontextlit 157.0/256.0,174.0/256.0,205.0/256.0,    (float)1.0
//#define buttontexton  (float)0.94117,(float)0.96470,(float)1.0,    (float)1.0

#define buttontextcol 1,1,1,1

#define col           88.0f/127.0f,105.0f/127.0f,133.0f/127.0f,0
#define col2          88.0f/511.0f,105.0f/511.0f,133.0f/511.0f,0
#define col3          88.0f/255.0f,105.0f/255.0f,133.0f/255.0f,0
#define col4          253.0f/255.0f,253.0f/255.0f,1,0

#define buttontexton  52.0f/255.0f,52.0f/255.0f,59.0f/255.0f,0
#define buttonhighlighted 104.0f/255.0f,133.0f/255.0f,179.0f/255.0f,0

#define DD_Draw 0
#define DD_Check 1
#define DD_AfterCheck 2

void line(float x1, float y1, float x2,float y2);
void line(int x1, int y1, int x2,int y2);
void specline(int x1, int y1, int x2,int y2, int time);
void rectangle(float x1, float y1, float x2,float y2);
void rectangle(int x1, int y1, int x2,int y2);
void specrectangle(int x1, int y1, int x2,int y2, int time);
//void specrectangle(float x1, float y1, float x2,float y2, int time);
void quad(float x1, float y1, float x2,float y2);
void quad(int x1, int y1, int x2,int y2);
int button(int x1, int y1, int x2, int y2,char* text,int tx,int ty, GLuint font, bool lit, byte mode);
int button(int x1, int y1, int x2, int y2,GLuint texture, double xs1, double ys1,double xs2, double ys2, bool lit, byte mode);
void ValueBar(int x1,int y1,int x2,int y2,int mi,int ma,byte *value,char *s,int s1,int s2, GLuint font, byte mode);
void ValueBar(int x1,int y1,int x2,int y2,int mi,int ma,int *value,char *s,int s1,int s2, GLuint font, byte mode);
void ValueBar(int x1,int y1,int x2,int y2,float mi,float ma,float *value,char *s,int s1,int s2, GLuint font, byte mode);

void ValueBar(int x1,int y1,int color,float mi,float ma,float skipper,float *value,char *s,int s1,int s2, GLuint font, byte mode);
void ValueBar(int x1,int y1,int color,byte mi,byte ma,byte *value,char *s,int s1,int s2, GLuint font, byte mode);
void ValueBar(int x1,int y1,int color,int mi,int ma,int *value,char *s,int s1,int s2, GLuint font, byte mode);

void scroller(int x1,int y1,int x2,int y2,int buttonsize,int scrollsize,int itemnumber,int size,int &pos, int mode);
void hscroller(int x1,int y1,int x2,int y2,int buttonsize,int scrollsize,int itemnumber,int size,int &pos, int mode);

int RadioButton(int x1, int y1, bool &lit, char *text, int mode);

char* readline(const char* windowheader,int x,int y,GLuint font,const char* def);

void initGUI();


#endif