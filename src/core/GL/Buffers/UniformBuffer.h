#pragma once
#include <glad/glad.h>
#include <string>
class UniformBuffer {
public:
	UniformBuffer(unsigned int size, int gl_type = GL_STATIC_DRAW);
	~UniformBuffer();

	void Bind() const;
	void UnBind() const;

	void Clean() const;

	unsigned int getID() const { return UBO_ID; }


	// 将 UBO 的一部分绑定到绑定点
	void BindRangeToBindingPoint(unsigned int blockIndex, unsigned int offset, unsigned int size);
	// 将整个 UBO 绑定到绑定点
	void BindToBindingPoint(unsigned int blockIndex);

	// 获取着色器中 Uniform Block 的索引
	unsigned int getBlockIndex(unsigned int shaderID,std::string blockName);
	// 将着色器的 Block 连接到绑定点
	void ConnectBlockToBindingPoint(GLuint shaderID, GLuint blockIndex, GLuint bindingPoint);
	void ConnectBlockToBindingPoint(GLuint shaderID, std::string blockName, GLuint bindingPoint);

	void SetData(GLintptr offset, GLsizeiptr size, const void* data) const;
private:
	unsigned int UBO_ID;
};