#ifndef __GRAPHIC_H_
#define __GRAPHIC_H_
#include "game.h"

#define GRID_SIZE 2
#define M_PI 3.14159

#define TILE_FLOOR 1
#define TILE_FRONT 2
#define TILE_SIDE 3
#define TILE_STATIC 4
#define TILE_CEILING 5
#define TILE_STATIC_NS 6
#define TILE_STATIC_EW 7

typedef struct {
        float dolly_xpos, dolly_zpos, dolly_ypos;  /*cmera dolly position (camera position without stepback)*/
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

typedef struct {
  unsigned short int anim_speed,anim_offset,anim_frames;
  TEXTURE *frame;
} ANIMATOR;

typedef struct {
		float w,h,x,y,z;
		int type;
		TEXTURE * texture;
		ANIMATOR * animator;
} TEXTURED_ELEMENT;

int load_graphics();
void unload_graphics();
void draw_view(int x, int y, int heading);
int count_view_x(int x, int i, int j, int h);
int count_view_y(int y, int i, int j, int h);
BITMAP * far_texture(TEXTURE * txt, int dist);

void make_floor_tile(V3D_f **v,int x, int z, int w, int h);
void make_ceiling_tile(V3D_f **v,int x, int z, int w, int h);
void make_static_tile(V3D_f **v,int x, int z, int w, int h, CAMERA * cam);
void make_static_ns_tile(V3D_f **v,int x, int z, int w, int h);
void make_static_ew_tile(V3D_f **v,int x, int z, int w, int h);
void make_front_tile(V3D_f **v,int x, int z, int w, int h, CAMERA * cam);
void make_side_tile(V3D_f **v,int x, int z, int w, int h, CAMERA * cam);
void make_static_element(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far);

#endif
