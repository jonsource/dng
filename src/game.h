#ifndef __GAME_H_
#define __GAME_H

#include <stdio.h>
#include <allegro.h>
#include <string>
#include "list.h"

using namespace std;

#define DEAD     0
#define CREATE   1
#define PLAY     2
#define PAUSE    -1

#define HEAD_NORTH  1
#define HEAD_EAST   0
#define HEAD_SOUTH  3
#define HEAD_WEST   2 

void game_load();
void game_unload();
void game_draw();
void keypress(int);
void pick(int);
void next_level();
void restart();
void debug(string s);
void debug(string s,int lvl);
void set_debug_lvl(int lvl);
void reset_debug_lvl();
bool to_bool(string s);

string to_str(int i);
string to_str(float f);
string to_str(bool b);
void dappend(string s, int lvl);
void dappend(string s);
int mstime();

#endif
