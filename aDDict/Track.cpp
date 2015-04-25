#include "Track.h"

#pragma warning(disable : 4244)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
#define TENS 1.0
#define BIAS 1.0
#define CONT 1.0
*/
#define TENS 0.0
#define BIAS 0.0
#define CONT 0.0
#define LOOP 1
#define MAXKEY 9999

CTrack::CTrack()
{
}

CTrack::~CTrack()
{

}

float hermite(float p1,float p2,float r1,float r2,float t) {
  return p1*( 2.0*(t*t*t)-3.0*(t*t)+1.0)+
         r1*(     (t*t*t)-2.0*(t*t)+t  )+
         p2*(-2.0*(t*t*t)+3.0*(t*t)    )+
         r2*(     (t*t*t)-    (t*t)    );
}

void  CTrack::InitVectors() {
 int i;
 if (numkey==1) return;
 for (i=0; i<numkey; i++) {
  keys[i].an=GetVector(0,i);
  keys[i].bn=GetVector(1,i);
 }

}

float CTrack::GetVector(unsigned char sel, unsigned n) {

  KEY   kn_1, kn, kn1;
  float pn_1, pn, pn1;

  kn = keys[n];
  pn = kn.data;

  if (sel == 2) return pn;

  if (n == 0) {
    //first key
    kn1 = keys[1];
    pn1 = kn1.data;

    if (numkey == 2) {
      //2 keys
      return (pn1 - pn)*(1.0 - TENS);
    };
    if (!LOOP) {
      //first key, no loop
      return ((pn1 - pn)*1.5 - GetVector(0,1)*0.5)*(1.0 - TENS);
    } else {
      //first key, loop
      kn_1 = keys[numkey-2];
    };
  } else if (n == numkey-1) {
    //last key
    kn_1 = keys[n-1];
    pn_1 = kn_1.data;

    if (numkey == 2) {
      //2 keys
      return (pn - pn_1)*(1.0 - TENS);
    };
    if (!LOOP) {
      //last key, no loop
      return ((pn - pn_1)*1.5 - GetVector(1,n-1)*0.5)*(1.0 - TENS);
    } else {
      //last key, loop
      kn1 = keys[1];
    };
  } else {
    //middle keys
    kn_1= keys[n-1];
    kn1 = keys[n+1];
  };
  pn_1= kn_1.data;
  pn1 = kn1 .data;

  float f = (sel == 0) ? 0.5 : -0.5;

  float g1 = (pn  - pn_1)*(1.0 + BIAS);
  float g2 = (pn1 - pn  )*(1.0 - BIAS);

  return (g1 + (g2-g1)*(0.5 + f*CONT))*(1.0 - TENS);

}


float CTrack::GetKey(float frame) {
	int x,y=MAXKEY;

	if (!numkey) return 0.0;

  if ((numkey==1) || ((keys[0].frame>=frame) && (keys[0].frame>=0)))
		return keys[0].data;
  
	for (x=0; x<numkey; x++)
		if ((frame<=keys[x].frame) && (y==MAXKEY)) y=x-1;

	if (y==MAXKEY) return keys[numkey-1].data;

	return hermite(keys[y].data,keys[y+1].data,
		             keys[y].an  ,keys[y+1].bn,
								 (float)(frame-keys[y].frame)/(float)(keys[y+1].frame-keys[y].frame) );
}

