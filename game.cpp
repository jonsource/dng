#include <allegro.h>
#include <time.h>
#include "stdio.h"
#include "game.h"
#include "load_classes.h"
#include "graphic.h"
#include "map.h"

extern int keyb_ignore,mode;
extern BITMAP *game_bmp;
extern int status;
extern ClassTemplates *Classes;
extern TEXTURED_ELEMENT ** Elements;
extern int Elements_max,Elements_len;
extern TILE ** Tiles;
extern int Tiles_max;
extern FILE *dbg;
Character *Player;
int gy=0,gz=2,gx=3,gh=0;
extern int FOV;
extern double STB;
int DEBUG_LVL_MAIN = 4;
int DEBUG_LVL = DEBUG_LVL_MAIN;
int TRANSPARENT = 0;

char chbuf[256];

void game_load()
{ FILE *log;
  srand(time(0));
  status = CREATE;
  Classes = new ClassTemplates();
  Player = new Character(1);
  load_graphics();
  load_map("map1.map");
  debug("done game_load");
  //debug("El max "+to_str(Elements_max)+"  El len "+to_str(Elements_len)+"  Element #0 -> "+to_str(Elements[0]->x));
  debug("Tiles max "+to_str(Tiles_max));
  dappend(" Tiles[0]->len "+to_str(Tiles[0]->types[0]));
  dappend(" .elements[0]"+to_str(Tiles[0]->elements[0]->type));
  //exit(1);
  //exit(1);
  FOV=40;
  STB=-0.35;
}

string to_str(int i)
{	sprintf(chbuf,"%d",i);
	return chbuf;
}

string to_str(float f)
{	sprintf(chbuf,"%.2f",f);
	return chbuf;
}

string to_str(bool b)
{	if(b) return "true";
	return "false";
}

void debug(string s, int lvl)
{  if(lvl<DEBUG_LVL) return;
   s="\n"+to_str((int)time(NULL))+" "+s;
   //fprintf(dbg,s.c_str(),time(NULL));
   fprintf(dbg,s.c_str());
   fflush(dbg); 
}

void debug(string s)
{ debug(s,4);
}

void dappend(string s, int lvl)
{ 	if(lvl<DEBUG_LVL) return;
	fprintf(dbg,s.c_str());
	fflush(dbg);
}

void dappend(string s)
{ 	dappend(s,4);
}

void set_debug_lvl(int lvl)
{
	DEBUG_LVL=lvl;
}

void reset_debug_lvl()
{
	DEBUG_LVL=DEBUG_LVL_MAIN;
}

void game_unload()
{ 
}



void line_print(/*char *lines[]*/)
{ int num,y;
  char *lines[] = {"Line %02d: Draws justified text",
                       "Line %02d: within the specified",
                       "Line %02d: x2-x1 area. But not",
                       "Line %02d: T H I S !", NULL};
  for (num = 0, y = 200; lines[num]; num++, y += text_height(font))
         textprintf_ex(game_bmp, font, 300, y, makecol(0, 0, 0), makecol(255, 255, 255), lines[num], num);
}

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

debug("begin game_draw");
  /*for (num = 0, y = 20; lines[num]; num++, y += text_height(font))
//         textout_ex(bmp, font, lines[num], 300, y,makecol(0, 0, 0), makecol(255, 255, 255));
         textprintf_ex(game_bmp, font, 30, y, makecol(255, 255, 255), -1, lines[num], stats[num]);
  pass=Classes->Check(Player);       
  for(num = 0; num<Classes->size; num++)
      if(pass[num]) str+=Classes->Templates[num]->name+" ";
  textprintf_ex(game_bmp, font, 30, 150, makecol(255, 255, 255), -1, str.c_str());
  delete pass;*/
  
  draw_view(gx,gy,gz,gh);
//  release_bitmap(bmp);       
}  


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
     if(i == KEY_H) FOV+=1;
     if(i == KEY_J) FOV-=1;
     if(i == KEY_I) STB+=0.1;
     if(i == KEY_K) STB-=0.1;
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
