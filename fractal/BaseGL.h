#ifndef __BASEGL_H__
#define __BASEGL_H__

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>

class BaseGL {
	GLFWwindow *window;

	static void doInit();
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
	virtual void paint();
	virtual void keyCallback(int key, int scancode, int action, int modes, double x, double y);
	virtual void mouseCallback(int button, int action, int modes, double x, double y);
	void display();
};

#endif
