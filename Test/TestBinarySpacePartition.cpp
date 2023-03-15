

#include "TestBinarySpacePartition.h"



Test::TestBinarySpacePartition::TestBinarySpacePartition( ) :
	mWindowHeight( 1152.0f ),
	mWindowWidth( 1536.0f ),
	mWindowDepth( 15360.0f * 2 ),
	mProj( glm::perspective( 45.0f, mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),
	mClearColor{ 0.2f, 0.3f, 0.9f, 1.0f },

	mCubeTranslation( -10, 0, 0, 1 ),
	mCubeScaleFactor( 100 ),
	mSphereTranslation( 500, 500, 500, 1 ),
    mSphereScaleFactor( 100 ),
	mPointTranslation( -11, -11, -11, 1 ),
	
	mObjects( ),
	mBlock( "res/DAE Files/Capsule.dae", 50.0f, 50.0f ),
	mBlocksCount( 0 ),

	mCamera( Camera( ) ),
	mCameraSpeed( 15.0f ),

	mDynamicColliderShader( std::make_unique<Shader>( "res/shaders/DynamicCollider.shader" ) ),
	mModelTransforms( ),

	mBSP( nullptr )

{
	glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
	glCall( glEnable( GL_DEPTH_TEST ) );
	glCall( glEnable( GL_BLEND ) );

	//Camera setup
	mCamera.setTranslation( glm::vec3( 0, 0, 500.0f ) );

	mBlocksCount = 3;
	mObjects.reserve( mBlocksCount );
	mObjects.emplace_back( "res/DAE Files/Cube.dae" );
	mObjects.emplace_back( "res/DAE Files/Cube.dae" );
	mObjects.emplace_back( "res/DAE Files/Cube.dae" );

	for( int i = 0; i < 32; i++ )
	{
		if( i < mBlocksCount )
			mModelTransforms[ i ] = mObjects[ i ].getTransform( );

		else
			mModelTransforms[ i ] = glm::mat4( 1 );
	}

	mDynamicColliderShader->bind( );
	mDynamicColliderShader->initRenderDataDynamicCollider( mObjects, mModelTransforms );
	mDynamicColliderShader->unbind( );

	mObjects[ 0 ].updateTransform( mPointTranslation, Quaternion( ), 10 );
	mObjects[ 1 ].updateTransform( mCubeTranslation, Quaternion( ), mCubeScaleFactor );
	mObjects[ 2 ].updateTransform( mSphereTranslation, Quaternion( ), mSphereScaleFactor );

	std::vector< const Block* > vec;
	vec.reserve( mBlocksCount );
	vec.push_back( mObjects[ 0 ].getColliderAddressAt( 0 ) );
	vec.push_back( mObjects[ 1 ].getColliderAddressAt( 0 ) );
	vec.push_back( mObjects[ 2 ].getColliderAddressAt( 0 ) );

	mBSP = new BinarySpacePartition( 1000.0f, 2, vec, mBlock );
}

Test::TestBinarySpacePartition::~TestBinarySpacePartition( )
{
}

void Test::TestBinarySpacePartition::getInputs( )
{
}

void Test::TestBinarySpacePartition::onUpdate( float deltaTime )
{
	// Update Model Transforms
	mObjects[ 0 ].updateTransform( mPointTranslation, Quaternion( ), 10 );
	mObjects[ 1 ].updateTransform( mCubeTranslation, Quaternion( ), mCubeScaleFactor );
	mObjects[ 2 ].updateTransform( mSphereTranslation, Quaternion( ), mSphereScaleFactor );

}

void Test::TestBinarySpacePartition::onRender( )
{
	glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

	glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
	glCall( glDepthRangef( -1, 1 ) );

	{
		for( int i = 0; i < mBlocksCount; i++ )
		{
			mModelTransforms[ i ] = mObjects[ i ].getTransform( );
		}

		mDynamicColliderShader->drawDynamicCollider( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ), mModelTransforms );
	}
}

void Test::TestBinarySpacePartition::onImGuiRender( )
{
	// Camera Movement
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

	// Model Transforms
	//ImGui::SliderFloat3( "Cube Position", &mCubeTranslation.x, -1000.0f, 1000.0f );
	//ImGui::SliderFloat3( "Sphere Position", &mSphereTranslation.x, -1000.0f, 1000.0f );

	//ImGui::SliderFloat( "Cube Scale", &mCubeScaleFactor, 0.1f, 1000.0f );
	//ImGui::SliderFloat( "Sphere Scale", &mSphereScaleFactor, 0.1f, 1000.0f );

	ImGui::SliderFloat3( "Point Position", &mPointTranslation.x, -1000.0f, 1000.0f );

	if( ImGui::Button( "Print Cube Model Corners" ) )
		mObjects[ 0 ].getColliders( )[ 0 ].printVertices( );

	if( ImGui::Button( "Print Sphere Data" ) )
	{
		std::cout << "\nSphere Radius: " << mObjects[ 1 ].getColliders( )[ 0 ].getSphereAdr( )->getRadius( ) << 
			". Center: ( " << mObjects[ 1 ].getColliders( )[ 0 ].getSphereAdr( )->getCenter( ).x << ", "
			<< mObjects[ 1 ].getColliders( )[ 0 ].getSphereAdr( )->getCenter( ).y << ", " << mObjects[ 1 ].getColliders( )[ 0 ].getSphereAdr( )->getCenter( ).z << " )" << std::endl;
	}

	std::vector< const Block* > vec = mBSP->getBlocksInLeaf( mObjects[ 0 ].getTranslation( ) );
	ImGui::Text( "Number of Blocks in Cube's area: %d", vec.size( ) );

	ImGui::Text( "Application Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO( ).Framerate, ImGui::GetIO( ).Framerate );

}
