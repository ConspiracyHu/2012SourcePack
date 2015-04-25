//===============================================================================================
// SIMPLE.EXE
// Copyright (c), Firelight Multimedia, 2000.
//
// This is a simple but descriptive way to get FMOD going, by loading a song and a few wav files
// and then playing them back.  It also shows how to do device enumeration and how to tweak
// various playback settings.
//
//===============================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

//#define USEMEMLOADRESOURCE

#define USEFMOD TRUE

#ifdef USEFMOD
	#include "fmod/minifmod.h"
#endif


// this is if you want to replace the samples with your own (in case you have compressed them)
void sampleloadcallback(void *buff, int lenbytes, int numbits, int instno, int sampno)
{
	printf("pointer = %p length = %d bits = %d instrument %d sample %d\n", buff, lenbytes, numbits, instno, sampno);
}


typedef struct 
{
	int length;
	int pos;
	void *data;
} MEMFILE;

unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = (MEMFILE *)calloc(sizeof(MEMFILE),1);

#ifndef USEMEMLOADRESOURCE
	{	// load an external file and read it
		FILE *fp;
		fp = fopen(name, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			memfile->length = ftell(fp);
			memfile->data = calloc(memfile->length,1);
			memfile->pos = 0;

			fseek(fp, 0, SEEK_SET);
			fread(memfile->data, 1, memfile->length, fp);
			fclose(fp);
		}
	}
#else
	{	// hey look some load from resource code!
		HRSRC		rec;
		HGLOBAL		handle;

		rec = FindResource(NULL, name, RT_RCDATA);
		handle = LoadResource(NULL, rec);
		
		memfile->data = LockResource(handle);
		memfile->length = SizeofResource(NULL, rec);
		memfile->pos = 0;
	}
#endif

	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

#ifndef USEMEMLOADRESOURCE
	free(memfile->data);			// dont free it if it was initialized with LockResource
#endif

	free(memfile);
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	return memfile->pos;
}

FMUSIC_MODULE *mod;

void stop() {
	if (!FMUSIC_PlayingSong) return;
	FMUSIC_StopSong(mod);
	FSOUND_Close();
}

void play(int ord) {
	if (FMUSIC_PlayingSong) stop();
	FSOUND_Init(44100, 0);
	FMUSIC_PlaySong(mod,ord);
}



void main(int argc, char *argv[])
{
#ifdef USEFMOD

	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
#endif

	if (argc < 2)
	{
		printf("-------------------------------------------------------------\n");
		printf("MINIFMOD example XM player.\n");
		printf("Copyright (c) Firelight Multimedia, 2000.\n");
		printf("-------------------------------------------------------------\n");
		printf("Syntax: simplest infile.xm\n\n");
		return;
	}


#ifdef USEFMOD
	// ==========================================================================================
	// INITIALIZE
	// ==========================================================================================
	if (!FSOUND_Init(44100, 0))
	{
		printf("Error upon initialization\n");
		return;
	}

	// ==========================================================================================
	// LOAD SONG
	// ==========================================================================================
	mod = FMUSIC_LoadSong(argv[1], NULL); //sampleloadcallback);
	if (!mod)
	{
		printf("Error loading song\n");
		return;
	}

	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================
	FMUSIC_PlaySong(mod,0);
#endif

	printf("Keyz0rz:\nX = Quit\nQ = prev pattern\nW = next pattern\nP = Pause\nO = Resume\n");
	printf("E = Stop\nR = Play\n");
	printf("=========================================================================\n");
	printf("Playing song...\n");
	FSOUND_SetPause(0);

//	int leng=FMUSIC_GetLength(mod);
	{
		char key = 0;
		int done=0;
		do
		{
			int		ord = 0, row = 0;
			unsigned long mytime = 0;

#ifdef USEFMOD
			ord = FMUSIC_GetOrder(mod);
			row = FMUSIC_GetRow(mod);
			mytime = FMUSIC_GetTime(mod);
#endif
		printf("ord %3d row %2d - %8d ms - %s      \r",
			ord, row, mytime, (row % 8 ? "    " : "TICK"));

		if (kbhit()) {
			char c = getch();
			if (c=='q') play(ord<0 ? 0 : ord-1);
			if (c=='w') play(ord+1);
			if (c=='p') FSOUND_SetPause(1);
			if (c=='o') FSOUND_SetPause(0);
			if (c=='e') stop();
			if (c=='r') play(0);
			if (c=='x') done=1;
		}
//		if (starts[ord]>mytime) starts[ord]=mytime;

		} while (!done);
	}

#ifdef USEFMOD
	FMUSIC_FreeSong(mod);
	FSOUND_Close();
#endif
}
