#pragma once

#include "glm/glm.hpp"
#include "Block/BlockController.h"
#include "Math/Quaternion.h"
#include "TimerEvent.h"


class BlockUnitTests
{
public:

	void testGJKCollision( void ) const
	{
		Block b( "res/DAE Files/DesktopCube.dae" );
		b.updateTransform( glm::vec4( 50, 0, 0, 1 ), Quaternion( glm::vec3( 1, 0, 0 ), 0 ), 100 );

		mController.setBlockMoveDirection( glm::vec3( 1, 0, 0 ) );
	}

	void testTimerEvent( void ) const
	{
		BezierCurve< float > bc = BezierCurve< float >( 0, 3.69f, 2.0f, 3 );	
		TimerEvent< float >* mTimerPtr = new TimerEvent< float >( 500.0f, 0.0f, 1.0f, bc );
		float timeStep = 50;


		while( !mTimerPtr->shouldEnd( ) )
		{
			std::cout << "\nTimer Event active. Elapsed time: " << mTimerPtr->getTimeElapsed( ) << " Data: " << mTimerPtr->getData( );

			mTimerPtr->tick( timeStep );
		}

		std::cout << "\nTimer Event active. Elapsed time: " << mTimerPtr->getTimeElapsed( ) << " Data: " << mTimerPtr->getData( );

		delete mTimerPtr;
		mTimerPtr = nullptr;
	}

	void testQuaternion( void ) const
	{
		Quaternion rot1 = Quaternion( glm::vec3( 0, 1, 0 ), 3.141f / 2 );
		//Quaternion rot2 = Quaternion( glm::vec3( 1, 0, 0 ), 3.141f / 2 );
		//rot1.composeWith( rot2 );
		glm::vec4 testPoint = glm::vec4( 1, 0, 0, 1 );

		testPoint = testPoint * rot1.toMat4( );


	}

	bool testSphereTriCollision( Block* sphere, Block* tri )
	{
		float radius = 50.0f;
		glm::vec3 center = sphere->getTranslation( );
		Face face = tri->getFaces( )[ 0 ];
		glm::vec3 points[ 3 ] =
		{
			face.getVec3At( 0 ),
			face.getVec3At( 1 ),
			face.getVec3At( 2 )
		};
		glm::vec3 faceNormal = face.getNormal( );
		faceNormal = glm::normalize( faceNormal );
		glm::vec3 proj = center - faceNormal * glm::dot( faceNormal, center - points[ 0 ] );

		if( glm::dot( glm::cross( faceNormal, points[ 1 ] - points[ 0 ] ), proj - points[ 1 ] ) > 0 )
		{
			if( glm::dot( proj - points[ 1 ], points[ 0 ] - points[ 1 ] ) < 0 )
				proj = points[ 1 ];

			else if( glm::dot( proj - points[ 0 ], points[ 1 ] - points[ 0 ] ) < 0 )
				proj = points[ 0 ];

			else 
			{
				faceNormal = glm::normalize( glm::cross( faceNormal, points[ 1 ] - points[ 0 ] ) );
				proj = proj + faceNormal * glm::dot( points[ 0 ] - proj, faceNormal );
			}
		}
		else if( glm::dot( glm::cross( faceNormal, points[ 2 ] - points[ 1 ] ), proj - points[ 2 ] ) > 0 )
		{
			if( glm::dot( proj - points[ 1 ], points[ 2 ] - points[ 1 ] ) < 0 )
				proj = points[ 1 ];

			else if( glm::dot( proj - points[ 2 ], points[ 1 ] - points[ 2 ] ) < 0 )
				proj = points[ 2 ];

			else
			{
				faceNormal = glm::normalize( glm::cross( faceNormal, points[ 2 ] - points[ 1 ] ) );
				proj = proj + faceNormal * glm::dot( points[ 1 ] - proj, faceNormal );
			}
		}
		else if( glm::dot( glm::cross( faceNormal, points[ 0 ] - points[ 2 ] ), proj - points[ 0 ] ) > 0 )
		{
			if( glm::dot( proj - points[ 0 ], points[ 2 ] - points[ 0 ] ) < 0 )
				proj = points[ 0 ];

			else if( glm::dot( proj - points[ 2 ], points[ 0 ] - points[ 2 ] ) < 0 )
				proj = points[ 2 ];

			else
			{
				faceNormal = glm::normalize( glm::cross( faceNormal, points[ 0 ] - points[ 2 ] ) );
				proj = proj + faceNormal * glm::dot( points[ 2 ] - proj, faceNormal );
			}
		}

		proj = proj - center;

		if( proj.x * proj.x + proj.y * proj.y + proj.z * proj.z < radius * radius )
		{
			return true;
		}
		return false;
	}

	BlockUnitTests( ) : mBlock( "res/DAE Files/Capsule.dae", 50.0f, 50.0f ), mController( &mBlock )
	{
		mController.setMoveSpeed( 300 );
		mBlock.updateTransform( glm::vec4( 0, 0, 0, 1 ), Quaternion( glm::vec3( 1, 0, 0 ), 0 ), 100 );
	}

	BlockUnitTests( const char* filePath ) : mBlock( "res/DAE Files/Capsule.dae", 50.0f, 50.0f ), mController( &mBlock )
	{
		mController.setMoveSpeed( 300 );
		mBlock.updateTransform( glm::vec4( 0, 0, 0, 1 ), Quaternion( glm::vec3( 1, 0, 0 ), 0 ), 100 );
	}

private:	
	BlockCapsule mBlock;
	BlockController mController;
};