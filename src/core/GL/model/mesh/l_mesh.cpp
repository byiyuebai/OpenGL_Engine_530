#include "l_mesh.h"
#include "Renderer.h"
L_Mesh::L_Mesh(std::vector<LineVertex>& lineVertices, std::vector<unsigned int>& indices)
{
	this->lineVertices = lineVertices;
	this->indices = indices;
	type = MeshType::LINEVERTEX;

	VAO = new VertexArray();
	VBO = new VertexBuffer(this->lineVertices.data(), this->lineVertices.size() * sizeof(LineVertex));
	EBO = new ElementBuffer(this->indices.data(), this->indices.size());
	VertexBufferLayout* layout = new VertexBufferLayout();
	layout->Push<float>(3);
	layout->Push<float>(4);
	VAO->AddBuffer(*VBO, *EBO, *layout);
}

L_Mesh::~L_Mesh(){}

void L_Mesh::Draw(Shader& shader)
{
	Renderer* renderer = &Renderer::Instance();
	renderer->DrawLine(*VAO, shader);
}

void L_Mesh::Clean(){
	delete VAO;
	delete VBO;
	delete EBO;
	//delete layout;
}
