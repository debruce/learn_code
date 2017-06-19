#include "PickStack.h"

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
		int		bits = 0;
		for (int j = 0; j < count; j++) {
			if (names[j] == 0xffffffff) continue;
			switch (names[j] >> 24) {
				case 1:
					shift |= names[j] & 0xffffff;
					bits |= 1;
					break;
				case 2:
					shift |= ((uint64_t)names[j] & 0xffffff) << 24;
					bits |= 2;
					break;
				case 3:
					shift |= ((uint64_t)names[j] & 0xffffff) << 48;
					bits |= 4;
					break;
			}
		}
		if (bits != 7) return 0;
		return (void*)shift;
	}
};

struct PickVoid::impl {
	struct DepthNode {
		float	depth;
		float	min;
		float	max;
		void*	ptr;
	};
	std::vector<DepthNode> ret;

	inline static void push(void* data)
	{
		PickStack::push(data);
	}

	inline static void pop()
	{
		PickStack::pop();
	}

	inline impl(std::vector<GLuint>& vec, int num)
	{
		PickStack* ptr = reinterpret_cast<PickStack*>(vec.data());
		ret.reserve(num);
		for (int i = 0; i < num; i++) {
			DepthNode	n;
			n.ptr = ptr->pack();
			if (n.ptr) {
				n.min = ptr->min();
				n.max = ptr->max();
				n.depth = .5 * (ptr->min() + ptr->max());
				ret.push_back(n);
			}
			ptr = ptr->next();
		}
		sort(ret.begin(), ret.end(), [](const DepthNode& a, const DepthNode& b) -> bool { return a.depth < b.depth; });
	}

	inline size_t size() const
	{
		return ret.size();
	}

	inline void* operator[](size_t i)
	{
		return ret[i].ptr;
	}
};

void PickVoid::push(void* data)
{
	PickVoid::impl::push(data);
}

void PickVoid::pop()
{
	PickVoid::impl::pop();
}

PickVoid::PickVoid(std::vector<GLuint>& vec, int num) : pimpl_(new impl(vec, num))
{
}

size_t PickVoid::size() const
{
	return pimpl_->size();
}

void* PickVoid::operator[](size_t i)
{
	return pimpl_->operator[](i);
}
