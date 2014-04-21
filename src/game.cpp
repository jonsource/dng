#include <allegro.h>
#include "jslib.h"
#include "libjsjson.h"
#include "game_lib.h"
#include "load_classes.h"
#include "game_map.h"
#include "mobile.h"
#include "pc_slot.h"

extern int keyb_ignore;
extern int status;
extern ClassTemplates *Classes;
extern int fps, tmsec;
extern BITMAP * game_bmp;
Character *Player;

using namespace jslib;

extern CAMERA * cam;
GAME * Game;

GAME::GAME()
{   jslib::set_debug_lvl_main(4);
    jslib::set_debug_lvl(4);
    this->light_power=128;
    this->info=0;
    this->transparent=1;
    this->time=0;
    this->PcSlots = new PCSLOT[6];
    this->show_inventory = -1;
    this->ActiveItem = NULL;
    this->y=0;
    this->z=2;
    this->x=3;
    this->h=0;
}

GAME::~GAME()
{   delete [] this->PcSlots;

}

int GAME::GetDebugLvlMain()
{   return jslib::get_debug_lvl_main();
}

int GAME::SetDebugLvlMain(int dbg_lvl)
{   return jslib::set_debug_lvl_main(dbg_lvl);
}

int GAME::GetDebugLvl()
{   return jslib::get_debug_lvl();
}

int GAME::SetDebugLvl(int dbg_lvl)
{   return jslib::set_debug_lvl(dbg_lvl);
}

int GAME::ResetDebugLvl()
{   return jslib::reset_debug_lvl();
}

int GAME::DestroyMobile(int i)
{   debug("mobile to destroy: "+to_str(i)+", mobiles length :"+to_str(Game->Mobiles.len()),5);
    if(i>=Game->Mobiles.len()) return 0;
    MOBILE * mob = Game->Mobiles[i];
    debug(" mob ",5);
    Game->Mobiles.remove(i);
    debug(" remove ",5);
    delete mob;
    debug("destroyed mobile: "+to_str(i)+", mobiles length :"+to_str(Game->Mobiles.len()),5);
    return 1;
}
/**
 * initialize and load game
 */

void game_load()
{
    srand(time(0));
	status = CREATE;
	load_graphics();

	Classes = new ClassTemplates();
	Player = new Character(1);
	Game->PcSlots[0].setCharacter(Player);
	Player = new Character(0);
	Player->name="shambala";
	Game->PcSlots[1].setCharacter(Player);

    INV_HEAP * ih = new INV_HEAP(4.25,3.25);
    ih->addItem(new ITEM("dagger"));
    ih->addItem(new ITEM("dagger"));
    ih->addItem(new ITEM("dagger"));
    Game->InvHeaps.add(ih);
    ih = new INV_HEAP(3.25,3.25);
    ih->addItem(new ITEM("dagger"));
    ih->addItem(new ITEM("dagger"));
    Game->InvHeaps.add(ih);
    ih = new INV_HEAP(3.75,3.25);
    ih->addItem(new ITEM("dagger"));
    Game->InvHeaps.add(ih);

    init_gui();

    if(!load_game_save("save/game.json"))
    {   change_area("area1.area","map1.map",3,3);
    }
    else
    {   //change_area(Game->area_name,Game->map_name,gx,gz);
        load_area(Game->area_name);
        load_map(Game->map_name);
        player_move_subr(Game->x,Game->y,Game->z,Game->h,true);
    }
    debug("done game_load");
}

/**
 * save current game
 */

int game_save()
{   FILE *f;

    string sav="save/game.json";
    f = fopen(sav.c_str(),"w");
    if(!f)
	{	debug("Couldn't write game save "+sav+"!\n",10);
		exit(0);
	}
	debug("Saving game "+sav+".",5);

	Node * r = new ObjectNode("{'area':'"+Game->area_name+"', 'map_name':'"+Game->map_name+"', 'time':"+to_str(Game->time)+"}");
    Node * pos = new ObjectNode();
    pos->add("x",new IntNode(Game->x));
    pos->add("y",new IntNode(Game->y));
    pos->add("z",new IntNode(Game->z));
    pos->add("h",new IntNode(Game->h));
    r->add("position",pos);

    fprintf(f,"%s",r->serialize().c_str());
    fclose(f);
    delete r;
    delete pos;
    return 1;
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
  draw_pc_slots(game_bmp);
  if(Game->show_inventory==-1) draw_view(Game->x,Game->y,Game->z,Game->h);
  else draw_inventory(game_bmp,Game->show_inventory);
  if(Game->info>0) draw_triggers(Game->x,Game->z,Game->h);
  draw_cursor();
//    unscare_mouse();
}

void player_move(int x, int y, int z, int h)
{   debug("Player move start",5);
    player_move_subr(x,y,z,h,false);
    debug("Player move stop\n",5);
}

