#include "allegro.h"
#include "sound.h"

SAMPLE * sample1;

void init_sound()
{   install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,"");
    sample1 = load_wav("data/sfx/switch9.wav");
}

void deinit_sound()
{   remove_sound();

}

void play_sound()
{
    play_sample(sample1,128,128,1000,0);
}
