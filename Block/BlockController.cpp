
#include "Block/BlockController.h"





glm::vec3 BlockController::getFrameTranslation( ) const
{
	glm::vec3 translation = mMoveDirection * mMoveAmount + mJumpDirection * mJumpAmount;
	if( mFalling )
		translation += mCapsulePtr->getFallTranslation( );

	float length = glm::sqrt( translation.x * translation.x + translation.y * translation.y + translation.z * translation.z );
	if( length < 50.0f )
		return translation;
	else
		return translation / length;
}

//Updates the movement flags
void BlockController::setMoveDirection( glm::vec3& dir )
{
	if( mCameraPtr && mMoveKeyPressed )
	{
		Quaternion yaw = mCameraPtr->getYaw( );
		yaw.rotate( dir );
	}

	if( mCapsulePtr->isGrounded( ) )
		mMoveDirection = dir - ( glm::dot( dir, mGroundFaceNormal ) / glm::dot( mGroundFaceNormal, mGroundFaceNormal ) ) * mGroundFaceNormal;

	else
		mMoveDirection = dir;

	
	if( mMoveKeyPressed )
	{
		if( mResting )
		{
			mResting = false;
			mAccelerating = true;

			mSpeedChangeCurve = BezierCurve< float >( mMinMoveSpeed, 1.23f * mMaxMoveSpeed, 0.66f * mMaxMoveSpeed, mMaxMoveSpeed );
			mSpeedChangeTimerPtr = new TimerEvent< float >( mAccelerationTime, mMinMoveSpeed, 1.0f, mSpeedChangeCurve );
		}
		if( mAccelerating )
		{
			if( mSpeedChangeTimerPtr->shouldEnd( ) )
			{
				mAccelerating = false;
				mAtFullSpeed = true;

				delete mSpeedChangeTimerPtr;
				mSpeedChangeTimerPtr = nullptr;
			}
		}
		if( mDecelerating )
		{
			mDecelerating = false;
			mAccelerating = true;
			float timeRemaining = mSpeedChangeTimerPtr->getTimeElapsed( );

			mSpeedChangeCurve = BezierCurve< float >( mMoveSpeed, 1.23f * mMaxMoveSpeed, 0.66f * mMaxMoveSpeed, mMaxMoveSpeed );
			delete mSpeedChangeTimerPtr;
			mSpeedChangeTimerPtr = new TimerEvent< float >( timeRemaining, mMoveSpeed, 1.0f, mSpeedChangeCurve );
		}
	}
	else
	{
		if( mAccelerating || mAtFullSpeed )
		{
			mAccelerating = false;
			mAtFullSpeed = false;
			mDecelerating = true;
			
			float timeRemaining = mAccelerationTime;

			if( mSpeedChangeTimerPtr )
			{
				timeRemaining = mSpeedChangeTimerPtr->getTimeElapsed( ) * mDecelerationFrictionTimeFactor;
				delete mSpeedChangeTimerPtr;
				mSpeedChangeTimerPtr = nullptr;
			}
			//Deceleration Curve
			mSpeedChangeCurve = BezierCurve< float >( mMoveSpeed, -0.39f * mMoveSpeed, 0.18f * mMoveSpeed, mMinMoveSpeed );
			mSpeedChangeTimerPtr = new TimerEvent< float >( timeRemaining, mMinMoveSpeed, 1.0f, mSpeedChangeCurve );
		}
	}
	if( mSpeedChangeTimerPtr )
	{
		if( mSpeedChangeTimerPtr->shouldEnd( ) )
		{
			if( mAccelerating )
			{
				mAccelerating = false;
				mAtFullSpeed = true;
			}
			if( mDecelerating )
			{
				mDecelerating = false;
				mResting = true;
			}

			delete mSpeedChangeTimerPtr;
			mSpeedChangeTimerPtr = nullptr;
		}
	}
}

