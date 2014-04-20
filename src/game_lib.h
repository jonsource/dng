#ifndef GAME_LIB_H_
#define GAME_LIB_H

#define HEAD_NORTH  0
#define HEAD_EAST   1
#define HEAD_SOUTH  2
#define HEAD_WEST   3

#include <string>
#include "list.h"

using namespace std;

string heading_to_str(int h);
int xz_to_heading(int x, int z);
bool heading_to_xz(int h, int *x, int *z);
float dist2(float x,float z,float xx, float zz);
int compare_ind_val(const void * a, const void * b);

#endif
