#ifndef __BASEGL_H__
#define __BASEGL_H__

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>

class BaseGL {
	GLFWwindow *window;
	float	xscale;
	float	yscale;
	float	aspect_;
	int	orthoSaveMode;

	static void doInit();
	static void size_callback(GLFWwindow* wind, int width, int height);
	static void mousebutton_callback(GLFWwindow* wind, int button, int action, int mods);
	static void keyboard_callback(GLFWwindow* wind, int key, int scancode, int action, int mods);
	static void cursor_callback(GLFWwindow* wind, double xoff, double yoff);
	static void scroll_callback(GLFWwindow* wind, double xoff, double yoff);
public:
	struct Exc : public std::runtime_error {
		Exc() : std::runtime_error("GLFW") {};
	};

	BaseGL(int width, int height, const std::string& title);
	~BaseGL();

	void setClose();
	bool shouldClose();
	void getSize(int& width, int& height) const;
	void getCursor(double& x, double& y) const;
	void setPickView(float x, float y);
	void useCursor(bool);
	void useScroll(bool);

	virtual void paint();
	virtual void mouseButtonCallback(int button, int action, int modes);
	virtual void keyboardCallback(int key, int scancode, int action, int modes);
	virtual void cursorCallback(double x, double y);
	virtual void scrollCallback(double xoff, double yoff);
	void display();

	float aspect()	const	{ return aspect_; }
	void pushOrtho();
	void popOrtho();
};

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

	virtual bool mouseButtonCallback(BaseGL& wind, int button, int action, int modes);
	virtual bool keyboardCallback(BaseGL& wind, int key, int scancode, int action, int modes);
};

#endif
