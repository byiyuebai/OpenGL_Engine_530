#pragma once
#include "ElementBuffer.h"
#include "VertexBuffer.h"
class VertexBufferLayout;

class VertexArray
{
private:
	unsigned int VAO_ID;

	unsigned int location = 0;
	unsigned int max_Count;
	unsigned int current_count;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const ElementBuffer& eb, const VertexBufferLayout& layout);
	void AddInstanceBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, unsigned int divisor);

	void Bind() const;
	static void Unbind();

	void Clean() const;

	unsigned int getID() const { return VAO_ID; }
	
	unsigned int GetMaxIndexCount() const { return max_Count; }

	unsigned int GetCurrentIndexCount() const { return current_count; }
	void SetCurrentIndexCount(unsigned int count) { current_count = count; }
};