void BlockController::setMoveDirectionDirect( const glm::vec3& dir )
{
	mMoveDirection = dir;
}

void BlockController::setMoveSpeed( float speed )
{
	mMoveSpeed = speed;
}

void BlockController::setMaxMoveSpeed( float speed )
{
	mMaxMoveSpeed = speed;
}

void BlockController::setBlockMoveDirection( const glm::vec3& blockDir ) const
{
	mCapsulePtr->setMoveDirection( blockDir );
}

void BlockController::setBlockGround( const Block* b )
{
	mCapsulePtr->setGround( b );
	mGroundFaceNormal = mCapsulePtr->getGroundNormal( *b );
}

//Slow Method, change later	8/15/2021			
void BlockController::updateGrounded( )
{
	
	if( mGrounded )
	{
		if( !mCapsulePtr->checkGrounded( ) )
		{
			mGrounded = false;
			mFalling = true;
			mCapsulePtr->setGround( nullptr );
			mGroundFaceNormal = glm::vec3( );
		}
	}
}

// Tick the speed TimerEvent, Change speed based on speedChangeCurve, then update moveAmount
void BlockController::updateMoveAmount( const float deltaTime )
{
	if( mResting )
		return;

	if( mAccelerating || mDecelerating )
	{
		mSpeedChangeTimerPtr->tick( deltaTime );
		mMoveSpeed = mSpeedChangeTimerPtr->getData( );
	}
	if( mAtFullSpeed )
		mMoveSpeed = mMaxMoveSpeed;

	mMoveAmount = mMoveSpeed * deltaTime * mUnitsPerMeter / 1000;
}

void BlockController::updateJumpAmount( const float deltaTime )
{
	if( mJumpKeyPressed )
	{
		if( mJumping && mJumpHeightInterpolatorPtr )
		{
			if( !mJumpHeightInterpolatorPtr->shouldEnd( ) )
			{
				mJumpHeightInterpolatorPtr->tick( deltaTime );
				mJumpHeight = mJumpHeightInterpolatorPtr->getHeight( );
				mJumpChangeCurve = BezierCurve< float >( 0, 1.0f * mJumpHeight, 1.0f * mJumpHeight, mJumpHeight );
				delete mJumpChangeTimerPtr;
				mJumpChangeTimerPtr = new TimerEvent< float >( mJumpTime * mJumpHeight / mMaxJumpHeight, 0, 0.8f, mJumpChangeCurve );

				mJumpChangeTimerPtr->tick( mJumpHeightInterpolatorPtr->getTimeElapsed( ) - deltaTime );
			}
			else
			{
				delete mJumpHeightInterpolatorPtr;
				mJumpHeightInterpolatorPtr = nullptr;
			}
		}

		if( mGrounded )
		{
			mGrounded = false;
			mJumping = true;
			mCapsulePtr->setGround( nullptr );
			mCapsulePtr->getPhys( )->resetFalling( );

			mJumpHeight = mMinJumpHeight;
			mJumpChangeCurve = BezierCurve< float >( 0, 1.0f * mJumpHeight, 1.0f * mJumpHeight, mJumpHeight );
			mJumpHeightInterpolatorPtr = new JumpAmountInterpolator( mMinJumpHeight, mMaxJumpHeight, mJumpTime / 4 );
			mJumpChangeTimerPtr = new TimerEvent< float >( mJumpTime * mJumpHeight / mMaxJumpHeight, 0, 0.8f, mJumpChangeCurve );
		}
	}
	else if( mJumpHeightInterpolatorPtr )
	{
		delete mJumpHeightInterpolatorPtr;
		mJumpHeightInterpolatorPtr = nullptr;
	}
	if( mGrounded )		//mGrounded gets set true in application if jump ends early
	{
		mJumping = false;
		mFalling = false;

		if( mJumpChangeTimerPtr )
		{
			delete mJumpChangeTimerPtr;
			mJumpChangeTimerPtr = nullptr;
		}
		mJumpAmount = 0;
	}
	if( mJumping )
	{
		if( !mJumpChangeTimerPtr->shouldEnd( ) )
		{
			float prev = mJumpChangeTimerPtr->getData( );
			mJumpChangeTimerPtr->tick( deltaTime );
			float current = mJumpChangeTimerPtr->getData( );

			mJumpAmount = ( current - prev ) / ( deltaTime / 10 );
		}
		else
		{
			mJumping = false;
			mFalling = true;
			mJumpAmount = 0;

			delete mJumpChangeTimerPtr;
			mJumpChangeTimerPtr = nullptr;
		}
	}
}

