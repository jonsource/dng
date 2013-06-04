/*
 * map.h
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#ifndef MAP_H_
#define MAP_H_

#include "allegro.h"
#include <string>

using namespace std;

int load_map(string fname);
void unload_map();
string serialize_map();
int load_area(string fname);
void unload_area();
string serialize_area();
void change_map(string fname, int x, int z);
void change_area(string aname, string mname, int x, int z);
int * load_impassable(string str);
int load_game_save(string fname);

#endif /* MAP_H_ */
