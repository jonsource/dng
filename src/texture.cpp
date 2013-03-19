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

ANIMATOR * create_animator(int speed, int offset, int frames, int w, int h)
{ ANIMATOR * ani = new(ANIMATOR);
	ani->speed=speed;
	ani->offset=offset;
	ani->frames=frames;
	ani->frame=create_bitmap(w,h);
  return ani;
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
	if(!type_ok)
	{
		debug("Unknown element type "+type,3);
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
	{ debug("Not enough parameters.");
	  exit(1);
	}
	return create_lightsource(power,dim,x,z);
}

ANIMATOR * load_animator(string s)
{	int speed, offset, frames, w, h;
	if(sscanf(s.c_str(),"%d %d %d %d %d",&speed,&offset,&frames,&w,&h)<5)
	{ debug("Not enough parameters.");
	  exit(1);
	}
	return create_animator(speed, offset, frames, w, h);
}

TEXTURED_ELEMENT * load_element(string s)
{	char type[32], transparent[32];
	float x,y,z,w,h;
	int texture,animator;
	if(sscanf(s.c_str(),"%s %f %f %f %f %f %s %d %d",type,&x,&y,&z,&w,&h,transparent,&texture,&animator)<9)
	{ debug("Not enough parameters.");
	  exit(1);
	}
	return create_element(type,x,y,z,w,h,transparent,texture,animator);
}

TILE * load_tile(string s)
{	char type[32];
 	string sub;
	int element;
	TILE * tile;
	tile = create_tile();
	while(sscanf(s.c_str(),"%s %d",type,&element)==2)
	{ sub=type;
	  sub+=" "+to_str(element)+" ";  //reconstruct read part to measure its length, add a whitespace at the end
	  s=s.substr(sub.size()); //remove read part from string
	  debug("sub: "+sub+" "+to_str((int)sub.size())+" str2: "+s,1);
	  tile_add_element(tile,type,element);
	}
	return tile;
}
