#ifndef __GRAPHIC_H_
#define __GRAPHIC_H_
#include "game.h"
#include "texture.h"

#define GRID_SIZE 2
#define M_PI 3.14159

#define MAX_VIEW_DIST 20

#define TILE_FLOOR 1
#define TILE_CEILING 2
#define TILE_FLAT 3
#define TILE_FRONT 4
#define TILE_SIDE 5
#define TILE_STATIC 6
#define TILE_STATIC_NS 7
#define TILE_STATIC_EW 8

#define ELEMENT_ROTATE 0
#define ELEMENT_NS 1
#define ELEMENT_EW 2

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

int init_graphic();
void init_camera(float stepback,float fov, float aspect);
int load_graphics();
void unload_graphics();
void draw_view(int x, int y,int z, int heading);
int count_view_x(int x, int i, int j, int h);
int count_view_y(int y, int i, int j, int h);
BITMAP * far_texture(TEXTURED_ELEMENT * txt, int dist);

void make_static_element_subr(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far, int type);
void make_static_element(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far);
void make_static_element_ns(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far);
void make_static_element_ew(V3D_f **v,TEXTURED_ELEMENT * element,int x, int z, CAMERA * cam, int far);
void make_floor_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far);
void make_ceiling_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far);
void make_flat_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far);
void make_flat_element_subr(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far, float height);
void make_front_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far);
void make_side_element(V3D_f **v, TEXTURED_ELEMENT * element, int x, int z, CAMERA * cam, int far);

#endif
