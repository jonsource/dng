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

TEXTURE ** Textures;
int Textures_len = 0;
int Textures_max = -1;

TEXTURED_ELEMENT ** Elements;
int Elements_len = 0;
int Elements_max = -1;

ANIMATOR ** Animators;
int Animators_len = 0;
int Animators_max = -1;

TILE ** Tiles;
int Tiles_len = 0;
int Tiles_max = -1;

void unload_textures()
{	delete []Textures;
}

int add_texture(TEXTURE * txt)
{	if(Textures_max+1 >= Textures_len)
	{	debug("Expanding Textures from: "+to_str(Textures_len),2);
		TEXTURE ** re = (TEXTURE **) malloc((Textures_len?(Textures_len*2):4)*sizeof(TEXTURE *));
		for(int i=0;i<Textures_len; i++) re[i]=Textures[i];
		delete []Textures;
		Textures = re;
		Textures_len=(Textures_len?(Textures_len*2):4);
		debug(" ... to: "+to_str(Textures_len),2);
	}
	debug("Adding texture as #"+to_str(Textures_max));
	Textures[Textures_max+1]=txt;
	dappend(" - texture added",1);
	Textures_max++;
	return 1;
}

TEXTURE * load_texture(string s)
{   PALETTE pal;
    TEXTURE *text;
    string fn,cfn,ext;
    char buf[30];
    int pos;

    text = new (TEXTURE);
    fn.assign("data/images/");

    pos = s.rfind(".");
    fn.append(s.substr(0,pos));
    ext = s.substr(pos+1);

    cfn.assign(fn);
    cfn.append("_close."+ext);
    text->close = load_bitmap(cfn.c_str(), pal);
    if(!text->close) debug("Missing texture "+cfn);
    else debug(cfn+" texture depth "+to_str(bitmap_color_depth(text->close)),3);
    cfn.assign(fn);
    cfn.append("_medium."+ext);
    text->medium = load_bitmap(cfn.c_str(), pal);
    if(!text->medium)
    { 	debug("Missing texture "+cfn,2);
    	if(text->close)
    	{ dappend(" - resizing close texture",3);
    	  text->medium = create_bitmap(128,128);
    	  stretch_blit(text->close,text->medium,0,0,text->close->w,text->close->h,0,0,text->medium->w,text->medium->h);
    	}
    }
    cfn.assign(fn);
    cfn.append("_far."+ext);
    text->far = load_bitmap(cfn.c_str(), pal);
    if(!text->far)
    { 	debug("Missing texture "+cfn,2);
		if(text->medium)
    	{ dappend(" - resizing medium texture",3);
    	  text->far = create_bitmap(64,64);
    	  stretch_blit(text->medium,text->far,0,0,text->medium->w,text->medium->h,0,0,text->far->w,text->far->h);
    	}
    }
    return text;
}

void unload_elements()
{	delete []Elements;
}

int add_element(TEXTURED_ELEMENT * ele)
{	if(Elements_max+1 >= Elements_len)
	{	debug("Expanding Elements from: "+to_str(Elements_len),2);
		TEXTURED_ELEMENT ** re = (TEXTURED_ELEMENT **) malloc((Elements_len?(Elements_len*2):4)*sizeof(TEXTURED_ELEMENT *));
		for(int i=0;i<Elements_len; i++) re[i]=Elements[i];
		delete []Elements;
		Elements = re;
		Elements_len=(Elements_len?(Elements_len*2):4);
		debug(" ... to: "+to_str(Elements_len),1);
	}
	debug("Adding element as #"+to_str(Elements_max));
	Elements[Elements_max+1]=ele;
	Elements_max++;
	dappend(" - element added",2);
	return 1;
}

TEXTURED_ELEMENT * create_element(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator)
{	//char buf[60];
	TEXTURED_ELEMENT * ele = new(TEXTURED_ELEMENT);
	unsigned short int typ;
   ele->x = x;
   ele->y = y;
   ele->z = z;
   ele->w = w;
   ele->h = h;
   ele->transparent = to_bool(transparent);
   if(texture>Textures_max)
   { debug("Referencing undefined texture "+to_str(texture),10);
   	 ele->texture = Textures[0];
   }
   else ele->texture = Textures[texture];
   ele->transparent = to_bool(transparent);
   if(animator>-1)
   {   if(animator<=Animators_max)	ele->animator = Animators[animator];
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

void unload_animators()
{	delete []Animators;
}

int add_animator(ANIMATOR * txt)
{	if(Animators_max+1 >= Animators_len)
	{	debug("Expanding Animators from: "+to_str(Animators_len),2);
		ANIMATOR ** re = (ANIMATOR **) malloc((Animators_len?(Animators_len*2):4)*sizeof(ANIMATOR *));
		for(int i=0;i<Animators_len; i++) re[i]=Animators[i];
		delete []Animators;
		Animators = re;
		Animators_len=(Animators_len?(Animators_len*2):4);
		debug(" ... to: "+to_str(Animators_len),1);
	}
	debug("Adding animator as #"+to_str(Animators_max));
	Animators[Animators_max+1]=txt;
	Animators_max++;
	dappend(" - animator added",2);
	return 1;
}

ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h)
{ ANIMATOR * ani = new(ANIMATOR);
	ani->speed=speed;
	ani->offset=offset;
	ani->frames=frames;
	ani->frame=create_bitmap(w,h);
  return ani;
}

void unload_tiles()
{	delete []Tiles;
}

int add_tile(TILE * til)
{	if(Tiles_max+1 >= Tiles_len)
	{	debug("Expanding Tiles from: "+to_str(Tiles_len),2);
		TILE ** re = (TILE **) malloc((Tiles_len?(Tiles_len*2):4)*sizeof(TILE *));
		for(int i=0;i<Tiles_len; i++) re[i]=Tiles[i];
		delete []Tiles;
		Tiles = re;
		Tiles_len=(Tiles_len?(Tiles_len*2):4);
		debug(" ... to: "+to_str(Tiles_len),1);
	}
	debug("Adding tile as #"+to_str(Tiles_max));
	Tiles[Tiles_max+1]=til;
	Tiles_max++;
	dappend(" - tile added",2);
	dappend(": "+to_str(Tiles[Tiles_max]->types[Tiles[Tiles_max]->len])+" "+to_str(Tiles[Tiles_max]->elements[Tiles[Tiles_max]->len]->type),2);
	return 1;
}

int tile_add_element(TILE * til, string type, int element)
{   unsigned short int typ;
	if(til->len+1 >= MAX_TILE_ELE)
	{	debug("Not enough space for further element in tile");
	    return 0;
	}
	typ = tile_type_resolve(type);
	if(element<=Elements_max)	til->elements[til->len] = Elements[element];
	else
	{	debug("Referencing undefined element "+to_str(element),10);
	   exit(1);
	}
	til->types[til->len] = typ;
	til->len++;
	debug("tile: "+type+":"+to_str(til->types[til->len])+" "+to_str(til->elements[til->len]->type),4);
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
	if(!type_ok)
	{
		debug("Unknown element type "+type,3);
		exit(1);
	}
	return typ;
}
