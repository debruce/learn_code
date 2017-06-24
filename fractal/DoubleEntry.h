#ifndef __DoubleEntry_H__
#define __DoubleEntry_H__

#define GLFW_INCLUDE_GLU
#include "BaseGL.h"
#include "PickStack.h"
#include "MyFont.h"

class DoubleEntry : public BaseObj {
	MyFont*	font;
	MyFont::Pos	pos;
	std::string	lbl;
	int	decWidth;
	int	fracWidth;
	int	pointPos;
	double	minVal;
	double	maxVal;
	double	val;
	int	updatePower;
	bool	doMod;

	void moveLeft();
	void moveRight();
	void rangeCorrect();
	void setDigit(int d);
public:
	DoubleEntry(MyFont* font_, MyFont::Pos pos_, const std::string& lbl_, size_t decWidth_, size_t fracWidth_);
	void setRange(double mn_, double mx_, bool doMod_);
	void adjust(double amount);

	void operator=(double val_);
	operator double();
	void draw(BaseGL* wind);
	bool keyboardCallback(BaseGL& wind, int key, int scancode, int action, int mods);
};

#endif
