#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "texgen.h"

char *fonts[8];

rgba buffer[256][256];
float distbuf[256][256];

int xc,yc,zc;

float sqr(float x) {return x*x;}
float frac(float x) {return (float)(x-floor(x));}

unsigned int rand_(byte r4, byte r3,byte r2,byte r1)
{
 return r1+r2*256+r3*65536+r4*65536*256;
}

///////////////// Spline ////////////////////

byte catmullrom_interpolate(float v0, float v1, float v2, float v3, float x, float distance)
{
	int t;
	float xx = x / distance;
	float P = (v3 - v2) - (v0 - v1);
	float Q = (v0 - v1) - P;
	float R =  v2 - v0;
	t=(int)((P*xx*xx*xx) + (Q*xx*xx) + (R*xx) + v1);
	if (t<0) t=0; if (t>255) t=255;
	return (byte)t;
}


/////////////////// HSV //////////////////////

rgba hsv_to_rgb(float h,float s,float v)
{
   float r,g,b;

   while (h<0.0) h += 360.0;
   while (h>=360.0) h -= 360.0;

   if (s==0.0) {
      r=v;
      g=v;
      b=v;
   }
   else
   {
      float f,p,q,t;
      float i;

      h /= 60.0;
      i = (float)floor(h);
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

   rgba out;

   out.a.r=ir;
   out.a.g=ig;
   out.a.b=ib;
   out.a.a=0;

   return out;
}

void rgb_to_hsv(float r,float g,float b,float* h,float* s,float* v)
{
   float max,min;
   //float temp;

   if (r>g)
      max = r;
   else
      max = g;

   if (b>max)
      max = b;

   if (r<g)
      min = r;
   else
      min = g;

   if (b<min)
      min = b;

   *v = max;

   *s = 0.0;
   if (max != 0.0)
      *s = ((max-min)/max);

   if (*s == 0.0)
      *h = -1;
   else {
      float delta = max - min;
      float maxr = max - r;
      float maxg = max - g;
      float maxb = max - b;

      if (r == max)
         *h = ((maxb)-(maxg)) / delta;
      else if (g == max)
         *h = (float)(2.0 + (((maxr)-(maxb)) / delta));
      else if (b == max)
         *h = (float)(4.0 + (((maxg)-(maxr)) / delta));

      *h *= 60.0;
      while (*h < 0.0)
         *h += 360.0;
      while (*h >= 360.0)
         *h -= 360.0;
   } 
}

void texture::init()
{
 int x;
 memset(layers,0,sizeof(layers));
 memset(commands,0,sizeof(commands));
 for (x=0;x<100;x++)
 {
	 memset(texts[x].text,0,256);
 }
 commandnum=0;

}

void dosub(byte size, byte col)
{
  unsigned int x,y,z;
  unsigned int psize,pnum;

  pnum = 1 << (8-size);
  psize = 1 << size;
  
  for (y = 0; y<pnum; y++)
    for (x = 0; x<pnum; x++)
      buffer[y << size][x << size].cols[col] = rand();

  for (x = 0; x<pnum; x++)
    for (y = 0; y<pnum; y++)
      for (z = 0; z<(psize); z++)
        buffer[(y << size) + z][x << size].cols[col] =
          catmullrom_interpolate(
            buffer[byte((y-1) << size)][x << size].cols[col],
            buffer[byte(( y ) << size)][x << size].cols[col],
            buffer[byte((y+1) << size)][x << size].cols[col],
            buffer[byte((y+2) << size)][x << size].cols[col],
            (float)((float)z / (float)psize),1);

  for (y = 0; y<256; y++)
    for (x = 0; x<pnum; x++)
      for (z = 0; z<(psize); z++)
        buffer[y][(x << size) + z].cols[col] =
          catmullrom_interpolate(
            buffer[y][byte((x-1) << size)].cols[col],
            buffer[y][byte(( x ) << size)].cols[col],
            buffer[y][byte((x+1) << size)].cols[col],
            buffer[y][byte((x+2) << size)].cols[col],
            (float)((float)z / (float)psize), 1);
}


void texture::performcommand(texturecommand incmnd)
{
 switch (incmnd.c.commandnumber)
 { case DD_fractal  : fractal(incmnd); break;
   case DD_plasma   : plasma(incmnd); break;
   case DD_cells    : cells(incmnd); break;
   case DD_envmap   : envmap(incmnd); break;
   case DD_subplasm : subplasma(incmnd); break;
   case DD_clear    : clear(incmnd); break;
   case DD_text     : text(incmnd); break;
   case DD_sindist  : sinustors(incmnd); break;
   case DD_offset   : offset(incmnd); break;
   case DD_twirl    : twirl(incmnd); break;
   case DD_blur     : blur(incmnd); break;
   case DD_map      : map(incmnd); break;
   case DD_dirblur  : dirblur(incmnd); break;
   case DD_xchng    : xch(incmnd); break;
   case DD_copy     : copy(incmnd); break;
   case DD_mix      : mix(incmnd); break;
   case DD_mul      : mul(incmnd); break;
   case DD_add      : add(incmnd); break;
   case DD_max      : max_(incmnd); break;
   case DD_contrast : contrast(incmnd); break;
   case DD_invert   : invert(incmnd); break;
   case DD_shade    : shade(incmnd); break;
   case DD_bright   : bright(incmnd); break;
   case DD_sincol   : sincol(incmnd); break;
   case DD_scale    : scale(incmnd); break;
   case DD_hsv      : dohsv(incmnd); break;
   case DD_colorize : colorize(incmnd); break;
   case DD_mixmap   : mixmap(incmnd); break;
   case DD_emboss   : emboss(incmnd); break;
 }
}

void texture::fractal(texturecommand incmnd)
{
  unsigned int x,y;
  byte b;
  
  srand(rand_(incmnd.command[4], incmnd.command[5], incmnd.command[6], incmnd.command[7]));
  for (x = 0; x<256;x++)
	for (y = 0; y<256;y++)
    {
      layers[incmnd.c.layer][x][y].cols[incmnd.command[2]] = 0;
      buffer[x][y].cols[incmnd.command[2]] = 0;
    }
  for (b = 1; b<8; b++)
    {
      dosub(b, incmnd.command[2]);
      for (x = 0; x<256;x++)
        for (y = 0; y<256;y++)
		 layers[incmnd.c.layer][x][y].cols[incmnd.command[2]] += buffer[x][y].cols[incmnd.command[2]] >> (8-b);
    }
}

void texture::plasma(texturecommand incmnd)
{
 int xc,yc;
 for (yc = 0; yc<256; yc++) 
  for (xc = 0; xc<256; xc++) 
  {
	 layers[incmnd.c.layer][yc][xc].cols[incmnd.command[8]] = 
	                       (byte)(127.0 + 
	                       63.5*sin((xc+incmnd.command[4])/256.0 * incmnd.command[2] * 2.0*pi) +
                           63.5*sin((yc+incmnd.command[5])/256.0 * incmnd.command[3] * 2.0*pi));
   
  }
}

void texture::cells(texturecommand incmnd)
{
  
  int i;
  float r,r2,s;
  byte dots[256][2];

  srand(rand_(incmnd.command[5], incmnd.command[6], incmnd.command[7], incmnd.command[8]));
  for (xc = 0; xc<incmnd.command[3];xc++)
    {
      dots[xc][0] = (byte)(rand());
      dots[xc][1] = (byte)(rand());
    }

  s = (float)(incmnd.command[4] * 5.0);
  for (yc = 0; yc<256; yc++)
    for (xc = 0; xc<256; xc++)
      {
        r = 255.0;
        for (i = 0; i<incmnd.command[3]; i++)
          {
            r2 = distbuf[(byte)(yc + dots[i][1]) ][(byte)(xc + dots[i][0])] * s;
            if (r2 < r) r = r2;
          }
        if (r > 255.0) r = 255.0;
		if (r < 0.0) r = 0.0;
        layers[incmnd.c.layer][yc][xc].cols[incmnd.command[2]] = (byte)r;
      }
}

void texture::envmap(texturecommand incmnd)
{
  float dist, distcntr;
  byte c;
	
  if (incmnd.command[2] == 0)
    {
      distcntr = 1;
      for (yc = 0; yc<256; yc++)
        for (xc = 0; xc<256; xc++)
          {
            dist = distbuf[yc][xc];
            if (dist <= distcntr) c = (byte)(255 - dist*255); else c = 0;
            layers[incmnd.c.layer][yc][xc].a.r = c;
            layers[incmnd.c.layer][yc][xc].a.g = c;
            layers[incmnd.c.layer][yc][xc].a.b = c;
          }
    }
  else
    {
      distcntr = (float)(1.0 - ((float)incmnd.command[4]/512.0));
      for (yc = 0; yc<256; yc++)
        for (xc = 0; xc<256; xc++)
          {
            dist = distbuf[yc][xc];
            if (dist <= distcntr) c = (byte)(255 - (dist/distcntr)*255); else c = 0;
            layers[incmnd.c.layer][yc][xc].cols[incmnd.command[3]] = c;
          }
    }
}

void texture::subplasma(texturecommand incmnd)
{
  int x,y;
  byte size,col;
	
  col = incmnd.command[2];
  if (incmnd.command[3] == 0) return;
  size = 0;
  while ( ( (incmnd.command[3]) & (1 << size) ) == 0) size++;
  
  srand(rand_(incmnd.command[4], incmnd.command[5], incmnd.command[6], incmnd.command[7]));
  
  dosub(size,col);

  for (y = 0; y<256; y++)
    for (x = 0; x<256; x++)
     layers[incmnd.c.layer][y][x].cols[col] = buffer[y][x].cols[col];

}

void texture::clear(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<4; zc++)
    layers[incmnd.c.layer][xc][yc].cols[zc]=0;
}

