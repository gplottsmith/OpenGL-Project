
#include "TestRotation.h"
#include <iostream>

namespace Test
{

	TestRotation::TestRotation() :
		mShader(std::make_unique<Shader>("res/shaders/Color.shader")),
		mProj(glm::ortho(0.0f, 1536.0f, 0.0f, 1152.0f, -1536.0f, 1536.0f)),
		//mProj(glm::perspective(glm::radians(85.0f), (float) 4 / 3, 0.0001f, 100000.0f)),
		mView(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
		mTranslation1(glm::vec4(400.0f, 600.0f, 0.0f, 1.0f)),
		mTranslation2(glm::vec4(500.0f, 600.0f, 0.0f, 1.0f)),
		mTranslationV(glm::vec4(0.0f, 0.0f, -100.0f, 1.0f)),
		mRotation1(glm::mat4(1.0f)),
		mRotation2(glm::mat4(1.0f)),
		mRotation3(glm::mat4(1.0f)),
		mAlpha(0),
		mBeta(0),
		mTheta(0),
		mRot1(false),
		mRot2(false),
		mRot3(false)
	{
		float positions[8 * 8] = {
			0.0f,   0.0f, 0.0f,   1.0f,    0.0f, 0.0f, 0.0f, 1.0f,  //0
			100.0f, 0.0f, 0.0f,   1.0f,    1.0f, 0.0f, 0.0f, 1.0f,  //1
			100.0f, 100.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,  //2
			0.0f,   100.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,  //3

			0.0f,   0.0f, 0.0f,   1.0f,    0.0f, 0.0f, 0.0f, 1.0f,  //4
			100.0f, 0.0f, 0.0f,   1.0f,    1.0f, 0.0f, 0.0f, 1.0f,  //5
			100.0f, 100.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,  //6
			0.0f,   100.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f   //7
		};

		int indices1[2 * 3] = {
			0, 1, 2,
			2, 3, 0
		};

		int indices2[2 * 3] = {
			4, 5, 6,
			6, 7, 4
		};

		glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		glCall(glEnable(GL_DEPTH_TEST));
		glCall(glEnable(GL_BLEND));

		mVB = std::make_unique<VertexBuffer>(positions, 8 * 8 * sizeof(float));

		mVA = std::make_unique<VertexArray>();

		VertexBufferLayout layout;
		layout.push<float>(4);
		layout.push<float>(4);
		mVA->addBuffer(*mVB, layout);
		mVA->unbind();

		mVB->unbind();

		mIB1 = std::make_unique<IndexBuffer>(indices1, 6);
		mIB2 = std::make_unique<IndexBuffer>(indices2, 6);

		mShader->bind();

		mIB1->unbind();
		mIB2->unbind();
		mShader->unbind();
	}

	TestRotation::~TestRotation()
	{
	}

	void TestRotation::onUpdate(float deltaTime)
	{
	}

	void TestRotation::onRender()
	{
		glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		if (mRot1)
			rotateAboutX(true);
		if (mRot2)
			rotateAboutY(true);
		if (mRot3)
			rotate2D(true);

		mView = glm::translate(glm::mat4(1.0f), mTranslationV);
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), mTranslation1);
			glm::mat4 mvp = mProj * mView * model;
			if (mRot1)
				mvp = mRotation1 * mvp;
			if (mRot2)
				mvp = mRotation2 * mvp;
			if (mRot3)
				mvp = mRotation3 * mvp;
			mShader->bind();
			mShader->setUniformMat4f("uMVP", mvp);

			Renderer::get( ).draw(*mVA, *mIB1, *mShader);
		}
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), mTranslation2);
			glm::mat4 mvp = mProj * mView * model;
			if (mRot1)
				mvp = mRotation1 * mvp;
			if (mRot2)
				mvp = mRotation2 * mvp;
			if (mRot3)
				mvp = mRotation3 * mvp;
			mShader->bind();
			mShader->setUniformMat4f("uMVP", mvp);

			Renderer::get( ).draw(*mVA, *mIB2, *mShader);
		}
	}

	void TestRotation::onImGuiRender()
	{
		ImGui::SliderFloat3("Translation 1", &mTranslation1.x, -1536.0f * 5, 1536.0f);
		ImGui::SliderFloat3("Translation 2", &mTranslation2.x, -1536.0f * 5, 1536.0f);
		ImGui::SliderFloat3("Translation View", &mTranslationV.x, -1536.0f, 1536.0f);
		if (ImGui::Button("Deeper 1"))
			mTranslation1.z += -10.1f;
		if (ImGui::Button("Shallower 1"))
			mTranslation1.z += 10.1f;
		if (ImGui::Button("Deeper 2"))
			mTranslation2.z += -10.1f;
		if (ImGui::Button("Shallower 2"))
			mTranslation2.z += 10.1f;
		if (ImGui::Button("Rot 1"))
			mRot1 = !mRot1;
		if (ImGui::Button("Rot 2"))
			mRot2 = !mRot2;
		if (ImGui::Button("Rot 3"))
			mRot3 = !mRot3;

		ImGui::Text("Application Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	void TestRotation::rotateAboutX(bool positive)
	{
		if (positive)
			mAlpha += 3.141f / 64;
		else
			mAlpha -= 3.141f / 64;
		mRotation1[1][1] = glm::cos(mAlpha);
		mRotation1[1][2] = glm::sin(mAlpha);
		mRotation1[2][1] = -(glm::sin(mAlpha));
		mRotation1[2][2] = glm::cos(mAlpha);
	}

	void TestRotation::rotateAboutY(bool positive)
	{
		if (positive)
			mBeta += 3.141f / 64;
		else
			mBeta -= 3.141f / 64;
		mRotation2[0][0] = glm::cos(mBeta);
		mRotation2[0][2] = -(glm::sin(mBeta));
		mRotation2[2][0] = glm::sin(mBeta);
		mRotation2[2][2] = glm::cos(mBeta);
	}

	void TestRotation::rotate2D(bool positive)
	{
		if (positive)
			mTheta += 3.141f / 64;
		else
			mTheta -= 3.141f / 64;
		mRotation3[0][0] = glm::cos(mTheta);
		mRotation3[0][1] = -(glm::sin(mTheta));
		mRotation3[1][0] = glm::sin(mTheta);
		mRotation3[1][1] = glm::cos(mTheta);
	}
}