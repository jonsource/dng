#ifndef INTERFACE_H_
#define INTERFACE_H_
#include <map>
#include <string>
#include "game_lib.h"

class ANIMATOR;

class TRIGGER
{
    private:
        void ConstructTrigger(int type, int xpos, int zpos, int w1, int h1, int w2, int h2,int animator, string action, void(*callback)(int mw, int mh));
    public:
        int xpos;
        int zpos;
        int type;
        int w1,h1,w2,h2;
        int animator_nr;
        void (*callback) (int mw, int mh);
        ANIMATOR * animator;
        TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, int animator);
        TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, std::string action);
        TRIGGER(int type, int xpos, int zpos, int w1, int h1, int w2, int h2, int animator, std::string action);
        TRIGGER(int type, void (*callback)(int mw, int mh));
        void fire();
        void fire(int mw, int mh);
        std::string serialize();
        std::string type_string();
        static int type_resolve(std::string type);
        std::string * action;

};

class CLICKABLE
{   public:
        CLICKABLE();
        CLICKABLE(int w1, int h1, int w2, int h2, void (*callback)(int mw, int mh));
        int w1,h1,w2,h2;
        void (*callback)(int mw, int mh);
        TRIGGER * trigger;
};

typedef std::map < std::string, List<CLICKABLE> > CLICKABLE_MAP;
typedef std::map < std::string, List<CLICKABLE> >::iterator CLICKABLE_MAP_ITERATOR;

void mouse_click(int mw, int mh);
void fire_triggers(string s);
void apply_local_triggers(int x, int z, int h);
void clear_triggers(string s);
TRIGGER * load_trigger(std::string s);

#endif // __INTERFACE_H_
