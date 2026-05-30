#include "testmenu.h"
#include "imgui.h"

#include "Renderer.h"
#include <glad/glad.h>
Test::TestMenu::TestMenu(Test*& test): currentTest(test){
}

Test::TestMenu::~TestMenu(){
}

void Test::TestMenu::update(double deltatime){
}

void Test::TestMenu::GuiRender(){
	Renderer::Instance().setClearColor(0.2,0.3,0.5,1);
	Renderer::Instance().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Begin("Test");
	for (auto& test : tests) {
		if(ImGui::Button(test.first.c_str())){
			currentTest = test.second();
		}
	}
	ImGui::End();
}