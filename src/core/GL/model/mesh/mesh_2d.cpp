#include "mesh_2d.h"
#include "Renderer.h"
#include "texture_manager.h"
Mesh_2D::Mesh_2D(std::vector<Vertex2D>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures) {
	this->vertices = vertices;
	this->indices = indices;

	if (!textures.empty()) {
		for (const auto& tex : textures) {
			this->textures.push_back(tex);
		}
	}

	type = MeshType::Vertex2D;

	VAO = new VertexArray();
	VBO = new VertexBuffer(this->vertices.data(), this->vertices.size() * sizeof(Vertex2D));
	EBO = new ElementBuffer(this->indices.data(), this->indices.size());
	VertexBufferLayout* layout = new VertexBufferLayout();
	layout->Push<float>(2);
	layout->Push<float>(2);
	VAO->AddBuffer(*VBO, *EBO, *layout);
}


Mesh_2D::~Mesh_2D() {}

void Mesh_2D::Draw(Shader& shader) {
	Renderer* renderer = &Renderer::Instance();

	for (unsigned int i = 0; i < textures.size(); i++) {
		textures[i]->activeTex(i);
		textures[i]->bind();
	}
	renderer->Draw(*VAO, shader);
}
void Mesh_2D::unTexDraw(Shader& shader){
	Renderer* renderer = &Renderer::Instance();
	renderer->Draw(*VAO, shader);
}
void Mesh_2D::Clean(){
	delete VAO;
	delete VBO;
	delete EBO;
}

void Mesh_2D::CleanTextures()
{
	for (int i = 0; i < textures.size(); i++) {
		TextureManager::getInstance()->unloadTexture(textures[i]->getPath());
	}
}
