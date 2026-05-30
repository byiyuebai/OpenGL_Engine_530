#include "VertexBuffer.h"
VertexBuffer::VertexBuffer(const void* data, unsigned int size, int gl_type) {
	glGenBuffers(1, &VBO_ID);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_type);
	//glBufferStorage(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_STORAGE_BIT);

	max_size = size;
}

VertexBuffer::~VertexBuffer() {
	Clean();
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
}

void VertexBuffer::UnBind() const {
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

	// 恢复绑定当前VBO
	Bind();
}

//void VertexBuffer::Expand(unsigned int expand_size, int gl_type)
//{
//	if (!expand_size) return;
//
//	Bind(); // 绑定当前 VBO（ID 不变）
//	unsigned int old_size = max_size;
//	unsigned int new_size = old_size + expand_size;
//
//	// ---------------------------
//	// 第一步：创建临时 staging 缓冲
//	// ---------------------------
//	GLuint staging;
//	glGenBuffers(1, &staging);
//	glBindBuffer(GL_COPY_WRITE_BUFFER, staging);
//	glBufferData(GL_COPY_WRITE_BUFFER, old_size, nullptr, GL_STATIC_COPY);
//
//	// ---------------------------
//	// 第二步：GPU 内部拷贝旧数据到临时缓冲
//	// ---------------------------
//	glBindBuffer(GL_COPY_READ_BUFFER, VBO_ID); // 读当前VBO
//	glCopyBufferSubData(
//		GL_COPY_READ_BUFFER,
//		GL_COPY_WRITE_BUFFER,
//		0, 0,          // 偏移都从0开始
//		old_size        // 拷贝全部旧数据
//	);
//
//	// ---------------------------
//	// 第三步：原地重新分配更大空间（ID 不变）
//	// ---------------------------
//	glBufferData(GL_ARRAY_BUFFER, new_size, nullptr, gl_type);
//
//	// ---------------------------
//	// 第四步：把旧数据拷回新空间
//	// ---------------------------
//	glCopyBufferSubData(
//		GL_COPY_WRITE_BUFFER,
//		GL_COPY_READ_BUFFER,
//		0, 0,
//		old_size
//	);
//
//	// ---------------------------
//	// 清理临时缓冲
//	// ---------------------------
//	glDeleteBuffers(1, &staging);
//	glBindBuffer(GL_COPY_READ_BUFFER, 0);
//	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
//
//	// 更新大小
//	max_size = new_size;
//}
