#ifndef __precalc__
#define __precalc__

#include "introinput.h"
#include "introengine.h"

extern scene *CNSprecalc;

void WINAPI PrecalcAnim(float endvalue);
void PrecalcRotate(float f);
void WINAPI animdummy(float f);
void InitPrecalc();

#endif