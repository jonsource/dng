#include "interface.h"
#include "game.h"
#include "texture.h"
#include "stdio.h"
#include "sound.h"
#include "game_map.h"
#include "pc_slot.h"

//CLICKABLE_MAP Clickables;
int TRIGGER::type_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
    if(type.compare("TRIGGER_ENTER")==0) {typ=TRIGGER_ENTER; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_LEAVE")==0) {typ=TRIGGER_LEAVE; type_ok=true;}

    if(!type_ok && type.compare("TRIGGER_NORTH")==0) {typ=TRIGGER_NORTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_EAST")==0) {typ=TRIGGER_EAST; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_SOUTH")==0) {typ=TRIGGER_SOUTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_WEST")==0) {typ=TRIGGER_WEST; type_ok=true;}

    if(!type_ok && type.compare("BLOCKER_ENTER")==0) {typ=BLOCKER_ENTER; type_ok=true;}
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

string TRIGGER::type_string()
{	switch(this->type)
    {
        case TRIGGER_NORTH: return "TRIGGER_NORTH";
        case TRIGGER_EAST: return "TRIGGER_EAST";
        case TRIGGER_SOUTH: return "TRIGGER_SOUTH";
        case TRIGGER_WEST: return "TRIGGER_WEST";

        case TRIGGER_ENTER: return "TRIGGER_ENTER";
        case TRIGGER_LEAVE: return "TRIGGER_LEAVE";

        case BLOCKER_NORTH: return "BLOCKER_NORTH";
        case BLOCKER_EAST: return "BLOCKER_EAST";
        case BLOCKER_SOUTH: return "BLOCKER_SOUTH";
        case BLOCKER_WEST: return "BLOCKER_WEST";
        case BLOCKER_ENTER: return "BLOCKER_ENTER";
    }
    debug("Unknown trigger type number: "+to_str(type),10);
	exit(1);
	return "";
}

TRIGGER * load_trigger(string s)
{	char buf[32],buf2[32];
    int xpos;
    int zpos;
    int w1,h1,w2,h2,anim;
    TRIGGER * ret;

    int params=0;
	if((params=sscanf(s.c_str(),"%s %d %d %d %d %d %d %d %s",buf,&xpos,&zpos,&w1,&h1,&w2,&h2,&anim,buf2))<8)
	{ debug("Not enough parameters loading trigger: "+s,10);
	  exit(1);
	}
	if(params==8) ret = new TRIGGER(TRIGGER::type_resolve(buf),xpos,zpos,w1,h1,w2,h2,anim);
	ret = new TRIGGER(TRIGGER::type_resolve(buf),xpos,zpos,w1,h1,w2,h2,anim,buf2);
	return ret;
}

void TRIGGER::ConstructTrigger(int type, int xpos, int zpos, int w1, int h1, int w2, int h2,int animator, string action, void(*callback)(int mw, int mh))
{   this->type=type;
    this->xpos=xpos;
    this->zpos=zpos;
    this->h1=h1;
    this->w1=w1;
    this->h2=h2;
    this->w2=w2;
    this->animator_nr=animator;
    this->callback = callback;
    if(animator>-1)
    {   if(animator<Game->Animators.len())	this->animator = Game->Animators[animator];
        else
        {	debug("Referencing undefined animator "+to_str(animator),10);
            exit(1);
        }
    }
    else this->animator=NULL;
    if(action=="") this->action=NULL;
    else this->action = new string(action);
}

TRIGGER::TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, string action)
{   ConstructTrigger(type,xpos,zpos,w1,h1,w2,h2,-1,action, NULL);
}
TRIGGER::TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, int animator)
{   ConstructTrigger(type,xpos,zpos,w1,h1,w2,h2,animator,"", NULL);
}
TRIGGER::TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, int animator, string action)
{   ConstructTrigger(type,xpos,zpos,w1,h1,w2,h2,animator,action, NULL);
}
TRIGGER::TRIGGER(int type, void(*callback)(int mw, int mh))
{   ConstructTrigger(type,0,0,0,0,0,0,-1,"", callback);
}

string TRIGGER::serialize()
{   string ret="";
    ret+=this->type_string();
    ret+=" "+to_str(this->xpos);
    ret+=" "+to_str(this->zpos);
    ret+=" "+to_str(this->w1);
    ret+=" "+to_str(this->h1);
    ret+=" "+to_str(this->w2);
    ret+=" "+to_str(this->h2);
    ret+=" "+to_str(this->animator_nr);
    ret+=" "+*this->action;
    return ret;
}

