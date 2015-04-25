#ifndef __OBJECTGENERATORS__
#define __OBJECTGENERATORS__

#include "memtrack.h"

#include "vectors.h"
#include "3dengine.h"

#define aDDict_BOX 0
#define aDDict_SPHERE 1
#define aDDict_CYLINDER 2
#define aDDict_CONE 3
#define aDDict_PLANE 4
#define aDDict_ARC 5
#define aDDict_LINE 6
#define aDDict_LOFT 7
#define aDDict_ICOSAEDER 8
#define aDDict_DODECAEDER 9
#define aDDict_CIRCLE 10
#define aDDict_CLONE 11
#define aDDict_STORED 12
#define aDDict_FUR 13
#define aDDict_SUPERSHAPE 14

#define aDDict_MAPXFORM 100
#define aDDict_LINEARSMOOTH 101
#define aDDict_BUTTERFLYSMOOTH 102
#define aDDict_MESHBLUR 103

void GeneratePrimitive(OBJECT *Object,int PrimitiveID,int Param1, int Param2, int Param3, int Param4, int Param5);
float SuperShapeRad(float m,float n1,float n2,float n3,float a, float b, float phi);

#endif