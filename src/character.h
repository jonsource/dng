#ifndef CHARACTER_H_
#define CHARACTER_H

#include <stdio.h>
#include <allegro.h>
#include <string>
#include "game.h"

using namespace std;

class Character
{ public:
    Character(int race);
    ~Character();
    void print_stats();
    void IncStat(int i);

    string name;
    string classname;
    int Class;
    int courage;
    int strength;
    int dexterity;
    int agility;
    int constitution;
    int wisdom;
    int intelligence;
    int charisma;

    int HP;
    int HP_max;
    int MP;
    int MP_max;
    int SP;
    int SP_max;

    int level;
    int experience;

  private:
};


#endif
