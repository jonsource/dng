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
//int fis[] = {0,1,2,3,4,5,1,2,3,4,5};
//int gis[] = {5,4,3,2,1,0,6,7,8,9,10};
FILE *dbg;


extern int **map;
extern unsigned short int MAP_SIZE;

/*[MAP_SIZE][MAP_SIZE] = {
              {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
              {1,1,1,1,5,1,1,1,1,1,1,1,1,1,1},
              {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
              {1,1,1,5,1,1,1,1,1,1,1,1,4,4,3},
              {3,3,3,3,3,3,3,3,3,3,3,4,3,3,3},
              {1,1,3,3,3,2,1,2,1,1,3,4,3,1,1},
              {1,3,3,4,4,2,1,2,1,1,3,5,3,1,1},
              {3,3,4,4,4,2,1,2,1,1,3,4,3,1,1},
              {3,4,4,4,4,3,3,1,1,1,3,4,3,1,1},
              {3,4,4,4,4,2,3,2,1,3,3,4,3,3,1},
              {1,3,3,3,4,3,3,3,3,3,4,4,4,3,1},
              {1,3,4,4,4,2,4,2,4,5,4,4,4,3,1},
              {1,3,3,3,4,3,3,3,3,3,4,4,4,3,1},
              {1,1,1,3,4,3,1,1,1,3,3,4,3,3,1},
              {1,1,1,3,3,3,1,1,1,1,3,4,3,1,1},

                         
                             };*/

typedef struct { 
        float xpos, zpos, ypos;        /* camera position */
		int xfront, yfront, zfront;  /* in-front vector */
		float xup, yup, zup;           /* up vector */
		float fov;                     /* field of view */
		float pitch, roll;
		float aspect;                 /* aspect ratio */
		int heading;				/* camera heading - to count infront vector*/
		double stepback; 					/* camera stepback */
		MATRIX_f camera, roller;
} CAMERA;

typedef struct {
        BITMAP *close, *medium, *far;
        } TEXTURE;

TEXTURE * floor1, * floor2, * floor3, * floor4;
TEXTURE * wall1, * wall2;
TEXTURE * static1, *static2;
CAMERA cam;
                             
int check_coords(int x, int y)
{ if(x<0 || y<0 || x>=MAP_SIZE || y>=MAP_SIZE) return 0;
  else return map[x][y];
}

TEXTURE * load_texture(string s)
{   PALETTE pal;
    TEXTURE *text;
    string fn,cfn;
    
    text = new (TEXTURE);
    fn.assign("data/images/");
    fn.append(s);
    cfn.assign(fn);
    cfn.append("_close.bmp");
    text->close = load_bmp(cfn.c_str(), pal);
    if(!text->close) { debug("Missing texture"); debug(s.c_str()); debug("close\n"); }
    cfn.assign(fn);
    cfn.append("_medium.bmp");
    text->medium = load_bmp(cfn.c_str(), pal);
    if(!text->medium)
    { 	debug("Missing texture"); debug(s.c_str()); debug("medium");
    	if(text->close)
    	{ debug("resizing close texture\n");
    	  text->medium = create_bitmap(128,128);
    	  stretch_blit(text->close,text->medium,0,0,text->close->w,text->close->h,0,0,text->medium->w,text->medium->h);
    	}
    	else debug("\n");
    }
    cfn.assign(fn);
    cfn.append("_far.bmp");
    text->far = load_bmp(cfn.c_str(), pal);
    if(!text->far)
    { 	debug("Missing texture"); debug(s.c_str()); debug("far");
		if(text->medium)
    	{ debug("resizing medium texture\n");
    	  text->far = create_bitmap(64,64);
    	  stretch_blit(text->medium,text->far,0,0,text->medium->w,text->medium->h,0,0,text->far->w,text->far->h);
    	}
    	else debug("\n");
    }
    return text;
}