void texture::sinustors(texturecommand incmnd)
{
  float xp,yp;
  float xfrc,yfrc,emxfrc,emyfrc,a,b,c,d;
  byte xflr,yflr,xflrp,yflrp;
  tRGBA buff[4];
  int i;
  for (yc=0; yc<256; yc++)
    for (xc=0; xc<256; xc++)
      {
		xp = (float)(xc+  cos((yc+incmnd.command[6])/256.0 * incmnd.command[2] * 2.0*pi) * incmnd.command[4] );
        yp = (float)(yc+  cos((xc+incmnd.command[7])/256.0 * incmnd.command[3] * 2.0*pi) * incmnd.command[5] );
		
		xfrc=frac(xp);
		xflr=(byte)floor(xp);
		xflrp=(byte)(xflr+1);
		emxfrc=(float)1.0-xfrc;

		yfrc=frac(yp);
		yflr=(byte)floor(yp);
		yflrp=(byte)(yflr+1);
		emyfrc=(float)1.0-yfrc;

		a=(emxfrc)*(emyfrc);
		b=(  xfrc)*(emyfrc);
		c=(emxfrc)*(  yfrc);
		d=(  xfrc)*(  yfrc);
		
		buff[0] = layers[incmnd.c.layer][ yflr  ][ xflr  ];
		buff[1] = layers[incmnd.c.layer][ yflr  ][ xflrp ];
		buff[2] = layers[incmnd.c.layer][ yflrp ][ xflr  ];
		buff[3] = layers[incmnd.c.layer][ yflrp ][ xflrp ];

        for (i = 0; i<3; i++)
          buffer[yc][xc].cols[i] = (byte)(
            a*buff[0].cols[i] +
            b*buff[1].cols[i] +
            c*buff[2].cols[i] +
            d*buff[3].cols[i] );
      }

  for (yc = 0; yc<256; yc++)
    for (xc = 0; xc<256; xc++)
     layers[incmnd.c.layer][yc][xc] = buffer[yc][xc];

}

