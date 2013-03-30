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

#define HEAD_NORTH  0
#define HEAD_EAST   1
#define HEAD_SOUTH  2
#define HEAD_WEST   3

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
void mouse_click(int mw, int mh);

string to_str(int i);
string to_str(float f);
string to_str(bool b);
string to_heading_str(int h);
int xz_to_heading(int x, int z);
bool heading_to_xz(int h, int *x, int *z);
void dappend(string s, int lvl);
void dappend(string s);
int mstime();

#endif
