
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Math/Quaternion.h"


class Camera
{
public:
	void setTranslation( const glm::vec3& t );
	void setTranslation( const glm::mat4& t );
	void setRotation( const Quaternion& r );
	void setProjection( const glm::mat4& proj );
	void setFOVY( float fovRad, float near, float far );
	
	void changePitch( float angle );
	void changeYaw( float angle );

	void moveForward( const float amount );

	glm::mat4 getInverseTransform( void ) const;
	glm::mat4 getTransform( void ) const;
	glm::mat4 getRotationAboutTranslation( void ) const;
	glm::vec3 getTranslationVec3( void ) const;
	glm::vec3 getUpDirection( void ) const;
	glm::vec3 getMouseDirection( const glm::vec2& pos, float wWidth, float wHeight ) const;

	inline const glm::mat4& getTranslation( void ) const { return mTranslation; }
	inline const glm::mat4& getProjection( void ) const { return mProj; }
	inline const Quaternion& getRotation( void ) const { return mRotation; }
	inline const Quaternion& getPitch( void ) const { return mPitch; }
	inline const Quaternion& getYaw( void ) const { return mYaw; }
	inline const glm::vec3& getForwardDirection( void ) const { return mForwardDirection; }
	inline float getAspectRatio( ) const { return mAspectRatio; }
	inline float getFovyRad( ) const { return mFovyRad; }
	inline float getFovyDeg( ) const { return mFovyRad * ( 180.0f / glm::pi< float >( ) ); }
	inline glm::vec3 getUpperRightCoord( ) const { return glm::vec3( 1.0f, 1.0f / mAspectRatio, mProj[ 0 ][ 0 ] ); }

	Camera( );
	Camera( const glm::mat4& proj );
	Camera( const Camera& cam );
	Camera( const glm::mat4& t, const glm::mat4& proj, const Quaternion& r, const Quaternion& p, const Quaternion& y );
	~Camera( );
private:
	void composeRotation( const Quaternion& q );

	glm::mat4 mTranslation;
	glm::mat4 mProj;
	Quaternion mRotation;
	Quaternion mPitch;
	Quaternion mYaw;

	float mAspectRatio;
	float mFovyRad;
	float mZNDC;

	glm::vec3 mForwardDirection;
};
