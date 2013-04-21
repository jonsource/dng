/*
 * texture.cpp
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#include "texture.h"
#include "graphic.h"
#include "game_map.h"
#include <string.h>
#include <stdio.h>

List<TEXTURE> Textures;
List<TEXTURED_ELEMENT> Elements;
List<ANIMATOR> Animators;
List<TILE> Tiles;
List<LIGHT_SOURCE> Lightsources;
List<TRIGGER> Triggers;

extern int tmsec;
/**
 * loads texture from file. Separates the string s into filename and extension, and loads filename_close.extension
 * and then _medium and _far. If can't find appropriate texture, falls back to resizing.
 */

TEXTURE * load_texture(string s)
{   PALETTE pal;
    TEXTURE *text;
    string fn,cfn,ext;
    int pos;

    text = new (TEXTURE);
    fn.assign("data/images/");

    pos = s.rfind(".");
    fn.append(s.substr(0,pos));
    ext = s.substr(pos+1);

    cfn.assign(fn);
    cfn.append("_close."+ext);
    text->close = load_bitmap(cfn.c_str(), pal);
    if(!text->close) debug("Missing texture "+cfn,10);
    else debug(cfn+" texture depth "+to_str(bitmap_color_depth(text->close)),3);
    cfn.assign(fn);
    cfn.append("_medium."+ext);
    text->medium = load_bitmap(cfn.c_str(), pal);
    if(!text->medium)
    { 	debug("Missing texture "+cfn,10);
    	if(text->close)
    	{ dappend(" - resizing close texture");
    	  text->medium = create_bitmap(128,128);
    	  stretch_blit(text->close,text->medium,0,0,text->close->w,text->close->h,0,0,text->medium->w,text->medium->h);
    	}
    }
    cfn.assign(fn);
    cfn.append("_far."+ext);
    text->far = load_bitmap(cfn.c_str(), pal);
    if(!text->far)
    { 	debug("Missing texture "+cfn,10);
		if(text->medium)
    	{ dappend(" - resizing medium texture");
    	  text->far = create_bitmap(64,64);
    	  stretch_blit(text->medium,text->far,0,0,text->medium->w,text->medium->h,0,0,text->far->w,text->far->h);
    	}
    }
    return text;
}

TEXTURED_ELEMENT::TEXTURED_ELEMENT(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator, string clip, string flip)
{   unsigned short int typ;
   this->x = x;
   this->y = y;
   this->z = z;
   this->w = w;
   this->h = h;
   this->transparent = to_bool(transparent);
   this->clip = to_bool(clip);
   if(texture>=Textures.len())
   { debug("Referencing undefined texture "+to_str(texture),10);
   	 this->texture = Textures[0];
   }
   else this->texture = Textures[texture];
   this->texture_nr = texture;
   this->transparent = to_bool(transparent);
   if(animator>-1)
   {   if(animator<Animators.len())	this->animator = Animators[animator];
   	   else
   	   {	debug("Referencing undefined animator "+to_str(animator),10);
   	   	   exit(1);
   	   }
   }
   else this->animator = NULL;
   this->animator_nr = animator;
   typ=tile_type_resolve(type);
   this->type=typ;
   this->flip=flip_resolve(flip);
   //debug("Element "+type);
}

string TEXTURED_ELEMENT::serialize()
{   return to_str(type)+" "+to_str(x)+" "+to_str(y)+" "+to_str(z)+" "+to_str(w)+" "+to_str(h)+" "+to_str(transparent)+" "+to_str(texture_nr)+" "+to_str(animator_nr)+" "+to_str(clip)+" "+to_str(flip);

}

TEXTURED_ELEMENT * load_element(string s)
{	char type[32], transparent[32], clip[16], flip[16];
	float x,y,z,w,h;
	int texture,animator;
	if(sscanf(s.c_str(),"%s %f %f %f %f %f %s %d %d %s %s",type,&x,&y,&z,&w,&h,transparent,&texture,&animator,clip,flip)<11)
	{ debug("Not enough parameters for textured element: "+s,10);
	  exit(1);
	}
	return new TEXTURED_ELEMENT(type,x,y,z,w,h,transparent,texture,animator,clip,flip);
}

