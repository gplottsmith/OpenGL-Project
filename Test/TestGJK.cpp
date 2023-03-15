
#include "TestGJK.h"
#include <iostream>

#define VECTOR_ZERO( a ) a.x == 0 && a.y == 0 && a.z == 0 ? true : false
#define VECTOR_TINY( a ) glm::abs( a.x ) + glm::abs( a.y ) + glm::abs( a.z ) < 0.01f ? true : false

// Started 7/10/2022

namespace Test
{
	TestGJK::TestGJK( ) :

		mWindowHeight( 1152.0f ),
		mWindowWidth( 1536.0f ),
		mWindowDepth( 15360.0f * 2 ),
		//mProj( glm::ortho( 0.0f, mWindowWidth, 0.0f, mWindowHeight, 0.0f, mWindowDepth ) ),
		mProj( glm::perspective( 45.0f, mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),
		mClearColor{ 0.2f, 0.3f, 0.9f, 1.0f },

		mCube( "res/DAE Files/SegmentedCube.dae" ), 
		mPlayer( "res/DAE Files/Capsule.dae", 50.0f, 50.0f ),
		mController( &mPlayer ),
		mModelTranslation( 1000, 805.0f, -1500.0f, 1.0f ),
		mModelScaleFactor( 50.0f ),
		mModelRotationAxis( 0.0f, 1.0f, 0.0f ),
		mModelRotationAngle( 0 ),
		mModelQuaternion( mModelRotationAxis, mModelRotationAngle ),
		mModelMoveDirection( glm::vec3( ) ),
		mModelMoveAmount( 5.0f ),
		mModelTranslationCube( 1300.0f, 701.0f, -1536.0f, 1.0f ),

		mCamera( Camera( ) ),
		mCameraSpeed( 15.0f ),

		mObjects( ),
		mDynamicModelBlocks( ),
		mDynamicModelBlocksCount( 0 ),
		mStaticModelBlocks( ),
		mStaticModelBlocksCount( 0 ),

		mSphereTesting( false ),
		mUnitTester( ),

		mModelTransforms( ),

		mBSP( nullptr ),

		mStaticColliderShader( std::make_unique<Shader>( "res/shaders/StaticCollider.shader" ) ),
		mDynamicModelShader( std::make_unique<Shader>( "res/shaders/DynamicTexture32.shader" ) ),
		mStaticModelShader( std::make_unique<Shader>( "res/shaders/StaticTexture32.shader" ) ),

		mTotalTriCount( 0 ),
		mGJKLoopCount( 0 ),
		mOutMove( 0 )
	{

		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		mCube.updateTransform( glm::vec4( 0, 0, 0, 1 ), Quaternion( glm::vec3( 0, 0, 1 ), 3.141f / 4 ), 2 );

		//Set Block's max move speed
		mController.setMaxMoveSpeed( 6 );
		mPlayer.setSimpleCollision( true );

		//Camera setup
		mCamera.setTranslation( glm::vec3( 1067, mWindowHeight / 2, 300.0f ) );
		mCamera.changeYaw( 3.141f / 18 );
		if( mCamControl )
			mController.setCamera( nullptr );
		else
			mController.setCamera( &mCamera );

		mObjects.reserve( 4 );
		mObjects.emplace_back( "res/DAE Files/CenterStairsModel.dae", "res/DAE Files/CenterStairsCollider.dae" );
		mObjects[ 0 ].updateTransform( mModelTranslationCube + glm::vec4( -1100, 0, 600, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), -3.141f / 2 ), 210.0f );
		mObjects.emplace_back( "res/DAE Files/StairsModel.dae", "res/DAE Files/StairsCollider.dae" );
		mObjects[ 1 ].updateTransform( mModelTranslationCube + glm::vec4( 350, -100, 600, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), -3.141f / 2 ), 100.0f );
		mObjects.emplace_back( "res/DAE Files/WarehouseModel.dae", "res/DAE Files/WarehouseCollider.dae" );
		mObjects[ 2 ].updateTransform( mModelTranslationCube + glm::vec4( 3000, 200, 600, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), -3.141f / 2 ), 150.0f );
		mObjects.emplace_back( "res/DAE Files/CobbleFloor.dae", "res/DAE Files/CobbleFloorColliders.dae" );
		mObjects[ 3 ].updateTransform( mModelTranslationCube + glm::vec4( 1000, -100, 600, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), -3.141f / 2 ), 100.0f );

		// Static Model Blocks
		mStaticModelBlocksCount = 4;
		mStaticModelBlocks.reserve( mStaticModelBlocksCount );
		mStaticModelBlocks.emplace_back( mObjects[ 0 ].getModel( ) );
		mStaticModelBlocks.emplace_back( mObjects[ 1 ].getModel( ) );
		mStaticModelBlocks.emplace_back( mObjects[ 2 ].getModel( ) );
		mStaticModelBlocks.emplace_back( mObjects[ 3 ].getModel( ) );

		mStaticModelShader->bind( );
		mStaticModelShader->initRenderDataStaticModel( mStaticModelBlocks, mStaticModelBlocksCount );
		mStaticModelShader->unbind( );

		mStaticColliderShader->bind( );
		mStaticColliderShader->initRenderDataStaticCollider( mObjects );
		mStaticColliderShader->unbind( );


		// Dynamic Model Blocks
		mDynamicModelShader->bind( );
		mDynamicModelBlocksCount = 1;
		mDynamicModelBlocks.reserve( mDynamicModelBlocksCount );
		mDynamicModelBlocks.emplace_back( mPlayer );

		for( int i = 0; i < 32; i++ )
			mModelTransforms[ i ] = glm::mat4( 1 );

		mModelTransforms[ 0 ] = mPlayer.getCombinedTransform( );
		mDynamicModelShader->initRenderDataDynamicModel( mDynamicModelBlocks, mDynamicModelBlocksCount, mModelTransforms );
		mDynamicModelShader->unbind( );
		mPlayer.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );

		// Setting Block's static
		mPlayer.setStatic( false );
		for( uint32_t i = 0; i < mObjects.size( ); i++ )
			mObjects[ i ].setStatic( true );

		mController.setCamera( &mCamera );

		// init TriCount for scene
		mTotalTriCount += mPlayer.getFaceCount( );
		for( const GameObject& g : mObjects )
		{
			for( const Block& b : g.getColliders( ) )
				mTotalTriCount += b.getFaceCount( );
		}
		for( uint32_t i = 0; i < mStaticModelBlocks.size( ); i++ )
			mTotalTriCount += mStaticModelBlocks[ i ].getFaceCount( );

		std::vector< const Block* > vec;
		uint32_t colliderCount = 0;
		for( const GameObject& g : mObjects )
			colliderCount += g.getCollidersSize( );
		vec.reserve( colliderCount );
		for( const GameObject& g : mObjects )
		{
			for( uint32_t i = 0; i < g.getCollidersSize( ); i++ )
				vec.emplace_back( g.getColliderAddressAt( i ) );
		}
		//for( uint32_t i = 0; i < mStaticModelBlocks.size( ); i++ )
			//mStaticModelBlocks[ i ].initBoundarySphereWire( );

		mBSP = new BinarySpacePartition( 12000.0f, 2, vec, mPlayer );
	}

	TestGJK::~TestGJK( )
	{
		delete mBSP;
		mBSP = nullptr;
	}

	void TestGJK::getInputs( )
	{

	}

	void TestGJK::onUpdate( float deltaTime )
	{
		mPlayer.setSimpleCollision( true );
		mGJKLoopCount = 0;

		glm::vec3 moveDir = *( mPlayer.getMoveDirectionPtr( ) );
		glm::vec4 originalPos = mModelTranslation;

		mController.updateMoveAmount( deltaTime );
		mController.updateGrounded( );
		mController.updateJumpAmount( deltaTime );
		mPlayer.updatePhysics( deltaTime, mPlayer.isGrounded( ) );

		glm::vec4 move = glm::vec4( mController.getFrameTranslation( ), 0 );
		mModelTranslation += move;
		mController.setBlockMoveDirection( mController.getFrameTranslation( ) );

		glm::vec4 originalPosWithMove = mModelTranslation;
		glm::vec3 originalMove( moveDir );

		mPlayer.updateTransform( mModelTranslation, Quaternion( ), mModelScaleFactor );

		mOutMove = glm::vec3( 0.0f );

		if( !( VECTOR_ZERO( moveDir ) ) )
		{
			//Look through all blocks in the Player's current BSPE: 1st do sphere check on each block, if check passed then
			//2nd do GJK collision and move player if necessary. Repeat until player does not move

			//Collision Check loop
			
			std::vector< const Block* > bspVec = mBSP->getBlocksInLeaf( mPlayer.getTranslation( ) );

			for( unsigned int i = 0; i < bspVec.size( ); i++ )
			{
				const Block* b = bspVec.at( i );

				if( mPlayer.getSphereAdr( )->isContacting( *( b->getSphereAdr( ) ) ) )
				{
					mGJKIntersect = mPlayer.gjkCollision( *b, moveDir, mOutMove );

					if( !( VECTOR_TINY( mOutMove ) ) )
					{
						if( mOutMove.y > 0 )
						{
							mController.setBlockGround( b );
							mController.setGrounded( true );
						}
						if( mGJKIntersect )
						{
							mGJKLoopCount++;

							mModelTranslation += glm::vec4( mOutMove, 0 );
							mPlayer.updateTransform( mModelTranslation, Quaternion( ), mModelScaleFactor );

							if( mPlayer.getSimpleCollision( ) )
							{
								glm::vec3 moveProj = -glm::dot( glm::normalize( mOutMove ), moveDir ) * glm::normalize( mOutMove );
								moveDir = moveDir + moveProj;

								if( VECTOR_ZERO( moveDir ) )
									break;
								// Only reset the search loop if we're in Simple Collision mode
								i = -1;	
							}
							if( mGJKLoopCount >= 20 && mPlayer.getSimpleCollision( ) )
							{
								mPlayer.setSimpleCollision( false );
								mModelTranslation = originalPosWithMove;
								mPlayer.updateTransform( mModelTranslation, Quaternion( ), mModelScaleFactor );
								moveDir = originalMove;
							}
							if( glm::length( originalMove ) < glm::length( mOutMove ) )
							{
								mModelTranslation = originalPos;
								mPlayer.updateTransform( mModelTranslation, Quaternion( ), mModelScaleFactor );
								break;
							}
							mOutMove = glm::vec3( 0.0f );
						}
					}
				}
			}
		}
	//	mBlock.updateTransform( mModelTranslation, Quaternion( ), mModelScaleFactor );

		if( !mThirdPerson )
			mController.updateCameraPos( );

	}

	void TestGJK::onRender( )
	{
		glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
		glCall( glDepthRangef( -1, 1 ) );

		// Player Renderer
		if( mThirdPerson )
		{
			for( int i = 0; i < 32; i++ )
				mModelTransforms[ i ] = glm::mat4( 1 );

			mModelTransforms[ 0 ] = mPlayer.getCombinedTransform( );

			mDynamicModelShader->drawDynamicModel( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ), mModelTransforms );
		}
		// Model Static Renderer
		if( mModelRender )
		{
			mStaticModelShader->drawStaticCollider( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
			for( uint32_t i = 0; i < mStaticModelBlocks.size( ); i++ )
				mStaticModelBlocks[ i ].getSphereAdr( )->drawWireSphere( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
		// Collider Static Renderer
		else
		{
			mStaticColliderShader->drawStaticCollider( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
	}

	void TestGJK::onImGuiRender( )
	{
		ImGui::SliderFloat3( "Model Translation", &mModelTranslation.x, -5000.0f, 5000.0f );

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

		if( ImGui::Button( "Print Player Vertices w/ Normals" ) )
			mPlayer.printVerticesWithNormals( );

		if( ImGui::Button( "Print mBlock Normals" ) )
		{
			for( int i = 0; i < mPlayer.getFaceCount( ); i++ )
			{
				std::cout << "\nFace #" << i << "\n";
				Face f = mPlayer.getFaceAtIndex( i );
				for( int i = 0; i < 3; i++ )
					f.getVertexAt( i ).print( );
				f.printNormal( );
			}
		}

		mController.setMoveDirection( mModelMoveDirection );

		if( mPlayer.isGrounded( ) )
			ImGui::Text( "Grounded: YES" );
		else
			ImGui::Text( "Grounded: NO" );

		ImGui::Text( "Application Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO( ).Framerate, ImGui::GetIO( ).Framerate );

		ImGui::Text( "Tri Count in scene: %.3f", ( float ) mTotalTriCount );

		if( ImGui::Button( "Camera Block Control Toggle" ) )
			mCamControl = !mCamControl;

		if( ImGui::Button( "Third / First Person Toggle" ) )
			mThirdPerson = !mThirdPerson;

		if( ImGui::Button( "Collider / Model Toggle" ) )
			mModelRender = !mModelRender;

		if( mGJKIntersect )
			ImGui::Text( "GJK Thinks Player Collided with a Block: YES" );
		else
			ImGui::Text( "GJK Thinks Player Collided with a Block: NO" );

		ImGui::Text( "Number of times gjk had to be called: %.3f", ( float ) mGJKLoopCount );

		ImGui::Text( "Out Translation: ( %.3f, %.3f, %.3f )", mOutMove.x, mOutMove.y, mOutMove.z );

		if( mPlayer.checkGrounded( ) )
			ImGui::Text( "Block is Grounded: TRUE" );
		else
			ImGui::Text( "Block is Grounded: FALSE" );
	}
}