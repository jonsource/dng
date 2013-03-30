#include <allegro.h>
#include <time.h>
#include "stdio.h"
#include "game.h"
#include "load_classes.h"
#include "graphic.h"
#include "game_map.h"
#include "interface.h"

extern int keyb_ignore,mode;
extern int status;
extern ClassTemplates *Classes;
extern List<TILE> Tiles;
extern FILE *dbg;
extern int fps, tmsec;
extern BITMAP * game_bmp;
Character *Player;
int gy=0,gz=2,gx=3,gh=0;
int light_power=128;
extern int FOV;
extern double STB;
int DEBUG_LVL_MAIN = 4;
int DEBUG_LVL = DEBUG_LVL_MAIN;
int TRANSPARENT = 0;

char chbuf[256];

extern CLICKABLE_MAP Clickables;
extern List<TRIGGER> Triggers;

/**
 * initialize and load game
 */

void game_load()
{
    srand(time(0));
	status = CREATE;
	Classes = new ClassTemplates();
	Player = new Character(1);
	load_graphics();
	change_map("map1.map",0,0);
	debug("done game_load");
}

/**
 * draw current view
 */

void game_draw()
{ /*int num,y,*pass;
  string str="";
  //line_print();
  char *lines[] = {"     Courage  %2d",
                   "    Strength  %2d",
                   "     Agility  %2d",
                   "   Dexterity  %2d",
                   "Constitution  %2d",
                   "Intelligence  %2d",
                   "      Wisdom  %2d",
                   "    Charisma  %2d",
                   NULL};
  int stats[] = {Player->courage,
                  Player->strength,
                  Player->agility,
                  Player->dexterity,
                  Player->constitution,
                  Player->intelligence,
                  Player->wisdom,
                  Player->charisma, -1};
//  acquire_bitmap(bmp);*/


  /*for (num = 0, y = 20; lines[num]; num++, y += text_height(font))
//         textout_ex(bmp, font, lines[num], 300, y,makecol(0, 0, 0), makecol(255, 255, 255));
         textprintf_ex(game_bmp, font, 30, y, makecol(255, 255, 255), -1, lines[num], stats[num]);
  pass=Classes->Check(Player);
  for(num = 0; num<Classes->size; num++)
      if(pass[num]) str+=Classes->Templates[num]->name+" ";
  textprintf_ex(game_bmp, font, 30, 150, makecol(255, 255, 255), -1, str.c_str());
  delete pass;*/
//  release_bitmap(bmp);
  debug("begin game_draw",2);
    scare_mouse();
  draw_view(gx,gy,gz,gh);
  draw_triggers(gx,gz,gh);
    unscare_mouse();
}

void player_move(int x, int y, int z, int h)
{   player_move_subr(x,y,z,h,false);

}

void player_move_subr(int x, int y, int z, int h, bool force)
{	int nz,nx;
    int can_pass=true;


    if(!force)
    {
        /* NORMAL MOVEMENT */

        /* change heading */
        if(h!=0)
        {   gh+=h;
            if(gh<0) gh=3;
            if(gh>3) gh=0;
            debug("Change heading to "+to_str(gh),5);
        }
        /* change position */
        else
        {   switch(gh)
            { case HEAD_NORTH: nz=gz+z; nx=gx+x; break;
            case HEAD_EAST: nz=gz+x; nx=gx-z; break;
            case HEAD_SOUTH: nz=gz-z; nx=gx-x; break;
            case HEAD_WEST: nz=gz-x; nx=gx+z; break;
            }
            gy+=y;
            if(check_coords(nx,nz)==3) can_pass = false;

            List<CLICKABLE> clklist;
            CLICKABLE * clk;

            clklist=Clickables["leave"];

            for(int i=0; i<clklist.len(); i++)
            {   clk=clklist[i];
                debug("leave trigger "+to_heading_str(clk->callback->type-8),5);
                //clk->callback->fire();
                if(clk->callback->type-8==xz_to_heading(nx-gx,nz-gz) && ((int)(get_movator_dif(clk->callback->animator,tmsec)*100))<clk->w1) can_pass=false;
            }


            if(can_pass)
            {
                debug("Move to "+to_heading_str(xz_to_heading(nx-gx,nz-gz))+" "+to_str(nx)+" "+to_str(nz)+" = "+to_str(check_coords(nz,nx)),5);
                gz=nz; gx=nx;

            }
            else
            {   debug("Bump! tried to move to "+to_heading_str(xz_to_heading(nx-gx,nz-gz))+" "+to_str(nx)+" "+to_str(nz)+" = "+to_str(check_coords(nz,nx)),5);
            }
        }
    }
    else
    {   /* FORCED MOVEMENT - map change, teleport etc.. */
        gh=h;
        gx=x;
        gz=z;
        gy=y;
    }

    Clickables["place"].clear_all();
    Clickables["leave"].clear_all();
    for(int i=0; i<Triggers.len(); i++)
    {   TRIGGER * t;
        t=Triggers[i];
        debug("["+to_str(gx)+","+to_str(gz)+"] Trigger "+to_str(t->xpos)+" "+to_str(t->zpos),3);
            /* correct coordinates and (TYPE_ENTER or correct heading) */
        if(t->xpos==gx && t->zpos==gz && (t->type==TRIGGER_ENTER || t->type==gh || t->type>=8 ))
        {   debug("Trigger encountered. "+to_str(t->type));
            CLICKABLE * clk = new CLICKABLE;
            clk->h1=t->h1;
            clk->w1=t->w1;
            clk->h2=t->h2;
            clk->w2=t->w2;
            clk->callback = t;
            if(t->type<8) Clickables["place"].add(clk);
            else Clickables["leave"].add(clk);
        }
    }
    //draw_triggers(gx,gz,gh);
}

