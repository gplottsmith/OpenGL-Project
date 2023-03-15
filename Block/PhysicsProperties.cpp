
#include "PhysicsProperties.h"



void PhysicsProperties::incrFallSpeed( float deltaTime )
{
	//deltaTime is in milliseconds. 
	mFallSpeed += ( mGravitationalAcceleration / 1000 ) * deltaTime;
}

void PhysicsProperties::updateFallDistance( float deltaTime )
{
	//fall speed is in meters / second
	//We arbitrarily set 100 units to be 1 meter.
	//mFallDistanceis the number of in game units the object falls per frame.
	mFallDistance = ( mFallSpeed * mUnitsPerMeter * deltaTime ) / ( 1000 );
	mFall.y = -mFallDistance;
}

void PhysicsProperties::resetFalling( void )
{
	mFallSpeed = 0;
	mFallDistance = 0;
	mFall.y = 0;
}

PhysicsProperties::PhysicsProperties( ) : mFallDistance( 0 ), mFallSpeed( 0 ), mFall( ), mGrounded( false )
{

}

PhysicsProperties::~PhysicsProperties( )
{

}