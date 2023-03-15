
#include "Camera.h"



void Camera::setTranslation( const glm::vec3& t )
{
	mTranslation[ 3 ][ 0 ] = t.x;
	mTranslation[ 3 ][ 1 ] = t.y;
	mTranslation[ 3 ][ 2 ] = t.z;
}

void Camera::setTranslation( const glm::mat4& t )
{
	mTranslation = t;
}

void Camera::setRotation( const Quaternion& r )
{
	mRotation = r;
	mForwardDirection = glm::vec3( 0.0f, 0.0f, -1.0f );
	r.rotate( mForwardDirection );
}

void Camera::setProjection( const glm::mat4& proj )
{
	mProj = proj;

	mAspectRatio = mProj[ 1 ][ 1 ] / mProj[ 0 ][ 0 ];
	mFovyRad = 2.0f * glm::atan( 1.0f / mProj[ 1 ][ 1 ] );

	glm::vec4 eyeVec( 1.0f, 1.0f / mAspectRatio, -mProj[ 0 ][ 0 ], 1.0f );
	eyeVec = mProj * eyeVec;
	mZNDC = eyeVec.z / eyeVec.w;
}

void Camera::setFOVY( float fovRad, float near, float far )
{
	mFovyRad = fovRad;
	mProj = glm::perspective( mFovyRad, mAspectRatio, near, far );

	glm::vec4 eyeVec( 1.0f, 1.0f / mAspectRatio, -mProj[ 0 ][ 0 ], 1.0f );
	eyeVec = mProj * eyeVec;
	mZNDC = eyeVec.z / eyeVec.w;
}

void Camera::composeRotation( const Quaternion& q )
{
	mRotation.composeWith( q );
	q.rotate( mForwardDirection );
}

void Camera::changePitch( float angle )
{
	glm::vec3 Y( 0, 1, 0 );
	glm::vec3 projXZ = mForwardDirection - ( glm::dot( mForwardDirection, Y ) / glm::dot( Y, Y ) ) * Y;
	glm::vec3 right = glm::cross( mForwardDirection, Y );
	Quaternion pitch( right, angle );

	pitch.rotate( mForwardDirection );
	if( glm::dot( mForwardDirection, projXZ ) >= 0.0f )
	{
		mRotation.composeWith( pitch );
		mPitch.composeWith( pitch );
	}
	else
		pitch.getInverse( ).rotate( mForwardDirection );
}	
void Camera::changeYaw( float angle )
{
	Quaternion yaw( glm::vec3( 0, 1, 0 ), angle );

	mRotation.composeWith( yaw );
	mYaw.composeWith( yaw );
	yaw.rotate( mForwardDirection );
}

void Camera::moveForward( const float amount )
{
	glm::vec3 movement( mForwardDirection * amount );

	mTranslation[ 3 ][ 0 ] += movement.x;
	mTranslation[ 3 ][ 1 ] += movement.y;
	mTranslation[ 3 ][ 2 ] += movement.z;
}

glm::mat4 Camera::getInverseTransform( void ) const
{
	return glm::inverse( mTranslation * mRotation.toMat4( ) );
}

glm::mat4 Camera::getTransform( void ) const
{
	return mTranslation * mRotation.toMat4( ) ;
}


//	Returns a Transformation Matrix describing being rotated by mRotation about a point set at mTranslation
glm::mat4 Camera::getRotationAboutTranslation( void ) const
{
	glm::vec3 translation( getTranslationVec3( ) );
	glm::mat4 reverseTranslation( glm::translate( glm::mat4( 1.0f ), -1.0f * translation ) );
	glm::mat4 result( mTranslation );

	result *= mRotation.getInverse( ).toMat4( );
	result *= reverseTranslation;

	return result;
}

glm::vec3 Camera::getTranslationVec3( void ) const
{
	return glm::vec3( mTranslation[ 3 ][ 0 ], mTranslation[ 3 ][ 1 ], mTranslation[ 3 ][ 2 ] );
}

glm::vec3 Camera::getUpDirection( void ) const
{
	glm::vec3 right = glm::cross( mForwardDirection, glm::vec3( 0, 1, 0 ) );
	return glm::normalize( glm::cross( right, mForwardDirection ) );
}

