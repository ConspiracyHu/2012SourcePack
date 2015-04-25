/******************************************************************************/
/* MUSIC.H                                                                    */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Multimedia will not support      */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.           */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Multimedia is a registered business name.                        */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#ifndef _MUSIC_H
#define _MUSIC_H

#include "sound.h"

#define FMUSIC_FLAGS_NORMALVOLUMESLIDES	0
#define FMUSIC_FLAGS_FASTVOLUMESLIDES		1
//#define FMUSIC_FLAGS_LINEARFREQUENCY		2
//#define FMUSIC_FLAGS_AMIGAFREQUENCY		4

#define FMUSIC_MAXORDERS					256

#define FMUSIC_KEYCUT						254
#define FMUSIC_KEYOFF						255

#define FMUSIC_FREQ							1
#define FMUSIC_VOLUME						2
#define FMUSIC_PAN							4
#define FMUSIC_TRIGGER						8
#define FMUSIC_SURROUND						16
#define FMUSIC_STOP							32
#define FMUSIC_SURROUNDOFF					64

#define FMUSIC_ENVELOPE_OFF					0
#define FMUSIC_ENVELOPE_ON					1
#define FMUSIC_ENVELOPE_SUSTAIN				2
#define FMUSIC_ENVELOPE_LOOP				4
#define FMUSIC_ENVELOPE_CARRY				8

#define FMUSIC_CUT							0
#define FMUSIC_CONTINUE						1
#define FMUSIC_OFF							2
#define FMUSIC_FADEOUT						3

#define FMUSIC_PLAYPACKED					TRUE

#if !defined (FMUSIC_PLAYPACKED) && defined (FINAL)
	#error Error - packed music playing is not turned on for FINAL
#endif

// Single note type - contains info on 1 note in a pattern
typedef struct 
{        
	unsigned char	note;			// note to play at     (0-133) 132=none,133=keyoff
	unsigned char	number;			// sample being played (0-99)
	unsigned char	volume;			// volume column value (0-64)  255=no volume
	unsigned char	effect;			// effect number       (0-1Ah)
	unsigned char	eparam;			// effect parameter    (0-255)
} FMUSIC_NOTE;

// pattern data type 
typedef struct
{
	int		rows;
	FMUSIC_NOTE	*data;
} FMUSIC_PATTERN;

#pragma pack(1)

typedef struct
{
	unsigned char order;
	unsigned char row;
	unsigned int  ms;
} FMUSIC_TIMMEINFO;

typedef struct
{
	signed char value;
	unsigned short tick;
} FMUSIC_ENVNODE;


#pragma pack()

// Multi sample extended instrument
typedef struct FMUSIC_INSTRUMENT
{
	int			numsamples;			// number of samples in this instrument 
	FSOUND_SAMPLE	*sample[16];		// 16 samples per instrument 
	unsigned char	keymap[96];			// sample keymap assignments 
										
	unsigned short	VOLPoints[40];		// Volume envelope points (80 bytes - enough for 25 3 byte IT envelopes)
	unsigned short	PANPoints[40];		// Panning envelope points 

	// ====================================================================================
	// 16 bytes read here in the loader for size, dont alter order or insert!!!
	unsigned char	VOLnumpoints;		// Number of volume envelope points 
	unsigned char	PANnumpoints;		// Number of panning envelope points 
	unsigned char	VOLsustain;			// Volume sustain point 
	unsigned char	VOLLoopStart;		// Volume envelope loop start 
	unsigned char	VOLLoopEnd;			// Volume envelope loop end 
	unsigned char	PANsustain;			// Panning sustain point 
	unsigned char	PANLoopStart;		// Panning envelope loop start 
	unsigned char	PANLoopEnd;			// Panning envelope loop end 
	unsigned char	VOLtype;			// Type of envelope,bit 0:On 1:Sustain 2:Loop 
	unsigned char	PANtype;			// Type of envelope,bit 0:On 1:Sustain 2:Loop 
	unsigned char	VIBtype;			// Instrument Vibrato type 
	unsigned char	VIBsweep;			// Time it takes for vibrato to fully kick in 
	unsigned char	VIBdepth;			// depth of vibrato 
	unsigned char	VIBrate;			// rate of vibrato 
	unsigned short	VOLfade;			// fade out value 
	// ====================================================================================


} FMUSIC_INSTRUMENT;



