#include "allegro.h"
#include "jslib.h"
#include "game.h"
#include "pc_slot.h"
#include "character.h"

using namespace jslib;

extern BITMAP * H_BAR;

PCSLOT::PCSLOT()
{
    this->state=PC_SLOT_EMPTY;
    this->character=NULL;
}

int PCSLOT::setCharacter(Character * cr)
{   PALETTE pal;
    this->character=cr;
    this->state=PC_SLOT_PORTRAIT;
    string str = "data/images/portraits/"+cr->name+".bmp";
    this->portrait = load_bmp(str.c_str(),pal);

    if(this->portrait == NULL)
    {   debug("Portrait "+cr->name+" not found!",10);
        this->portrait = create_bitmap(95,95);
        clear_to_color(this->portrait,makecol(255,128,72));
        return 2;
    }
    return 1;
}

void draw_pc_slots(BITMAP * game_bmp)
{
    int col=makecol(255,255,255);

    for(int slot_no=0; slot_no<6; slot_no++)
    {   Character * cr = Game->PcSlots[slot_no].character;
        string s;

        int state = Game->PcSlots[slot_no].state;
        if(state == PC_SLOT_PORTRAIT)
        {   textprintf_ex(game_bmp, font, 0+slot_no*108, 450, col, -1,cr->name.c_str());
            blit(GRAPHICS->getHealthBar(),game_bmp,95-cr->HP*95/cr->HP_max,0,0+slot_no*108,466,95,10);
        }
        switch(state)
        {   case PC_SLOT_EMPTY: textprintf_ex(game_bmp, font, 0+slot_no*108, 340, col, -1,"Empty");
                                break;
            case PC_SLOT_DEAD:  textprintf_ex(game_bmp, font, 0+slot_no*108, 340, col, -1,"Dead");
                                break;
            case PC_SLOT_PORTRAIT:  masked_blit(Game->PcSlots[slot_no].portrait,game_bmp,0,0,0+slot_no*108,344,95,95);
                                    //textprintf_ex(game_bmp, font, 0+i*108, 470, col, -1,"%d/%d",cr->HP,cr->HP_max);
                                    //textprintf_ex(game_bmp, font, 0+slot_no*108, 450, col, -1,cr->name.c_str());
                                    //blit(GRAPHICS->getHealthBar(),game_bmp,95-cr->HP*95/cr->HP_max,0,0+slot_no*108,466,95,10);
                                break;
            case PC_SLOT_COMBAT: textprintf_ex(game_bmp, font, 0+slot_no*108, 340, col, -1,"Combat");
                                break;

            case PC_SLOT_SKILLS: textprintf_ex(game_bmp, font, 0+slot_no*108, 340, col, -1,"Skills");
                                break;
            case PC_SLOT_SPELLS: textprintf_ex(game_bmp, font, 0+slot_no*108, 340, col, -1,"Spells");
                                break;
            case PC_SLOT_DETAIL:
                                s="HP: "+to_str(cr->HP)+"/"+to_str(cr->HP_max);
                                textprintf_ex(game_bmp, font, 0+slot_no*108, 360, col, -1,s.c_str());
                                s="SP: "+to_str(cr->HP)+"/"+to_str(cr->SP_max);
                                textprintf_ex(game_bmp, font, 0+slot_no*108, 376, col, -1,s.c_str());
                                s="MP: "+to_str(cr->HP)+"/"+to_str(cr->MP_max);
                                textprintf_ex(game_bmp, font, 0+slot_no*108, 392, col, -1,s.c_str());
                                s="lvl: "+to_str(cr->level)+" Exp:"+to_str(cr->experience);
                                textprintf_ex(game_bmp, font, 0+slot_no*108, 408, col, -1,s.c_str());
                                break;
            case PC_SLOT_MAP:
                                int see=makecol(128,128,128);
                                int not_see=makecol(72,72,72);
                                for(int i=0; i<Game->map_size; i++)
                                {   for(int j=0; j<Game->map_size; j++)
                                    {   if(Game->linesight[i][j]==0) putpixel(game_bmp,20+i+slot_no*108,360+j,see);
                                        else putpixel(game_bmp,20+i+slot_no*108,360+j,not_see);
                                    }
                                }
                                break;
        }

    }
}

void click_pc_slots(int mw, int mh)
{
    debug("click pc slots "+to_str(mw)+" "+to_str(mh),5);
    int slot_no = mw / 108;
    if(mh<=440)
    {   Game->PcSlots[slot_no].state++;
        if(Game->PcSlots[slot_no].state>PC_SLOT_MAP) Game->PcSlots[slot_no].state=PC_SLOT_PORTRAIT;
    }
    else
    {
        open_inventory(slot_no);
    }
}


