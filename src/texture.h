/*
 * texture.h
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "game.h"

#define TRIGGER_ENTER 5
#define TRIGGER_LEAVE 6
#define TRIGGER_NORTH 1
#define TRIGGER_EAST 0
#define TRIGGER_SOUTH 3
#define TRIGGER_WEST 2

#define MAX_TILE_ELE 6

typedef struct {
        BITMAP *close, *medium, *far;
} TEXTURE;

typedef struct {
  unsigned short int speed,offset,frames;
  BITMAP *frame;
} ANIMATOR;

typedef struct {
		float x,y,z,w,h;
		int type;
		bool transparent;
		TEXTURE * texture;
		ANIMATOR * animator;
} TEXTURED_ELEMENT;

typedef struct {
	int len;
	TEXTURED_ELEMENT ** elements;
	unsigned short int * types;
} TILE;

typedef struct
{	int x, z, power,dim;
} LIGHT_SOURCE;

class TRIGGER
{   public:
        int xpos;
        int zpos;
        int type;
        int w1,h1,w2,h2;
        TRIGGER(int xpos, int zpos, int type, int w1, int h1, int w2, int h2);
        void fire();
};

TEXTURE * load_texture(string s);
TEXTURED_ELEMENT * load_element(string s);
TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator);
ANIMATOR * load_animator(string s);
ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h);
TILE * load_tile(string s);
TILE * create_tile();
int tile_add_element(TILE * til,string type,int element);
unsigned short int tile_type_resolve(string type);
LIGHT_SOURCE * load_lightsource(string s);
LIGHT_SOURCE * create_lightsource(int power, int dim, int x, int z);
TRIGGER * load_trigger(string s);

#endif /* TEXTURE_H_ */