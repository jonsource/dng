#ifndef __GAME_H_
#define __GAME_H_

#include <stdio.h>
#include <allegro.h>
#include <string>
#include "list.h"
#include "game_lib.h"
#include "texture.h"
#include "interface.h"
#include "graphic.h"

using namespace std;

#define DEAD     0
#define CREATE   1
#define PLAY     2
#define PAUSE    -1

class GAME;

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


#endif
