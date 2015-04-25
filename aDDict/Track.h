#ifndef __track__
#define __track__

typedef struct {
  float data;
  float an,bn;
  unsigned int frame;
} KEY;

class CTrack {
public:
	CTrack();
	virtual ~CTrack();

	int numkey;
  KEY *keys;
  
  void  InitVectors();
  float GetKey(float frame);
  float GetVector(unsigned char v, unsigned int frame);
};

#endif