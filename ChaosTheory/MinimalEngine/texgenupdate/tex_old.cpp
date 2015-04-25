#include "TexGen.h"

TEXTURE *TextureList=NULL;

#define SCALEX(x) (((x)*XRes)>>8)
#define WRAPX(x) ((x)&(XRes-1))

#ifdef __TEXGEN_SQUARE_TEXTURES__
#define SCALEY(y) (((y)*YRes)>>8)
#define WRAPY(y) ((y)&(YRes-1))
#else
#define SCALEY(y) SCALEX(y)
#define WRAPY(y) WRAPX(y)
#define YRes XRes
#endif

float * distancebuffer;

void memcpy_dw(void * to, void * from, int count) {
	__asm {
		mov edi,to
		mov esi,from
		mov ecx,count
		rep movsd
	}
}

unsigned int interpolate_colors(unsigned int c1, unsigned int c2, unsigned int alpha) {
	unsigned int r;
	__asm {
		pxor mm7,mm7
		movd mm0,c1
		movd mm1,c2

		mov eax,alpha
		shl eax,16
		add eax,alpha
		movd mm3,eax
		movq mm2,mm3  
		punpckldq mm3,mm2

		punpcklbw mm0,mm7
		punpcklbw mm1,mm7
		psubw mm1,mm0
  
		pmullw mm1,mm3
		psrlw mm1,8
		paddb mm1,mm0
		packuswb mm1,mm7
  
		movd r,mm1
		emms;
	}
	return r;
}

#ifdef __TEXGEN_BILINEAR__
unsigned int TEXTURE::Bilinear(RGBA * Layer,unsigned int u, unsigned int v) {
	unsigned int c00=Layer[WRAPX((u>>8)  )+WRAPY((v>>8)  )*XRes].dw; 
	unsigned int c01=Layer[WRAPX((u>>8)  )+WRAPY((v>>8)+1)*XRes].dw;
	unsigned int c10=Layer[WRAPX((u>>8)+1)+WRAPY((v>>8)  )*XRes].dw;
	unsigned int c11=Layer[WRAPX((u>>8)+1)+WRAPY((v>>8)+1)*XRes].dw;
	u&=0xFF;
	v&=0xFF;
	unsigned int ci0=interpolate_colors(c00,c01,v);
	unsigned int ci1=interpolate_colors(c10,c11,v);

	return interpolate_colors(ci0,ci1,u);

}
#endif

void __stdcall operator_asm_add(RGBA*Old, RGBA*New,int opsize) {
	__asm {
		mov ecx,opsize
		mov esi,New
		mov edi,Old
		addloop:
			movd mm0,[edi]
			movd mm1,[esi]
			paddusb mm0,mm1
			movd eax,mm0
			stosb
			add edi,3
			add esi,4
		loop addloop
		emms
	}
}
void __stdcall operator_asm_sub(RGBA*Old, RGBA*New,int opsize) {
	__asm {
		mov ecx,opsize
		mov esi,New
		mov edi,Old
		subloop:
			movd mm0,[edi]
			movd mm1,[esi]
			psubusb mm0,mm1
			movd eax,mm0
			stosb
			add edi,3
			add esi,4
		loop subloop
		emms
	}
}
void __stdcall operator_asm_mul(RGBA*Old, RGBA*New,int opsize) {
	__asm {
		mov ecx,opsize
		mov esi,New
		mov edi,Old
		xor eax,eax
		mulloop:
			lodsb
			mul [edi]
			shr eax,8
			stosb
			add edi,3
			add esi,3
		loop mulloop
		emms
	}	
}
void __stdcall operator_asm_sel(RGBA*Old, RGBA*New,int opsize) {
	__asm {
		mov ecx,opsize
		mov esi,New
		mov edi,Old
		selloop:
			movsb
			add edi,3
			add esi,3
		loop selloop
	}
}

typedef void (WINAPI * OPERATORPROC)(RGBA*,RGBA*,int);

OPERATORPROC opproc[]={
	&operator_asm_sel,
	&operator_asm_add,
	&operator_asm_sub,
	&operator_asm_mul,
};

void TEXTURE::PerformFilter(RGBA*Old, RGBA*New, COMMAND*Params) {
	int opsize=XRes * YRes;
	for (int i=0; i<4; i++) {
		if (Params->OperatorMask[i])
		opproc[Params->Operator](
			(RGBA*)( ((unsigned char*)Old)+Params->OperatorMask[i]*i ),
			(RGBA*)( ((unsigned char*)New)+Params->OperatorMask[i]*i ),
			opsize);
	}
}

#ifdef INCLUDE_TEX_Envmap
void TEXTURE::Envmap(RGBA* Layer, COMMAND* Parameters)
{
	int a;
	float s=255/(float)((ENVMAPTEMPLATE*)Parameters->data)->Size;

	int Resolution=XRes;
	for (int x=0; x<Resolution; x++)
		for (int y=0; y<Resolution; y++)
		{
			a=y*Resolution+x;
			unsigned char r=(unsigned char)((float)min(255,max(0,(255-255*s*(float)sqrt((float)((y-Resolution/2)*(y-Resolution/2)+(x-Resolution/2)*(x-Resolution/2)))/(float)(Resolution/2)))));
			Layer[a].dw=r*0x01010101;
		}
}
#endif

#ifdef INCLUDE_TEX_Plasma
void TEXTURE::Plasma(RGBA* Layer, COMMAND* Parameters) {
	int a;
	unsigned char xsines=((PLASMATEMPLATE*)Parameters->data)->XSines;
	unsigned char ysines=((PLASMATEMPLATE*)Parameters->data)->YSines;
	int Resolution=XRes;
	for (int x=0; x<Resolution; x++)
		for (int y=0; y<Resolution; y++)
		{
			a=y*Resolution+x;
			Layer[a].dw=((unsigned char)
				(127.0f + 63.5f*sin(x/(float)XRes * xsines * 2.0f * 3.1415f) +
									63.5f*sin(y/(float)YRes * ysines * 2.0f * 3.1415f) ))*0x01010101;
		}
}
#endif

