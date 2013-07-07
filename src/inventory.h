#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#include "list.h"
#include "string.h"

#define INV_CLOAK 0
#define INV_HEAD 1
#define INV_NECK 2
#define INV_BODY 3
#define INV_LEGS 4
#define INV_HANDS 5
#define INV_FEET 6
#define INV_WLD_RIGHT 7
#define INV_WLD_LEFT 8
#define INV_BELT_1 9
#define INV_BELT_2 10
#define INV_BELT_3 11
#define INV_BACKPACK 12
#define INV_TOTAL_SIZE 24

class Character;

class ITEM
{
    public:
        std::string name;
        int count;
        int countable;
        int weight;
        int type;
        ITEM(std::string);
};

class INVENTORY
{   private:

    public:
        ITEM ** Items;
        int add(ITEM * item);
        int remove(int i);
        int hasSpace();
        int getWeight();
        Character * chr;
        INVENTORY();
        ~INVENTORY();

};

void click_inv_box(int slot_no, int box_no);
void click_inventory(int mw, int mh);
void open_inventory(int slot_no);
void close_inventory();
void create_inv_box(BITMAP * game_bmp, int box_type, int i);
void draw_inventory(BITMAP * game_bmp, int slot_no);


#endif // INVENTORY_H_INCLUDED
