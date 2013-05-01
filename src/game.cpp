#include <allegro.h>
#include "game_lib.h"
#include "load_classes.h"
#include "game_map.h"
#include "mobile.h"


extern int keyb_ignore;
extern int status;
extern ClassTemplates *Classes;
extern int fps, tmsec;
extern BITMAP * game_bmp;
Character *Player;
int gy=0,gz=2,gx=3,gh=0;

GAME * Game;

GAME::GAME()
{   game_lib::set_debug_lvl_main(4);
    game_lib::set_debug_lvl(4);
    this->light_power=128;
    this->INFO=0;
    this->TRANSPARENT=1;
}

int GAME::GetDebugLvlMain()
{   return game_lib::get_debug_lvl_main();
}

int GAME::SetDebugLvlMain(int dbg_lvl)
{   return game_lib::set_debug_lvl_main(dbg_lvl);
}

int GAME::GetDebugLvl()
{   return game_lib::get_debug_lvl();
}

int GAME::SetDebugLvl(int dbg_lvl)
{   return game_lib::set_debug_lvl(dbg_lvl);
}

int GAME::ResetDebugLvl()
{   return game_lib::reset_debug_lvl();
}

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
	change_area("area1.area","map1.map",3,3);
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
//    scare_mouse();
  draw_view(gx,gy,gz,gh);
  if(Game->INFO>0) draw_triggers(gx,gz,gh);
//    unscare_mouse();
}

void player_move(int x, int y, int z, int h)
{   player_move_subr(x,y,z,h,false);

}

bool can_leave(int x, int z, int dir)
{   /* apply leave triggers and blockers */

    List<CLICKABLE> clklist;
    CLICKABLE * clk;

    clklist=Game->Clickables["leave"];

    for(int i=0; i<clklist.len(); i++)
    {   clk=clklist[i];
        debug("leave trigger "+heading_to_str(clk->callback->type-8),5);
        /* correct heading && moving blockation is in position to block */
        if(clk->callback->type - BLOCKER_TO_HEADING == dir && ((int)(get_movator_dif(clk->callback->animator,tmsec)*100))<clk->w1) return false;
    }
    return true;
}

bool is_passable(int x, int z)
{   int t=check_coords(x,z);
    for(int i=0; i<10; i++)
    {   if(t==Game->Impassable[i]) return false; }
    return true;
}

bool can_enter(int x, int z, int dir)
{   if(!is_passable(x,z)) return false;

    /* search for entry blockers */
    for(int i=0; i<Game->Triggers.len(); i++)
    {   TRIGGER * t;
        t=Game->Triggers[i];
        if(t->type<BLOCKER_NORTH || t->type>BLOCKER_ENTER) continue; // not a blocker, skipping
        debug("["+to_str(x)+","+to_str(z)+"] Trigger "+to_str(t->xpos)+" "+to_str(t->zpos),3);
        /* correct coordinates and (BLOCKER_ENTER or correct heading) */
        if(t->xpos==x && t->zpos==z && (t->type==BLOCKER_ENTER || t->type-BLOCKER_TO_HEADING==dir))
        {   debug("Blocker encountered. "+to_str(t->type));
            /* moving blockation is in position to block */
            if((int)(get_movator_dif(t->animator,tmsec)*100)<t->w1) return false;
        }
    }
    return true;
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
            {   case HEAD_NORTH: nz=gz+z; nx=gx+x; break;
                case HEAD_EAST: nz=gz+x; nx=gx-z; break;
                case HEAD_SOUTH: nz=gz-z; nx=gx-x; break;
                case HEAD_WEST: nz=gz-x; nx=gx+z; break;
            }
            gy+=y;

            if(!can_leave(gx,gz,xz_to_heading(nx-gx,nz-gz))) // new - old: leaving IN direction
            {   debug("Blocked on leave.",4);
                can_pass=false;
            }
            if(!can_enter(nx,nz,xz_to_heading(gx-nx,gz-nz))) // old - new: entering FROM direction
            {   debug("Blocked on entry.",4);
                can_pass=false;
            }
            if(can_pass)
            {
                debug("Move to "+heading_to_str(xz_to_heading(nx-gx,nz-gz))+" "+to_str(nx)+" "+to_str(nz)+" = "+to_str(check_coords(nz,nx)),5);
                gz=nz; gx=nx;

            }
            else
            {   debug("Bump! tried to move to "+heading_to_str(xz_to_heading(nx-gx,nz-gz))+" "+to_str(nx)+" "+to_str(nz)+" = "+to_str(check_coords(nz,nx)),5);
            }
        }

            /* clear old triggers */
        Game->Clickables["place"].clear_all();
        Game->Clickables["leave"].clear_all();
        Game->Clickables["enter"].clear_all();

        /* apply local clickable triggers */
        for(int i=0; i<Game->Triggers.len(); i++)
        {   TRIGGER * t;
            t=Game->Triggers[i];
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
                if(t->type<TRIGGER_WEST) Game->Clickables["place"].add(clk);
                else if(t->type==TRIGGER_LEAVE || (t->type>=BLOCKER_NORTH && t->type<=BLOCKER_WEST)) Game->Clickables["leave"].add(clk);
                else if(t->type==TRIGGER_ENTER) Game->Clickables["enter"].add(clk);
            }
        }

        /* moved (not just changed heading), apply enter triggers */
        if(x!=0 || z!=0)
        {   List<CLICKABLE> clklist=Game->Clickables["enter"];
            CLICKABLE * clk = new CLICKABLE;
            for(int i=0; i<clklist.len(); i++)
            {   clk=clklist[i];
                debug("enter trigger "+*clk->callback->action,5);
                clk->callback->fire();
            }
        }
    }
    else
    {   /* FORCED MOVEMENT - map change, teleport etc.. */
        if(h!=-1)
        gh=h;
        gx=x;
        gz=z;
        gy=y;
    }
}

