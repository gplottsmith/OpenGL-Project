#pragma once


#include "Math/BezierCurve.h"


template < class T >


//Time is in milliseconds
class TimerEvent
{
public:

	bool shouldEnd( void ) const
	{
		if( mTimeElapsed >= mDuration )
			return true;

		return false;
	}
	
	void tick( const float deltaTime )
	{
		mTimeElapsed = ( ( ( mTimeElapsed + deltaTime ) < ( mDuration ) ) ? mTimeElapsed + deltaTime : mDuration );
		mData = mInterpolator.interpolation( mTimeElapsed / mDuration );
	}

	float getTimeElapsed( void ) const
	{
		return mTimeElapsed;
	}

	T getData( void ) const
	{
		return mData;
	}

	TimerEvent< T >( const float maxTime, const float Data, float ratio, const BezierCurve< T >& bc ) :
		mTimeElapsed( 0 ), mDuration( maxTime ), mInterpolator( bc ), mData( ), mEarlyEndRatio( ratio )
	{
	}

	~TimerEvent( )
	{

	}

protected:
	float mTimeElapsed;
	float mDuration;
	float mEarlyEndRatio;
	BezierCurve< T > mInterpolator;

	T mData;
};


template < >

class TimerEvent< float >
{
public:

	bool shouldEnd( void ) const
	{
		if( mTimeElapsed >= mDuration * mEarlyEndRatio )
			return true;

		return false;
	}

	void tick( const float deltaTime )
	{
		mTimeElapsed = ( ( ( mTimeElapsed + deltaTime ) < ( mDuration ) ) ? mTimeElapsed + deltaTime : mDuration );

		mData = mInterpolator.interpolation( mTimeElapsed / mDuration );
	}

	float getTimeElapsed( void ) const
	{
		return mTimeElapsed;
	}

	float getData( void ) const
	{
		return mData;
	}

	TimerEvent( const float maxTime, const float initialData, float ratio, const BezierCurve< float >& bc ) :
		mTimeElapsed( 0 ), mDuration( maxTime ), mInterpolator( bc ), mData( initialData ), mEarlyEndRatio( ratio )
	{
	}

	~TimerEvent( )
	{
	}

protected:
	float mTimeElapsed;
	float mDuration;
	float mEarlyEndRatio;
	BezierCurve< float > mInterpolator;

	float mData;
};