unsigned short int ANIMATOR::type_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
    if(type.compare("ANIMATOR_TOGGLE")==0) {typ=ANIMATOR_TOGGLE; type_ok=true;}
    if(!type_ok && type.compare("ANIMATOR_ON")==0) {typ=ANIMATOR_ON; type_ok=true;}
    if(!type_ok && type.compare("ANIMATOR_OFF")==0) {typ=ANIMATOR_OFF; type_ok=true;}
    if(!type_ok && type.compare("MOVATOR_Y")==0) {typ=MOVATOR_Y; type_ok=true;}
    if(!type_ok)
	{	debug("Unknown trigger type "+type,10);
		exit(1);
	}
	return typ;
}

string ANIMATOR::type_string()
{   switch(this->type)
    {
        case ANIMATOR_TOGGLE: return "ANIMATOR_TOGGLE";
        case ANIMATOR_ON: return "ANIMATOR_ON";
        case ANIMATOR_OFF: return "ANIMATOR_OFF";
        case MOVATOR_Y: return "MOVATOR_Y";
    }
    debug("Unknown trigger type number: "+to_str(type),10);
	exit(1);
	return "";
}

ANIMATOR::ANIMATOR(int type, int speed, int offset, int frames, int w, int h)
{ 	this->speed=speed;
	this->offset=offset;
	this->_offset=offset;
	this->frames=frames;
	this->w=w;
	this->h=h;
	this->frame=create_bitmap(w,h);
	this->type=type;
	if(this->type==ANIMATOR_ON) this->on=1;
	else this->on=0;
	this->start=0;
}

ANIMATOR::ANIMATOR(int type, int speed, int offset, int frames, int w, int h, int on, int start)
{   this->type=type;
    this->speed=speed;
	this->offset=offset;
	this->_offset=offset;
	this->frames=frames;
	this->w=w;
	this->h=h;
	this->frame=create_bitmap(w,h);
	this->on=on;
    this->start=start;
}

string ANIMATOR::serialize()
{   return this->type_string()+" "+to_str(speed)+" "+to_str(offset)+" "+to_str(frames)+" "+to_str(w)+" "+to_str(h)+" "+to_str(on)+" "+to_str(start);
}

ANIMATOR * load_animator(string s)
{	char buf[32];
    int speed, offset, frames, w, h;
	if(sscanf(s.c_str(),"%s %d %d %d %d %d",buf,&speed,&offset,&frames,&w,&h)<6)
	{ debug("Not enough parameters for animator.",10);
	  exit(1);
	}
	return new ANIMATOR(ANIMATOR::type_resolve(buf), speed, offset, frames, w, h);
}

float get_movator_dif(ANIMATOR * a, int t)
{   float ydif=0;
    if(a->on && (t-a->start)*a->speed/100000 >= a->frames)
    {   a->on=0;
        a->offset=!a->offset;
    }
    if(a->on)
    {   if(a->offset==0) ydif=(float)(t-a->start)*a->speed/100000;
        else ydif=a->frames-(float)(t-a->start)*a->speed/100000;
    }
    else ydif=a->offset*a->frames;
    return ydif;
}

int tile_add_element(TILE * til, string type, int element)
{   unsigned short int typ;
	if(til->len+1 >= MAX_TILE_ELE)
	{	debug("Not enough space for further element in tile");
	    return 0;
	}
	typ = tile_type_resolve(type);
	if(element<Elements.len())	til->elements[til->len] = Elements[element];
	else
	{	debug("Referencing undefined element "+to_str(element),10);
	   exit(1);
	}
	til->types[til->len] = typ;
	debug("tile: "+type+":"+to_str(til->types[til->len])+" "+to_str(til->elements[til->len]->type),3);
	til->len++;
	return 1;
};

TILE * create_tile()
{  TILE * til = new(TILE);
   til->len=0;
   til->types = new short unsigned int [MAX_TILE_ELE];
   til->elements = new TEXTURED_ELEMENT * [MAX_TILE_ELE];
   for(int i=0; i<MAX_TILE_ELE; i++)
//	   til->elements[i]= new TEXTURED_ELEMENT;
        til->elements[i]=NULL;
   return til;
}

