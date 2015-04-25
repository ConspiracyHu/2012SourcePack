#ifndef __cubes__
#define __cubes__

#include "Vectors.h"

struct TRIANGLE
{
   VECTOR3 p[3];
   VECTOR3 n[3];
};

struct GRIDCELL
{
   VECTOR3 p[8];
   VECTOR3 n[8];
   float val[8];
};

VECTOR3 VertexInterp(float isolevel,VECTOR3 p1,VECTOR3 p2,float valp1,float valp2);
int Polygonise(GRIDCELL grid,float isolevel,TRIANGLE *triangles);


#endif