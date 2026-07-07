#include "VertexBuffer.h"
VertexBuffer::VertexBuffer(const void* data, unsigned int size, int gl_type) {
	glBindVertexArray(0);//确保没有VAO绑定

	glGenBuffers(1, &VBO_ID);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_type);
	UnBind();
	max_size = size;
}

VertexBuffer::~VertexBuffer() {
	Clean();
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
}

void VertexBuffer::UnBind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Clean() const
{
	glDeleteBuffers(1, &VBO_ID);
}

void VertexBuffer::UpdateData(const void* data, int offset, unsigned int size)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::ResetData(const void* data, unsigned int size, int gl_type)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_type);
	max_size = size;
}

void VertexBuffer::Expand(unsigned int expand_size, int gl_type)
{
	if (!expand_size) return;

	unsigned int new_size = max_size + expand_size;

	// 新建更大VBO
	GLuint new_VBO;
	glGenBuffers(1, &new_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, new_VBO);
	glBufferData(GL_ARRAY_BUFFER, new_size, nullptr, gl_type);

	// 显存拷贝旧数据
	glBindBuffer(GL_COPY_READ_BUFFER, VBO_ID);
	glBindBuffer(GL_COPY_WRITE_BUFFER, new_VBO);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, max_size);

	// 替换VBO
	glDeleteBuffers(1, &VBO_ID);
	VBO_ID = new_VBO;
	max_size = new_size;

	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}