#pragma once 

#include <windows.h>
#include "tlD3D.h"

class tlFont {
public:
  tlFont(LPDIRECT3DDEVICE9 dev);
  ~tlFont();

	LPDIRECT3DDEVICE9 dev;
  LPDIRECT3DTEXTURE9 tex;

	void PutChar(int x,int y,char c);
	void PrintF(int x,int y,unsigned int color,float size,char * s, ...);
	void SetClipArea(LPRECT r);

  float texcoordu1[256];
  float texcoordu2[256];
  float texcoordv1[256];
  float texcoordv2[256];
};