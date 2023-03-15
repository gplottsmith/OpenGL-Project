#pragma once

#include "glm/glm.hpp"
#include "Block/GameObject.h"
#include "TimerEvent.h"
#include "Camera.h"


//This Object's move Direction is what the user sets it to be with keyboard inputs;
//mBlock's move Direction will be set based on that direction and other factors: jumping, falling, etc.

class BlockController
{

	struct JumpAmountInterpolator
	{
	public:
		inline float getTimeElapsed( ) const { return mTime; }
		inline float getHeight( ) const{ return mHeight; }

		bool shouldEnd( )
		{
			if( mTime >= mDuration )
				return true;

			return false;
		}
		void tick( float deltaTime )
		{
			mTime = ( ( ( mTime + deltaTime ) < ( mDuration ) ) ? mTime + deltaTime : mDuration );
			mHeight = ( 1 - mTime / mDuration ) * mMinHeight + ( mTime / mDuration ) * mMaxHeight;
		}
		void reset( )
		{
			mTime = 0;
			mHeight = mMinHeight;
		}


		JumpAmountInterpolator( float minHeight, float maxHeight, float duration ) : mMinHeight( minHeight ),
			mMaxHeight( maxHeight ), mDuration( duration ), mTime( 0 ), mHeight( minHeight )
		{
		}
		JumpAmountInterpolator( ) : mMinHeight( 0 ), mMaxHeight( 0 ), mDuration( 0 ), mTime( 0 ), mHeight( 0 )
		{
		}
		~JumpAmountInterpolator( )
		{
		}
	protected:
		float mHeight;
		const float mMinHeight;
		const float mMaxHeight;
		const float mDuration;
		float mTime;
	};


public:

	glm::vec3 getFrameTranslation( ) const;
	inline glm::vec3 getMoveTranslation( ) const { return mMoveDirection * mMoveAmount; }
	inline glm::vec3 getMoveDirection( ) const { return mMoveDirection; }
	inline glm::vec3 getJumpTranslation( ) const { return mJumpDirection * mJumpAmount; }
	inline glm::vec3 getFallTranslation( ) const { return mFalling ? mCapsulePtr->getFallTranslation( ) : glm::vec3( ); }
	inline float getMoveSpeed( ) const { return mMoveSpeed; }
	inline float getMinMoveSpeed( ) const { return mMinMoveSpeed; }
	inline float getMaxMoveSpeed( ) const { return mMaxMoveSpeed; }
	inline float getMoveAmount( ) const { return mMoveAmount; }
	inline float getAccelerationTime( ) const { return mAccelerationTime; }
	inline Camera* getCamera( ) const { return mCameraPtr; }
	inline float getCameraHeight( ) const { return mCameraHeight; }

	inline float isResting( ) const { return mResting; }
	inline float isAccelerating( ) const { return mAccelerating; }
	inline float isAtFullSpeed( ) const { return mAtFullSpeed; }
	inline float isDecelerating( ) const { return mDecelerating; }
	inline float isGrounded( ) const { return mGrounded; }
	inline float isJumping( ) const { return mJumping; }
	inline float isFalling( ) const { return mFalling; }

	inline float setAccelerationTime( float f ) { mAccelerationTime = f; }
	inline void setMoveKeyPressed( bool pressed ) { mMoveKeyPressed = pressed; }
	inline void setJumpKeyPressed( bool pressed ) { mJumpKeyPressed = pressed; }
	inline void setGrounded( bool g ) { mGrounded = g; }
	inline void setCamera( Camera* c ) { mCameraPtr = c; }
	inline void setCameraHeight( float h ) { mCameraHeight = h; }
	void setMoveSpeed( float speed );
	void setMaxMoveSpeed( float speed );
	void setMoveDirection( glm::vec3& dir );
	void setMoveDirectionDirect( const glm::vec3& dir );
	void setBlockMoveDirection( const glm::vec3& blockDir ) const;
	void setBlockGround( const Block* g );
	void updateGrounded( void );
	void updateMoveAmount( const float deltaTime );
	void updateJumpAmount( const float deltaTime );
	void updateCameraPos( void );

	const Block* getGround( void ) const;
	bool inTri( const Face& tri, const glm::vec3& point ) const;

	BlockController( void );
	BlockController( BlockCapsule* b );

protected:
	glm::vec3 mMoveDirection;
	glm::vec3 mGroundFaceNormal;
	glm::vec3 mJumpDirection;
	float mMoveAmount;
	float mMoveSpeed;
	float mMinMoveSpeed;
	float mMaxMoveSpeed;
	BlockCapsule* mCapsulePtr;
	Camera* mCameraPtr;
	float mCameraHeight;

	const float mUnitsPerMeter = 100;
	float mAccelerationTime = 300;
	float mDecelerationFrictionTimeFactor = 0.5f;
	float mJumpAmount = 0;
	float mMinJumpHeight = 100;
	float mJumpHeight = mMinJumpHeight;
	float mMaxJumpHeight = 600;
	float mJumpTime = 500;
	JumpAmountInterpolator* mJumpHeightInterpolatorPtr;

	//Movement Flags are to be set in the setDirection function
	bool mResting = true;
	bool mAccelerating = false;
	bool mAtFullSpeed = false;
	bool mDecelerating = false;
	bool mMoveKeyPressed = false;

	//Jumping Flags are to be set in the updateJumpDistance function
	bool mGrounded = false;
	bool mJumping = false;
	bool mFalling = true;
	bool mJumpKeyPressed = false;

	BezierCurve< float > mSpeedChangeCurve;
	TimerEvent< float >* mSpeedChangeTimerPtr;
	BezierCurve< float > mJumpChangeCurve;
	TimerEvent< float >* mJumpChangeTimerPtr;
};