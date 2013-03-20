#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include <map>
#include <string>
#include "texture.h"

typedef struct
{   int w1,h1,w2,h2;
    TRIGGER * callback;
} CLICKABLE;

typedef map < string, List<CLICKABLE> > CLICKABLE_MAP;
typedef map < string, List<CLICKABLE> >::iterator CLICKABLE_MAP_ITERATOR;

#endif // __INTERFACE_H_
