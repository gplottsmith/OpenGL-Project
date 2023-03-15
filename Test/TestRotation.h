 #pragma once

#include "Test.h"
#include "Renderer.h"
#include "ImGUI/imgui.h"
#include "glfw3.h"
#include "glew.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

namespace Test
{
	class TestRotation : public Test
	{
	private:
		std::unique_ptr<VertexArray> mVA;
		std::unique_ptr <VertexBuffer> mVB;
		std::unique_ptr <VertexBuffer> mVB2;
		std::unique_ptr <IndexBuffer> mIB1;
		std::unique_ptr <IndexBuffer> mIB2;
		std::unique_ptr <Shader> mShader;
		glm::vec3 mTranslation1;
		glm::vec3 mTranslation2;
		glm::vec3 mTranslationV;
		glm::mat4 mRotation1;
		glm::mat4 mRotation2;
		glm::mat4 mRotation3;
		glm::mat4 mProj;
		glm::mat4 mView;
		float mAlpha;
		float mBeta;
		float mTheta;
		bool mRot1; 
		bool mRot2;
		bool mRot3;

	public:
		TestRotation();
		~TestRotation();

		void onUpdate(float deltaTime) override;
		void onRender() override;
		void onImGuiRender() override;

		void rotateAboutX(bool positive);
		void rotateAboutY(bool positive);
		void rotate2D(bool positive);
	};
}