/*
// exclude rarely used windows stuff
#define WIN32_LEAN_AND_MEAN

//include windows headers
#include <windows.h>
*/

//include c headers
//#include "main.h"
#include <stdlib.h>
#include <allegro.h>
#include "game.h"
#include "graphic.h"
#include "chat.h"

#define TIMER 10

int frames,tot_frames;
int game_time;
int msec,sec,mins,hr;
long int tmsec;
int quit;
int fps;
int keyb_ignore;
BITMAP *game_bmp=NULL,*first=NULL,*second=NULL;
int status,mode;
FILE *dbg=NULL;

int courage, strength, constitution, intelligence, wisdom, charisma, dexterity, agility;
int level,XP,maxHP,HP,MP;


void game_timer(void)
{
  game_time++;
  msec+=TIMER;
  tmsec+=TIMER;
  if(msec>999) 
  { msec-=1000; sec++;}
  if(sec>59)
  { sec-=60; mins++; }
  if(mins>59)
  { mins-=60; hr++; }
}

END_OF_FUNCTION(game_timer)

void fps_proc(void)
{
  fps=frames;
  frames=0;
}

END_OF_FUNCTION(fps_proc)

void init()
{
  mode = 1;
  
dbg = fopen("dng0a.log","w");  
debug("*** NEW RUN ***");


  LOCK_VARIABLE(game_time);
  LOCK_VARIABLE(state);
  LOCK_VARIABLE(tmsec);
  LOCK_VARIABLE(fps);
  LOCK_VARIABLE(frames);
  LOCK_VARIABLE(sec);
  LOCK_VARIABLE(msec);
  LOCK_VARIABLE(min);
  LOCK_VARIABLE(hr);
  LOCK_VARIABLE(tot_frames);
  LOCK_FUNCTION(fps_proc);
  LOCK_FUNCTION(game_timer);
  allegro_init();
  install_keyboard();
 //install_mouse();
  install_timer();

  init_graphic();

  install_int(fps_proc,1000);
  install_int(game_timer,TIMER);

  quit=0;
  fps=0;
  keyb_ignore=0;
  
}

void deinit()
{ remove_int(fps_proc);
  remove_int(game_timer);
  
  
  unload_graphics();
  
 fclose(dbg);
}

int interpret(int a)
{  return 0;
}

int main(int argc, char *argv[])
{ 
  //chat_main();
  //return 0;  
  init();
  game_load();
  if(quit) { return 1;}
    
  while(!quit)
  { while(game_time)
    { game_time=0;
      if(keyb_ignore>0) keyb_ignore--;
      
 //     game_turn();
    }
    if(status!=PAUSE) 
    { clear(game_bmp); 
      game_draw(); 
    }
    if(game_bmp==first)
    { blit(game_bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
      //show_video_bitmap(first);
      frames++;tot_frames++;
      game_bmp=second;
    }
    else
    { blit(game_bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
      //show_video_bitmap(second);
      frames++;tot_frames++;
      game_bmp=first;
    } 

//    game_interpret();
    if(!keyb_ignore)
    { if(key[KEY_F]) keypress(KEY_F);
      
      if(key[KEY_W]) keypress(KEY_W);
      if(key[KEY_S]) keypress(KEY_S);
      if(key[KEY_A]) keypress(KEY_A);
      if(key[KEY_D]) keypress(KEY_D);
      if(key[KEY_R]) keypress(KEY_R);
      if(key[KEY_F]) keypress(KEY_F);
      if(key[KEY_E]) keypress(KEY_E);
      if(key[KEY_Q]) keypress(KEY_Q);
      if(key[KEY_M]) keypress(KEY_M);
      
      if(key[KEY_B]) keypress(KEY_B);
      if(key[KEY_P]) keypress(KEY_P);
      if(key[KEY_H]) keypress(KEY_H);
      if(key[KEY_J]) keypress(KEY_J);
      if(key[KEY_I]) keypress(KEY_I);
      if(key[KEY_K]) keypress(KEY_K);
      if(key[KEY_L]) keypress(KEY_L);
      if(key[KEY_O]) keypress(KEY_O);
      if(key[KEY_X]) keypress(KEY_X);
      if(key[KEY_V]) keypress(KEY_V);
      if(key[KEY_N]) keypress(KEY_N);
      if(key[KEY_T]) keypress(KEY_T);
      if(key[KEY_SPACE]) keypress(KEY_SPACE);
    }
    if(key[KEY_ESC]) quit=1;
  }  
  game_unload();
  deinit();
  return 0;
}
END_OF_MAIN();