#ifdef INCLUDE_TEX_Map
void TEXTURE::Map(RGBA* Layer, COMMAND* Parameters) {
	RGBA * buffer = new RGBA[XRes*YRes];
	MAPTEMPLATE * m = (MAPTEMPLATE*)Parameters->data;
	int Resolution=XRes;
//	return;
	for (int x=0; x<Resolution; x++)
		for (int y=0; y<Resolution; y++) {
			int ofs=x+y*XRes;
			unsigned char sx=Layers[m->XDataLayer].Data[ofs].c[m->XDataChannel];
			unsigned char sy=Layers[m->XDataLayer].Data[ofs].c[m->XDataChannel];
#ifndef __TEXGEN_BILINEAR__
			unsigned int ox=WRAPX(x+((sx*m->XAmount*XRes)>>16));
			unsigned int oy=WRAPY(y+((sy*m->YAmount*YRes)>>16));
			buffer[ofs].dw=Layer[ox+oy*XRes].dw;
#else
			unsigned int ox=x+(unsigned int)((sx*m->XAmount*XRes)/65536.0);
			unsigned int oy=y+(unsigned int)((sy*m->YAmount*YRes)/65536.0);
			buffer[x+y*XRes].dw=Bilinear(Layer,ox*256,oy*256);
#endif
		}
	//memcpy(Layer,buffer,XRes*YRes*sizeof(RGBA));
	memcpy_dw(Layer,buffer,XRes*YRes);
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_Blur
void TEXTURE::Blur(RGBA* Layer, COMMAND* Parameters) {
	RGBA * buffer = new RGBA[XRes*YRes];//
	int s=SCALEX(((BLURTEMPLATE*)Parameters->data)->SampleWidth);
	int it=((BLURTEMPLATE*)Parameters->data)->Iterations;
	for (int z=0; z<it; z++) {
		int offset = 0;
		for (int j=0; j<YRes; j++)
		{
			for (int i=0; i<XRes; i++) 
			{				
				unsigned int r01 = Layer[WRAPX(i-s)+WRAPY(j  )*XRes].dw;
				unsigned int r21 = Layer[WRAPX(i+s)+WRAPY(j  )*XRes].dw;
				unsigned int r10 = Layer[WRAPX(i  )+WRAPY(j-s)*XRes].dw;
				unsigned int r12 = Layer[WRAPX(i  )+WRAPY(j+s)*XRes].dw;
				unsigned int z=0;
				__asm {
					pxor mm0,mm0
					pxor mm7,mm7

					movd mm1,r01
					punpcklbw mm1,mm7
					paddw mm0,mm1

					movd mm1,r21
					punpcklbw mm1,mm7
					paddw mm0,mm1

					movd mm1,r10
					punpcklbw mm1,mm7
					paddw mm0,mm1

					movd mm1,r12
					punpcklbw mm1,mm7
					paddw mm0,mm1

					psrlw mm0,2
					packuswb mm0,mm7
					movd z,mm0;
				}

				buffer[offset].dw=z;

				offset++;
			}
		}
		//memcpy(Layer,buffer,XRes*YRes*sizeof(RGBA));
		memcpy_dw(Layer,buffer,XRes*YRes);
	}
	__asm { emms }
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_DirBlur
void TEXTURE::DirBlur(RGBA* Layer, COMMAND* Parameters) {
	RGBA * buffer = new RGBA[XRes*YRes];
  int countr,countg,countb,counta;
	DIRBLURTEMPLATE* d=(DIRBLURTEMPLATE*)Parameters->data;
	int numsamples=d->Depth;
	if (numsamples) {
		for (int x=0; x<XRes; x++)
			for (int y=0; y<YRes; y++) {
				countr=countg=countb=counta=0;

				float dir=Layers[d->DataLayer].Data[x+XRes*y].c[d->DataChannel]/256.0f*2.0f*3.14f;
				int b1=(int)(cos(dir)*XRes);
				int b2=(int)(sin(dir)*YRes);

				for (int zc=0;zc<numsamples;zc++) {
					unsigned int p1=WRAPX(x+((b1*zc)>>8));
					unsigned int p2=WRAPY(y+((b2*zc)>>8));
					countr+=Layer[p1+XRes*p2].r;
					countg+=Layer[p1+XRes*p2].g;
					countb+=Layer[p1+XRes*p2].b;
					counta+=Layer[p1+XRes*p2].a;
				}
				buffer[x+XRes*y].r=countr/numsamples;
				buffer[x+XRes*y].g=countg/numsamples;
				buffer[x+XRes*y].b=countb/numsamples;
				buffer[x+XRes*y].a=counta/numsamples;
			}
	}
	//memcpy(Layer,buffer,XRes*YRes*sizeof(RGBA));
	memcpy_dw(Layer,buffer,XRes*YRes);
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_Text
char * fonts[]={
	"Arial",
	"Verdana",
	"Courier New",
	"Webdings",
	"Lucida Console",
	"Tahoma",
	"Impact",
	"Wingdings",
	"Times New Roman",
	"Microsoft Sans Serif",
	"Trebuchet MS",
	"Symbol"
};

void TEXTURE::Text(RGBA* Layer, COMMAND* Parameters) {
	RGBA * buffer = new RGBA[XRes*YRes];
	TEXTTEMPLATE * t = (TEXTTEMPLATE*)Parameters->data;

	if (t->Text)
	{

		HDC hdc = GetDC(NULL);
		HDC mdc = CreateCompatibleDC(hdc);
		HBITMAP bm = CreateCompatibleBitmap(hdc,XRes,YRes);     
		BITMAPINFO bmi;    

		HFONT hf=CreateFont((t->Size*XRes)>>8, 0, 0, 0, t->Bold?FW_BOLD:FW_NORMAL,
		t->Italic, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		ANTIALIASED_QUALITY, DEFAULT_PITCH,fonts[t->Font]);


		RECT r={0,0,XRes,YRes};

		SelectObject(mdc, bm);
		FillRect(mdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

		SelectObject(mdc, hf);      
		SetBkMode(mdc, TRANSPARENT);
		SetTextColor(mdc, 0xFFFFFF);
		SetTextCharacterExtra(mdc,SCALEX(t->Spacing));
		TextOut(mdc, 0, 0, t->Text, strlen(t->Text));
	//	TextOut(mdc, SCALEX(t->X), SCALEY(t->Y), t->Text, strlen(t->Text));

		bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth=XRes;
		bmi.bmiHeader.biHeight=YRes;
		bmi.bmiHeader.biPlanes=1;
		bmi.bmiHeader.biBitCount=32;
		bmi.bmiHeader.biCompression=BI_RGB;

		GetDIBits(mdc,bm,0,YRes,buffer,&bmi,DIB_RGB_COLORS);

	  for (int zx=0;zx<XRes;zx++)
		for (int zy=0;zy<YRes;zy++) {
		  Layer[WRAPX(zx+SCALEX(t->X))+WRAPY((YRes-zy-1)+SCALEX(t->Y))*XRes].dw=buffer[zx+zy*XRes].c[0]*0x01010101;
			 }

	//	memcpy(Layer,buffer,XRes*YRes*sizeof(RGBA));
		delete buffer;
		DeleteDC(mdc);
		//DeleteObject(hf);
		//DeleteObject(bm);
	}
}
#endif

unsigned char catmullrom_interpolate(float v0, float v1, float v2, float v3, unsigned int x, unsigned int distance)
{
	float xx = (float)x / (float)distance;
	float P = (v3 - v2) - (v0 - v1);
	float Q = (v0 - v1) - P;
	float R =  v2 - v0;
	int t=(int)((P*xx*xx*xx) + (Q*xx*xx) + (R*xx) + v1);

	_asm {
		movd mm0,t
		packuswb mm0,mm0
		movd t,mm0
		emms;
	}

//	if (t<0) t=0; if (t>255) t=255;
	return t;
}

#ifdef OLD_TEXGEN

void generate_subplasma(unsigned char * buffer, int size, int randseed, int rx, int ry, int max) {
	int x,y;
	//int np=rx/size;
	int np=size;
	if (size>rx) {
		memset(buffer,0,rx*ry);
		return;
	}
	size=rx/np;
	srand(randseed);
  for (y=0; y<np; y++)
    for (x=0; x<np; x++)
      buffer[x*size+y*size*rx] = rand()&max;

	if (size==1) return;

  for (y=0; y<np; y++)
    for (x=0; x<rx; x++) {
			//int p=(x/size)*size;
			int p=x&(~(size-1));
			int zy=y*size*rx;
			buffer[x+zy] = catmullrom_interpolate(
				buffer[((p-size*1)&(rx-1))+zy],
				buffer[((p-size*0)&(rx-1))+zy],
				buffer[((p+size*1)&(rx-1))+zy],
				buffer[((p+size*2)&(rx-1))+zy],
				x%size,size);
		}
			
  for (y=0; y<ry; y++)
    for (x=0; x<rx; x++) {
			int p=y&(~(size-1));
//			int p=(y/size)*size;
			buffer[x+y*rx] = catmullrom_interpolate(
				buffer[x+((p-size*1)&(ry-1))*rx],
				buffer[x+((p-size*0)&(ry-1))*rx],
				buffer[x+((p+size*1)&(ry-1))*rx],
				buffer[x+((p+size*2)&(ry-1))*rx],
				y%size,size);
		}

}

#ifdef INCLUDE_TEX_SubPlasma
void TEXTURE::SubPlasma(RGBA* Layer, COMMAND* Parameters) {
	SUBPLASMATEMPLATE * s = (SUBPLASMATEMPLATE*)Parameters->data;
	unsigned char * buffer = new unsigned char[XRes*YRes];
	generate_subplasma(buffer, 1 << s->Size, s->RandSeed,XRes,YRes,255);
  for (int zx=0;zx<XRes;zx++)
    for (int zy=0;zy<YRes;zy++)
			Layer[zx+zy*XRes].dw=buffer[zx+zy*XRes]*0x01010101;
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_FractalPlasma
void TEXTURE::FractalPlasma(RGBA* Layer, COMMAND* Parameters) {
	FRACTALPLASMATEMPLATE * s = (FRACTALPLASMATEMPLATE*)Parameters->data;
	int opsize=XRes*YRes;
	unsigned char * buffer = new unsigned char[opsize];
	unsigned char * buffer2 = new unsigned char[opsize];
	memset(buffer2,0,opsize);
	for (int x=1; x<=6; x++) {
		generate_subplasma(buffer, 1 << x, s->RandSeed,XRes,YRes,(1 << (8-x))-1);
		__asm{
			mov esi,buffer
			mov edi,buffer2
			mov ecx,opsize
			fploop:
				lodsb
				add al,[edi]
				stosb
			loop fploop
		}
	}
  for (int zx=0;zx<XRes;zx++)
    for (int zy=0;zy<YRes;zy++)
			Layer[zx+zy*XRes].dw=buffer2[zx+zy*XRes]*0x01010101;

	delete[] buffer;
	delete[] buffer2;
}
#endif

#else

void generate_subplasma(unsigned char * buffer, int np, int randseed, int rx, int ry) {
	int x,y;
	if (np>rx)
		np=rx;
/*
	if (size>rx) {
		memset(buffer,0,rx*ry);
		return;
	}
*/
	int ssize=rx/np;
	srand(randseed);
  for (y=0; y<np; y++)
    for (x=0; x<np; x++)
      buffer[x*ssize+y*ssize*rx] = rand();

	if (ssize<=1) return;

  for (y=0; y<np; y++)
    for (x=0; x<rx; x++) {
			int p=x&(~(ssize-1));
			int zy=y*ssize*rx;
			buffer[x+zy] = catmullrom_interpolate(
				buffer[((p-ssize*1)&(rx-1))+zy],
				buffer[((p-ssize*0)&(rx-1))+zy],
				buffer[((p+ssize*1)&(rx-1))+zy],
				buffer[((p+ssize*2)&(rx-1))+zy],
				x%ssize,ssize);
		}
			
  for (y=0; y<ry; y++)
    for (x=0; x<rx; x++) {
			int p=y&(~(ssize-1));
			buffer[x+y*rx] = catmullrom_interpolate(
				buffer[x+((p-ssize*1)&(ry-1))*rx],
				buffer[x+((p-ssize*0)&(ry-1))*rx],
				buffer[x+((p+ssize*1)&(ry-1))*rx],
				buffer[x+((p+ssize*2)&(ry-1))*rx],
				y%ssize,ssize);
		}
}

#ifdef INCLUDE_TEX_SubPlasma
void TEXTURE::SubPlasma(RGBA* Layer, COMMAND* Parameters) {
	SUBPLASMATEMPLATE * s = (SUBPLASMATEMPLATE*)Parameters->data;
	unsigned char * buffer = new unsigned char[XRes*YRes];
	generate_subplasma(buffer, 1 << s->Size, s->RandSeed,XRes,YRes);
  for (int zx=0;zx<XRes;zx++)
    for (int zy=0;zy<YRes;zy++)
			Layer[zx+zy*XRes].dw=buffer[zx+zy*XRes]*0x01010101;
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_FractalPlasma
void TEXTURE::FractalPlasma(RGBA* Layer, COMMAND* Parameters) {
	FRACTALPLASMATEMPLATE * s = (FRACTALPLASMATEMPLATE*)Parameters->data;
	int opsize=XRes*YRes;
	unsigned char * buffer = new unsigned char[opsize];
	unsigned char * buffer2 = new unsigned char[opsize];
	memset(buffer2,0,opsize);
	for (int x=s->Min; x<=s->Max; x++) {
		generate_subplasma(buffer, 1 << x, s->RandSeed+x,XRes,YRes);
		int z = (s->Max-s->Min+1);
		float bf = (float)(s->Blend-0x80);
		float tf = (x - s->Min)/(float)(z-1)-0.5f;
		int masik = (int)((255/z) * ((bf*tf)/127.0f+0.5f) * 2);
		__asm{
			mov esi,buffer
			mov edi,buffer2
			mov ecx,opsize
			xor eax,eax
			xor edx,edx
			fploop:
				xor eax,eax
				lodsb
				mov ebx,ecx
				mov ecx,masik
        mul cl
				shr ax,8
				mov ecx,ebx
				add [edi],al
				inc edi
			loop fploop
		}
	}
  for (int zx=0;zx<XRes;zx++)
    for (int zy=0;zy<YRes;zy++)
			Layer[zx+zy*XRes].dw=buffer2[zx+zy*XRes]*0x01010101;
	delete[] buffer;
	delete[] buffer2;
}
#endif

#endif

#ifdef INCLUDE_TEX_Colorize
void TEXTURE::Colorize(RGBA* Layer, COMMAND* Parameters) {
	COLORIZETEMPLATE * c = (COLORIZETEMPLATE*)Parameters->data;
	unsigned int * lut = new unsigned int [256];
	for (int z=0; z<256; z++) lut[z]=interpolate_colors(c->StartColor,c->EndColor,z);
  for (int x=0; x<XRes*YRes; x++)
		Layer[x].dw=lut[Layer[x].c[c->DataChannel]];
	delete[] lut;
}
#endif

#ifdef INCLUDE_TEX_Emboss
void TEXTURE::Emboss(RGBA* Layer, COMMAND* Parameters) {
	unsigned int * buffer = new unsigned int[XRes*YRes];
	unsigned int nyaff =XRes*0x00010001;
	__asm{
		pxor mm7,mm7
		movd mm3,nyaff
		movq mm2,mm3  
		punpckldq mm3,mm2
	}
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++) {
				RGBA &mu0 = Layer[WRAPX(x-1)+WRAPY(y-1)*XRes];
				RGBA &mu1 = Layer[WRAPX(x-1)+WRAPY(y  )*XRes];
				RGBA &mu2 = Layer[WRAPX(x-1)+WRAPY(y+1)*XRes];
				RGBA &md0 = Layer[WRAPX(x+1)+WRAPY(y-1)*XRes];
				RGBA &md1 = Layer[WRAPX(x+1)+WRAPY(y  )*XRes];
				RGBA &md2 = Layer[WRAPX(x+1)+WRAPY(y+1)*XRes];
				RGBA * p = (RGBA*)(buffer+x+y*XRes);
				for (int c=0; c<4; c++) {
					int q=0;
					q-=SCALEX(mu0.c[c]);
					q-=SCALEX(mu1.c[c]);
					q-=SCALEX(mu2.c[c]);
					q+=SCALEX(md0.c[c]);
					q+=SCALEX(md1.c[c]);
					q+=SCALEX(md2.c[c]);
					p->c[c]=(q<-127?-127:(q>127?127:q))+0x80;
				}
			}

	__asm{ emms }

	memcpy_dw(Layer,buffer,XRes*YRes);

	delete[] buffer;

}
#endif

void apply_brightness(RGBA* Layer, unsigned char * brightmap, int rx, int ry) {
	for (int x=0; x<rx; x++) 
 		for (int y=0; y<ry; y++) {
			int ofs=x+y*rx;
			unsigned char d=brightmap[ofs];
			RGBA * p=Layer+ofs;
			if (d>128) {
				unsigned int dw=(unsigned char)(d-128)*0x01010101;
				__asm {
					pxor mm7,mm7
/**/			pcmpeqb mm6,mm6
					mov edi,p
					movd mm0,[edi]
					movd mm1,dw
/**/			movq mm2,mm0
/**/			pxor mm0,mm6
					punpcklbw mm0,mm7
					punpcklbw mm1,mm7
					pmullw mm0,mm1
					psrlw mm0,7
					packuswb mm0,mm7
/**/			paddusb mm0,mm2
					movd [edi],mm0
				}
			}
			else {
				unsigned int dw=d*0x01010101;
				__asm {
					pxor mm7,mm7
					mov edi,p
					movd mm0,[edi]
					movd mm1,dw
					punpcklbw mm0,mm7
					punpcklbw mm1,mm7
					pmullw mm0,mm1
					psrlw mm0,7
					packuswb mm0,mm7
					movd [edi],mm0
				}
			}
		}
	__asm{ emms };
}

#ifdef INCLUDE_TEX_Shade
void TEXTURE::Shade(RGBA* Layer, COMMAND* Parameters) {
	SHADETEMPLATE * s = (SHADETEMPLATE*)Parameters->data;
	int opsize=XRes*YRes;
	unsigned char * buffer = new unsigned char[opsize];
	unsigned char * l=(unsigned char*)(Layers[s->DataLayer].Data)+s->DataChannel;
	__asm{
		mov esi,l
		mov edi,buffer
		mov ecx,opsize
		shadeloop:
			movsb
			add esi,3
		loop shadeloop
	}
	apply_brightness(Layer,buffer,XRes,YRes);
	delete[] buffer;

}
#endif

#ifdef INCLUDE_TEX_Brightness
void TEXTURE::Brightness(RGBA* Layer, COMMAND* Parameters) {
	unsigned char * buffer = new unsigned char[XRes*YRes];
	memset(buffer,((BRIGHTNESSTEMPLATE*)Parameters->data)->Amount,XRes*YRes);
	apply_brightness(Layer,buffer,XRes,YRes);
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_Copy
void TEXTURE::Copy(RGBA* Layer, COMMAND* Parameters) {
	memcpy_dw(Layer,Layers[((COPYTEMPLATE*)Parameters->data)->DataLayer].Data,XRes*YRes);
}
#endif

#ifdef INCLUDE_TEX_Cells
void TEXTURE::Cells(RGBA* Layer, COMMAND* Parameters) {
	CELLSTEMPLATE * c = (CELLSTEMPLATE*)Parameters->data;

	float s=c->Power*5.0f*2.0f/((float)XRes);
	int x,y;

	int * disttab = new int[XRes*YRes];
	for (y=0; y<YRes; y++) 
		for (x=0; x<XRes; x++) {
			disttab[x+y*XRes]=(int)(sqrtf((float)(x*x+y*y))*s);
		}


	unsigned int dots[256][2];
	srand(c->RandSeed);
	for (int p=0; p<c->Points; p++) {
		dots[p][0]=SCALEX((unsigned char)rand());
		dots[p][1]=SCALEY((unsigned char)rand());
	}

	int XRes2=XRes/2;
	int YRes2=YRes/2;

	for (y=0; y<YRes; y++) 
		for (x=0; x<XRes; x++) {
			int r=255;
			for (p=0; p<c->Points; p++) {
				int sx=XRes2-WRAPX(x+dots[p][0]);
				int sy=YRes2-WRAPY(y+dots[p][1]);
				int r2 = disttab[abs(sx)+abs(sy)*XRes];
 				if (r2<r) r=r2;
				
			}
			Layer[x+y*XRes].dw = r*0x01010101;
		}
	delete[] disttab;
}
#endif

#ifdef INCLUDE_TEX_Twirl
void TEXTURE::Twirl(RGBA* Layer, COMMAND* Parameters) {
	unsigned int * buffer = new unsigned int[XRes*YRes];
	float deg = ((((TWIRLTEMPLATE*)Parameters->data)->Amount - 127) * 16.0f)*(3.14f/180.0f);
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
			int dx=x-(XRes/2);
			int dy=y-(YRes/2);
			float r = (float)sqrt((float)dx*dx+dy*dy)/(float)(XRes>>1);
			if (r>1) r=1;
			float phi = (float)cos(r*3.1415f/2.0f);	phi*=phi;
			float dpr =deg*phi;
			float	cdpr=(float)cos(dpr);
			float sdpr=(float)sin(dpr);
#ifndef __TEXGEN_BILINEAR__
			unsigned int xn = WRAPX((unsigned int)( dx*cdpr+dy*sdpr+(XRes>>1)));
			unsigned int yn = WRAPY((unsigned int)(-dx*sdpr+dy*cdpr+(YRes>>1)));
			buffer[x+y*XRes]=Layer[xn+yn*XRes].dw;
#else
			float xn =  dx*cdpr+dy*sdpr+(XRes>>1);
			float yn = -dx*sdpr+dy*cdpr+(YRes>>1);
			buffer[x+y*XRes]=Bilinear(Layer,(unsigned int)(xn*256),(unsigned int)(yn*256));
#endif
		}
	memcpy_dw(Layer,buffer,XRes*YRes);
	delete buffer;
}
#endif

#ifdef INCLUDE_TEX_SineDist
void TEXTURE::SineDist(RGBA* Layer, COMMAND* Parameters) {
	unsigned int * buffer = new unsigned int[XRes*YRes];
	SINEDISTTEMPLATE * s = (SINEDISTTEMPLATE*)Parameters->data;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
#ifndef __TEXGEN_BILINEAR__
			int xp = x+(int)(cos(y/(float)XRes * s->XSines * 2.0f * 3.14f)*SCALEX(s->XAmp));
      int yp = y+(int)(cos(x/(float)YRes * s->YSines * 2.0f * 3.14f)*SCALEY(s->YAmp));
			buffer[x+y*XRes]=Layer[WRAPX(xp)+WRAPY(yp)*XRes].dw;
#else
			float xp = x+((float)cos(y/(float)XRes * s->XSines * 2.0f * 3.14f)*SCALEX(s->XAmp));
      float yp = y+((float)cos(x/(float)YRes * s->YSines * 2.0f * 3.14f)*SCALEY(s->YAmp));
			buffer[x+y*XRes]=Bilinear(Layer,(unsigned int)(xp*256),(unsigned int)(yp*256));
#endif
		}
	memcpy_dw(Layer,buffer,XRes*YRes);
}
#endif

#ifdef INCLUDE_TEX_Mix
void TEXTURE::Mix(RGBA* Layer, COMMAND* Parameters) {
	MIXTEMPLATE * m = (MIXTEMPLATE*)Parameters->data;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++)
			Layer[x+y*XRes].dw=
				interpolate_colors(
					Layer[x+y*XRes].dw,
					Layers[m->DataLayer].Data[x+y*XRes].dw,
					m->Amount);
}
#endif

#ifdef INCLUDE_TEX_MixMap
void TEXTURE::MixMap(RGBA* Layer, COMMAND* Parameters) {
	MIXMAPTEMPLATE * m = (MIXMAPTEMPLATE*)Parameters->data;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++)
			Layer[x+y*XRes].dw=
				interpolate_colors(
					Layer[x+y*XRes].dw,
					Layers[m->DataLayer].Data[x+y*XRes].dw,
					Layers[m->MixLayer].Data[x+y*XRes].c[m->MixChannel]);
}
#endif

#ifdef INCLUDE_TEX_Invert
void TEXTURE::Invert(RGBA* Layer, COMMAND* Parameters) {
	int opsize=XRes*YRes;
	__asm{
		mov edi,Layer
		mov ecx,opsize
		invloop:
			mov eax,[edi]
			xor eax,0xFFFFFFFF
			stosd
		loop invloop
	}
}
#endif

#ifdef INCLUDE_TEX_Scale
void TEXTURE::Scale(RGBA* Layer, COMMAND* Parameters) {
	SCALETEMPLATE * s = (SCALETEMPLATE*)Parameters->data;
	if (s->Max <= s->Min) return;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
			Layer[x+y*XRes].c[s->DataChannel]=
				Layer[x+y*XRes].c[s->DataChannel] > s->Max ? s->Max :
				Layer[x+y*XRes].c[s->DataChannel] < s->Min ? s->Min :
				Layer[x+y*XRes].c[s->DataChannel];
		}
}
#endif

#ifdef INCLUDE_TEX_SineColor
void TEXTURE::SineColor(RGBA* Layer, COMMAND* Parameters) {
	SINECOLORTEMPLATE * s = (SINECOLORTEMPLATE*)Parameters->data;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
			int n = (int)(sin(Layer[x+y*XRes].c[s->DataChannel]*
				(3.14f/180.0f)*(255.0f/360.0f)*s->NumSines )*255.0f);
			Layer[x+y*XRes].c[s->DataChannel]=(unsigned char)abs(n);
		}
}
#endif