void TRIGGER::fire()
{   fire(-1,-1);
}

void TRIGGER::fire(int mw, int mh)
{   if(callback!=NULL)
    {   callback(mw,mh);
        return;
    }

    ANIMATOR * a = this->animator;
    if(a!=NULL)
    {   if(a->type==MOVATOR_Y)
        {   if(!a->on)
            {   a->on=1;
                a->start=tmsec;
            }
        }
        else
        {   a->on=!a->on;
            a->start=tmsec;
            debug("animator toggle "+to_str(a->on)+" "+to_str(a->start));
        }
        play_sound();
    }
    debug("Trigger at "+to_str(this->xpos)+" "+to_str(this->zpos)+" fired!");
    if(this->action!=NULL) dappend(" action: "+*this->action);
    STR_LIST * tokens = tokenize(*this->action,"/");
    debug("tokens("+to_str(tokens->len())+"):");
    for(int i=0; i<tokens->len(); i++)
        dappend(" "+to_str(i)+"("+*(*tokens)[i]);

    /* change map within area */
    if(tokens->len()==2 && *(*tokens)[0]=="change_map")
    {   change_map(*(*tokens)[1],this->w1,this->h1);
    }

    /* change area and map */
    if(tokens->len()==3 && *(*tokens)[0]=="change_area")
    {   change_area(*(*tokens)[1],*(*tokens)[2],this->w1,this->h1);
    }

    tokens->clear_all();
    delete tokens;
}

CLICKABLE::CLICKABLE()
{   this->callback=NULL;
    this->trigger=NULL;
}

CLICKABLE::CLICKABLE(int w1, int h1, int w2, int h2, void (*callback)(int mw, int mh))
{   this->callback = callback;
    this->w1=w1;
    this->h1=h1;
    this->w2=w2;
    this->h2=h2;
}

void mouse_click(int mw, int mh)
{   debug("Left mouse button "+to_str(mw)+" "+to_str(mh),5);

    List<CLICKABLE> * clklist;
    CLICKABLE * clk;
    CLICKABLE_MAP_ITERATOR it;
    for(it = Game->Clickables.begin(); it!=Game->Clickables.end(); it++)
    {   if(it->first=="leave") continue;
        clklist=&Game->Clickables[it->first];

        for(int i=0; i<clklist->len(); i++)
        {   clk=(* clklist)[i];
            if(mw>=clk->w1 && mw <=clk->w2 && mh>=clk->h1 && mh<= clk->h2)
            {   debug("Clickable type: "+it->first+" ",5);
                if(clk->callback!=NULL) clk->callback(mw,mh);
                if(clk->trigger!=NULL) clk->trigger->fire(mw,mh);
            }
        }
    }
}

void fire_triggers(string s)
{   List<CLICKABLE> clklist=Game->Clickables[s];
    CLICKABLE * clk = new CLICKABLE;
    for(int i=0; i<clklist.len(); i++)
    {   clk=clklist[i];
        debug(s+" trigger "+*clk->trigger->action,5);
        clk->trigger->fire();
    }
}

void clear_triggers(string s)
{   Game->Clickables[s].clear_all();
}

void apply_local_triggers(int x, int z, int h)
{   for(int i=0; i<Game->Triggers.len(); i++)
    {   TRIGGER * t;
        t=Game->Triggers[i];

        debug("["+to_str(x)+","+to_str(z)+"] Trigger "+to_str(t->xpos)+" "+to_str(t->zpos),3);

            /* correct coordinates and (TYPE_ENTER or correct heading) */
        if(t->xpos==x && t->zpos==z && (t->type==TRIGGER_ENTER || t->type==h || t->type>= BLOCKER_NORTH ))
        {   debug("Trigger encountered. "+to_str(t->type));
            CLICKABLE * clk = new CLICKABLE;
            clk->h1=t->h1;
            clk->w1=t->w1;
            clk->h2=t->h2;
            clk->w2=t->w2;
            clk->trigger = t;
            if(t->type<=TRIGGER_WEST) Game->Clickables["place"].add(clk);
            else if(t->type==TRIGGER_LEAVE || (t->type>=BLOCKER_NORTH && t->type<=BLOCKER_WEST)) Game->Clickables["leave"].add(clk);
            else if(t->type==TRIGGER_ENTER) Game->Clickables["enter"].add(clk);
        }
    }
}
