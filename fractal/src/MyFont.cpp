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

static FTPoint getOffset(MyFont::Pos p, const FTBBox& bbox)
{
	float xMiddle = .5 * (bbox.Lower().X() + bbox.Upper().X());
	float yMiddle = .5 * (0 + bbox.Upper().Y());

	switch (p) {
		case MyFont::LowerLeft:
		return FTPoint(-bbox.Lower().X(), 0);
		case MyFont::UpperLeft:
		return FTPoint(-bbox.Lower().X(), -bbox.Upper().Y());
		case MyFont::LowerRight:
		return FTPoint(-bbox.Upper().X(), 0);
		case MyFont::UpperRight:
		return FTPoint(-bbox.Upper().X(), -bbox.Upper().Y());
		case MyFont::LowerMiddle:
		return FTPoint(-xMiddle, 0);
		case MyFont::UpperMiddle:
		return FTPoint(-xMiddle, -bbox.Upper().Y());
		case MyFont::Middle:
		return FTPoint(-xMiddle, -yMiddle);
	};
}

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

void MyFont::paint(const string& str, Pos p)
{
	float sz = 1.0 / font->FaceSize();
	FTBBox bbox = font->BBox(str.c_str());
	FTPoint off = getOffset(p, bbox);

	int mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glScalef(sz, sz, 1);
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

void MyFont::paintBackground(const string& str, Pos p, const float fg[4], const float bg[4])
{
	float sz = 1.0 / font->FaceSize();
	FTBBox bbox = font->BBox(str.c_str());
	FTPoint off = getOffset(p, bbox);

	int mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glScalef(sz, sz, 1);
		glPushAttrib(GL_POLYGON_BIT|GL_ENABLE_BIT|GL_CURRENT_BIT);

		glColor4fv(bg);
		glBegin(GL_QUADS);
			glVertex2f(bbox.Lower().X() + off.X(), bbox.Lower().Y() + off.Y());
			glVertex2f(bbox.Lower().X() + off.X(), bbox.Upper().Y() + off.Y());
			glVertex2f(bbox.Upper().X() + off.X(), bbox.Upper().Y() + off.Y());
			glVertex2f(bbox.Upper().X() + off.X(), bbox.Lower().Y() + off.Y());
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glFrontFace(GL_CCW);
		glColor4fv(fg);
		font->Render(str.c_str(), -1, off, FTPoint(0,0), FTGL::RENDER_FRONT);

		glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(mode);
}
