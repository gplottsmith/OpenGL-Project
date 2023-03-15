
#include "TestCube.h"
#include <iostream>

namespace Test
{

	TestCube::TestCube(  ) :
		mShader( std::make_unique<Shader>( "res/shaders/QuaternionRotation.shader" ) ),
		mProj( glm::ortho( 0.0f, 1536.0f, 0.0f, 1152.0f, 0.0f, 1536.0f ) ),
		//mProj( glm::perspective( glm::radians( 85.0f ), ( float ) 4 / 3, 0.0001f, 100000.0f ) ),
		mView( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ) ) ),
		mTranslationC( glm::vec4( 1536.0f / 2, 1152.0f / 2, 0, 1.0f ) ),
		mTranslationV( glm::vec4( 0.0f, 0.0f, -100.0f, 1.0f ) ),
		mRotation1( glm::mat4( 1.0f ) ),
		mRotation2( glm::mat4( 1.0f ) ),
		mRotation3( glm::mat4( 1.0f ) ),
		mAlpha( 0 ),
		mBeta( 0 ),
		mTheta( 0 ),
		mRot1( false ),
		mRot2( false ),
		mRot3( false ),
		mQuaternion( 0, 0, 0, 1 )
	{
		/*
		float positions[8 * 8] = {
			0.0f,   0.0f,   0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,  //0
			100.0f, 0.0f,   0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,  //1
			100.0f, 100.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,  //2
			0.0f,   100.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,  //3

			0.0f,   0.0f,   -100.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,  //4
			100.0f, 0.0f,   -100.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,  //5
			100.0f, 100.0f, -100.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,  //6
			0.0f,   100.0f, -100.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f   //7
		};
		*/
		double positions[8 * 8] = {
			0.0,   0.0,   0.0, 1.0,    0.0, 0.0, 0.0, 1.0,  //0
			100.0, 0.0,   0.0, 1.0,    1.0, 0.0, 0.0, 1.0,  //1
			100.0, 100.0, 0.0, 1.0,    1.0, 1.0, 0.0, 1.0,  //2
			0.0,   100.0, 0.0, 1.0,    0.0, 1.0, 0.0, 1.0,  //3

			0.0,   0.0,   -100.0, 1.0,    0.0, 0.0, 0.0, 1.0,  //4
			100.0, 0.0,   -100.0, 1.0,    1.0, 0.0, 0.0, 1.0,  //5
			100.0, 100.0, -100.0, 1.0,    1.0, 1.0, 0.0, 1.0,  //6
			0.0f,   100.0, -100.0, 1.0,    0.0, 1.0, 0.0, 1.0   //7
		};

		unsigned int indices1[2 * 3] = {
			0, 1, 2,
			2, 3, 0
		};

		unsigned int indices2[2 * 3] = {
			4, 5, 6,
			6, 7, 4
		};

		unsigned int indices3[2 * 3] = {
			3, 2, 6,
			6, 7, 3
		};

		unsigned int indices4[2 * 3] = { 
			2, 1, 5,
			5, 6, 2
		};

		unsigned int indices5[2 * 3] = {
			0, 1, 5,
			5, 4, 0
		};

		unsigned int indices6[2 * 3] = {
			3, 0, 4,
			4, 7, 3
		};

		glCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
		glCall( glEnable( GL_DEPTH_TEST ) );
		glCall( glEnable( GL_BLEND ) );

		mVB = std::make_unique<VertexBuffer>( positions, 8 * 8 * sizeof( double ) );

		mVA = std::make_unique<VertexArray>(  );

		VertexBufferLayout layout;
		layout.push<double>( 4 );
		layout.push<double>( 4 );
		mVA->addBuffer( *mVB, layout );
		mVA->unbind(  );

		mVB->unbind(  );

		mIB1 = std::make_unique<IndexBuffer>( indices1, 6 );
		mIB2 = std::make_unique<IndexBuffer>( indices2, 6 );
		mIB3 = std::make_unique<IndexBuffer>( indices3, 6 );
		mIB4 = std::make_unique<IndexBuffer>( indices4, 6 );
		mIB5 = std::make_unique<IndexBuffer>( indices5, 6 );
		mIB6 = std::make_unique<IndexBuffer>( indices6, 6 );
	}

	TestCube::~TestCube(  )
	{
	}

	void TestCube::onUpdate( float deltaTime )
	{
	}

	void TestCube::onRender(  )
	{
		glCall( glClearColor( 0.1f, 0.1f, 0.1f, 1.0f ) );
		glCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );

		if ( mRot1 )
			rotateAboutX( true );
		if ( mRot2 )
			rotateAboutY( true );
		if ( mRot3 )
			rotate2D( true );

		mView = glm::translate( glm::mat4( 1.0f ), mTranslationV );
		glm::mat4 model = glm::translate( glm::mat4( 1.0f ), mTranslationC );
		model = model * mQuaternion.toMat4(  );
		glm::mat4 mvp = mProj * mView * model;
		if ( mRot1 )
			mvp = mRotation1 * mvp;
		if ( mRot2 )
			mvp = mRotation2 * mvp;
		if ( mRot3 )
			mvp = mRotation3 * mvp;
		mShader->bind(  ); 
		
		mShader->setUniformMat4f( "uMVP", mvp );

		{
			Renderer::get( ).draw( *mVA, *mIB1, *mShader );
			Renderer::get( ).draw( *mVA, *mIB2, *mShader );
			Renderer::get( ).draw( *mVA, *mIB3, *mShader );
			Renderer::get( ).draw( *mVA, *mIB4, *mShader );
			Renderer::get( ).draw( *mVA, *mIB5, *mShader );
			Renderer::get( ).draw( *mVA, *mIB6, *mShader );
		}
	}

	void TestCube::onImGuiRender(  )
	{
		ImGui::SliderFloat3( "Translation 1", &mTranslationC.x, 0, 1536.0f );
		ImGui::SliderFloat3( "Translation View", &mTranslationV.x, 0, 1536.0f );
		if ( ImGui::Button( "Deeper 1" ) )
			mTranslationC.z += -10.1f;
		if ( ImGui::Button( "Shallower 1" ) )
			mTranslationC.z += 10.1f;
		if ( ImGui::Button( "Deeper 2" ) )
			mTranslationC.z += -10.1f;
		if ( ImGui::Button( "Shallower 2" ) )
			mTranslationC.z += 10.1f;
		if ( ImGui::Button( "Rot 1" ) )
			mRot1 = !mRot1;
		if ( ImGui::Button( "Rot 2" ) )
			mRot2 = !mRot2;
		if ( ImGui::Button( "Rot 3" ) )
			mRot3 = !mRot3;
		if ( ImGui::Button( "Incr W: 0.1" ) )
		{
			mQuaternion.dW( 0.1f );
			mQuaternion.print(  );
		}
		if ( ImGui::Button( "Incr X: 0.1" ) )
		{
			mQuaternion.dX( 0.1f );
			mQuaternion.print(  );
		}
		if ( ImGui::Button( "Incr Y: 0.1" ) )
		{
			mQuaternion.dY( 0.1f );
			mQuaternion.print(  );
		}
		if ( ImGui::Button( "Incr Z: 0.1" ) )
		{
			mQuaternion.dZ( 0.1f );
			mQuaternion.print(  );
		}

		ImGui::Text( "Application Avg %.3f ms/frame ( %.1f FPS )", 1000.0f / ImGui::GetIO(  ).Framerate, ImGui::GetIO(  ).Framerate );
	}

	void TestCube::rotateAboutX( bool positive )
	{
		if ( positive )
			mAlpha += 3.141f / 64;
		else
			mAlpha -= 3.141f / 64;
		mRotation1[1][1] = glm::cos( mAlpha );
		mRotation1[1][2] = glm::sin( mAlpha );
		mRotation1[2][1] = -( glm::sin( mAlpha ) );
		mRotation1[2][2] = glm::cos( mAlpha );
	}

	void TestCube::rotateAboutY( bool positive )
	{
		if ( positive )
			mBeta += 3.141f / 64;
		else
			mBeta -= 3.141f / 64;
		mRotation2[0][0] = glm::cos( mBeta );
		mRotation2[0][2] = -( glm::sin( mBeta ) );
		mRotation2[2][0] = glm::sin( mBeta );
		mRotation2[2][2] = glm::cos( mBeta );
	}

	void TestCube::rotate2D( bool positive )
	{
		if ( positive )
			mTheta += 3.141f / 64;
		else
			mTheta -= 3.141f / 64;
		mRotation3[0][0] = glm::cos( mTheta );
		mRotation3[0][1] = -( glm::sin( mTheta ) );
		mRotation3[1][0] = glm::sin( mTheta );
		mRotation3[1][1] = glm::cos( mTheta );
	}
	
	void TestCube::printMat( glm::mat4& mat ) const
	{
		for ( int i = 0; i < 4; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				std::cout << mat[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}
