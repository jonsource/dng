#include "game.h"
#include "allegro.h"
#include "inventory.h"
#include "pc_slot.h"
#include "character.h"

ITEM::ITEM(string s)
{   this->name = s;
    this->count=1;
    this->countable=0;
    this->type=0;
    this->weight=1;
}


INVENTORY::INVENTORY()
{   this->Items = new ITEM * [INV_TOTAL_SIZE];
    for(int i=0; i<INV_TOTAL_SIZE; i++)this->Items[i]=NULL;
    this->chr = NULL;
    this->Items[INV_BACKPACK] = new ITEM("dagger");
    this->Items[INV_BACKPACK+1] = new ITEM("knife");
}

INVENTORY::~INVENTORY()
{   delete [] this->Items;

}

void click_inventory(int mw, int mh)
{
    debug("click inventory "+to_str(mw)+" "+to_str(mh),5);
    if(mh>440)
    {   int slot_no = mw / 108;
        close_inventory();
        open_inventory(slot_no);
        return;
    }
    if(mw<80 && mh<80) close_inventory();
}

void open_inventory(int slot_no)
{   if(Game->PcSlots[slot_no].state<=PC_SLOT_EMPTY) return;
    Game->clickable_level="inventory";
    Game->show_inventory=slot_no;
    Game->Clickables["inventory"].add(new CLICKABLE(0,440,640,480,click_inventory));
    Game->Clickables["inventory"].add(new CLICKABLE(0,0,80,80,click_inventory));
    for(int i=0; i<INV_TOTAL_SIZE; i++)
        create_inv_box(NULL,slot_no,i); //create clickables for inventory boxes
}

void close_inventory()
{   Game->clickable_level="gui";
    Game->show_inventory = -1;
    Game->Clickables["inventory"].clear_all();
}

void create_inv_box(BITMAP * game_bmp, int slot_no, int box_no)
{   int bx,by;
    if(box_no<INV_BACKPACK)
    {   switch(box_no)
        {   case INV_HEAD:  bx=420; by=80; break;
            case INV_NECK:  bx=480; by=110; break;
            case INV_BODY:  bx=420; by=140; break;
            case INV_CLOAK:  bx=360; by=110; break;
            case INV_BELT_1:  bx=420; by=200; break;
            case INV_BELT_2:  bx=480; by=230; break;
            case INV_BELT_3:  bx=540; by=230; break;
            case INV_LEGS:  bx=420; by=260; break;
            case INV_FEET:  bx=420; by=320; break;
            case INV_WLD_LEFT:  bx=485; by=170; break;
            case INV_WLD_RIGHT:  bx=355; by=170; break;
        }

    }
    else
    {   bx=130+60*((box_no-INV_BACKPACK)%3);
        by=80+60*((box_no-INV_BACKPACK)/3);
    }
    // if bitmap supplied - draw boxes, else create clickables;
    if(game_bmp!=NULL)
    {   rect(game_bmp, bx, by, bx+55, by+55, makecol(95,58,21));
        string s = to_str((box_no-INV_BACKPACK)%3)+" "+to_str((box_no-INV_BACKPACK)/3);
        //textprintf_ex(game_bmp, font, bx+5, by+5, makecol(50,50,50), -1,s.c_str());
        ITEM * it = Game->PcSlots[slot_no].character->Inventory->Items[box_no];
        if(it != NULL)
        {   //debug("Item box "+to_str(slot_no)+" "+to_str(box_no));
            rect(game_bmp, bx+2, by+2, bx+53, by+53, makecol(95,58,21));
            BITMAP * b = GRAPHICS->getItem(it->name);
            masked_blit(b, game_bmp, 0, 0, bx, by, 50, 50);

        }
    }
    else Game->Clickables["inventory"].add(new CLICKABLE(bx,by,bx+50,by+50,slot_no,box_no,click_inv_box));
}

void click_inv_box(int slot_no, int box_no)
{   INVENTORY * inv = Game->PcSlots[slot_no].character->Inventory;
    ITEM * it = inv->Items[box_no];
    if(Game->ActiveItem==NULL)
    {   Game->ActiveItem=it;
        inv->Items[box_no]=NULL;
    }
    else
    {   if(it==NULL)
        {   inv->Items[box_no]=Game->ActiveItem;
            Game->ActiveItem=NULL;
        }
    }
}

void draw_inventory(BITMAP * game_bmp, int slot_no)
{
    blit(GRAPHICS->getInventoryBg(),game_bmp,0,0,0,20,640,420);
    textprintf_ex(game_bmp, font, 100, 60, makecol(50,50,50), -1,Game->PcSlots[slot_no].character->name.c_str());

    for(int i=0; i<INV_TOTAL_SIZE; i++)
        create_inv_box(game_bmp,slot_no,i);


}
