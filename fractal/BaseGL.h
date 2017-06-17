#ifndef __BASEGL_H__
#define __BASEGL_H__

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>

class BaseGL;

class BaseObj {
	bool	selectFlag;
public:
	bool isSelected() const
	{
		return selectFlag;
	}

	virtual void select();
	virtual void deselect();
	virtual void draw(BaseGL* wind) = 0;

	virtual bool keyCallback(BaseGL& wind, int key, int scancode, int action, int modes, double x, double y);
	virtual bool mouseCallback(BaseGL& wind, int button, int action, int modes, double x, double y);
};

class BaseGL {
	GLFWwindow *window;
	float	xscale;
	float	yscale;
	float	aspect_;
	int	orthoSaveMode;

	static void doInit();
	static void size_callback(GLFWwindow* wind, int width, int height);
	static void key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* wind, int button, int action, int mods);
public:
	struct Exc : public std::runtime_error {
		Exc() : std::runtime_error("GLFW") {};
	};

	BaseGL(int width, int height, const std::string& title);
	~BaseGL();

	void setClose();
	bool shouldClose();
	void getSize(int& width, int& height);
	void setPickView(float x, float y);
	virtual void paint();
	virtual void keyCallback(int key, int scancode, int action, int modes, double x, double y);
	virtual void mouseCallback(int button, int action, int modes, double x, double y);
	void display();

	float aspect()	const	{ return aspect_; }
	void pushOrtho();
	void popOrtho();
};

#endif
