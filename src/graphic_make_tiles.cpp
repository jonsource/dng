/*
 * graphic_make_tiles.cpp
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#include "graphic.h"

void make_floor_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far)
{	make_flat_element_subr(v, element, x, z, cam, far,0);

}

void make_ceiling_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far)
{	make_flat_element_subr(v, element, x, z, cam, far,1);

}

void make_flat_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far)
{	make_flat_element_subr(v, element, x, z, cam, far,0);

}

void make_flat_element_subr(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far, float height)
{	BITMAP * text = far_texture(element,far);
	int w1=0;
	int w2 = text->w*element->w;
	int h1 =0;
	int h2 = text->h*element->h;
	if( ((element->flip & V_FLIPPING) && x%2) || (element->flip & V_FLIP)  ) { w1=w2; w2=0; } // v-flip
	if( ((element->flip & H_FLIPPING) && z%2) || (element->flip & H_FLIP)  ) { h1=h2; h2=0; } // h-flip

	float elw=element->w/2;
	float elh=element->h/2;
	v[0]->x = x + 0.5 - elw;
	v[0]->y = element->y+height;
	v[0]->z = z + 0.5 - elh;
	if( (element->flip & TURN_90)  ) v[0]->u = w2; // rotate 90 degrees
	else v[0]->u = w1;
	v[0]->v = h1;


	v[1]->x = x + 0.5 + elw;
	v[1]->y = element->y+height;
	v[1]->z = z + 0.5 - elh;
	v[1]->u = w2;
	if( (element->flip & TURN_90)  ) v[1]->v = h2; // rotate 90 degrees
	else v[1]->v = h1;

	v[2]->x = x + 0.5 + elw;
	v[2]->y = element->y+height;
	v[2]->z = z + 0.5 + elh;
	if( (element->flip & TURN_90)  ) v[2]->u = w1; // rotate 90 degrees
	else v[2]->u = w2;
	v[2]->v = h2;

	v[3]->x = x + 0.5 - elw;
	v[3]->y = element->y+height;
	v[3]->z = z + 0.5 + elh;
	v[3]->u = w1;
	if( (element->flip & TURN_90)  ) v[3]->v = h1; // rotate 90 degrees
	else v[3]->v = h2;

}

void make_front_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far)
{  BITMAP * text = far_texture(element,far);
   int w1 = 0;
   int w2 = text->w*element->w;
   int h = text->h*element->h;
   if( ((element->flip & V_FLIPPING) && x%2 == z%2) || (element->flip & V_FLIP) ) { w1=w2; w2=0; } // v-flip
   int dif;
   if(cam->xfront==0)  //xfront je nula
   { if(cam->zfront<0) dif=1; else dif = 0;
     v[0]->x = x+0;
     v[0]->y = element->y;
     v[0]->z = z+dif;
     v[0]->u = w2;
     v[0]->v = h;

     v[1]->x = x + 1;
     v[1]->y = element->y;
     v[1]->z = z+dif;
     v[1]->u = w1;
     v[1]->v = h;

     v[2]->x = x + 1;
     v[2]->y = element->y+element->h;
     v[2]->z = z+dif;
     v[2]->u = w1;
     v[2]->v = 0;

     v[3]->x = x + 0;
     v[3]->y = element->y+element->h;
     v[3]->z = z+dif;
     v[3]->u = w2;
     v[3]->v = 0;
   }
   else
   { if(cam->xfront<0) dif=1; else dif = 0;

   v[0]->x = x+dif;
   v[0]->y = element->y;
   v[0]->z = z+0;
   v[0]->u = w2;
   v[0]->v = h;

   v[1]->x = x+dif;
   v[1]->y = element->y;
   v[1]->z = z + 1;
   v[1]->u = w1;
   v[1]->v = h;

   v[2]->x = x+dif;
   v[2]->y = element->y+element->h;
   v[2]->z = z + 1;
   v[2]->u = w1;
   v[2]->v = 0;

   v[3]->x = x+dif;
   v[3]->y = element->y+element->h;
   v[3]->z = z + 0;
   v[3]->u = w2;
   v[3]->v = 0;
   }

}

void make_side_element(V3D_f **v,TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far)
{  BITMAP * text = far_texture(element,far);
   int w1 = 0;
   int w2 = text->w*element->w;
   int h = text->h*element->h;
   if( ((element->flip & V_FLIPPING) && x%2 == z%2) || (element->flip & V_FLIP) ) { w1=w2; w2=0; } // v-flip
   int dif=0;
   if(cam->zfront==0)  //zfront je nula
   { if(cam->zpos > z) dif=1; else dif=0;
     v[0]->x = x;
     v[0]->y = element->y;
     v[0]->z = z+dif;
     v[0]->u = w2;
     v[0]->v = h;

     v[1]->x = x + 1;
     v[1]->y = element->y;
     v[1]->z = z+dif;
     v[1]->u = w1;
     v[1]->v = h;

     v[2]->x = x + 1;
     v[2]->y = element->y+element->h;
     v[2]->z = z+dif;
     v[2]->u = w1;
     v[2]->v = 0;

     v[3]->x = x + 0;
     v[3]->y = element->y+element->h;
     v[3]->z = z+dif;
     v[3]->u = w2;
     v[3]->v = 0;
   }
   else
   {
    if(cam->xpos > x) dif=1; else dif=0;
   v[0]->x = x+dif;
   v[0]->y = element->y;
   v[0]->z = z+0;
   v[0]->u = w2;
   v[0]->v = h;

   v[1]->x = x+dif;
   v[1]->y = element->y;
   v[1]->z = z + 1;
   v[1]->u = w1;
   v[1]->v = h;

   v[2]->x = x+dif;
   v[2]->y = element->y+element->h;
   v[2]->z = z + 1;
   v[2]->u = w1;
   v[2]->v = 0;

   v[3]->x = x+dif;
   v[3]->y = element->y+element->h;
   v[3]->z = z + 0;
   v[3]->u = w2;
   v[3]->v = 0;
   }

}

void make_static_element_ew(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far)
{	make_static_element_subr(v,element,x,z,cam,far,ELEMENT_EW);
}

void make_static_element_ns(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far)
{	make_static_element_subr(v,element,x,z,cam,far,ELEMENT_NS);
}

void make_static_element(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far)
{	make_static_element_subr(v,element,x,z,cam,far,ELEMENT_ROTATE);
}

void make_static_element_subr(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far, int type)
{  BITMAP * text = far_texture(element,far);
   int w = text->w;
   int h = text->h;
   if(type==ELEMENT_NS || ((type==ELEMENT_ROTATE) && (cam->xfront==0)) )
   {   v[0]->x = x+element->x-element->w/2;
       v[0]->y = 0+element->y;
       v[0]->z = z+element->z;
       v[0]->u = w;
       v[0]->v = h;

       v[1]->x = x + element->x+element->w/2;
       v[1]->y = 0 + element->y;
       v[1]->z = z + element->z;
       v[1]->u = 0;
       v[1]->v = h;

       v[2]->x = x + element->x+element->w/2;
       v[2]->y = element->y+element->h;
       v[2]->z = z + element->z;
       v[2]->u = 0;
       v[2]->v = 0;

       v[3]->x = x + element->x-element->w/2;
       v[3]->y = element->y+element->h;
       v[3]->z = z + element->z;
       v[3]->u = w;
       v[3]->v = 0;
   }
   if(type==ELEMENT_EW || ((cam->zfront==0) && (type==ELEMENT_ROTATE)))
   {   v[0]->x = x + element->x;
       v[0]->y = 0 + element->y;
       v[0]->z = z + element->z+element->w/2;
       v[0]->u = w;
       v[0]->v = h;

       v[1]->x = x + element->x;
       v[1]->y = 0 + element->y;
       v[1]->z = z + element->z-element->w/2;
       v[1]->u = 0;
       v[1]->v = h;

       v[2]->x = x + element->x;
       v[2]->y = element->y+element->h;
       v[2]->z = z + element->z-element->w/2;
       v[2]->u = 0;
       v[2]->v = 0;

       v[3]->x = x + element->x;
       v[3]->y = element->y+element->h;
       v[3]->z = z + element->z+element->w/2;
       v[3]->u = w;
       v[3]->v = 0;
   }

}


