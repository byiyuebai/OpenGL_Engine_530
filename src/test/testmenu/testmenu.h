#pragma once
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <tuple>
#include <utility>

namespace Test {
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void update(double deltatime) {}
		virtual void Render() {}
		virtual void GuiRender() {}

		bool getIsBack() const { return isback; }
	private:
		
	protected:
		bool isback = false;
	};

	class TestMenu : public Test {
	public:
		TestMenu(Test*& test);
		~TestMenu();
		void update(double deltatime) override;
		void GuiRender() override;

		template <typename T, typename... Args>
		void RegisterTest(const std::string& name, Args&&... args)
		{
			std::cout << "[Register test]: " << name << std::endl;

			// 简化版：直接捕获参数包，无需手动打包tuple
			auto createTest = [&]() -> Test* {
				// 直接构造T对象，完美转发参数包
				return new T(std::forward<Args>(args)...);
				};

			tests.push_back({ name, createTest }); // 简化pair的构造（C++11列表初始化）
		}


	private:
		Test*& currentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> tests;
	};
}