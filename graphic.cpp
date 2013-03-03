#include "graphic.h"
#include <string.h>
#include <math.h>

BITMAP * sky1[4];
BITMAP * api;
RGB pal[256];
int FOV;
double STB;
extern BITMAP *game_bmp;
extern int fps, tot_frames, tmsec, mode;
extern int TRANSPARENT;
FILE *dbg;

extern int **map;
extern unsigned short int MAP_SIZE;

TEXTURE * floor1, * floor2, * floor3, * floor4;
TEXTURE * wall1, * wall2;
TEXTURE * static1, *static2, *static3;
TEXTURED_ELEMENT * element1;
CAMERA cam;

int check_coords(int x, int y)
{ if(x<0 || y<0 || x>=MAP_SIZE || y>=MAP_SIZE) return 0;
  else return map[x][y];
}

TEXTURE * load_texture(string s)
{   PALETTE pal;
    TEXTURE *text;
    string fn,cfn,ext;
    char buf[30];
    int pos;
    
    text = new (TEXTURE);
    fn.assign("data/images/");

    pos = s.rfind(".");
    fn.append(s.substr(0,pos));
    ext = s.substr(pos+1);

    cfn.assign(fn);
    cfn.append("_close."+ext);
    text->close = load_bitmap(cfn.c_str(), pal);
    if(!text->close) { debug("Missing texture"); debug(cfn.c_str()); }
    else { sprintf(buf,"Texture depth %d\n",bitmap_color_depth(text->close)); debug(buf); }
    cfn.assign(fn);
    cfn.append("_medium."+ext);
    text->medium = load_bitmap(cfn.c_str(), pal);
    if(!text->medium)
    { 	debug("Missing texture"); debug(cfn.c_str());
    	if(text->close)
    	{ debug("resizing close texture\n");
    	  text->medium = create_bitmap(128,128);
    	  stretch_blit(text->close,text->medium,0,0,text->close->w,text->close->h,0,0,text->medium->w,text->medium->h);
    	}
    	else debug("\n");
    }
    cfn.assign(fn);
    cfn.append("_far."+ext);
    text->far = load_bitmap(cfn.c_str(), pal);
    if(!text->far)
    { 	debug("Missing texture"); debug(cfn.c_str());
		if(text->medium)
    	{ debug("resizing medium texture\n");
    	  text->far = create_bitmap(64,64);
    	  stretch_blit(text->medium,text->far,0,0,text->medium->w,text->medium->h,0,0,text->far->w,text->far->h);
    	}
    	else debug("\n");
    }
    return text;
}

TEXTURED_ELEMENT * load_textured_element()
{	TEXTURED_ELEMENT * txtel = new(TEXTURED_ELEMENT);
	txtel->animator = NULL;
	txtel->w=1.5;
	txtel->h=0.5;
	txtel->x=0.5;
	txtel->z=0.5;
	txtel->y=0;
	txtel->texture = static1;
	txtel->type = TILE_STATIC;
	return txtel;
}