void texture::offset(texturecommand incmnd)
{
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   buffer[xc][yc]=layers[incmnd.c.layer][(byte)(xc-incmnd.command[2])][(byte)(yc-incmnd.command[3])];
 
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   layers[incmnd.c.layer][yc][xc] = buffer[yc][xc];
}
 

void texture::twirl(texturecommand incmnd)
{
  float deg,xn,yn,r,phi;
  float xfrc,yfrc,emxfrc,emyfrc,a,b,c,d,cdpr,sdpr;
  byte xflr,yflr,xflrp,yflrp;
  float xcm128,ycm128,dpr;
  tRGBA buff[4];
  int i;
	
  deg = (float)(((float)incmnd.command[2] - 128.0) * 16.0);
  for (yc = 0; yc<256; yc++)
    for (xc = 0; xc<256; xc++)
      {
		xcm128=(float)xc-128;
		ycm128=(float)yc-128;

		r = (float)(sqrt(sqr((float)(xcm128)) + sqr((float)(ycm128)))/128.0);
        if (r > 1)  r = 1;
        phi = (float)sqr((float)cos(r*pi/2));
		dpr = (float)(deg*phi*radtheta);
		cdpr=(float)cos(dpr);
		sdpr=(float)sin(dpr);

        xn = (float)( (xcm128)*cdpr + (ycm128)*sdpr + 128.0);
        yn = (float)(-(xcm128)*sdpr + (ycm128)*cdpr + 128.0);

        if (xn < 0.0)  xn = 0; if (yn < 0.0)  yn = 0;
        if (xn > 255.0)  xn = 255; if (yn > 255.0) yn = 255;

		xfrc=frac(xn);
		xflr=(byte)floor(xn);
		xflrp=(byte)(xflr+1);
		emxfrc=(float)1.0-xfrc;

		yfrc=frac(yn);
		yflr=(byte)floor(yn);
		yflrp=(byte)(yflr+1);
		emyfrc=(float)1.0-yfrc;

		a=(emxfrc)*(emyfrc);
		b=(  xfrc)*(emyfrc);
		c=(emxfrc)*(  yfrc);
		d=(  xfrc)*(  yfrc);

		buff[0]=layers[incmnd.c.layer][ yflr  ][ xflr  ];
		buff[1]=layers[incmnd.c.layer][ yflr  ][ xflrp ];
		buff[2]=layers[incmnd.c.layer][ yflrp ][ xflr  ];
		buff[3]=layers[incmnd.c.layer][ yflrp ][ xflrp ];
		
        for (i = 0; i<3; i++)
          buffer[yc][xc].cols[i] = (byte)(
            a*buff[0].cols[i] +
            b*buff[1].cols[i] +
            c*buff[2].cols[i] +
            d*buff[3].cols[i] );
      }
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   layers[incmnd.c.layer][yc][xc] = buffer[yc][xc];
}