int load_graphics()
{   int i,j,ii,fi,a,b;
    string s;
    PALETTE pal;
    
    floor1 = load_texture("floor/floor1");
    floor2 = load_texture("floor/floor2");
    floor3 = load_texture("floor/floor3");
    floor4 = load_texture("floor/floor4");
    wall1 = load_texture("wall/wall1");
    wall2 = load_texture("wall/wall2");
	static1 = load_texture("static/tree");
	static2 = load_texture("static/door");
    
    debug("Done loading tiles!\n");
      
      s="data/images/sky1/skylarge1.bmp";
      sky1[0] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing skylarge1 texture!\n");
      s="data/images/sky1/sky1.bmp";
      sky1[1] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky1 texture!\n");
      s="data/images/sky1/sky2.bmp";
      sky1[2] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky2 texture!\n");
      s="data/images/sky1/sky3.bmp";
      sky1[3] = load_bmp(s.c_str(), pal);
      if(!sky1[0]) debug("Missing sky3 texture!\n");
      s="data/images/api/api_mask.bmp";
      api = load_bmp(s.c_str(), pal);
      if(!api) debug("Missing api_mask texture!\n");
debug("done load graphics\n",10);
}

void unload_graphics()
{ delete [] (void **) floor1;
//  delete [] (void **) floor2;
//  delete [] (void *) fwall1;
  delete [] (void **) wall1;
}



#define GRID_SIZE 2
#define M_PI 3.14159

