#ifndef __LOAD_CLASSES_H_
#define __LOAD_CLASSES_H

#include <stdio.h>
#include <allegro.h>
#include <string>
#include "game.h"
#include "character.h"

#define NO_STATS 8

using namespace std;

class Class 
{ public:
    Class(FILE *f);
    ~Class();
    void print_stats();
    int Check(Character *Ch);
    void NextLevel(Character *Ch);
    string name;
  
    int courage;
    int strength;
    int dexterity;
    int agility;
    int constitution;
    int wisdom;
    int intelligence;
    int charisma;
    int priorities[8];
  private:  
    int HP_base;
    int HP_rand;
    int MP_base;
    int MP_rand;
    int SP_base;
    int SP_rand;
    
};

class ClassTemplates
{ public:
    ClassTemplates();
    ~ClassTemplates();
    Class *GetTemplate(string name);
    Class **Templates;
    int *Check(Character *ch);
    int size;
    int max;
  private:
};

#endif
