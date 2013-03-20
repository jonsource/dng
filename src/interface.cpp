#include "list.h"
#include "interface.h"

TRIGGER_MAP Triggers;

unsigned short int trigger_type_resolve(string type)
{	bool type_ok=false;
	unsigned short int typ;
	if(type.compare("TRIGGER_ENTER")==0) {typ=TRIGGER_ENTER; type_ok=true;}
	if(!type_ok && type.compare("TRIGGER_LEAVE")==0) {typ=TRIGGER_LEAVE; type_ok=true;}
	if(!type_ok && type.compare("TRIGGER_NORTH")==0) {typ=TRIGGER_NORTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_EAST")==0) {typ=TRIGGER_EAST; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_SOUTH")==0) {typ=TRIGGER_SOUTH; type_ok=true;}
    if(!type_ok && type.compare("TRIGGER_WEST")==0) {typ=TRIGGER_WEST; type_ok=true;}
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
    int w1,h1,w2,h2;
	if(sscanf(s.c_str(),"%s %d %d %d %d %d %d",buf,&xpos,&zpos,&w1,&h1,&w2,&h2)<7)
	{ debug("Not enough parameters loading trigger: "+s);
	  exit(1);
	}
	TRIGGER * ret = new TRIGGER(trigger_type_resolve(buf),xpos,zpos,w1,h1,w2,h2);
	return ret;
}

TRIGGER::TRIGGER(int xpos, int zpos, int type, int w1, int h1, int w2, int h2)
{   this->type=type;
    this->xpos=xpos;
    this->zpos=zpos;
    this->h1=h1;
    this->w1=w1;
    this->h2=h2;
    this->w2=w2;
}

void TRIGGER::fire()
{   debug("Trigger at "+to_str(this->xpos)+" "+to_str(this->zpos)+" fired!");

}
