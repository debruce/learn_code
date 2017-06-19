#include <iostream>
#include <cmath>
#include <vector>
#include <sys/time.h>
#if 0
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

#define GLFW_INCLUDE_GLU
#include "BaseGL.h"
#include "PickStack.h"
#include "MyFont.h"

using namespace std;

struct Color {
	float	red;
	float	green;
	float	blue;
	float	alpha;
};

typedef PickClass<BaseObj>	PickBaseObj;

struct Patch : public BaseObj {
	float	x;
	float	y;
	float	z;
	Color	color;

	void draw(BaseGL* wind)
	{
		glPushMatrix();
		PickBaseObj::push(this);
			if (isSelected()) {
				Color flip = color;
				flip.red = 1 - color.red;
				flip.green = 1 - color.green;
				flip.blue = 1 - color.blue;
				glColor4fv((float*)&flip);
			}
			else {
				glColor4fv((float*)&color);
			}
			glTranslatef(x, y, z);
			glScalef(.125, .125, 1);
			glBegin(GL_QUADS);
				glVertex2f(-1, -1);
				glVertex2f(1, -1);
				glVertex2f(1, 1);
				glVertex2f(-1, 1);
			glEnd();
		PickBaseObj::pop();
		glPopMatrix();
	}

	bool keyboardCallback(BaseGL& wind, int key, int scancode, int action, int modes)
	{
		if (action == GLFW_RELEASE) return true;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			wind.setClose();
			return true;
		}

		cout << "keyCallback key=" << key
			<< " scancode=" << scancode
			<< " modes=" << modes
			<< endl;

		return true;
	}

	bool mouseButtonCallback(BaseGL& wind, int button, int action, int modes)
	{
		if (action == GLFW_RELEASE) return true;

		cout << "mouseCallback button=" << button << " modes=" << modes << endl;
		select();
		return true;
	}
};

struct MyGL : public BaseGL {
	vector<BaseObj*>	objs;
	GLUquadric*	sphere;

	MyGL(int width, int height, const std::string& title) : BaseGL(width, height, title)
	{
		const int cnt = 20;

		for (int i = 0; i < cnt; i++) {
			Patch*	p = new Patch();;
			p->x = .5 * cos(i * M_PI * 2 / cnt);
			p->y = .5 * sin(i * M_PI * 2 / cnt);
			p->z = (float)i/cnt - .5;
			p->deselect();
			p->color.red = p->x + .5;
			p->color.green = .2;
			p->color.blue = p->y + .5;
			p->color.alpha = 1.0;
			objs.push_back(p);
		}

		sphere = gluNewQuadric();
		// useCursor(true);
		// useScroll(true);
	}

	void draw()
	{
		struct timeval	tv;
		gettimeofday(&tv, 0);
		double t = ((tv.tv_sec % 10) + (double)(tv.tv_usec / 1e6)) / 10.0;

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glFrustum(-4*aspect(), 4*aspect(), -4, 4, 4, 40);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0,0,-5);
		glRotatef(t * 360.0, 1, 1, 1);

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
#if 0
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		gluLookAt(1, 1, 1, 0, 0, 0, 0, 0, 1);

			glColor4f(1,1,1,1);
			gluQuadricDrawStyle(sphere, GLU_LINE);
			gluSphere(sphere, .1, 10, 10);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
#endif
		pushOrtho();

		for (auto& p : objs) p->draw(dynamic_cast<BaseGL*>(this));

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

	void mouseButtonCallback(int button, int action, int modes)
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

		for (auto& p : objs) p->deselect();

		PickBaseObj	pick(vec, num);
		for (size_t i = 0; i < pick.size(); i++) {
			if (pick[i].mouseButtonCallback(*this, button, action, modes))
				return;
		}
	}

	void keyboardCallback(int key, int scancode, int action, int modes)
	{
		if (key == 344) {
			if (modes == 1) useHand(true);
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
			if (pick[i].keyboardCallback(*this, key, scancode, action, modes))
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
