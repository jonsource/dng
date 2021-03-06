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
class SPRITE_MODE;
class SPRITE;

class MOBILE_TEMPLATE
{   public:
        std::string fname;
        SPRITE * sprite;
        //int spr_w, spr_h;
        int speed;
        int template_nr;
        MOBILE_TEMPLATE();
        ~MOBILE_TEMPLATE();
        MOBILE * Clone();
        MOBILE * CloneAt(int x, int z);
        int ResetClone(MOBILE * mob);
};

class MOBILE
{   public:
        MOBILE_TEMPLATE * parent;
        int parent_nr;
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
        std::string serialize();
        std::string save_string();
};

SPRITE_MODE * load_sprite_mode(std::string s);
int load_mobile_save(MOBILE * mob, std::string s);
MOBILE_TEMPLATE* load_mobile_template(std::string fname);

#endif // MOBILE_H_INCLUDED