glm::vec3 Camera::getMouseDirection( const glm::vec2& pos, float wWidth, float wHeight ) const
{
	float xndc = pos.x / ( wWidth / 2 );
	float yndc = pos.y / ( wHeight / 2 ); 
	float zndc = mZNDC;
	float w = mProj[ 0 ][ 0 ];
	glm::vec4 clip( xndc * w, yndc * w, zndc * w, w );
	clip = glm::inverse( mProj ) * clip;
	clip = mRotation.rotate( clip );

	return clip;
}

Camera::Camera( ) :
	mAspectRatio( 0.0f ), mFovyRad( 0.0f ), mZNDC( 0.0f ),
	mTranslation( glm::mat4( 1.0f ) ),
	mProj( glm::perspective( 45.0f, 1536.0f / 1152.0f, 0.1f, 15360.0f * 2 ) ),
	mRotation( Quaternion( ) ),
	mPitch( Quaternion( ) ),
	mYaw( Quaternion( ) ),
	mForwardDirection( glm::vec3( 0.0f, 0.0f, -1.0f ) )
{
	mAspectRatio = mProj[ 1 ][ 1 ] / mProj[ 0 ][ 0 ];
	mFovyRad = 2.0f * glm::atan( 1.0f / mProj[ 1 ][ 1 ] );

	glm::vec4 eyeVec( 1.0f, 1.0f / mAspectRatio, -mProj[ 0 ][ 0 ], 1.0f );
	eyeVec = mProj * eyeVec;
	mZNDC = eyeVec.z / eyeVec.w;
}

Camera::Camera( const glm::mat4& proj ) :
	mAspectRatio( 0.0f ), mFovyRad( 0.0f ), mZNDC( 0.0f ),
	mTranslation( glm::mat4( 1.0f ) ),
	mProj( proj ),
	mRotation( Quaternion( ) ),
	mPitch( Quaternion( ) ),
	mYaw( Quaternion( ) ),
	mForwardDirection( glm::vec3( 0.0f, 0.0f, -1.0f ) )
{
	mAspectRatio = mProj[ 1 ][ 1 ] / mProj[ 0 ][ 0 ];
	mFovyRad = 2.0f * glm::atan( 1.0f / mProj[ 1 ][ 1 ] );

	glm::vec4 eyeVec( 1.0f, 1.0f / mAspectRatio, -mProj[ 0 ][ 0 ], 1.0f );
	eyeVec = mProj * eyeVec;
	mZNDC = eyeVec.z / eyeVec.w;
}

Camera::Camera( const Camera& cam ) :
	mAspectRatio( cam.mAspectRatio ), mFovyRad( cam.mFovyRad ), mZNDC( cam.mZNDC ),
	mTranslation( cam.mTranslation ),
	mProj( cam.mProj ),
	mRotation( cam.mRotation ),
	mPitch( cam.mPitch ),
	mYaw( cam.mYaw ),
	mForwardDirection( cam.mForwardDirection )
{
}

Camera::Camera( const glm::mat4& t, const glm::mat4& proj, const Quaternion& r, const Quaternion& p, const Quaternion& y ) :
	mAspectRatio( 0.0f ), mFovyRad( 0.0f ), mZNDC( 0.0f ),
	mTranslation( t ),
	mProj( proj ),
	mRotation( r ),
	mPitch( p ),
	mYaw( y ),
	mForwardDirection( glm::vec3( 0.0f, 0.0f, -1.0f ) )
{
	//The forward Direction has a default value of -1.0z because -1.0z goes into the screen in opengl's coordinates
	mAspectRatio = mProj[ 1 ][ 1 ] / mProj[ 0 ][ 0 ];
	mFovyRad = 2.0f * glm::atan( 1.0f / mProj[ 1 ][ 1 ] );

	glm::vec4 eyeVec( 1.0f, 1.0f / mAspectRatio, -mProj[ 0 ][ 0 ], 1.0f );
	eyeVec = mProj * eyeVec;
	mZNDC = eyeVec.z / eyeVec.w;
}

Camera::~Camera( )
{
}
