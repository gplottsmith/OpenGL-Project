#pragma once

#include "glew.h"
#include <memory>

#define ASSERT(x) if(!(x)) __debugbreak();
#define glCall(x) glErrorClear(); x; ASSERT(glLogCall(#x, __FILE__, __LINE__))

void glErrorClear();

bool glLogCall(const char* function, const char* file, int line);

class VertexArray;
class IndexBuffer;
class Shader;
class Texture;
class Block;


//Singleton Class

class Renderer {
private:
	Renderer( ) {  }

public:
	static Renderer& get( ) 
	{ 
		static Renderer instance;
		return instance; 
	}

	void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, const Texture& texture) const;
	void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void drawMultiple(const VertexArray& va, int* data,
		const int indexCount, const int indecesPerFace, const Shader& shader) const;
	void drawMultiple( const Block& block, const Shader& shader ) const;
	void batchRender( std::unique_ptr< VertexArray >& va, IndexBuffer& ib ) const;
	void clear() const;

	Renderer( const Renderer& r ) = delete;
};