void mouse_click(int mw, int mh)
{   List<CLICKABLE> clklist;
    CLICKABLE * clk;
    debug("Left mouse button "+to_str(mw)+" "+to_str(mh));

    CLICKABLE_MAP_ITERATOR it;
    for(it = Clickables.begin(); it!=Clickables.end(); it++)
    {   if(it->first=="leave") continue;
        clklist=Clickables[it->first];

        for(int i=0; i<clklist.len(); i++)
        {   clk=clklist[i];
            if(mw>=clk->w1 && mw <=clk->w2 && mh>=clk->h1 && mh<= clk->h2)
            {   debug(it->first+" ",5);
                clk->callback->fire();
            }
        }
    }
}

/**
 * handle input
 */
void keypress(int i)
{  int x=0,z=0,y=0,h=0;
   if(status == CREATE)
   { if(i == KEY_P) status = PLAY;
     if(i == KEY_M) { mode = mode+1; mode = mode%6; }
     if(i == KEY_V) { delete(Player); Player=new Character(1); }
     if(i == KEY_W) z=-1;
     if(i == KEY_S) z=1;
     if(i == KEY_A) x=-1;
     if(i == KEY_D) x=1;
     if(i == KEY_R) y=1;
     if(i == KEY_F) y-=1;
     if(i == KEY_H) { FOV+=1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_J) { FOV-=1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_I) { STB+=0.1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_K) { STB-=0.1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_O) light_power+=1;
     if(i == KEY_L) light_power-=1;
     if(i == KEY_T) {if(TRANSPARENT) TRANSPARENT =0; else TRANSPARENT=1;}
     if(i == KEY_E) {h=1;}
     if(i == KEY_Q) {h=-1;}
     if(i == KEY_L) { change_map("map2.map",0,0); }
     if(i == KEY_N)
     { Class *cl=Classes->GetTemplate(Player->classname);
       cl->NextLevel(Player);
     }
     player_move(x,y,z,h);
   }
   if(status == PLAY)
   {
   }
   if(status == PAUSE)
   { if(i == KEY_SPACE) { status = PLAY; } }
   keyb_ignore = 10;
}

/**
 * convert string to bool
 */
bool to_bool(string s)
{  if(s=="false" || s=="False" || s=="solid") return false;
   if(s=="true" || s=="True" || s=="trans") return true;
   if(s.find("no-")==0) return false;
   else
   { debug("Unclear argument "+s+", defaulting to True",9);
     return true;
   }
}

/**
 * convert integer to string
 */
string to_str(int i)
{	sprintf(chbuf,"%d",i);
	return chbuf;
}

/**
 * convert float to string
 */
string to_str(float f)
{	sprintf(chbuf,"%.2f",f);
	return chbuf;
}

/**
 * convert double to string
 */
string to_str(double f)
{	sprintf(chbuf,"%.2f",f);
	return chbuf;
}

/**
 * convert bool to string
 */
string to_str(bool b)
{	if(b) return "true";
	return "false";
}

/**

*/

bool heading_to_xz(int h,int *x,int *z)
{   switch(h)
    {   case HEAD_EAST : *x = 1; *z = 0; return true;
        case HEAD_NORTH : *x = 0; *z = -1; return true;
        case HEAD_WEST : *x = -1; *z = 0; return true;
        case HEAD_SOUTH : *x = 0; *z = 1; return true;
    }
    return false;
}

int xz_to_heading(int xfront, int zfront)
{   if(zfront<0) return HEAD_NORTH;
    if(zfront>0) return HEAD_SOUTH;
    if(xfront<0) return HEAD_WEST;
    return HEAD_EAST;

}

string to_heading_str(int h)
{   if(h==HEAD_NORTH) return "north";
    if(h==HEAD_EAST) return "east";
    if(h==HEAD_SOUTH) return "south";
    if(h==HEAD_WEST) return "west";
    return "unknown";
}

/**
 * get current time in miliseconds based on processor clock
 */
int mstime()
{	return clock()/(float)CLOCKS_PER_SEC*100;
}

/**
 * write debug information to log
 * lvl - level of importance, the higher the more important
 */
void debug(string s, int lvl)
{
	if(lvl<DEBUG_LVL) return;
	s="\n"+to_str((int)mstime())+" "+s;
	printf(s.c_str());
	fprintf(dbg,s.c_str());
	fflush(dbg);
}
/**
 * write debug information to log with default lvl
 */
void debug(string s)
{ debug(s,4);
}

/**
 * append debug information (omits current time stamp)
 */
void dappend(string s, int lvl)
{ 	if(lvl<DEBUG_LVL) return;
    printf(s.c_str());
	fprintf(dbg,s.c_str());
	fflush(dbg);
}

/**
 * append debug information (omits current time stamp), with default lvl
 */
void dappend(string s)
{ 	dappend(s,4);
}

/**
 * set what lvl of importance is needed to be logged, 0 for everything, 10 for production version
 * may be used to temporarily increase logging in parts of code
 */
void set_debug_lvl(int lvl)
{
	DEBUG_LVL=lvl;
}

/**
 * reset debug level to main lvl
 */
void reset_debug_lvl()
{
	DEBUG_LVL=DEBUG_LVL_MAIN;
}

void game_unload()
{
}
