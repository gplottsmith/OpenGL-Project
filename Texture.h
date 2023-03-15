#pragma once

#include "Renderer.h"
#include <string>

class Texture
{
private:
	unsigned int mRendererID;
	const std::string& mFilepath;
	unsigned char* mLocalBuffer;
	int mWidth, mHeight, mBPP;
public:
	Texture( const char* filepath );
	Texture( const std::string& filepath );
	~Texture();

	void bind(unsigned int slot = 0) const;
	void unbind() const;	

	inline int getWidth() const { return mWidth; }
	inline int getHeight() const { return mHeight; }
	inline int getBPP() const { return mBPP; }
};
