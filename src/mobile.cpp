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
}
