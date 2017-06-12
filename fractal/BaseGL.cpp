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

void BaseGL::key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods)
{
	void* ptr = glfwGetWindowUserPointer(wind);
	double	x, y;
	glfwGetCursorPos(wind, &x, &y);
	x *= 2;
	y *= 2;
	reinterpret_cast<BaseGL*>(ptr)->keyCallback(key, scancode, action, mods, x, y);
}

void BaseGL::mouse_callback(GLFWwindow* wind, int button, int action, int mods)
{
	void* ptr = glfwGetWindowUserPointer(wind);
	double	x, y;
	glfwGetCursorPos(wind, &x, &y);
	x *= 2;
	y *= 2;
	reinterpret_cast<BaseGL*>(ptr)->mouseCallback(button, action, mods, x, y);
}

BaseGL::BaseGL(int width, int height, const std::string& title)
{
	doInit();
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window) throw Exc();
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
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

void BaseGL::getSize(int& width, int& height)
{
	glfwGetFramebufferSize(window, &width, &height);
}

void BaseGL::paint()
{
}

void BaseGL::keyCallback(int key, int scancode, int action, int modes, double x, double y)
{
}

void BaseGL::mouseCallback(int button, int action, int modes, double x, double y)
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