void mouse_click(int mw, int mh)
{   List<CLICKABLE> clklist;
    CLICKABLE * clk;
    debug("Left mouse button "+to_str(mw)+" "+to_str(mh));

    CLICKABLE_MAP_ITERATOR it;
    for(it = Game->Clickables.begin(); it!=Game->Clickables.end(); it++)
    {   if(it->first=="leave") continue;
        clklist=Game->Clickables[it->first];

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
     //if(key[KEY_M]) { mode = mode+1; mode = mode%6; }

    /* movement controls */
     if(i == KEY_W) z=-1;
     if(i == KEY_S) z=1;
     if(i == KEY_A) x=-1;
     if(i == KEY_D) x=1;
     if(i == KEY_E) {h=1;}
     if(i == KEY_Q) {h=-1;}
    /* optional and debugging movement controls */
     if(i == KEY_R) y=1;
     if(i == KEY_F) y=-1;

    if(Game->INFO==2)
    {   /* development camera controls */
         if(i == KEY_H) { Game->view_settings.fov+=1; init_camera(&Game->view_settings); }
         if(i == KEY_J) { Game->view_settings.fov-=1; init_camera(&Game->view_settings); }
         if(i == KEY_I) { Game->view_settings.step_back+=0.1; init_camera(&Game->view_settings); }
         if(i == KEY_K) { Game->view_settings.step_back-=0.1; init_camera(&Game->view_settings); }

        /* development light setting controls */
         if(i == KEY_O) Game->light_power+=1;
         if(i == KEY_L) Game->light_power-=1;
    }

    if(i == KEY_ESC) { Game->INFO = 0;}

    if(i == KEY_F1) {  if(Game->INFO!=2) Game->INFO=2;
                       else Game->INFO=0;
                    }

     if(i == KEY_F2) {  clear_keybuf();
                        Game->INFO=1;
                    }
     if(i == KEY_N)
     { Class *cl=Classes->GetTemplate(Player->classname);
       cl->NextLevel(Player);
     }
     if(i == KEY_V) { delete(Player); Player=new Character(1); }

     if(x!=0 || y!=0 || z!=0 || h!=0) player_move(x,y,z,h);
   }
   if(status == PLAY)
   {
   }
   if(status == PAUSE)
   { if(i == KEY_SPACE) { status = PLAY; } }
   keyb_ignore = 10;
}

int _interpret_1int(STR_LIST * l, string command, int * val, int minval, int maxval)
{   int v=minval-1;
    if(l->len()==1)
    {   printf("%s = %d\n",command.c_str(),*val);
        return *val;
    }
    else
    {
        string h=*(*l)[1];
        sscanf(h.c_str(),"%d",&v);
        if(v<minval || v>maxval) printf("Invalid parameter to %s [%d-%d]\n",command.c_str(),minval,maxval);
        else
        {   *val = v;
            printf("%s = %d\n",command.c_str(),*val);
        }
    }
    return v;
}

void text_interpret(string s)
{   STR_LIST * l=tokenize(s," ");
    if(l->len()<1) return;
    if(*(*l)[0]=="exit")
    {   Game->INFO=(++Game->INFO)%3;
        return;
    }
    if(*(*l)[0]=="transparent") _interpret_1int(l,"transparent",&Game->TRANSPARENT,0,1);
    if(*(*l)[0]=="light_power") _interpret_1int(l,"light_power",&Game->light_power,0,255);
    if(*(*l)[0]=="mob_go")
    {   int direction=0;
        _interpret_1int(l,"mob_go",&direction,0,4);
        Game->Mobiles[0]->next_action = direction;
    }
    if(*(*l)[0]=="Trigger")
    {   int v;
        if(l->len()==1)
        {   printf("Provide a number [0-%d] to print info on that trigger.\n",Game->Triggers.len()-1);
            return;
        }
        else
        {   string h=*(*l)[1];
            sscanf(h.c_str(),"%d",&v);
            if(v<0 || v>=Game->Triggers.len()) printf("Invalid parameter to Trigger [0-%d]\n",Game->Triggers.len()-1);
            else
            {   printf("%s\n",Game->Triggers[v]->serialize().c_str());
            }
        }
    }
    if(*(*l)[0]=="Triggers")
    {   printf(Game->Triggers.serialize().c_str());
    }
    if(*(*l)[0]=="Lightsources")
    {   printf(Game->Lightsources.serialize().c_str());
    }
    if(*(*l)[0]=="Animators")
    {   printf(Game->Animators.serialize().c_str());
    }
    if(*(*l)[0]=="Elements")
    {   printf(Game->Elements.serialize().c_str());
    }
    if(*(*l)[0]=="Tiles")
    {   printf(Game->Tiles.serialize().c_str());
    }
    if(*(*l)[0]=="Textures")
    {   printf(Game->Textures.serialize().c_str());
    }
    if(*(*l)[0]=="Area")
    {  printf(serialize_area().c_str());
    }
    if(*(*l)[0]=="Map")
    {   printf(serialize_map().c_str());
    }
}

void game_unload()
{
}

void game_turn()
{   for(int i=0; i<Game->Mobiles.len(); i++)
    {   MOBILE * mob = Game->Mobiles[i];
        mob->HeartBeat();

    }


}