#ifdef INCLUDE_TEX_Max
void TEXTURE::Max(RGBA* Layer, COMMAND* Parameters) {
	int opsize=XRes * YRes; 
	RGBA * dlayer = Layers[((MAXTEMPLATE*)Parameters->data)->DataLayer].Data;
	__asm {
		mov ecx,opsize
		mov esi,dlayer
		mov edi,Layer
		pcmpeqb mm7,mm7
		pxor mm6,mm6
		addloop:
			movd mm0,[edi]
			movd mm1,[esi]

			punpcklbw mm0,mm6  // ki kell tomni wordbe mer pcmpgt elojeles
			punpcklbw mm1,mm6  // es igy nem lehet ket pixelt egyszerre :((((((

			movq mm2,mm0
			pcmpgtw mm2,mm1    

			movq mm3,mm0
			pand mm3,mm2
			pxor mm2,mm7
			pand mm1,mm2

			paddsw mm3,mm1			

			packuswb mm3,mm6

			movd [edi],mm3
			add edi,4
			add esi,4
		loop addloop
		emms
	}
}
#endif

#ifdef INCLUDE_TEX_HSV
RGBA hsv_to_rgb(float h,float s,float v)
{
   float r,g,b;

	 //h=fmod(f,360.0f);
   while (h<0.0) h += 360.0;
   while (h>=360.0) h -= 360.0;

   if (s==0.0) {
      r=v; g=v; b=v;
   }
   else
   {
      float f,p,q,t;
      float i;

      h /= 60.0;
      i = (float)(int)(h);
      f = h - i;
      p = (float)(v * (1.0-s));
      q = (float)(v * (1.0-(s*f)));
      t = (float)(v * (1.0-(s*(1.0-f))));

      int o = (int)i;
      switch(o) {
         case 0: r=v; g=t; b=p; break;
         case 1: r=q; g=v; b=p; break;
         case 2: r=p; g=v; b=t; break;
         case 3: r=p; g=q; b=v; break;
         case 4: r=t; g=p; b=v; break;
         case 5: r=v; g=p; b=q; break;
      } // switch
   }

   int ir,ig,ib;

   ir=(int)r;
   ig=(int)g;
   ib=(int)b;
   if (ir>255) ir = 255;
   if (ig>255) ig = 255;
   if (ib>255) ib = 255;

   RGBA out={ir,ig,ib,0};
   return out;
}

