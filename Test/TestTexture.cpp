
#include "TestTexture.h"
#include <iostream>

namespace Test
{

	TestTexture::TestTexture() : 
		mShader(std::make_unique<Shader>("res/shaders/Texture.shader")),
		mTexture1(std::make_unique<Texture>("res/Textures/sprPlayer.png")),
		mTexture2(std::make_unique<Texture>("res/Textures/sprEnemy.png")),
		mProj(glm::ortho(0.0f, 1536.0f, 0.0f, 1152.0f, -1.0f, 1.0f)),
		mView(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
		mTranslation1(glm::vec4(500.0f, 400.0f, 0.0f, 1.0f)),
		mTranslation2(glm::vec4(400.0f, 400.0f, 0.0f, 1.0f))
	{

		float positions[8 * 6] = {
			0.0f,   0.0f,   0.0f, 1.0f,  0.0f, 0.0f,  //0
			100.0f, 0.0f,   0.0f, 1.0f,  1.0f, 0.0f,  //1
	    	100.0f, 150.0f, 0.0f, 1.0f,  1.0f, 1.0f,  //2
		    0.0f,   150.0f, 0.0f, 1.0f,  0.0f, 1.0f,  //3

			0.0f,   0.0f,   0.0f, 1.0f,  0.0f, 0.0f,  //4
			100.0f, 0.0f,   0.0f, 1.0f,  1.0f, 0.0f,  //5
			100.0f, 150.0f, 0.0f, 1.0f,  1.0f, 1.0f,  //6
			0.0f,   150.0f, 0.0f, 1.0f,  0.0f, 1.0f   //7
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
		glCall(glEnable(GL_BLEND));

		mVB = std::make_unique<VertexBuffer>(positions, 8 * 6 * sizeof(float));

		mVA = std::make_unique<VertexArray>();

		VertexBufferLayout layout;
		layout.push<float>(4);
		layout.push<float>(2);
		mVA->addBuffer(*mVB, layout);
		mVA->unbind();

		mVB->unbind();

		mIB1 = std::make_unique<IndexBuffer>(indices1, 6);
		mIB2 = std::make_unique<IndexBuffer>(indices2, 6);

		mShader->bind();

		mTexture1->bind();
		mShader->setUniform1i("uTexture", 0);

		mIB1->unbind();
		mIB2->unbind();
		mShader->unbind();
	}

	TestTexture::~TestTexture()
	{
	}

	void TestTexture::onUpdate(float deltaTime)
	{
	}

	void TestTexture::onRender()
	{
		glCall(glClearColor(0.4f, 0.4f, 0.4f, 1.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT));
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), mTranslation1);
			glm::mat4 mvp = mProj * mView * model;
			mShader->bind();
			mShader->setUniformMat4f("uMVP", mvp);

			Renderer::get( ).draw(*mVA, *mIB1, *mShader, *mTexture1);
		}
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), mTranslation2);
			glm::mat4 mvp = mProj * mView * model;
			mShader->bind();
			mShader->setUniformMat4f("uMVP", mvp);

			Renderer::get( ).draw(*mVA, *mIB2, *mShader, *mTexture2);
		} 
	}

	void TestTexture::onImGuiRender()
	{
		ImGui::SliderFloat3("Translation 1", &mTranslation1.x, 0.0f, 1536.0f);
		ImGui::SliderFloat3("Translation 2", &mTranslation2.x, 0.0f, 1536.0f);
		if (ImGui::Button("Increment Bottom Left X"))
			incrementVertex(2, -0.1f);

		ImGui::Text("Application Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	void TestTexture::incrementVertex(unsigned int index, float amount) const
	{
		float val;
		mVB->bind();
		glCall(glGetBufferSubData(GL_ARRAY_BUFFER, index * sizeof(float), sizeof(float), &val));
		val += amount;
		glCall(glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(float), sizeof(float), &val));
	}
}