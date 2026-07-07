#include "m_mesh.h"
#include "Renderer.h"
#include "texture_manager.h"
M_Mesh::M_Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures) {

	this->indices = indices;
	if (!textures.empty()) {
		for (const auto& tex : textures) {
			this->textures.push_back(tex);
		}
	}
	type = MeshType::VERTEX;

	VAO = new VertexArray();
	VBO = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
	EBO = new ElementBuffer(this->indices.data(), this->indices.size());
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(2);
	VAO->AddBuffer(*VBO, *EBO, layout);
}

M_Mesh::~M_Mesh() {}

void M_Mesh::Draw(Shader& shader) {
	Renderer* renderer = &Renderer::Instance();

	for (unsigned int i = 0; i < textures.size(); i++) {
		
		textures[i]->activeTex(i);
		textures[i]->bind();
	}
	renderer->Draw(*VAO, shader);
}
void M_Mesh::Clean()
{
	delete VAO;
	delete VBO;
	delete EBO;
}
