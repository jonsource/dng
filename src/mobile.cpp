#include "mobile.h"

MOBILE::MOBILE()
{
    this->x=0;
    this->y=0;
    this->z=0;
    this->mode = STAND;
    this->sprite = NULL;
    this->spr_w=0;
    this->spr_h=0;
    this->progress=0;
    this->speed=100;
    this->ani = new ANIMATOR(ANIMATOR_MOBILE,0,0,0,128,128);
    this->ele = new TEXTURED_ELEMENT("TILE_STATIC",0,0,0.5,1,1,"no-trans",13,-1,"no-clip","NO_FLIP");
    this->ele->texture=NULL;
    this->ele->animator = this->ani;
}

MOBILE::~MOBILE()
{
    delete this->ani;
    delete this->ele;
}
