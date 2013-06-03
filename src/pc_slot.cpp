#include "allegro.h"
#include "game.h"
#include "pc_slot.h"
#include "character.h"

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

    for(int i=0; i<6; i++)
    {   Character * cr = Game->PcSlots[i].character;
        switch(Game->PcSlots[i].state)
        {   case PC_SLOT_EMPTY: textprintf_ex(game_bmp, font, 0+i*108, 340, col, -1,"Empty");
                                break;
            case PC_SLOT_DEAD:  textprintf_ex(game_bmp, font, 0+i*108, 340, col, -1,"Dead");
                                break;
            case PC_SLOT_PORTRAIT:  masked_blit(Game->PcSlots[i].portrait,game_bmp,0,0,0+i*108,344,95,95);
                                    textprintf_ex(game_bmp, font, 0+i*108, 450, col, -1,cr->name.c_str());
                                    blit(H_BAR,game_bmp,95-cr->HP*95/cr->HP_max,0,0+i*108,466,95,10);
                                    //textprintf_ex(game_bmp, font, 0+i*108, 470, col, -1,"%d/%d",cr->HP,cr->HP_max);
                                break;

        }

    }
}
