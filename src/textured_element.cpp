#include "textured_element.h"
#include "jslib.h"
#include "game_lib.h"
#include "game.h"

using namespace std;
using namespace jslib;

TEXTURED_ELEMENT::TEXTURED_ELEMENT(string type, float x, float y, float z, float w, float h, string transparent, int texture, int animator, string clip, string flip)
{   unsigned short int typ;
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    this->h = h;
    this->transparent = to_bool(transparent);
    this->clip = to_bool(clip);
    if(texture==-1)
    {   this->texture = NULL;
    }
    else
    {   if(texture>=Game->Textures.len())
        {   debug("Referencing undefined texture "+to_str(texture),10);
            this->texture = Game->Textures[0];
        }
        else this->texture = Game->Textures[texture];
    }
    this->texture_nr = texture;
    this->transparent = to_bool(transparent);
    if(animator>-1)
    {   if(animator<Game->Animators.len())	this->animator = Game->Animators[animator];
       else
       {	debug("Referencing undefined animator "+to_str(animator),10);
           exit(1);
       }
    }
    else this->animator = NULL;
    this->animator_nr = animator;
    typ=TILE::type_resolve(type);
    this->type=typ;
    this->flip=flip_resolve(flip);
    //debug("Element "+type);
}

string TEXTURED_ELEMENT::serialize()
{   return TILE::type_string(type)+" "+to_str(x)+" "+to_str(y)+" "+to_str(z)+" "+to_str(w)+" "+to_str(h)+" "+trans_to_str(transparent)+" "+to_str(texture_nr)+" "+to_str(animator_nr)+" "+clip_to_str(clip)+" "+flip_to_str(flip);
}

