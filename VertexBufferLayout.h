#pragma once

#include <vector>
#include "Renderer.h"

struct VertexBufferElement 
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int getTypeSize(unsigned int type) 
	{
		switch (type)
		{
			case GL_FLOAT: return			4;
			case GL_DOUBLE: return			8;
			case GL_UNSIGNED_INT: return	4;
			case GL_UNSIGNED_BYTE: return	1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> mElements;
	unsigned int mStride;
public:
	VertexBufferLayout()
		: mStride(0) 
	{

	}

	template<typename T>
	void push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void push<double>(unsigned int count)
	{
		mElements.push_back({ GL_DOUBLE, count, GL_FALSE });
		mStride += count * VertexBufferElement::getTypeSize(GL_DOUBLE);
	}

	template<>
	void push<float>(unsigned int count)
	{
		mElements.push_back({ GL_FLOAT, count, GL_FALSE });
		mStride += count * VertexBufferElement::getTypeSize(GL_FLOAT);
	}

	template<>
	void push<unsigned int>(unsigned int count)
	{
		mElements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		mStride += count * VertexBufferElement::getTypeSize(GL_UNSIGNED_INT);
	}

	template<>
	void push<unsigned char>(unsigned int count)
	{
		mElements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		mStride += count * VertexBufferElement::getTypeSize(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& getElements() const { return mElements; }
	inline unsigned int getStride() const { return mStride; }
};