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

	float xLeft = bbox.Lower().X();
	float xRight = bbox.Upper().X();
	float yBottom = bbox.Lower().Y();
	float yTop = bbox.Upper().Y();

	switch (p) {
		case MyFont::LowerLeft:
		return FTPoint(-xLeft, 0);
		case MyFont::UpperLeft:
		return FTPoint(-xLeft, -yTop);
		case MyFont::LowerRight:
		return FTPoint(-xRight, 0);
		case MyFont::UpperRight:
		return FTPoint(-xRight, -yTop);
		case MyFont::LowerMiddle:
		return FTPoint(-xMiddle, 0);
		case MyFont::UpperMiddle:
		return FTPoint(-xMiddle, -yTop);
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

	float zoff = -.000001;

	float xLeft = bbox.Lower().X() + off.X();
	float xRight = bbox.Upper().X() + off.X();

	float yBottom = bbox.Lower().Y() + off.Y();
	float yTop = bbox.Upper().Y() + off.Y();

	int mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glScalef(sz, sz, 1);

		glColor4fv(bg);
		glBegin(GL_QUADS);
			glVertex3f(xLeft, yBottom, zoff);
			glVertex3f(xLeft, yTop, zoff);
			glVertex3f(xRight, yTop, zoff);
			glVertex3f(xRight, yBottom, zoff);
		glEnd();

		glPushAttrib(GL_POLYGON_BIT|GL_ENABLE_BIT|GL_CURRENT_BIT);
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

void MyFont::paintHighlight(const string& str, Pos p, const float fg[4], const float bg[4], size_t idx, const float hl[4])
{
	if (idx >= str.size() - 1) idx = str.size() - 1;

	float sz = 1.0 / font->FaceSize();
	FTBBox bbox = font->BBox(str.c_str());
	FTPoint off = getOffset(p, bbox);

	float zoff = -.000001;

	float xLeft = bbox.Lower().X() + off.X();
	float xRight = bbox.Upper().X() + off.X();

	float yBottom = bbox.Lower().Y() + off.Y();
	float yTop = bbox.Upper().Y() + off.Y();

	FTBBox ibox = font->BBox(str.substr(0,idx).c_str());
	FTBBox obox = font->BBox(str.substr(0,idx+1).c_str());

	float xhleft = ibox.Upper().X() + off.X();
	float xhright = obox.Upper().X() + off.X();

	int mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glScalef(sz, sz, 1);

		glBegin(GL_QUADS);
			glColor4fv(bg);
			glVertex3f(xLeft, yBottom, zoff);
			glVertex3f(xLeft, yTop, zoff);
			glVertex3f(xhleft, yTop, zoff);
			glVertex3f(xhleft, yBottom, zoff);

			glColor4fv(hl);
			glVertex3f(xhleft, yBottom, zoff);
			glVertex3f(xhleft, yTop, zoff);
			glVertex3f(xhright, yTop, zoff);
			glVertex3f(xhright, yBottom, zoff);

			glColor4fv(bg);
			glVertex3f(xhright, yBottom, zoff);
			glVertex3f(xhright, yTop, zoff);
			glVertex3f(xRight, yTop, zoff);
			glVertex3f(xRight, yBottom, zoff);
		glEnd();

		glPushAttrib(GL_POLYGON_BIT|GL_ENABLE_BIT|GL_CURRENT_BIT);
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
