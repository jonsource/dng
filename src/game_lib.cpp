#include <stdio.h>
#include <time.h>
#include "texture.h"
#include "game_lib.h"

FILE *dbg=NULL;
char chbuf[256];

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
/**
 * convert string to bool
 */
bool to_bool(string s)
{  if(s=="false" || s=="False") return false;
   if(s=="true" || s=="True") return true;
   if(s.find("no-")==0) return false;
   else
   { debug("Unclear argument "+s+", defaulting to True",9);
     return true;
   }
}

/**
 * convert integer to string
 */
string to_str(int i)
{	sprintf(chbuf,"%d",i);
	return chbuf;
}

/**
 * convert float to string
 */
string to_str(float f)
{	sprintf(chbuf,"%.2f",f);
	return chbuf;
}

/**
 * convert double to string
 */
string to_str(double f)
{	sprintf(chbuf,"%.2f",f);
	return chbuf;
}

/**
 * convert bool to string
 */
string to_str(bool b)
{	if(b) return "true";
	return "false";
}

/**

*/

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

/**
 * get current time in miliseconds based on processor clock
 */
int mstime()
{	return clock()/(float)CLOCKS_PER_SEC*100;
}

/**
 * write debug information to log
 * lvl - level of importance, the higher the more important
 */
void debug(string s, int lvl)
{
	if(lvl<Game->DEBUG_LVL) return;
	s="\n"+to_str((int)mstime())+" "+s;
	printf(s.c_str());
	fprintf(dbg,s.c_str());
	fflush(dbg);
}
/**
 * write debug information to log with default lvl
 */
void debug(string s)
{ debug(s,4);
}

/**
 * append debug information (omits current time stamp)
 */
void dappend(string s, int lvl)
{ 	if(lvl<Game->DEBUG_LVL) return;
    printf(s.c_str());
	fprintf(dbg,s.c_str());
	fflush(dbg);
}

/**
 * append debug information (omits current time stamp), with default lvl
 */
void dappend(string s)
{ 	dappend(s,4);
}

/**
 * set what lvl of importance is needed to be logged, 0 for everything, 10 for production version
 * may be used to temporarily increase logging in parts of code
 */
void set_debug_lvl(int lvl)
{
	Game->DEBUG_LVL=lvl;
}

/**
 * reset debug level to main lvl
 */
void reset_debug_lvl()
{
	Game->DEBUG_LVL=Game->DEBUG_LVL_MAIN;
}

/**
    explode given string s using sep as separator
*/

STR_LIST * tokenize(string s, string sep)
{   STR_LIST * ret= new STR_LIST;
    string * str;
    unsigned int st,en=0;
    en=s.find(sep,0);
    if(en==(unsigned int)-1)
    {   ret->add(new string(s));
        return ret;
    }
    st=en+1;
    str = new string(s.substr(0,en));
    ret->add(str);
    //debug("first string found '"+*str+"' searching in "+to_str((int)st)+" '"+s.substr(st,-1)+"'");
    while(st<s.length())
    {   en=s.find(sep,st);
        if(en==(unsigned int)-1)
        {   str = new string(s.substr(st,-1));
            //debug("last string found '"+*str+"'");
            ret->add(str);
            break;
        }
        str = new string(s.substr(st,en-st));
        st=en+1;
        ret->add(str);
        //debug("next string found "+to_str((int)st)+"'"+*str+"'"+to_str((int)en)+" searching in "+to_str((int)st)+" '"+s.substr(st,-1)+"'");

    }
    return ret;
}
