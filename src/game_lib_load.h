#ifndef GAME_LIB_LOAD_H_INCLUDED
#define GAME_LIB_LOAD_H_INCLUDED

#include <string>
#include "list.h"
#include "jslib.h"
#include "stdio.h"

struct RGB;

std::string serialize_color(RGB * color);
RGB * load_color(std::string str);
int load_ini(std::string fname);

template<class T>
int load_block(FILE *f, string block, List<T> * l, T * (*loader)(string), string * str1)
{	string str2;
	unsigned short int count = 0;
	if(str1->compare(":"+block)==0)
	{ 	jslib::debug("Loading "+block);
		while(!feof(f))
		{ 	str2=jslib::get_line(f);
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
			if(str2.find(":")==0)
			{	jslib::debug("Done loading "+block);
				str1[0] = str2; break;
			} //next part of definitions
			l->add(loader(str2));
			count ++;
		}
	}
	return count;
}

template<class T>
int load_block_save(FILE *f, string block, List<T> * l, int (*loader)(T *, string), string * str1, bool create)
{	string str2;
	int count = 0;
	if(str1->compare(":"+block)==0)
	{ 	jslib::debug("Loading "+block+" save");
		while(!feof(f))
		{ 	str2=jslib::get_line(f);
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
			if(str2.find(":")==0)
			{	jslib::debug("Done loading "+block+" save");
				str1[0] = str2; break;
			} //next part of definitions
			if(create)
            {
                l->add(new T);
            }
			if(loader((*l)[count],str2)) count ++;
		}
	}
	if(count < l->len()-1) jslib::debug("Not enough entries for members of "+block+". Save Corrupt!",10);
	return count;
}

template<class T>
int load_multivar(FILE *f, string block, T ** l, T * (*loader)(string), string * str1)
{	string str2;
	unsigned short int count = 0;
	if(str1->compare(":"+block)==0)
	{ 	jslib::debug("Loading "+block);
		while(!feof(f))
		{ 	str2=jslib::get_line(f);
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
			if(str2.find(":")==0)
			{	jslib::debug("Done loading "+block);
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
int load_variable_subr(FILE *f, string block, T * var, T (*loader)(string), string * str1, bool report)
{   string str2;
    unsigned short int count = 0;
    if(str1->compare(":"+block)==0)
    {   jslib::debug("Loading "+block);
		while(!feof(f)) // must be a while statement - to skip empty lines and comments
        {   str2=jslib::get_line(f);
            if(str2.length()==0) continue;
            if(str2.find("#")==0) continue;
			if(str2.find(":")==0)
			{	jslib::debug("Done loading "+block);
				str1[0] = str2; break;
			} //next part of definitions
			*var = loader(str2);
			if(report)
            {
                jslib::debug(block+" loaded: "+jslib::to_str(*var),10);
            }
            count++;
			break;
        }
    }
    return count;
}

template<class T>
int load_variable(FILE *f, string block, T * var, T (*loader)(string), string * str1)
{   return load_variable_subr(f,block,var,loader,str1,true);
}

#endif // GAME_LIB_LOAD_H_INCLUDED
