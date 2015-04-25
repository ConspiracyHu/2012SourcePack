#include <math.h>
#include <windows.h>
#include "synth.h"

#define READ(t) *((t *)raw); raw+=sizeof(t)

#define C0 65.406396f

// ====================
// ===== ENVELOPERS

CEnvelope::CEnvelope() {}

CEnvelope::~CEnvelope(){}

#if 0
void CEnvelope::LoadRaw(char * raw) {
  attack   =READ(float);
  sustain  =READ(float);
  release  =READ(float);
  amplitude=READ(float);
//  shape    =READ(float);
}
#endif

float CEnvelope::GetEnv(int time) {
	float r=attack+sustain+release;
	if      (time <   attack) return (time/attack)*amplitude;
	else if (time <= (attack+sustain)) return amplitude;
	else if (time <= (attack+sustain+release)) return ((release-(time-attack-sustain))/release)*amplitude;
	return 0.0f;
}



// =============================
// ===== SAMPLEGENERATION

#ifdef SYNTH_EDITOR
CSample::CSample() {

  int i;
	numenv=10;
	env = new CEnvelope[10];
  for (i=0; i<numenv; i++) {
		//env=new CEnvelope();
		env[i].attack=0;
		env[i].sustain=0;
		env[i].release=75;
		env[i].amplitude=1;
	}

	numosc=10;
	osc = new OSCILLATOR[10];
  for (i=0; i<numosc; i++) {
		osc[i].frequency=440;
		osc[i].volume=50;
		osc[i].frequency_env=-1;
		osc[i].volume_env=-1;
		osc[i].waveform=0;
		osc[i].pw=50;
		osc[i].pw_env=-1;
	}

  for (i=0; i<16; i++) sequence[i]=0xFF;
	seqenv=new CEnvelope;
	seqenv->attack=0;
	seqenv->sustain=0;
	seqenv->release=100;
	seqenv->amplitude=1;

	cutoff=0;
	cutoff_env=-1;
	resonance=0;
	resonance_env=-1;
	resoamp=0;
	resoamp_env=-1;
  filter=0;
  rezo=0;

	clip_level=1.0;
	gain=1.0;

	length=65536;
}
#endif
CSample::~CSample() {}

CSample::CSample(char * raw) {

  int i;

  numenv=READ(BYTE);
  env = new CEnvelope[numenv];
  for (i=0; i<numenv; i++) {
		memcpy(&(env[i].attack),raw,4*4);
    raw+=sizeof(float)*4;
  }
  
  numosc=READ(BYTE);
	int needseq=0;
  osc = new OSCILLATOR[numosc];
  for (i=0; i<numosc; i++) {
		memcpy(osc+i,raw,sizeof(OSCILLATOR));
		raw+=sizeof(OSCILLATOR);
    if (osc[i].frequency_env==10) needseq=1;
	}

	if (needseq) {
		memcpy(sequence,raw,16);
		raw+=16;
		seqenv = new CEnvelope;
		memcpy(&(seqenv->attack),raw,4*4);
    raw+=sizeof(float)*4;
	}
  
	memcpy(&filter,raw,2+5*3+4+4+4);
}

#define PI 3.141592653f

float a0,a1,b1;
float i0,i1,o0,o1;

void setfilter(char f, float fCut) {
	float w = 2.0 * 44100; 
  fCut *= 2.0F * PI; 
	float Norm = 1.0 / (fCut + w); 

	if (f==1) {
    a0 = a1 = fCut * Norm; 
	}
	if (f==2) {
    a0 = w * Norm; 
    a1 = -a0; 
	}
  b1 = (w - fCut) * Norm; 
}

float vibrapos = 0; 
float vibraspeed = 0; 

float rezonate(float insmp, float resofreq,float amp) {

	float w = 2.0*PI*resofreq/44100.0; // Pole angle 
	float q = 1.0-w/(2.0*(amp+0.5/(1.0+w))+w-2.0); // Pole magnitude 
	float r = q*q; 
	float c = r+1.0-2.0*cos(w)*q; 

  vibraspeed += (insmp - vibrapos) * c; 
  vibrapos += vibraspeed; 
  vibraspeed *= r; 

	return vibrapos;
}

void CSample::GetFullSample(float * buff, int len) {
//	if (!buff) return;
//  len = len > length ? length : len;

	srand(31337);

	double phase[10];							

  for (int i=0; i<len; i++) {

		float envpos=(i/(float)length)*100;
		float df=0.0f;
		for (int j=0; j<numosc; j++) {
			if (!i) phase[j]=0.0;
			float d;
			float f1=osc[j].frequency;
			float fm;
			float v1=osc[j].volume/100.0f;
			float w1;
			if (osc[j].volume_env > -1) v1*=env[osc[j].volume_env].GetEnv(envpos);

			if (osc[j].frequency_env==10) {
				int note=sequence[(int)((i/(float)length)*16)];
				float t=fmod(((i/(float)length)*16),1.0f);
				fm=0;
				f1= note==0xFF ? 0.0 : C0*pow(2.0, note/12.0);
				v1= note==0xFF ? 0.0 : seqenv->GetEnv(t*100);
			}
			else if (osc[j].frequency_env!=-1)
				fm = env[osc[j].frequency_env].GetEnv(envpos);
			else fm=0.0;


			switch (osc[j].waveform) {
				case 0: d=0.0f; break;
				case 1: d=(v1*    sin(phase[j])); break;
				case 2: d=(v1*  (fmod(phase[j]/(PI*2.0),1.0)-0.5)); break;
				case 3:
					w1=osc[j].pw;
					if (osc[j].pw_env > -1) w1*=env[osc[j].pw_env].GetEnv(envpos);
					d=(v1*  (fmod(phase[j]/(PI*2.0),1.0)*100.0 < w1 ? 1.0 : 0.0)  );
					break;
				case 4: d= (rand()/32767.0-0.5)*v1; break;
			}
			df+=d;

			phase[j] += (f1+fm*880.0*2.0)*PI/44100.0f;
		}
		float f=df;

    if (filter) {
			float co=cutoff;
			if (cutoff_env > -1) co*=env[cutoff_env].GetEnv(envpos);
			setfilter(filter,co);
      i1 = f;
      o1 = i0*a0 + i1*a1 + o0*b1;
      i0 = i1;
      o0 = o1;
      f = o1;
    }

    if (rezo) {
			float rz=resonance;
			if (resonance_env > -1) rz*=env[resonance_env].GetEnv(envpos);
			float ra=resoamp;
			if (resoamp_env > -1)   ra*=env[resoamp_env].GetEnv(envpos);
			f=rezonate(f,rz,ra);
		}

    f*=gain;

    buff[i]=(f<-clip_level) ? -clip_level : (
			      (f> clip_level) ?  clip_level : f);
  }
}

void CSample::GetFullSampleToXM(char * buff, int len) {
//	if (!buff) return;
  len = len > length ? length : len;
  float * fp=new float[length];
	int i;
	GetFullSample(fp,len);
	for (i=0; i<len; i++) buff[i]=((int)(fp[i]*127)+127)+0x80;
}


