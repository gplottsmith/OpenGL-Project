#pragma once
#include "Renderer.h"

class VertexBuffer
{
private:
	unsigned int mRendererID;

public:
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer();
	~VertexBuffer();

	inline unsigned int getID( void ) const { return mRendererID; }

	void resubmitData( const void* data, unsigned int size );
	void bind() const;
	void unbind() const;
};