void texture::map(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
	  buffer[xc][yc]=layers[incmnd.c.layer][(byte)((float)(xc)+(float)(layers[incmnd.command[2]][xc][yc].cols[incmnd.command[3]])/255.0*(float)(incmnd.command[4]))]
                                           [(byte)((float)(yc)+(float)(layers[incmnd.command[5]][xc][yc].cols[incmnd.command[6]])/255.0*(float)(incmnd.command[7]))];    

  }
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   layers[incmnd.c.layer][yc][xc] = buffer[yc][xc];
}

void texture::dirblur(texturecommand incmnd)
{
 float b1,b2;
 int countr,countg,countb;
 byte p1,p2;

	
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
    int dir=(int)( (float)(layers[incmnd.command[2]][xc][yc].cols[incmnd.command[3]])*360.0/256.0 ) +270;
    countr=0;
    countg=0;
    countb=0;
    b1=(float)cos(dir*radtheta);
    b2=(float)sin(dir*radtheta);
    for (zc=1;zc<=incmnd.command[4];zc++)
     {
      p1=(byte)( (float)xc + b1*zc );
      p2=(byte)( (float)yc + b2*zc );
      countr+=layers[incmnd.c.layer][p1][p2].a.r;
      countg+=layers[incmnd.c.layer][p1][p2].a.g;
      countb+=layers[incmnd.c.layer][p1][p2].a.b;
     }
    if (zc!=0)
    {
     countr=(int)((float)(countr) / (float)zc);
     countg=(int)((float)(countg) / (float)zc);
     countb=(int)((float)(countb) / (float)zc);
    }
    buffer[xc][yc].a.r=countr;
    buffer[xc][yc].a.g=countg;
    buffer[xc][yc].a.b=countb;
   }
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   layers[incmnd.c.layer][yc][xc] = buffer[yc][xc];
}

