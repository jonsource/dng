/*
 * map.h
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#ifndef MAP_H_
#define MAP_H_

#include "game.h"
#include "texture.h"
#include "interface.h"
#include "graphic.h"
#include <stdio.h>

int load_map(string fname);
void unload_map();
string serialize_map();
int load_area(string fname);
void unload_area();
string serialize_area();
int load_ini(string str);
void change_map(string fname, int x, int z);
void change_area(string aname, string mname, int x, int z);
int load_int(string str);
string load_string(string str);
float load_float(string str);
double load_double(string str);
bool load_bool(string str);
RGB * load_color(string str);
int * load_impassable(string str);

#endif /* MAP_H_ */