int can_leave(int x, int z, int dir)
{   /* apply leave triggers and blockers */

    /* wrong usage of clickable - doesn't block npcs
    List<CLICKABLE> clklist;
    CLICKABLE * clk;


    clklist=Game->Clickables["leave"];

    for(int i=0; i<clklist.len(); i++)
    {   clk=clklist[i];

        // correct heading && moving blockation is in position to block
        if(clk->callback->type - BLOCKER_TO_HEADING == dir && ((int)(get_movator_dif(clk->callback->animator,tmsec)*100))<clk->w1)
        {   debug("Blocked by leave trigger "+heading_to_str(clk->callback->type-8),5);
            return 0;
        }
    }
    return 1;*/

    /* search for leave blockers */
    for(int i=0; i<Game->Triggers.len(); i++)
    {   TRIGGER * t;
        t=Game->Triggers[i];
        if(t->type<BLOCKER_NORTH || t->type>BLOCKER_LEAVE || t->type==BLOCKER_ENTER) continue; // not a leave blocker, skipping
        debug("Can leave ["+to_str(x)+","+to_str(z)+"] Trigger "+to_str(t->xpos)+" "+to_str(t->zpos),5);
        /* correct coordinates and (BLOCKER_ENTER or correct heading) */
        if(t->xpos==x && t->zpos==z && (t->type==BLOCKER_ENTER || t->type-BLOCKER_TO_HEADING==dir))
        {   debug("Blocker encountered. "+to_str(t->type),5);
            /* moving blockation is in position to block */
            if((int)(get_movator_dif(t->animator,tmsec)*100)<t->w1) return 0;
        }
    }
    return 1;
}

int is_passable(int x, int z)
{   int t=check_coords(x,z);
    for(int i=0; i<10; i++)
    {   if(t==Game->Impassable[i]) return 0; }
    return 1;
}

int can_enter(int x, int z, int dir)
{   if(!is_passable(x,z)) return 0;

    /* search for entry blockers */
    for(int i=0; i<Game->Triggers.len(); i++)
    {   TRIGGER * t;
        t=Game->Triggers[i];
        if(t->type<BLOCKER_NORTH || t->type>BLOCKER_ENTER) continue; // not a blocker, skipping
        debug("Can enter ["+to_str(x)+","+to_str(z)+"] Trigger "+to_str(t->xpos)+" "+to_str(t->zpos),5);
        /* correct coordinates and (BLOCKER_ENTER or correct heading) */
        if(t->xpos==x && t->zpos==z && (t->type==BLOCKER_ENTER || t->type-BLOCKER_TO_HEADING==dir))
        {   debug("Blocker encountered. "+to_str(t->type),5);
            /* moving blockation is in position to block */
            if((int)(get_movator_dif(t->animator,tmsec)*100)<t->w1) return 0;
        }
    }
    return 1;
}