void texture::xch(texturecommand incmnd)
{
 byte buf;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
    buf=layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]];
    layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]]=layers[incmnd.command[3]][xc][yc].cols[incmnd.command[4]];
    layers[incmnd.command[3]][xc][yc].cols[incmnd.command[4]]=buf;
   }
}

void texture::copy(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]]=
    layers[incmnd.command[3]][xc][yc].cols[incmnd.command[4]];
}

void texture::mix(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    layers[incmnd.c.layer][xc][yc].cols[zc]=(byte)(((float)layers[incmnd.c.layer][xc][yc].cols[zc]/255.0)*(float)incmnd.command[3]+((float)layers[incmnd.command[2]][xc][yc].cols[zc]/255.0)*(255.0-(float)incmnd.command[3]));
}

void texture::mul(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    layers[incmnd.c.layer][xc][yc].cols[zc]=(byte)( (float)(layers[incmnd.c.layer][xc][yc].cols[zc])*
	                                                (float)(layers[incmnd.command[2]][xc][yc].cols[zc]) / 255.0);
}

void texture::add(texturecommand incmnd)
{
 byte cl;
 int acc;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (cl=0; cl<3; cl++)
    {
     acc=layers[incmnd.command[2]][xc][yc].cols[cl]+layers[incmnd.c.layer][xc][yc].cols[cl];
     if (acc>255)  acc=255;
     if (acc<0)  acc=0;
     layers[incmnd.c.layer][xc][yc].cols[cl]=acc;
    }
}

void texture::max_(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    if (layers[incmnd.c.layer][xc][yc].cols[zc] < layers[incmnd.command[2]][xc][yc].cols[zc])  layers[incmnd.c.layer][xc][yc].cols[zc]=layers[incmnd.command[2]][xc][yc].cols[zc];
}

void texture::invert(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    layers[incmnd.c.layer][xc][yc].cols[zc]=255-layers[incmnd.c.layer][xc][yc].cols[zc];
}

void texture::shade(texturecommand incmnd)
{
 float coll,c2;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
    coll=(float)((float)(layers[incmnd.command[2]][xc][yc].cols[incmnd.command[3]])/127.0);
    c2=(float)((255.0-(float)layers[incmnd.command[2]][xc][yc].cols[incmnd.command[3]])/127.0);
    for (zc=0; zc<3; zc++)
     {
      if (coll<=1)  layers[incmnd.c.layer][xc][yc].cols[zc]=(byte)((float)layers[incmnd.c.layer][xc][yc].cols[zc]*coll);
      else layers[incmnd.c.layer][xc][yc].cols[zc]=255-(byte)((255.0-(float)layers[incmnd.c.layer][xc][yc].cols[zc])*c2);
     }
   }
}

void texture::sincol(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]]=(byte)(sin( layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]]* radtheta* (255.0/360.0)* (float)incmnd.command[3] )*255.0);
}

void texture::scale(texturecommand incmnd)
{
 int b;
 float d;
 b=incmnd.command[4]-incmnd.command[3];
 d=(float)((float)b/255.0);
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]]=(byte)((float)incmnd.command[3]+d*(float)layers[incmnd.c.layer][xc][yc].cols[incmnd.command[2]]);
}

void texture::dohsv(texturecommand incmnd)
{
 rgba a;
 hsv b;
 byte h,s;
 h=incmnd.command[2];
 s=255-incmnd.command[3];
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
    rgb_to_hsv(layers[incmnd.c.layer][xc][yc].a.r,layers[incmnd.c.layer][xc][yc].a.g,layers[incmnd.c.layer][xc][yc].a.b,&b.h,&b.s,&b.v);
    b.h=(float)(b.h+(float)h*360.0/255.0);
    b.s=(float)(b.s*(float)s/255.0);
    a=hsv_to_rgb(b.h,b.s,b.v);
    layers[incmnd.c.layer][xc][yc]=a;
   }
}

