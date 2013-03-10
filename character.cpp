/*
 *	Player character Class - basic definitions
 */

#include "character.h"

Character::Character(int race)
{ courage = rand()%8+6;
  strength = rand()%8+6;
  agility = rand()%8+6;
  dexterity = rand()%8+6;
  constitution = rand()%8+6;
  intelligence = rand()%8+6;
  wisdom = rand()%8+6;
  charisma = rand()%8+6;
  name = "nulhar";
  classname = "Fighter";
  HP = SP = MP = HP_max = SP_max = MP_max = 10;
  Class = -1;
}

Character::~Character()
{
                       
}

void Character::IncStat(int i)
{ switch(i)
  { case 0: courage++; break;
    case 1: strength++; break;
    case 2: agility++; break;
    case 3: dexterity++; break;
    case 4: constitution++; break;
    case 5: intelligence++; break;
    case 6: wisdom++; break;
    case 7: charisma++; break;
  }                     
}
