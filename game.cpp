#include <allegro.h>
#include <time.h>
#include "stdio.h"
#include "game.h"
#include "load_classes.h"
#include "graphic.h"
#include "map.h"

extern int keyb_ignore,mode;
extern int status;
extern ClassTemplates *Classes;
extern List<TILE> Tiles;
extern FILE *dbg;
Character *Player;
int gy=0,gz=2,gx=3,gh=0;
int light_power=128;
extern int FOV;
extern double STB;
int DEBUG_LVL_MAIN = 4;
int DEBUG_LVL = DEBUG_LVL_MAIN;
int TRANSPARENT = 0;

char chbuf[256];

/**
 * initialiye and load game
 */

void game_load()
{	srand(time(0));
	status = CREATE;
	Classes = new ClassTemplates();
	Player = new Character(1);
	load_graphics();
	load_map("map1.map");
	debug("done game_load");
	debug("Tiles.len() "+to_str(Tiles.len()));
	dappend(" Tiles[0]->len "+to_str(Tiles[0]->types[0]));
	dappend(" .elements[0]"+to_str(Tiles[0]->elements[0]->type));
	FOV=40;
	STB=-0.85;
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
  debug("begin game_draw",3);
  draw_view(gx,gy,gz,gh);
}  

/**
 * handle input
 */

void keypress(int i)
{  int x=0,z=0;
   if(status == CREATE)
   { if(i == KEY_P) status = PLAY;
     if(i == KEY_M) { mode = mode+1; mode = mode%6; }
     if(i == KEY_V) { delete(Player); Player=new Character(1); }
     if(i == KEY_W) z=1;
     if(i == KEY_S) z=-1;
     if(i == KEY_A) x=1;
     if(i == KEY_D) x=-1;
     if(i == KEY_R) gy+=1;
     if(i == KEY_F) gy-=1;
     if(i == KEY_H) { FOV+=1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_J) { FOV-=1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_I) { STB+=0.1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_K) { STB-=0.1; init_camera(STB,FOV,1.33f); }
     if(i == KEY_O) light_power+=1;
     if(i == KEY_L) light_power-=1;
     if(i == KEY_T) {if(TRANSPARENT) TRANSPARENT =0; else TRANSPARENT=1;}
     switch(gh)
     { case HEAD_NORTH: gz-=x; gx+=z; break;
       case HEAD_EAST: gz+=z; gx+=x; break;
       case HEAD_SOUTH: gz+=x; gx-=z; break;
       case HEAD_WEST: gz-=z; gx-=x; break;
     }
     if(i == KEY_E) {gh--; if(gh<0) gh=3; }
     if(i == KEY_Q) {gh++; if(gh>3) gh=0; }
     
     if(i == KEY_N) 
     { Class *cl=Classes->GetTemplate(Player->classname);
       cl->NextLevel(Player);
     }
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
{  if(s=="false" || s=="False") return false;
   if(s=="true" || s=="True") return true;
   else
   { char buf[30];
     sprintf(buf,"Unclear argument %s, making False\n",s.c_str());
     debug(buf,9);
     return false;
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
 * convert bool to string
 */
string to_str(bool b)
{	if(b) return "true";
	return "false";
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
