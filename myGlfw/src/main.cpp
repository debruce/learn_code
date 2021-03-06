//============================================================================
// Name        : myGlfw.cpp
// Author      : David Bruce
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <FTGL/ftgl.h>

#include <thread>
#include <mutex>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
using namespace std;

static void error_callback(int error, const char* description)
{
	cerr << description << endl;
	exit(-1);
}

float scale = 1.0;

static void worker()
{
	for (;;) {
		sleep(1);
		scale = 1.0;
		sleep(1);
		scale = .5;
	}
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

	FTGLTextureFont font("/Library/Fonts/Arial Bold.ttf");
	float	faceScale = 1/72.0f;
	font.FaceSize(72);

	thread	t1(worker);

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

        time_t	now;
        time(&now);
        struct tm tm;
        localtime_r(&now, &tm);

        float hourAngle = tm.tm_hour * (360/12) - 90;
        float minuteAngle =	tm.tm_min * (360/60) - 90;
        float secondAngle = tm.tm_sec * (360/60) - 90;

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushMatrix();
        	glScalef(.1*faceScale,.1*faceScale,1.0f);
        	font.Render("Hello World!");
        glPopMatrix();

        glPushMatrix();
			glRotatef(secondAngle, 0.f, 0.f, -1.f);
			glBegin(GL_LINES);
				glVertex3f(0.f, 0.f, 0.f);
				glVertex3f(1.f, 0.f, 0.f);
			glEnd();
		glPopMatrix();

        glPushMatrix();
			glRotatef(minuteAngle, 0.f, 0.f, -1.f);
			glBegin(GL_LINES);
				glVertex3f(0.f, 0.f, 0.f);
				glVertex3f(.7f, 0.f, 0.f);
			glEnd();
		glPopMatrix();

        glPushMatrix();
			glRotatef(hourAngle, 0.f, 0.f, -1.f);
			glBegin(GL_LINES);
				glVertex3f(0.f, 0.f, 0.f);
				glVertex3f(.3f, 0.f, 0.f);
			glEnd();
		glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cout << "after loop" << endl;
    exit(0);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
