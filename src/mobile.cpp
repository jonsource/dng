#include "allegro.h"
#include "mobile.h"
#include "game_lib.h"
#include "game.h"

extern int gz,gx;

SPRITE::SPRITE()
{
    this->sprite = NULL;
}

SPRITE::~SPRITE()
{
    destroy_bitmap(this->sprite);
    this->Modes.clear_all();
}

MOBILE_TEMPLATE::MOBILE_TEMPLATE()
{   this->sprite = new SPRITE();
    this->fname = "none";
    this->speed=100;
}

MOBILE_TEMPLATE::~MOBILE_TEMPLATE()
{   this->sprite->Modes.clear_all();
    destroy_bitmap(this->sprite->sprite);
    delete this->sprite;
}

MOBILE * MOBILE_TEMPLATE::Clone()
{   MOBILE * ret = new MOBILE();
    ret->parent = this;
    ret->speed = this->speed;
    return ret;
}

MOBILE * MOBILE_TEMPLATE::CloneAt(int x, int z)
{   MOBILE * ret = new MOBILE();
    ret->parent = this;
    ret->speed = this->speed;
    ret->x = x;
    ret->z = z;
    return ret;
}

MOBILE::MOBILE()
{   this->x=0;
    this->y=0;
    this->z=0;
    this->heading=0;
    this->parent = NULL;
    this->act_progress=0;
    this->act_target=100;
    this->speed=100;
    this->ani = new ANIMATOR(ANIMATOR_MOBILE,0,0,0,128,128);
    this->ele = new TEXTURED_ELEMENT("TILE_STATIC",0.5,0,0.5,0.8,0.8,"no-trans",-1,-1,"no-clip","NO_FLIP");
    this->ele->animator = this->ani;
    this->action = ACT_DECIDE;
    this->mode = MODE_HUNT;
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
    if(this->next_action==ACT_DECIDE) this->actionDecide();
    this->action = this->next_action;
    this->next_action = ACT_DECIDE;
    debug(" ["+to_str(this->x)+","+to_str(this->z)+"]"+" action changed to :"+to_str(this->action),5);
    if(this->action>=ACT_GO_NORTH && this->action<=ACT_GO_WEST)
    {
        this->act_target = 3000;
        this->heading = this->action;
    }
    if(this->action==ACT_FIGHT)
    {
        this->act_target = 3000;
    }
    this->flag_pass=0;
    debug("finish action, next action :"+to_str(this->next_action),1);
}

void MOBILE::actionGo()
{
    int x,z;
    heading_to_xz(this->heading,&x,&z);
    float perc = this->act_progress/(float)this->act_target;
    if(perc>0.5)
    {
        if(!this->flag_pass)
        {
            this->x+=x;
            this->z+=z;
            this->flag_pass=1;
        }
        perc=perc-1;
    }
    debug("action Go");
    this->ele->x = 0.5 + perc * x;
    this->ele->z = 0.5 + perc * z;
    debug("action Go, progress :"+to_str(this->act_progress)+" ["+to_str(this->x)+","+to_str(this->x)+"]",1);
}
void MOBILE::actionDecide()
{
    this->actionDecide(MODE_NO_OVERRIDE);
}

void MOBILE::actionDecide(int mode_override)
{
    int x,z;
    int i = rand()%4;
    int dir;
    int _mode;

    if(mode_override == -1) _mode = this->mode;
    else _mode = mode_override;

    debug(" mob mode:"+to_str(_mode),5);
    if(_mode == MODE_FIGHT)
    {
        this->next_action = ACT_FIGHT;
        return;
    }

    if(_mode == MODE_HUNT)
    {
        if(dist2(this->x,this->z,gx,gz) < 1) //gotcha, let's fight
        {
            debug("player here - fight!",5);
            this->actionDecide(MODE_FIGHT);
            return;
        }
        if(dist2(this->x,this->z,gx,gz) > 100) //player too fat, wander
        {
            debug("player too far - wander",5);
            this->actionDecide(MODE_WANDER);
            return;
        }
        if(dist2(this->x,0,gx,0) > dist2(0,this->z,0,gz)) // reduce X coordinate
        {
            if(gx>this->x) this->next_action = ACT_GO_EAST;
            else this->next_action = ACT_GO_WEST;

        }
        else // reduce Z coordinate
        {
            if(gz>this->z) this->next_action = ACT_GO_SOUTH;
            else this->next_action = ACT_GO_NORTH;
        }
        debug("player close - hunt : "+heading_to_str(this->next_action)+"["+to_str(this->x)+","+to_str(this->z)+"]->("+to_str(dist2(this->x,0,gx,0))+","+to_str(dist2(0,this->z,0,gz))+")->["+to_str(gx)+","+to_str(gz)+"]",5);

    }
    if(_mode == MODE_WANDER)
    {
        if(rand()%3)
        {
            heading_to_xz(this->heading,&x,&z);
            if(is_passable(this->x+x,this->z+z))
            {
                this->next_action = ACT_GO_NORTH+this->heading;
                return;
            }
        }

        for(dir=i; dir<i+4; dir++)
        {
            heading_to_xz(dir%4,&x,&z);
            if(is_passable(this->x+x,this->z+z)) break;
        }
        this->next_action = ACT_GO_NORTH+dir%4;
    }

    //debug("action Decide, next action :"+to_str(this->next_action),5);
}

int MOBILE::HeartBeat()
{
    this->act_progress += this->speed/2;
    if(this->act_progress >= this->act_target)
    {
        this->act_progress -= this->act_target;
        this->finishAction();
    }
    switch(action)
    {
    case ACT_GO_NORTH:
    case ACT_GO_EAST:
    case ACT_GO_SOUTH:
    case ACT_GO_WEST:
        this->actionGo();
        break;
    }

    debug(" succesfull end heart beat",1);
    return 1;
}

SPRITE_MODE * load_sprite_mode(string s)
{
    SPRITE_MODE * ret = new SPRITE_MODE;
    if(sscanf(s.c_str(),"%d %d %d %d",&ret->start_y,&ret->width,&ret->height,&ret->frames)<4)
    {
        debug("Not enough parameters for sprite_mode: "+s,10);
        exit(1);
    }
    return ret;
}
