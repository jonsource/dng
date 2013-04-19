/***
edittext.cpp
Author: crbarry@mts.net

*/

#include <allegro.h>
#include <string>
#include "edittext.h"

using namespace std;

#define WHITE makecol(255, 255, 255)

string  edittext;                         // an empty string for editting
string::iterator iter = edittext.begin(); // string iterator
int caret  = 0;                       // tracks the text caret
bool insert = true;                    // true if text should be inserted

   // the game loop

void text_input()
{
    //if(!keypressed()) return;
    int  newkey   = readkey();
    char ASCII    = newkey & 0xff;
    char scancode = newkey >> 8;

    // a character key was pressed; add it to the string
    if(ASCII >= 32 && ASCII <= 126)
    {
            // add the new char, inserting or replacing as need be
            if(insert || iter == edittext.end())
               iter = edittext.insert(iter, ASCII);
            else
               edittext.replace(caret, 1, 1, ASCII);

            // increment both the caret and the iterator
            caret++;
            iter++;
    }
    else    // some other, "special" key was pressed; handle it here
    switch(scancode)
    {   case KEY_DEL:
                  if(iter != edittext.end()) iter = edittext.erase(iter);
               break;
        case KEY_BACKSPACE:
                  if(iter != edittext.begin())
                  {
                     caret--;
                     iter--;
                     iter = edittext.erase(iter);
                  }
               break;
        case KEY_RIGHT:
                  if(iter != edittext.end())   caret++, iter++;
               break;
        case KEY_LEFT:
                  if(iter != edittext.begin()) caret--, iter--;
               break;
        case KEY_INSERT:
                  insert = !insert;
               break;
        case KEY_ENTER:
                  caret = 0;
                  iter = edittext.begin();
                  edittext.clear();
               break;
        default:
               break;
    }

}

void text_output(BITMAP *buffer)
{   // output the string to the screen
      textprintf_ex(buffer, font, 0, 20, WHITE, -1, edittext.c_str());
      // output some stats using Allegro's printf functions

      textprintf_ex(buffer, font,  0, 30, WHITE, -1, "length:   %d", edittext.length());
      textprintf_ex(buffer, font,  0, 40, WHITE, -1, "capacity: %d", edittext.capacity());
      textprintf_ex(buffer, font,  0, 50, WHITE, -1, "empty?:   %d", edittext.empty());
      if(insert)
         textprintf_ex(buffer, font, 0, 60, WHITE, -1, "Inserting");
      else
         textprintf_ex(buffer, font, 0, 70, WHITE, -1, "Replacing");
      // draw the caret
      vline(buffer, caret * 8, 20, 30, WHITE);

      // blit to screen
      //blit(buffer, screen, 0, 0, 0, 0, 320, 240);
}
