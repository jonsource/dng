#ifndef MOBILE_H_INCLUDED
#define MOBILE_H_INCLUDED

#include <string>
#include "list.h"

#define ACT_GO_NORTH 0
#define ACT_GO_EAST 1
#define ACT_GO_SOUTH 2
#define ACT_GO_WEST 3

#define ACT_STAND 4

#define ACT_DECIDE 5

#define ACT_FIGHT 6

#define MODE_NO_OVERRIDE -1
#define MODE_WANDER 1
#define MODE_HUNT 2
#define MODE_FIGHT 3

class TEXTURED_ELEMENT;
class ANIMATOR;
class MOBILE;

typedef struct
{   int start_y;
    int width;
    int height;
    int frames;
} SPRITE_MODE;

class SPRITE
{   public:
        BITMAP * sprite;
        List<SPRITE_MODE> Modes;
        SPRITE();
        ~SPRITE();
};

class MOBILE_TEMPLATE
{   public:
        std::string fname;
        SPRITE * sprite;
        //int spr_w, spr_h;
        int speed;
        MOBILE_TEMPLATE();
        ~MOBILE_TEMPLATE();
        MOBILE * Clone();

};

class MOBILE
{   public:
        MOBILE_TEMPLATE * parent;
        int heading;
        float x,y,z;
        TEXTURED_ELEMENT * ele;
        ANIMATOR * ani;
        int speed;
        int mode;
        int last_action, next_action, action;
        int flag_pass;
        int act_progress;
        int act_target;
        MOBILE();
        ~MOBILE();
        int HeartBeat();
        void actionDecide();
        void actionDecide(int mode_override);
        void actionGo();
        void finishAction();
        void Init();
};

SPRITE_MODE * load_sprite_mode(std::string s);

#endif // MOBILE_H_INCLUDED
