void plane_imp_triangle_int_add_3d ( float x1, float y1, float z1, 
  float x2, float y2, float z2, float dist1, float dist2, 
  int *num_int, float x[], float y[], float z[] )
{
  float alpha;

  if ( dist1 == 0.0 ) {
    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;
  }
  else if ( dist2 == 0.0 ) {
    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;
  }
  else if ( dist1 * dist2 < 0.0 ) {
    alpha = dist2 / ( dist2 - dist1 );
    x[*num_int] = alpha * x1 + ( 1.0f - alpha ) * x2;
    y[*num_int] = alpha * y1 + ( 1.0f - alpha ) * y2;
    z[*num_int] = alpha * z1 + ( 1.0f - alpha ) * z2;
    *num_int = *num_int + 1;
  }

  return;
}

/**********************************************************************/

void plane_imp_triangle_int_3d ( float x1, float y1, float z1, float x2, 
  float y2, float z2, float x3, float y3, float z3, float a, float b, 
  float c, float d, int *num_int, float x[], float y[], float z[] )

/**********************************************************************/

/*
    Input, float X1, Y1, Z1, X2, Y2, Z2, X3, Y3, Z3, the coordinates of the vertices of the triangle.
    Input, float A, B, C, D, the parameters that define the implicit plane.

    Output, float DIST, the distance between the triangle and the plane.
    Output, int *NUM_INT, the number of intersection points return;ed.
    Output, float X[3], Y[3], Z[3], the coordinates of the intersection points.
*/
{
  float dist1;
  float dist2;
  float dist3;

  *num_int = 0;
/*
  Compute the signed distances between the vertices and the plane.
*/
  dist1 = a * x1 + b * y1 + c * z1 + d;
  dist2 = a * x2 + b * y2 + c * z2 + d;
  dist3 = a * x3 + b * y3 + c * z3 + d;
/*
  Consider any zero distances.
*/
  if ( dist1 == 0.0 ) {

    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;

  }

  if ( dist2 == 0.0 ) {

    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;

  }

  if ( dist3 == 0.0 ) {

    x[*num_int] = x3;
    y[*num_int] = y3;
    z[*num_int] = z3;
    *num_int = *num_int + 1;

  }
/*
  If 2 or 3 of the nodes intersect, we're already done.
*/
  if ( *num_int >= 2 ) {
    return;
  }
/*
  If one node intersects, then we're done unless the other two
  are of opposite signs.
*/
  if ( *num_int == 1 ) {

    if ( dist1 == 0.0 ) {

      plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
        dist2, dist3, num_int, x, y, z );

    }
    else if ( dist2 == 0.0 ) {

      plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
        dist1, dist3, num_int, x, y, z );

    }
    else if ( dist3 == 0.0 ) {

      plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
        dist1, dist2, num_int, x, y, z );

    }

    return;

  }
/*
  All nodal distances are nonzero, and there is at least one
  positive and one negative.
*/
  if ( dist1 * dist2 < 0.0 && dist1 * dist3 < 0.0 ) {

    plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
      dist1, dist2, num_int, x, y, z );

    plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
      dist1, dist3, num_int, x, y, z );

  }
  else if ( dist2 * dist1 < 0.0 && dist2 * dist3 < 0.0 ) {

    plane_imp_triangle_int_add_3d ( x2, y2, z2, x1, y1, z1, 
      dist2, dist1, num_int, x, y, z );

    plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
      dist2, dist3, num_int, x, y, z );

  }
  else if ( dist3 * dist1 < 0.0 && dist3 * dist2 < 0.0 ) {

    plane_imp_triangle_int_add_3d ( x3, y3, z3, x1, y1, z1, 
      dist3, dist1, num_int, x, y, z );

    plane_imp_triangle_int_add_3d ( x3, y3, z3, x2, y2, z2, 
      dist3, dist2, num_int, x, y, z );

  }

  return;
}

void cutplanepoly(vector3 normal, vector3 planevert, cpolygon p, cpolygon *&frontlist, cpolygon *&backlist)
{
	float planedist=-normal.b.x*planevert.b.x-normal.b.y*planevert.b.y-normal.b.z*planevert.b.z;
	float x[3],y[3],z[3];
	int num_int;
	plane_imp_triangle_int_3d ( p.a.b.x, p.a.b.y, p.a.b.z, 
		                        p.b.b.x, p.b.b.y, p.b.b.z, 
								p.c.b.x, p.c.b.y, p.c.b.z,  
								normal.b.x, normal.b.y, normal.b.z, planedist,
								&num_int, x, y, z);
	
	char *s2s = new char[256];
	sprintf(s2s,"%d",num_int);
	MessageBox( 0, s2s, "HelloWorld", MB_OK );
}

float vectorvsplanecheck(vector3 normal, vector3 planevert, vector3 dot)
{
	vector3 v=vect_kivonas(dot,planevert);
	return normal.b.x*v.b.x+normal.b.y*v.b.y+normal.b.z*v.b.z;
}

int checkplanetri(vector3 normal, vector3 planevert, vector3 a, vector3 b, vector3 c)
{
	float planedist=-normal.b.x*planevert.b.x-normal.b.y*planevert.b.y-normal.b.z*planevert.b.z;
	float x[3],y[3],z[3];
	int num_int;
	plane_imp_triangle_int_3d ( a.b.x, a.b.y, a.b.z, 
		                        b.b.x, b.b.y, b.b.z, 
								c.b.x, c.b.y, c.b.z,  
								normal.b.x, normal.b.y, normal.b.z, planedist,
								&num_int, x, y, z);
	if (num_int==3) return tri_onplane;
	if (num_int==2) 
	{
		if (!((x[0]==a.b.x && y[0]==a.b.y && z[0]==a.b.z)  || 
		      (x[0]==b.b.x && y[0]==b.b.y && z[0]==b.b.z)  ||
		      (x[0]==c.b.x && y[0]==c.b.y && z[0]==c.b.z)) &&
		     ((x[1]==a.b.x && y[1]==a.b.y && z[1]==a.b.z)  || 
		      (x[1]==b.b.x && y[1]==b.b.y && z[1]==b.b.z)  ||
		      (x[1]==c.b.x && y[1]==c.b.y && z[1]==c.b.z)))
			return tri_split;
	}
	float ta,tb,tc;
	ta=normal.b.x*a.b.x+normal.b.y*a.b.y+normal.b.z*a.b.z+planedist;
	tb=normal.b.x*c.b.x+normal.b.y*b.b.y+normal.b.z*b.b.z+planedist;
	tc=normal.b.x*c.b.x+normal.b.y*c.b.y+normal.b.z*c.b.z+planedist;
	if (ta<0 || tb<0 || tc<0) return tri_back;
	return tri_front;
}
