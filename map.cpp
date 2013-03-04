/*
 * map.cpp
 *
 *  Created on: 2.3.2013
 *      Author: tzeentch
 */

#include "map.h"
#include "game.h"
#include "texture.h"
#include <string>

using namespace std;

unsigned short int MAP_SIZE;
int **map;

extern char chbuf[256];

string get_line(FILE * f)
{	string ret="";
	ret=fgets(chbuf,256,f);
	if(ret.find("#")==0) // # at the beginning of new line
		return "";	// is comment go-on
	ret=ret.substr(0,ret.size()-1);
	return ret;
}

int load_map(string fname)
{	char chbuf[256];
	string str1, str2;
	int mapsize;
	int tile;
	FILE *f=fopen(fname.c_str(),"r");
	if(!f)
	{	debug("Map "+fname+" not found!\n",10);
		exit(0);
	}
	while(!feof(f))
	{ 	str1=get_line(f);
		if(str1.find(":")==0) // : at the beginning of new line
		{				// is new block
			if(str1.compare(":mapsize")==0)
			{	str2=get_line(f);
				if(sscanf(str2.c_str(),"%d",&MAP_SIZE))
				{	delete []map;
					map = (int **) malloc(MAP_SIZE*sizeof(int *));
					for(int i=0; i<MAP_SIZE; i++)
					{	map[i]= (int*) malloc(MAP_SIZE*sizeof(int));
						for(int j=0; j<MAP_SIZE; j++)
						map[i][j]=0;
					}
					debug("Map size: "+to_str(MAP_SIZE));
				}
				else
				{ 	debug("Error reading map size in "+fname+".");
					exit(0);
				}
			}
			if(str1.compare(":map")==0)
			{	int i=0,j,found;
				while(!feof(f) && i<MAP_SIZE)
				{	str2=get_line(f);
					j=0;
					debug("str2 "+str2);
					while(sscanf(str2.c_str(),"%d,",&tile) && j<MAP_SIZE)
					{	map[i][j++]=tile;
					 	debug("Read tile ["+to_str(i)+","+to_str(j)+"]="+to_str(tile),1);
					  	found=str2.find_first_of(",");
					  	str2=str2.substr(found+1);
					}
					i++;
				}
				if(i==MAP_SIZE && j==MAP_SIZE)
				{	debug("Map read successfully");

				}
			}
			if(str1.compare(":textures")==0)
			{ 	debug("Loading textures");
				while(!feof(f))
			    { 	str2=get_line(f);
					if(str2.find(":")==0)
					{	debug("Done loading textures");
						str1=str2; break;
					} //next part of definitions
			        add_texture(load_texture(str2));
				}
			}
			if(str1.compare(":animators")==0)
			{ 	debug("Loading animators");
				int speed, offset, frames, w, h;

				while(!feof(f))
				{ 	str2=get_line(f);
					if(str2.find(":")==0)
					{	debug("Done loading animators");
						str1=str2; break;
					} //next part of definitions
					if(sscanf(str2.c_str(),"%d %d %d %d %d",&speed,&offset,&frames,&w,&h)<5)
					{ debug("Not enough parameters.");
					  exit(1);
					}
					add_animator(create_animator(speed,offset,frames,w,h));
				}
			}
			if(str1.compare(":elements")==0)
			{ 	debug("Loading elements");
				char type[20], transparent[10];
				float x, y, z, w, h;
				int texture, animator;

				while(!feof(f))
				{ 	str2=get_line(f);
					if(str2.find(":")==0)
					{	debug("Done loading elements");
						str1=str2; break;
					} //next part of definitions
					if(sscanf(str2.c_str(),"%s %f %f %f %f %f %s %d %d",type,&x,&y,&z,&w,&h,transparent,&texture,&animator)<9)
					{ debug("Not enough parameters.");
					  exit(1);
					}
					add_element(create_element(type,x,y,z,w,h,transparent,texture,animator));
				}
			}

			if(str1.compare(":end")==0)
			{ 	debug("End of definitions"); break; }
		}
	}
	exit(1);
/*
	  if(str1==":animators")
	  {  debug("Loading animators\n");
		 int speed, offset, frames,w,h,count;
		 while( (count=fscanf(f,"%s %d %d %d %d",chbuf,&offset,&frames,&w,&h)) )
		 {  str2=chbuf;
			if(str2.find(":")==0)  //next part of definitions
	 		{ str1=str2; break; }
	 	    if(count<5)
	 	    { debug("Not enough arguments, skipping\n",8);
			  continue;
		    }
	 	    if(!sscanf(str2.c_str(),"%d",&speed))
	 	    {	debug("Invalid speed, should be integer\n");
	 	    	speed=100;
	 	    }
		   ANIMATOR * ntx = create_animator(speed,offset,frames,w,h);
		   add_animator(ntx);
	 	}
	  }
	  if(str1==":elements")
	  { debug("Loading elements\n");
		float x,y,z,w,h;
	    int txt, anim, count;
	    char chbuf2[64];
		while( (count=fscanf(f,"%s %f %f %f %f %f %s %d %d",chbuf,&x,&y,&z,&w,&h,chbuf2,&txt,&anim)) )
	    {  str2=chbuf;
		   if(str2.find(":")==0)  //next part of definitions
           { str1=str2; break; }
	       if(count<9)
	       { debug("Not enough arguments, skiping\n",8);
	         continue;
	       }
		   TEXTURED_ELEMENT * ntx = create_element(chbuf,x,y,z,w,h,chbuf2,txt,anim);
		   add_element(ntx);
	    }
	  }
	}*/
	return 1;
}



