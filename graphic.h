#ifndef __GRAPHIC_H_
#define __GRAPHIC_H_
#include "game.h"

int load_graphics();
void unload_graphics();
void draw_view(int x, int y, int heading);
int count_view_x(int x, int i, int j, int h);
int count_view_y(int y, int i, int j, int h);

#endif
