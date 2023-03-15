
#include "TestBatchRendering.h"
#include "ImGUI/imgui.h"

#include <iostream>


// Started 6/26/2022

namespace Test
{
	TestBatchRendering::TestBatchRendering( ) :

		mWindowHeight( 1152.0f ),
		mWindowWidth( 1536.0f ),
		mWindowDepth( 15360.0f * 2 ),
		//mProj( glm::ortho( 0.0f, mWindowWidth, 0.0f, mWindowHeight, 0.0f, mWindowDepth ) ),
		mProj( glm::perspective( 45.0f, mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),
		mClearColor{ 0.2f, 0.3f, 0.9f, 1.0f },

		//mBlock( "res/DAE Files/Icosphere.dae" ),
		mBlock( "res/DAE Files/Capsule.dae", 50.0f, 50.0f ),
		mController( &mBlock ),
		mModelTranslation( 1100, 1000.0f, -1400.0f, 1.0f ),
		mModelScaleFactor( 50.0f ),
		mModelRotationAxis( 0.0f, 1.0f, 0.0f ),
		mModelRotationAngle( 0 ),
		mModelQuaternion( mModelRotationAxis, mModelRotationAngle ),
		mModelMoveDirection( glm::vec3( ) ),
		mModelMoveAmount( 5.0f ),
		mModelTranslationCube( 1300.0f, 701.0f, -1536.0f, 1.0f ),

		mCamera( Camera( ) ),
		mCameraSpeed( 15.0f ),

		mColorBlocks( ),
		mStaticModelBlocks( ),
		mStaticModelBlockCount( 0 ),
		mDynamicModelBlocks( ),
		mDynamicModelBlocksCount( 0 ),

		mSphereTesting( false ),
		mUnitTester( ),

		mModelTransforms( ),

		mBSP( ),

		mStaticColliderShader( std::make_unique<Shader>( "res/shaders/StaticCollider.shader" ) ),
		mDynamicModelShader( std::make_unique<Shader>( "res/shaders/DynamicTexture32.shader" ) ),
		mStaticModelShader( std::make_unique<Shader>( "res/shaders/StaticTexture32.shader" ) ),

		mTotalTriCount( 0 )
	{

		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		//Set Transforms

		//Set Block's max move speed
		mController.setMaxMoveSpeed( 6 );

		//Camera setup
		mCamera.setTranslation( glm::vec3( 1067, mWindowHeight / 2, 300.0f ) );
		if( mCamControl )
			mController.setCamera( nullptr );
		else
			mController.setCamera( &mCamera );

		mColorBlocks.reserve( 10 );
		mColorBlocks.emplace_back( "res/DAE Files/Cube.dae" );    // TODO 6/23/2022: Fix crash when loading HexStaircasePlus.dae
		mColorBlocks[ 0 ].updateTransform( mModelTranslationCube + glm::vec4( -300, 0, 0, 0 ), Quaternion( ), 100.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Triangle.dae" );
		mColorBlocks[ 1 ].updateTransform( mModelTranslationCube + glm::vec4( -200, -300, 0, 0 ), Quaternion( ), 1500.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Cube.dae" );
		mColorBlocks[ 2 ].updateTransform( mModelTranslationCube + glm::vec4( -200, -150, 0, 0 ), Quaternion( ), 10.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Cube.dae" );
		mColorBlocks[ 3 ].updateTransform( mModelTranslationCube + glm::vec4( 800, -300, 0, 0 ), Quaternion( glm::vec3( 1, 1, 0 ), 3.141f / 5.0f ), 500.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Terrain.dae" );
		mColorBlocks[ 4 ].updateTransform( mModelTranslationCube + glm::vec4( 1800, -300, 0, 0 ), Quaternion( ), 100.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Terrain.dae" );
		mColorBlocks[ 5 ].updateTransform( mModelTranslationCube + glm::vec4( -200, -300, -1000, 0 ), Quaternion( ), 100.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Terrain.dae" );
		mColorBlocks[ 6 ].updateTransform( mModelTranslationCube + glm::vec4( 800, -300, -1000, 0 ), Quaternion( ), 100.0f );
		mColorBlocks.emplace_back( "res/DAE Files/JankStairs.dae" );
		mColorBlocks[ 7 ].updateTransform( mModelTranslationCube + glm::vec4( 1800, -300, -1000, 0 ), Quaternion( ), 100.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Cube.dae" );
		mColorBlocks[ 8 ].updateTransform( glm::vec4( 0, 0, 0, 0 ), Quaternion( ), 12000.0f );
		mColorBlocks.emplace_back( "res/DAE Files/Cube.dae" );
		mColorBlocks[ 9 ].updateTransform( mModelTranslationCube + glm::vec4( -500, 0, 0, 0 ), Quaternion( ), 100.0f );

		//TODO 2/21/2023: Fix crash with Copied Game Object on updateTransform

		mStaticColliderShader->bind( );
		mStaticColliderShader->initRenderDataStaticCollider( mColorBlocks );
		mStaticColliderShader->unbind( );



		mStaticModelShader->bind( );
		int manuallyAddedBlockCount = 3;
		int X = 4;
		int Y = 3;
		int Z = 6;
		mStaticModelBlockCount = manuallyAddedBlockCount + X * Y * Z;
		mStaticModelBlocks.reserve( mStaticModelBlockCount );    // TODO 6/23/2022: Fix crash when loading HexStaircasePlus.dae
		mStaticModelBlocks.emplace_back( Block( "res/DAE Files/VarietyMat.dae" ) );
		mStaticModelBlocks[ 0 ].updateTransform( mModelTranslationCube + glm::vec4( 1800, -300, -1000, 0 ), Quaternion( ), 100.0f );
		mStaticModelBlocks.emplace_back( Block( "res/DAE Files/CobbleFloor.dae" ) );
		mStaticModelBlocks[ 1 ].updateTransform( mModelTranslationCube + glm::vec4( 1800, -300, -1000, 0 ), Quaternion( ), 100.0f );
		mStaticModelBlocks.emplace_back( Block( "res/DAE Files/CobbleFloor.dae" ) );
		mStaticModelBlocks[ 2 ].updateTransform( mModelTranslationCube + glm::vec4( 5000, -300, -1000, 0 ), Quaternion( ), 100.0f );

		glm::vec4 offsetX( 1600, 0, 0, 0 );
		glm::vec4 offsetY( 0, 1600, 0, 0 );
		glm::vec4 offsetZ( 0, 0, 1600, 0 );
		int blockIndex = manuallyAddedBlockCount;
		Block* templateBlock = new Block( "res/DAE Files/CobbleFloor.dae" );

		for( float i = 0; i < X; i++ )
		{
			for( float j = 0; j < Z; j++ )
			{
				for( float k = 0; k < Y; k++ )
				{
					mStaticModelBlocks.emplace_back( *templateBlock );
					mStaticModelBlocks[ blockIndex ].updateTransform( glm::vec4( -5000, 0, 1000, 0 ) + mModelTranslationCube + offsetX * i + offsetZ * j + offsetY * k,
						Quaternion( glm::vec3( 0, 1, 0 ), ( 3.141f / 2.0f ) * ( float ) ( std::rand( ) % 4 ) ), 100.0f );

					blockIndex++;
				}
			}
		}
		/*
		for( int i = manuallyAddedBlockCount + 1; i < mStaticModelBlockCount; i++ )
		{
			delete mStaticModelBlocks[ i ];
		}
		delete templateBlock;
		templateBlock = nullptr;   */

		mStaticModelShader->initRenderDataStaticModel( mStaticModelBlocks, mStaticModelBlockCount );
		mStaticModelShader->unbind( );


		// Dynamic Model Blocks
		mDynamicModelShader->bind( );
		mDynamicModelBlocksCount = 1;
		mDynamicModelBlocks.reserve( mDynamicModelBlocksCount );
		mDynamicModelBlocks.emplace_back( mBlock );

		for( int i = 0; i < 32; i++ )
			mModelTransforms[ i ] = glm::mat4( 1 );

		mModelTransforms[ 0 ] = mBlock.getCombinedTransform( );
		mDynamicModelShader->initRenderDataDynamicModel( mDynamicModelBlocks, mDynamicModelBlocksCount, mModelTransforms );
		mDynamicModelShader->unbind( );
		mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );

		// Setting Block's static
		mBlock.setStatic( false );
		for( GameObject& g : mColorBlocks )
		{
			g.setStatic( true );
		}
		for( uint32_t i = 0; i < mStaticModelBlockCount; i++ )
			mStaticModelBlocks[ i ].setStatic( true );

		mController.setCamera( &mCamera );

		uint32_t collidersCount = 0;
		// init TriCount for scene
		for( const GameObject& g : mColorBlocks )
		{
			collidersCount += g.getCollidersSize( );
			for( const Block& b : g.getColliders( ) )
				mTotalTriCount += b.getFaceCount( );
		}

		for( uint32_t i = 0; i < mStaticModelBlockCount; i++ )
			mTotalTriCount += mStaticModelBlocks[ i ].getFaceCount( );

		std::vector< const Block* > vec;
		vec.reserve( collidersCount + mStaticModelBlockCount );
		for( uint32_t i = 0; i < collidersCount + mStaticModelBlockCount; i++ )
		{
			if( i < collidersCount )
				vec.emplace_back( mColorBlocks[ i ].getColliderAddressAt( 0 ) );
			else
				vec.emplace_back( &( mStaticModelBlocks[ i - collidersCount ] ) );
		}

		mBSP.initAll( 12000.0f, 6, vec, mBlock );
	}

	TestBatchRendering::~TestBatchRendering( )
	{
	}

	void TestBatchRendering::getInputs( )
	{

	}

	void TestBatchRendering::onUpdate( float deltaTime )
	{
		mModelQuaternion = Quaternion( mModelRotationAxis, mModelRotationAngle );

		glm::vec3* collisionAmount = nullptr;

		glm::vec4 oldTranslation = mModelTranslation;
		//mController.updateMoveAmount( deltaTime );
		//mController.updateGrounded( );
		//mController.updateJumpAmount( deltaTime );
		//mBlock.updatePhysics( deltaTime, mBlock.isGrounded( ) );

		glm::vec4 move = glm::vec4( mController.getFrameTranslation( ), 0 );
		mModelTranslation += move;
		mController.setBlockMoveDirection( mController.getFrameTranslation( ) );

		mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );
		/*
		mEdgeCase = false;
		if( mBlock.getMoveDirectionPtr( )->x || mBlock.getMoveDirectionPtr( )->y || mBlock.getMoveDirectionPtr( )->z )
		{
			collisionAmount = new glm::vec3( );
			int count = 0;
			mTestBlockSearchCount = 0;

			//Look through all blocks in the Player's current BSPE: 1st do sphere check on each block, if check passed then
			//2nd do sphere check on each Face in that block, if check passed then 3rd do gjk check on Face, if passed then
			//4th move block based on gjk return value and repeat loop from begining

			//Collision Check loop
			while( collisionAmount && count < 100 )
			{
				count++;

				delete collisionAmount;
				collisionAmount = nullptr;

				std::vector< glm::vec3 > collisions;
				std::vector< Face* > collidingFaces;
				std::vector< Block* > vec = mBSP->getBlocksInLeaf( mBlock.getTranslation( ) );
				for( unsigned int i = 0; i < vec.size( ); i++ )
				{
					Block* b = vec.at( i );

					if( mBlock.getSpherePtr( )->isContacting( *( b->getSpherePtr( ) ) ) )
					{
						for( int j = 0; j < b->getFaceCount( ); j++ )
						{
							Face* f( &( b->getFaces( )[ j ] ) );
							if( mBlock.getSpherePtr( )->isContacting( *( f->getSpherePtr( ) ) ) )
							{
								collisionAmount = mBlock.gjkCollisionFace( *f, &mEdgeCase, mTestPenDepthSearchCount );
								if( mEdgeCase )
									break;

								if( collisionAmount )
								{
									collisions.push_back( *collisionAmount );
									collidingFaces.push_back( f );

									delete collisionAmount;
									collisionAmount = nullptr;
								}
							}
						}
						if( mEdgeCase )
							break;
					}
				}
				if( mEdgeCase )
					break;

				mTestBlockSearchCount = count;

				if( count == 5 )
				{
					//std::cout << "\nERROR: Collision Detection ran into a loop.";
					mEdgeCase = true;
				}

				if( collisions.size( ) > 0 )
				{
					glm::vec3 oldNormal = collidingFaces.at( 0 )->getNormal( ), newNormal;
					glm::vec3 potentialMovement = collisions.at( 0 );
					float max = glm::dot( collisions.at( 0 ), oldNormal );
					int index = 0;
					for( unsigned int i = 1; i < collisions.size( ); i++ )
					{
						newNormal = collidingFaces.at( i )->getNormal( );
						float temp = glm::dot( collisions.at( i ), newNormal );
						if( temp > max && oldNormal.x - newNormal.x + oldNormal.y - newNormal.y + oldNormal.z - newNormal.z < 0.001f )
						{
							max = temp;
							potentialMovement = collisions.at( i );
							index = i;
						}
					}
					move = glm::vec4( potentialMovement, 0.0f );

					mModelTranslation += move;
					mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );

					if( collisions.at( index ).y > 0 )
					{
						mController.setBlockGround( collidingFaces.at( index ), collisions.at( index ) );
						mController.setGrounded( true );
					}

					collisionAmount = new glm::vec3( );
				}
			}
			if( collisionAmount )
			{
				delete collisionAmount;
				collisionAmount = nullptr;
			}
		}
		if( mEdgeCase )
		{
			mModelTranslation = oldTranslation;
			mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );
		}
		*/
		if( !mThirdPerson )
			mController.updateCameraPos( );

	}

	void TestBatchRendering::onRender( )
	{
		glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
		glCall( glDepthRangef( -1, 1 ) );
		
		// Player Renderer
		if( mThirdPerson )
		{
			for( int i = 0; i < 32; i++ )
				mModelTransforms[ i ] = glm::mat4( 1 );

			mModelTransforms[ 0 ] = mBlock.getCombinedTransform( );

			mDynamicModelShader->drawDynamicModel( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ), mModelTransforms );
		}
		// Collider Static Renderer
		{
			mStaticColliderShader->drawStaticCollider( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
		// Model Static Renderer
		{
			mStaticModelShader->drawStaticModel( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}

	}

	void TestBatchRendering::onImGuiRender( )
	{
		ImGui::SliderFloat3( "Model Translation", &mModelTranslation.x, -1536.0f, 1536.0f );

		ImGui::SliderFloat3( "Model Rotation Axis", &mModelRotationAxis.x, -1.0f, 1.0f );
		ImGui::SliderFloat( "Model Rotation Angle", &mModelRotationAngle, 0.0f, 2.0f * 3.141f );

		ImGui::SliderFloat( "Model Scale Factor", &mModelScaleFactor, 0.0f, 500.0f );

		ImGui::SliderFloat3( "Model Move Direction", &mModelMoveDirection.x, -1.0f, 1.0f );

		ImGui::SliderFloat( "Camera Speed", &mCameraSpeed, 5.0f, 75.0f );

		if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_Tab ) ) )
			mController.setMaxMoveSpeed( 10 );
		else
			mController.setMaxMoveSpeed( 6 );

		if( mCamControl )
		{
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_LeftArrow ) ) )
				mCamera.changeYaw( 3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_RightArrow ) ) )
				mCamera.changeYaw( -3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_UpArrow ) ) )
				mCamera.changePitch( 3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_DownArrow ) ) )
				mCamera.changePitch( -3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_Space ) ) )
				mCamera.setTranslation( mCamera.getTranslationVec3( ) + mCamera.getUpDirection( ) * mCameraSpeed );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_Tab ) ) )
				mCamera.setTranslation( mCamera.getTranslationVec3( ) - mCamera.getUpDirection( ) * mCameraSpeed );

			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_W ) ) )
				mCamera.setTranslation( mCamera.getTranslationVec3( ) + mCamera.getForwardDirection( ) * mCameraSpeed );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_S ) ) )
				mCamera.setTranslation( mCamera.getTranslationVec3( ) - mCamera.getForwardDirection( ) * mCameraSpeed );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_A ) ) )
			{
				glm::vec3 right = glm::normalize( glm::cross( mCamera.getForwardDirection( ), glm::vec3( 0, 1, 0 ) ) );
				mCamera.setTranslation( mCamera.getTranslationVec3( ) - right * mCameraSpeed );
			}
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_D ) ) )
			{
				glm::vec3 right = glm::normalize( glm::cross( mCamera.getForwardDirection( ), glm::vec3( 0, 1, 0 ) ) );
				mCamera.setTranslation( mCamera.getTranslationVec3( ) + right * mCameraSpeed );
			}
		}
		else
		{
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_Space ) ) )
				mController.setJumpKeyPressed( true );
			else
				mController.setJumpKeyPressed( false );

			//mBlock's Movement from keyboard
			glm::vec3 dir = glm::vec3( );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_W ) ) )
				dir.z -= 1.0f;
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_S ) ) )
				dir.z += 1.0f;
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_A ) ) )
				dir.x -= 1.0f;
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_D ) ) )
				dir.x += 1.0f;
			//mBlock's Camera Controls
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_LeftArrow ) ) )
				mCamera.changeYaw( 3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_RightArrow ) ) )
				mCamera.changeYaw( -3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_UpArrow ) ) )
				mCamera.changePitch( 3.141f / 100 );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_DownArrow ) ) )
				mCamera.changePitch( -3.141f / 100 );

			if( dir.x || dir.y || dir.z )
			{
				mController.setMoveKeyPressed( true );
				mModelMoveDirection = glm::normalize( dir );
			}
			else
				mController.setMoveKeyPressed( false );
		}

		if( ImGui::Button( "Print Forward Direction" ) )
		{
			glm::vec3 forward( mCamera.getForwardDirection( ) );
			std::cout << "\nCamera's Forward Direction: \n  ( " << forward.x << ", " << forward.y << ", " << forward.z << " )";
		}

		if( ImGui::Button( "Print mBlock Vertices w/ Normals" ) )
			mBlock.printVerticesWithNormals( );

		if( ImGui::Button( "Print mBlock Normals" ) )
		{
			for( int i = 0; i < mBlock.getFaceCount( ); i++ )
			{
				std::cout << "\nFace #" << i << "\n";
				Face f = mBlock.getFaceAtIndex( i );
				for( int i = 0; i < 3; i++ )
					f.getVertexAt( i ).print( );
				f.printNormal( );
			}
		}

		mController.setMoveDirection( mModelMoveDirection );

		if( mBlock.isGrounded( ) )
			ImGui::Text( "Grounded: YES" );
		else
			ImGui::Text( "Grounded: NO" );

		ImGui::Text( "Application Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO( ).Framerate, ImGui::GetIO( ).Framerate );

		ImGui::Text( "Tri Count in scene: %.3f", ( float ) mTotalTriCount );
		ImGui::Text( "Count of Faces mBlock has searched through: %.3f", ( float ) mTestBlockSearchCount );

		if( ImGui::Button( "Begin Sphere Collision Testing" ) )
			mSphereTesting = !mSphereTesting;

		if( ImGui::Button( "Begin Quaternion Testing" ) )
			mUnitTester.testQuaternion( );

		if( ImGui::Button( "Camera Block Control Toggle" ) )
			mCamControl = !mCamControl;

		if( ImGui::Button( "Third / First Person Toggle" ) )
			mThirdPerson = !mThirdPerson;

		std::vector< const Block* > vec = mBSP.getBlocksInLeaf( mModelTranslation );
		ImGui::Text( "Blocks in Player's BSP element: %.0f", ( float ) vec.size( ) );

	}
}