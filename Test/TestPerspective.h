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

#include "Math/Quaternion.h"
#include "FBXFileParser.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

namespace Test
{
	class TestPerspective : public Test
	{
	private:
		std::unique_ptr <VertexArray> mVA;
		std::unique_ptr <VertexBuffer> mVB;
		std::unique_ptr <VertexBuffer> mVB2;
		std::unique_ptr <IndexBuffer> mIB1;
		std::unique_ptr <IndexBuffer> mIB2;
		std::unique_ptr <IndexBuffer> mIB3;
		std::unique_ptr <IndexBuffer> mIB4;
		std::unique_ptr <IndexBuffer> mIB5;
		std::unique_ptr <IndexBuffer> mIB6;
		std::unique_ptr <Shader> mShader;
		glm::vec3 mTranslationC;
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
		Quaternion mQuaternion;
		unsigned int* mIndeces;
		double* mVertices;
		double* mNormals;
		unsigned int mIndexCount;

	public:
		TestPerspective();
		~TestPerspective();

		void onUpdate(float deltaTime) override;
		void onRender() override;
		void onImGuiRender() override;

		void rotateAboutX(bool positive);
		void rotateAboutY(bool positive);
		void rotate2D(bool positive);

		void printMat4(const glm::mat4& mat) const;
		void printVec4(const glm::vec4& vec) const;
		void printFirstVertexTransformations( glm::vec4& vert ) const;
	};
}


