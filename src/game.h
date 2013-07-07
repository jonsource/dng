#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <allegro.h>
#include <string>
#include "list.h"
//#include "game_lib.h"
#include "texture.h"
#include "textured_element.h"
#include "mobile.h"
#include "interface.h"
#include "graphic.h"

#define DEAD     0
#define CREATE   1
#define PLAY     2
#define PAUSE    -1

class PCSLOT;
class ITEM;

class GAME
{   public:
        List<TEXTURE> Graphics;
        List<TEXTURE> Textures;
        List<TEXTURED_ELEMENT> Elements;
        List<ANIMATOR> Animators;
        List<TILE> Tiles;
        List<LIGHT_SOURCE> Lightsources;
        List<TRIGGER> Triggers;
        List<MOBILE> Mobiles;
        List<MOBILE_TEMPLATE> MobileTemplates;
        ITEM * ActiveItem;
        CLICKABLE_MAP Clickables;
        std::string clickable_level;
        VIEW_SETTINGS view_settings;
        RGB * fade_color;
        int * Impassable;
        int transparent;
        int map_size;
        int **game_map;
        int **linesight;
        int light_power;
        int info;
        int time;
        int show_inventory;
        std::string map_name;
        std::string area_name;
        PCSLOT * PcSlots;
        GAME();
        ~GAME();
        int GetDebugLvl();
        int GetDebugLvlMain();
        int SetDebugLvl(int dbg_lvl);
        int SetDebugLvlMain(int dbg_lvl);
        int ResetDebugLvl();
        int DestroyMobile(int i);

};

void game_load();
int game_save();
void game_unload();
void game_draw();
void game_turn();
void keypress(int);
void pick(int);
void next_level();
void restart();
void mouse_click(int mw, int mh);
void player_move(int x, int y, int z, int h);
void player_move_subr(int x, int y, int z, int h, bool force);
void text_interpret(std::string s);
int can_leave(int x, int z, int dir);
int can_enter(int x, int z, int dir);
int is_passable(int x, int z);

extern GAME * Game;
extern int tmsec;

#endif
