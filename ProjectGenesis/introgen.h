#ifndef __introgen__
#define __introgen__

#include "vectors.h"
#include "introengine.h"

#define aDDict_box                 1
#define aDDict_sphere              3
#define aDDict_hasab               6
#define aDDict_cone                7
#define aDDict_arc                 9
#define aDDict_loft                10
#define aDDict_line                11
#define aDDict_grid                12
#define aDDict_clone               15

#define aDDict_map                 104

void obj_generatenormals(object *o);
void obj_counttexturecoordinates(object *o, int xr, int yr, int xoff, int yoff, bool swap, bool xswp, bool yswp);
float getmappixel(vector2 t, int channel, bool nowrap);

void obj_createprimitive(object *o, char primitive, int param1, int param2, int param3, int param4);

#endif