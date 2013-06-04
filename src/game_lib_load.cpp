#include "allegro.h"
#include "game.h"
#include "game_lib_load.h"
#include "game_lib.h"
#include "stdio.h"

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

int load_int(string str)
{   int ret;
    if(sscanf(str.c_str(),"%d",&ret)==1) return ret;
    debug("Wrong value "+str+" for load_int()!",10);
    exit(1);
    return 0;
}

string load_string(string str)
{   return str;
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

            int dbg_lvl;
		    if(load_variable_subr(f,"debuglevel",&dbg_lvl,load_int,&str1,false))
            {   Game->SetDebugLvlMain(dbg_lvl);
                Game->ResetDebugLvl();
                debug("Debug level: "+to_str(Game->GetDebugLvlMain())+" (0 - all, 10 - none)",10);
            }
            load_variable(f, "field-of-view", &Game->view_settings.fov,load_int, &str1);
            load_variable(f, "stepback", &Game->view_settings.step_back,load_double, &str1);
            load_variable(f, "aspect", &Game->view_settings.aspect,load_double, &str1);
            load_variable(f, "view_height", &Game->view_settings.view_height,load_double, &str1);

            if(str1.compare(":end")==0)
			{ 	debug("End of "+fname+"\n"); break; }
		}
	}
	return 1;
}
