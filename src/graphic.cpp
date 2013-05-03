#include "graphic.h"
#include <math.h>
#include "edittext.h"
#include "game_lib.h"
#include "game.h"
#include "mobile.h"

BITMAP * sky1[4];
BITMAP * api=NULL;
BITMAP * CURSOR;
RGB pal[256];
extern BITMAP *game_bmp,* first,* second;
extern int fps, tmsec;

bool blender_set;
CAMERA * cam=NULL;
//RGB * fade_color;

int init_graphic()
{	int depth = 32;

    Game->fade_color = new RGB;
    Game->fade_color->r=255;
    Game->fade_color->g=0;
    Game->fade_color->b=0;
	set_color_depth(depth);
	if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0)
	{ 	debug("Select video mode failed!",10);
		return 0;
	}
	/* set up the viewport region */
	int viewport_w = 640;
	int viewport_h = 320;
	int x = 0;
	int y=20;
	set_projection_viewport(x-1, y-1, viewport_w+1, viewport_h+1);
	set_alpha_blender();
	set_trans_blender(0,0,0,128);
	first=create_bitmap(SCREEN_W, SCREEN_H);
	second=create_bitmap(SCREEN_W, SCREEN_H);
	game_bmp = first;
	//rect(game_bmp, x, y, x+w-1, y+h-1, makecol(255, 0, 0));
	set_clip_rect(first, x, y, x+viewport_w, y+viewport_h);
	set_clip_rect(second, x, y, x+viewport_w, y+viewport_h);
	set_clip_state(first,0);
	set_clip_state(second,0);
	if(game_bmp==NULL)
	{  debug("Couldn't acquire screen!");
	   return 0;
	}
	debug("screen depth: "+to_str(bitmap_color_depth(game_bmp)));
	clear(game_bmp);
	init_camera(&Game->view_settings);
//    show_mouse(screen);
	return 1;
}

/**
 * square of distance of coordinates to camera
 */
float dist(float x,float z,CAMERA * cam)
{ return dist2(x,z,cam->dolly_xpos,cam->dolly_zpos);

}

/**
 * check whether coordinates are on map and return them
 */
int check_coords(int x, int y)
{ if(x<0 || y<0 || x>=Game->MAP_SIZE || y>=Game->MAP_SIZE) return 0;
  else return Game->game_map[x][y];
}

/**
 * check whether coordinates are on game_map
 */
int check_coords_subr(int x, int y)
{ if(x<0 || y<0 || x>=Game->MAP_SIZE || y>=Game->MAP_SIZE) return 0;
  else return 1;
}

/**
 * check whether we can see the coordinates,
 * valid only after make_linesight is called for current camera position
 */
int see_coords(int x, int y)
{   if(x<0 || y<0 || x>=Game->MAP_SIZE || y>=Game->MAP_SIZE) return 0;
    return Game->linesight[x][y];
}

/**
 * load graphics
 */
