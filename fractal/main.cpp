#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <boost/lexical_cast.hpp>

#define GLFW_INCLUDE_GLU
#include "BaseGL.h"
#include "PickStack.h"
#include "MyFont.h"
#include "DoubleEntry.h"

using namespace std;

typedef PickClass<BaseObj>	PickBaseObj;

struct MyGL : public BaseGL {
	GLUquadric*	sphere;
	DoubleEntry*	xrot;
	DoubleEntry*	yrot;
	DoubleEntry*	zrot;
	DoubleEntry*	near;
	DoubleEntry*	far;
	DoubleEntry*	angle;

	MyGL(int width, int height, const std::string& title, MyFont* font) : BaseGL(width, height, title)
	{
		sphere = gluNewQuadric();

		xrot = new DoubleEntry(font, MyFont::UpperLeft, "xrot", 1, 2);
		*xrot = 1;

		yrot = new DoubleEntry(font, MyFont::UpperLeft, "yrot", 1, 2);
		*yrot = 0;

		zrot = new DoubleEntry(font, MyFont::UpperLeft, "zrot", 1, 2);
		*zrot = 0;

		near = new DoubleEntry(font, MyFont::UpperLeft, "near", 3, 1);
		*near = 4;

		far = new DoubleEntry(font, MyFont::UpperLeft, "far", 3, 1);
		*far = 40;

		angle = new DoubleEntry(font, MyFont::UpperLeft, "angle", 3, 1);
		angle->setRange(0, 360, 1);
		*angle = 0;

		// useCursor(true);
		useScroll(true);
	}

	void draw()
	{
		struct timeval	tv;
		gettimeofday(&tv, 0);
		double t = ((tv.tv_sec % 10) + (double)(tv.tv_usec / 1e6)) / 10.0;

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glFrustum(-4*aspect(), 4*aspect(), -4, 4, *near, *far);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0,0,-5);
		// glRotatef(t * 360.0, *xrot, *yrot, *zrot);
		glRotatef(*angle, *xrot, *yrot, *zrot);

			glColor4f(.25,.25,.25,1);
			gluQuadricDrawStyle(sphere, GLU_FILL);
			gluSphere(sphere, 1, 10, 10);

			glColor4f(1,1,1,1);
			gluQuadricDrawStyle(sphere, GLU_LINE);
			gluSphere(sphere, 1, 10, 10);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		pushOrtho();

		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
			glTranslatef(-aspect(), 1, 0);
			glScalef(.1, .1, 1);
			xrot->draw(this);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-aspect(), .9, 0);
			glScalef(.1, .1, 1);
			yrot->draw(this);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-aspect(), .8, 0);
			glScalef(.1, .1, 1);
			zrot->draw(this);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-aspect(), .7, 0);
			glScalef(.1, .1, 1);
			near->draw(this);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-aspect(), .6, 0);
			glScalef(.1, .1, 1);
			far->draw(this);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-aspect(), .5, 0);
			glScalef(.1, .1, 1);
			angle->draw(this);
		glPopMatrix();

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

	void mouseButtonCallback(int button, int action, int mods)
	{
		vector<GLuint>	vec(1024);
		glSelectBuffer(vec.size(), vec.data());

		double x, y;
		getCursor(x, y);
		setPickView(x, y);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(-1);

		draw();

		glFlush();
		int num = glRenderMode(GL_RENDER);

		PickBaseObj	pick(vec, num);
		for (size_t i = 0; i < pick.size(); i++) {
			if (pick[i].mouseButtonCallback(*this, button, action, mods))
				return;
		}
	}

	void keyboardCallback(int key, int scancode, int action, int mods)
	{
		if (key == 344) {
			if (mods == 1) useHand(true);
			else useHand(false);
		}

		vector<GLuint>	vec(1024);
		glSelectBuffer(vec.size(), vec.data());

		double x, y;
		getCursor(x, y);
		setPickView(x, y);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(-1);

		draw();

		glFlush();
		int num = glRenderMode(GL_RENDER);

		PickBaseObj	pick(vec, num);
		for (size_t i = 0; i < pick.size(); i++) {
			if (pick[i].keyboardCallback(*this, key, scancode, action, mods))
				return;
		}
	}

	void cursorCallback(double x, double y)
	{
		cout << "cursor x=" << x << " y=" << y << endl;
	}

	void scrollCallback(double x, double y)
	{
		cout << "scroll x=" << x << " y=" << y << endl;
		angle->adjust(x);
		// xrot->adjust(x);
		// yrot->adjust(y);
	}
};

int main()
{
	try {
		MyFont* font = new MyFont("Tahoma");
		MyGL	myWindow(640, 480, "MyTitle", font);

		while (!myWindow.shouldClose()) {
			myWindow.display();
		}
	}
	catch (std::exception& exc) {
		cerr << "Caught exception: " << exc.what() << endl;
	}

	return 0;
}
