
#include <memory>

#include "Test.h"
#include "Block/PhysicsProperties.h"
#include "Block/BlockController.h"

#include "UnitTests/BlockUnitTests.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"

#include <queue>
#include <unordered_map>
#include <vector>

#include "Math/Quaternion.h"

#include "TimerEvent.h"

namespace Test
{
	class TestProcGeo : public Test
	{
	private:

		float mWindowHeight;
		float mWindowWidth;
		float mWindowDepth;
		float mClearColor[ 4 ];
		glm::mat4 mProj;

		Camera mCamera;
		float mCameraSpeed;

		Block mCube;
		Shader* mWireSphere;

		float mRadius;
		uint32_t mDivisions;
		glm::vec3 mCenter;
		float mColor[ 4 ];
		float mThickness;

		std::vector< WireSphere* > mSpheres;
		std::unique_ptr< Shader > mStaticColliderShader;
		std::vector< GameObject > mColorBlocks;

	public:
		TestProcGeo( );
		~TestProcGeo( );

		void getInputs( ) override;
		void onUpdate( float deltaTime ) override;
		void onRender( ) override;
		void onImGuiRender( ) override;
	};
}


