
#include <memory>

#include "Test.h"
#include "Block/PhysicsProperties.h"
#include "Block/BlockController.h"

#include "Mouse.h"

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
	class TestMouseSelect : public Test
	{
	private:

		float mWindowHeight;
		float mWindowWidth;
		float mWindowDepth;
		float mClearColor[ 4 ];
		float mFOVY;

		Camera mCamera;
		float mCameraSpeed;
		Mouse mMouse;

		std::vector< GameObject > mGameObjects;
		GameObject* mActiveObject;
		bool mEditMode;
		glm::vec3 mEditTranslation;

		float mColor[ 4 ];

		std::vector< WireSphere > mSpheres;
		Shader* mStaticColliderShader;
		std::unique_ptr< Shader > mStaticModelShader;

	public:
		TestMouseSelect( );
		~TestMouseSelect( );

		void getInputs( ) override;
		void onUpdate( float deltaTime ) override;
		void onRender( ) override;
		void onImGuiRender( ) override;
	};
}



