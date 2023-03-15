#pragma once

#include "Test.h"



namespace Test
{
	class TestClearColor : public Test
	{
	private:
		float mClearColor[4];
	public:
		TestClearColor();
		~TestClearColor();

		void onUpdate(float deltaTime) override;
		void onRender() override;
		void onImGuiRender() override;
	};
}