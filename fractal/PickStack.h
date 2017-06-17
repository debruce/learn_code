#ifndef __PickStack_H__
#define __PickStack_H__

#include <GLFW/glfw3.h>
#include <vector>

struct PickStack {
	GLuint	count;
	GLuint	minI;
	GLuint	maxI;
	GLuint	names[];
	float min()	{ return (float)minI / (float)0xffffffff; }
	float max()	{ return (float)maxI / (float)0xffffffff; }
	PickStack* next()	{ return (PickStack*) ( ((GLuint*)this)+count+3); }

	static void push(void* data)
	{
		uint64_t shift = (uint64_t) data;
		glPushName(0x01000000 | (shift & 0xffffff));
		shift >>= 24;
		glPushName(0x02000000 | (shift & 0xffffff));
		shift >>= 24;
		glPushName(0x03000000 | (shift & 0xffffff));
	}

	static void pop()
	{
		glPopName();
		glPopName();
		glPopName();
	}

	void* pack()
	{
		uint64_t	shift = 0;
		for (int j = 0; j < count; j++) {
			if (names[j] == 0xffffffff) continue;
			switch (names[j] >> 24) {
				case 1:
					shift |= names[j] & 0xffffff;
					break;
				case 2:
					shift |= ((uint64_t)names[j] & 0xffffff) << 24;
					break;
				case 3:
					shift |= ((uint64_t)names[j] & 0xffffff) << 48;
					break;
			}
		}
		return (void*)shift;
	}
};

template <typename T>
class PickClass {
	struct DepthNode {
		float	depth;
		float	min;
		float	max;
		void*	ptr;
	};
	std::vector<DepthNode> ret;
public:
	PickClass(std::vector<GLuint>& vec, int num)
	{
		PickStack* ptr = reinterpret_cast<PickStack*>(vec.data());
		ret.reserve(num);
		for (int i = 0; i < num; i++) {
			DepthNode	n;
			n.min = ptr->min();
			n.max = ptr->max();
			n.depth = .5 * (ptr->min() + ptr->max());
			n.ptr = ptr->pack();
			ret.push_back(n);
			ptr = ptr->next();
		}
		sort(ret.begin(), ret.end(), [](const DepthNode& a, const DepthNode& b) -> bool { return a.depth < b.depth; });
	}

	size_t size() const
	{
		return ret.size();
	}

	T& operator[](size_t i)
	{
		return *reinterpret_cast<T*>(ret[i].ptr);
	}
};
#endif
