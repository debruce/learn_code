#include <iostream>
#include <cmath>
#include <vector>
#include <sys/time.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLFW_INCLUDE_GLU
#include "BaseGL.h"
#include "PickStack.h"
#include "MyFont.h"
#include "DoubleEntry.h"

using namespace std;

typedef PickClass<BaseObj>	PickBaseObj;

// glVertex3fv(glm::value_ptr(v));

void drawAxis()
{
	float	len = 10;
	int panels = 10;
	float depth = .1;
	float width = .1;
	float cwidth = .02;
	float p = 2 * M_PI / panels;

	glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i <= panels; i++) {
			glNormal3f(0, cosf(i*p), sinf(i*p));
			glm::vec3 v(len, cwidth*cosf(i*p), cwidth*sinf(i*p));
			glVertex3f(len, cwidth*cosf(i*p), cwidth*sinf(i*p));
			glVertex3f(-len, cwidth*cosf(i*p), cwidth*sinf(i*p));
		}
	glEnd();

	float f = width / hypotf(width, depth);
	float u = depth / hypotf(width, depth);
	for (float x = -len; x <= len; x += 1.0) {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i <= panels; i++) {
			glNormal3f(f, u*cosf(p*(i+.5)), u*sinf(p*(i+.5)));
			glVertex3f(x, 0, 0);
			glVertex3f(x-depth, width * cosf(p*i), width * sinf(p*i));
			glVertex3f(x-depth, width * cosf(p*(i+1)), width * sinf(p*(i+1)));
		}
		glEnd();
	}
}

void drawAxes()
{
	static GLuint index;

	if (index) {
		glCallList(index);
		return;

	}

	index = glGenLists(1);

	glNewList(index, GL_COMPILE_AND_EXECUTE);

	glPushMatrix();
		// glColor3f(1, 0, 0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const float[]){0.3, 0.0, 0.0, 1.0});
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const float[]){0.6, 0.0, 0.0, 1.0});
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const float[]){0.8, 0.6, 0.6, 1.0});
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
		drawAxis();
	glPopMatrix();

	glPushMatrix();
		// glColor3f(0, 1, 0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const float[]){0.0, 0.3, 0.0, 1.0});
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const float[]){0.0, 0.6, 0.0, 1.0});
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const float[]){0.6, 0.8, 0.6, 1.0});
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
		glRotatef(90, 0, 1, 0);
		drawAxis();
	glPopMatrix();

	glPushMatrix();
		// glColor3f(0, 0, 1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const float[]){0.0, 0.0, 0.3, 1.0});
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const float[]){0.0, 0.0, 0.6, 1.0});
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const float[]){0.6, 0.6, 0.8, 1.0});
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
		glRotatef(90, 0, 0, 1);
		drawAxis();
	glPopMatrix();

	glEndList();
}

struct MyGL : public BaseGL {
	GLUquadric*	sphere;
	DoubleEntry*	xrot;
	DoubleEntry*	yrot;
	DoubleEntry*	zrot;
	DoubleEntry*	near;
	DoubleEntry*	far;
	DoubleEntry*	angle;
	DoubleEntry*	xlight;
	DoubleEntry*	ylight;
	DoubleEntry*	zlight;
	vector<BaseObj*>	orthoObjs;

	MyGL(int width, int height, const std::string& title, MyFont* font) : BaseGL(width, height, title)
	{
		sphere = gluNewQuadric();

		xrot = new DoubleEntry(font, MyFont::UpperLeft, "xrot", 1, 2);
		*xrot = 1;
		orthoObjs.push_back(xrot);

		yrot = new DoubleEntry(font, MyFont::UpperLeft, "yrot", 1, 2);
		*yrot = 1;
		orthoObjs.push_back(yrot);

		zrot = new DoubleEntry(font, MyFont::UpperLeft, "zrot", 1, 2);
		*zrot = 1;
		orthoObjs.push_back(zrot);

		near = new DoubleEntry(font, MyFont::UpperLeft, "near", 3, 1);
		*near = 2;
		orthoObjs.push_back(near);

		far = new DoubleEntry(font, MyFont::UpperLeft, "far", 3, 1);
		*far = 40;
		orthoObjs.push_back(far);

		angle = new DoubleEntry(font, MyFont::UpperLeft, "angle", 3, 1);
		angle->setRange(0, 360, 1);
		*angle = 0;
		orthoObjs.push_back(angle);

		xlight = new DoubleEntry(font, MyFont::UpperLeft, "xlight", 2, 2);
		*xlight = 0;
		orthoObjs.push_back(xlight);

		ylight = new DoubleEntry(font, MyFont::UpperLeft, "ylight", 2, 2);
		*ylight = 0;
		orthoObjs.push_back(ylight);

		zlight = new DoubleEntry(font, MyFont::UpperLeft, "zlight", 2, 2);
		*zlight = 10;
		orthoObjs.push_back(zlight);

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
		{
			// glLightfv(GL_LIGHT0, GL_POSITION, (const float[]){*xlight, *ylight, *zlight, 0});
			float	v[4];
			v[0] = *xlight;
			v[1] = *ylight;
			v[2] = *zlight;
			v[3] = 0;
			glLightfv(GL_LIGHT0, GL_POSITION, v);
		}
		glPopMatrix();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glPushMatrix();
			glTranslatef(0,0,-5);
			glRotatef(t * 360.0, *xrot, *yrot, *zrot);
				drawAxes();

				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const float[]){0.33, 0.22, 0.03, 1.0});
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const float[]){0.78, 0.57, 0.11, 1.0});
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const float[]){0.99, 0.91, 0.81, 1.0});
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27.8);
				gluQuadricDrawStyle(sphere, GLU_FILL);

				for (int i = 0; i < 8; i++) {
					float xc = (i & 1) ? 1 : -1;
					float yc = (i & 2) ? 1 : -1;
					float zc = (i & 4) ? 1 : -1;
					glPushMatrix();
						glTranslatef(xc,yc,zc);
						gluSphere(sphere, 1, 10, 10);
					glPopMatrix();
				}

			glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		pushOrtho();
			glMatrixMode(GL_MODELVIEW);
			float ypos = 1.0;
			float ydelta = .1;
			for (int i = 0; i < orthoObjs.size(); i++) {
				glPushMatrix();
					glTranslatef(-aspect(), ypos, 0);
					glScalef(.1, .1, 1);
					orthoObjs[i]->draw(this);
				glPopMatrix();
				ypos -= ydelta;
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
