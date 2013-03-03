#ifndef __GRAPHIC_H_
#define __GRAPHIC_H_
#include "game.h"

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

#endif
