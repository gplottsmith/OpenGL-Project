
#include "TestMouseSelect.h"
#include <iostream>

#define VECTOR_ZERO( a ) a.x == 0 && a.y == 0 && a.z == 0 ? true : false
#define VECTOR_TINY( a ) glm::abs( a.x ) + glm::abs( a.y ) + glm::abs( a.z ) < 0.01f ? true : false

// Started 1/26/2023

namespace Test
{
	TestMouseSelect::TestMouseSelect( ) :

		mWindowHeight( 1152.0f ),
		mWindowWidth( 1536.0f ),
		mWindowDepth( 15360.0f * 2 ),
		mFOVY( 60.0f ),
		mClearColor{ 0.2f, 0.3f, 0.9f, 1.0f },

		mCamera( glm::perspective( mFOVY * ( glm::pi< float >( ) / 180.0f ), mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),
		mCameraSpeed( 15.0f ),
		mMouse( ),

		mGameObjects( ),
		mActiveObject( nullptr ),
		mEditMode( false ),
		mEditTranslation( ),

		mStaticColliderShader( new Shader( "res/shaders/StaticCollider.shader" ) ),
		mStaticModelShader( std::make_unique<Shader>( "res/shaders/StaticTexture32.shader" ) ),
		mColor{ 0.0f, 0.0f, 0.0f, 1.0f },

		mSpheres( )
	{
		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		mGameObjects.emplace_back( "res/DAE Files/CobbleFloor.dae", "res/DAE Files/CobbleFloorColliders.dae" );
		mGameObjects[ 0 ].updateTransform( glm::vec4( 0, 0, -100.0f, 1 ), Quaternion( ), 100.0f );

		uint32_t colliderCount = 0;
		for( const GameObject& g : mGameObjects )
			colliderCount += g.getColliders( ).size( );

		mStaticColliderShader->bind( );
		mStaticColliderShader->initRenderDataStaticCollider( mGameObjects );
		mStaticColliderShader->unbind( );

		int width, height;
		GLFWwindow* window = glfwGetCurrentContext( );
		glfwGetWindowSize( window, &width, &height );
		mWindowWidth = ( float ) width;
		mWindowHeight = ( float ) height;

		mSpheres.reserve( 100 );
	}

	TestMouseSelect::~TestMouseSelect( )
	{
		delete mStaticColliderShader;
		mStaticColliderShader = nullptr;
	}

	void TestMouseSelect::getInputs( )
	{

	}

	void TestMouseSelect::onUpdate( float deltaTime )
	{
		mCamera.setFOVY( mFOVY * ( glm::pi< float >( ) / 180.0f ), 0.1f, mWindowDepth );

		GLFWwindow* window = glfwGetCurrentContext( );
		mMouse.update( window );
		int width, height;
		glfwGetWindowSize( window, &width, &height );

		glm::vec2 mopo = mMouse.getPosition( );
		glm::vec3 mouseDir = mCamera.getMouseDirection( mopo, ( float ) width, ( float ) height );
		bool overMenu = false;
		if( mopo.x < -347.0f && mopo.y > 17.0f )
			overMenu = true;

		if( !overMenu && ( mMouse.isLPressed( ) || mMouse.isRPressed( ) ) )
		{
			int index = 0;
			int activeIndex = -1;
			glm::vec3 cubeHit, minHit;
			float distSquared;
			float min = INFINITY;
			for( const GameObject& g : mGameObjects )
			{
				for( const Block& b : g.getColliders( ) )
				{
					for( const Facet& f : b.getFacets( ) )
					{
						if( f.rayIntersects( mouseDir, mCamera.getTranslationVec3( ), &cubeHit, &distSquared ) )
						{
							if( distSquared < min )
							{
								activeIndex = index;
								min = distSquared;
								minHit = cubeHit;
							}
						}
					}
				}
				index++;
			}
			if( min != INFINITY )
			{
				glm::vec4 col;
				mMouse.isLPressed( ) ? col = glm::vec4( 0.1f, 0.9f, 0.3f, 1.0f ) : col = glm::vec4( 0.9f, 0.1f, 0.3f, 1.0f );
				mSpheres.emplace_back( minHit, 2.0f, 0.5f, 6, col );

				mActiveObject = &( mGameObjects[ activeIndex ] );
				mEditMode = true;
				mEditTranslation = mActiveObject->getTranslation( );
			}
			else
			{
				mActiveObject = nullptr;
				mEditMode = false;
			}
		}
		if( mEditMode )
		{ 
			mActiveObject->updateTransform( glm::vec4( mEditTranslation, 1.0f ), mActiveObject->getRotation( ), mActiveObject->getScaleFactor( ) );

			delete mStaticColliderShader;
			mStaticColliderShader = new Shader( "res/shaders/StaticCollider.shader" );
			mStaticColliderShader->bind( );
			mStaticColliderShader->initRenderDataStaticCollider( mGameObjects );
			mStaticColliderShader->unbind( );
		}
		else if( !mStaticColliderShader )
		{
			mStaticColliderShader = new Shader( "res/shaders/StaticCollider.shader" );
			mStaticColliderShader->bind( );
			mStaticColliderShader->initRenderDataStaticCollider( mGameObjects );
			mStaticColliderShader->unbind( );
		}
		//    TODO: Figure out window resizing 1/31/2023
	}

	void TestMouseSelect::onRender( )
	{
		glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
		glCall( glDepthRangef( -1, 1 ) );
		glCall( glEnable( GL_MULTISAMPLE ) );

		{
			//mWireSphere.draw( mProj * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
		{
			mStaticColliderShader->drawStaticCollider( mCamera.getProjection( ) * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
			//mCube.getSphereAdr( )->drawWireSphere( mCamera.getProjection( ) * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
		for( uint32_t i = 0; i < mSpheres.size( ); i++ )
		{
			mSpheres[ i ].draw( mCamera.getProjection( ) * glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) );
		}
	}

	void TestMouseSelect::onImGuiRender( )
	{
		ImGui::SliderFloat( "Camera Speed", &mCameraSpeed, 0.001f, 75.0f );

		ImGui::SliderFloat( "FOVY", &mFOVY, 10.0f, 170.0f );
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

		glm::vec3 camForward = mCamera.getForwardDirection( );
		ImGui::Text( "Camera's Forward Direction: ( %.2f, %.2f, %.2f )", camForward.x, camForward.y, camForward.z );
		glm::vec3 camPos = mCamera.getTranslationVec3( );
		ImGui::Text( "Camera's World Position: ( %.2f, %.2f, %.2f )", camPos.x, camPos.y, camPos.z );

		double mx, my;
		GLFWwindow* window = glfwGetCurrentContext( );
		glfwGetCursorPos( window, &mx, &my );
		ImGui::Text( "Mouse GLFW Position: ( %.2f, %.2f )", ( float ) mx, ( float ) my );
		ImVec2 mousePos = ImGui::GetMousePos( );
		ImGui::Text( "Mouse ImGui Position: ( %.2f, %.2f )", mousePos.x , mousePos.y );
		ImGui::Text( "Mouse Clip Space Position: ( %.2f, %.2f )", mMouse.getX( ) / ( mWindowWidth / 2 ), mMouse.getY( ) / ( mWindowHeight / 2 ) );
		ImGui::Text( "Number of Spheres: %.0f", ( float )mSpheres.size( ) );

		if( mEditMode )
		{
			ImGui::Text( "Active Game Object has %.0f Colliders", ( float )mActiveObject->getColliders( ).size( ) );
			ImGui::SliderFloat3( "Object Translation", &mEditTranslation.x, -5000.0f, 5000.0f );
		}
		else
		{
			ImGui::Text( "No Active Game Object" );
		}
	}
}