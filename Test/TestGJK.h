
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
	class TestGJK : public Test
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

		std::vector< GameObject > mObjects;

		std::vector< Block > mStaticModelBlocks;
		int mStaticModelBlocksCount;

		std::vector< Block > mDynamicModelBlocks;
		int mDynamicModelBlocksCount;
		Block mCube;
		BlockCapsule mPlayer;

		BlockController mController;

		Camera mCamera;
		float mCameraSpeed;

		BlockUnitTests mUnitTester;

		int mGJKLoopCount;
		bool mSphereTesting;
		bool mCamControl = false;
		bool mThirdPerson = false;
		bool mModelRender = true;

		std::unique_ptr< Shader > mStaticModelShader;
		std::unique_ptr< Shader > mStaticColliderShader;
		std::unique_ptr< Shader > mDynamicColliderShader;
		std::unique_ptr< Shader > mDynamicModelShader;

		glm::mat4 mModelTransforms[ 32 ];

		BinarySpacePartition* mBSP;

		// Debug Variables
		int mTotalTriCount;
		bool mGJKIntersect = false;
		glm::vec3 mOutMove;

	public:
		TestGJK( );
		~TestGJK( );

		void getInputs( ) override;
		void onUpdate( float deltaTime ) override;
		void onRender( ) override;
		void onImGuiRender( ) override;
	};
}


