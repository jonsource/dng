#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include <map>
#include <string>

#define TRIGGER_ENTER 5
#define TRIGGER_LEAVE 6
#define TRIGGER_NORTH 1
#define TRIGGER_EAST 0
#define TRIGGER_SOUTH 3
#define TRIGGER_WEST 2

class TRIGGER
{   public:
        int xpos;
        int zpos;
        int type;
        int w1,h1,w2,h2;
        TRIGGER(int xpos, int zpos, int type, int w1, int h1, int w2, int h2);
        void fire();
};

typedef struct
{   int w1,h1,w2,h2;
    TRIGGER * callback;
} CLICKABLE;

typedef map < string, List <TRIGGER> > TRIGGER_MAP;

#endif // __INTERFACE_H_
