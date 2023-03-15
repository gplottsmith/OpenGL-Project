#pragma once


class IndexBuffer
{
private:
	unsigned int mRendererID;
	unsigned int mCount;

public:
	IndexBuffer(const int* data, int count);
	IndexBuffer();
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return mCount; }
};