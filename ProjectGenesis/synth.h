// Code done by Gargaj

#pragma warning(disable : 4244)

class CEnvelope {
public:
	CEnvelope();
	virtual ~CEnvelope();

  float attack;
  float sustain;
  float release;
  float amplitude;
  float shape;
  float GetEnv(int time);
};

#pragma pack(1)

typedef struct {
  float frequency;	char frequency_env;
  float volume;			char volume_env;
	float pw;					char pw_env;
  char waveform;
} OSCILLATOR;

class CSample {
public:
	CSample(char * raw);
	virtual ~CSample();

  CEnvelope  * env; int numenv;
  OSCILLATOR * osc; int numosc;

  char filter;
  char rezo;
  float cutoff;    char cutoff_env;
  float resonance; char resonance_env;
  float resoamp;   char resoamp_env;

	float clip_level;
	float gain;

	int length;

#if 0
  int filter;
  int rezo;
  float cutoff;    int cutoff_env;
  float resonance; int resonance_env;
  float resoamp;   int resoamp_env;

	float clip_level;
	float gain;
#endif
	
	char sequence[16]; CEnvelope * seqenv;

	void zVCF( float *out, float *in, int num,
                    float &h0, float &h1, float rate, float bandwidth, int type );
    
	void GetFullSample(float * buff, int len);
	void GetFullSampleToXM(char * buff, int len);
};

#pragma pack()
