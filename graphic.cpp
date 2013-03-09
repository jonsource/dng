#include "graphic.h"
#include <string.h>
#include <math.h>

BITMAP * sky1[4];
BITMAP * api;
RGB pal[256];
int FOV;
int **line_sight();
double STB;
extern BITMAP *game_bmp;
extern int fps, tot_frames, tmsec, mode;
extern int TRANSPARENT;
extern TEXTURE ** Textures;
extern TEXTURED_ELEMENT ** Elements;
extern ANIMATOR ** Animators;
extern TILE ** Tiles;
FILE *dbg;

extern int **map;
extern int **linesight;
extern unsigned short int MAP_SIZE;

TEXTURE * floor1, * floor2, * floor3, * floor4;
TEXTURE * wall1, * wall2;
TEXTURE * static1, *static2, *static3;
TEXTURED_ELEMENT * element1, * element2, * element3, * element4, * element5, * element6;
CAMERA cam;

float dist(int x,int z,CAMERA * cam)
{ return ((x-(int)cam->xpos)*(x-(int)cam->xpos))+((z-(int)cam->zpos)*(z-(int)cam->zpos));

}

int check_coords(int x, int y)
{ if(x<0 || y<0 || x>=MAP_SIZE || y>=MAP_SIZE) return 0;
  else return map[x][y];
}

int see_coords(int x, int y)
{ if(check_coords(x,y)) return linesight[x][y];
  return 0;
}

TEXTURED_ELEMENT * load_textured_element(int i)
{	TEXTURED_ELEMENT * txtel = new(TEXTURED_ELEMENT);
	txtel->transparent=false;
	if(i==1)
	{txtel->animator = NULL;
	txtel->w=1;
	txtel->h=1;
	txtel->x=0;
	txtel->z=0;
	txtel->y=0;
	txtel->texture = floor1;
	txtel->type = TILE_FLOOR;
	return txtel;}
	if(i==2)
	{	txtel->animator = NULL;
		txtel->w=1;
		txtel->h=1;
		txtel->x=0;
		txtel->z=0;
		txtel->y=0;
		txtel->texture = wall1;
		txtel->type = TILE_FRONT;
		return txtel;
	}
	if(i==3)
	{	txtel->animator = NULL;
		txtel->w=1;
		txtel->h=1;
		txtel->x=0;
		txtel->z=0;
		txtel->y=0;
		txtel->texture = wall1;
		txtel->type = TILE_SIDE;
		return txtel;
	}
	if(i==4)
	{	txtel->animator = NULL;
		txtel->transparent = true;
		txtel->w=1.5;
		txtel->h=0.5;
		txtel->x=0.5;
		txtel->z=0.5;
		txtel->y=0;
		txtel->texture = static1;
		txtel->type = TILE_STATIC;
		return txtel;
	}
	if(i==5)
		{	ANIMATOR * an = new(ANIMATOR);
			an->frames = 8;
			an->offset = 128;
			an->speed = 100;
			txtel->animator = an;
			txtel->w=1;
			txtel->h=1;
			txtel->x=0;
			txtel->z=0.5;
			txtel->y=0;
			txtel->texture = static3;
			an->frame = create_bitmap(txtel->texture->close->w,txtel->texture->close->w);
			txtel->type = TILE_STATIC;
			return txtel;
		}
	if(i==6)
		{	txtel->animator = NULL;
			txtel->w=1;
			txtel->h=1;
			txtel->x=0.5;
			txtel->z=0;
			txtel->y=0;
			txtel->texture = static2;
			txtel->type = TILE_STATIC;
			return txtel;
		}
}

int load_graphics()
{   string s;
    PALETTE pal;

          s="data/images/sky1/skylarge1.bmp";
      sky1[0] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing skylarge1 texture!\n",4);
      s="data/images/sky1/sky1.bmp";
      sky1[1] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky1 texture!\n",4);
      s="data/images/sky1/sky2.bmp";
      sky1[2] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky2 texture!\n",4);
      s="data/images/sky1/sky3.bmp";
      sky1[3] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky3 texture!\n",4);
      s="data/images/api/api_mask.bmp";
      api = load_bmp(s.c_str(), pal);
      if(!api) debug("Missing api_mask texture!\n",4);
      debug("done load graphics\n",10);

return 1;
}

void unload_graphics()
{ delete [] (void **) floor1;
//  delete [] (void **) floor2;
//  delete [] (void *) fwall1;
  delete [] (void **) wall1;
}

/* updates the camera matrix 
   needs the variables heading, pitch and roll to be set */

