/*
 * game_map.cpp
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#include "game_map.h"
#include "game.h"
#include "game_lib.h"
#include "graphic.h"
#include "texture.h"
#include <string>
#include "list.h"
#include "interface.h"

using namespace std;

int MAP_SIZE;
int **game_map;
int **linesight;

extern char chbuf[256];
extern int DEBUG_LVL_MAIN;
extern List<TEXTURE> Textures;
extern List<TEXTURED_ELEMENT> Elements;
extern List<ANIMATOR> Animators;
extern List<TILE> Tiles;
extern List<LIGHT_SOURCE> Lightsources;
extern List<TRIGGER> Triggers;
extern CLICKABLE_MAP Clickables;
extern VIEW_SETTINGS view_settings;
extern RGB * fade_color;
int * Impassable;

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
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
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

template<class T>
unsigned short int load_multivar(FILE *f, string block, T ** l, T * (*loader)(string), string * str1)
{	string str2;
	unsigned short int count = 0;
	if(str1->compare(":"+block)==0)
	{ 	debug("Loading "+block);
		while(!feof(f))
		{ 	str2=get_line(f);
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
			if(str2.find(":")==0)
			{	debug("Done loading "+block);
				str1[0] = str2; break;
			} //next part of definitions
			*l=loader(str2);
			count ++;
			break;
		}
	}
	return count;
}

template<class T>
unsigned short int load_variable_subr(FILE *f, string block, T * var, T (*loader)(string), string * str1, bool report)
{   string str2;
    unsigned short int count = 0;
    if(str1->compare(":"+block)==0)
    {   debug("Loading "+block);
		while(!feof(f)) // must be a while statement - to skip empty lines and comments
        {   str2=get_line(f);
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
			if(str2.find(":")==0)
			{	debug("Done loading "+block);
				str1[0] = str2; break;
			} //next part of definitions
			*var = loader(str2);
			if(report)
            {
                debug(block+" loaded: "+to_str(*var),10);
            }
            count++;
			break;
        }
    }
    return count;
}

template<class T>
unsigned short int load_variable(FILE *f, string block, T * var, T (*loader)(string), string * str1)
{   return load_variable_subr(f,block,var,loader,str1,true);
}

int load_ini(string fname)
{   string str1, str2;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("File "+fname+" not found!\n",10);
		exit(0);
	}
	while(!feof(f))
	{ 	str1=get_line(f);

        if(str1.length()==0) continue;
        if(str1.find("#")==0) continue;
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block

		    if(load_variable_subr(f,"debuglevel",&DEBUG_LVL_MAIN,load_int,&str1,false))
            {   reset_debug_lvl();
                debug("Debug level: "+to_str(DEBUG_LVL_MAIN)+" (0 - all, 10 - none)",10);
            }
            load_variable(f,"field-of-view",&view_settings.fov,load_int, &str1);
            load_variable(f,"stepback",&view_settings.step_back,load_double, &str1);
            load_variable(f,"aspect",&view_settings.aspect,load_double, &str1);
            load_variable(f,"view_height",&view_settings.view_height,load_double, &str1);

            if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
	return 1;
}

int load_area(string fname)
{	string str1, str2;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Area "+fname+" not found!\n",10);
		exit(0);
	}
	while(!feof(f))
	{ 	str1=get_line(f);

        if(str1.length()==0) continue;
        if(str1.find("#")==0) continue;
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block

            load_multivar(f,"fade_color",&fade_color, load_color, &str1);
            load_multivar(f,"impassable",&Impassable, load_impassable, &str1);

			load_block(f,"textures",&Textures, load_texture, &str1);
			load_block(f,"animators", &Animators, load_animator, &str1);
			load_block(f,"elements", &Elements, load_element, &str1);
			load_block(f,"tiles", &Tiles, load_tile, &str1);

			if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}

	return 1;
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

        if(str1.length()==0) continue;
        if(str1.find("#")==0) continue;
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block

		    if(load_variable(f,"mapsize",&MAP_SIZE,load_int, &str1))
            {   delete []game_map;
                delete []linesight;
                game_map = new int * [MAP_SIZE];
                linesight = new int * [MAP_SIZE];
                for(int i=0; i<MAP_SIZE; i++)
                {	game_map[i]= new int [MAP_SIZE];
                    linesight[i]= new int [MAP_SIZE];
                    for(int j=0; j<MAP_SIZE; j++)
                    { game_map[i][j]=0;
                      linesight[i][j]=0;
                    }
                }
                debug("Map size: "+to_str(MAP_SIZE)+" (0 - all, 10 - none)",10);
            }

			if(str1.compare(":map")==0)
			{	int i=0,j,found;
				while(!feof(f) && i<MAP_SIZE)
				{	str2=get_line(f);
					j=0;
					debug("str2 "+str2);
					while(sscanf(str2.c_str(),"%d,",&tile) && j<MAP_SIZE)
					{	game_map[j][i]=tile;
                        j++;
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

			load_block(f,"lightsources", &Lightsources, load_lightsource, &str1);
			load_block(f,"triggers", &Triggers, load_trigger, &str1);

			if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
	return 1;
}

void unload_area()
{   for(int i=0; i<Textures.len(); i++)
    {   destroy_bitmap(Textures.items[i]->close);
        destroy_bitmap(Textures.items[i]->medium);
        destroy_bitmap(Textures.items[i]->far);
    }
    Textures.clear_all();

    for(int i=0; i<Animators.len(); i++)
    {   destroy_bitmap(Animators.items[i]->frame);
    }
    Animators.clear_all();

    for(int i=0; i<Tiles.len(); i++)
    {   delete[] Tiles.items[i]->elements;
        delete[] Tiles.items[i]->types;
    }
    Tiles.clear_all();

    Elements.clear_all();

    delete fade_color;
    delete[] Impassable;
}

void unload_map()
{   Lightsources.clear_all();
    Triggers.clear_all();
    Clickables.clear();
}

void change_map(string fname, int x, int z)
{   unload_map();
    load_map(fname);

    debug("Map changed to :"+fname);
    debug("lightsources :"+to_str(Lightsources.len()));
    debug("triggers :"+to_str(Triggers.len()));

    /* reset animators */
    for(int i=0;i<Animators.len(); i++)
    {   Animators[i]->offset=Animators[i]->_offset;

    }
    player_move_subr(x,0,z,-1,true);
}

