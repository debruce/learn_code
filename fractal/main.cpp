//============================================================================
// Name        : myGlfw.cpp
// Author      : David Bruce
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#define GLFW_INCLUDE_GLU
#include <FTGL/ftgl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sys/time.h>

#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <exception>
#include <unistd.h>

#include "MyFont.h"

using namespace std;

struct Hit {
	GLuint	count;
	GLuint	minI;
	GLuint	maxI;
	GLuint	names[];
	float min()	{ return (float)minI / (float)0xffffffff; }
	float max()	{ return (float)maxI / (float)0xffffffff; }
	Hit* next()	{ return (Hit*) ( ((GLuint*)this)+count+3); }
};

static void error_callback(int error, const char *description) {
  cerr << description << endl;
  exit(-1);
}

class BaseGL {
	GLFWwindow *window;

	static void doInit()
	{
		static bool done;
		if (!done) {
			glfwSetErrorCallback(error_callback);
			if (!glfwInit()) Exc();
			done = true;
		}
	}

	static void key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods)
	{
		void* ptr = glfwGetWindowUserPointer(wind);
		double	x, y;
		glfwGetCursorPos(wind, &x, &y);
		x *= 2;
		y *= 2;
		reinterpret_cast<BaseGL*>(ptr)->keyCallback(key, scancode, action, mods, x, y);
	}

	static void mouse_callback(GLFWwindow* wind, int button, int action, int mods)
	{
		void* ptr = glfwGetWindowUserPointer(wind);
		double	x, y;
		glfwGetCursorPos(wind, &x, &y);
		x *= 2;
		y *= 2;
		reinterpret_cast<BaseGL*>(ptr)->mouseCallback(button, action, mods, x, y);
	}
public:
	struct Exc : public runtime_error {
		Exc() : runtime_error("GLFW") {}
	};

	BaseGL(int width, int height, const std::string& title)
	{
		doInit();
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		if (!window) throw Exc();
		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_callback);
	}

	~BaseGL()
	{
		glfwDestroyWindow(window);
	}

	void setClose()
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	bool shouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void getSize(int& width, int& height)
	{
		glfwGetFramebufferSize(window, &width, &height);
	}

	virtual void paint()
	{
	}

	virtual void keyCallback(int key, int scancode, int action, int modes, double x, double y)
	{
	}

	virtual void mouseCallback(int button, int action, int modes, double x, double y)
	{
	}

	void display()
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		paint();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
};

struct MyGL : public BaseGL {
	MyGL(int width, int height, const std::string& title) : BaseGL(width, height, title)
	{
	}

	void draw()
	{
		double ratio;
		int width, height;
		getSize(width, height);
		ratio = width / (float)height;

		glMatrixMode(GL_PROJECTION);
		glOrtho(-ratio, ratio, -1.0f, 1.0f, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_BLEND);

		glPushName(1);
		glColor4f(1,0,0,1);
		glBegin(GL_QUADS);
			glVertex2f(-ratio, -1);
			glVertex2f(0, -1);
			glVertex2f(0, 0);
			glVertex2f(-ratio, 0);
		glEnd();
		glPopName();

		glPushName(2);
		glColor4f(1,1,0,1);
		glBegin(GL_QUADS);
			glVertex2f(-ratio, 0);
			glVertex2f(0, 0);
			glVertex2f(0, 1);
			glVertex2f(-ratio, 1);
		glEnd();
		glPopName();

		glPushName(3);
		glColor4f(0,1,0,1);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(ratio, 0);
			glVertex2f(ratio, 1);
			glVertex2f(0, 1);
		glEnd();
		glPopName();

		glPushName(4);
		glColor4f(0,1,1,1);
		glBegin(GL_QUADS);
			glVertex2f(0, -1);
			glVertex2f(ratio, -1);
			glVertex2f(ratio, 0);
			glVertex2f(0, 0);
		glEnd();
		glPopName();

		glDisable(GL_BLEND);
	}

	void paint()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		draw();
	}

	void keyCallback(int key, int scancode, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			setClose();
			return;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLint v[4];
		glGetIntegerv(GL_VIEWPORT, v);
		glTranslatef(v[2] - 2 * x, -v[3] + 2 * y, 0);
		glScalef(v[2], v[3], 1.0);

		glInitNames();
		glPushName(0);

		GLuint selectBuf[1024];
		memset(selectBuf, 0, sizeof(selectBuf));
		glSelectBuffer(sizeof(selectBuf)/sizeof(selectBuf[0]), selectBuf);

		glRenderMode(GL_SELECT);

		draw();

		glFlush();
		int hits = glRenderMode(GL_RENDER);
		Hit* ptr = (Hit*)selectBuf;
		cout << "hits=" << hits << endl;
		for (int i = 0; i < hits; i++) {
			cout << "\tcount=" << ptr->count << ' ';
			for (int j = 0; j < ptr->count; j++) {
				cout << ptr->names[j] << ' ';
			}
			cout << endl;
			ptr = ptr->next();
		}
		cout << endl << endl;
	}

	void mouseCallback(int button, int action, int modes, double x, double y)
	{
		cout << "mouse button=" << button << " action=" << action << " x=" << x << " y=" << y << endl;
	}
};

int main()
{
	try {
		MyGL	myWindow(640, 480, "MyTitle");

		while (!myWindow.shouldClose()) {
			myWindow.display();
		}
	}
	catch (std::exception& exc) {
		cerr << "Caught exception: " << exc.what() << endl;
	}

	return 0;
}
