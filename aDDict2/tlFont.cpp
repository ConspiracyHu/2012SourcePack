
#include <stdio.h>
#include <windows.h>
#include "tlFont.h"

#define WRITER_WIDTH  512
#define WRITER_HEIGHT 512

#define SAVETGA

#ifdef SAVETGA
unsigned char tgahead[]={
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0xC8, 0x00, 
  0x20, 0x08
};

#define TGAW(x) (*(unsigned short *)(tgahead+0x0C))=(x)
#define TGAH(x) (*(unsigned short *)(tgahead+0x0E))=(x)
#endif

tlFont::tlFont(LPDIRECT3DDEVICE9 dev_) {
	dev=dev_;
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
	for (int i=0; i<256; i++) {
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

	dev->CreateTexture(WRITER_WIDTH,WRITER_HEIGHT,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&tex,NULL);

	D3DLOCKED_RECT rc;
	if (tex->LockRect(0,&rc,NULL,0)) return;

	unsigned char * trow=(unsigned char *)rc.pBits;
	for (int j=0; j<WRITER_HEIGHT; j++) {
		unsigned int* t=(unsigned int*)trow;
		for (int k=0; k<WRITER_WIDTH; k++) {
			*t = ((buffer[j*WRITER_WIDTH+k] & 0xff) << 24) | 0x00FFFFFF;
//	*t = ((buffer[j*WRITER_WIDTH+k] ^ 0xAA) << 24) | 0x00FFFFFF;
			t++;
		}
		trow+=rc.Pitch;
	}
	tex->UnlockRect(0);

}

tlFont::~tlFont() {}

#define LOADCOLOR3 0xFFFFFFFF
TL_VEC letterquad[]={
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, LOADCOLOR3, 0.0001, 0.0001, 0.0, 0.0}, // bal fent
	{ 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, LOADCOLOR3, 0.9999, 0.0001, 1.0, 0.0}, // jobb fent
	{ 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, LOADCOLOR3, 0.0001, 0.9999, 0.0, 1.0}, // bal lent
	{ 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, LOADCOLOR3, 0.9999, 0.9999, 1.0, 1.0}, // jobb lent
};
/*
void tlFont::PutChar(int x,int y,char c) {

	unsigned char cc=c;
	float letterwidth  = texcoordu2[cc]-texcoordu1[cc];
	float letterheight = texcoordv2[cc]-texcoordv1[cc];

	letterquad[0].u1=letterquad[2].u1=  texcoordu1[cc];
	letterquad[1].u1=letterquad[3].u1=  texcoordu2[cc];
	letterquad[0].v1=letterquad[1].v1=1-texcoordv1[cc];
	letterquad[2].v1=letterquad[3].v1=1-texcoordv2[cc];

	Matrix m;
	m.SetOrtho(0,BBX,0,BBY,0,1);
	dev->SetTransform(D3DTS_PROJECTION,&m);

	float size=0.2;

//	Matrix m;
	m.setIdentity();
	m.TranslatePosition(Vector(x,y,1));
	m.TranslateScale(Vector(letterwidth*WRITER_WIDTH*size,letterheight*WRITER_HEIGHT*size,1));
	dev->SetTransform(D3DTS_WORLD,&m);

	dev->SetRenderState(D3DRS_ZENABLE,FALSE);
	dev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	dev->SetRenderState(D3DRS_TEXTUREFACTOR,0xFF000000);
	dev->SetRenderState(D3DRS_SRCBLEND ,D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
	dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);

	dev->SetTexture(0,tex);
	dev->BeginScene();
	dev->SetFVF(TL_FVF);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,letterquad,sizeof(TL_VEC));
	dev->EndScene();

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

}
*/

void tlFont::PrintF(int x,int y,unsigned int color,float size,char * fmt, ...) {

	size /= 20.0;

	char text[2560];
	va_list		ap;

	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);


	Matrix m;
	m.SetOrtho(0,BBX,0,BBY,0,1);
	dev->SetTransform(D3DTS_PROJECTION,&m);

//	float size=0.3;

	dev->SetRenderState(D3DRS_ZENABLE,FALSE);
	dev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	dev->SetRenderState(D3DRS_TEXTUREFACTOR,0xFF000000 | (color&0xFFFFFF));
	dev->SetRenderState(D3DRS_SRCBLEND ,D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
	dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);

	dev->SetTexture(0,tex);
	dev->BeginScene();
	dev->SetFVF(TL_FVF);

	float ox=0,oy=0;

	for (int i=0; i<strlen(text); i++) {
		unsigned char cc=text[i];
		float letterwidth  = texcoordu2[cc]-texcoordu1[cc];
		float letterheight = texcoordv2[cc]-texcoordv1[cc];

		letterquad[0].u1=letterquad[2].u1=  texcoordu1[cc];
		letterquad[1].u1=letterquad[3].u1=  texcoordu2[cc];
		letterquad[0].v1=letterquad[1].v1=1-texcoordv1[cc];
		letterquad[2].v1=letterquad[3].v1=1-texcoordv2[cc];

	//	Matrix m;
		m.setIdentity();
		m.TranslatePosition(Vector(x+ox*WRITER_WIDTH*size,y+oy*WRITER_HEIGHT*size,1));
		m.TranslateScale(Vector(letterwidth*WRITER_WIDTH*size,letterheight*WRITER_HEIGHT*size,1));
		dev->SetTransform(D3DTS_WORLD,&m);

		ox+=letterwidth;
		dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,letterquad,sizeof(TL_VEC));
	}
	dev->EndScene();

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

}

void tlFont::SetClipArea(LPRECT r) {
	if (r) {
		RECT q = *r;
		if (q.left >tlD3D::wndw) return;
		if (q.right>tlD3D::wndh) return;
		q.left   = q.left  <0 ? 0 : q.left;
		q.top    = q.top   <0 ? 0 : q.top;
		q.right  = q.right >tlD3D::wndw ? tlD3D::wndw : q.right;
		q.bottom = q.bottom>tlD3D::wndh ? tlD3D::wndh : q.bottom;
		dev->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE);
		dev->SetScissorRect(&q);
	}
	else {
		dev->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
	}
}