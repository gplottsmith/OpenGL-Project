

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
	class ModelViewer : public Test
	{
	private:

		float mWindowHeight;
		float mWindowWidth;
		float mWindowDepth;
		float mClearColor[ 4 ];
		std::unique_ptr <Shader> mShader;
		glm::mat4 mProj;
		glm::vec4 mModelTranslation;
		float mModelScaleFactor;
		glm::vec3 mModelRotationAxis;
		float mModelRotationAngle;	
		Quaternion mModelQuaternion;

		Block* mModels[ 6 ];
		int mModelIndex;

		float mCameraSpeed;
		Camera mCamera;
		BlockController mController;

	public:
		ModelViewer( );
		~ModelViewer( );

		void onRender( ) override;
		void onImGuiRender( ) override;
	};
}