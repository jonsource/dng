/*
 * texture.h
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "game.h"

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

int add_texture(TEXTURE * txt);
TEXTURE * load_texture(string s);
int add_element(TEXTURED_ELEMENT * ele);
TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator);
int add_animator(ANIMATOR * txt);
ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h);
int add_tile(TILE * til);
TILE * create_tile();
int tile_add_element(TILE * til,string type,int element);

unsigned short int tile_type_resolve(string type);


#endif /* TEXTURE_H_ */
