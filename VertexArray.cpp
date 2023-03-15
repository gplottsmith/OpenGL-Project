#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"	

VertexArray::VertexArray()
{
	glCall(glGenVertexArrays(1, &mRendererID));
}

VertexArray::~VertexArray()
{
	glCall(glDeleteVertexArrays(1, &mRendererID));
}

void VertexArray::addBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glCall(glEnableVertexAttribArray(i));
		glCall(glVertexAttribPointer(i, element.count, element.type,
			element.normalized, layout.getStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::getTypeSize(element.type);
	}
}

void VertexArray::bind() const
{
	glCall(glBindVertexArray(mRendererID));
}

void VertexArray::unbind() const
{
	glCall(glBindVertexArray(0));
}
