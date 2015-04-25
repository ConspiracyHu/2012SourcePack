#ifndef __introplayer__
#define __introplayer__

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "introengine.h"
#include "introwindow.h"

// EFFECTS

void InitEffect(float u1, float v1, float u2, float v2);
void displayframe(int icurrentframe);
float linear(float a, float b, float t);

#endif