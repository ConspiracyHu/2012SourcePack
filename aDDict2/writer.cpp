#include <stdio.h>
#include <windows.h>
#include "writer.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WRITER_WIDTH  512
#define WRITER_HEIGHT 512

//#define SAVETGA

#ifdef SAVETGA
unsigned char tgahead[]={
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0xC8, 0x00, 
  0x20, 0x08
};

#define TGAW(x) (*(unsigned short *)(tgahead+0x0C))=(x)
#define TGAH(x) (*(unsigned short *)(tgahead+0x0E))=(x)
#endif

CWriter::CWriter() {
	HDC hdc = GetDC(NULL);
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP bm = CreateCompatibleBitmap(hdc,WRITER_WIDTH,WRITER_HEIGHT);     

	int size = 32;
	int track = 5;
	HFONT hf=CreateFont(size, 0, 0, 0, FW_NORMAL, 
	0  , FALSE, FALSE,
	DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
	ANTIALIASED_QUALITY, DEFAULT_PITCH,"Verdana");

	unsigned int * buffer = new unsigned int[WRITER_WIDTH*WRITER_HEIGHT];

	RECT r={0,0,WRITER_WIDTH,WRITER_HEIGHT};

	SelectObject(mdc, bm);
	FillRect(mdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

	SelectObject(mdc, hf);      
	SetBkMode(mdc, TRANSPARENT);
	SetTextColor(mdc, 0xFFFFFF);
	int x=0;
	int y=0;
	int i;
	for (i=0; i<256; i++) {
		char s[2]={i,0};
		TextOut(mdc, x, y, s, 1);

		SIZE sz;
		GetTextExtentPoint(mdc,s,1,&sz);
		texcoordu1[i]=x/(float)WRITER_WIDTH;	x+=sz.cx;
		texcoordu2[i]=x/(float)WRITER_WIDTH;	x+=track;

		texcoordv1[i]= y       /(float)WRITER_WIDTH;
		texcoordv2[i]=(y+sz.cy)/(float)WRITER_WIDTH;

		if (x>450) {
			x=0;
			y+=size+track;
		}
	}

	BITMAPINFO bmi;    
	bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth=WRITER_WIDTH;
	bmi.bmiHeader.biHeight=WRITER_HEIGHT;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=32;
	bmi.bmiHeader.biCompression=BI_RGB;

	GetDIBits(mdc,bm,0,WRITER_HEIGHT,buffer,&bmi,DIB_RGB_COLORS);

#ifdef SAVETGA
  FILE * f =fopen("dump.tga","wb");
  TGAW(WRITER_WIDTH);
  TGAH(WRITER_HEIGHT);
  fwrite(tgahead,0x12,1,f);
  fwrite(buffer,WRITER_WIDTH*WRITER_HEIGHT*4,1,f);
  fclose(f);
#endif

	for (i=0; i<WRITER_WIDTH*WRITER_HEIGHT; i++) {
	buffer[i] = ((buffer[i]&0xFF)<<24)|0x00FFFFFF;
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D,    GL_RGBA8, WRITER_WIDTH, WRITER_HEIGHT,    GL_RGBA, GL_UNSIGNED_BYTE, buffer);

}

void CWriter::Print(float x1,float y1,float x2,float y2,float size,char * s) {


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, tex);
	glColor4f(1,1,1,1);

	glPushMatrix();
	glTranslatef(x1,y1,0);

	int ln=0;
	int i;
	for(i=0; i<(int)strlen(s); i++) if(s[i]=='\n') ln++;

	float rw=x2-x1;
	float oy=0;

	int align=3; // 1 = left, 2 = center, 3 = right


	char ss[500],*sc=s;
	for(i=0; i<ln; i++) {
		char * snn=strchr(sc,'\n');
		int l=snn-sc;
		memcpy(ss,sc,l);
		ss[l]=0;
		
		float tw=0;
		for(int j=0; j<(int)strlen(ss); j++) {
			unsigned char cc=ss[j];
			if(ss[j]>=1 && ss[j]<=3) {
				align=ss[j];
			}
			float lw = texcoordu2[cc]-texcoordu1[cc];
			if (cc<32) lw =0;
			tw += lw;
		}

		float ox=0;
		if (align==2) ox = (rw/size-tw)/2.0f;
		if (align==3) ox = (rw/size-tw);

		for (int k=0; k<(int)strlen(ss); k++) {
			unsigned char cc=ss[k];
			float lw = texcoordu2[cc]-texcoordu1[cc];
			float lh = texcoordv2[cc]-texcoordv1[cc];
			if (cc<32) lw =0;

			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(texcoordu1[cc],1-texcoordv1[cc]); glVertex2f((ox   )*size,(oy   )*size); 
				glTexCoord2f(texcoordu1[cc],1-texcoordv2[cc]); glVertex2f((ox   )*size,(oy+lh)*size); 
				glTexCoord2f(texcoordu2[cc],1-texcoordv1[cc]); glVertex2f((ox+lw)*size,(oy   )*size); 
				glTexCoord2f(texcoordu2[cc],1-texcoordv2[cc]); glVertex2f((ox+lw)*size,(oy+lh)*size); 
			glEnd();
			ox+=lw;
		}
		oy+=texcoordv2['(']-texcoordv1['('];

	    sc=snn+1;
	}
	glPopMatrix();

}

/*
#define WIDTH 150
#define TRACK 0.001

void CWriter::PutChar(float x,float y,int sz,char c) {
  unsigned char cc=c;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, tex);
  glColor4f(1,1,1,1);
  float w=startcoordz[cc+1]-startcoordz[cc];
  w*=WIDTH;
  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(startcoordz[cc  ],0);                       glVertex2f(x   ,y  ); 
    glTexCoord2f(startcoordz[cc  ],1);                       glVertex2f(x   ,y-sz); 
    glTexCoord2f(startcoordz[cc+1]-(4/(float)WRITER_WIDTH),0); glVertex2f(x+sz*w,y  ); 
    glTexCoord2f(startcoordz[cc+1]-(4/(float)WRITER_WIDTH),1); glVertex2f(x+sz*w,y-sz); 
  glEnd();
}

void CWriter::Print(float x,float y,int sz,char * s) {
  float cx=x,cy=y;
  int ln=0;
  for(int i=0; i<(int)strlen(s); i++) if(s[i]=='\n') ln++;

  char ss[500],*sc=s;
  for(i=0; i<ln; i++) {
    char * snn=strchr(sc,'\n');
    int l=snn-sc;
    memcpy(ss,sc,l);
    ss[l]=0;

    int sl=strlen(ss);

    float wi=0;
    for(int j=0; j<sl; j++) {
      unsigned char cc=ss[j];
      wi+=(float)((startcoordz[cc+1]-startcoordz[cc]-TRACK)*sz*WIDTH);
    }

    cx=cx-wi/2.0f;
    for(j=0; j<sl; j++) {
      unsigned char cc=ss[j];
      PutChar(cx,cy,sz,cc);
      cx+=(float)((startcoordz[cc+1]-startcoordz[cc]-TRACK)*sz*WIDTH);
    }
    cx=x;
    cy+=sz*0.6f;
    sc=snn+1;
  }

  for(int i=0; i<strlen(s); i++) {
    switch(s[i]) {
      case '\n': {
        cx=x;
        cy-=sz*0.6;
      } break;
      default: {
        PutChar(cx,cy,sz,s[i]);
        cx+=(startcoordz[s[i]+1]-startcoordz[s[i]]-TRACK)*sz*WIDTH;
      } break;
    }
  }
}
*/