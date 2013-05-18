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

extern CAMERA * cam;
GAME * Game;

GAME::GAME()
{   game_lib::set_debug_lvl_main(4);
    game_lib::set_debug_lvl(4);
    this->light_power=128;
    this->INFO=0;
    this->TRANSPARENT=1;
    this->time=0;
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
	Classes = new ClassTemplates();
	Player = new Character(1);
	load_graphics();
	if(!load_game_save("save/game.sav"))
    {   change_area("area1.area","map1.map",3,3);
    }
    else
    {   //change_area(Game->area_name,Game->map_name,gx,gz);
        load_area(Game->area_name);
        load_map(Game->map_name);
        player_move_subr(gx,gy,gz,gh,true);
    }
    debug("done game_load");
}

/**
 * save current game
 */

int game_save()
{   FILE *f;
    string sav="save/game.sav";
    f = fopen(sav.c_str(),"w");
    if(!f)
	{	debug("Couldn't write game save "+sav+"!\n",10);
		exit(0);
	}
	debug("Saving game "+sav+".",5);
	fprintf(f,":area\n");
    fprintf(f,Game->area_name.c_str());
    fprintf(f,"\n:map\n");
    fprintf(f,Game->map_name.c_str());
    fprintf(f,"\n:time\n");
    fprintf(f,to_str(Game->time).c_str());
    fprintf(f,"\n:position_x\n");
    fprintf(f,to_str(gx).c_str());
    fprintf(f,"\n:position_y\n");
    fprintf(f,to_str(gy).c_str());
    fprintf(f,"\n:position_z\n");
    fprintf(f,to_str(gz).c_str());
    fprintf(f,"\n:position_h\n");
    fprintf(f,to_str(gh).c_str());
    /*string position=to_str(gx)+" "+to_str(gy)+" "+to_str(gz)+" "+to_str(gh);
    fprintf(f,position.c_str());*/
    fprintf(f,"\n:end\n");
    fclose(f);
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
  draw_view(gx,gy,gz,gh);
  if(Game->INFO>0) draw_triggers(gx,gz,gh);
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
        cam->dolly_xpos=gx+0.5;
        cam->dolly_zpos=gz+0.5;
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
        sscanf(h.c_str(),"%d",val);
        if(*val<minval || *val>maxval) printf("Invalid parameter to %s [%d-%d]\n",command.c_str(),minval,maxval);
        else
        {   v = *val;
            printf("%s = %d\n",command.c_str(),*val);
        }
    }
    return v;
}

int _interpret_2int(STR_LIST *l, string command, int * v1, int *v2, int min1, int max1, int min2, int max2  )
{   int v = (min1 < min2)? min1-1 : min2-1;
    if(l->len()==1)
    {   printf("%s = %d %d\n",command.c_str(),*v1,*v2);
        return *v1;
    }
    if(l->len()==2)
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
{   STR_LIST * l=tokenize(s," ");
    if(l->len()<1) return;
    if(*(*l)[0]=="exit")
    {   Game->INFO=(++Game->INFO)%3;
        return;
    }
    if(*(*l)[0]=="transparent") _interpret_1int(l,"transparent",&Game->TRANSPARENT,0,1);
    if(*(*l)[0]=="light_power") _interpret_1int(l,"light_power",&Game->light_power,0,255);
    if(*(*l)[0]=="clone")
    {   int temp=-1;
        if(_interpret_1int(l,"clone",&temp,0,Game->MobileTemplates.len()-1) != -1)
        {   Game->Mobiles.add(Game->MobileTemplates[temp]->CloneAt(gx,gz));
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
    if(*(*l)[0]=="Teleport")
    {   int destx,destz;
        _interpret_2int(l,"Teleport",&destx,&destz,0,Game->MAP_SIZE-1,0,Game->MAP_SIZE-1);
        player_move_subr(destx, gy, destz, gh, true);
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

    if(cam->dolly_xpos-0.45 < gx) cam->dolly_xpos+=0.1;
    if(cam->dolly_xpos-0.55 > gx) cam->dolly_xpos-=0.1;
    if(cam->dolly_zpos-0.45 < gz) cam->dolly_zpos+=0.1;
    if(cam->dolly_zpos-0.55 > gz) cam->dolly_zpos-=0.1;

    Game->time+=1;
    debug("end game turn",1);

}
