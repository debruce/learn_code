#include <iostream>
#include <cmath>
#include <vector>

#include "BaseGL.h"
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

void rect(float lx, float ux, float ly, float uy)
{
	glBegin(GL_QUADS);
		glVertex2f(lx, ly);
		glVertex2f(ux, ly);
		glVertex2f(ux, uy);
		glVertex2f(lx, uy);
	glEnd();
}

struct MyGL : public BaseGL {
	MyGL(int width, int height, const std::string& title) : BaseGL(width, height, title)
	{
	}

	void draw()
	{
		pushOrtho();

		const int cnt = 20;

		for (int i = 0; i < cnt; i++) {
			float x = cos(i * M_PI * 2 / cnt);
			float y = sin(i * M_PI * 2 / cnt);
			glPushMatrix();
			glPushName(1+i);
				glColor4f(.5*x+.5, .2, .5*y+.5, 1);
				glTranslatef(.5 * x, .5 * y, (float)i/cnt - .5);
				glScalef(.125, .125, 1);
				rect(-1, 1, -1, 1);
			glPopName();
			glPopMatrix();
		}

		popOrtho();
	}

	void paint()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glEnable(GL_BLEND);
		draw();
		glDisable(GL_BLEND);
	}

	int pick(vector<GLuint>& vec, double x, double y)
	{
		glSelectBuffer(vec.size(), vec.data());
		setPickView(x, y);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);

		draw();

		glFlush();
		return glRenderMode(GL_RENDER);
	}

	void keyCallback(int key, int scancode, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			setClose();
			return;
		}

		vector<GLuint>	selectBuf(1024);
		int hits = pick(selectBuf, x, y);
		Hit* ptr = (Hit*)selectBuf.data();

		cout << "keyCallback key=" << key
			<< " scancode=" << scancode
			<< " modes=" << modes
			<< " x=" << x
			<< " y=" << y
			<< endl;

		for (int i = 0; i < hits; i++) {
			for (int j = 0; j < ptr->count; j++) {
				cout << ptr->names[j] << ' ';
			}
			cout << " min=" << ptr->min() << " max=" << ptr->max() << endl;
			ptr = ptr->next();
		}
		cout << endl << endl;
	}

	void mouseCallback(int button, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return;

		vector<GLuint>	selectBuf(1024);
		int hits = pick(selectBuf, x, y);
		Hit* ptr = (Hit*)selectBuf.data();

		cout << "mouseCallback button=" << button
			<< " modes=" << modes
			<< " x=" << x
			<< " y=" << y
			<< endl;

		for (int i = 0; i < hits; i++) {
			for (int j = 0; j < ptr->count; j++) {
				cout << ptr->names[j] << ' ';
			}
			cout << " min=" << ptr->min() << " max=" << ptr->max() << endl;
			ptr = ptr->next();
		}
		cout << endl << endl;
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
