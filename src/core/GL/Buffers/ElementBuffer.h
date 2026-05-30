#pragma once
#include <glad/glad.h>
class ElementBuffer {
public:
	ElementBuffer(const void* data, unsigned int count, int gl_type = GL_STATIC_DRAW);
	~ElementBuffer();

	void Bind() const;
	void UnBind() const;

	void Clean() const;

	unsigned int getID() const { return EBO_ID; }

	void UpdateData(const void* data, int offset, unsigned int size);
	void ResetData(const void* data, unsigned int size, int gl_type = GL_STATIC_DRAW);
	void Expand(unsigned int new_count, int gl_type = GL_STATIC_DRAW);

	unsigned int GetMaxCount() const;

private:
	unsigned int EBO_ID;
	unsigned int max_count;
};