void texture::emboss(texturecommand incmnd)
{
 int acc;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    {
     acc=-layers[incmnd.c.layer][byte(xc-1)][byte(yc-1)].cols[zc]
          -layers[incmnd.c.layer][byte(xc-1)][byte(yc)].cols[zc]
          -layers[incmnd.c.layer][byte(xc-1)][byte(yc+1)].cols[zc]
          +layers[incmnd.c.layer][byte(xc+1)][byte(yc-1)].cols[zc]
          +layers[incmnd.c.layer][byte(xc+1)][byte(yc)].cols[zc]
          +layers[incmnd.c.layer][byte(xc+1)][byte(yc+1)].cols[zc]
          +128;
     if (acc<0)  acc=0;
     if (acc>255)  acc=255;
     buffer[xc][yc].cols[zc]=acc;
    }
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   layers[incmnd.c.layer][yc][xc] = buffer[yc][xc];
}

void texture::blur(texturecommand incmnd)
{
 byte b;
 for (b=1; b<=incmnd.command[2];b++)
  for (xc=0; xc<256; xc++)
   for (yc=0; yc<256; yc++)
    for (zc=0; zc<3; zc++)
     layers[incmnd.c.layer][xc][yc].cols[zc]=(byte)((float)(layers[incmnd.c.layer][xc][yc].cols[zc]+
                                                                layers[incmnd.c.layer][(byte)(xc+1)][yc].cols[zc]+
                                                                layers[incmnd.c.layer][xc][(byte)(yc+1)].cols[zc]+
                                                                layers[incmnd.c.layer][(byte)(xc-1)][yc].cols[zc]+
                                                                layers[incmnd.c.layer][xc][(byte)(yc-1)].cols[zc]+
                                                                layers[incmnd.c.layer][(byte)(xc+1)][(byte)(yc+1)].cols[zc]+
                                                                layers[incmnd.c.layer][(byte)(xc-1)][(byte)(yc-1)].cols[zc]+
                                                                layers[incmnd.c.layer][(byte)(xc-1)][(byte)(yc+1)].cols[zc]+
                                                                layers[incmnd.c.layer][(byte)(xc+1)][(byte)(yc-1)].cols[zc]) / 9.0);
}

void texture::colorize(texturecommand incmnd)
{
 rgba a,b;
 byte c;
 a.a.r=incmnd.c.datalayer2;
 b.a.r=incmnd.c.param1;
 a.a.g=incmnd.c.param2;
 b.a.g=incmnd.c.param3;
 a.a.b=incmnd.c.param4;
 b.a.b=incmnd.c.param5;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
    c=layers[incmnd.c.layer][xc][yc].cols[incmnd.c.datalayer1];
    layers[incmnd.c.layer][xc][yc].cols[0]=(byte)(((float)b.a.r-(float)a.a.r)/255.0*(float)c+(float)a.a.r);
    layers[incmnd.c.layer][xc][yc].cols[1]=(byte)(((float)b.a.g-(float)a.a.g)/255.0*(float)c+(float)a.a.g);
    layers[incmnd.c.layer][xc][yc].cols[2]=(byte)(((float)b.a.b-(float)a.a.b)/255.0*(float)c+(float)a.a.b);
   }
}

void texture::bright(texturecommand incmnd)
{
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   {
    if (incmnd.c.datalayer1<=128)
    {
     for (zc=0; zc<3; zc++)
      layers[incmnd.c.layer][xc][yc].cols[zc]=(byte)((float)layers[incmnd.c.layer][xc][yc].cols[zc]/128.0*(float)incmnd.c.datalayer1);
    }
    else
    {
     for (zc=0; zc<3; zc++)
      layers[incmnd.c.layer][xc][yc].cols[zc]=255-(byte)((255.0-(float)layers[incmnd.c.layer][xc][yc].cols[zc])/128.0*(255.0-(float)incmnd.c.datalayer1));
    }
   }
}

