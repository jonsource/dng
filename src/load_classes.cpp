/*
 * Character classes (classes are in-game RPG classes, Classes are C++ Classes) - proof of concept
 */

#include "load_classes.h"

#define CL_NAME   1
#define CL_GENDER  2 
#define CL_STATS  3
#define CL_PARAMS 4

ClassTemplates *Classes;

int *ClassTemplates::Check(Character *ch)
{ int *pass;
  pass = (int *) malloc(size*sizeof(int));
  
  for(int i=0; i<size; i++) 
          pass[i]=Templates[i]->Check(ch);
  return pass;
}

ClassTemplates::ClassTemplates()
{ FILE *f,*log;
  Class *cl,**re;
  string buf;
  char cstr[30];
  
  /* TODO switch from malloc and local implementation to Class List in "list.h"
   */

  Templates = (Class **) malloc(4*sizeof(Class *));
  max=4;
  size=0;
  f=fopen("classes.def","r");
  log=fopen("load_classes.log","w");
  
  while(fscanf(f,"%s",cstr)>-1)
  { buf=cstr;
    fprintf(log,"read: %s\n",buf.c_str());
    if(buf=="class")
    { cl = new Class(f);
      if(size>=max)
      { re = (Class **) malloc(max*2*sizeof(Class *));
        for(int i=0;i<max;i++) re[i]=Templates[i];
        delete []Templates;
        Templates = re; 
        max=max*2;
      }
      Templates[size]=cl;
      size++;
    }
  }
  fclose(f);
  for(int i=0;i<size;i++) fprintf(log,"%s %d %d %d %d %d %d %d %d\n",Templates[i]->name.c_str(),Templates[i]->courage,
  Templates[i]->strength,Templates[i]->agility,Templates[i]->dexterity,Templates[i]->constitution,
  Templates[i]->intelligence,Templates[i]->wisdom,Templates[i]->charisma);
  fclose(log);
}

ClassTemplates::~ClassTemplates()
{ delete []Templates;
                                 
}

Class *ClassTemplates::GetTemplate(string name)
{ int i=size;
  while(i--) if(Templates[i]->name==name)
	  return Templates[i];
  return NULL;
}

Class::Class(FILE *f)
{ char cstr[30];
  int a,b;
  string buf;
  FILE *l;
  
  l=fopen("picus.log","a");
  fscanf(f,"%s",cstr);
  name=cstr;
  while(fscanf(f,"%s",cstr))
  { buf=cstr;
    fprintf(l,"read: %s ",buf.c_str());  
    if(buf=="endclass") break;
    fscanf(f,"%d/%d\n",&a,&b);
    fprintf(l,"rd: %d %d \n",a,b);
    if(buf=="courage") {courage=a; priorities[0]=b; continue;}
    if(buf=="strength") {strength=a; priorities[1]=b; continue;}
    if(buf=="agility") {agility=a; priorities[2]=b; continue;}
    if(buf=="dexterity") {dexterity=a; priorities[3]=b; continue;}
    if(buf=="constitution") {constitution=a; priorities[4]=b; continue;}
    if(buf=="intelligence") {intelligence=a; priorities[5]=b; continue;}
    if(buf=="wisdom") {wisdom=a; priorities[6]=b; continue;}
    if(buf=="charisma") {charisma=a; priorities[7]=b; continue;}
  
    if(buf=="HP") {HP_base=a; HP_rand=b; continue;}
    if(buf=="MP") {MP_base=a; MP_rand=b; continue;}
    if(buf=="SP") {SP_base=a; SP_rand=b; continue;}
    fprintf(l,"error: %s\n",buf.c_str());
  }
  fprintf(l,"st %d %d %d %d %d %d %d %d\n\n",courage,strength,agility,dexterity,
  constitution, intelligence,wisdom,charisma);
  fclose(l);
}

int Class::Check(Character *Ch)
{ 
  if(Ch->courage<courage) return 0;
  if(Ch->strength<strength) return 0;
  if(Ch->agility<agility) return 0;
  if(Ch->dexterity<dexterity) return 0;
  if(Ch->constitution<constitution) return 0;
  if(Ch->intelligence<intelligence) return 0;
  if(Ch->wisdom<wisdom) return 0;
  if(Ch->charisma<charisma) return 0;
  return 1;
}


void Class::NextLevel(Character *Ch)
{ int primary=0;
  int primaries[NO_STATS];
  int secondary=0;
  int secondaries[NO_STATS];
  int tertiary=0;
  int tertiaries[NO_STATS];
  int i;
  int increases;
  
  for(i = 0; i<NO_STATS; i++)
  { if(priorities[i]==1) primaries[primary++]=i;
    if(priorities[i]==2) secondaries[secondary++]=i;
    if(priorities[i]==3) tertiaries[tertiary++]=i;
  }
  switch(rand()%6)
  { case 0: case 1: case 2: increases = 1; break;
    case 3: case 4: increases = 2; break;
    case 5: increases = 3; break;
  }
  while(increases--)
  { switch(rand()%3)
    { case 0: Ch->IncStat(primaries[rand()%primary]); break;
      case 1: Ch->IncStat(secondaries[rand()%secondary]); break;
      case 2: Ch->IncStat(tertiaries[rand()%tertiary]); break;
    }
    
  }
}
  
Class::~Class()
{ 
}
