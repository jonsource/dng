/*
 * texture.cpp
 *
 *  Created on: 3.3.2013
 *      Author: tzeentch
 */

#include "texture.h"
#include "graphic.h"
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

TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator, string clip)
{	//char buf[60];
	TEXTURED_ELEMENT * ele = new(TEXTURED_ELEMENT);
	unsigned short int typ;
   ele->x = x;
   ele->y = y;
   ele->z = z;
   ele->w = w;
   ele->h = h;
   ele->transparent = to_bool(transparent);
   ele->clip = to_bool(clip);
   if(texture>=Textures.len())
   { debug("Referencing undefined texture "+to_str(texture),10);
   	 ele->texture = Textures[0];
   }
   else ele->texture = Textures[texture];
   ele->transparent = to_bool(transparent);
   if(animator>-1)
   {   if(animator<Animators.len())	ele->animator = Animators[animator];
   	   else
   	   {	debug("Referencing undefined animator "+to_str(animator),10);
   	   	   exit(1);
   	   }
   }
   else ele->animator = NULL;
   typ=tile_type_resolve(type);
   ele->type=typ;
   //debug("Element "+type);
   return ele;
}

TEXTURED_ELEMENT * load_element(string s)
{	char type[32], transparent[32], clip[32];
	float x,y,z,w,h;
	int texture,animator;
	if(sscanf(s.c_str(),"%s %f %f %f %f %f %s %d %d %s",type,&x,&y,&z,&w,&h,transparent,&texture,&animator,clip)<10)
	{ debug("Not enough parameters for textured element.",10);
	  exit(1);
	}
	return create_element(type,x,y,z,w,h,transparent,texture,animator,clip);
}

unsigned short int animator_type_resolve(string type)
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

ANIMATOR * create_animator(int type, int speed, int offset, int frames, int w, int h)
{ ANIMATOR * ani = new(ANIMATOR);
	ani->speed=speed;
	ani->offset=offset;
	ani->frames=frames;
	ani->frame=create_bitmap(w,h);
	ani->type=type;
	if(type==ANIMATOR_ON) ani->on=1;
	else ani->on=0;
	ani->start=0;
  return ani;
}

ANIMATOR * load_animator(string s)
{	char buf[32];
    int speed, offset, frames, w, h;
	if(sscanf(s.c_str(),"%s %d %d %d %d %d",buf,&speed,&offset,&frames,&w,&h)<6)
	{ debug("Not enough parameters for animator.",10);
	  exit(1);
	}
	return create_animator(animator_type_resolve(buf), speed, offset, frames, w, h);
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
	debug("tile: "+type+":"+to_str(til->types[til->len])+" "+to_str(til->elements[til->len]->type),4);
	til->len++;
	return 1;
};

TILE * create_tile()
{  TILE * til = new(TILE);
   til->len=0;
   til->types = (short unsigned int *) malloc(MAX_TILE_ELE*sizeof(short unsigned int));
   til->elements = (TEXTURED_ELEMENT **) malloc(MAX_TILE_ELE*sizeof(TEXTURED_ELEMENT *));
   for(int i=0; i<MAX_TILE_ELE; i++)
	   til->elements[i]=(TEXTURED_ELEMENT *) malloc(sizeof(TEXTURED_ELEMENT));
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

LIGHT_SOURCE * create_lightsource(int power, int dim, int x, int z)
{	LIGHT_SOURCE * ls;
	ls=new(LIGHT_SOURCE);
	ls->power=power;
	ls->dim=dim;
	ls->x=x;
	ls->z=z;
	return ls;
}

LIGHT_SOURCE * load_lightsource(string s)
{	int power, dim, x, z;
	if(sscanf(s.c_str(),"%d %d %d %d",&power,&dim,&x,&z)<4)
	{ debug("Not enough parameters for lightsource.",10);
	  exit(1);
	}
	return create_lightsource(power,dim,x,z);
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
        dappend(" str2: "+s+".");
        tile_add_element(tile,type,element);
	}
	debug("return",3);
	return tile;
}

/*
TRIGGER * create_trigger(int speed, int offset, int frames, int w, int h)
{ ANIMATOR * ani = new(ANIMATOR);
	ani->speed=speed;
	ani->offset=offset;
	ani->frames=frames;
	ani->frame=create_bitmap(w,h);
  return ani;
}*/
unsigned short int trigger_type_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
    if(type.compare("TRIGGER_ENTER")==0) {typ=TRIGGER_ENTER; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_LEAVE")==0) {typ=TRIGGER_LEAVE; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_NORTH")==0) {typ=TRIGGER_NORTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_EAST")==0) {typ=TRIGGER_EAST; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_SOUTH")==0) {typ=TRIGGER_SOUTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_WEST")==0) {typ=TRIGGER_WEST; type_ok=true;}
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

TRIGGER * load_trigger(string s)
{	char buf[30];
    int xpos;
    int zpos;
    int w1,h1,w2,h2,anim;
	if(sscanf(s.c_str(),"%s %d %d %d %d %d %d %d",buf,&xpos,&zpos,&w1,&h1,&w2,&h2,&anim)<8)
	{ debug("Not enough parameters loading trigger: "+s,10);
	  exit(1);
	}
	TRIGGER * ret = new TRIGGER(trigger_type_resolve(buf),xpos,zpos,w1,h1,w2,h2,anim);
	return ret;
}

TRIGGER::TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2,int animator)
{   this->type=type;
    this->xpos=xpos;
    this->zpos=zpos;
    this->h1=h1;
    this->w1=w1;
    this->h2=h2;
    this->w2=w2;
    if(animator>-1)
    {   if(animator<Animators.len())	this->animator = Animators[animator];
        else
        {	debug("Referencing undefined animator "+to_str(animator),10);
            exit(1);
        }
    }
    else this->animator=NULL;
}

void TRIGGER::fire()
{   if(this->animator!=NULL)
    {   this->animator->on=!this->animator->on;
        this->animator->start=tmsec;
        debug("animator toggle "+to_str(this->animator->on)+" "+to_str(this->animator->start));
    }
    debug("Trigger at "+to_str(this->xpos)+" "+to_str(this->zpos)+" fired!");

}
