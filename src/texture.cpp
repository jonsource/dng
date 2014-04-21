/*
 * texture.cpp
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#include "texture.h"
#include "jslib.h"
#include "game_lib.h"
#include "game.h"
#include <string>
#include "stdio.h"

using namespace std;
using namespace jslib;
/**
 * loads texture from file. Separates the string s into filename and extension, and loads filename_close.extension
 * and then _medium and _far. If can't find appropriate texture, falls back to resizing.
 */

TEXTURE * load_texture(string s)
{   PALETTE pal;
    TEXTURE *text;
    string fn,cfn,ext;
    int pos;

    text = new TEXTURE();
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
    text->name=s;
    return text;
}

TEXTURE::TEXTURE()
{   this->close=NULL;
    this->medium=NULL;
    this->far=NULL;
    this->name="none";

}

string TEXTURE::serialize()
{   return this->name;

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

ANIMATOR::ANIMATOR()
{   this->speed=0;
	this->offset=0;
	this->_offset=0;
	this->frames=0;
	this->w=0;
	this->h=0;
	this->frame=NULL;
	this->type=0;
	this->on=0;
	this->start=0;

}

string ANIMATOR::serialize()
{   return "{ \"type\":"+this->type_string()+", \"speed\":"+to_str(speed)+", \"_offset\":"+to_str(_offset)+
", \"frames\":"+to_str(frames)+" \"w\":"+to_str(w)+", \"h\":"+to_str(h)+", \"on\":"+to_str(on)+
", \"start\":"+to_str(start)+", \"mode\":"+to_str(mode)+" }";
}

string ANIMATOR::save_string()
{   return "{ \"offset\":"+to_str(offset)+", \"on\":"+to_str(on)+", \"start\":"+to_str(start)+", \"mode\":"+to_str(mode)+" }";
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

/*
int load_animator_save(ANIMATOR * animator, string s)
{	int offset, on, start, mode;
	if(sscanf(s.c_str(),"%d %d %d %d",&offset,&on,&start,&mode)<4)
	{ debug("Not enough parameters for animator save.",10);
	  exit(1);
	  return 0;
	}
	animator->start=start;
	animator->on=on;
	animator->offset=offset;
	animator->mode=mode;
	return 1;
}*/

int load_animator_save(ANIMATOR * animator, Node * n)
{	animator->start=n->getMember("start")->getInt();
	animator->on=n->getMember("on")->getInt();
	animator->offset=n->getMember("offset")->getInt();
	animator->mode=n->getMember("mode")->getInt();
	return 1;
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

int TILE::add_element(string type, int element)
{   unsigned short int typ;
    typ = TILE::type_resolve(type);
    if(typ>=TILE_STATIC) return this->add_static_subr(typ,element);
    else
    {   if(typ==TILE_FRONT) this->block_sight=1;
        return this->add_element_subr(typ,element);
    }
}

int TILE::add_element_subr(unsigned short int typ, int element)
{   if(this->elements_len+1 >= MAX_TILE_ELE)
	{	debug("Not enough space for further element in tile",10);
	    return 0;
	}
	if(element<Game->Elements.len())
    {   this->elements[this->elements_len] = Game->Elements[element];
        this->element_nrs[this->elements_len] = element;
    }
	else
	{	debug("Referencing undefined element "+to_str(element),10);
	   exit(1);
	}
	this->element_types[this->elements_len] = typ;
	debug("tile: "+TILE::type_string(typ)+":"+to_str(this->element_types[this->elements_len])+" "+to_str(this->elements[this->elements_len]->type),3);
	this->elements_len++;
	return 1;
};

int TILE::add_static_subr(unsigned short int typ, int stat)
{   if(this->statics_len+1 >= MAX_TILE_ELE)
	{	debug("Not enough space for further static in tile",10);
	    return 0;
	}
	if(stat<Game->Elements.len())
    {   this->statics[this->statics_len] = Game->Elements[stat];
        this->static_nrs[this->statics_len] = stat;
    }
	else
	{	debug("Referencing undefined static "+to_str(stat),10);
	   exit(1);
	}
	this->static_types[this->statics_len] = typ;
	debug("tile: "+TILE::type_string(typ)+":"+to_str(this->static_types[this->statics_len])+" "+to_str(this->statics[this->statics_len]->type),3);
	this->statics_len++;
	return 1;
};

TILE::TILE()
{  this->elements_len=0;
   this->statics_len=0;
   this->block_sight=0;
   this->element_types = new short unsigned int [MAX_TILE_ELE];
   this->static_types = new short unsigned int [MAX_TILE_ELE];
   this->element_nrs = new int [MAX_TILE_ELE];
   this->static_nrs = new int [MAX_TILE_ELE];
   this->elements = new TEXTURED_ELEMENT * [MAX_TILE_ELE];
   this->statics = new TEXTURED_ELEMENT * [MAX_TILE_ELE];
   for(int i=0; i<MAX_TILE_ELE; i++)
//	   til->elements[i]= new TEXTURED_ELEMENT;
        this->elements[i]=NULL;
}

unsigned short int TILE::type_resolve(string type)
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

string TILE::type_string(int type)
{   switch(type)
    {   case TILE_FLOOR: return "TILE_FLOOR";
        case TILE_CEILING: return "TILE_CEILING";
        case TILE_FLAT: return "TILE_FLAT";
        case TILE_FRONT: return "TILE_FRONT";
        case TILE_SIDE: return "TILE_SIDE";
        case TILE_STATIC: return "TILE_STATIC";
        case TILE_STATIC_NS: return "TILE_STATIC_NS";
        case TILE_STATIC_EW: return "TILE_STATIC_EW";
        case TILE_STATIC_NS_X: return "TILE_STATIC_NS_X";
        case TILE_STATIC_EW_X: return "TILE_STATIC_EW_X";
    }
    debug("Unknown tile type number: "+to_str(type),10);
	exit(1);
	return "";

}

TILE * load_tile(string s)
{	char type[32];
 	string sub;
	int element;
	TILE * tile;
	tile = new TILE();
	while(sscanf(s.c_str(),"%s %d",type,&element)==2)
	{   sub=type;
        sub+=" "+to_str(element)+" ";  //reconstruct read part to measure its length, add a whitespace at the end
        debug("sub: "+sub+" "+to_str((int)sub.size()),3);
        if(sub.size()>s.size()) s="";
        else s=s.substr(sub.size());//remove read part from string
        dappend(" str2: "+s+".",3);
        tile->add_element(type,element);
	}
	debug("return",3);
	return tile;
}

string TILE::serialize()
{   string ret="";
    for(int i=0; i < this->elements_len; i++)
    {   if(i>0) ret+=" ";
        if(elements[i]!=NULL) ret+=TILE::type_string(this->element_types[i])+" "+to_str(this->element_nrs[i]);
    }
    for(int i=0; i < this->statics_len; i++)
    {   if(i>0) ret+=" ";
        if(statics[i]!=NULL) ret+=TILE::type_string(this->static_types[i])+" "+to_str(this->static_nrs[i]);
    }
    return ret;
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

string flip_to_str(int type)
{   switch(type)
    {
        case NO_FLIP : return "NO_FLIP";
        case V_FLIP : return "V_FLIP";
        case H_FLIP : return "H_FLIP";
        case VH_FLIP : return "VH_FLIP";
        case TURN_90 : return "TURN_90";
        case TURN_270 : return "TURN_270";
        case V_FLIPPING : return "V_FLIPPING";
        case H_FLIPPING : return "H_FLIPPING";
        case VH_FLIPPING : return "VH_FLIPPING";
        case V_FLIPPING_90 : return "V_FLIPPING_90";
        case H_FLIPPING_90 : return "H_FLIPPING_90";
        case VH_FLIPPING_90 : return "VH_FLIPPING_90";
    }
    debug("Unknown flip type number: "+to_str(type),10);
	exit(1);
	return "";
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

SPRITE::SPRITE()
{
    this->sprite = NULL;
}

SPRITE::~SPRITE()
{
    destroy_bitmap(this->sprite);
    this->Modes.clear_all();
}

string clip_to_str(bool clip)
{
    if(clip) return "clip";
    return "no-clip";
}

string trans_to_str(bool trans)
{
    if(trans) return "trans";
    return "no-trans";
}
