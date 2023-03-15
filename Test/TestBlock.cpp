
#include "TestBlock.h"
#include "ImGUI/imgui.h"

#include <iostream>

namespace Test
{
	TestBlock::TestBlock( ) :

		mDebugCount( 0 ),

		mWindowHeight( 1152.0f ),
		mWindowWidth( 1536.0f ),
		mWindowDepth( 15360.0f ),
		//mProj( glm::ortho( 0.0f, mWindowWidth, 0.0f, mWindowHeight, 0.0f, mWindowDepth ) ),
		mProj( glm::perspective( 45.0f, mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),
		mClearColor{ 0.2f, 0.3f, 0.9f, 1.0f },

		//mBlock( "res/DAE Files/Icosphere.dae" ),
		mBlock( "res/DAE Files/Capsule.dae", 50.0f, 50.0f ),
		mController( &mBlock ),
		mShader( std::make_unique<Shader>( "res/shaders/Color.shader" ) ),
		mShaderTexture( std::make_unique<Shader>( "res/shaders/Texture.shader" ) ),
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

		mColorBlocks( nullptr ),
		mColorBlockCount( 0 ),
		mTexBlocks( nullptr ),
		mTexBlockCount( 0 ),

		mUnitTester( )
	{

		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		mShaderTexture->bind( );
		mShaderTexture->setUniform1i( "uTexture", 0 );

		//Set Transforms
		mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );

		//Set Block's max move speed
		mController.setMaxMoveSpeed( 6 );

		//Camera setup
		mCamera.setTranslation( glm::vec3( 1067, mWindowHeight / 2, 300.0f ) );
		if( mCamControl )
			mController.setCamera( nullptr );
		else
			mController.setCamera( &mCamera );

		mColorBlockCount = 8;
		mColorBlocks = new Block* [ mColorBlockCount ];
		mColorBlocks[ 0 ] = new Block( "res/DAE Files/Cube.dae" );    // TODO 6/23/2022: Fix crash when loading HexStaircasePlus.dae
		mColorBlocks[ 0 ]->updateTransform( mModelTranslationCube + glm::vec4( -300, 0, 0, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );
		mColorBlocks[ 1 ] = new Block( "res/DAE Files/Triangle.dae" );
		mColorBlocks[ 1 ]->updateTransform( mModelTranslationCube + glm::vec4( -200, -300, 0, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 1500.0f );
		mColorBlocks[ 2 ] = new Block( "res/DAE Files/Cube.dae" );
		mColorBlocks[ 2 ]->updateTransform( mModelTranslationCube + glm::vec4( -200, -150, 0, 0 ), Quaternion( glm::vec3( 1, 0, 0 ), 0 ), 10.0f );
		mColorBlocks[ 3 ] = new Block( "res/DAE Files/Cube.dae" );
		mColorBlocks[ 3 ]->updateTransform( mModelTranslationCube + glm::vec4( 800, -300, 0, 0 ), Quaternion( glm::vec3( 1, 1, 0 ), 3.141f / 5.0f ), 500.0f );
		mColorBlocks[ 4 ] = new Block( "res/DAE Files/Terrain.dae" );
		mColorBlocks[ 4 ]->updateTransform( mModelTranslationCube + glm::vec4( 1800, -300, 0, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );
		mColorBlocks[ 5 ] = new Block( "res/DAE Files/Terrain.dae" );
		mColorBlocks[ 5 ]->updateTransform( mModelTranslationCube + glm::vec4( -200, -300, -1000, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );
		mColorBlocks[ 6 ] = new Block( "res/DAE Files/Terrain.dae" );
		mColorBlocks[ 6 ]->updateTransform( mModelTranslationCube + glm::vec4( 800, -300, -1000, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );
		mColorBlocks[ 7 ] = new Block( "res/DAE Files/JankStairs.dae" );
		mColorBlocks[ 7 ]->updateTransform( mModelTranslationCube + glm::vec4( 1800, -300, -1000, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );

		/*
		mTexBlockCount = 2;
		mTexBlocks = new Block* [ mTexBlockCount ];
		mTexBlocks[ 0 ] = new Block( "res/DAE Files/CobbleFloor.dae" );
		mTexBlocks[ 0 ]->updateTransform( mModelTranslationCube, Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );
		mTexBlocks[ 1 ] = new Block( "res/DAE Files/WoodShelter.dae" );
		mTexBlocks[ 1 ]->updateTransform( mModelTranslationCube + glm::vec4( -600, 12, 830, 0 ), Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 30.0f );
		*/
		mTexBlockCount = 1;
		mTexBlocks = new Block * [ mTexBlockCount ];
		mTexBlocks[ 0 ] = new Block( "res/DAE Files/GoyaTri.dae" );
		mTexBlocks[ 0 ]->updateTransform( mModelTranslationCube, Quaternion( glm::vec3( 0, 1, 0 ), 0 ), 100.0f );

		//Setting Block's static
		mBlock.setStatic( false );
		for( int i = 0; i < mColorBlockCount; i++ )
			mColorBlocks[ i ]->setStatic( true );

		for( int i = 0; i < mTexBlockCount; i++ )
			mTexBlocks[ i ]->setStatic( true );

		mController.setCamera( &mCamera );
	}

	TestBlock::~TestBlock( )
	{
		for( int i = 0; i < mColorBlockCount; i++ )
		{
			delete mColorBlocks[ i ];
			mColorBlocks[ i ] = nullptr;
		}

		for( int i = 0; i < mTexBlockCount; i++ )
		{
			delete mTexBlocks[ i ];
			mTexBlocks[ i ] = nullptr;
		}

		delete[ ] mColorBlocks;
		mColorBlocks = nullptr; 
		delete[ ] mTexBlocks;
		mTexBlocks = nullptr;
	}

	void TestBlock::getInputs( )
	{

	}

	void TestBlock::onUpdate( float deltaTime )
	{
		//deltaTime is the number of milliseconds since the last step.
		mModelQuaternion = Quaternion( mModelRotationAxis, mModelRotationAngle );

		glm::vec3* collisionAmount = nullptr;

		glm::vec4 oldTranslation = mModelTranslation;
		mController.updateMoveAmount( deltaTime );
		mController.updateGrounded( );
		mController.updateJumpAmount( deltaTime );
		mBlock.updatePhysics( deltaTime, mBlock.isGrounded( ) );

		glm::vec4 move = glm::vec4( mController.getFrameTranslation( ), 0 );
		mModelTranslation += move;
		mController.setBlockMoveDirection( mController.getFrameTranslation( ) );

		mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );

		mEdgeCase = false;
		if( mBlock.getMoveDirectionPtr( )->x || mBlock.getMoveDirectionPtr( )->y || mBlock.getMoveDirectionPtr( )->z )
		{
			collisionAmount = new glm::vec3( );
			int count = 0;
			mTestBlockSearchCount = 0;

			//Look through all blocks in the level: 1st do sphere check on each block, if check passed then
			//2nd do sphere check on each Face in that block, if check passed then 3rd do gjk check on Face, if passed then
			//4th move block based on gjk return value and repeat loop from begining
			/*
			//Collision Check loop
			while( collisionAmount && count < 100 )
			{
				count++;

				delete collisionAmount;
				collisionAmount = nullptr;

				std::vector< glm::vec3 > collisions;
				std::vector< Face* > collidingFaces;

				for( int i = 0; i < mColorBlockCount + mTexBlockCount; i++ )
				{
					Block* b;
					if( i < mColorBlockCount )
						b = mColorBlocks[ i ];
					else
						b = mTexBlocks[ i - mColorBlockCount ];

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
			*/
		}
		if( mEdgeCase )
		{
			mModelTranslation = oldTranslation;
			mBlock.updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );
		}

		if( !mThirdPerson )
			mController.updateCameraPos( );

	}

	void TestBlock::onRender( )
	{
		glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
		glCall( glDepthRangef( -1, 1 ) );
		/*									TODO /2/2022: Create Renderer data in TestBlock.h
		{
			mShader->bind( );
			if( mThirdPerson )
			{
				mShader->setUniformMat4f( "uMVP", mProj *
					glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) * mBlock.getCombinedTransform( ) );
				Renderer::get( ).drawMultiple( mBlock.getVertexArray( ), mBlock.getIndeces( ), mBlock.getFaceCount( ) * 3, 3, *mShader );
			}
			for( int i = 0; i < mColorBlockCount; i++ )
			{
				mShader->setUniformMat4f( "uMVP", mProj *
					glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) * mColorBlocks[ i ]->getCombinedTransform( ) );
				Renderer::get( ).drawMultiple( mColorBlocks[ i ]->getVertexArray( ), mColorBlocks[ i ]->getIndeces( ), mColorBlocks[ i ]->getFaceCount( ) * 3, 3, *mShader );
			}
			mShader->unbind( );
		}
		
		{

			mShaderTexture->bind( );
			for( int i = 0; i < mTexBlockCount; i++ )
			{
				mShaderTexture->setUniformMat4f( "uMVP", mProj *
					glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) * mTexBlocks[ i ]->getCombinedTransform( ) );
				Renderer::get( ).drawMultiple( *mTexBlocks[ i ], *mShaderTexture );
			}
			mShaderTexture->unbind( );
		} 
		*/
	}

	void TestBlock::onImGuiRender( )
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
				mCamera.setTranslation( mCamera.getTranslationVec3( ) + glm::vec3( 0, 1, 0 ) * mCameraSpeed );
			if( ImGui::IsKeyDown( ImGui::GetKeyIndex( ImGuiKey_Tab ) ) )
				mCamera.setTranslation( mCamera.getTranslationVec3( ) - glm::vec3( 0, 1, 0 ) * mCameraSpeed );

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
		ImGui::Text( "Count of Faces mBlock has searched through: %.3f", ( float )mTestBlockSearchCount );
		ImGui::Text( "Count of PenDepth Searches this Frame: %.3f", ( float ) mTestBlockSearchCount );

		if( ImGui::Button( "Begin Quaternion Testing" ) )
			mUnitTester.testQuaternion( );

		if( ImGui::Button( "Camera Block Control Toggle" ) )
			mCamControl = !mCamControl;

		if( ImGui::Button( "Third / First Person Toggle" ) )
			mThirdPerson = !mThirdPerson;

		if( ImGui::Button( "Start Testing" ) )
			std::cout << "Here";
			
	}
}