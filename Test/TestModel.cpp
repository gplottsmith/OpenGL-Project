
#include "TestModel.h"
#include <iostream>

namespace Test
{

	TestModel::TestModel() :
		mShader(std::make_unique<Shader>("res/shaders/QuaternionRotation.shader")),
		mProj(glm::ortho(0.0f, 1536.0f, 0.0f, 1152.0f, 0.0f, 1536.0f)),
		//mProj( glm::perspective(  glm::radians(45.0f), 4.0f / 3.0f, 1.0f, 10.0f ) ),
		mView(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
		mTranslationC(glm::vec4(1536.0f / 2, 1152.0f / 2, 0.0f, 1.0f)),
		mTranslationV(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		mRotation1(glm::mat4(1.0f)),
		mRotation2(glm::mat4(1.0f)),
		mRotation3(glm::mat4(1.0f)),
		mAlpha(0),
		mBeta(0),
		mTheta(0),
		mRot1(false),
		mRot2(false),
		mRot3(false),
		mQuaternion(0, 0, 0, 1),
		mIndeces(nullptr),
		mVertices(nullptr),
		mIndexCount(0)
	{
		FBXFileParser fp("res/FBX Files/skullPlain.fbx");

		mShader->bind();

		fp.initVertexIndexNormalsArrays(&mVertices, &mIndeces, &mNormals);
		mIndexCount = fp.getIndexCount();
		unsigned int vertexComponentCount = fp.getVertexComponentCount();

		unsigned int positionsSize = (vertexComponentCount / 3) * 8;
		float* positions = new float[positionsSize];

		std::cout << std::endl;
		unsigned int i = 0, vertexIndex = 0;
		while (i < positionsSize)
		{
			positions[i++] = (float)mVertices[vertexIndex++];
			positions[i++] = (float)mVertices[vertexIndex++];
			positions[i++] = (float)mVertices[vertexIndex++];
			positions[i++] = 1.0f;
			positions[i++] = (std::rand() % 256) / 255.0f;
			positions[i++] = (std::rand() % 256) / 255.0f;
			positions[i++] = (std::rand() % 256) / 255.0f;
			positions[i++] = 1.0f;

		}

		mVB = std::make_unique<VertexBuffer>(positions, positionsSize * sizeof(float));

		VertexBufferLayout layout;
		layout.push<float>(4);
		layout.push<float>(4);

		glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		glCall(glEnable(GL_DEPTH_TEST));
		glCall(glEnable(GL_BLEND));

		mVA = std::make_unique<VertexArray>();
		mVA->addBuffer(*mVB, layout);
		mVA->unbind();

		mVB->unbind();

		mView = glm::translate(glm::mat4(1.0f), mTranslationV);
		glm::mat4 model = glm::translate(glm::mat4(1.0f), mTranslationC);
		model = model * mQuaternion.toMat4();
		model = model * glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 1));
		glm::mat4 mvp = mProj * mView * model;

		glm::vec4 vert(positions[0], positions[1], positions[2], positions[3]);
		vert = model * vert;
		std::cout << vert.x << ", " << vert.y << ", " << vert.z << ", " << vert.w << std::endl;
		
		vert = mView * vert;
		std::cout << vert.x << ", " << vert.y << ", " << vert.z << ", " << vert.w << std::endl;

		vert = mProj * vert;
		std::cout << vert.x << ", " << vert.y << ", " << vert.z << ", " << vert.w << std::endl << std::endl;
		
		for (unsigned int i = 0; i < positionsSize; i += 8)
		{
			glm::vec4 vert(positions[i], positions[i + 1], positions[i + 2], positions[i + 3]);
			vert = mvp * vert;
			std::cout << vert.x << ", " << vert.y << ", " << vert.z << ", " << vert.w << std::endl;
		}
		std::cout << "\nProj: \n";
		printMat4(mProj);
		std::cout << "\nMVP: \n";
		printMat4(mvp);

		delete[ ] positions;
		positions = nullptr;
	}

	TestModel::~TestModel()
	{
		delete[] mIndeces;
		mIndeces = nullptr;
	}

	void TestModel::onUpdate(float deltaTime)
	{
	}

	void TestModel::onRender()
	{
		glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDepthRangef(-1, 1));
		

		if (mRot1)
			rotateAboutX(true);
		if (mRot2)
			rotateAboutY(true);
		if (mRot3)
			rotate2D(true);

		mView = glm::translate(glm::mat4(1.0f), mTranslationV);
		glm::mat4 model = glm::translate(glm::mat4(1.0f), mTranslationC);
		model = model * mQuaternion.toMat4();
		model = model * glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100));
		glm::mat4 mvp = mProj * mView * model;
		if (mRot1)
			mvp = mRotation1 * mvp;
		if (mRot2)
			mvp = mRotation2 * mvp;
		if (mRot3)
			mvp = mRotation3 * mvp;
		mShader->bind();

		mShader->setUniformMat4f("uMVP", mvp);

		{
			Renderer::get( ).drawMultiple(*mVA, mIndeces, mIndexCount, 3, *mShader);
		}
	}

	void TestModel::onImGuiRender()
	{
		ImGui::SliderFloat3("Translation 1", &mTranslationC.x, -1536.0f, 1536.0f);
		ImGui::SliderFloat3("Translation View", &mTranslationV.x, -1536.0f, 1536.0f);
		if (ImGui::Button("Deeper 1"))
			mTranslationC.z += -10.1f;
		if (ImGui::Button("Shallower 1"))
			mTranslationC.z += 10.1f;
		if (ImGui::Button("Deeper 2"))
			mTranslationC.z += -10.1f;
		if (ImGui::Button("Shallower 2"))
			mTranslationC.z += 10.1f;
		if (ImGui::Button("Rot 1"))
			mRot1 = !mRot1;
		if (ImGui::Button("Rot 2"))
			mRot2 = !mRot2;
		if (ImGui::Button("Rot 3"))
			mRot3 = !mRot3;
		if (ImGui::Button("Incr W: 0.1"))
		{
			mQuaternion.dW(0.1f);
			mQuaternion.print();
		}
		if (ImGui::Button("Incr X: 0.1"))
		{
			mQuaternion.dX(0.1f);
			mQuaternion.print();
		}
		if (ImGui::Button("Incr Y: 0.1"))
		{
			mQuaternion.dY(0.1f);
			mQuaternion.print();
		}
		if (ImGui::Button("Incr Z: 0.1"))
		{
			mQuaternion.dZ(0.1f);
			mQuaternion.print();
		}

		ImGui::Text("Application Avg %.3f ms/frame ( %.1f FPS )", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	void TestModel::rotateAboutX(bool positive)
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

	void TestModel::rotateAboutY(bool positive)
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

	void TestModel::rotate2D(bool positive)
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

	void TestModel::printMat4( const glm::mat4& mat ) const
	{
		std::cout << std::endl;
		for (int i = 0; i < 4; i++)
		{
			std::cout << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << ", " << mat[i][3] << std::endl;
		}
		std::cout << std::endl;
	}

	void TestModel::printVec4(const glm::vec4& vec) const
	{
		std::cout << std::endl << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3] << std::endl;
	}
}
