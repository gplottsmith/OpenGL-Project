
#include "TestProcGeo.h"
#include <iostream>

#define VECTOR_ZERO( a ) a.x == 0 && a.y == 0 && a.z == 0 ? true : false
#define VECTOR_TINY( a ) glm::abs( a.x ) + glm::abs( a.y ) + glm::abs( a.z ) < 0.01f ? true : false

// Started 1/26/2023

namespace Test
{
	TestProcGeo::TestProcGeo( ) :

		mWindowHeight( 1152.0f ),
		mWindowWidth( 1536.0f ),
		mWindowDepth( 15360.0f * 2 ),
		mProj( glm::perspective( 45.0f, mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),
		mClearColor{ 0.2f, 0.3f, 0.9f, 1.0f },

		mCamera( Camera( ) ),
		mCameraSpeed( 15.0f ),
		mCube( "res/DAE Files/SegmentedCube.dae" ),
		mColorBlocks( ),
		
		mWireSphere( new Shader( "res/shaders/Color.shader" ) ),
		mStaticColliderShader( std::make_unique<Shader>( "res/shaders/StaticCollider.shader" ) ),
		mRadius( 10.0f ),
		mDivisions( 1 ),
		mThickness( 10.0f ),
		mCenter( 0 ),
		mColor{ 0.0f, 0.0f, 0.0f, 1.0f },

		mSpheres( )
	{

		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		mCube.updateTransform( glm::vec4( 0, 0, 0, 1 ), Quaternion( glm::vec3( 0, 0, 1 ), 3.141f / 4 ), 100 );
		std::vector< Block > colorBlockFiller;
		colorBlockFiller.emplace_back( mCube );
		mColorBlocks.emplace_back( colorBlockFiller );

		mCube.initBoundarySphereWire( );

		mStaticColliderShader->bind( );
		mStaticColliderShader->initRenderDataStaticCollider( mColorBlocks );
		mStaticColliderShader->unbind( );
	}

	TestProcGeo::~TestProcGeo( )
	{
		delete mWireSphere;
		for( WireSphere* ptr : mSpheres )
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	void TestProcGeo::getInputs( )
	{

	}

	void TestProcGeo::onUpdate( float deltaTime )
	{
		delete mWireSphere;
		mWireSphere = new Shader( "res/shaders/Color.shader" );
		mWireSphere->initRenderDataWireSphere( mRadius, mThickness, mDivisions, glm::vec3( 0 ), glm::vec4( mColor[ 0 ], mColor[ 1 ], mColor[ 2 ], mColor[ 3 ] ) );


	}

	void TestProcGeo::onRender( )
	{
		glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
		glCall( glDepthRangef( -1, 1 ) );

		{
			mWireSphere->drawWireSphere( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) * glm::translate( glm::mat4( 1.0f ), mCenter ), 
				glm::vec4( mColor[ 0 ], mColor[ 1 ], mColor[ 2 ], mColor[ 3 ] ) );
		}
		{
			mStaticColliderShader->drawStaticCollider( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
			mCube.getSphereAdr( )->drawWireSphere( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
		for( uint32_t i = 0; i < mSpheres.size( ); i++ )
		{
			mSpheres[ i ]->draw( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
	}

	void TestProcGeo::onImGuiRender( )
	{
		ImGui::SliderFloat( "Camera Speed", &mCameraSpeed, 0.1f, 75.0f );

		ImGui::SliderFloat( "Sphere Radius", &mRadius, 1.0f, 100.0f );
		if( ImGui::Button( "Incr Divisions" ) )
			mDivisions++;
		if( ImGui::Button( "Decr Divisions" ) )
			mDivisions == 0 ? mDivisions = 0 : mDivisions--;

		ImGui::SliderFloat( "Wire Thickness", &mThickness, 0.1f, 10.0f );
		ImGui::SliderFloat3( "Sphere Position", &mCenter.x, -100.0f, 100.0f );
		ImGui::ColorEdit4( "Clear Color", mColor );

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

		ImGui::Text( "Application Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO( ).Framerate, ImGui::GetIO( ).Framerate );

		ImGui::Text( "Divisions per Sphere Arc: %.1f", ( float )mDivisions );

		if( ImGui::Button( "Copy Sphere" ) )
		{
			mSpheres.emplace_back( new WireSphere( mCenter, mRadius, mThickness, mDivisions, glm::vec4( mColor[ 0 ], mColor[ 1 ], mColor[ 2 ], mColor[ 3 ] ) ) );
		}
		if( ImGui::Button( "Delete Cube's Sphere" ) )
		{
			mCube.deleteBoundarySphereWire( );
		}
		if( ImGui::Button( "Create Cube's Sphere" ) )
		{
			mCube.initBoundarySphereWire( );
		}
	}
}