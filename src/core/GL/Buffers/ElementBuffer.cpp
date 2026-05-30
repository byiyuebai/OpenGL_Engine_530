#include "ElementBuffer.h"
ElementBuffer::ElementBuffer(const void* data, unsigned int count, int gl_type) {

	glGenBuffers(1, &EBO_ID);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, gl_type);

	max_count = count;
}

ElementBuffer::~ElementBuffer() {
	Clean();
}

void ElementBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);
}

void ElementBuffer::UnBind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ElementBuffer::Clean() const
{
	glDeleteBuffers(1, &EBO_ID);
}

void ElementBuffer::UpdateData(const void* data, int offset, unsigned int size)
{
	Bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(unsigned int), size * sizeof(unsigned int), data);
}

void ElementBuffer::ResetData(const void* data, unsigned int size, int gl_type)
{
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, gl_type);

	max_count = size;
}

 //换id扩容
void ElementBuffer::Expand(unsigned int expand_count, int gl_type) {
    if (!expand_count) return;

    unsigned int new_count = max_count + expand_count;
    GLuint new_EBO;

    // 【关键修复】绑定索引缓冲，而非顶点缓冲
    glGenBuffers(1, &new_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, new_count * sizeof(unsigned int), nullptr, gl_type);

    // 拷贝旧数据
    glBindBuffer(GL_COPY_READ_BUFFER, EBO_ID);
    glBindBuffer(GL_COPY_WRITE_BUFFER, new_EBO);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,0, 0, max_count * sizeof(unsigned int));

    // 替换缓冲
    glDeleteBuffers(1, &EBO_ID);
    EBO_ID = new_EBO;
    max_count = new_count;

    // 解绑
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    Bind();
}


//void ElementBuffer::Expand(unsigned int expand_count, int gl_type)
//{
//	if (!expand_count) return;
//
//	Bind();
//	unsigned int new_count = max_count + expand_count;
//	unsigned int old_size = max_count * sizeof(unsigned int);
//	unsigned int new_size = new_count * sizeof(unsigned int);
//
//	// 临时缓冲
//	GLuint staging;
//	glGenBuffers(1, &staging);
//	glBindBuffer(GL_COPY_WRITE_BUFFER, staging);
//	glBufferData(GL_COPY_WRITE_BUFFER, old_size, nullptr, GL_STATIC_COPY);
//
//	// 拷贝旧数据
//	glBindBuffer(GL_COPY_READ_BUFFER, EBO_ID);
//	glCopyBufferSubData(
//		GL_COPY_READ_BUFFER,
//		GL_COPY_WRITE_BUFFER,
//		0, 0, old_size
//	);
//
//	// 原地扩容（ID 不变）
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, new_size, nullptr, gl_type);
//
//	// 拷回旧数据
//	glCopyBufferSubData(
//		GL_COPY_WRITE_BUFFER,
//		GL_COPY_READ_BUFFER,
//		0, 0, old_size
//	);
//
//	// 清理
//	glDeleteBuffers(1, &staging);
//	glBindBuffer(GL_COPY_READ_BUFFER, 0);
//	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
//
//	max_count = new_count;
//}

unsigned int ElementBuffer::GetMaxCount() const { return max_count; }
