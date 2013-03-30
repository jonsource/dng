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
void change_map(string fname, int x, int z);

#endif /* MAP_H_ */
