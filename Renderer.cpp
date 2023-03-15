

#include "Renderer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Block/Block.h"


#include <iostream>

#include "glew.h"

void glErrorClear()
{
	while (glGetError());
}

bool glLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {

		std::cout << "[OpenGL Error] ( " << error << " )" << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, const Texture& texture) const
{
	va.bind();
	ib.bind();
	shader.bind();
	texture.bind();
	glCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	va.bind();
	ib.bind();
	shader.bind();
	glCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::drawMultiple(const VertexArray& va, int* indexData, const int indexCount, 
	const int indecesPerFace, const Shader& shader) const
{
	va.bind();
	int temp[ 3 ];
	for ( int i = 0; i < indexCount ; i += indecesPerFace)
	{
		temp[ 0 ] = indexData[ i ];
		temp[ 1 ] = indexData[ i + 1 ];
		temp[ 2 ] = indexData[ i + 2 ];
		IndexBuffer tempIB(temp, indecesPerFace);
		tempIB.bind();
		glCall(glDrawElements(GL_TRIANGLES, tempIB.getCount(), GL_UNSIGNED_INT, nullptr));
	}
	va.unbind( );
}

void Renderer::drawMultiple( const Block& block, const Shader& shader ) const
{
}

void Renderer::batchRender( std::unique_ptr< VertexArray >& va, IndexBuffer& ib ) const
{

}

void Renderer::clear() const 
{
	glCall(glClear(GL_COLOR_BUFFER_BIT));
}   
