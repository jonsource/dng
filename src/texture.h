/*
 * texture.h
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "allegro.h"
#include "list.h"
#include <map>
#include <string>

using namespace std;

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

#define ANIMATOR_TOGGLE 1
#define ANIMATOR_ON 2
#define ANIMATOR_OFF 3
#define MOVATOR_Y 4

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

#define STAND 0
#define WALK 1
#define TURN_LEFT 2
#define TURN_RIGHT 3
#define TURN_AROUND 4
#define ATTACK 5
#define DIE 6

class TEXTURE;

class MOBILE
{   public:
        int heading;
        float x,y,z;
        TEXTURE * sprite;
        int spr_w, spr_h;
        int speed;
        int mode;
        float progress;
        MOBILE();
};

class TEXTURE
{
    public:
        string name;
        BITMAP *close, *medium, *far;
        TEXTURE();
        string serialize();
};

class ANIMATOR
{   public:
        unsigned short int type, speed, offset, _offset, frames, on, mode;
        int start,w,h;
        BITMAP * frame;
        ANIMATOR(int type, int speed, int offset, int frames, int w, int h);
        ANIMATOR(int type, int speed, int offset, int frames, int w, int h, int on, int start);
        string serialize();
        string save_string();
        string type_string();
        static unsigned short int type_resolve(string s);
};

class TEXTURED_ELEMENT
{   public:
		float x,y,z,w,h;
		int type,flip;
		bool transparent,clip;
		int texture_nr;
		TEXTURE * texture;
		int animator_nr;
		ANIMATOR * animator;
		TEXTURED_ELEMENT(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator, string clip, string flip);
		string serialize();
		string type_string();
		static int type_resolve(string s);
};

class TILE
{   public:
        int len;
        int *element_nrs;
        TEXTURED_ELEMENT ** elements;
        unsigned short int * types;
        TILE();
        int add_element(string type, int element);
        string serialize();
        static string type_string(int i);
        static unsigned short int type_resolve(string s);
};

class LIGHT_SOURCE
{   public:
        int power,dim;
        float x,z;
        LIGHT_SOURCE(int power, int dim, float x, float z);
        string serialize();
};

class TRIGGER
{   public:
        int xpos;
        int zpos;
        int type;
        int w1,h1,w2,h2;
        int animator_nr;
        ANIMATOR * animator;
        TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, int animator);
        TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, int animator, string action);
        void fire();
        string serialize();
        string type_string();
        static int type_resolve(string type);
        string * action;
};

typedef struct
{   int w1,h1,w2,h2;
    TRIGGER * callback;
} CLICKABLE;

typedef map < string, List<CLICKABLE> > CLICKABLE_MAP;
typedef map < string, List<CLICKABLE> >::iterator CLICKABLE_MAP_ITERATOR;

typedef struct {
        double aspect, view_height, step_back;
        int fov;
} VIEW_SETTINGS;

class GAME
{   public:
        int DEBUG_LVL_MAIN, DEBUG_LVL;
        List<TEXTURE> Textures;
        List<TEXTURED_ELEMENT> Elements;
        List<ANIMATOR> Animators;
        List<TILE> Tiles;
        List<LIGHT_SOURCE> Lightsources;
        List<TRIGGER> Triggers;
        List<MOBILE> Mobiles;
        CLICKABLE_MAP Clickables;
        VIEW_SETTINGS view_settings;
        RGB * fade_color;
        int * Impassable;
        int TRANSPARENT;
        int MAP_SIZE;
        int **game_map;
        int **linesight;
        int light_power;
        int INFO;
        string map_name;
        GAME();
};

extern GAME * Game;

TEXTURE * load_texture(string s);
TEXTURED_ELEMENT * load_element(string s);
//TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator, string clip, string flip);
ANIMATOR * load_animator(string s);
int load_animator_save(ANIMATOR * animator, string s);
//ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h);
float get_movator_dif(ANIMATOR * a,int t);
TILE * load_tile(string s);
//TILE * create_tile();
int tile_add_element(TILE * til,string type,int element);
unsigned short int tile_type_resolve(string type);
unsigned short int flip_resolve(string type);
LIGHT_SOURCE * load_lightsource(string s);
//LIGHT_SOURCE * create_lightsource(int power, int dim, float x, float z);
TRIGGER * load_trigger(string s);
string clip_to_str(bool clip);
string flip_to_str(int type);
string trans_to_str(bool trans);

#endif /* TEXTURE_H_ */
