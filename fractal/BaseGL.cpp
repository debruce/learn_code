//============================================================================
// Name        : myGlfw.cpp
// Author      : David Bruce
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "BaseGL.h"

using namespace std;

static void error_callback(int error, const char *description) {
  cerr << description << endl;
  exit(-1);
}

void BaseGL::doInit()
{
	static bool done;
	if (!done) {
		glfwSetErrorCallback(error_callback);
		if (!glfwInit()) Exc();
		done = true;
	}
}

void BaseGL::size_callback(GLFWwindow* wind, int w, int h)
{
	BaseGL* obj = reinterpret_cast<BaseGL*>(glfwGetWindowUserPointer(wind));
	obj->aspect_ = (float)w / h;
}

void BaseGL::key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods)
{
	BaseGL* obj = reinterpret_cast<BaseGL*>(glfwGetWindowUserPointer(wind));
	obj->keyCallback(key, scancode, action, mods);
}

void BaseGL::mouse_callback(GLFWwindow* wind, int button, int action, int mods)
{
	BaseGL* obj = reinterpret_cast<BaseGL*>(glfwGetWindowUserPointer(wind));
	obj->mouseCallback(button, action, mods);
}

BaseGL::BaseGL(int width, int height, const std::string& title)
{
	doInit();
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window) throw Exc();
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, size_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	int	fwidth, fheight, swidth, sheight;
	glfwGetWindowSize(window, &swidth, &sheight);
	glfwGetFramebufferSize(window, &fwidth, &fheight);
	xscale = (float)fwidth / swidth;
	yscale = (float)fheight / sheight;
	aspect_ = (float)fwidth / fheight;
	orthoSaveMode = -1;
}

BaseGL::~BaseGL()
{
	glfwDestroyWindow(window);
}

void BaseGL::setClose()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

bool BaseGL::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void BaseGL::getSize(int& width, int& height) const
{
	glfwGetFramebufferSize(window, &width, &height);
}

void BaseGL::getCursor(double& x, double& y) const
{
	glfwGetCursorPos(window, &x, &y);
	x *= xscale;
	y *= yscale;
}

void BaseGL::setPickView(float x, float y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glTranslatef(viewport[2] - 2 * x, -viewport[3] + 2 * y, 0);
	glScalef(viewport[2], viewport[3], 1.0);
}

void BaseGL::paint()
{
}

void BaseGL::keyCallback(int key, int scancode, int action, int modes)
{
}

void BaseGL::mouseCallback(int button, int action, int modes)
{
}

void BaseGL::display()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	paint();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void BaseGL::pushOrtho()
{
	if (orthoSaveMode != -1) {
		cerr << "pushOrtho called when already set" << endl;
		exit(-1);
	}

	glGetIntegerv(GL_MATRIX_MODE, &orthoSaveMode);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(-aspect(), aspect(), -1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void BaseGL::popOrtho()
{
	if (orthoSaveMode == -1) {
		cerr << "popOrtho called when not set" << endl;
		exit(-1);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(orthoSaveMode);
	orthoSaveMode = -1;
}

void BaseObj::select()
{
	selectFlag = true;
}

void BaseObj::deselect()
{
	selectFlag = false;
}

bool BaseObj::keyCallback(BaseGL& wind, int key, int scancode, int action, int modes)
{
	return false;
}

bool BaseObj::mouseCallback(BaseGL& wind, int button, int action, int modes)
{
	return false;
}