unsigned short int tile_type_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
	if(type.compare("TILE_FLOOR")==0) {typ=TILE_FLOOR; type_ok=true;}
	if(type.compare("TILE_CEILING")==0) {typ=TILE_CEILING; type_ok=true;}
	if(type.compare("TILE_FLAT")==0) {typ=TILE_FLAT; type_ok=true;}
	if(type.compare("TILE_FRONT")==0) {typ=TILE_FRONT; type_ok=true;}
	if(type.compare("TILE_SIDE")==0) {typ=TILE_SIDE; type_ok=true;}
	if(type.compare("TILE_STATIC")==0) {typ=TILE_STATIC; type_ok=true;}
	if(type.compare("TILE_STATIC_NS")==0) {typ=TILE_STATIC_NS; type_ok=true;}
	if(type.compare("TILE_STATIC_EW")==0) {typ=TILE_STATIC_EW; type_ok=true;}
	if(type.compare("TILE_STATIC_NS_X")==0) {typ=TILE_STATIC_NS_X; type_ok=true;}
	if(type.compare("TILE_STATIC_EW_X")==0) {typ=TILE_STATIC_EW_X; type_ok=true;}
	if(!type_ok)
	{
		debug("Unknown element type "+type,10);
		exit(1);
	}
	return typ;
}

unsigned short int flip_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
	if(type.compare("NO_FLIP")==0) {typ=NO_FLIP; type_ok=true;}
	if(type.compare("V_FLIP")==0) {typ=V_FLIP; type_ok=true;}
	if(type.compare("H_FLIP")==0) {typ=H_FLIP; type_ok=true;}
	if(type.compare("VH_FLIP")==0) {typ=VH_FLIP; type_ok=true;}
	if(type.compare("TURN_90")==0) {typ=TURN_90; type_ok=true;}
	if(type.compare("TURN_270")==0) {typ=TURN_270; type_ok=true;}
	if(type.compare("V_FLIPPING")==0) {typ=V_FLIPPING; type_ok=true;}
	if(type.compare("H_FLIPPING")==0) {typ=H_FLIPPING; type_ok=true;}
	if(type.compare("VH_FLIPPING")==0) {typ=VH_FLIPPING; type_ok=true;}
	if(type.compare("V_FLIPPING_90")==0) {typ=V_FLIPPING_90; type_ok=true;}
	if(type.compare("H_FLIPPING_90")==0) {typ=H_FLIPPING_90; type_ok=true;}
	if(type.compare("VH_FLIPPING_90")==0) {typ=VH_FLIPPING_90; type_ok=true;}
	if(!type_ok)
	{
		debug("Unknown flip type "+type,10);
		exit(1);
	}
	return typ;
}

LIGHT_SOURCE::LIGHT_SOURCE(int power, int dim, float x, float z)
{	this->power=power;
	this->dim=dim;
	this->x=x;
	this->z=z;
}

string LIGHT_SOURCE::serialize()
{   return to_str(this->power)+" "+to_str(this->dim)+" "+to_str(this->x)+" "+to_str(this->z);
}

LIGHT_SOURCE * load_lightsource(string s)
{	int power, dim;
    float x,z;
	if(sscanf(s.c_str(),"%d %d %f %f",&power,&dim,&x,&z)<4)
	{ debug("Not enough parameters for lightsource :"+s,10);
	  exit(1);
	}
	return new LIGHT_SOURCE(power,dim,x,z);
}

TILE * load_tile(string s)
{	char type[32];
 	string sub;
	int element;
	TILE * tile;
	tile = create_tile();
	while(sscanf(s.c_str(),"%s %d",type,&element)==2)
	{   sub=type;
        sub+=" "+to_str(element)+" ";  //reconstruct read part to measure its length, add a whitespace at the end
        debug("sub: "+sub+" "+to_str((int)sub.size()),3);
        if(sub.size()>s.size()) s="";
        else s=s.substr(sub.size());//remove read part from string
        dappend(" str2: "+s+".",3);
        tile_add_element(tile,type,element);
	}
	debug("return",3);
	return tile;
}

int TRIGGER::type_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
    if(type.compare("TRIGGER_ENTER")==0) {typ=TRIGGER_ENTER; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_LEAVE")==0) {typ=TRIGGER_LEAVE; type_ok=true;}

    if(!type_ok && type.compare("TRIGGER_NORTH")==0) {typ=TRIGGER_NORTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_EAST")==0) {typ=TRIGGER_EAST; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_SOUTH")==0) {typ=TRIGGER_SOUTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_WEST")==0) {typ=TRIGGER_WEST; type_ok=true;}

    if(!type_ok && type.compare("BLOCKER_ENTER")==0) {typ=BLOCKER_ENTER; type_ok=true;}
    if(!type_ok && type.compare("BLOCKER_NORTH")==0) {typ=BLOCKER_NORTH; type_ok=true;}
    if(!type_ok && type.compare("BLOCKER_EAST")==0) {typ=BLOCKER_EAST; type_ok=true;}
    if(!type_ok && type.compare("BLOCKER_SOUTH")==0) {typ=BLOCKER_SOUTH; type_ok=true;}
    if(!type_ok && type.compare("BLOCKER_WEST")==0) {typ=BLOCKER_WEST; type_ok=true;}
	if(!type_ok)
	{
		debug("Unknown trigger type "+type,10);
		exit(1);
	}
	return typ;
}

