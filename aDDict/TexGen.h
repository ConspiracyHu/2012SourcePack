#ifndef __TexGen__
#define __TexGen__
#include <math.h>

extern char *fonts[8];

#define pi 3.141592654
#define radtheta 0.017453292

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
	struct {byte r,g,b,a;} a;
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
						} c;
				byte command[9];


				} texturecommand;

class texture {
                public:
				byte           commandnum;
				texturecommand commands[100];				
	            rgba           layers[4][256][256];				
				string		   texts[100];

				void performcommand(texturecommand command);
				void init();

                void fractal(texturecommand incmnd);
                void plasma(texturecommand incmnd);
                void cells(texturecommand incmnd);
                void envmap(texturecommand incmnd);
                void subplasma(texturecommand incmnd);
                void clear(texturecommand incmnd);

                void sinustors(texturecommand incmnd);
                void offset(texturecommand incmnd);
                void twirl(texturecommand incmnd);
                void map(texturecommand incmnd);
                void dirblur(texturecommand incmnd);

                void xch(texturecommand incmnd);
                void copy(texturecommand incmnd);
                void mix(texturecommand incmnd);
                void mul(texturecommand incmnd);
                void add(texturecommand incmnd);
                void max_(texturecommand incmnd);
                void invert(texturecommand incmnd);
                void shade(texturecommand incmnd);
                void sincol(texturecommand incmnd);
                void scale(texturecommand incmnd);
                void dohsv(texturecommand incmnd);
                void emboss(texturecommand incmnd);
                void blur(texturecommand incmnd);
                void colorize(texturecommand incmnd);
                void bright(texturecommand incmnd);
                void contrast(texturecommand incmnd);
                void mixmap(texturecommand incmnd);

                void text(texturecommand incmnd);

				};

//void texture::init();
//void texture::performcommand(texturecommand incmnd);
void inittextureengine();
#endif