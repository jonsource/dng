/*
 * game_map.cpp
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#include "game_map.h"
#include "game.h"
#include "game_lib.h"
#include "game_lib_load.h"
#include "texture.h"
#include <string>
#include "mobile.h"

using namespace std;

extern char chbuf[256];

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

            load_multivar(f, "fade_color", &Game->fade_color, load_color, &str1);
            load_multivar(f, "impassable", &Game->Impassable, load_impassable, &str1);

            if(str1.compare(":mobiles")==0)
            {   debug("Loading mobiles");
                while(!feof(f)) // must be a while statement - to skip empty lines and comments
                {   str2 = get_line(f);
                    if(str2.length() == 0) continue;
                    if(str2.find("#") == 0) continue;
                    if(str2.find(":") == 0)
                    {	debug("Done loading mobiles");
                        str1 = str2;
                        break;
                    }

                    Game->MobileTemplates.add(load_mobile_template(str2.c_str()));
                    int last = Game->MobileTemplates.len() - 1;
                    Game->MobileTemplates[last]->template_nr = last;  // register the number for reloading mobiles from save
                    debug("after Loading mobile");
                }
            }

			load_block(f, "textures", &Game->Textures, load_texture, &str1);
			load_block(f, "animators", &Game->Animators, load_animator, &str1);
			load_block(f, "elements", &Game->Elements, load_element, &str1);
			load_block(f, "tiles", &Game->Tiles, load_tile, &str1);

			if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
    Game->area_name=fname;
	return 1;
}

int load_map_base(string fname)
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

		    if(load_variable(f,"mapsize",&Game->map_size,load_int, &str1))
            {   delete []Game->game_map;
                delete []Game->linesight;
                Game->game_map = new int * [Game->map_size];
                Game->linesight = new int * [Game->map_size];
                for(int i=0; i<Game->map_size; i++)
                {	Game->game_map[i]= new int [Game->map_size];
                    Game->linesight[i]= new int [Game->map_size];
                    for(int j=0; j<Game->map_size; j++)
                    { Game->game_map[i][j]=0;
                      Game->linesight[i][j]=0;
                    }
                }
                debug("Map size: "+to_str(Game->map_size),10);
            }

			if(str1.compare(":map")==0)
			{	int i=0,j,found;
				while(!feof(f) && i<Game->map_size)
				{	str2=get_line(f);
					j=0;
					debug("str2 "+str2);
					while(sscanf(str2.c_str(),"%d,",&tile) && j<Game->map_size)
					{	Game->game_map[j][i]=tile;
                        j++;
					 	debug("Read tile ["+to_str(i)+","+to_str(j)+"]="+to_str(tile),1);
					  	found=str2.find_first_of(",");
					  	str2=str2.substr(found+1);
					}
					i++;
				}
				if(i==Game->map_size && j==Game->map_size)
				{	debug("Map read successfully");

				}
			}

			load_block(f,"lightsources", &Game->Lightsources, load_lightsource, &str1);
			load_block(f,"triggers", &Game->Triggers, load_trigger, &str1);

			if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
    Game->map_name=fname;
	return 1;
}

int load_map_save(string fname)
{	string str1, str2;
	int tile;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Map save "+fname+" not found!\n",10);
		exit(0);
	}
	while(!feof(f))
	{ 	str1=get_line(f);

        if(str1.length()==0) continue;
        if(str1.find("#")==0) continue;
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block

		    if(str1.compare(":map")==0)
			{	int i=0,j,found;
				while(!feof(f) && i<Game->map_size)
				{	str2=get_line(f);
					j=0;
					debug("str2 "+str2);
					while(sscanf(str2.c_str(),"%d,",&tile) && j<Game->map_size)
					{	Game->game_map[j][i]=tile;
                        j++;
					 	debug("Read tile ["+to_str(i)+","+to_str(j)+"]="+to_str(tile),1);
					  	found=str2.find_first_of(",");
					  	str2=str2.substr(found+1);
					}
					i++;
				}
				if(i==Game->map_size && j==Game->map_size)
				{	debug("Map read successfully");

				}
			}

			/*load_block(f,"lightsources", &Game->Lightsources, load_lightsource, &str1);
			load_block(f,"triggers", &Game->Triggers, load_trigger, &str1);*/
			load_block_save(f, "animators", &Game->Animators, load_animator_save, &str1, false); // false - don't create the object, only fill in
			load_block_save(f, "mobiles", &Game->Mobiles, load_mobile_save, &str1, true); // true - create the object from the save

			if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
	return 1;
}

int load_map(string fname)
{   string sav="save/"+fname;
    load_map_base(fname);
    //reset animators
    for(int i=0;i<Game->Animators.len(); i++)
    {   Game->Animators[i]->offset=Game->Animators[i]->_offset;

    }
    debug("Looking for "+sav,5);
    FILE *f=fopen(sav.c_str(),"r");
    if(f)
    {
        fclose(f);
        dappend(" ... found",5);
        load_map_save(sav);

    }
    Game->map_name=fname;
    return 1;
}

