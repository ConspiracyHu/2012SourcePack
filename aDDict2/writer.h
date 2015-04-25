#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#define WRITER_STYLE_BOLD   0x00000001
#define WRITER_STYLE_ITALIC 0x00000002

class CWriter {
public:
  CWriter();
//  virtual ~CWriter(void);

//  void Upload();

//  void PutChar(float,float,int,char);
  void Print(float,float,float,float,float,char*);

  HFONT font;
  unsigned int * buffer;

  float texcoordu1[256];
  float texcoordu2[256];
  float texcoordv1[256];
  float texcoordv2[256];
  
  GLuint tex;
};