/*
 * texture.h
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "game.h"

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

int add_texture(TEXTURE * txt);
TEXTURE * load_texture(string s);
int add_element(TEXTURED_ELEMENT * ele);
TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator);
int add_animator(ANIMATOR * txt);
ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h);

#endif /* TEXTURE_H_ */
