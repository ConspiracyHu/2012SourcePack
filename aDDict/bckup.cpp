struct pathedge{
	vector3 a,b;
};

struct pathedgelist{ 
	int pathedgeplace;
	int pathedgenum;
	pathedge *list;
};

pathedge intersecttriangles(object *o1, object*o2, int p1, int p2, int *intersect, int *coplanar)
{
	pathedge result;
	memset(&result,0,sizeof(pathedge));

	vector3 a1 = o1->vertexlist[o1->polygonlist[p1].v1].d;
	vector3 b1 = o1->vertexlist[o1->polygonlist[p1].v2].d;
	vector3 c1 = o1->vertexlist[o1->polygonlist[p1].v3].d;
	vector3 a2 = o2->vertexlist[o2->polygonlist[p2].v1].generated;
	vector3 b2 = o2->vertexlist[o2->polygonlist[p2].v2].generated;
	vector3 c2 = o2->vertexlist[o2->polygonlist[p2].v3].generated;
	
	vector3 v1min,v1max,v2min,v2max;
	v1min.b.x=min(min(a1.b.x,b1.b.x),c1.b.x);
	v1min.b.y=min(min(a1.b.y,b1.b.y),c1.b.y);
	v1min.b.z=min(min(a1.b.z,b1.b.z),c1.b.z);
	v1max.b.x=max(max(a1.b.x,b1.b.x),c1.b.x);
	v1max.b.y=max(max(a1.b.y,b1.b.y),c1.b.y);
	v1max.b.z=max(max(a1.b.z,b1.b.z),c1.b.z);
	v2min.b.x=min(min(a2.b.x,b2.b.x),c2.b.x);
	v2min.b.y=min(min(a2.b.y,b2.b.y),c2.b.y);
	v2min.b.z=min(min(a2.b.z,b2.b.z),c2.b.z);
	v2max.b.x=max(max(a2.b.x,b2.b.x),c2.b.x);
	v2max.b.y=max(max(a2.b.y,b2.b.y),c2.b.y);
	v2max.b.z=max(max(a2.b.z,b2.b.z),c2.b.z);

	//if (boxintersect(v1min,v1max,v2min,v2max))
	{
		vector3 v1,v2;
		*intersect = tri_tri_intersect_with_isectline(a1.a,b1.a,c1.a,
			                                         a2.a,b2.a,c2.a,
													 coplanar,
													 v1.a,v2.a);
		result.a=v1;
		result.b=v2;
	}
	return result;
}

void obj_boolean(object *o1, object*o2, int function)
{
	matrix m;
	memcpy(m,o1->xformmatrix,sizeof(matrix));
	m_invert(m);
	matrix o2bckup;
	memcpy(o2bckup,o2->xformmatrix,sizeof(matrix));
	obj_transform(o2,m);
	memcpy(o2->xformmatrix,o2bckup,sizeof(matrix));
	//o1 default poziciot vagjuk o2 aktualis pozicio

	int o1pnum=o1->polynum, o2pnum=o2->polynum;
	pathedgelist *obj1paths = new pathedgelist[o1->polynum], *obj2paths  = new pathedgelist[o2->polynum];
	
	/*object *dummy1=newobject(),*dummy2=newobject();
	dummy1->polygonlist = new polygon [o1->polynum];
	dummy2->polygonlist = new polygon [o2->polynum];
	memcpy(dummy1->polygonlist,o1->polygonlist,sizeof(polygon)*o1->polynum);
	memcpy(dummy2->polygonlist,o2->polygonlist,sizeof(polygon)*o2->polynum);*/

	int coplanar=1;
	for (int x=o1->polynum-1;x>=0;x--)
		for (int y=o2->polynum-1;y>=0;y--)
		{
			coplanar=1;
			pathedge section;
			if (tri_tri_intersect_with_isectline(o1->vertexlist[o1->polygonlist[x].v1].d.a,o1->vertexlist[o1->polygonlist[x].v2].d.a,o1->vertexlist[o1->polygonlist[x].v3].d.a,
			                                     o2->vertexlist[o2->polygonlist[y].v1].d.a,o2->vertexlist[o2->polygonlist[y].v2].d.a,o2->vertexlist[o2->polygonlist[y].v3].d.a,
												 &coplanar,
												 section.a.a,section.b.a) && coplanar)
			{
				char ss[256];
				sprintf(ss,"%d",coplanar);
				MessageBox( 0, ss, "HelloWorld", MB_OK );
				addvertex(o1,section.a.b.x,section.a.b.y,section.a.b.z);
				addvertex(o1,section.b.b.x,section.b.b.y,section.b.b.z);
				addpolygon(o1,o1->vertexnum-2,o1->vertexnum-1,o1->vertexnum-1,dd3d_flatshade);
			}
		}
	obj_transform(o1,o1->xformmatrix);
	obj_transform(o2,o2->xformmatrix);
}

