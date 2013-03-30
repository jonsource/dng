/*
 * map.h
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#ifndef MAP_H_
#define MAP_H_

#include "game.h"
#include <stdio.h>

int load_map(string fname);
int load_ini(string str);
void change_map(string fname, int x, int z);
int load_int(string str);
string load_string(string str);
float load_float(string str);
double load_double(string str);
bool load_bool(string str);

#endif /* MAP_H_ */
