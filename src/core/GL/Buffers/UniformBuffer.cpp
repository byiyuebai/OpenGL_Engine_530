#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(unsigned int size, int gl_type)
{
	glGenBuffers(1, &UBO_ID);

	Bind();

	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, gl_type);
	UnBind();
}

void UniformBuffer::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_ID);
}

void UniformBuffer::UnBind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Clean() const
{
	glDeleteBuffers(1, &UBO_ID);
}

unsigned int UniformBuffer::getBlockIndex(unsigned int shaderID, std::string blockName)
{
	return glGetUniformBlockIndex(shaderID, blockName.c_str());
}

void UniformBuffer::BindRangeToBindingPoint(unsigned int Point, unsigned int offset, unsigned int size)
{
	glBindBufferRange(GL_UNIFORM_BUFFER, Point, UBO_ID, offset, size);
}
void UniformBuffer::BindToBindingPoint(unsigned int Point)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, Point, UBO_ID);
}

void UniformBuffer::ConnectBlockToBindingPoint(GLuint shaderID, GLuint blockIndex, GLuint bindingPoint)
{
	if (blockIndex != GL_INVALID_INDEX) {
		glUniformBlockBinding(shaderID, blockIndex, bindingPoint);
	}
}
void UniformBuffer::ConnectBlockToBindingPoint(GLuint shaderID, std::string blockName, GLuint bindingPoint)
{
	GLuint blockIndex = getBlockIndex(shaderID, blockName);

	if (blockIndex != GL_INVALID_INDEX) {
		glUniformBlockBinding(shaderID, blockIndex, bindingPoint);
	}
}
// 更新 UBO 的部分数据
void UniformBuffer::SetData(GLintptr offset, GLsizeiptr size, const void* data) const
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}