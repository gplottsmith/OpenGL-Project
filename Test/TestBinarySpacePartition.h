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
	class TestBinarySpacePartition : public Test
	{
	private:

		float mWindowHeight;
		float mWindowWidth;
		float mWindowDepth;
		float mClearColor[ 4 ];
		glm::mat4 mProj;

		glm::vec4 mCubeTranslation;
		float mCubeScaleFactor;
		glm::vec4 mSphereTranslation;
		float mSphereScaleFactor;
		glm::vec4 mPointTranslation;

		std::vector< GameObject > mObjects;
		BlockCapsule mBlock;
		int mBlocksCount;

		Camera mCamera;
		float mCameraSpeed;

		std::unique_ptr< Shader > mDynamicColliderShader;

		glm::mat4 mModelTransforms[ 32 ];

		BinarySpacePartition* mBSP;

	public:
		TestBinarySpacePartition( );
		~TestBinarySpacePartition( );

		void getInputs( ) override;
		void onUpdate( float deltaTime ) override;
		void onRender( ) override;
		void onImGuiRender( ) override;
	};
}