void BlockController::updateCameraPos( void )
{
	mCameraPtr->setTranslation( mCapsulePtr->getTranslation( ) + glm::vec3( 0, mCameraHeight, 0 ) );
}

const Block* BlockController::getGround( ) const
{
	// This function is called after the Player has been moved up after its collision checks, and we know Block b has the face that is its ground

	return mCapsulePtr->getGround( );
}

bool BlockController::inTri( const Face& tri, const glm::vec3& point ) const // point is in the plane of tri
{
	// Line 1 -> 0
	glm::vec3 edgeNormal = glm::cross( tri.getNormal( ), tri.getVec3At( 0 ) - tri.getVec3At( 1 ) );

	if( glm::dot( edgeNormal, tri.getVec3At( 2 ) ) > 0 )
		edgeNormal = -edgeNormal;

	if( glm::dot( edgeNormal, point - tri.getVec3At( 0 ) ) > 0 )
		return false;

	// Line 2 -> 1
	edgeNormal = glm::cross( tri.getNormal( ), tri.getVec3At( 1 ) - tri.getVec3At( 2 ) );

	if( glm::dot( edgeNormal, tri.getVec3At( 0 ) ) > 0 )
		edgeNormal = -edgeNormal;

	if( glm::dot( edgeNormal, point - tri.getVec3At( 1 ) ) > 0 )
		return false;

	// Line 0 -> 2
	edgeNormal = glm::cross( tri.getNormal( ), tri.getVec3At( 2 ) - tri.getVec3At( 0 ) );

	if( glm::dot( edgeNormal, tri.getVec3At( 1 ) ) > 0 )
		edgeNormal = -edgeNormal;

	if( glm::dot( edgeNormal, point - tri.getVec3At( 2 ) ) > 0 )
		return false;

	return true;
}

//		mGroundFaceNormal lets the controller's move direction be parallel to the face Block is grounded to.
BlockController::BlockController( void ) : mMoveDirection( ), mGroundFaceNormal( ), mMoveSpeed( 0 ), mMinMoveSpeed( 0 ), mMaxMoveSpeed( 0 ), 
	mMoveAmount( 0 ), mCapsulePtr( nullptr ), mCameraPtr( nullptr ), mSpeedChangeCurve( ), mSpeedChangeTimerPtr( nullptr ), mJumpChangeCurve( ), 
	mJumpChangeTimerPtr( nullptr ), mJumpDirection( 0, 1, 0 ), mJumpHeightInterpolatorPtr( nullptr ), mCameraHeight( 50.0f )
{
}

BlockController::BlockController( BlockCapsule* b ) : mMoveDirection( ), mGroundFaceNormal( ), mMoveSpeed( 0 ), mMinMoveSpeed( 0 ), mMaxMoveSpeed( 3 ),
	mMoveAmount( 0 ), mCapsulePtr( b ), mCameraPtr( nullptr ), mSpeedChangeCurve( ), mSpeedChangeTimerPtr( nullptr ), mJumpChangeCurve( ), mJumpChangeTimerPtr( nullptr ),
	mJumpDirection( 0, 1, 0 ), mJumpHeightInterpolatorPtr( nullptr ), mCameraHeight( 50.0f )
{
}