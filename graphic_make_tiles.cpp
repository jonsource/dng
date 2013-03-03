/*
 * graphic_make_tiles.cpp
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#include "graphic.h"

void make_floor_tile(V3D_f **v,int x, int z, int w, int h)
{  v[0]->x = x;
   v[0]->y = 0;
   v[0]->z = z;
   v[0]->u = 0;
   v[0]->v = 0;

   v[1]->x = x + 1;
   v[1]->y = 0;
   v[1]->z = z;
   v[1]->u = w;
   v[1]->v = 0;

   v[2]->x = x + 1;
   v[2]->y = 0;
   v[2]->z = z + 1;
   v[2]->u = w;
   v[2]->v = h;

   v[3]->x = x;
   v[3]->y = 0;
   v[3]->z = z + 1;
   v[3]->u = 0;
   v[3]->v = h;
}

void make_ceiling_tile(V3D_f **v,int x, int z, int w, int h)
{  v[0]->x = x;
   v[0]->y = 1;
   v[0]->z = z;
   v[0]->u = 0;
   v[0]->v = 0;

   v[1]->x = x + 1;
   v[1]->y = 1;
   v[1]->z = z;
   v[1]->u = w;
   v[1]->v = 0;

   v[2]->x = x + 1;
   v[2]->y = 1;
   v[2]->z = z + 1;
   v[2]->u = w;
   v[2]->v = h;

   v[3]->x = x;
   v[3]->y = 1;
   v[3]->z = z + 1;
   v[3]->u = 0;
   v[3]->v = h;
}

void make_static_tile(V3D_f **v,int x, int z, int w, int h, CAMERA * cam)
{
   if(cam->xfront<0.5 && cam->xfront>-0.5)
   { v[0]->x = x+0;
     v[0]->y = 0;
     v[0]->z = z+0.5;
     v[0]->u = w;
     v[0]->v = h;

   v[1]->x = x + 1;
   v[1]->y = 0;
   v[1]->z = z + 0.5;
   v[1]->u = 0;
   v[1]->v = h;

   v[2]->x = x + 1;
   v[2]->y = 2;
   v[2]->z = z + 0.5;
   v[2]->u = 0;
   v[2]->v = 0;

   v[3]->x = x + 0;
   v[3]->y = 2;
   v[3]->z = z + 0.5;
   v[3]->u = w;
   v[3]->v = 0;
   }
   else
   { v[0]->x = x+0.5;
   v[0]->y = 0;
   v[0]->z = z+0;
   v[0]->u = w;
   v[0]->v = h;

   v[1]->x = x + 0.5;
   v[1]->y = 0;
   v[1]->z = z + 1;
   v[1]->u = 0;
   v[1]->v = h;

   v[2]->x = x + 0.5;
   v[2]->y = 2;
   v[2]->z = z + 1;
   v[2]->u = 0;
   v[2]->v = 0;

   v[3]->x = x + 0.5;
   v[3]->y = 2;
   v[3]->z = z + 0;
   v[3]->u = w;
   v[3]->v = 0;
   }

}

void make_static_ns_tile(V3D_f **v,int x, int z, int w, int h)
{
   v[0]->x = x+0;
   v[0]->y = 0;
   v[0]->z = z+0.5;
   v[0]->u = w;
   v[0]->v = h;

   v[1]->x = x + 1;
   v[1]->y = 0;
   v[1]->z = z + 0.5;
   v[1]->u = 0;
   v[1]->v = h;

   v[2]->x = x + 1;
   v[2]->y = 1;
   v[2]->z = z + 0.5;
   v[2]->u = 0;
   v[2]->v = 0;

   v[3]->x = x + 0;
   v[3]->y = 1;
   v[3]->z = z + 0.5;
   v[3]->u = w;
   v[3]->v = 0;
}

void make_static_ew_tile(V3D_f **v,int x, int z, int w, int h)
{ v[0]->x = x+0.5;
 v[0]->y = 0;
 v[0]->z = z+0;
 v[0]->u = w;
 v[0]->v = h;

 v[1]->x = x + 0.5;
 v[1]->y = 0;
 v[1]->z = z + 1;
 v[1]->u = 0;
 v[1]->v = h;

 v[2]->x = x + 0.5;
 v[2]->y = 1;
 v[2]->z = z + 1;
 v[2]->u = 0;
 v[2]->v = 0;

 v[3]->x = x + 0.5;
 v[3]->y = 1;
 v[3]->z = z + 0;
 v[3]->u = w;
 v[3]->v = 0;
}

void make_front_tile(V3D_f **v,int x, int z, int w, int h, CAMERA * cam)
{  int dif;
   if(cam->xfront<0.5 && cam->xfront>-0.5)  //xfront je nula
   { if(cam->zfront<0.0) dif=1; else dif = 0;
     v[0]->x = x+0;
     v[0]->y = 0;
     v[0]->z = z+dif;
     v[0]->u = w;
     v[0]->v = h;

     v[1]->x = x + 1;
     v[1]->y = 0;
     v[1]->z = z+dif;
     v[1]->u = 0;
     v[1]->v = h;

     v[2]->x = x + 1;
     v[2]->y = 1;
     v[2]->z = z+dif;
     v[2]->u = 0;
     v[2]->v = 0;

     v[3]->x = x + 0;
     v[3]->y = 1;
     v[3]->z = z+dif;
     v[3]->u = w;
     v[3]->v = 0;
   }
   else
   { if(cam->xfront<0.0) dif=1; else dif = 0;

   v[0]->x = x+dif;
   v[0]->y = 0;
   v[0]->z = z+0;
   v[0]->u = w;
   v[0]->v = h;

   v[1]->x = x+dif;
   v[1]->y = 0;
   v[1]->z = z + 1;
   v[1]->u = 0;
   v[1]->v = h;

   v[2]->x = x+dif;
   v[2]->y = 1;
   v[2]->z = z + 1;
   v[2]->u = 0;
   v[2]->v = 0;

   v[3]->x = x+dif;
   v[3]->y = 1;
   v[3]->z = z + 0;
   v[3]->u = w;
   v[3]->v = 0;
   }

}

void make_side_tile(V3D_f **v,int x, int z, int w, int h, CAMERA * cam)
{  int dif=0;
   if(cam->zfront<0.5 && cam->zfront>-0.5)  //zfront je nula
   { if(cam->zpos > z) dif=1; else dif=0;
     v[0]->x = x+0;
     v[0]->y = 0;
     v[0]->z = z+dif;
     v[0]->u = w;
     v[0]->v = h;

     v[1]->x = x + 1;
     v[1]->y = 0;
     v[1]->z = z+dif;
     v[1]->u = 0;
     v[1]->v = h;

     v[2]->x = x + 1;
     v[2]->y = 1;
     v[2]->z = z+dif;
     v[2]->u = 0;
     v[2]->v = 0;

     v[3]->x = x + 0;
     v[3]->y = 1;
     v[3]->z = z+dif;
     v[3]->u = w;
     v[3]->v = 0;
   }
   else
   {
    if(cam->xpos > x) dif=1; else dif=0;
   v[0]->x = x+dif;
   v[0]->y = 0;
   v[0]->z = z+0;
   v[0]->u = w;
   v[0]->v = h;

   v[1]->x = x+dif;
   v[1]->y = 0;
   v[1]->z = z + 1;
   v[1]->u = 0;
   v[1]->v = h;

   v[2]->x = x+dif;
   v[2]->y = 1;
   v[2]->z = z + 1;
   v[2]->u = 0;
   v[2]->v = 0;

   v[3]->x = x+dif;
   v[3]->y = 1;
   v[3]->z = z + 0;
   v[3]->u = w;
   v[3]->v = 0;
   }

}

void make_static_element(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far)
{  BITMAP * text = far_texture(element->texture,far);
   int w = text->w;
   int h = text->h;
   if(cam->xfront<0.5 && cam->xfront>-0.5)
   { v[0]->x = x+element->x-element->w/2;
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
   else
   { v[0]->x = x + element->x;
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


