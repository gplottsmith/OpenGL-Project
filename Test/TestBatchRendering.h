
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
#include <unordered_map>

#include "Math/Quaternion.h"

#include "TimerEvent.h"

namespace Test
{
	class TestBatchRendering : public Test
	{
	private:

		float mWindowHeight;
		float mWindowWidth;
		float mWindowDepth;
		float mClearColor[ 4 ];
		glm::mat4 mProj;
		glm::vec4 mModelTranslation;
		float mModelScaleFactor;
		glm::vec3 mModelRotationAxis;
		float mModelRotationAngle;
		Quaternion mModelQuaternion;
		glm::vec3 mModelMoveDirection;
		float mModelMoveAmount;
		glm::vec4 mModelTranslationCube;

		std::vector< GameObject > mColorBlocks; 

		std::vector< Block > mStaticModelBlocks;
		uint32_t mStaticModelBlockCount;

		std::vector< Block > mDynamicModelBlocks;
		int mDynamicModelBlocksCount;

		BlockCapsule mBlock;

		BlockController mController;

		Camera mCamera;
		float mCameraSpeed;

		BlockUnitTests mUnitTester;

		int mTestBlockSearchCount = 0;
		bool mSphereTesting;
		bool mEdgeCase = false;
		bool mCamControl = true;
		bool mThirdPerson = true;
		int mTestPenDepthSearchCount = 0;

		std::unique_ptr< Shader > mStaticColliderShader;
		std::unique_ptr< Shader > mDynamicColliderShader;
		std::unique_ptr< Shader > mStaticModelShader;
		std::unique_ptr< Shader > mDynamicModelShader;

		glm::mat4 mModelTransforms[ 32 ];

		BinarySpacePartition mBSP;

		int mTotalTriCount;

	public:
		TestBatchRendering( );
		~TestBatchRendering( );

		void getInputs( ) override;
		void onUpdate( float deltaTime ) override;
		void onRender( ) override;
		void onImGuiRender( ) override;
	};
}

