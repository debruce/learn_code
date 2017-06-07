//============================================================================
// Name        : myGlfw.cpp
// Author      : David Bruce
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <FTGL/ftgl.h>
#include <sys/time.h>

#include <thread>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <ctime>
#include <cmath>

#include "MyFont.h"

using namespace std;

static void error_callback(int error, const char* description)
{
	cerr << description << endl;
	exit(-1);
}

static void wsize_callback(GLFWwindow* window, int x, int y)
{
	cout << "wsize " << x << ' ' << y << endl;
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	double	x, y;
	glfwGetCursorPos(window, &x, &y);
	cout << "mouse x=" << x << " y=" << y << " button=" << button << " action=" << action << " mods=" << mods << endl;
}

#if 0
static void cursor_callback(GLFWwindow* window, double x, double y)
{
	cout << "cursor " << x << ' ' << y << endl;
}
#endif

static void enter_callback(GLFWwindow* window, int ans)
{
	double	x, y;
	glfwGetCursorPos(window, &x, &y);
	cout << "enter x=" << x << " y=" << y << " ans=" << ans << endl;
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
	cout << "scroll " << x << ' ' << y << endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	double	x, y;
	glfwGetCursorPos(window, &x, &y);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

	cout << "x=" << x << " y=" << y << ' ';
    if (mods & GLFW_MOD_SHIFT) cout << "shift ";
    if (mods & GLFW_MOD_CONTROL) cout << "control ";
    if (mods & GLFW_MOD_ALT) cout << "alt ";
    if (mods & GLFW_MOD_SUPER) cout << "super ";

    if (isprint(key)) cout << (char)key << endl;
    else {
    	cout << '<' << hex << key << '>' << endl;
    }
}

void showFontTest(MyFont& font, float left, float right, float bottom, float top)
{
	font.paint("lower left", MyFont::LowerLeft, .1, left, bottom);
	font.paint("upper left", MyFont::UpperLeft, .1, left, top);
	font.paint("lower right", MyFont::LowerRight, .1, right, bottom);
	font.paint("upper right", MyFont::UpperRight, .1, right, top);
	font.paint("lower middle", MyFont::LowerMiddle, .1, 0, bottom);
	font.paint("upper middle", MyFont::UpperMiddle, .1, 0, top);
	font.paint("middle", MyFont::Middle, .1, 0, 0);
}

void showMat4(MyFont& font, const glm::mat4& mat, int width, int prec, float sz, float left, float upper)
{
	for (int r = 0; r < 4; r++) {
		stringstream	ss;
		ss << fixed << setprecision(prec);
		for (int c = 0; c < 4; c++) {
			ss << setw(width) << mat[r][c];
			if (c != 3) ss << ", ";
		}
		font.paint(ss.str(), MyFont::LowerLeft, sz, left, upper-sz*r);
	}
}

int main()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	    exit(EXIT_FAILURE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window) {
	    glfwTerminate();
	    exit(EXIT_FAILURE);
	}

	glEnable(GL_BLEND);

	MyFont font("Tahoma");

	glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, wsize_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    // glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetCursorEnterCallback(window, enter_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        struct timeval tv;
        gettimeofday(&tv, 0);
        double r = (((tv.tv_sec % 10) + tv.tv_usec/1e6)/10.0) * 360.0;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glm::mat4 camera = glm::perspective(60.0*M_PI/180, 640.0/480, .1, 100.0);
        camera *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5));
        glLoadMatrixf(glm::value_ptr(camera));

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(r, .5, .5, 0);
        glm::mat4 model;
        glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(model));

       	glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
         	glVertex3f(1, 1, 0);
        	glVertex3f(-1, 1, 0);
        	glVertex3f(-1, -1, 0);
        	glVertex3f(1, -1, 0);
        	glVertex3f(1, 1, 0);
        glEnd();

       	glColor3f(0, 1, 0);
        glBegin(GL_QUADS);
         	glVertex3f(1, 0, 1);
        	glVertex3f(-1, 0, 1);
        	glVertex3f(-1, 0, -1);
        	glVertex3f(1, 0, -1);
        	glVertex3f(1, 0, 1);
        glEnd();

       	glColor3f(0, 0, 1);
        glBegin(GL_QUADS);
         	glVertex3f(0, 1, 1);
        	glVertex3f(0, -1, 1);
        	glVertex3f(0, -1, -1);
        	glVertex3f(0, 1, -1);
        	glVertex3f(0, 1, 1);
        glEnd();

        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.5, 1.5, -1.0f, 1.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor3f(1,1,1);

        showFontTest(font, -1.5, 1.5, -1, 1);

        showMat4(font, model, 5, 2, .07, -1.3, .8);
        showMat4(font, camera, 5, 2, .07, -1.3, .5);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cout << "after loop" << endl;
    exit(0);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