void change_area(string aname, string mname, int x, int z)
{   unload_area();
    load_area(aname);

    debug("Area changed to :"+aname);
    debug("textures :"+to_str(Textures.len()));
    debug("animators :"+to_str(Animators.len()));
    debug("elements :"+to_str(Elements.len()));
    debug("tiles :"+to_str(Tiles.len()));

    change_map(mname,x,z);
}

int load_int(string str)
{   int ret;
    if(sscanf(str.c_str(),"%d",&ret)==1) return ret;
    debug("Wrong value "+str+" for load_float()!",10);
    exit(1);
    return 0;
}

string load_string(string str)
{   return str;
}

float load_float(string str)
{   float ret;
    if(sscanf(str.c_str(),"%f",&ret)==1) return ret;
    debug("Wrong value "+str+" for load_float()!",10);
    exit(1);
    return 0.0;
}

double load_double(string str)
{   float ret;
    if(sscanf(str.c_str(),"%f",&ret)==1) return ret;
    debug("Wrong value "+str+" for load_double()!",10);
    exit(1);
    return 0.0;
}

bool load_bool(string str)
{   return to_bool(str);

}

RGB * load_color(string str)
{	RGB * ret = new RGB;
    int r,g,b;
    if(sscanf(str.c_str(),"%d %d %d",&r,&g,&b)<3)
	{ debug("Not enough parameters for color: "+str,10);
	  exit(1);
	}
	ret->r=(char)r;
	ret->g=(char)g;
	ret->b=(char)b;
	return ret;
}

int * load_impassable(string str)
{	int * ret = new int[10];
    if(sscanf(str.c_str(),"%d %d %d %d %d %d %d %d %d %d",&ret[0],&ret[1],&ret[2],&ret[3],&ret[4],&ret[5],&ret[6],&ret[7],&ret[8],&ret[9])<10)
	{ debug("Not enough parameters for impassable: "+str,10);
	  exit(1);
	}
	return ret;
}
