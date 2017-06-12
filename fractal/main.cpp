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

struct Color {
	float	red;
	float	green;
	float	blue;
	float	alpha;
};

struct Patch {
	int	name;
	float	x;
	float	y;
	float	z;
	bool	selected;
	Color	color;

	void paint()
	{
		glPushMatrix();
		glPushName(name);
			if (selected) {
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
		glPopName();
		glPopMatrix();
	}
};

struct NamePair {
	float	depth;
	int	name;
};

struct MyGL : public BaseGL {
	vector<Patch>	patches;

	MyGL(int width, int height, const std::string& title) : BaseGL(width, height, title)
	{
		const int cnt = 20;

		for (int i = 0; i < cnt; i++) {
			Patch	p;
			p.name = i;
			p.x = .5 * cos(i * M_PI * 2 / cnt);
			p.y = .5 * sin(i * M_PI * 2 / cnt);
			p.z = (float)i/cnt - .5;
			p.selected = false;
			p.color.red = p.x + .5;
			p.color.green = .2;
			p.color.blue = p.y + .5;
			p.color.alpha = 1.0;
			patches.push_back(p);
		}
	}

	void draw()
	{
		pushOrtho();

		for (auto& p : patches) {
			p.paint();
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

	void pick(double x, double y, vector<NamePair>& hits)
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
		hits.clear();
		Hit* ptr = reinterpret_cast<Hit*>(vec.data());
		for (int i = 0; i < num; i++) {
			NamePair p;
			p.depth = (ptr->min()  + ptr->max()) / 2.0;
			p.name = ptr->names[1];
			hits.push_back(p);
			ptr = ptr->next();
		}
		sort(hits.begin(), hits.end(), [](const NamePair& a, const NamePair& b) -> bool { return a.depth < b.depth; });
	}

	void keyCallback(int key, int scancode, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			setClose();
			return;
		}

		vector<NamePair>	hits;
		pick(x, y, hits);

		cout << "keyCallback key=" << key
			<< " scancode=" << scancode
			<< " modes=" << modes
			<< " x=" << x
			<< " y=" << y
			<< endl;

		for (auto& h : hits) {
			cout << "depth=" << h.depth <<  " name=" << h.name << endl;
		}
		cout << endl;
	}

	void mouseCallback(int button, int action, int modes, double x, double y)
	{
		if (action == GLFW_RELEASE) return;

		vector<NamePair>	hits;
		pick(x, y, hits);

		for (auto& p : patches) p.selected = false;

		cout << "mouseCallback button=" << button << " modes=" << modes << endl;
		for (auto& h : hits) {
			for (auto& p : patches) {
				if (p.name == h.name) {
					p.selected = true;
					return;
				}
			}
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