void player_move_subr(int x, int y, int z, int h, bool force)
{	int nz,nx;
    int can_pass=true;

    if(!force)
    {
        /* NORMAL MOVEMENT */

        /* change heading */
        if(h!=0)
        {   Game->h+=h;
            if(Game->h<0) Game->h=3;
            if(Game->h>3) Game->h=0;
            debug("Change heading to "+to_str(Game->h),5);
        }
        /* change position */
        else
        {   switch(Game->h)
            {   case HEAD_NORTH: nz=Game->z+z; nx=Game->x+x; break;
                case HEAD_EAST: nz=Game->z+x; nx=Game->x-z; break;
                case HEAD_SOUTH: nz=Game->z-z; nx=Game->x-x; break;
                case HEAD_WEST: nz=Game->z-x; nx=Game->x+z; break;
            }
            Game->y+=y;

            if(!can_leave(Game->x,Game->z,xz_to_heading(nx-Game->x,nz-Game->z))) // new - old: leaving IN direction
            {   debug("Blocked on leave.",4);
                can_pass=false;
            }
            if(!can_enter(nx,nz,xz_to_heading(Game->x-nx,Game->z-nz))) // old - new: entering FROM direction
            {   debug("Blocked on entry.",4);
                can_pass=false;
            }
            if(can_pass)
            {
                debug("Move to "+heading_to_str(xz_to_heading(nx-Game->x,nz-Game->z))+" "+to_str(nx)+" "+to_str(nz)+" = "+to_str(check_coords(nz,nx)),5);
                Game->z=nz; Game->x=nx;

            }
            else
            {   debug("Bump! tried to move to "+heading_to_str(xz_to_heading(nx-Game->x,nz-Game->z))+" "+to_str(nx)+" "+to_str(nz)+" = "+to_str(check_coords(nz,nx)),5);
            }
        }

            /* clear old triggers */
            clear_triggers("place");
            clear_triggers("leave");
            clear_triggers("enter");

        /* apply local triggers and clickables */
            apply_local_triggers(Game->x, Game->z, Game->h);

        /* moved (not just changed heading), fire enter triggers */
        if(x!=0 || z!=0)
        {   fire_triggers("enter");
        }
    }
    else
    {   /* FORCED MOVEMENT - map change, teleport etc.. */
        if(h!=-1)
        Game->h=h;
        Game->x=x;
        Game->z=z;
        Game->y=y;
        cam->dolly_xpos=Game->x+0.5;
        cam->dolly_zpos=Game->z+0.5;
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

    if(Game->info==2)
    {   /* development camera controls */
         if(i == KEY_H) { Game->view_settings.fov+=1; init_camera(&Game->view_settings); }
         if(i == KEY_J) { Game->view_settings.fov-=1; init_camera(&Game->view_settings); }
         if(i == KEY_I) { Game->view_settings.step_back+=0.1; init_camera(&Game->view_settings); }
         if(i == KEY_K) { Game->view_settings.step_back-=0.1; init_camera(&Game->view_settings); }

        /* development light setting controls */
         if(i == KEY_O) Game->light_power+=1;
         if(i == KEY_L) Game->light_power-=1;
    }

    if(i == KEY_ESC) { Game->info = 0;}

    if(i == KEY_F1) {  if(Game->info!=2) Game->info=2;
                       else Game->info=0;
                    }

     if(i == KEY_F2) {  clear_keybuf();
                        Game->info=1;
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

int _interpret_1int(StringArray * l, string command, int * val, int minval, int maxval)
{   int v=minval-1;
    if(l->size()==1)
    {   printf("%s = %d\n",command.c_str(),*val);
        return *val;
    }
    else
    {
        string h=*(*l)[1];
        sscanf(h.c_str(),"%d",val);
        if(*val<minval || *val>maxval) printf("Invalid parameter to %s [%d-%d]\n",command.c_str(),minval,maxval);
        else
        {   v = *val;
            printf("%s = %d\n",command.c_str(),*val);
        }
    }
    return v;
}

int _interpret_2int(StringArray *l, string command, int * v1, int *v2, int min1, int max1, int min2, int max2  )
{   int v = (min1 < min2)? min1-1 : min2-1;
    if(l->size()==1)
    {   printf("%s = %d %d\n",command.c_str(),*v1,*v2);
        return *v1;
    }
    if(l->size()==2)
    {   printf("Wrong number of arguments to %s. Either 0 to print or 2 to assign.",command.c_str());
        return v;
    }
    else
    {   string h=*(*l)[1];
        sscanf(h.c_str(),"%d",v1);
        if(*v1<min1 || *v1>max1)
        {   printf("Invalid parameter 1 to %s [%d-%d]\n",command.c_str(),min1,max1);
            return v;
        }
        h=*(*l)[2];
        sscanf(h.c_str(),"%d",v2);
        if(*v2<min2 || *v2>max2)
        {   printf("Invalid parameter 2 to %s [%d-%d]\n",command.c_str(),min2,max2);
            return v;
        }
        {   v = *v1;
            printf("%s = %d %d\n",command.c_str(),*v1,*v2);
        }
    }
    return v;
}

void text_interpret(string s)
{   StringArray * l=explode(" ",s);
    if(l->size()<1) return;
    if(*(*l)[0]=="exit")
    {   Game->info=(++Game->info)%3;
        return;
    }
    if(*(*l)[0]=="transparent") _interpret_1int(l,"transparent",&Game->transparent,0,1);
    if(*(*l)[0]=="light_power") _interpret_1int(l,"light_power",&Game->light_power,0,255);
    if(*(*l)[0]=="clone")
    {   int temp=-1;
        if(_interpret_1int(l,"clone",&temp,0,Game->MobileTemplates.len()-1) != -1)
        {   Game->Mobiles.add(Game->MobileTemplates[temp]->CloneAt(Game->x,Game->z));
        }
    }
    if(*(*l)[0]=="mob_go")
    {   int direction=0;
        _interpret_1int(l,"mob_go",&direction,0,4);
        Game->Mobiles[0]->next_action = direction;
    }
    if(*(*l)[0]=="kill")
    {   int mob_i;
        _interpret_1int(l,"kill",&mob_i,0,Game->Mobiles.len()-1);
        Game->DestroyMobile(mob_i);
    }
    if(*(*l)[0]=="Trigger")
    {   int v;
        if(l->size()==1)
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
    if(*(*l)[0]=="Teleport")
    {   int destx,destz;
        _interpret_2int(l,"Teleport",&destx,&destz,0,Game->map_size-1,0,Game->map_size-1);
        player_move_subr(destx, Game->y, destz, Game->h, true);
    }
    debug("<<< end interpret");
}

void game_unload()
{   game_save();
    unload_area();
}

void game_turn()
{   debug("game turn "+to_str(Game->Mobiles.len()),1);
    for(int i=0; i<Game->Mobiles.len(); i++)
    {   MOBILE * mob = Game->Mobiles[i];
        mob->HeartBeat();

    }

    /* player multimove */

    //debug("multimove ["+to_str(gx)+","+to_str(gz)+"]",5);
    //debug("multimove ["+to_str(gx)+","+to_str(gz)+"]",5);

    if(cam->dolly_xpos-0.45 < Game->x) cam->dolly_xpos+=0.1;
    if(cam->dolly_xpos-0.55 > Game->x) cam->dolly_xpos-=0.1;
    if(cam->dolly_zpos-0.45 < Game->z) cam->dolly_zpos+=0.1;
    if(cam->dolly_zpos-0.55 > Game->z) cam->dolly_zpos-=0.1;

    Game->time+=1;
    debug("end game turn",1);

}
