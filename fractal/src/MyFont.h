#ifndef __MyFont_H__
#define __MyFont_H__

#include <FTGL/ftgl.h>
#include <string>

class MyFont {
	FTTextureFont*	font;
public:
	enum Pos {
		LowerLeft,
		UpperLeft,
		LowerRight,
		UpperRight,
		LowerMiddle,
		UpperMiddle,
		Middle
	};

	MyFont(const std::string& fname);

	~MyFont();

	void paint(const std::string& str, Pos p, float sz, float x, float y, float z = 0);
};

#endif
