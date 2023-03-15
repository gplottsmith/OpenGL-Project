#include "Texture.h"
#include "stb_image/stb_image.h"


Texture::Texture( const char* filepath )
	: mRendererID( 0 ), mFilepath( filepath ), mLocalBuffer( nullptr ),
	mWidth( 0 ), mHeight( 0 ), mBPP( 0 )
{
	stbi_set_flip_vertically_on_load( 1 );
	mLocalBuffer = stbi_load( filepath, &mWidth, &mHeight, &mBPP, 4 );

	glCall( glGenTextures( 1, &mRendererID ) );
	glCall( glBindTexture( GL_TEXTURE_2D, mRendererID ) );

	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );

	glCall( glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer ) );
	glCall( glBindTexture( GL_TEXTURE_2D, 0 ) );

	if( mLocalBuffer )
		stbi_image_free( mLocalBuffer );
}

Texture::Texture( const std::string& filepath ) 
	: mRendererID( 0 ), mFilepath( filepath ), mLocalBuffer( nullptr ),
	mWidth( 0 ), mHeight( 0 ), mBPP( 0 )
{
	stbi_set_flip_vertically_on_load( 1 );
	mLocalBuffer = stbi_load( filepath.c_str( ), &mWidth, &mHeight, &mBPP, 4 );

	glCall( glGenTextures( 1, &mRendererID ) );
	glCall( glBindTexture( GL_TEXTURE_2D, mRendererID ) );

	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
	glCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );

	glCall( glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer ) );
	glCall( glBindTexture( GL_TEXTURE_2D, 0 ) );

	if( mLocalBuffer )
		stbi_image_free( mLocalBuffer );
}

Texture::~Texture()
{
	glCall(glDeleteTextures(1, &mRendererID));
}

void Texture::bind(unsigned int slot) const
{
	glCall( glActiveTexture( GL_TEXTURE0 + slot ) );
	//glCall(glBindTexture(GL_TEXTURE_2D, mRendererID));
	glCall( glBindTextureUnit( slot, mRendererID ) );
}

void Texture::unbind() const
{
	glCall(glBindTexture(GL_TEXTURE_2D, 0));
}