void rgb_to_hsv(float r,float g,float b,float* h,float* s,float* v)
{
   float max,min;
   //float temp;

   if (r>g)   max = r; else max = g;
   if (b>max) max = b;
   if (r<g)   min = r; else min = g;
   if (b<min) min = b;

   *v = max;

   *s = 0.0;
   if (max != 0.0) *s = ((max-min)/max);

   if (*s == 0.0) *h = -1;
   else {
      float delta = max - min;
      float maxr = max - r;
      float maxg = max - g;
      float maxb = max - b;

           if (r == max) *h =          ((maxb)-(maxg)) / delta;
      else if (g == max) *h = (2.0f + (((maxr)-(maxb)) / delta));
      else if (b == max) *h = (4.0f + (((maxg)-(maxr)) / delta));

      *h *= 60.0;
      while (*h < 0.0)    *h += 360.0;
      while (*h >= 360.0) *h -= 360.0;
   } 
}

void TEXTURE::HSV(RGBA* Layer, COMMAND* Parameters) {
	HSVTEMPLATE * q = (HSVTEMPLATE*)Parameters->data;
	float h,s,v;
	float nh=q->Hue;
	int ns=q->Saturation^0xFF;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
			rgb_to_hsv(
				Layer[x+y*XRes].r,
				Layer[x+y*XRes].g,
				Layer[x+y*XRes].b,
				&h,&s,&v);

			h=(float)(h+(float)nh*360.0/255.0);
			s=(float)(s*(float)ns/255.0);
			Layer[x+y*XRes]=hsv_to_rgb(h,s,v);
		}
}
#endif

