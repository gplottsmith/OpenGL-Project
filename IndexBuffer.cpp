 
#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer() : mRendererID( 0 ), mCount( 0 )
{
}


IndexBuffer::IndexBuffer(const int* data, int count)
	: mCount(count)
{
	glCall(glGenBuffers(1, &mRendererID));
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID));
	glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	glCall(glDeleteBuffers(1, &mRendererID));
}

void IndexBuffer::bind() const
{
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID));
}

void IndexBuffer::unbind() const
{
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}