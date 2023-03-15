#include "Test.h"


namespace Test
{
	TestMenu::TestMenu(Test*& current) : mCurrentTest(current)
	{
	}

	void TestMenu::onImGuiRender()
	{
		for (auto& test : mTests)
		{
			if(ImGui::Button(test.first.c_str()))
				mCurrentTest = test.second();
		}
	}
	/*
	std::string& TestMenu::getName() const
	{
		return mCurrentTest.
	} */
}