// Channel type - contains information on a mod channel
typedef struct FMUSIC_CHANNEL
{
									
	unsigned char  	note;  				// last note set in channel 
	unsigned char	samp;				// last sample set in channel 
										
	unsigned char	notectrl;			// flags for FSOUND 
					
	FSOUND_CHANNEL	*cptr;				// pointer to FSOUND system mixing channel 
	FSOUND_SAMPLE	*sptr;				// pointer to FSOUND system sample 

	int			freq;				// current mod frequency period for this channel 
	int			volume;				// current mod volume for this channel 
	int			pan;				// current mod pan for this channel 
	int			voldelta;			// delta for volume commands.. tremolo/tremor etc 
	int			freqdelta;			// delta for frequency commands.. vibrato/arpeggio etc 
	int			pandelta;			// delta for pan commands.. panbrello
										
	int			samp_globalvol;		// this current sample's global volume 
										
	int			envvoltick;			// tick counter for envelope position 
	int			envvolpos;			// envelope position 
	int			envvolfrac;			// fractional interpolated envelope volume 
	int			envvol;				// final interpolated envelope volume 
	int			envvoldelta;		// delta step between points 
	signed char	envvolstopped;		// flag to say whether envelope has finished or not 
										
	int			envpantick;			// tick counter for envelope position 
	int			envpanpos;			// envelope position 
	int			envpanfrac;			// fractional interpolated envelope pan 
	int			envpan;				// final interpolated envelope pan 
	int			envpandelta;		// delta step between points 
	signed char	envpanstopped;		// flag to say whether envelope has finished or not 
										
	int			fadeoutvol;			// volume fade out 
	int			ivibpos;   			// instrument vibrato position
	int			ivibsweeppos;		// instrument vibrato sweep position
	signed char	fade;				// flag whether to start fade or not 
	signed char	keyoff;				// flag whether keyoff has been hit or not) 
									
	unsigned char	inst;				// last instrument set in channel 
	unsigned char  	realnote;  			// last realnote set in channel 
	unsigned int	period;				// last period set in channel 
	unsigned char	recenteffect;		// previous row's effect.. used to correct tremolo volume 

	unsigned int	sampleoffset;		// sample offset for this channel in SAMPLES

	int			globalvolume;		// global volume for this channel 
//	unsigned char	portaupdown;  		// last porta up or down value (S3M / IT)
	unsigned char	portadown;  		// last porta down value (XM) 
	unsigned char	portaup;   			// last porta up value (XM) 
	unsigned char	xtraportadown;		// last porta down value (XM) 
	unsigned char	xtraportaup;  		// last porta up value (XM) 
	unsigned char	volslide;   		// last volume slide value (XM + S3M)
	unsigned char	panslide;			// pan slide parameter (XM) 
	unsigned char	retrigx;   			// last retrig volume slide used (XM + S3M)
	unsigned char	retrigy;   			// last retrig tick count used (XM + S3M)
//	unsigned char	retrigcount;		// retrig timer (IT)

	int			portatarget; 		// note to porta to 
	unsigned char	portaspeed;			// porta speed
//	unsigned char	portareached;		// flag for IT to say portamento has been reached

	signed char		vibpos;   			// vibrato position
	unsigned char	vibspeed;  			// vibrato speed
	unsigned char	vibdepth;  			// vibrato depth
	unsigned char	vibtype;			// vibrato type (IT) .. I cant believe this shit
	
	signed char		tremolopos;   		// tremolo position
	unsigned char  	tremolospeed; 		// tremolo speed
	unsigned char  	tremolodepth; 		// tremolo depth
	
	int			panbrellopos;  		// panbrello position
	unsigned char  	panbrellospeed;		// panbrello speed
	unsigned char  	panbrellodepth;		// panbrello depth
	
	unsigned char	tremorpos; 			// tremor position (XM + S3M)
	unsigned char 	tremoron;   		// remembered parameters for tremor (XM + S3M) 
	unsigned char 	tremoroff;   		// remembered parameters for tremor (XM + S3M) 
	unsigned char	arpeggio;			// remembered parameters for arpeggio (S3M) 
 	int			patlooprow;			
 	int 			patloopno;  		// pattern loop variables for effect  E6x
	unsigned char  	channelvsl;			// global volume slide parameters

	unsigned char	specialparam;		// remembered parameter for Sxy
	unsigned char 	wavecontrol;		// waveform type for vibrato and tremolo (4bits each)
	unsigned char 	wavecontrolvib;		// waveform type for vibrato (IT) 
	unsigned char 	wavecontroltrem;	// waveform type for tremolo (IT)
	unsigned char 	wavecontrolpan;		// waveform type for panbrello (IT)

	unsigned char	finevsldown;		// parameter for fine volume slide down 
	unsigned char	finevslup;			// parameter for fine volume slide down 
	unsigned char	fineportaup;		// parameter for fine porta slide up
	unsigned char	fineportadown;		// parameter for fine porta slide down 
	unsigned char	highoffset;			// high part of sample offset - ie the 'y' part of yxx00 (IT) 
	unsigned char	volcolumn_volslide; // volume column parameter remembered for volume sliding (IT) 
} FMUSIC_CHANNEL;


