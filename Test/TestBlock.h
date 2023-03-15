
#include <memory>

#include "Test.h"
#include "Block/PhysicsProperties.h"
#include "Block/BlockController.h"

#include "UnitTests/BlockUnitTests.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include <queue>

#include "Math/Quaternion.h"

#include "TimerEvent.h"

namespace Test
{
	class TestBlock : public Test
	{
	private:
		int mDebugCount;

		float mWindowHeight;
		float mWindowWidth;
		float mWindowDepth;
		float mClearColor[ 4 ];
		std::unique_ptr <Shader> mShader;
		std::unique_ptr <Shader> mShaderTexture;
		glm::mat4 mProj;
		glm::vec4 mModelTranslation;
		float mModelScaleFactor;
		glm::vec3 mModelRotationAxis;
		float mModelRotationAngle;
		Quaternion mModelQuaternion;
		glm::vec3 mModelMoveDirection;
		float mModelMoveAmount;
		glm::vec4 mModelTranslationCube;

		Block** mColorBlocks;
		int mColorBlockCount;

		Block** mTexBlocks;
		int mTexBlockCount;
		BlockCapsule mBlock;

		BlockController mController;

		Camera mCamera;
		float mCameraSpeed;

		BlockUnitTests mUnitTester;

		int mTestBlockSearchCount = 0;
		bool mEdgeCase = false;
		bool mCamControl = true;
		bool mThirdPerson = true;
		int mTestPenDepthSearchCount = 0;

	public:
		TestBlock( );
		~TestBlock( );

		void getInputs( ) override;
		void onUpdate( float deltaTime ) override;
		void onRender( ) override;
		void onImGuiRender( ) override;

	};
}