#ifdef INCLUDE_TEX_Glass
void TEXTURE::Glass(RGBA* Layer, COMMAND* Parameters) {
	GLASSTEMPLATE * g = (GLASSTEMPLATE*)Parameters->data;
	RGBA * buffer = new RGBA[XRes*YRes];
	RGBA * dlayer = Layers[g->DataLayer].Data;
//	int a=g->Amount-127;
#ifdef __TEXGEN_BILINEAR__
	float a=((g->Amount-127)*XRes/256.0f)*XRes/256.0f;
#else
	int a=((g->Amount-127)*XRes/256)*XRes/256;
#endif
	int dc=g->DataChannel;
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++) {
			int ofs=x+y*XRes;
			int p=dlayer[x+y*XRes].c[dc];
			int dx=p - dlayer[WRAPX(x+1)+      y   *XRes].c[dc];
			int dy=p - dlayer[      x   +WRAPY(y+1)*XRes].c[dc];
#ifdef __TEXGEN_BILINEAR__
			float ox=x+(dx*a)/32.0f;
			float oy=y+(dy*a)/32.0f;
			buffer[ofs].dw=Bilinear(Layer,(unsigned int)(ox*256),(unsigned int)(oy*256));
#else
			unsigned int ox=(x+(dx*a)/32)&(XRes-1);
			unsigned int oy=(y+(dy*a)/32)&(XRes-1);
			buffer[ofs].dw=Layer[ox+oy*XRes].dw;
#endif
		}
	memcpy_dw(Layer,buffer,XRes*YRes);
	delete[] buffer;

}
#endif



