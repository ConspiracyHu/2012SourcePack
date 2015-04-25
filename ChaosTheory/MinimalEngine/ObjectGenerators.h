#ifndef __OBJECTGENERATORS__
#define __OBJECTGENERATORS__

#include "vectors.h"
#include "3dengine.h"
#include "includelist.h"

#ifdef INCLUDE_OBJ_BOX
#define aDDict_BOX 0
#endif
#ifdef INCLUDE_OBJ_SPHERE
#define aDDict_SPHERE 1
#endif
#ifdef INCLUDE_OBJ_CYLINDER
#define aDDict_CYLINDER 2
#endif
#ifdef INCLUDE_OBJ_CONE
#define aDDict_CONE 3
#endif
#ifdef INCLUDE_OBJ_PLANE
#define aDDict_PLANE 4
#endif
#ifdef INCLUDE_OBJ_ARC
#define aDDict_ARC 5
#endif
#ifdef INCLUDE_OBJ_LINE
#define aDDict_LINE 6
#endif
#ifdef INCLUDE_OBJ_LOFT
#define aDDict_LOFT 7
#endif
#ifdef INCLUDE_OBJ_ICOSAEDER
#define aDDict_ICOSAEDER 8
#endif
#ifdef INCLUDE_OBJ_DODECAEDER
#define aDDict_DODECAEDER 9
#endif
#ifdef INCLUDE_OBJ_CIRCLE
#define aDDict_CIRCLE 10
#endif
#ifdef INCLUDE_OBJ_CLONE
#define aDDict_CLONE 11
#endif
#ifdef INCLUDE_OBJ_STORED
#define aDDict_STORED 12
#endif
#ifdef INCLUDE_OBJ_FUR
#define aDDict_FUR 13
#endif

#ifdef INCLUDE_OBJ_SUPERSHAPE
#define aDDict_SUPERSHAPE 14
#endif

#ifdef INCLUDE_OBJ_MAPXFORM
#define aDDict_MAPXFORM 100
#endif

#ifdef INCLUDE_MESHSMOOTH
#ifdef INCLUDE_OBJ_LINEARSMOOTH
#define aDDict_LINEARSMOOTH 101
#endif
#ifdef INCLUDE_OBJ_BUTTERFLYSMOOTH
#define aDDict_BUTTERFLYSMOOTH 102
#endif
#endif
#ifdef INCLUDE_OBJ_MESHBLUR
#define aDDict_MESHBLUR 103
#endif

#ifdef INCLUDE_OBJ_MAPXFORM
void CalculateMapTransform(int MapXformType, int Color, MATRIX MXMatrix, OBJECT *, VECTOR3, VECTOR3, VECTOR3, float);
#endif
void GeneratePrimitive(OBJECT *Object);

#endif