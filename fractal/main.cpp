#include <iostream>
#include <cmath>
#include <vector>
#include <string>
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

typedef PickClass<BaseObj>	PickBaseObj;

MyFont	*font;

class IntegerEntry : public BaseObj {
	string	data;
	MyFont::Pos	pos;
public:
	IntegerEntry(const string& def, MyFont::Pos pos_)
	{
		data = def;
		pos = pos_;
	}

	void draw(BaseGL* wind)
	{
		PickBaseObj::push(this);
		float fg[] = {1, 1, 1, 1};
		float bg[] = {.25, .25, .25, 1};
		float hl[] = {.5, 0, 0, 1};
		// font->paintBackground(data, pos, fg, bg);
		font->paintHighlight(data, pos, fg, bg, 9, hl);
		// font->paint(data, pos);
		PickBaseObj::pop();
	}

	bool keyboardCallback(BaseGL& wind, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_RELEASE) return true;
		cout << "keyboardCallback ";
		if (mods & GLFW_MOD_SHIFT) cout << "shift ";
		if (mods & GLFW_MOD_CONTROL) cout << "control ";
		if (mods & GLFW_MOD_ALT) cout << "alt ";
		if (mods & GLFW_MOD_SUPER) cout << "super ";
		switch (key) {
			case GLFW_KEY_0:
				cout << "zero";
				break;
			case GLFW_KEY_1:
				cout << "one";
				break;
			case GLFW_KEY_2:
				cout << "two";
				break;
			case GLFW_KEY_3:
				cout << "three";
				break;
			case GLFW_KEY_4:
				cout << "four";
				break;
			case GLFW_KEY_5:
				cout << "five";
				break;
			case GLFW_KEY_6:
				cout << "six";
				break;
			case GLFW_KEY_7:
				cout << "seven";
				break;
			case GLFW_KEY_8:
				cout << "eight";
				break;
			case GLFW_KEY_9:
				cout << "nine";
				break;
			case GLFW_KEY_LEFT:
				cout << "left";
				break;
			case GLFW_KEY_RIGHT:
				cout << "right";
				break;
			case GLFW_KEY_UP:
				cout << "up";
				break;
			case GLFW_KEY_DOWN:
				cout << "down";
				break;

			default:
				cout << "key=" << dec << key;
				break;
		}
		cout << endl;
		return true;
	}
};

struct MyGL : public BaseGL {
	GLUquadric*	sphere;
	IntegerEntry*	str;

	MyGL(int width, int height, const std::string& title) : BaseGL(width, height, title)
	{
		sphere = gluNewQuadric();
		str = new IntegerEntry("upper left", MyFont::UpperLeft);
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

		pushOrtho();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glTranslatef(-aspect(), 1, 0);
			glScalef(.1, .1, 1);
			str->draw(this);
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
	}
};

int main()
{
	try {
		MyGL	myWindow(640, 480, "MyTitle");
		font = new MyFont("Tahoma");

		while (!myWindow.shouldClose()) {
			myWindow.display();
		}
	}
	catch (std::exception& exc) {
		cerr << "Caught exception: " << exc.what() << endl;
	}

	return 0;
}