void update_camera(CAMERA * cam)
{
     /* calculate the in-front vector */
/*   cam->xfront = sin(cam->heading) * cos(cam->pitch);
   cam->yfront = sin(cam->pitch);
   cam->zfront = cos(cam->heading) * cos(cam->pitch);*/

   switch(cam->heading)
   { case HEAD_EAST : cam->xfront = 0; cam->zfront = 1; break;
     case HEAD_NORTH : cam->xfront = 1; cam->zfront = 0; break;
     case HEAD_WEST : cam->xfront = 0; cam->zfront = -1; break;
     case HEAD_SOUTH : cam->xfront = -1; cam->zfront = 0; break;
     
   }
   cam->dolly_xpos=cam->xpos+0.5;
   cam->dolly_ypos=cam->ypos+0.5;
   cam->dolly_zpos=cam->zpos+0.5;
   if(cam->xfront==0) cam->xpos+=0.5;
   	  else cam->xpos+=(cam->xfront>0)?cam->stepback:1-cam->stepback;
   if(cam->zfront==0) cam->zpos+=0.5;  //ano zfront a ypos tu maj byt takhle
      else cam->zpos+=(cam->zfront>0)?cam->stepback:1-cam->stepback;
   cam->ypos+=0.5;
   cam->yfront=0;

   /* rotate the up vector around the in-front vector by the roll angle */
   get_vector_rotation_matrix_f(&cam->roller, cam->xfront, cam->yfront, cam->zfront,
				cam->roll*128.0/M_PI);
   apply_matrix_f(&cam->roller, 0, 1, 0, &cam->xup, &cam->yup, &cam->zup);

   /* build the camera matrix */
   get_camera_matrix_f(&cam->camera,
		       cam->xpos, cam->ypos, cam->zpos,        /* camera position */
		       cam->xfront, cam->yfront, cam->zfront,  /* in-front vector */
		       cam->xup, cam->yup, cam->zup,           /* up vector */
		       cam->fov,                               /* field of view */
		       cam->aspect);                           /* aspect ratio */
}

void render_element(int type, TEXTURED_ELEMENT * element, BITMAP *bmp, int x, int z, CAMERA * cam,int far)
{  V3D_f _v[4], _vout[8], _vtmp[8];
   V3D_f *v[4], *vout[8], *vtmp[8];
   int flags[4], out[8];
   int c, vc;
   int polytype;

   if(TRANSPARENT && element->transparent)
   {	polytype = POLYTYPE_PTEX_MASK_TRANS;
   	   set_alpha_blender();
   }
   else
   {	polytype = POLYTYPE_PTEX_MASK_LIT;
   	   set_trans_blender(0,0,0,128);
   }

   for (c=0; c<4; c++)
	   v[c] = &_v[c];

   for (c=0; c<8; c++) {
      vout[c] = &_vout[c];
      vtmp[c] = &_vtmp[c];
   }

   switch(type)
   {   case TILE_FLOOR: make_floor_element(v,element,x,z,cam,far); break;
   	   case TILE_CEILING: make_ceiling_element(v,element,x,z,cam,far); break;
   	   case TILE_FLAT: make_flat_element(v,element,x,z,cam,far); break;
   	   case TILE_FRONT: make_front_element(v,element,x,z,cam,far); break;
   	   case TILE_SIDE: make_side_element(v,element,x,z,cam,far); break;
   	   case TILE_STATIC: make_static_element(v,element,x,z,cam,far); break;
   	   case TILE_STATIC_NS: make_static_element_ns(v,element,x,z,cam,far); break;
   	   case TILE_STATIC_EW: make_static_element_ew(v,element,x,z,cam,far); break;
   	   default: debug("Missing or bad tile type in render element!",3); break;
   }
   for (c=0; c<4; c++)
   {  	v[c]->c=256-dist(v[c]->x,v[c]->z,cam)*3;

   }

   /* apply the camera matrix, translating world space -> view space */
   for (c=0; c<4; c++) {
      apply_matrix_f(&cam->camera, v[c]->x, v[c]->y, v[c]->z,
		     &v[c]->x, &v[c]->y, &v[c]->z);

      flags[c] = 0;

      /* set flags if this vertex is off the edge of the screen */
      if (v[c]->x < -v[c]->z)
	 flags[c] |= 1;
      else if (v[c]->x > v[c]->z)
	 flags[c] |= 2;

      if (v[c]->y < -v[c]->z)
	 flags[c] |= 4;
      else if (v[c]->y > v[c]->z)
	 flags[c] |= 8;

      if (v[c]->z < 0.1)
	 flags[c] |= 16;
   }
   /* quit if all vertices are off the same edge of the screen */
   if (flags[0] & flags[1] & flags[2] & flags[3])
      return;

   if (flags[0] | flags[1] | flags[2] | flags[3]) {
      /* clip if any vertices are off the edge of the screen */
      vc = clip3d_f(polytype, 0.45, 0, 4, (AL_CONST V3D_f **)v,
		    vout, vtmp, out);

      if (vc <= 0)
	 return;
   }
   else {
      /* no need to bother clipping this one */
      vout[0] = v[0];
      vout[1] = v[1];
      vout[2] = v[2];
      vout[3] = v[3];

      vc = 4;
   }
   /* project view space -> screen space */
   for (c=0; c<vc; c++)
   {   	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z, &vout[c]->x, &vout[c]->y);
   }
   /* render the polygon */
   //debug("before polygon 3d "+to_str(x)+" "+to_str(z),1);
   polygon3d_f(bmp, polytype, far_texture(element,far), vc, vout);
}

