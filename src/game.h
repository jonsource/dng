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

using namespace std;

#define DEAD     0
#define CREATE   1
#define PLAY     2
#define PAUSE    -1

class GAME
{   public:
        List<TEXTURE> Textures;
        List<TEXTURED_ELEMENT> Elements;
        List<ANIMATOR> Animators;
        List<TILE> Tiles;
        List<LIGHT_SOURCE> Lightsources;
        List<TRIGGER> Triggers;
        List<MOBILE> Mobiles;
        List<MOBILE_TEMPLATE> MobileTemplates;
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
        int GetDebugLvl();
        int GetDebugLvlMain();
        int SetDebugLvl(int dbg_lvl);
        int SetDebugLvlMain(int dbg_lvl);
        int ResetDebugLvl();
        int DestroyMobile(int i);
};

void game_load();
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
void text_interpret(string s);
bool is_passable(int x, int z);

extern GAME * Game;

#endif