int load_graphics()
{   int i,j,ii,fi,a,b;
    string s;
    PALETTE pal;
    
    floor1 = load_texture("floor/floor1.bmp");
    floor2 = load_texture("floor/floor2.bmp");
    floor3 = load_texture("floor/floor3.bmp");
    floor4 = load_texture("floor/floor4.bmp");
    wall1 = load_texture("wall/wall1.bmp");
    wall2 = load_texture("wall/wall2.bmp");
	static1 = load_texture("static/mist1.tga");
	static2 = load_texture("static/door.bmp");
	static3 = load_texture("static/explosion.bmp");

	element1 = load_textured_element();
    
    debug("Done loading tiles!\n");
      
      s="data/images/sky1/skylarge1.bmp";
      sky1[0] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing skylarge1 texture!\n",3);
      s="data/images/sky1/sky1.bmp";
      sky1[1] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky1 texture!\n",3);
      s="data/images/sky1/sky2.bmp";
      sky1[2] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky2 texture!\n",3);
      s="data/images/sky1/sky3.bmp";
      sky1[3] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky3 texture!\n",3);
      s="data/images/api/api_mask.bmp";
      api = load_bmp(s.c_str(), pal);
      if(!api) debug("Missing api_mask texture!\n",3);
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

void render_tile(int type, BITMAP *bmp, BITMAP * text, int w, int h, CAMERA * cam, int x, int z)
{  V3D_f _v[4], _vout[8], _vtmp[8];
   V3D_f *v[4], *vout[8], *vtmp[8];
   int flags[4], out[8];
   int c, vc;

   // inicializace pointeru
   for (c=0; c<4; c++)
      v[c] = &_v[c];

   // inicializace pointeru
   for (c=0; c<8; c++) {
      vout[c] = &_vout[c];
      vtmp[c] = &_vtmp[c];
   }
   
   //make_static_tile(v,x,z,w,h,cam);

   switch(type)
   { case TILE_FLOOR: make_floor_tile(v,x,z,w,h);
   	   	   	 debug("  floor tile",2);
   	   	   	 break;
     case TILE_STATIC: make_static_tile(v,x,z,w,h,cam);
     	 	 debug("  static tile",2);
     	 	 break;
     case TILE_FRONT: make_front_tile(v,x,z,w,h,cam);
     	 	 debug("  front tile",2);
     	 	 break;
     case TILE_SIDE: make_side_tile(v,x,z,w,h,cam);
     	 	 debug("  side tile",2);
     	 	 break;
     case TILE_STATIC_NS: make_static_ns_tile(v,x,z,w,h);
          	 	 debug("  static NS tile",2);
          	 	 break;
     case TILE_STATIC_EW: make_static_ew_tile(v,x,z,w,h);
               	 	 debug("  static EW tile",2);
               	 	 break;
     case TILE_CEILING: make_ceiling_tile(v,x,z,w,h);
     	 	 debug("  ceiling tile",2);
     	 	 break;
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
   debug("  before clipping!\n");
   /* quit if all vertices are off the same edge of the screen */
   if (flags[0] & flags[1] & flags[2] & flags[3])
      return;

   if (flags[0] | flags[1] | flags[2] | flags[3]) {
       /*clip if any vertices are off the edge of the screen */
      vc = clip3d_f(POLYTYPE_PTEX_MASK, 0.45, 0, 4, (AL_CONST V3D_f **)v,
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
   debug("  before projection!\n");
   /* project view space -> screen space */
   for (c=0; c<vc; c++)
      persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
		      &vout[c]->x, &vout[c]->y);
   debug("  before polygon3d!\n");
   if(!text) debug("  missing texture!\n");
   /* render the polygon */
   polygon3d_f(bmp, POLYTYPE_PTEX_MASK, text, vc, vout);
   debug("  after polygon3d!\n");
} 

void render_element(TEXTURED_ELEMENT * element, BITMAP *bmp, int x, int z, CAMERA * cam,int far)
{  V3D_f _v[4], _vout[8], _vtmp[8];
   V3D_f *v[4], *vout[8], *vtmp[8];
   int flags[4], out[8];
   int c, vc;

   float dist = ((x-cam->xpos)*(x-cam->xpos))+((z-cam->zpos)*(z-cam->zpos));

   for (c=0; c<4; c++)
      v[c] = &_v[c];

   for (c=0; c<8; c++) {
      vout[c] = &_vout[c];
      vtmp[c] = &_vtmp[c];
   }


   make_static_element(v,element,x,z,cam,far);

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
   debug("  before clipping!\n");
   /* quit if all vertices are off the same edge of the screen */
   if (flags[0] & flags[1] & flags[2] & flags[3])
      return;

   if (flags[0] | flags[1] | flags[2] | flags[3]) {
      /* clip if any vertices are off the edge of the screen */
      vc = clip3d_f(POLYTYPE_PTEX_MASK, 0.45, 0, 4, (AL_CONST V3D_f **)v,
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
   debug("  in elment before projection!\n");
   /* project view space -> screen space */
   for (c=0; c<vc; c++)
      persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
		      &vout[c]->x, &vout[c]->y);
   debug("  in element before polygon3d!\n");
   if(!element->texture->close) debug("  missing texture!\n");
   /* render the polygon */
   polygon3d_f(bmp, TRANSPARENT?POLYTYPE_PTEX_MASK_TRANS:POLYTYPE_PTEX_MASK, far_texture(element->texture,far), vc, vout);

   debug("  in element after polygon3d!\n");
}

BITMAP * far_texture(TEXTURE * txt, int far)
{	/*if(txt->animated)
	{	int frame=(tmsec/txt->anim_speed)%txt->anim_frames;
		//destroy_bitmap(txt->medium);
		//txt->medium=create_bitmap(txt->close->w,txt->close->w);
		blit(txt->close,txt->medium,0,frame*txt->anim_offset,0,0,txt->close->w,txt->close->h);
		return txt->medium;
	}*/
	switch(far)
	{ case 0: return txt->close;
	  case 1: return txt->medium;
	  case 2: return txt->far;
	}
	return txt->close;
}

/* nakresli tile z mapy */
void draw_tile(BITMAP *bmp, CAMERA * cam, int x, int z)
{ TEXTURE * ct_wall,* ct_static,* ct_floor;
  int wf,hf,far;
  int dist = ((x-(int)cam->xpos)*(x-(int)cam->xpos))+((z-(int)cam->zpos)*(z-(int)cam->zpos));

  if((x+z)%2) ct_wall=wall1;
  else ct_wall = wall2;
  ct_static=static1;
  switch(x%2+(z%2)*2)
  { case 0: ct_floor=floor1; break;
    case 1: ct_floor=floor2; break;
    case 2: ct_floor=floor3; break;
    case 3: ct_floor=floor4; break;
  }
  ct_static = static2;
  switch(map[x][z])
  { case 2: ct_static=static1; break;
    case 5: ct_static=static2; break;
    case 6: ct_static=static3; break;
  }

  wf=64; hf=64; far=2;
  if(dist<9) { wf=128; hf=128; far=1;}
  if(dist<4) { wf=256; hf=256; far=0;}

  switch(map[x][z])
  { case 1: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z); break;
    case 2: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
            //render_tile(TILE_STATIC, bmp, far_texture(ct_static,far), wf, hf, cam, x, z); break;
    		render_element(element1,bmp,x,z,cam,far); break;
    case 3: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
            render_tile(TILE_SIDE, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
            render_tile(TILE_FRONT, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z); break;
    case 4: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
    		render_tile(TILE_CEILING, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
            break;
    case 5: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
        	render_tile(TILE_CEILING, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
        	//render_tile(TILE_STATIC_NS, bmp, far_texture(ct_static,far), wf, hf, cam, x, z); break;
            break;  
    case 6: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
           	render_tile(TILE_CEILING, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
           	//render_tile(TILE_STATIC, bmp, far_texture(ct_static,far), wf, hf, cam, x, z); break;
            break;
  }
}

void draw_view(int xpos, int zpos, int heading)
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
   cam.ypos = 0.5;
   cam.xpos=xpos;
   cam.zpos=zpos;
   cam.aspect = 1.33f;
   int viewport_w = 640;
   int viewport_h = 320;
   debug("begin draw_view()\n");
   
   //blit(api, game_bmp, 0, 0, 0, 0, 640, 480);
   
   c=(tmsec/500)%2560;  

   switch(heading)
   { case 0: blit(sky1[0], game_bmp, 0+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 1: blit(sky1[0], game_bmp, 640+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 2: blit(sky1[0], game_bmp, 1280+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 3: blit(sky1[0], game_bmp, 1920+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
   }


   /* set up the viewport region */
   x = (SCREEN_W - viewport_w) / 2;
   //y = (SCREEN_H - viewport_h) / 2;
   y=20;
   w = viewport_w;
   h = viewport_h;

   set_projection_viewport(x, y, w, h);
   rect(game_bmp, x, y, x+w-1, y+h-1, makecol(255, 0, 0));
   set_clip_rect(game_bmp, x, y, x+w-1, y+h-1);
   
   cam.heading=heading;
   update_camera(&cam);

   
  int fx, fz;
 for(dx=11; dx>=0; dx--)
     for(dz=dx+1; dz>=0; dz--)
     { fx=xpos+dx*cam.xfront+dz*cam.zfront;  // ano zfront, y je nahoru
       fz=zpos+dz*cam.xfront+dx*cam.zfront;
       if(check_coords(fx,fz))
         draw_tile(game_bmp, &cam, fx, fz);
       fx=xpos+dx*cam.xfront-dz*cam.zfront;  // ano zfront, y je nahoru
       fz=zpos-dz*cam.xfront+dx*cam.zfront;
       if(check_coords(fx,fz))
         draw_tile(game_bmp, &cam, fx, fz);
     }

    /* overlay some text */
   BITMAP *bmp = game_bmp;  
   set_clip_rect(bmp, 0, 0, bmp->w, bmp->h);
   textprintf_ex(bmp, font, 0,  0, makecol(0, 0, 0), -1,
		 "Viewport width: %d  height: %d", viewport_w,viewport_h);
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
