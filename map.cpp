/*
 * map.cpp
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#include "map.h"
#include "game.h"
#include <string>

using namespace std;

unsigned short int MAP_SIZE;
int **map;

int load_map(string fname)
{	char cstr[256],cstr2[30];
	int mapsize;
	int tile;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Map "+fname+" not found!\n",10);
		exit(0);
	}
	fscanf(f,"[%d]",&mapsize);
	sprintf(cstr,"Loading map %s size %d\n",fname.c_str(),mapsize);
	debug(cstr);
	//if(mapsize<15) mapsize=15;
	MAP_SIZE=mapsize;
	map = (int **) malloc(mapsize*sizeof(int *));
	for(int i=0; i<MAP_SIZE; i++)
	{	map[i]= (int*) malloc(mapsize*sizeof(int));
		for(int j=0; j<MAP_SIZE; j++)
			map[i][j]=0;
	}
	int i=0;
	while(fscanf(f,"%s",cstr) && i<MAP_SIZE)
	{	int j=0;
		string str=cstr;
		int found;
		while(sscanf(str.c_str(),"%d,",&tile) && j<MAP_SIZE)
		{	map[i][j++]=tile;
			sprintf(cstr2,"Read tile [%d,%d]=%d\n",i,j-1,tile);
			debug(cstr2,1);
			found=str.find_first_of(",");
			string str2=str.substr(found+1);
			str=str2;
		}
		i++;

	}
	debug("Map loaded\n");
	return 1;
}



