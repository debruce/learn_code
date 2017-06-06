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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <thread>
#include <mutex>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

int getRand()
{
	static random_device	rd;
	static mt19937			gen(rd());
	static uniform_int_distribution<>	uniform(0,2);

	return uniform(gen);
}

static void error_callback(int error, const char* description)
{
	cerr << description << endl;
	exit(-1);
}

FTGLPolygonFont* font;

void paintText(string str, float scale, float x, float y)
{
    glPushMatrix();
    	glTranslatef(x, y, 0);
    	glScalef(scale/font->LineHeight(), scale/font->LineHeight(), 1.0f);
    	FTBBox bbox = font->BBox(str.c_str());

    	float xf = -(bbox.Lower().X() + bbox.Upper().X()) / 2.0;
    	float yf = -.5 * font->FaceSize();

    	glColor4f(0, 0, 0, 0);
    	glBegin(GL_QUADS);
    		glVertex2f(bbox.Lower().X()+xf, bbox.Lower().Y()+yf);
    		glVertex2f(bbox.Upper().X()+xf, bbox.Lower().Y()+yf);
    		glVertex2f(bbox.Upper().X()+xf, bbox.Upper().Y()+yf);
    		glVertex2f(bbox.Lower().X()+xf, bbox.Upper().Y()+yf);
    	glEnd();

    	glColor3f(1.0f, 1.0f, 1.0f);
    	font->Render(str.c_str(), -1, FTPoint(xf, yf));
    glPopMatrix();
}

#define	ROCK		10
#define	PAPER		20
#define	SCISSORS	30

const char *choiceStr[] = {
		"Rock",
		"Paper",
		"Scissors"
};

const char *winStr[] = {
		"Tie",
		"Computer wins",
		"Human wins"
};

char win[3][3] = {
		{ 0, 1, 2 },
		{ 2, 0, 1 },
		{ 1, 2, 0 }
};

void paintAll(int mode, float xmax)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	if (mode == GL_SELECT) glLoadName(ROCK);
    paintText("Rock", 1, -xmax*.666, 0);

    if (mode == GL_SELECT) glLoadName(PAPER);
    paintText("Paper", 1, 0, 0);

    if (mode == GL_SELECT) glLoadName(SCISSORS);
    paintText("Scissors", 1, xmax*.666, 0);
}

class Hit {
public:
	GLuint	count;
	GLuint	minI;
	GLuint	maxI;
	GLuint	names[];
	float	min()	{ return (float)minI/0xffffffff; };
	float	max()	{ return (float)maxI/0xffffffff; };
	Hit*	next()	{ return (Hit*) ( ((GLuint*)this)+count+3); };
};

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action != 1) return;

	double	x, y;
	glfwGetCursorPos(window, &x, &y);

	int pwidth, pheight, swidth, sheight;
	glfwGetFramebufferSize(window, &pwidth, &pheight);
	glfwGetWindowSize(window, &swidth, &sheight);

	x = x * pwidth / swidth;
	y = y * pheight / sheight;

	GLuint	nameBuffer[512];
	memset(nameBuffer, 0, sizeof(nameBuffer));
	glSelectBuffer(512, nameBuffer);
	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	GLint	viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glm::vec4 vp;
	for (int i = 0; i < 4; i++) vp[i] = viewport[i];
	glm::mat4 pmat = glm::pickMatrix(glm::vec2(x, viewport[3] - y), glm::vec2(5,5), vp);
	glLoadMatrixf(glm::value_ptr(pmat));

	float ratio = viewport[2] / (float) viewport[3];
	glOrtho(-ratio, ratio, -1, 1, 1, -1);

	glInitNames();
	glPushName(0);

	paintAll(GL_SELECT, ratio);

	glFlush();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	int hits = glRenderMode(GL_RENDER);
	if (hits == 0) return;

	float	highDist = -1e38;
	int highName;

	Hit* ptr = (Hit*) nameBuffer;
	for (int i = 0; i < hits; i++) {
		if (ptr->max() > highDist) {
			highDist = ptr->max();
			highName = ptr->names[0];
		}
		ptr = ptr->next();
	}

	int	humanAnswer;
	switch (highName) {
	case ROCK:
		humanAnswer = 0;
		break;
	case PAPER:
		humanAnswer = 1;
		break;
	case SCISSORS:
		humanAnswer = 2;
		break;
	default:
		return;
	}

	int computerAnswer = getRand();
	cout << "Human: " << choiceStr[humanAnswer] << ", Computer: " << choiceStr[computerAnswer] << endl;
	cout << win[humanAnswer][computerAnswer] << ' ' << winStr[win[humanAnswer][computerAnswer]] << endl;
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

	GLFWwindow* window = glfwCreateWindow(640, 480, "Rock, Paper, Scissors", NULL, NULL);
	if (!window) {
	    glfwTerminate();
	    exit(EXIT_FAILURE);
	}

	// glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	font = new FTGLPolygonFont("/Library/Fonts/Zapfino.ttf");
	font->FaceSize(72);

	glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1, 1, 1, -1);

    	glRenderMode(GL_RENDER);
    	paintAll(GL_RENDER, ratio);
    	glFlush();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    exit(0);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
