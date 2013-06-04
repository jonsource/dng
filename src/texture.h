/*
 * texture.h
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "allegro.h"
#include <string>

#define TRIGGER_NORTH 0
#define TRIGGER_EAST 1
#define TRIGGER_SOUTH 2
#define TRIGGER_WEST 3

#define TRIGGER_ENTER 5
#define TRIGGER_LEAVE 6

#define BLOCKER_TO_HEADING 8

#define BLOCKER_NORTH 8
#define BLOCKER_EAST 9
#define BLOCKER_SOUTH 10
#define BLOCKER_WEST 11
#define BLOCKER_ENTER 12
#define BLOCKER_LEAVE 13

#define ANIMATOR_TOGGLE 1
#define ANIMATOR_ON 2
#define ANIMATOR_OFF 3
#define MOVATOR_Y 4
#define ANIMATOR_MOBILE 5 // no need to resolve or save or load this type of animator

#define MAX_TILE_ELE 10

#define NO_FLIP 1
#define V_FLIP 2
#define H_FLIP 4
#define VH_FLIP 6
#define TURN_90 32
#define TURN_270 34
#define V_FLIPPING 8
#define H_FLIPPING 16
#define VH_FLIPPING 24
#define V_FLIPPING_90 40
#define H_FLIPPING_90 48
#define VH_FLIPPING_90 56

#define TILE_FLOOR 1
#define TILE_CEILING 2
#define TILE_FLAT 3
#define TILE_FRONT 4
#define TILE_SIDE 5
#define TILE_STATIC 6
#define TILE_STATIC_NS 7
#define TILE_STATIC_EW 8
#define TILE_STATIC_NS_X 9
#define TILE_STATIC_EW_X 10

class TEXTURE;
class ANIMATOR;
class TEXTURED_ELEMENT;

class TEXTURE
{
    public:
        std::string name;
        BITMAP *close, *medium, *far;
        TEXTURE();
        std::string serialize();
};

class ANIMATOR
{   public:
        unsigned short int type, speed, offset, _offset, frames, on, mode;
        int start,w,h;
        BITMAP * frame;
        ANIMATOR();
        ANIMATOR(int type, int speed, int offset, int frames, int w, int h);
        ANIMATOR(int type, int speed, int offset, int frames, int w, int h, int on, int start);
        std::string serialize();
        std::string save_string();
        std::string type_string();
        static unsigned short int type_resolve(std::string s);
};

class TILE
{   public:
        int elements_len;
        int *element_nrs;
        int block_sight;
        TEXTURED_ELEMENT ** elements;
        unsigned short int * element_types;
        int *static_nrs;
        int statics_len;
        TEXTURED_ELEMENT ** statics;
        unsigned short int * static_types;
        TILE();
        int add_element(std::string type, int element);
        int add_element_subr(unsigned short int typ, int element);
        int add_static_subr(unsigned short int typ, int stat);
        std::string serialize();
        static std::string type_string(int i);
        static unsigned short int type_resolve(std::string s);
};

class LIGHT_SOURCE
{   public:
        int power,dim;
        float x,z;
        LIGHT_SOURCE(int power, int dim, float x, float z);
        std::string serialize();
};

typedef struct {
        double aspect, view_height, step_back;
        int fov;
} VIEW_SETTINGS;

typedef struct
{   TEXTURED_ELEMENT * ele;
    unsigned short int ele_t;
    int ind; //debuging use only
    float value;
} IND_VAL;

TEXTURE * load_texture(std::string s);
TEXTURED_ELEMENT * load_element(std::string s);
//TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator, string clip, string flip);
ANIMATOR * load_animator(std::string s);
int load_animator_save(ANIMATOR * animator, std::string s);
//ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h);
float get_movator_dif(ANIMATOR * a,int t);
TILE * load_tile(std::string s);
//TILE * create_tile();
int tile_add_element(TILE * til,std::string type,int element);
unsigned short int tile_type_resolve(std::string type);
unsigned short int flip_resolve(std::string type);
LIGHT_SOURCE * load_lightsource(std::string s);
//LIGHT_SOURCE * create_lightsource(int power, int dim, float x, float z);
std::string clip_to_str(bool clip);
std::string flip_to_str(int type);
std::string trans_to_str(bool trans);

#endif /* TEXTURE_H_ */
