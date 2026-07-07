#pragma once
#include <glad/glad.h>
class VertexBuffer {
public:
	VertexBuffer(const void* data, unsigned int size, int gl_type= GL_STATIC_DRAW);
	~VertexBuffer();

	void Bind() const;
	static void UnBind();

	void Clean() const;

	void UpdateData(const void* data, int offset, unsigned int size);
	void ResetData(const void* data, unsigned int size, int gl_type = GL_STATIC_DRAW);
	void Expand(unsigned int new_size, int gl_type = GL_STATIC_DRAW);

	unsigned int GetMaxSize() const { return max_size; }
private:
	unsigned int VBO_ID;
	unsigned int max_size;
};