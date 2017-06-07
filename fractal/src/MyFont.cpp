/*
 * MyFont.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: robert
 */


#include <GLFW/glfw3.h>
#include <iostream>
#include "MyFont.h"

using namespace std;

MyFont::MyFont(const string& fname)
{
	string path = "/Library/Fonts/" + fname + ".ttf";
	font = new FTTextureFont(path.c_str());
	if (font->Error()) {
		cerr << "Failed to load font \"" << path << "\"" << endl;
		exit(-1);
	}
	font->FaceSize(24);
	font->UseDisplayList(true);
}

MyFont::~MyFont()
{
	delete font;
}

void MyFont::paint(const string& str, Pos p, float sz, float x, float y, float z)
{
	int mode;

	glGetIntegerv(GL_MATRIX_MODE, &mode);

	FTBBox bbox = font->BBox(str.c_str());
	sz /= font->FaceSize();

	float xMiddle = .5 * (bbox.Lower().X() + bbox.Upper().X());
	// float yMiddle = .5 * (bbox.Lower().Y() + bbox.Upper().Y());
	float yMiddle = .5 * (0 + bbox.Upper().Y());

	FTPoint off;
	switch (p) {
	case LowerLeft:
		// off = FTPoint(-bbox.Lower().X(), -bbox.Lower().Y());
		off = FTPoint(-bbox.Lower().X(), 0);
		break;
	case UpperLeft:
		off = FTPoint(-bbox.Lower().X(), -bbox.Upper().Y());
		break;
	case LowerRight:
		// off = FTPoint(-bbox.Upper().X(), -bbox.Lower().Y());
		off = FTPoint(-bbox.Upper().X(), 0);
		break;
	case UpperRight:
		off = FTPoint(-bbox.Upper().X(), -bbox.Upper().Y());
		break;
	case LowerMiddle:
		// off = FTPoint(-xMiddle, -bbox.Lower().Y());
		off = FTPoint(-xMiddle, 0);
		break;
	case UpperMiddle:
		off = FTPoint(-xMiddle, -bbox.Upper().Y());
		break;
	case Middle:
		off = FTPoint(-xMiddle, -yMiddle);
		break;
	};

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(x, y, z);
		glScalef(sz, sz, 1);
		// glTranslatef(-off.Xf(), -off.Yf(), 0);
		glPushAttrib(GL_POLYGON_BIT|GL_ENABLE_BIT|GL_CURRENT_BIT);
		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glFrontFace(GL_CCW);
		font->Render(str.c_str(), -1, off, FTPoint(0,0), FTGL::RENDER_FRONT);
		glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(mode);
}
