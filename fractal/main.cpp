#include <iostream>
#include <cmath>
#include <vector>

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

	bool keyCallback(BaseGL& wind, int key, int scancode, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return true;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			wind.setClose();
			return true;
		}

		cout << "keyCallback key=" << key
			<< " scancode=" << scancode
			<< " modes=" << modes
			<< " x=" << x
			<< " y=" << y
			<< endl;

		return true;
	}

	bool mouseCallback(BaseGL& wind, int button, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return true;

		cout << "mouseCallback button=" << button << " modes=" << modes << endl;
		select();
		return true;
	}
};

struct MyGL : public BaseGL {
	vector<BaseObj*>	objs;

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
	}

	void draw()
	{
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

	void keyCallback(int key, int scancode, int action, int modes, double x, double y)
	{
#if 0
		vector<GLuint>	vec(1024);
		glSelectBuffer(vec.size(), vec.data());

		setPickView(x, y);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(-1);

		draw();

		glFlush();
		int num = glRenderMode(GL_RENDER);

		vector<DepthNode> ptrs = pickSort(vec, num);

		for (auto& p : objs) p->deselect();

		for (auto& p : ptrs) {
			if (reinterpret_cast<BaseObj*>(p.ptr)->keyCallback(*this, key, scancode, action, modes, x, y))
				return;
		}
#endif
	}

	void mouseCallback(int button, int action, int modes, double x, double y)
	{
		vector<GLuint>	vec(1024);
		glSelectBuffer(vec.size(), vec.data());

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
			if (pick[i].mouseCallback(*this, button, action, modes, x, y))
				return;
		}
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
