#ifndef TEXTURED_ELEMENT_H_INCLUDED
#define TEXTURED_ELEMENT_H_INCLUDED

#include <string>

class TEXTURE;
class ANIMATOR;

class TEXTURED_ELEMENT
{   public:
		float x,y,z,w,h;
		int type,flip;
		int transparent,clip;
		int texture_nr;
		TEXTURE * texture;
		int animator_nr;
		ANIMATOR * animator;
		TEXTURED_ELEMENT(std::string type, float x, float y, float z, float w, float h, std::string transparent, int texture, int animator, std::string clip, std::string flip);
		TEXTURED_ELEMENT(std::string);
		std::string serialize();
		std::string type_string();
		static int type_resolve(std::string s);
};

#endif // TEXTURED_ELEMENT_H_INCLUDED