#ifdef INCLUDE_TEX_Pixelize
void TEXTURE::Pixelize(RGBA* Layer, COMMAND* Parameters) {
	PIXELIZETEMPLATE * p = (PIXELIZETEMPLATE*)Parameters->data;
	int sxsize=max(1,XRes/p->XSquares);
	int sysize=max(1,YRes/p->YSquares);
	for (int x=0; x<p->XSquares+1; x++)
		for (int y=0; y<p->YSquares+1; y++)
			for (int c=0; c<4; c++) {
				unsigned int t=0;
				for (int zx=0; zx<sxsize && (x*sxsize+zx)<XRes; zx++)
					for (int zy=0; zy<sysize && (y*sysize+zy)<YRes; zy++)
						t+=Layer[(x*sxsize+zx)+(y*sysize+zy)*XRes].c[c];
				t/=sxsize*sysize;
				for (zx=0; zx<sxsize && (x*sxsize+zx)<XRes; zx++)
					for (int zy=0; zy<sysize && (y*sysize+zy)<YRes; zy++)
						Layer[(x*sxsize+zx)+(y*sysize+zy)*XRes].c[c]=t;
			}
	
}
#endif

#ifdef INCLUDE_TEX_Offset
void TEXTURE::Offset(RGBA* Layer, COMMAND* Parameters) {
	OFFSETTEMPLATE * o = (OFFSETTEMPLATE*)Parameters->data;
	RGBA * buffer = new RGBA[XRes*YRes];
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++)
			buffer[x+y*XRes]=
				Layer[WRAPX(x+SCALEX(o->X))+
				      WRAPY(y+SCALEY(o->Y))*XRes];
	memcpy_dw(Layer,buffer,XRes*YRes);
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_Crystalize
void TEXTURE::Crystalize(RGBA* Layer, COMMAND* Parameters) {
	CRYSTALIZETEMPLATE * c = (CRYSTALIZETEMPLATE*)Parameters->data;
	
	int z=c->Points*2;

	unsigned int dots[256*2][2];
	srand(c->RandSeed);
	for (int p=0; p<z; p++) {
		dots[p][0]=SCALEX((unsigned char)rand());
		dots[p][1]=SCALEY((unsigned char)rand());
	}

	RGBA * buffer = new RGBA[XRes*YRes];

	int XRes2=XRes/2;
	int YRes2=YRes/2;

//	float s=c->Power*5.0f*2.0f/((float)XRes);
	int cp=0;
	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
			int r=0x7FFFFFFF;
			for (p=0; p<z; p++) {
				int sx=XRes2-WRAPX(x+dots[p][0]);
				int sy=YRes2-WRAPY(y+dots[p][1]);
				int r2;
				__asm {
					fild sx
					fmul st(0),st(0)
					fild sy
					fmul st(0),st(0)
					faddp st(1),st(0)
					fsqrt
					fistp r2
				}
 				if (r2<r) { r=r2; cp=p; }
			}
			buffer[x+y*XRes].dw =
				Layer[(XRes-WRAPX(dots[cp][0]+XRes2))+
						  (YRes-WRAPY(dots[cp][1]+YRes2))*XRes].dw;
		}

	memcpy_dw(Layer,buffer,XRes*YRes);
	delete buffer;
}
#endif

#ifdef OLD_TEXGEN

#ifdef INCLUDE_TEX_Rectangle
void TEXTURE::Rectangle(RGBA* Layer, COMMAND* Parameters) {
	RECTANGLETEMPLATE * r = (RECTANGLETEMPLATE*)Parameters->data;

	for (int x=0; x<SCALEX(r->SizeX); x++)
		for (int y=0; y<SCALEY(r->SizeY); y++) 
			Layer[WRAPX(SCALEX(r->StartX)+x)+
			      WRAPY(SCALEY(r->StartY)+y)*YRes].dw=0xFFFFFFFF;
}
#endif

#else

#ifdef INCLUDE_TEX_Rectangle
void TEXTURE::Rectangle(RGBA* Layer, COMMAND* Parameters) {
	RECTANGLETEMPLATE * r = (RECTANGLETEMPLATE*)Parameters->data;
	memset(Layer,0,XRes*YRes*4);

	for (int x=0; x<SCALEX(r->SizeX+1); x++)
		for (int y=0; y<SCALEY(r->SizeY+1); y++) 
			Layer[WRAPX(SCALEX(r->StartX)+x)+
			      WRAPY(SCALEY(r->StartY)+y)*YRes].dw=0xFFFFFFFF;
}
#endif

#endif

#ifdef OLD_TEXGEN

void TEXTURE::Circle(RGBA* Layer, COMMAND* Parameters) {
	CIRCLETEMPLATE * r = (CIRCLETEMPLATE*)Parameters->data;
	int ir=SCALEX(r->InnerRadius);
	int or=SCALEX(r->InnerRadius+r->OuterRadius);
	float aar=XRes/256.0f;
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++) {
			int cx=WRAPX(x+SCALEX(r->X-128));
			int cy=WRAPY(y+SCALEY(r->Y-128));
			float dx=x-(XRes/2.0f);
			float dy=y-(XRes/2.0f);
			float d =(float)sqrt(dx*dx+dy*dy);
			if ((d>ir) && (d<or)) Layer[cx+cy*YRes].dw=0xFFFFFFFF;
			else if (fabs(d-ir)<aar)
				Layer[cx+cy*YRes].dw=((unsigned char)(255-fabs(d-ir)*255/aar))*0x01010101;
			else if (fabs(d-or)<aar)
				Layer[cx+cy*YRes].dw=((unsigned char)(255-fabs(d-or)*255/aar))*0x01010101;
			else Layer[cx+cy*YRes].dw=0;
			//fabs(d-or)<1.0
				
		}

}

#else

void circ(unsigned char * Layer, int qx, int qy, int XRes, int YRes, int innerr, int outerr, int color) {
	int ir=SCALEX(innerr);
	int or=SCALEX(innerr+outerr);
	float aar=XRes/256.0f;
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++) {
			int cx=WRAPX(x+SCALEX(qx-128));
			int cy=WRAPY(y+SCALEY(qy-128));
			float dx=x-(XRes/2.0f);
			float dy=y-(XRes/2.0f);
			float d =(float)sqrt(dx*dx+dy*dy);

			unsigned int c = 0;
			if ((d>ir) && (d<or)) c=0xFF;
			else if (fabs(d-ir)<aar) c=((unsigned char)(255-fabs(d-ir)*255/aar));
			else if (fabs(d-or)<aar) c=((unsigned char)(255-fabs(d-or)*255/aar));
			else c=0;
			Layer[cx+cy*YRes] = (c * color) >> 8;
			//fabs(d-or)<1.0
				
		}
}

