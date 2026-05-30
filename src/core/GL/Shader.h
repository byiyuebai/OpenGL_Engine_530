#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	// 构造函数：加载并编译顶点+片段着色器
	Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);

	// 析构函数：释放着色器程序 GPU 资源
	~Shader();

	// 启用着色器程序（核心接口）
	void use() const;

	// 获取着色器程序 ID（对外只读）
	unsigned int getID() const;

	// -------------------------- Uniform 变量设置接口 --------------------------
	void uniformsetBool(const std::string& name, bool value) const;
	void uniformsetInt(const std::string& name, int value) const;
	void uniformsetFloat(const std::string& name, float value) const;
	void uniformsetVec2(const std::string& name, const glm::vec2& value) const;
	void uniformsetVec3(const std::string& name, const glm::vec3& value) const;
	void uniformsetVec4(const std::string& name, const glm::vec4& value) const;
	void uniformsetMat2(const std::string& name, const glm::mat2& mat) const;
	void uniformsetMat3(const std::string& name, const glm::mat3& mat) const;
	void uniformsetMat4(const std::string& name, const glm::mat4& mat) const;

private:
	// 着色器程序 ID（私有，仅内部修改）
	unsigned int ID;

	// 私有工具函数：仅内部使用，不对外暴露
	// 加载着色器文件（从路径读取 GLSL 源码）
	std::string loadShaderFromFile(const std::string& filePath);

	// 私有工具函数：检查着色器编译/程序链接错误
	void checkCompileErrors(unsigned int shader, const std::string& type);

	// 静态成员变量：跟踪当前绑定的着色器程序 ID，优化 use() 函数避免重复绑定
	static unsigned int currentShaderID;
};

