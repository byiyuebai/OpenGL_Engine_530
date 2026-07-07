#pragma once
#include <vector>

#include "ElementBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include <glm/glm.hpp>
class Shader;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
struct Vertex2D
{
	glm::vec2 Position;
	glm::vec2 TexCoords;
};
struct LineVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
};
// 纹理索引约定
enum class TextureSlot : int8_t {
	BaseColor = 0,
	Normal = 1,
};
class Mesh {
public:
	enum class MeshType { VERTEX, Vertex2D,LINEVERTEX };

	MeshType type = MeshType::VERTEX;

	std::vector<unsigned int> indices;

	glm::mat4 transform = glm::mat4(1.0f);

	VertexArray* GetVAO() const { return VAO; }
	VertexBuffer* GetVBO() const { return VBO; }
	ElementBuffer* GetEBO() const { return EBO; }

	Mesh() = default;
	virtual ~Mesh() = default;

	virtual void Draw(Shader& shader) = 0;

	virtual void Clean() = 0;
protected:
	/*  渲染数据  */
	VertexArray* VAO = nullptr;
	VertexBuffer* VBO = nullptr;
	ElementBuffer* EBO = nullptr;

	bool isLoadToGPU = false;
private:

};