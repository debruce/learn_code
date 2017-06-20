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

	void paint(const std::string& str, Pos p);
	void paintBackground(const std::string& str, Pos p, const float fg[4], const float bg[4]);
	void paintHighlight(const std::string& str, Pos p, const float fg[4], const float bg[4], size_t, const float hl[4]);
};

#endif
