#pragma once

#include "glm/glm.hpp"


class PhysicsProperties
{
public:
	inline float getFallDistance( void ) const { return mFallDistance; }
	inline float getFallSpeed( void ) const { return mFallSpeed; }
	inline glm::vec3 getFallTranslation( void ) const { return mFall;  }

	void incrFallSpeed( float deltaTime );
	void updateFallDistance( float deltaTime );

	inline bool isGrounded( void ) const { return mGrounded; }
	inline void setGrounded( bool g ) { mGrounded = g; }
	void resetFalling( void );

	PhysicsProperties( );

	PhysicsProperties( const PhysicsProperties& other ) :
		mFallDistance( other.mFallDistance ),
		mFallSpeed( other.mFallSpeed ),
		mGravitationalAcceleration( other.mGravitationalAcceleration ),
		mUnitsPerMeter( other.mUnitsPerMeter ),
		mGrounded( other.mGrounded ),
		mFall( other.mFall )
	{

	}

	~PhysicsProperties( );
		
protected:
	float mFallDistance;
	float mFallSpeed;
	const float mGravitationalAcceleration = 9.8f;
	const float mUnitsPerMeter = 100;
	bool mGrounded;

	glm::vec3 mFall;
};