BITMAP * far_texture(TEXTURED_ELEMENT * txt, int far)
{	if(txt->animator)
	{	int frame=(tmsec/txt->animator->speed)%txt->animator->frames;
		blit(txt->texture->close,txt->animator->frame,0,frame*txt->animator->offset,0,0,txt->animator->frame->w,txt->animator->frame->h);
		return txt->animator->frame;
	}
	switch(far)
	{ case 0: return txt->texture->close;
	  case 1: return txt->texture->medium;
	  case 2: return txt->texture->far;
	}
	return txt->texture->close;
}

void render_tile(TILE * tile,BITMAP * bmp, int x, int z, CAMERA * cam, int far)
{	//debug("render_tile "+to_str(x)+" "+to_str(z),1);
	for(int i=0; i<tile->len; i++)
	{	if(tile->types[i]==TILE_FRONT)
		{	render_element(TILE_SIDE,tile->elements[i],bmp,x,z,cam,far);
			render_element(TILE_FRONT,tile->elements[i],bmp,x,z,cam,far);
		}
		else render_element(tile->types[i],tile->elements[i],bmp,x,z,cam,far);
	}
}

/* nakresli tile z mapy */
void draw_tile(BITMAP *bmp, CAMERA * cam, int x, int z)
{ 	unsigned short int far;
	int d = dist(x,z,cam);

	far=2;
	if(d<9) { far=1;}
	if(d<4) { far=0;}

	render_tile(Tiles[map[x][z]-1],bmp,x,z,cam,far);
	//debug("end draw tile "+to_str(x)+" "+to_str(z),1);
}

void see_tile(int x, int z, CAMERA cam)
{	//debug("  see tile "+to_str(x)+" "+to_str(z),1);
	int s=check_coords(x,z);
	int see=1;
	if(s)
	{ if(linesight[x][z])
	  { dappend(" already seen",1);
		return;
	  }
	  linesight[x][z]=s;
	  for(int i=0;i<Tiles[s-1]->len;i++)
		  if(Tiles[s-1]->types[i]==TILE_FRONT) { see=0; return; }
	  if(see)
	  {   if(!see_coords(x+cam.xfront,z+cam.zfront)) see_tile(x+cam.xfront,z+cam.zfront,cam);
	  	  if(cam.xfront==0)
	  	  { if(x>=cam.dolly_xpos-1 && !see_coords(x+1,z+cam.zfront)) see_tile(x+1,z+cam.zfront,cam);
	  	    if(x<cam.dolly_xpos && !see_coords(x-1,z+cam.zfront)) see_tile(x-1,z+cam.zfront,cam);
	  	  }
	  	  else
	  	  { if(z>=cam.dolly_zpos-1 && !see_coords(x+cam.xfront,z+1)) see_tile(x+cam.xfront,z+1,cam);
	  	  	if(z<cam.dolly_zpos && !see_coords(x+cam.xfront,z-1)) see_tile(x+cam.xfront,z-1,cam);
	  	  }
	  }
	}
	//dappend("  done "+to_str(x)+" "+to_str(z));
}

void make_linesight(int x, int z, CAMERA cam)
{	debug(" linesight "+to_str(x)+" "+to_str(z),1);
	for(int i=0; i<MAP_SIZE; i++)
		for(int j=0; j<MAP_SIZE; j++)
			linesight[i][j]=0;
	debug(" * after sight reset",1);
	// three initial tiles
	see_tile(x,z,cam);
	if(cam.xfront==0)
	{ 	see_tile(x-1,z,cam);
		see_tile(x+1,z,cam);
	}
	else
	{ 	see_tile(x,z+1,cam);
		see_tile(x,z-1,cam);
	}
}

