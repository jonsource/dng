#include "allegro.h"
#include "jslib.h"
#include "libjsjson.h"
#include "mobile.h"
#include "game_lib.h"
#include "game_lib_load.h"
#include "game.h"
#include "pc_slot.h"
#include "character.h"

using namespace jslib;

MOBILE_TEMPLATE::MOBILE_TEMPLATE()
{   this->sprite = new SPRITE();
    this->fname = "none";
    this->speed=100;
    this->template_nr = -1;
}

MOBILE_TEMPLATE::~MOBILE_TEMPLATE()
{   delete this->sprite;
}

int MOBILE_TEMPLATE::ResetClone(MOBILE * mob)
{   mob->parent = this;
    mob->speed = this->speed;
    return 0;
}

MOBILE * MOBILE_TEMPLATE::Clone()
{   MOBILE * ret = new MOBILE();
    this->ResetClone(ret);
    return ret;
}

MOBILE * MOBILE_TEMPLATE::CloneAt(int x, int z)
{   MOBILE * ret = new MOBILE();
    this->ResetClone(ret);
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
    debug("NPC finish action",5);

    /* TODO: finish combat system */
    if(this->action == ACT_FIGHT)
    {   if(this->x==Game->x && this->z==Game->z)
        {
            Game->PcSlots[0].character->HP--;

        }
    }

    this->last_action = this->action;
    if(this->next_action==ACT_DECIDE) this->actionDecide();
    this->action = this->next_action;
    this->next_action = ACT_DECIDE;
    //debug("NPC ["+to_str(this->x)+","+to_str(this->z)+"]"+" action changed to :"+to_str(this->action),5);
    if(this->action>=ACT_GO_NORTH && this->action<=ACT_GO_WEST)
    {
        this->act_target = 3000;
        this->heading = this->action;
    }
    if(this->action==ACT_FIGHT)
    {
        this->act_target = 3000;
    }
    if(this->action==ACT_STAND)
    {
        this->act_target = 5000;
    }
    this->flag_pass=0;
    debug("NPC finish action, next action :"+to_str(this->next_action)+"\n",5);
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
        if(dist2(this->x,this->z,Game->x,Game->z) < 1) //gotcha, let's fight
        {
            debug("player here - fight!",5);
            this->actionDecide(MODE_FIGHT);
            return;
        }
        if(dist2(this->x,this->z,Game->x,Game->z) > 100) //player too far, wander
        {
            debug("player too far - wander",5);
            this->actionDecide(MODE_WANDER);
            return;
        }
        if(dist2(this->x,0,Game->x,0) > dist2(0,this->z,0,Game->z)) // reduce X coordinate
        {
            if(Game->x>this->x) this->next_action = ACT_GO_EAST;
            else this->next_action = ACT_GO_WEST;

        }
        else // reduce Z coordinate
        {
            if(Game->z>this->z) this->next_action = ACT_GO_SOUTH;
            else this->next_action = ACT_GO_NORTH;
        }
        /* check whether can leave and enter, else wander */
        heading_to_xz(this->next_action,&x,&z);
        if(!can_enter(this->x+x,this->z+z,(this->next_action+2)%4) || !can_leave(this->x,this->z,this->next_action))
        {   debug("can't reach directly - wander",5);
            this->actionDecide(MODE_WANDER);
            return;
        }
        debug("player close - hunt : "+heading_to_str(this->next_action)+"["+to_str(this->x)+","+to_str(this->z)+"]->("+to_str(dist2(this->x,0,Game->x,0))+","+to_str(dist2(0,this->z,0,Game->z))+")->["+to_str(Game->x)+","+to_str(Game->z)+"]",5);

    }
    if(_mode == MODE_WANDER)
    {
        if(rand()%3) //continue walking forward
        {
            heading_to_xz(this->heading,&x,&z);
            if(can_enter(this->x+x,this->z+z,(this->heading+2)%4) && can_leave(this->x,this->z,this->heading))
            {
                this->next_action = ACT_GO_NORTH+this->heading;
                return;
            }
        }

        int dir_set=0;
        for(dir=i; dir<i+4; dir++)
        {
            heading_to_xz(dir%4,&x,&z);
            if(can_enter(this->x+x,this->z+z,(dir+2)%4) && can_leave(this->x,this->z,dir%4))
            {   dir_set=1;
                break;
            }
        }
        if(dir_set) this->next_action = ACT_GO_NORTH+dir%4; // can go somewhere
        else this->next_action = ACT_STAND;
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

std::string MOBILE::serialize()
{   if(this->parent->template_nr==-1)
    {   debug("ERROR: Unindexed parent template when serializing "+this->parent->fname,10);
    }
    std::string ret="{ \"template\":\""+to_str(this->parent->fname)+"\", \"template_nr\":"+to_str(this->parent->template_nr)+", \"x\":"+to_str(this->x)+", \"y\":"+to_str(this->y)+", \"z\":"+to_str(this->z)+
    ", \"heading\":"+to_str(this->heading)+", \"action\":"+to_str(this->action)+", \"act_progress\":"+to_str(this->act_progress)+
    ", \"act_target\":"+to_str(this->act_target)+", \"flag_pass\":"+to_str(this->flag_pass)+
    ", \"last_action\":"+to_str(this->last_action)+", \"next_action\":"+to_str(this->next_action)+" }";
    return ret;
}

std::string MOBILE::save_string()
{
    return this->serialize();
}

/*
SPRITE_MODE * load_sprite_mode(string s)
{
    SPRITE_MODE * ret = new SPRITE_MODE;
    if(sscanf(s.c_str(),"%d %d %d %d",&ret->start_y,&ret->width,&ret->height,&ret->frames)<4)
    {
        debug("Not enough parameters for sprite_mode: "+s,10);
        exit(1);
    }
    return ret;
}*/

MOBILE_TEMPLATE* load_mobile_template(string fname)
{	/*string str1, str2;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Mobile template "+fname+" not found!\n",10);
		exit(0);
	}*/
    PALETTE pal;

	MOBILE_TEMPLATE * mob = new MOBILE_TEMPLATE();
	mob->fname = fname;

	JsonReader jr;

	Node * root = jr.read(fname);

    if(root == NULL)
    {   debug("Mobile template "+fname+" not found!\n",10);
        exit(1);
    }

    string str2 = root->getMember("sprite_bmp")->getString();
    mob->sprite->sprite = load_bitmap(str2.c_str(), pal);
    debug("load bitmap "+str2);
    if(mob->sprite->sprite==NULL)
    {    debug("Missing sprite : "+str2,10);
         exit(1);
    }
    mob->speed = root->getMember("speed")->getInt();

    Node * n = root->getMember("sprite_modes");
    SPRITE_MODE * sm = new SPRITE_MODE;
    sm->frames = n->getMember("frames")->getInt();
    sm->width = n->getMember("width")->getInt();
    sm->height = n->getMember("height")->getInt();
    sm->start_y = n->getMember("start_y")->getInt();
    mob->sprite->Modes.add(sm);
    return mob;
}

/*
int load_mobile_save(MOBILE * mob, string s)
{	int ind;
    if(sscanf(s.c_str(),"%d %*s",&ind))
    {   Game->MobileTemplates[ind]->ResetClone(mob);
    }
    if(sscanf(s.c_str(),"%*d %f %f %f %d %d %d %d %d %d %d",&mob->x,&mob->y,&mob->z,&mob->heading,&mob->action,&mob->act_progress,&mob->act_target,&mob->flag_pass,&mob->last_action,&mob->next_action)<10)
	{ debug("Not enough parameters for mobile save.",10);
	  exit(1);
	  return 0;
	}
	return 1;
}*/

int load_mobile_save(MOBILE * mob, Node * n)
{   Game->MobileTemplates[n->getMember("template_nr")->getInt()]->ResetClone(mob);
	mob->x=n->getMember("x")->getFloat();
	mob->y=n->getMember("y")->getFloat();
	mob->z=n->getMember("z")->getFloat();
	mob->heading=n->getMember("heading")->getInt();
	mob->action=n->getMember("action")->getInt();
	mob->act_progress=n->getMember("act_progress")->getInt();
	mob->act_target=n->getMember("act_target")->getInt();
	mob->flag_pass=n->getMember("flag_pass")->getInt();
	mob->last_action=n->getMember("last_action")->getInt();
	mob->next_action=n->getMember("next_action")->getInt();
	return 1;
}