string TRIGGER::type_string()
{	switch(this->type)
    {
        case TRIGGER_NORTH: return "TRIGGER_NORTH";
        case TRIGGER_EAST: return "TRIGGER_EAST";
        case TRIGGER_SOUTH: return "TRIGGER_SOUTH";
        case TRIGGER_WEST: return "TRIGGER_WEST";

        case TRIGGER_ENTER: return "TRIGGER_ENTER";
        case TRIGGER_LEAVE: return "TRIGGER_LEAVE";

        case BLOCKER_NORTH: return "BLOCKER_NORTH";
        case BLOCKER_EAST: return "BLOCKER_EAST";
        case BLOCKER_SOUTH: return "BLOCKER_SOUTH";
        case BLOCKER_WEST: return "BLOCKER_WEST";
        case BLOCKER_ENTER: return "BLOCKER_ENTER";
    }
    debug("Unknown trigger type number: "+to_str(type),10);
	exit(1);
	return "";
}

TRIGGER * load_trigger(string s)
{	char buf[32],buf2[32];
    int xpos;
    int zpos;
    int w1,h1,w2,h2,anim;
    TRIGGER * ret;

    int params=0;
	if((params=sscanf(s.c_str(),"%s %d %d %d %d %d %d %d %s",buf,&xpos,&zpos,&w1,&h1,&w2,&h2,&anim,buf2))<8)
	{ debug("Not enough parameters loading trigger: "+s,10);
	  exit(1);
	}
	if(params==8) ret = new TRIGGER(TRIGGER::type_resolve(buf),xpos,zpos,w1,h1,w2,h2,anim);
	ret = new TRIGGER(TRIGGER::type_resolve(buf),xpos,zpos,w1,h1,w2,h2,anim,buf2);
	return ret;
}

TRIGGER::TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2,int animator, string action)
{   this->type=type;
    this->xpos=xpos;
    this->zpos=zpos;
    this->h1=h1;
    this->w1=w1;
    this->h2=h2;
    this->w2=w2;
    this->animator_nr=animator;
    if(animator>-1)
    {   if(animator<Animators.len())	this->animator = Animators[animator];
        else
        {	debug("Referencing undefined animator "+to_str(animator),10);
            exit(1);
        }
    }
    else this->animator=NULL;
    if(action=="") this->action=NULL;
    else this->action = new string(action);
}

TRIGGER::TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2,int animator)
{   TRIGGER(type,xpos,zpos,w1,h1,w2,h2,animator,"");
}

string TRIGGER::serialize()
{   string ret="";
    ret+=this->type_string();
    ret+=" "+to_str(this->xpos);
    ret+=" "+to_str(this->zpos);
    ret+=" "+to_str(this->w1);
    ret+=" "+to_str(this->h1);
    ret+=" "+to_str(this->w2);
    ret+=" "+to_str(this->h2);
    ret+=" "+to_str(this->animator_nr);
    ret+=" "+*this->action;
    return ret;
}

void TRIGGER::fire()
{   ANIMATOR * a = this->animator;
    if(a!=NULL)
    {   if(a->type==MOVATOR_Y)
        {   if(!a->on)
            {   a->on=1;
                a->start=tmsec;
            }
        }
        else
        {   a->on=!a->on;
            a->start=tmsec;
            debug("animator toggle "+to_str(a->on)+" "+to_str(a->start));
        }
    }
    debug("Trigger at "+to_str(this->xpos)+" "+to_str(this->zpos)+" fired!");
    if(this->action!=NULL) dappend(" action: "+*this->action);
    STR_LIST * tokens = tokenize(*this->action,"/");
    debug("tokens("+to_str(tokens->len())+"):");
    for(int i=0; i<tokens->len(); i++)
        dappend(" "+to_str(i)+"("+*(*tokens)[i]);

    /* change map within area */
    if(tokens->len()==2 && *(*tokens)[0]=="change_map")
    {   change_map(*(*tokens)[1],this->w1,this->h1);
    }

    /* change area and map */
    if(tokens->len()==3 && *(*tokens)[0]=="change_area")
    {   change_area(*(*tokens)[1],*(*tokens)[2],this->w1,this->h1);
    }

    tokens->clear_all();
    delete tokens;
}
