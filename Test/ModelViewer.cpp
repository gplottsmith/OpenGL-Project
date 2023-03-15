

#include "ModelViewer.h"
#include "ImGUI/imgui.h"

#include <iostream>

namespace Test
{

	ModelViewer::ModelViewer( ) :

		mWindowHeight( 1152.0f ),
		mWindowWidth( 1536.0f ),
		mWindowDepth( 15360.0f ),
		mClearColor{ 0.7f, 0.5f, 0.7f, 1.0f },

		mShader( std::make_unique<Shader>( "res/shaders/Color.shader" ) ),
		mProj( glm::perspective( 45.0f, mWindowWidth / mWindowHeight, 0.1f, mWindowDepth ) ),

		mModelTranslation( 0, 0, -0, 1.0f ),
		mModelScaleFactor( 50.0f ),
		mModelRotationAxis( 0.0f, 1.0f, 0.0f ),
		mModelRotationAngle( 0 ),
		mModelQuaternion( mModelRotationAxis, mModelRotationAngle ),

		mModels( ),
		mModelIndex( 0 ),

		mCameraSpeed( 5.0f ),
		mCamera( Camera( ) )
	{
		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		mShader->bind( );

		//Set Transforms
		
		mModels[ 0 ] = new Block( "res/DAE Files/Spider.dae"  );
		mModels[ 1 ] = new Block( "res/DAE Files/Fortress.dae" );
		mModels[ 2 ] = new Block( "res/DAE Files/Archway.dae" );
		mModels[ 3 ] = new Block( "res/DAE Files/ChessKing.dae" );
		mModels[ 4 ] = new Block( "res/DAE Files/Terrain.dae" );
		mModels[ 5 ] = new Block( "res/DAE Files/Mask.dae" );

		for( int i = 0; i < 6; i++ )
			mModels[ i ]->updateTransform( mModelTranslation, mModelQuaternion, mModelScaleFactor );

		mCamera.setTranslation( glm::vec3( 0, 0, 500.0f ) );
		mController.setCamera( &mCamera );
	}

	ModelViewer::~ModelViewer( )
	{
		for( int i = 0; i < 5; i++ )
		{
			delete mModels[ i ];
			mModels[ i ] = nullptr;
		}
	}

	void ModelViewer::onRender( )
	{
		glCall( glClearColor( mClearColor[ 0 ], mClearColor[ 1 ], mClearColor[ 2 ], mClearColor[ 3 ] ) );

		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
		glCall( glDepthRangef( -1, 1 ) );


		mShader->bind( );

		mShader->setUniformMat4f( "uMVP", mProj *
			glm::inverse( mCamera.getRotationAboutTranslation( ) * mCamera.getTranslation( ) ) * mModels[ mModelIndex ]->getCombinedTransform( ) );
		//Renderer::get( ).drawMultiple( mModels[ mModelIndex ]->getVertexArray( ), mModels[ mModelIndex ]->getIndeces( ), mModels[ mModelIndex ]->getFaceCount( ) * 3, 3, *mShader );

		mShader->unbind( );
	}

	void ModelViewer::onImGuiRender( )
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


		if( ImGui::Button( "Prev" ) )
		{
			mModelIndex--;
			if( mModelIndex < 0 )
				mModelIndex = 5;
		}
		if( ImGui::Button( "Next" ) )
		{
			mModelIndex++;
			if( mModelIndex > 5 )
				mModelIndex = 0;
		}
	}

}