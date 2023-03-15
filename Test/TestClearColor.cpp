
#include "TestClearColor.h"
#include "ImGUI/imgui.h"

namespace Test
{

	TestClearColor::TestClearColor() : mClearColor{0.2f, 0.3f, 0.9f, 1.0f}
	{
	}

	TestClearColor::~TestClearColor()
	{
	}

	void TestClearColor::onUpdate(float deltaTime)
	{
	}

	void TestClearColor::onRender()
	{
		glCall(glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]));
		glCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestClearColor::onImGuiRender()
	{
		ImGui::ColorEdit4("Clear Color", mClearColor);
	}

}