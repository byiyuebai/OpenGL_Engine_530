#include "VertexArray.h"
#include "VertexBufferLayout.h"
VertexArray::VertexArray()
{
	glGenVertexArrays(1, &VAO_ID);
	Bind();
}

VertexArray::~VertexArray()
{
	Clean();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const ElementBuffer& eb, const VertexBufferLayout& layout) {
	Bind();
	vb.Bind();
	eb.Bind();
	location = 0;
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(static_cast<uintptr_t>(offset)));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		location++;
	}
	max_Count = eb.GetMaxCount();
	current_count = max_Count;
}

void VertexArray::AddInstanceBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, unsigned int divisor)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(static_cast<uintptr_t>(offset)));

		glVertexAttribDivisor(location, divisor);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		location++;
	}
}

void VertexArray::Bind() const {
	//std::cout << "Bind VAO: " << VAO_ID << std::endl;
	glBindVertexArray(VAO_ID);
}
void VertexArray::Unbind() const {
	glBindVertexArray(0);
}

void VertexArray::Clean() const
{
	glDeleteVertexArrays(1, &VAO_ID);
}
