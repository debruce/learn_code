#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <boost/lexical_cast.hpp>

#include "DoubleEntry.h"

using namespace	std;

typedef PickClass<BaseObj>	PickBaseObj;

void DoubleEntry::moveLeft()
{
	updatePower++;
	if (updatePower > decWidth-1) updatePower = decWidth-1;
}

void DoubleEntry::moveRight()
{
	updatePower--;
	if (updatePower < -fracWidth) updatePower = -fracWidth;
}

void DoubleEntry::setDigit(int d)
{
	bool	isNeg = false;
	if (val < 0) {
		isNeg = true;
		val = -val;
	}
	double scale = pow(10.0, updatePower+1);
	int64_t ival = val / scale;
	ival = 10 * ival + d;
	val = ival * pow(10.0, updatePower);
	if (isNeg) val = -val;
	moveRight();
}

DoubleEntry::DoubleEntry(MyFont* font_, MyFont::Pos pos_, const string& lbl_, size_t decWidth_, size_t fracWidth_)
{
	font = font_;
	pos = pos_;
	lbl = lbl_;
	decWidth = decWidth_;
	fracWidth = fracWidth_;
	pointPos = lbl.size() + 2 + decWidth;

	string	n;
	for (int i = 0; i < decWidth; i++) n += '9';
	n += '.';
	for (int i = 0; i < fracWidth; i++) n += '9';
	n += '0';
	maxVal = boost::lexical_cast<double>(n);
	updatePower = 0;
}

void DoubleEntry::operator=(double val_)
{
	val = val_;
}

DoubleEntry::operator double()
{
	return val;
}

void DoubleEntry::draw(BaseGL* wind)
{
	PickBaseObj::push(this);
	float fg[] = {1, 1, 1, 1};
	float bg[] = {.25, .25, .25, 1};
	float hl[] = {.5, 0, 0, 1};
	stringstream	ss;
	ss << lbl << '=';
	ss << fixed << setw(decWidth+fracWidth+2) << setprecision(fracWidth) << val;
	size_t updatePos = pointPos;
	if (updatePower >= 0) updatePos -= updatePower + 1;
	else updatePos -= updatePower;
	font->paintHighlight(ss.str(), pos, fg, bg, updatePos, hl);
	PickBaseObj::pop();
}

bool DoubleEntry::keyboardCallback(BaseGL& wind, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) return true;
#if 0
	if (mods & GLFW_MOD_SHIFT) cout << "shift ";
	if (mods & GLFW_MOD_CONTROL) cout << "control ";
	if (mods & GLFW_MOD_ALT) cout << "alt ";
	if (mods & GLFW_MOD_SUPER) cout << "super ";
	cout << endl;
#endif
	switch (key) {
		case GLFW_KEY_MINUS:
			val = -val;
			break;
		case GLFW_KEY_0:
			setDigit(0);
			break;
		case GLFW_KEY_1:
			setDigit(1);
			break;
		case GLFW_KEY_2:
			setDigit(2);
			break;
		case GLFW_KEY_3:
			setDigit(3);
			break;
		case GLFW_KEY_4:
			setDigit(4);
			break;
		case GLFW_KEY_5:
			setDigit(5);
			break;
		case GLFW_KEY_6:
			setDigit(6);
			break;
		case GLFW_KEY_7:
			setDigit(7);
			break;
		case GLFW_KEY_8:
			setDigit(8);
			break;
		case GLFW_KEY_9:
			setDigit(9);
			break;
		case GLFW_KEY_LEFT:
			moveLeft();
			break;
		case GLFW_KEY_RIGHT:
			moveRight();
			break;
		case GLFW_KEY_UP:
			val += pow(10.0, updatePower);
			if (val > maxVal) val = maxVal;
			break;
		case GLFW_KEY_DOWN:
			val -= pow(10.0, updatePower);
			if (val < -maxVal) val = -maxVal;
			break;

		default:
			break;
	}
	return true;
}
