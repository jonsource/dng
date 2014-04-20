#include "allegro.h"
#include "game.h"
#include "game_lib_load.h"
#include "game_lib.h"
#include "stdio.h"
#include "../lib/libjsjson.h"

using namespace jslib;

string serialize_color(RGB * color)
{   return to_str(color->r)+" "+to_str(color->g)+" "+to_str(color->b);
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

int load_ini(string fname)
{   JsonReader jr;

	Node * root = jr.read(fname);

    if(root == NULL)
    {   printf("didn't read");
        exit(1);
    }
    Game->SetDebugLvlMain(root->getMember("debuglevel")->getInt());
    Game->ResetDebugLvl();
    debug("Debug level: "+to_str(Game->GetDebugLvlMain())+" (0 - all, 10 - none)",10);
    Game->view_settings.fov = root->getMember("field-of-view")->getInt();
    Game->view_settings.step_back = root->getMember("stepback")->getFloat();
    Game->view_settings.aspect = root->getMember("aspect")->getInt();
    Game->view_settings.view_height = root->getMember("view_height")->getFloat();

	return 1;
}
