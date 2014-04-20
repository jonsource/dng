#include <stdio.h>
#include <time.h>
#include "../lib/jslib.h"
#include "texture.h"
#include "game_lib.h"

FILE *dbg=NULL;
char chbuf[256];
int DEBUG_LVL, DEBUG_LVL_MAIN;

using namespace jslib;

/**
 * square of distance of two coordinates
 */
float dist2(float x,float z,float xx, float zz)
{ return ((x-xx)*(x-xx))+((z-zz)*(z-zz));

}

void init_debug(string fname)
{   dbg = fopen(fname.c_str(),"w");
	debug("*** NEW RUN ***");
}

void deinit_debug()
{   fclose(dbg);
}

bool heading_to_xz(int h,int *x,int *z)
{   switch(h)
    {   case HEAD_EAST : *x = 1; *z = 0; return true;
        case HEAD_NORTH : *x = 0; *z = -1; return true;
        case HEAD_WEST : *x = -1; *z = 0; return true;
        case HEAD_SOUTH : *x = 0; *z = 1; return true;
    }
    return false;
}

int xz_to_heading(int xfront, int zfront)
{   if(zfront<0) return HEAD_NORTH;
    if(zfront>0) return HEAD_SOUTH;
    if(xfront<0) return HEAD_WEST;
    return HEAD_EAST;

}

string heading_to_str(int h)
{   if(h==HEAD_NORTH) return "north";
    if(h==HEAD_EAST) return "east";
    if(h==HEAD_SOUTH) return "south";
    if(h==HEAD_WEST) return "west";
    return "unknown";
}

/* comparator function for qicksort */
int compare_ind_val(const void * a, const void * b)
{   if ( (*(IND_VAL*)a).value <  (*(IND_VAL*)b).value ) return 1;
    if ( (*(IND_VAL*)a).value == (*(IND_VAL*)b).value ) return 0;
    if ( (*(IND_VAL*)a).value >  (*(IND_VAL*)b).value ) return -1;
    return 0;
}

/* get line from file, ignore commented lines */
string get_line(FILE * f)
{	string ret="";
	ret=fgets(chbuf,256,f);
	if(ret.find("#")==0) // # at the beginning of new line
		return "";	// is comment go-on
	ret=ret.substr(0,ret.size()-1);
	return ret;
}
