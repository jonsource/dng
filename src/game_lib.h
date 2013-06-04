#ifndef GAME_LIB_H_
#define GAME_LIB_H

#define HEAD_NORTH  0
#define HEAD_EAST   1
#define HEAD_SOUTH  2
#define HEAD_WEST   3

#include <string>
#include "list.h"

using namespace std;

void init_debug(string s);
void deinit_debug();
void debug(string s);
void debug(string s,int lvl);
void dappend(string s, int lvl);
void dappend(string s);

namespace game_lib
{
    int get_debug_lvl_main();
    int set_debug_lvl_main(int lvl);
    int get_debug_lvl();
    int set_debug_lvl(int lvl);
    int reset_debug_lvl();
}

bool to_bool(string s);
string to_str(int i);
string to_str(float f);
string to_str(double f);
string to_str(bool b);
string heading_to_str(int h);
string to_str(string s);
int xz_to_heading(int x, int z);
bool heading_to_xz(int h, int *x, int *z);
int mstime();
STR_LIST * tokenize(string s, string sep);
float dist2(float x,float z,float xx, float zz);
int compare_ind_val(const void * a, const void * b);
string get_line(FILE * f);

#endif
