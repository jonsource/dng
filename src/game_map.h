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

class MOBILE_TEMPLATE;

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
int load_game_save(string fname);
MOBILE_TEMPLATE * load_mobile_template(string fname);

#endif /* MAP_H_ */
