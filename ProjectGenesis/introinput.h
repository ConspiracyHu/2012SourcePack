#ifndef __introinput__
#define __introinput__

#include <memory.h>
#include "intro.h"
#include "introengine.h"
#include "introgen.h"

struct camfield
{
	unsigned fovwritten : 1,
             rollwritten: 1,
             eyexwritten : 1,
             eyeywritten : 1,
             eyezwritten : 1,
             targetxwritten: 1,
             targetywritten: 1,
             targetzwritten: 1;
};

struct objfield
{
	unsigned posx : 1,
             posy : 1,
             posz : 1,
             rotx : 1,
             roty : 1,
             rotz : 1,
             rota : 1,
             strx : 1,
             stry : 1,
             strz : 1,
             colr : 1,
             colg : 1,
             colb : 1,
             cola : 1;
};

void loadprojectfile(unsigned char *file);

#endif