#ifdef INCLUDE_TEX_Circle
void TEXTURE::Circle(RGBA* Layer, COMMAND* Parameters) {
	CIRCLETEMPLATE * r = (CIRCLETEMPLATE*)Parameters->data;
	unsigned char * buffer = new unsigned char[XRes*YRes];
	memset(buffer,0,XRes*YRes);
	circ(buffer,r->X,r->Y,XRes,YRes,r->InnerRadius,r->OuterRadius,0xFF);
  for (int zx=0;zx<XRes;zx++)
    for (int zy=0;zy<YRes;zy++)
			Layer[zx+zy*XRes].dw=buffer[zx+zy*XRes]*0x01010101;
	delete[] buffer;
}
#endif

#endif

#ifdef INCLUDE_TEX_Contrast
void TEXTURE::Contrast(RGBA* Layer, COMMAND* Parameters) {
	CONTRASTTEMPLATE * co = (CONTRASTTEMPLATE*)Parameters->data;
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++)
			for (int c=0; c<4; c++) {
				// amount;
				//   0 = grey image     = delta * 0
				// 128 = original image = delta * 1
				// 255 = full contrast  = delta * n

				int z =	((Layer[x+y*XRes].c[c]-0x80)*(co->Amount)/128)+0x80;
				Layer[x+y*XRes].c[c]=(z < 0 ? 0 : (z>255 ? 255 : z));
			}
}
#endif

#ifdef INCLUDE_TEX_MakeMaterial
void TEXTURE::MakeMaterial(RGBA* Layer, COMMAND* Parameters) {
}
#endif

#ifdef INCLUDE_TEX_Gradient
void TEXTURE::Gradient(RGBA* Layer, COMMAND* Parameters) {
	GRADIENTTEMPLATE * g = (GRADIENTTEMPLATE*)Parameters->data;
	for (int x=0; x<XRes; x++)
		for (int y=0; y<YRes; y++) {
			unsigned int ci0=interpolate_colors(g->_00,g->_10,(x<<8)/XRes);
			unsigned int ci1=interpolate_colors(g->_01,g->_11,(x<<8)/XRes);
			Layer[x+y*XRes].dw = interpolate_colors(ci0,ci1,(y<<8)/YRes);
		}
}
#endif

#ifdef INCLUDE_TEX_Rotozoom
void TEXTURE::Rotozoom(RGBA* Layer, COMMAND* Parameters) {
	ROTOZOOMTEMPLATE * r = (ROTOZOOMTEMPLATE*)Parameters->data;
	unsigned int * buffer = new unsigned int[XRes*YRes];

	float ZoomX=r->ZoomX;
	float ZoomY=r->ZoomY;
	
	if (r->ZoomIn) 
	{
		ZoomX = r->ZoomX/256.0f;
		ZoomY = r->ZoomY/256.0f;
	}

	for (int y=0; y<YRes; y++) 
		for (int x=0; x<XRes; x++) {
			int dx=x-SCALEX(r->CX);
			int dy=y-SCALEY(r->CY);
			float phi =r->Rotation*(2*3.1415f)/256.0f;
			float cdpr=(float)cos(phi);
			float sdpr=(float)sin(phi);
#ifndef __TEXGEN_BILINEAR__
			unsigned int xn = WRAPX( (unsigned int)( dx*cdpr+dy*sdpr+(XRes>>1))*ZoomX );
			unsigned int yn = WRAPY( (unsigned int)(-dx*sdpr+dy*cdpr+(YRes>>1))*ZoomY );
			buffer[x+y*XRes]=Layer[xn+yn*XRes].dw;
#else
			float xn = ( dx*cdpr+dy*sdpr+(XRes>>1))*ZoomX;
			float yn = (-dx*sdpr+dy*cdpr+(YRes>>1))*ZoomY;
			buffer[x+y*XRes]=Bilinear(Layer,(unsigned int)(xn*256),(unsigned int)(yn*256));
#endif
		}
	memcpy_dw(Layer,buffer,XRes*YRes);
	delete buffer;
}
#endif

void chamfrect(unsigned char * Layer,int _sx,int _sy,int ex,int ey, int XRes,int YRes,int cham,int color) {
	int sx = SCALEX(ex+1);
	int sy = SCALEY(ey+1);
	int x,y;
	for (x=0; x<sx; x++)
		for (y=0; y<sy; y++) 
			Layer[WRAPX(SCALEX(_sx)+x)+
			      WRAPY(SCALEY(_sy)+y)*XRes]=color;

	if (!cham) return;
	unsigned char MaxChamfer = sx > sy ? sy/2 : sx/2;
	unsigned char Chamfer = (MaxChamfer * cham)/255;

	for (x=0; x<Chamfer; x++)
		for (y=0; y<Chamfer; y++) {
			float dx = (float)x;
			float dy = (float)y;
			int or = Chamfer;
			float aar=XRes/256.0f;
			unsigned int c = 0;
			float d = (float)sqrt(dx*dx+dy*dy);
			if (d<or) c=0xFF;
			else if (fabs(d-or)<aar)
				c=((unsigned char)(255-fabs(d-or)*255/aar));
			else c=0;

			c = (c * color) >> 8;

			unsigned int cleft   = WRAPX(SCALEX(_sx)+(Chamfer-x-1));
			unsigned int ctop    = WRAPY(SCALEY(_sy)+(Chamfer-y-1));
			unsigned int cright  = WRAPX(SCALEX(_sx)+x+sx-Chamfer);
			unsigned int cbottom = WRAPY(SCALEY(_sy)+y+sy-Chamfer);

			Layer[cright+cbottom*XRes] = c;
			Layer[cleft +ctop   *XRes] = c;
			Layer[cright+ctop   *XRes] = c;
			Layer[cleft +cbottom*XRes] = c;
		}
}

#ifdef INCLUDE_TEX_ChamferRectangle
void TEXTURE::ChamferRectangle(RGBA* Layer, COMMAND* Parameters) {
	CHAMFERRECTANGLETEMPLATE * r = (CHAMFERRECTANGLETEMPLATE*)Parameters->data;

	unsigned char * buffer = new unsigned char[XRes*YRes];
	memset(buffer,0,XRes*YRes);
	chamfrect(buffer,r->StartX,r->StartY,r->SizeX,r->SizeY,XRes,YRes,r->Chamfer,0xFF);
  for (int zx=0;zx<XRes;zx++)
    for (int zy=0;zy<YRes;zy++)
			Layer[zx+zy*XRes].dw=buffer[zx+zy*XRes]*0x01010101;
	delete[] buffer;
}
#endif

#ifdef INCLUDE_TEX_Dots
void TEXTURE::Dots(RGBA* Layer, COMMAND* Parameters) {
	DOTSTEMPLATE * r = (DOTSTEMPLATE*)Parameters->data;

	memset(Layer,0,XRes*YRes*4);

	srand(r->RandSeed);

	unsigned char * buffer  = new unsigned char[XRes*YRes];
	unsigned char * bufferm = new unsigned char[XRes*YRes];
	memset(bufferm,0,XRes*YRes);

  for (int x=0; x<r->Count; x++) {
		memset(buffer,0,XRes*YRes);
		int radius = r->SizeMin + (rand()*(r->SizeMax-r->SizeMin)/RAND_MAX);
		int color  = r->Min     + (rand()*(r->Max    -r->Min    )/RAND_MAX);
		int cx = rand();
		int cy = rand();
		if (r->Rounded) {
			circ(buffer,SCALEX(cx),SCALEX(cy),XRes,YRes,0,radius,color);
		} else {
			chamfrect(buffer,SCALEX(cx),SCALEX(cy),SCALEX(radius),SCALEX(radius),XRes,YRes,0,color);
		}
		for (int zx=0;zx<XRes;zx++)
			for (int zy=0;zy<YRes;zy++) {
				unsigned char c = buffer[zx+zy*XRes];
				bufferm[zx+zy*XRes]=
					(bufferm[zx+zy*XRes] > c) ? bufferm[zx+zy*XRes] : c;
			}
	}

	for (int zx=0;zx<XRes;zx++)
		for (int zy=0;zy<YRes;zy++) {
			Layer[zx+zy*XRes].dw = bufferm[zx+zy*XRes]*0x01010101;
		}

	delete[] bufferm;
	delete[] buffer;

}
#endif

