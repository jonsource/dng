#include "mobile.h"
#include "game_lib.h"
#include "game.h"

MOBILE::MOBILE()
{
    this->x=0;
    this->y=0;
    this->z=0;
    this->mode = MODE_WANDER;
    this->sprite = NULL;
    this->spr_w=0;
    this->spr_h=0;
    this->act_progress=0;
    this->act_target=100;
    this->speed=100;
    this->ani = new ANIMATOR(ANIMATOR_MOBILE,0,0,0,128,128);
    this->ele = new TEXTURED_ELEMENT("TILE_STATIC",0,0,0.5,1,1,"no-trans",13,-1,"no-clip","NO_FLIP");
    this->ele->texture=NULL;
    this->ele->animator = this->ani;
    this->action = ACT_DECIDE;
    this->mode = MODE_WANDER;
    this->flag_pass = 0;
}

MOBILE::~MOBILE()
{
    delete this->ani;
    delete this->ele;
}

void MOBILE::finishAction()
{
    this->last_action = this->action;
    this->actionDecide();
    this->action = this->next_action;
    debug(" ["+to_str(this->x)+","+to_str(this->z)+"]"+" action changed tp :"+to_str(this->action),5);
    if(this->action>=ACT_GO_NORTH && this->action<=ACT_GO_WEST)
    {   this->act_target = 4000;
        this->heading = this->action;
    }
    this->flag_pass=0;
    //debug("finish action, next action :"+to_str(this->next_action),5);
}

void MOBILE::actionGo()
{   int x,z;
    heading_to_xz(this->heading,&x,&z);
    float perc = this->act_progress/(float)this->act_target;
    if(perc>0.5)
    {   if(!this->flag_pass)
        {   this->x+=x;
            this->z+=z;
            this->flag_pass=1;
        }
        perc=perc-1;
    }
    this->ele->x = 0.5 + perc * x;
    this->ele->z = 0.5 + perc * z;
    //debug("action Go, progress :"+to_str(this->act_progress)+" ["+to_str(this->x)+","+to_str(this->x)+"]",5);
}

void MOBILE::actionDecide()
{   int x,z;
    int i = rand()%4;
    int dir;

    if(rand()%3)
    {   heading_to_xz(this->heading,&x,&z);
        if(is_passable(this->x+x,this->z+z))
        {
            this->next_action = ACT_GO_NORTH+this->heading;
            return;
        }
    }

    for(dir=i; dir<i+4; dir++)
    {   heading_to_xz(dir%4,&x,&z);
        if(is_passable(this->x+x,this->z+z)) break;
    }
    this->next_action = ACT_GO_NORTH+dir%4;
    //debug("action Decide, next action :"+to_str(this->next_action),5);
}

int MOBILE::HeartBeat()
{
    this->act_progress += this->speed/2;
    if(this->act_progress >= this->act_target)
    {   this->act_progress -= this->act_target;
        this->finishAction();
    }
    switch(action)
    {
        case ACT_GO_NORTH:
        case ACT_GO_EAST:
        case ACT_GO_SOUTH:
        case ACT_GO_WEST:   this->actionGo();
                            break;
    }

    return 1;
}
