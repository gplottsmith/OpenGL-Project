#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include "Renderer.h"

#include "ImGUI/imgui.h"
#include "glfw3.h"


#include "BinarySpacePartition.h"


namespace Test
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void getInputs( ) {}
		virtual void onUpdate(float deltaTime) {}
		virtual void onRender() {}
		virtual void onImGuiRender() {}
	};

	class TestMenu : public Test
	{
	private:
		Test*& mCurrentTest;
		std::vector<std::pair<std::string, std::function<Test*()>>> mTests;
	public:
		TestMenu(Test*& current);

		void onImGuiRender() override;

		template<typename T>
		void registerTest(const std::string& name)
		{
			std::cout << "Registering test " << name << std::endl;

			mTests.push_back(std::make_pair(name, []() { return new T(); }));
		}
	};
}