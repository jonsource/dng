/*
 * NPC dialogue system - proof of concept
 */

#include "chat.h"
#include <iostream>
#include <string>

using namespace std;

map<string,string> GlobalCast;
map<string,string> NpcCast;

typedef struct 
{ string condition;
  string text;
  string result;
} Theme;

void eval_condition(string s)
{
 }
        
typedef map<string,string> Topic;
typedef map<string,string>::iterator Titer;

void chat_main()
{ map<string,Topic> conversation;
  map<string,Topic>::iterator citer;
  Titer tit;
  string current="default";
  Topic top;
  char c;
  int i;
  top["'default NPC speech"]="NULL";
  top["A) first option - end"]="end";
  top["B) second option - topic1"]="topic1";
  top["C) third option - topic2"]="topic2";
  conversation["default"] = top;
  top.clear();
  top["'topic1 NPC speech"]="NULL";
  top["A) first option - end"]="end";
  top["B) second option - topic2"]="topic2";
  top["C) third option - default"]="default";
  conversation["topic1"] = top;
  top.clear();
  top["'topic2 NPC speech"]="NULL";
  top["A) first option - end"]="end";
  top["B) second option - topic3"]="topic3";
  top["C) third option - default"]="default";
  conversation["topic2"] = top;
  top.clear();
  top["'topic3 NPC speech"]="NULL";
  top["A) first option - end"]="end";
  top["B) second option - topic1"]="topic1";
  conversation["topic3"] = top;
  
  while(current!="end")
  { top=conversation[current];
    i=0;
    for(tit = top.begin(); tit!=top.end(); tit++)
    { if(i==0)
      { cout<< tit->first <<"\n\n"; }
       else cout<< tit->first<<"\n";
      i++;
    }
    cin >> c;
    c-='a';
    c+=1;
    int i=0;
    for(tit = top.begin(); tit!=top.end(); tit++)
    { if(i==c) { current=tit->second; cout <<i<<" "<<current<<"\n";break;}
      i++;
    }
  }
  //system("PAUSE");
  return; 
}