// Song type - contains info on song
typedef struct FMUSIC_MODULE 
{							
	FMUSIC_PATTERN	*pattern;			// patterns array for this song
	FMUSIC_INSTRUMENT *instrument;		// instrument array for this song (not used in MOD/S3M)
	int				mixer_samplesleft;
	int				mixer_samplespertick;

	// ====================================================================================
	// 6 bytes read here in the loader for size, dont alter order or insert!!!
	short			numorders;			// number of orders (song length)
	short			restart;			// restart position
	short			numchannels;		// number of channels
	// ====================================================================================

	int				numpatterns;		// number of physical patterns
	int				numpatternsmem;		// number of allocated patterns

	// ====================================================================================
	// 256+8 6 bytes read here in the loader for size, dont alter order or insert!!!
	short			numinsts;			// number of instruments
	unsigned short	flags;				// flags such as linear frequency, format specific quirks etc
	short			defaultspeed;
	short			defaultbpm;
	unsigned char	orderlist[FMUSIC_MAXORDERS];	// pattern playing order list
	// ====================================================================================

	unsigned char	defaultglobalvolume;

	int				globalvolume;		// global mod volume
	unsigned char	globalvsl;			// global mod volume
	int				tick;				// current mod tick	
	int				speed;				// speed of song in ticks per row
	int				bpm;				// speed of song in bpm
	int				row;				// current row in pattern
	int				order;				// current song order position
	int				patterndelay;		// pattern delay counter
	int				nextrow;			// current row in pattern
	int				nextorder;			// current song order position
	int				time_ms;			// time passed in seconds since song started

	void			(*Play)(struct FMUSIC_MODULE *mod);		// callback when the song starts
	void			(*Update)(struct FMUSIC_MODULE *mod);	// playback routine callback... for different formats
	SAMPLELOADCALLBACK	samplecallback;

} FMUSIC_MODULE;


//= VARIABLE EXTERNS ========================================================================
#ifdef __cplusplus
extern "C" 
{
#endif

extern FMUSIC_MODULE *			FMUSIC_PlayingSong;
extern FSOUND_SAMPLE			FMUSIC_DummySample;
extern FSOUND_CHANNEL			FMUSIC_DummyChannel;
extern FMUSIC_INSTRUMENT		FMUSIC_DummyInstrument;
extern FMUSIC_CHANNEL			FMUSIC_Channel[];		// channel array for this song
extern FMUSIC_TIMMEINFO *		FMUSIC_TimeInfo;

#ifdef __cplusplus
}
#endif


//= FUNCTION DECLARATIONS ====================================================================

#define FMUSIC_PERIOD2HZ(_per) (14317056L / (_per))

// private (internal functions)
void	FMUSIC_SetBPM(FMUSIC_MODULE *mod, int bpm);
int		FMUSIC_FineTune2Hz(unsigned char ft);

#endif