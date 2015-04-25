#ifndef __TexGen__
#define __TexGen__
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <windows.h>
#include <gl\gl.h>

#define pi 3.141592654
#define radtheta 0.017453292

// I wonder why the effect IDs match with the ATG2 effect IDs...

#define DD_fractal   1
#define DD_plasma    2
#define DD_cells     3
#define DD_envmap    4
#define DD_subplasm  5
#define DD_clear     6
#define DD_text      7

#define DD_sindist   10
#define DD_offset    11
#define DD_twirl     12
#define DD_blur      15
#define DD_map       17
#define DD_dirblur   18
#define DD_xchng     20
#define DD_copy      23
#define DD_mix       24
#define DD_mul       25
#define DD_add       26
#define DD_max       27
#define DD_contrast  30
#define DD_invert    31
#define DD_shade     32
#define DD_bright    33
#define DD_sincol    34
#define DD_scale     35
#define DD_hsv       36
#define DD_colorize  37
#define DD_mixmap    38

#define DD_emboss    39
#define DD_stored    255

typedef unsigned char byte;
typedef union tRGBA {
	struct {byte r,g,b,a;};
	byte   cols[4];

} rgba;

typedef struct hsv_ {float h,s,v;} hsv;

typedef struct {
				char text[256];
				} string;

typedef union {
				struct {
						byte commandnumber,
							 layer,
							 datalayer1,
							 datalayer2,
							 param1,
							 param2,
							 param3,
							 param4,
							 param5;
						};
				byte command[9];


				} texturecommand;

struct texture {
				byte           commandnum;
				texturecommand commands[100];
	            rgba           layers[4][256][256];				
				string		   texts[100];
				texture        *next;
				int            ID;
				};

struct material {
				 unsigned char texture;
				 unsigned char layer;
				 unsigned char alphatexture;
				 unsigned char alphalayer;
				 unsigned char alphamode;
				 unsigned char number;
				 GLuint        handle;
				 rgba          image[256][256];
				 material      *next;
				};

void inittexture(texture &t);
void performcommand(texture &t, texturecommand c);
void inittextureengine();
#endif