void unload_area()
{   unload_map();
    for(int i=0; i<Game->Textures.len(); i++)
    {   destroy_bitmap(Game->Textures[i]->close);
        destroy_bitmap(Game->Textures[i]->medium);
        destroy_bitmap(Game->Textures[i]->far);
    }
    Game->Textures.clear_all();

    for(int i=0; i<Game->Animators.len(); i++)
    {   destroy_bitmap(Game->Animators[i]->frame);
    }
    Game->Animators.clear_all();

    for(int i=0; i<Game->Tiles.len(); i++)
    {   delete[] Game->Tiles[i]->elements;
        delete[] Game->Tiles[i]->element_types;
    }
    for(int i=0; i<Game->Tiles.len(); i++)
    {   delete[] Game->Tiles[i]->statics;
        delete[] Game->Tiles[i]->static_types;
    }
    Game->Tiles.clear_all();

    Game->Elements.clear_all();

    delete Game->fade_color;
    delete[] Game->Impassable;

    Game->Mobiles.clear_all();
    Game->MobileTemplates.clear_all();
}

int save_map()
{   FILE *f;
    string sav="save/"+Game->map_name;
    f = fopen(sav.c_str(),"w");
    if(!f)
	{	debug("Couldn't write map save "+sav+"!\n",10);
		exit(0);
	}
	debug("Saving map "+sav+".",5);
	fprintf(f,":animators\n");
    fprintf(f,Game->Animators.save_string().c_str());
    fprintf(f,":mobiles\n");
    fprintf(f,Game->Mobiles.save_string().c_str());
    fprintf(f,":end\n");
    fclose(f);
    return 1;
}

void unload_map()
{   debug("Game map name to save: "+Game->map_name,4);
    if(Game->map_name.length()) save_map();
    Game->map_name.clear();
    Game->Lightsources.clear_all();
    Game->Triggers.clear_all();
    //Game->Clickables.clear();
    Game->Clickables["enter"].clear_all();
    Game->Clickables["leave"].clear_all();
    Game->Clickables["place"].clear_all();

    Game->Mobiles.clear_all();
}

void change_map(string fname, int x, int z)
{   unload_map();
    load_map(fname);

    debug("Map changed to :"+fname);
    debug("lightsources :"+to_str(Game->Lightsources.len()));
    debug("triggers :"+to_str(Game->Triggers.len()));

    /* reset animators */
    player_move_subr(x,0,z,-1,true);
}

void change_area(string aname, string mname, int x, int z)
{   unload_area();
    load_area(aname);

    debug("Area changed to :"+aname);
    debug("textures :"+to_str(Game->Textures.len()));
    debug("animators :"+to_str(Game->Animators.len()));
    debug("elements :"+to_str(Game->Elements.len()));
    debug("tiles :"+to_str(Game->Tiles.len()));

    change_map(mname,x,z);
}

int load_game_save(string fname)
{	string str1, str2;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Game save "+fname+" not found! Beginning from start.\n",10);
		return 0;
	}
	while(!feof(f))
	{ 	str1=get_line(f);

        if(str1.length()==0) continue;
        if(str1.find("#")==0) continue;
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block

            load_variable(f, "area", &Game->area_name,load_string, &str1);
            load_variable(f, "map", &Game->map_name,load_string, &str1);
            load_variable(f, "time", &Game->time,load_int, &str1);
            load_variable(f, "position_x", &Game->x,load_int, &str1);
            load_variable(f, "position_y", &Game->y,load_int, &str1);
            load_variable(f, "position_z", &Game->z,load_int, &str1);
            load_variable(f, "position_h", &Game->h,load_int, &str1);

            if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
	return 1;
}

TEXTURED_ELEMENT * load_element(string  str)
{	char type[32], transparent[32], clip[16], flip[16];
	float x,y,z,w,h;
	int texture,animator;
	if(sscanf(str.c_str(),"%s %f %f %f %f %f %s %d %d %s %s",type,&x,&y,&z,&w,&h,transparent,&texture,&animator,clip,flip)<11)
	{ debug("Not enough parameters for textured element: "+str,10);
	  exit(1);
	}
	return new TEXTURED_ELEMENT(type,x,y,z,w,h,transparent,texture,animator,clip,flip);
}

int * load_impassable(string str)
{	int * ret = new int[10];
    if(sscanf(str.c_str(),"%d %d %d %d %d %d %d %d %d %d",&ret[0],&ret[1],&ret[2],&ret[3],&ret[4],&ret[5],&ret[6],&ret[7],&ret[8],&ret[9])<10)
	{ debug("Not enough parameters for impassable: "+str,10);
	  exit(1);
	}
	return ret;
}

string serialize_impassable()
{   string ret="";
    for(int i=0; i<10; i++)
    {   if(i>0) ret+=" ";
        ret+=to_str(Game->Impassable[i]);
    }
    return ret;
}

string serialize_area()
{   string ret="";
    ret+=":fade_color\n";
    ret+=serialize_color(Game->fade_color);
    ret+="\n\n:impassable\n";
    ret+=serialize_impassable();
    ret+="\n\n:textures\n";
    ret+=Game->Textures.serialize();
    ret+="\n:animators\n";
    ret+=Game->Animators.serialize();
    ret+="\n:elements\n";
    ret+=Game->Elements.serialize();
    ret+="\n:tiles\n";
    ret+=Game->Tiles.serialize();
    ret+="\n:end\n";
    return ret;
}

string serialize_map()
{   char buf[8];
    string ret="";
    ret+="\n:mapsize\n";
    ret+=to_str(Game->map_size)+"\n";
    ret+="\n:map\n";
    for(int i=0; i<Game->map_size; i++)
    {   for(int j=0;j<Game->map_size; j++)
        {   sprintf(buf,"%2d,",Game->game_map[j][i]);
            ret+=buf;
        }
        ret+="\n";
    }
    ret+="\n:lightsources\n";
    ret+=Game->Lightsources.serialize();
    ret+="\n:triggers\n";
    ret+=Game->Triggers.serialize();
    ret+="\n:end\n";
    return ret;
}