int load_graphics()
{   string s;
    PALETTE pal;

    CURSOR = load_bmp("data/images/api/cursor.bmp",pal);
      //set_mouse_sprite(load_bmp("data/images/api/cursor.bmp",pal));

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
{ //delete [] (void **) floor1;
//  delete [] (void **) floor2;
//  delete [] (void *) fwall1;
  //delete [] (void **) wall1;
}

/**
 * updates the camera matrix after moving
 * needs the variables heading, pitch and roll to be set
 */
void update_camera(CAMERA * cam)
{
     /* calculate the in-front vector */
/*   cam->xfront = sin(cam->heading) * cos(cam->pitch);
   cam->yfront = sin(cam->pitch);
   cam->zfront = cos(cam->heading) * cos(cam->pitch);*/
/*
   switch(cam->heading)
   { case HEAD_EAST : cam->xfront = 1; cam->zfront = 0; break;
     case HEAD_NORTH : cam->xfront = 0; cam->zfront = -1; break;
     case HEAD_WEST : cam->xfront = -1; cam->zfront = 0; break;
     case HEAD_SOUTH : cam->xfront = 0; cam->zfront = 1; break;

   }*/
   heading_to_xz(cam->heading,&cam->xfront,&cam->zfront);

   if(cam->xfront==0) cam->xpos=cam->dolly_xpos;
   	  else cam->xpos=cam->dolly_xpos+((cam->xfront>0)?cam->step_back:-cam->step_back);
   if(cam->zfront==0) cam->zpos=cam->dolly_zpos;  //ano zfront a ypos tu maj byt takhle
      else cam->zpos=cam->dolly_zpos+((cam->zfront>0)?cam->step_back:-cam->step_back);
   cam->ypos=cam->dolly_ypos+cam->view_height;
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

/**
 * render a single element with type
 */
void render_element(int type, TEXTURED_ELEMENT * element, BITMAP *bmp, int x, int z, CAMERA * cam,int far)
{  V3D_f _v[4], _vout[8], _vtmp[8];
   V3D_f *v[4], *vout[8], *vtmp[8];
   int flags[4], out[8];
   int c, vc;
   int polytype;

   /* set necessary POLYTYPE, affine texture mapping is OK and faster than perspective correct mapping */
   if(Game->TRANSPARENT && element->transparent)
   {	polytype = POLYTYPE_ATEX_MASK_TRANS;
   		set_alpha_blender();
   		//blender_set=false;
   }
   else
   {	switch(type)
   	   	{	case TILE_FLOOR:
   	   		case TILE_CEILING:
   	   		case TILE_SIDE: polytype = POLYTYPE_PTEX_LIT; break;
   	   		case TILE_FRONT:
   	   		case TILE_STATIC: polytype = POLYTYPE_ATEX_MASK_LIT; break; //no need to account for perspective with front tiles
   	   		default: polytype = POLYTYPE_PTEX_MASK_LIT; break;
   	   	}
   		/*if(!blender_set)
        {   set_trans_blender(fade_color.r,fade_color.g,fade_color.b,1);
            blender_set=true;
        }*/
        set_trans_blender(Game->fade_color->r,Game->fade_color->g,Game->fade_color->b,1);
   }

   for (c=0; c<4; c++)
	   v[c] = &_v[c];

   for (c=0; c<8; c++) {
      vout[c] = &_vout[c];
      vtmp[c] = &_vtmp[c];
   }

   /* create vertices of rendered element */
   switch(type)
   {   case TILE_FLOOR: make_floor_element(v,element,x,z,cam,far); break;
   	   case TILE_CEILING: make_ceiling_element(v,element,x,z,cam,far); break;
   	   case TILE_FLAT: make_flat_element(v,element,x,z,cam,far); break;
   	   case TILE_FRONT: make_front_element(v,element,x,z,cam,far); break;
   	   case TILE_SIDE: make_side_element(v,element,x,z,cam,far); break;
   	   case TILE_STATIC: make_static_element(v,element,x,z,cam,far); break;
   	   case TILE_STATIC_NS: make_static_element_ns(v,element,x,z,cam,far); break;
   	   case TILE_STATIC_EW: make_static_element_ew(v,element,x,z,cam,far); break;
   	   case TILE_STATIC_NS_X: if(cam->xfront==0) { make_static_element_ns(v,element,x,z,cam,far); break; }
                              else return;
   	   case TILE_STATIC_EW_X: if(cam->zfront==0) { make_static_element_ew(v,element,x,z,cam,far); break; }
                              else return;
   	   default: debug("Missing or bad tile type in render element!",3); return;
   }

   /* apply lighting to vertices, used to render lit textures */
   int ls;
   for (c=0; c<4; c++)
   {  	/* apply players light */
        ls=Game->light_power-sqrt(dist2(v[c]->x,v[c]->z,cam->dolly_xpos,cam->dolly_zpos))*30;
   	    v[c]->c=ls>0?ls:0;
	    //v[c]->c=0;
	    /* apply lightsources */
   	    for(int i=0; i<Game->Lightsources.len(); i++)
   	    { ls=Game->Lightsources[i]->power-sqrt(dist2(v[c]->x,v[c]->z,Game->Lightsources[i]->x,Game->Lightsources[i]->z))*Game->Lightsources[i]->dim;
   	      //if(i==0) debug("lightsource :"+to_str(Lightsources[i]->x)+" "+to_str(Lightsources[i]->z)+", v :"+to_str(v[c]->x)+" "+to_str(v[c]->z)+" "+to_str(sqrt(dist2(v[c]->x,v[c]->z,Lightsources[i]->x,Lightsources[i]->z)))+" ");
   	      v[c]->c+=ls>0?ls:0;
   	    }
        if(v[c]->c<0) v[c]->c=0;
        if(v[c]->c>255) v[c]->c=255;
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

      if (v[c]->z < -cam->step_back+0.05)
	 flags[c] |= 16;
   }
   /* quit if all vertices are off the same edge of the screen */
   if (element->clip && (flags[0] & flags[1] & flags[2] & flags[3]))
      return;

   /* no clipping elements are clipped to fit in the viewport */
   if (!element->clip || (flags[0] | flags[1] | flags[2] | flags[3])) {
      /* clip if any vertices are off the edge of the screen */
      vc = clip3d_f(polytype, -cam->step_back-0.05, 0, 4, (AL_CONST V3D_f **)v,
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

/**
 * get appropriate texture for given distance and animation
 */
BITMAP * far_texture(TEXTURED_ELEMENT * txt, int far)
{	/* animate texture */

    if(txt->animator!=NULL)
	{	/* apply MOVATORS */

        int t=tmsec;
        ANIMATOR * a=txt->animator;
        if(a->type==MOVATOR_Y)
        {   /* update state, a->on 1 means it is moving (from position),
               a->offset 0 means start position, a->offset 1 means end position */

            float ydif=get_movator_dif(a,t);
            //if(a->on) debug("MOVATOR "+to_str(a->on)+" "+to_str(ydif),3);
            clear_to_color(a->frame,makecol(255,0,255));
            blit(txt->texture->close,a->frame,0,0,0,-(int)(ydif*a->frame->h),a->frame->w,a->frame->h);
            return a->frame;
        }
        else
	    {

	        int frame_no=0;
            if(a->type==ANIMATOR_MOBILE)
            {   return a->frame;
            }
            if(a->type==ANIMATOR_TOGGLE)
            {   //if(a->on) debug("toggle animator "+to_str(a->start)+" "+to_str(a->speed)+" "+to_str(a->frames)+" "+to_str(a->start+a->speed*a->frames)+">"+to_str(tmsec));
                if(a->on && tmsec>=(a->start+a->speed*a->frames) )
                { a->on=0;
                }
            }
            if(a->on)
            {   frame_no=(((tmsec-a->start)/a->speed)+1)%a->frames;
                //debug("  frame no:"+to_str(frame_no));
            }
            blit(txt->texture->close,a->frame,frame_no*a->offset,0,0,0,a->frame->w,a->frame->h);
            return a->frame;
	    }

    }
    switch(far)
	{ case 0: return txt->texture->close;
	  case 1: return txt->texture->medium;
	  case 2: return txt->texture->far;
	}
	return txt->texture->close;
}

void render_tile_floor(TILE * tile,BITMAP * bmp, int x, int z, CAMERA * cam)
{   int d = dist(x,z,cam);

	int far=2;
	if(d<9) { far=1;}
	if(d<4) { far=0;}

	/** TODO
	 *  for SIDE and FRONT check if neighboring tile is seen, if not, don't draw
	 */

    /* old way of rendering - without sorting - used for walls ets.. */
   /* int upper=tile->elements_len;
    if (upper>1) upper = 1;
	for(int i=0; i<upper; i++)
	{	render_element(tile->element_types[i],tile->elements[i],bmp,x,z,cam,far);
	}*/
	if(tile->element_types[0]==TILE_FLOOR)
        render_element(tile->element_types[0],tile->elements[0],bmp,x,z,cam,far);
}

/**
 * render a tile of map, i.e. render all of its elements as necessary
 */
void render_tile(TILE * tile,BITMAP * bmp, int x, int z, CAMERA * cam)
{	//debug("render_tile "+to_str(x)+" "+to_str(z),1);
	int d = dist(x,z,cam);

	int far=2;
	if(d<9) { far=1;}
	if(d<4) { far=0;}

	/** TODO
	 *  for SIDE and FRONT check if neighboring tile is seen, if not, don't draw
	 */

    /* old way of rendering - without sorting - used for walls ets.. */
	for(int i=0; i<tile->elements_len; i++)
	{	if(tile->element_types[i]!=TILE_FLOOR) render_element(tile->element_types[i],tile->elements[i],bmp,x,z,cam,far);
	}

    TEXTURED_ELEMENT * mobile = NULL;
    /* prepare mobile - if present, only one mobile per square allowed */
	for(int i=0; i<Game->Mobiles.len(); i++)
    {   MOBILE * mob = Game->Mobiles[i];
        if(floor(mob->x) == x && floor(mob->z) == z)
        {   clear_to_color(mob->ani->frame,makecol(255,0,255));
            if(mob->sprite==NULL)
            {
                debug("Missing sprite - mobile :"+to_str(i));
                exit(1);
            }
            int mod=cam->heading-mob->heading+2;
            if(mod<0) mod+=4;
            mod=mod%4;
            if(cam->heading==0 || cam->heading==3)
            {
                if(mod==3) mod=1;
                else if(mod==1) mod=3;
            }



            int fr = (mob->act_progress/500)%4;
            int width = 72;
            if(mod==2) width=64;

            if(mob->action==ACT_FIGHT)
            {   mod=4;
                fr = (mob->act_progress/500)%6;
                if(fr<3)
                {
                    mod=0;
                    fr=0;
                }
                else fr=fr-3;
                width = 72;
            }
            blit(mob->sprite->sprite, mob->ani->frame,width*fr,72*mod,0,53,width,75);
            blit(mob->sprite->sprite, mob->ani->frame,width*fr,72*mod,128-width,53,width,75);
            //render_element(TILE_STATIC,mob->ele,bmp,x,z,cam,far);
            mobile = mob->ele;
            break; // no need to search further - only one mobile per square
        }
    }

    /* sorting static elements + mobile if present */


    int stat_len = tile->statics_len;
    if(mobile != NULL ) stat_len++;
    IND_VAL * sort_array = new IND_VAL[stat_len]; // allocate enough space for statics and a mobile - if present
    for(int i=0; i<tile->statics_len; i++) // yes, use tile->statics_len, we're filling in the statics only
    {   sort_array[i].ele=tile->statics[i];
        sort_array[i].ind=i; //debuging use only
        sort_array[i].ele_t= tile->static_types[i];
        sort_array[i].value=dist2(cam->dolly_xpos,cam->dolly_zpos,x+tile->statics[i]->x,z+tile->statics[i]->z);
    }
    // add mobile to the end of the list
    if(mobile != NULL )
    {   sort_array[stat_len-1].ele = mobile;
        sort_array[stat_len-1].ind = -10; //debuging use only
        sort_array[stat_len-1].ele_t = mobile->type;
        sort_array[stat_len-1].value=dist2(cam->dolly_xpos,cam->dolly_zpos,x+mobile->x,z+mobile->x);
    }


    qsort(sort_array,stat_len,sizeof(IND_VAL),compare_ind_val);

    for(int i=0; i<stat_len; i++)
	{
        //dappend(" "+to_str((int)*sort_array[i].ele_t),5);
	    render_element((int)sort_array[i].ele_t,sort_array[i].ele,bmp,x,z,cam,far);
	}
	delete [] sort_array;
}

/**
 * subroutine of make_linesight, check whether the given tile can be seen and recursively add it's neighbors if true
 */

void see_tile(int x, int z, CAMERA * cam);

void see_tile_subr(int x, int z, CAMERA * cam, int recourse)
{	//debug("  see tile "+to_str(x)+" "+to_str(z),1);
	int s=check_coords(x,z);
	if(s)
	{ if(Game->linesight[x][z])
	  { //dappend(" already seen",1);
		return;
	  }
	  Game->linesight[x][z]=s;
	  if(!recourse) return;
	  for(int i=0;i<Game->Tiles[s-1]->elements_len;i++)
		  if(Game->Tiles[s-1]->element_types[i]==TILE_FRONT) { return; }

	  if(!see_coords(x+cam->xfront,z+cam->zfront)) see_tile(x+cam->xfront,z+cam->zfront,cam);
	  if(cam->xfront==0)
	  { if(x>cam->dolly_xpos-1 && !see_coords(x+1,z+cam->zfront)) see_tile(x+1,z+cam->zfront,cam);
	    if(x<cam->dolly_xpos && !see_coords(x-1,z+cam->zfront)) see_tile(x-1,z+cam->zfront,cam);
	  }
	  else
	  { if(z>cam->dolly_zpos-1 && !see_coords(x+cam->xfront,z+1)) see_tile(x+cam->xfront,z+1,cam);
	  	if(z<cam->dolly_zpos && !see_coords(x+cam->xfront,z-1)) see_tile(x+cam->xfront,z-1,cam);
	  }
	}
	//dappend("  done "+to_str(x)+" "+to_str(z));
}

void see_tile(int x, int z, CAMERA * cam)
{   see_tile_subr(x,z,cam,1);
}

/**
 * recursively count what tiles we  can see from current position
 */
void make_linesight(int x, int z, CAMERA * cam)
{	//debug(" linesight "+to_str(x)+" "+to_str(z),1);

	for(int i=0; i<Game->MAP_SIZE; i++)
		for(int j=0; j<Game->MAP_SIZE; j++)
			Game->linesight[i][j]=0;

	//debug(" * after sight reset",1);
	// three initial tiles
	see_tile(x,z,cam);
	if(cam->xfront==0)
	{ 	see_tile_subr(x-1,z,cam,0);
		see_tile_subr(x+1,z,cam,0);
    }
	else
	{ 	see_tile_subr(x,z+1,cam,0);
		see_tile_subr(x,z-1,cam,0);
	}
}

/**
 * init camera, or reset the values afteer changes to FOV, aspoct and stepback
 */
void init_camera(VIEW_SETTINGS * view_settings)
{	if(cam==NULL) cam=new CAMERA;
	if(cam==NULL)
	{	debug("Couldn't init camera",10);
		exit(1);
	}
	cam->pitch=0;
	cam->roll=0;
	cam->fov=view_settings->fov;
	cam->step_back=view_settings->step_back;
	cam->aspect = view_settings->aspect;
	cam->view_height = view_settings->view_height;
	debug("Camera init",10);
}

/**
 * draw the current view of the map
 */
void draw_view(int xpos, int ypos, int zpos, int heading)
{  int dz,dx,c;


   cam->dolly_ypos=ypos+0.5;
   cam->dolly_xpos=xpos+0.5;
   cam->dolly_zpos=zpos+0.5;
   debug("begin draw_view()",2);

   //blit(api, game_bmp, 0, 0, 0, 0, 640, 480);

   c=(tmsec/500)%2560;

   switch(heading)
   { case 0: blit(sky1[0], game_bmp, 0+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 1: blit(sky1[0], game_bmp, 640+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 2: blit(sky1[0], game_bmp, 1280+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
     case 3: blit(sky1[0], game_bmp, 1920+c, 145, 0, 20, sky1[0]->w, sky1[0]->h); break;
   }


   cam->heading=heading;
   update_camera(cam);

   make_linesight(xpos,zpos,cam);
   //debug(" * in draw_view after linesight",1);
    int fx, fz;
    int zfront = cam->zfront;
    int xfront = cam->xfront;
    /* render floors and ceilings */
    for(dx=MAX_VIEW_DIST; dx>=0; dx--)
    {   for(dz=dx+1; dz>=0; dz--)
        {   fx=xpos+dx*xfront+dz*zfront;  // ano zfront, y je nahoru
            fz=zpos+dz*xfront+dx*zfront;
            if(see_coords(fx,fz))
                render_tile_floor(Game->Tiles[Game->game_map[fx][fz]-1],game_bmp, fx, fz, cam);
            fx=xpos+dx*xfront-dz*zfront;  // ano zfront, y je nahoru
            fz=zpos-dz*xfront+dx*zfront;
            if(see_coords(fx,fz))
                render_tile_floor(Game->Tiles[Game->game_map[fx][fz]-1],game_bmp, fx, fz, cam);
        }
    }

    /* render the rest */
    for(dx=MAX_VIEW_DIST; dx>=0; dx--)
    {   for(dz=dx+1; dz>=0; dz--)
        {   fx=xpos+dx*xfront+dz*zfront;  // ano zfront, y je nahoru
            fz=zpos+dz*xfront+dx*zfront;
            if(see_coords(fx,fz))
                render_tile(Game->Tiles[Game->game_map[fx][fz]-1],game_bmp, fx, fz, cam);
            fx=xpos+dx*xfront-dz*zfront;  // ano zfront, y je nahoru
            fz=zpos-dz*xfront+dx*zfront;
            if(see_coords(fx,fz))
                render_tile(Game->Tiles[Game->game_map[fx][fz]-1],game_bmp, fx, fz, cam);
        }
    }

   if(Game->INFO>1)
   {
    /* overlay some text */
       set_clip_rect(game_bmp, 0, 0, game_bmp->w, game_bmp->h);
      // textprintf_ex(bmp, font, 0,  0, makecol(0, 0, 0), -1,
    //		 "Viewport width: %d  height: %d", viewport_w,viewport_h);
       //textprintf_ex(bmp, font, 0,  32, makecol(0, 0, 0), -1,
             //"Viewport height: %d (h/H changes)", viewport_h);
       textprintf_ex(game_bmp, font, 0, 40, makecol(0, 0, 0), -1,
             "Field of view: %f  Aspect: %.2f Light: %d", cam->fov,cam->aspect,Game->light_power);
       //textprintf_ex(bmp, font, 0, 48, makecol(0, 0, 0), -1,
             //"Aspect ratio: %.2f (a/A changes)", cam.aspect);
       textprintf_ex(game_bmp, font, 0, 56, makecol(0, 0, 0), -1,
             "Position X: %.2f(%.2f) Y: %.2f(%.2f) Z: %.2f(%.2f)", (float)cam->xpos,cam->dolly_xpos,(float)cam->ypos,cam->dolly_ypos,(float)cam->zpos,cam->dolly_zpos);
       //textprintf_ex(bmp, font, 0, 64, makecol(0, 0, 0), -1,
        //	 "Y position: %.2f (y/Y changes)", (float)cam.ypos);
       //textprintf_ex(bmp, font, 0, 72, makecol(0, 0, 0), -1,
        //	 "Z position: %.2f (z/Z changes)", (float)cam.zpos);
       textprintf_ex(game_bmp, font, 0, 80, makecol(0, 0, 0), -1,
             "Heading: %d %s Stepback: %.2f", cam->heading,heading_to_str(cam->heading).c_str(),cam->step_back);
       textprintf_ex(game_bmp, font, 0, 88, makecol(0, 0, 0), -1,
             "Pitch: %.2f deg (pgup/pgdn changes)", cam->pitch);
       textprintf_ex(game_bmp, font, 0, 96, makecol(0, 0, 0), -1,
             "Roll: %.2f deg (r/R changes)", cam->roll);
       textprintf_ex(game_bmp, font, 0, 104, makecol(0, 0, 0), -1,
             "Front vector: %d, %d, %d", cam->xfront, cam->yfront, cam->zfront);
       textprintf_ex(game_bmp, font, 0, 112, makecol(0, 0, 0), -1,
             "Up vector: %.2f, %.2f, %.2f", cam->xup, cam->yup, cam->zup);
       textprintf_ex(game_bmp, font, 0, 120, makecol(0, 0, 0), -1,
             "Frames per second: %d", fps);

        int see=makecol(128,128,128);
        int not_see=makecol(72,72,72);
        for(int i=0; i<Game->MAP_SIZE; i++)
        {   for(int j=0; j<Game->MAP_SIZE; j++)
            {   if(Game->linesight[i][j]==0) putpixel(game_bmp,20+i,330+j,see);
                else putpixel(game_bmp,20+i,330+j,not_see);


            }
        }
    }

    if(Game->INFO==1) text_output(game_bmp);

    masked_blit(CURSOR,game_bmp,0,0,mouse_x,mouse_y,CURSOR->w,CURSOR->h);
}

void draw_triggers(int x, int z, int heagind)
{   CLICKABLE* c;
    List<CLICKABLE> clklist;
    clklist=Game->Clickables["place"];
    for(int i=0; i<clklist.len(); i++)
    {   //debug("["+to_str(x)+","+to_str(z)+"] Trigger "+to_str(Triggers[i]->xpos)+" "+to_str(Triggers[i]->zpos),4);
        c=clklist[i];
            //debug("Trigger encountered. "+to_str(t->type));
        rect(game_bmp,c->w1,c->h1,c->w2,c->h2,makecol(255,0,0));
        textprintf_ex(game_bmp, font, 0, 140, makecol(255, 0, 0), -1,
                    "Trigger!");


    }

}
