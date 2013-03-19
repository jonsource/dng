/*
 * map.cpp
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#include "map.h"
#include "game.h"
#include "texture.h"
#include <string>
#include "list.h"

using namespace std;

unsigned short int MAP_SIZE;
int **map;
int **linesight;

extern char chbuf[256];
extern int DEBUG_LVL_MAIN;
extern List<TEXTURE> Textures;
extern List<TEXTURED_ELEMENT> Elements;
extern List<ANIMATOR> Animators;
extern List<TILE> Tiles;
extern List<LIGHT_SOURCE> Lightsources;
extern List<TRIGGER> Triggers;

string get_line(FILE * f)
{	string ret="";
	ret=fgets(chbuf,256,f);
	if(ret.find("#")==0) // # at the beginning of new line
		return "";	// is comment go-on
	ret=ret.substr(0,ret.size()-1);
	return ret;
}

template<class T>
unsigned short int load_block(FILE *f, string block, List<T> * l, T * (*loader)(string), string * str1)
{	string str2;
	unsigned short int count = 0;
	if(str1->compare(":"+block)==0)
	{ 	debug("Loading "+block);
		while(!feof(f))
		{ 	str2=get_line(f);
			if(str2.find(":")==0)
			{	debug("Done loading "+block);
				str1[0] = str2; break;
			} //next part of definitions
			l->add(loader(str2));
			count ++;
		}
	}
	return count;
}

int load_map(string fname)
{	string str1, str2;
	int tile;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Map "+fname+" not found!\n",10);
		exit(0);
	}
	while(!feof(f))
	{ 	str1=get_line(f);
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block
			if(str1.compare(":debuglevel")==0)
			{	str2=get_line(f);
				if(sscanf(str2.c_str(),"%d",&DEBUG_LVL_MAIN))
				{	reset_debug_lvl();
					debug("Debug level: "+to_str(DEBUG_LVL_MAIN),10);
				}
				else
				{ 	debug("Debug level missing: 0 for all, 10 for none.");
					exit(0);
				}
			}
			if(str1.compare(":mapsize")==0)
			{	str2=get_line(f);
				if(sscanf(str2.c_str(),"%hu",&MAP_SIZE))
				{	delete []map;
					map = (int **) malloc(MAP_SIZE*sizeof(int *));
					linesight = (int **) malloc(MAP_SIZE*sizeof(int *));
					for(int i=0; i<MAP_SIZE; i++)
					{	map[i]= (int*) malloc(MAP_SIZE*sizeof(int));
						linesight[i]= (int*) malloc(MAP_SIZE*sizeof(int));
						for(int j=0; j<MAP_SIZE; j++)
						{ map[i][j]=0;
						  linesight[i][j]=0;
						}
					}
					debug("Map size: "+to_str(MAP_SIZE));
				}
				else
				{ 	debug("Error reading map size in "+fname+".");
					exit(0);
				}
			}
			if(str1.compare(":map")==0)
			{	int i=0,j,found;
				while(!feof(f) && i<MAP_SIZE)
				{	str2=get_line(f);
					j=0;
					debug("str2 "+str2);
					while(sscanf(str2.c_str(),"%d,",&tile) && j<MAP_SIZE)
					{	map[i][j++]=tile;
					 	debug("Read tile ["+to_str(i)+","+to_str(j)+"]="+to_str(tile),1);
					  	found=str2.find_first_of(",");
					  	str2=str2.substr(found+1);
					}
					i++;
				}
				if(i==MAP_SIZE && j==MAP_SIZE)
				{	debug("Map read successfully");

				}
			}

			load_block(f,"textures",&Textures, load_texture, &str1);
			load_block(f,"lightsources", &Lightsources, load_lightsource, &str1);
			load_block(f,"animators", &Animators, load_animator, &str1);
			load_block(f,"elements", &Elements, load_element, &str1);
			load_block(f,"tiles", &Tiles, load_tile, &str1);
			load_block(f,"triggers", &Triggers, load_trigger, &str1);

			if(str1.compare(":end")==0)
			{ 	debug("End of definitions"); break; }
		}
	}

	return 1;
}