#ifdef INCLUDE_TEX_Jpeg
#include "jpeglib.h"
#pragma comment(lib,"libjpeg.lib")

void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData)
{
	int rowsRead = 0;
	unsigned char** rowPtr;
	int i;

	jpeg_read_header(cinfo, TRUE);
	jpeg_start_decompress(cinfo);

	pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
	pImageData->sizeX   = cinfo->image_width;
	pImageData->sizeY   = cinfo->image_height;
	
	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
		
	rowPtr = new unsigned char*[pImageData->sizeY];
	
	for (i = 0; i < pImageData->sizeY; i++)
		rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);

	while (cinfo->output_scanline < cinfo->output_height) 
		rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height - rowsRead);
	
	delete [] rowPtr;

	jpeg_finish_decompress(cinfo);
}

tImageJPG *LoadJPG(unsigned char *filename, int filesize)
{
	struct jpeg_decompress_struct cinfo;
	tImageJPG *pImageData = NULL;
		
	jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, filename, filesize);
	pImageData = new tImageJPG;
	DecodeJPG(&cinfo, pImageData);
	jpeg_destroy_decompress(&cinfo);
	
	return pImageData;
}

void TEXTURE::Jpeg(RGBA* Layer, COMMAND* Parameters) 
{
	JPEGTEMPLATE * r = (JPEGTEMPLATE*)Parameters->data;
	if (r->JPEGData)
	{
		tImageJPG *a=LoadJPG(r->JPEGData,r->JPEGDataSize);
		for (int x=0; x<min(XRes,a->sizeX); x++)
			for (int y=0; y<min(YRes,a->sizeY); y++)
			{
				int p=(y*a->sizeX+x)*3;
				Layer[XRes*y+x].r=a->data[p];
				Layer[XRes*y+x].g=a->data[p+1];
				Layer[XRes*y+x].b=a->data[p+2];
			}
		delete[] a->data;
		delete a;
	}
}
#endif

typedef void (TEXTURE::*GENCALLBACK)(RGBA*,COMMAND*);

GENCALLBACK jumptable[]={
#ifdef INCLUDE_TEX_Envmap
	&TEXTURE::Envmap, //0
#else
		0,
#endif

#ifdef INCLUDE_TEX_Plasma
	&TEXTURE::Plasma, //1
#else
		0,
#endif

#ifdef INCLUDE_TEX_Map
	&TEXTURE::Map, //2
#else
		0,
#endif

#ifdef INCLUDE_TEX_Blur
	&TEXTURE::Blur, //3
#else
		0,
#endif
	
#ifdef INCLUDE_TEX_DirBlur
	&TEXTURE::DirBlur, //4
#else
		0,
#endif

#ifdef INCLUDE_TEX_Text
	&TEXTURE::Text, //5
#else
		0,
#endif

#ifdef INCLUDE_TEX_SubPlasma
	&TEXTURE::SubPlasma, //6
#else
		0,
#endif

#ifdef INCLUDE_TEX_FractalPlasma
	&TEXTURE::FractalPlasma, //7
#else
		0,
#endif

#ifdef INCLUDE_TEX_Colorize
	&TEXTURE::Colorize, //8
#else
		0,
#endif

#ifdef INCLUDE_TEX_Shade
	&TEXTURE::Shade, //9
#else
		0,
#endif

#ifdef INCLUDE_TEX_Brightness
	&TEXTURE::Brightness, //10
#else
		0,
#endif

#ifdef INCLUDE_TEX_Copy
	&TEXTURE::Copy, //11
#else
		0,
#endif

#ifdef INCLUDE_TEX_Cells
	&TEXTURE::Cells, //12
#else
		0,
#endif

#ifdef INCLUDE_TEX_Twirl
	&TEXTURE::Twirl, //13
#else
		0,
#endif

#ifdef INCLUDE_TEX_SineDist
	&TEXTURE::SineDist, //14
#else
		0,
#endif

#ifdef INCLUDE_TEX_Mix
	&TEXTURE::Mix, //15
#else
		0,
#endif

#ifdef INCLUDE_TEX_MixMap
	&TEXTURE::MixMap, //16
#else
		0,
#endif

#ifdef INCLUDE_TEX_Scale
	&TEXTURE::Scale, //17
#else
		0,
#endif

#ifdef INCLUDE_TEX_SineColor
	&TEXTURE::SineColor, //18
#else
		0,
#endif

#ifdef INCLUDE_TEX_Max
	&TEXTURE::Max, //19
#else
		0,
#endif

#ifdef INCLUDE_TEX_HSV
	&TEXTURE::HSV, //20
#else
		0,
#endif

#ifdef INCLUDE_TEX_Emboss
	&TEXTURE::Emboss, //21
#else
		0,
#endif

#ifdef INCLUDE_TEX_Invert
	&TEXTURE::Invert, //22
#else
		0,
#endif

#ifdef INCLUDE_TEX_Glass
	&TEXTURE::Glass, //23
#else
		0,
#endif

#ifdef INCLUDE_TEX_Pixelize
	&TEXTURE::Pixelize, //24
#else
		0,
#endif

#ifdef INCLUDE_TEX_Offset
	&TEXTURE::Offset, //25
#else
		0,
#endif

#ifdef INCLUDE_TEX_Crystalize
	&TEXTURE::Crystalize, //26
#else
		0,
#endif

#ifdef INCLUDE_TEX_Rectangle
	&TEXTURE::Rectangle, //27
#else
		0,
#endif

#ifdef INCLUDE_TEX_Circle
	&TEXTURE::Circle, //28
#else
		0,
#endif

#ifdef INCLUDE_TEX_Contrast
	&TEXTURE::Contrast, //29
#else
		0,
#endif

#ifdef INCLUDE_TEX_MakeMaterial
	&TEXTURE::MakeMaterial, //30
#else
		0,
#endif

#ifdef INCLUDE_TEX_Gradient
	&TEXTURE::Gradient, //31
#else
		0,
#endif

#ifdef INCLUDE_TEX_Rotozoom
	&TEXTURE::Rotozoom, //32
#else
		0,
#endif

#ifdef INCLUDE_TEX_ChamferRectangle
	&TEXTURE::ChamferRectangle, //33
#else
		0,
#endif

#ifdef INCLUDE_TEX_Dots
	&TEXTURE::Dots, //34
#else
		0,
#endif

#ifdef INCLUDE_TEX_Jpeg
	&TEXTURE::Jpeg, //35
#else
		0,
#endif


};

void TEXTURE::PerformCommand(RGBA* Layer, COMMAND* Parameters)
{
	(this->*jumptable[Parameters->Filter])(Layer,Parameters);
}
