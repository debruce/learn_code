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
#include <FTGL/ftgl.h>
#include <sys/time.h>

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

float clr = 1.0;

static void worker()
{
	for (;;) {
		sleep(1);
		clr = .5;
		sleep(1);
		clr = 1.0;
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

	glEnable(GL_BLEND);

	FTGLTextureFont font("/Library/Fonts/Zapfino.ttf");
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
#if 0
        time_t	now;
        time(&now);
        struct tm tm;
        localtime_r(&now, &tm);

        float hourAngle = tm.tm_hour * (360/12) - 90;
        float minuteAngle =	tm.tm_min * (360/60) - 90;
        float secondAngle = tm.tm_sec * (360/60) - 90;
#endif
        struct timeval tv;
        gettimeofday(&tv, 0);
        double r = (((tv.tv_sec % 10) + tv.tv_usec/1e6)/10.0) * 360.0;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        // glLoadIdentity();
        // glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        {
        	using namespace	glm;
        	mat4 pm = perspective(60.0*M_PI/180, 640.0/480, .1, 100.0);
        	pm *= translate(mat4(1.0f), vec3(0.0f, 0.0f, -5));
        	glLoadMatrixf(value_ptr(pm));
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(r, .5, .5, 0);

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

#if 0
        glPushMatrix();
        	glScalef(.1*faceScale,.1*faceScale,1.0f);
        	glColor3f(clr, 1.0f, 1.0f);
        	font.Render("Hello World!");
        glPopMatrix();

        glPushMatrix();
			glRotatef(secondAngle, 0.f, 0.f, -1.f);
			glBegin(GL_TRIANGLES);
				glColor3f(.5f, .5f, .5f);
				glVertex3f(0.f, -0.01f, 0.f);
				glVertex3f(0.f, 0.01f, 0.f);
				glColor3f(1.f, 1.f, 1.f);
				glVertex3f(1.f, 0.f, 0.f);
			glEnd();
		glPopMatrix();

        glPushMatrix();
			glRotatef(minuteAngle, 0.f, 0.f, -1.f);
			glBegin(GL_TRIANGLES);
				glColor3f(1.f, .0f, .0f);
				glVertex3f(0.f, -0.01f, 0.f);
				glVertex3f(0.f, 0.01f, 0.f);
				glColor3f(1.f, 1.f, 1.f);
				glVertex3f(.9f, 0.f, 0.f);
			glEnd();
		glPopMatrix();

        glPushMatrix();
			glRotatef(hourAngle, 0.f, 0.f, -1.f);
			glBegin(GL_TRIANGLES);
				glVertex3f(0.f, -0.01f, 0.f);
				glVertex3f(0.f, 0.01f, 0.f);
				glVertex3f(.9f, 0.f, 0.f);
			glEnd();
		glPopMatrix();
#endif
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cout << "after loop" << endl;
    exit(0);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