#define TILE_FLOOR 1
#define TILE_FRONT 2
#define TILE_SIDE 3
#define TILE_STATIC 4
#define TILE_CEILING 5
#define TILE_STATIC_NS 6
#define TILE_STATIC_EW 7

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
   if(cam->xfront==0) cam->xpos+=0.5;
   	  else cam->xpos+=(cam->xfront>0)?cam->stepback:1-cam->stepback;
   if(cam->zfront==0) cam->ypos+=0.5;  //ano zfront a ypos tu maj byt takhle
      else cam->ypos+=(cam->zfront>0)?cam->stepback:1-cam->stepback;
   cam->yfront=0;

   /* rotate the up vector around the in-front vector by the roll angle */
   get_vector_rotation_matrix_f(&cam->roller, cam->xfront, cam->yfront, cam->zfront,
				cam->roll*128.0/M_PI);
   apply_matrix_f(&cam->roller, 0, 1, 0, &cam->xup, &cam->yup, &cam->zup);

   /* build the camera matrix */
   get_camera_matrix_f(&cam->camera,
		       cam->xpos, cam->zpos, cam->ypos,        /* camera position */
		       cam->xfront, cam->yfront, cam->zfront,  /* in-front vector */
		       cam->xup, cam->yup, cam->zup,           /* up vector */
		       cam->fov,                               /* field of view */
		       cam->aspect);                           /* aspect ratio */
}

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
   { if(cam->ypos > z) dif=1; else dif=0;
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

void render_tile(int type, BITMAP *bmp, BITMAP * text, int w, int h, CAMERA * cam, int x, int z)
{  V3D_f _v[4], _vout[8], _vtmp[8];
   V3D_f *v[4], *vout[8], *vtmp[8];
   int flags[4], out[8];
   int c, vc;

   for (c=0; c<4; c++)
      v[c] = &_v[c];

   for (c=0; c<8; c++) {
      vout[c] = &_vout[c];
      vtmp[c] = &_vtmp[c];
   }
   
   make_static_tile(v,x,z,w,h,cam);

   switch(type)
   { case TILE_FLOOR: make_floor_tile(v,x,z,w,h);
   	   	   	 debug("  floor tile");
   	   	   	 break;
     case TILE_STATIC: make_static_tile(v,x,z,w,h,cam);
     	 	 debug("  static tile");
     	 	 break;
     case TILE_FRONT: make_front_tile(v,x,z,w,h,cam);
     	 	 debug("  front tile");
     	 	 break;
     case TILE_SIDE: make_side_tile(v,x,z,w,h,cam);
     	 	 debug("  side tile");
     	 	 break;
     case TILE_STATIC_NS: make_static_ns_tile(v,x,z,w,h);
          	 	 debug("  side tile");
          	 	 break;
     case TILE_CEILING: make_ceiling_tile(v,x,z,w,h);
     	 	 debug("  ceiling tile");
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
      /* clip if any vertices are off the edge of the screen */
      vc = clip3d_f(POLYTYPE_PTEX_MASK, 0.1, 0.1, 4, (AL_CONST V3D_f **)v,
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

BITMAP * far_texture(TEXTURE * txt, int dist)
{	switch(dist)
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
  int dist = ((x-(int)cam->xpos)*(x-(int)cam->xpos))+((z-(int)cam->ypos)*(z-(int)cam->ypos));

  if((x+z)%2) ct_wall=wall1;
  else ct_wall = wall2;
  ct_static=static1;
  switch(x%2+(z%2)*2)
  { case 0: ct_floor=floor1; break;
    case 1: ct_floor=floor2; break;
    case 2: ct_floor=floor3; break;
    case 3: ct_floor=floor4; break;
  }
  if(map[x][z]==5) ct_static=static2;
  else ct_static=static1;

  wf=64; hf=64; far=2;
  if(dist<9) { wf=128; hf=128; far=1;}
  if(dist<4) { wf=256; hf=256; far=0;}

  switch(map[x][z])
  { case 1: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z); break;
    case 2: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
            render_tile(TILE_STATIC, bmp, far_texture(ct_static,far), wf, hf, cam, x, z); break;
    case 3: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
            render_tile(TILE_SIDE, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
            render_tile(TILE_FRONT, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z); break;
    case 4: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
    		render_tile(TILE_CEILING, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
            break;
    case 5: render_tile(TILE_FLOOR, bmp, far_texture(ct_floor,far), wf, hf, cam, x, z);
        	render_tile(TILE_CEILING, bmp, far_texture(ct_wall,far), wf, hf, cam, x, z);
        	render_tile(TILE_STATIC_NS, bmp, far_texture(ct_static,far), wf, hf, cam, x, z); break;
            break;  
  }
}

void draw_view(int xpos, int ypos, int heading)
{  int c,x, y, w, h, dx, dy;
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
   cam.zpos = 0.5;
   cam.xpos=xpos;
   cam.ypos=ypos;
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

   
  int fx, fy;
 for(dx=11; dx>=0; dx--)
     for(dy=dx+1; dy>=0; dy--)
     { fx=xpos+dx*cam.xfront+dy*cam.zfront;  // ano zfront, y je nahoru
       fy=ypos+dy*cam.xfront+dx*cam.zfront;
       if(check_coords(fx,fy))
         draw_tile(game_bmp, &cam, fx, fy);
       fx=xpos+dx*cam.xfront-dy*cam.zfront;  // ano zfront, y je nahoru
       fy=ypos-dy*cam.xfront+dx*cam.zfront;
       if(check_coords(fx,fy))
         draw_tile(game_bmp, &cam, fx, fy);
     }

    /* overlay some text */
   BITMAP *bmp = game_bmp;  
   set_clip_rect(bmp, 0, 0, bmp->w, bmp->h);
   textprintf_ex(bmp, font, 0,  0, makecol(0, 0, 0), -1,
		 "Viewport width: %d (w/W changes)", viewport_w);
   textprintf_ex(bmp, font, 0,  32, makecol(0, 0, 0), -1,
		 "Viewport height: %d (h/H changes)", viewport_h);
   textprintf_ex(bmp, font, 0, 40, makecol(0, 0, 0), -1,
		 "Field of view: %f (h/j changes)", cam.fov);
   textprintf_ex(bmp, font, 0, 48, makecol(0, 0, 0), -1,
		 "Aspect ratio: %.2f (a/A changes)", cam.aspect);
   textprintf_ex(bmp, font, 0, 56, makecol(0, 0, 0), -1,
		 "X position: %.2f (x/X changes)", (float)cam.xpos);
   textprintf_ex(bmp, font, 0, 64, makecol(0, 0, 0), -1,
		 "Y position: %.2f (y/Y changes)", (float)cam.ypos);
   textprintf_ex(bmp, font, 0, 72, makecol(0, 0, 0), -1,
		 "Z position: %.2f (z/Z changes)", (float)cam.zpos);
   textprintf_ex(bmp, font, 0, 80, makecol(0, 0, 0), -1,
		 "Heading: %d 0 = EAST(left/right changes)", cam.heading);
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
