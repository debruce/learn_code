#ifndef __PickStack_H__
#define __PickStack_H__

#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

class PickVoid {
	struct impl;
	std::shared_ptr<impl>	pimpl_;
public:
	static void push(void* data);
	static void pop();

	PickVoid(std::vector<GLuint>& vec, int num);

	size_t size() const;
	void* operator[](size_t i);
};

template <typename T>
class PickClass : private PickVoid {
public:
	static void push(T* data)
	{
		PickVoid::push(data);
	}

	static void pop()
	{
		PickVoid::pop();
	}

	PickClass(std::vector<GLuint>& vec, int num) : PickVoid(vec, num)
	{
	}

	size_t size() const
	{
		return PickVoid::size();
	}

	T& operator[](size_t i)
	{
		return *reinterpret_cast<T*>(PickVoid::operator[](i));
	}
};

#endif
