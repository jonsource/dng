#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jslib.h"

namespace jslib {

    FILE *dbg=NULL;
    char chbuf[256];
    int DEBUG_LVL, DEBUG_LVL_MAIN;

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

    string to_str(string s)
    {	return s;
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
        if(lvl<DEBUG_LVL) return;
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
    { 	if(lvl<DEBUG_LVL) return;
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
    int set_debug_lvl(int lvl)
    {
        return DEBUG_LVL=lvl;
    }

    int get_debug_lvl()
    {
        return DEBUG_LVL;
    }



    int set_debug_lvl_main(int lvl)
    {
        return DEBUG_LVL_MAIN=lvl;
    }


    int get_debug_lvl_main()
    {
        return DEBUG_LVL_MAIN;
    }

    /**
     * reset debug level to main lvl
     */
    int reset_debug_lvl()
    {
        return DEBUG_LVL=DEBUG_LVL_MAIN;
    }

    /**
        explode given string s using sep as separator
    */

    StringArray * explode(string sep, string s)
    {   StringArray * ret= new StringArray;
        string * str;
        unsigned int st,en=0;
        en=s.find(sep,0);
        if(en==(unsigned int)-1)
        {   ret->push_back(new string(s));
            return ret;
        }
        st=en+1;
        str = new string(s.substr(0,en));
        ret->push_back(str);
        //debug("first string found '"+*str+"' searching in "+to_str((int)st)+" '"+s.substr(st,-1)+"'");
        while(st<s.length())
        {   en=s.find(sep,st);
            if(en==(unsigned int)-1)
            {   str = new string(s.substr(st,-1));
                //debug("last string found '"+*str+"'");
                ret->push_back(str);
                break;
            }
            str = new string(s.substr(st,en-st));
            st=en+1;
            ret->push_back(str);
            //debug("next string found "+to_str((int)st)+"'"+*str+"'"+to_str((int)en)+" searching in "+to_str((int)st)+" '"+s.substr(st,-1)+"'");

        }
        return ret;
    }

    string implode(string sep,StringArray * s)
    {
        if(s->size()==1)
        {   return *(*s)[0];
        }
        string ret=*(*s)[0];
        SAi it = s->begin();
        it++;
        for(; it < s->end(); it++)
        {   ret+=sep+*(*it);

        }
        return ret;
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

}