void draw_view(int xpos, int ypos, int zpos, int heading)
{  int c,x, y, w, h, dx, dz;
//   float xfront, yfront, zfront;
//   float xup, yup, zup;
   double head=0;   
//   float pitch=0;
   cam.pitch=0;
//   float roll=0;
   cam.roll=0;
//   float fov = 75;
   cam.fov=FOV;
   cam.stepback=STB;
   cam.ypos = ypos;
   cam.xpos=xpos;
   cam.zpos=zpos;
   cam.aspect = 1.33f;
   debug("begin draw_view()",3);
   
   //blit(api, game_bmp, 0, 0, 0, 0, 640, 480);
   
   c=(tmsec/500)%2560;  

   switch(heading)
   { case 0: blit(sky1[0], game_bmp, 0+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 1: blit(sky1[0], game_bmp, 640+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 2: blit(sky1[0], game_bmp, 1280+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 3: blit(sky1[0], game_bmp, 1920+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
   }


   cam.heading=heading;
   update_camera(&cam);

   make_linesight(xpos,zpos,cam);
   debug(" * in draw_view after linesight",1);
  int fx, fz;
 for(dx=MAX_VIEW_DIST; dx>=0; dx--)
     for(dz=dx+1; dz>=0; dz--)
     { fx=xpos+dx*cam.xfront+dz*cam.zfront;  // ano zfront, y je nahoru
       fz=zpos+dz*cam.xfront+dx*cam.zfront;
       if(see_coords(fx,fz))
         draw_tile(game_bmp, &cam, fx, fz);
       fx=xpos+dx*cam.xfront-dz*cam.zfront;  // ano zfront, y je nahoru
       fz=zpos-dz*cam.xfront+dx*cam.zfront;
       if(see_coords(fx,fz))
         draw_tile(game_bmp, &cam, fx, fz);
     }

    /* overlay some text */
   BITMAP *bmp = game_bmp;  
   set_clip_rect(bmp, 0, 0, bmp->w, bmp->h);
  // textprintf_ex(bmp, font, 0,  0, makecol(0, 0, 0), -1,
//		 "Viewport width: %d  height: %d", viewport_w,viewport_h);
   //textprintf_ex(bmp, font, 0,  32, makecol(0, 0, 0), -1,
		 //"Viewport height: %d (h/H changes)", viewport_h);
   textprintf_ex(bmp, font, 0, 40, makecol(0, 0, 0), -1,
		 "Field of view: %f  Aspect: %.2f", cam.fov,cam.aspect);
   //textprintf_ex(bmp, font, 0, 48, makecol(0, 0, 0), -1,
		 //"Aspect ratio: %.2f (a/A changes)", cam.aspect);
   textprintf_ex(bmp, font, 0, 56, makecol(0, 0, 0), -1,
		 "Position X: %.2f  Y: %.2f  Z: %.2f", (float)cam.xpos,(float)cam.ypos,(float)cam.zpos);
   //textprintf_ex(bmp, font, 0, 64, makecol(0, 0, 0), -1,
	//	 "Y position: %.2f (y/Y changes)", (float)cam.ypos);
   //textprintf_ex(bmp, font, 0, 72, makecol(0, 0, 0), -1,
	//	 "Z position: %.2f (z/Z changes)", (float)cam.zpos);
   textprintf_ex(bmp, font, 0, 80, makecol(0, 0, 0), -1,
		 "Heading: %d  Stepback: %.2f", cam.heading,cam.stepback);
   textprintf_ex(bmp, font, 0, 88, makecol(0, 0, 0), -1,
		 "Pitch: %.2f deg (pgup/pgdn changes)", cam.pitch);
   textprintf_ex(bmp, font, 0, 96, makecol(0, 0, 0), -1,
		 "Roll: %.2f deg (r/R changes)", cam.roll);
   textprintf_ex(bmp, font, 0, 104, makecol(0, 0, 0), -1,
		 "Front vector: %d, %d, %d", cam.xfront, cam.yfront, cam.zfront);
   textprintf_ex(bmp, font, 0, 112, makecol(0, 0, 0), -1,
		 "Up vector: %.2f, %.2f, %.2f", cam.xup, cam.yup, cam.zup);
   textprintf_ex(bmp, font, 0, 120, makecol(0, 0, 0), -1,
		 "Frames per second: %d", fps);
   
}
