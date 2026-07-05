// Shader.cpp
#include "Shader.h"
// 实现文件中必须包含 OpenGL/GLFW 相关头文件（因为要调用具体函数）
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

unsigned int Shader::currentShaderID = 0;
// 构造函数实现
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
	// 你的原有实现代码（加载源码→编译→链接→清理）
	std::string vertexCode = loadShaderFromFile(vertexPath);
	std::string fragmentCode = loadShaderFromFile(fragmentPath);
	std::string geometryCode;
	if (geometryPath != "")geometryCode = loadShaderFromFile(geometryPath);


	if (vertexCode.empty() || fragmentCode.empty()) {
		ID = 0;
		return;
	}
	ID = glCreateProgram();
	unsigned int vertex, fragment, geometry;

	const char* vShaderCode = vertexCode.c_str();
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	const char* fShaderCode = fragmentCode.c_str();
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	if (geometryCode.empty()) {
		geometry = 0;
	}
	else {
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
		
		glAttachShader(ID, geometry);
	}



	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);

	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

// 析构函数实现
Shader::~Shader() {
	glDeleteProgram(ID); // 释放着色器程序资源
}

// use() 函数实现
void Shader::use() const {
	if (currentShaderID == this->ID) {
		return;  // 已经是当前Shader → 不重复绑定 ✅
	}
	if (ID != 0) {

		glUseProgram(ID);
		currentShaderID = ID;
	}
}

void Shader::unuse() const
{
	if (currentShaderID == ID) {
		glUseProgram(0);
		currentShaderID = 0;
	}
}

// getID() 函数实现
unsigned int Shader::getID() const {
	return ID;
}

// -------------------------- Uniform 工具函数实现 --------------------------
void Shader::uniformsetBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::uniformsetInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::uniformsetFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::uniformsetVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::uniformsetVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::uniformsetVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::uniformsetMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::uniformsetMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::uniformsetMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// -------------------------- 私有工具函数实现 --------------------------
std::string Shader::loadShaderFromFile(const std::string& filePath) {
	std::ifstream shaderFile;
	std::stringstream shaderStream;


	shaderFile.open(filePath);
	if (!shaderFile.is_open()) {
		std::cerr << "\033[31m[ERROR]\033[0m [SHADER] FILE_LOAD_FAILED: " << filePath << std::endl;
		return "";
	}
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();



	return shaderStream.str();
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type) {
	int success;
	char infoLog[512];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "\033[31m[ERROR]\033[0m [SHADER] COMPILE_FAILED (" << type << "):\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "\033[31m[ERROR]\033[0m [SHADER] LINK_FAILED (" << type << "):\n" << infoLog << std::endl;
		}
	}
}