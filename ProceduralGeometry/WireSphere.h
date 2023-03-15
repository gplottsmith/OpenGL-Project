

#include "Shader.h"
#include <iostream>

class WireSphere
{
public:

	inline void setCenter( const glm::vec3& cen ) { mCenter = cen; }
	inline void setColor( const glm::vec4& col ) { mColor = col; }

	inline glm::vec3 getCenter( ) const { return mCenter; }
	inline glm::vec4 getColor( ) const { return mColor; }

	void draw( const glm::mat4& viewTimesProjection )
	{
		mShader->drawWireSphere( viewTimesProjection * glm::translate( glm::mat4( 1.0f ), mCenter ), mColor );
	}
	WireSphere( const glm::vec3& center, float radius, float thickness, uint32_t divisions, const glm::vec4& color ) :
		mShader( std::make_unique< Shader >( "res/shaders/Color.shader" ) ), mCenter( center ), mColor( color ), mThickness( thickness ), mRadius( radius ), mDiv( divisions )
	{
		mShader->initRenderDataWireSphere( mRadius, mThickness, mDiv, mCenter, mColor );
	}
	WireSphere( const WireSphere& other ) : mShader( std::make_unique< Shader >( "res/shaders/Color.shader" ) ), mCenter( other.mCenter ), mColor( other.mColor ),
		mThickness( other.mThickness ), mRadius( other.mRadius ), mDiv( other.mDiv )
	{
		mShader->initRenderDataWireSphere( mRadius, mThickness, mDiv, mCenter, mColor );
	}
	WireSphere( ) : mShader( nullptr ), mCenter( ), mColor( ), mThickness( 0.0f ), mRadius( 0.0f ), mDiv( 0 )
	{
	}
	~WireSphere( )
	{
		//DEBUG
		std::cout << "Destroyed Wire Sphere\n";
	}
private:
	std::unique_ptr< Shader > mShader;
	glm::vec3 mCenter;
	glm::vec4 mColor;
	float mThickness;
	float mRadius;
	uint32_t mDiv;
};