void texture::contrast(texturecommand incmnd)
{
 int i;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    {
     i=(int)(   ((float)layers[incmnd.c.layer][xc][yc].cols[zc]-128.0 )*
		          (float)incmnd.c.datalayer1/32.0+128.0);
     if (i<0)  i=0; if (i>255)  i=255;
     layers[incmnd.c.layer][xc][yc].cols[zc]=i;
    }
}

void texture::mixmap(texturecommand incmnd)
{
 byte c;
 int i;
 for (xc=0; xc<256; xc++)
  for (yc=0; yc<256; yc++)
   for (zc=0; zc<3; zc++)
    {
     c=layers[incmnd.c.datalayer2][xc][yc].cols[incmnd.c.param1];
     i=(byte)((float)layers[incmnd.c.layer     ][xc][yc].cols[zc]/255.0*(float)c+
              (float)layers[incmnd.c.datalayer1][xc][yc].cols[zc]/255.0*(255.0-(float)c));
     layers[incmnd.c.layer][xc][yc].cols[zc]=i;
    }
}

void texture::text(texturecommand incmnd)

{
 int zx,zy,i;
 rgba ss;
 char *s = new(char[256]);
 memcpy(s,texts[incmnd.command[8]].text,256);
	
 i=FW_NORMAL;
 if (incmnd.command[7]) i=FW_BOLD;

 HDC hdc = GetDC(NULL);
 HDC mdc = CreateCompatibleDC(hdc);
 HBITMAP bm = CreateCompatibleBitmap(hdc,256,256);     
 BITMAPINFO bmi;    

 HFONT hf=CreateFont(incmnd.command[3], 0, 0, 0, i, incmnd.command[6], FALSE, FALSE,
 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
 ANTIALIASED_QUALITY, DEFAULT_PITCH,fonts[incmnd.command[2]]);

 RECT r;
 r.top=0;
 r.left=0;
 r.right=256;
 r.bottom=256;

 SelectObject(mdc, bm);
 FillRect(mdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

 SelectObject(mdc, hf);      
 SetBkMode(mdc, TRANSPARENT);
 SetTextColor(mdc, 0xFFFFFF);
 TextOut(mdc, incmnd.command[4], incmnd.command[5], s, strlen(s));

 bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
 bmi.bmiHeader.biWidth=256;
 bmi.bmiHeader.biHeight=256;
 bmi.bmiHeader.biPlanes=1;
 bmi.bmiHeader.biBitCount=32;
 bmi.bmiHeader.biCompression=BI_RGB;

 GetDIBits(mdc,bm,0,256,buffer,&bmi,DIB_RGB_COLORS);

 DeleteObject(hf);
 DeleteObject(bm);

  for (zx=0;zx<256;zx++)
    for (zy=0;zy<256;zy++)
     {
      ss=buffer[zx][zy];
      layers[incmnd.c.layer][zy][255-zx].a.r=(byte)(min(ss.a.r+layers[incmnd.c.layer][zy][255-zx].a.r,255.0));
      layers[incmnd.c.layer][zy][255-zx].a.g=(byte)(min(ss.a.g+layers[incmnd.c.layer][zy][255-zx].a.g,255.0));
      layers[incmnd.c.layer][zy][255-zx].a.b=(byte)(min(ss.a.b+layers[incmnd.c.layer][zy][255-zx].a.b,255.0));
     }
}


extern void inittextureengine()
{
 for (yc = 0; yc<256; yc++)
  for (xc = 0; xc<256; xc++)
   distbuf[yc][xc] = (float)(sqrt( (float)(sqr((float)(127-xc)) + sqr((float)(127-yc))) ) *2.0/256.0);

 fonts[0]="Arial";
 fonts[1]="Courier New";
 fonts[2]="Fixedsys";
 fonts[3]="Impact";
 fonts[4]="Symbol";
 fonts[5]="Tahoma";
 fonts[6]="Times New Roman";
 fonts[7]="Verdana";

}
