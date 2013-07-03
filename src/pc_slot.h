#ifndef PC_SLOT_H_INCLUDED
#define PC_SLOT_H_INCLUDED

#define PC_SLOT_EMPTY 0
#define PC_SLOT_DEAD 1
#define PC_SLOT_PORTRAIT 2
#define PC_SLOT_COMBAT 3
#define PC_SLOT_DETAIL 4
#define PC_SLOT_SKILLS 5
#define PC_SLOT_SPELLS 6
#define PC_SLOT_MAP 7


class Character;

class PCSLOT
{   public:
        int state;
        BITMAP * portrait;
        Character * character;
        PCSLOT();
        int setCharacter(Character * cr);
};

void draw_pc_slots(BITMAP * bmp);
void click_pc_slots(int mw, int mh);


#endif // PC_SLOT_H_INCLUDED
