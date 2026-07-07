#include "c_mesh.h"
#include "texture_manager.h"
#include "block_best.h"
#include "Renderer.h"
#include <iostream>
C_Mesh::C_Mesh(std::vector<Texture*>& textures) {


	this->textures = textures;
	this->textures.resize(5, nullptr);

	type = MeshType::VERTEX;
}

C_Mesh::C_Mesh()
{
	type = MeshType::VERTEX;

	this->textures.resize(5, nullptr);

	while (!freeFaceSlots.empty()) {
		freeFaceSlots.pop();
	}
}


C_Mesh::~C_Mesh() {
	Clean();
}

void C_Mesh::initGPU()
{

	VAO = new VertexArray();
	VBO = new VertexBuffer(nullptr, 0 * sizeof(Vertex), GL_DYNAMIC_DRAW);
	EBO = new ElementBuffer(nullptr, 0, GL_DYNAMIC_DRAW);

	layout = new VertexBufferLayout();
	layout->Push<float>(3);
	layout->Push<float>(3);
	layout->Push<float>(2);
	VAO->AddBuffer(*VBO, *EBO, *layout);

	init = true;
}

void C_Mesh::ClearData()
{
	indices.clear();
	while (!freeFaceSlots.empty()) {
		freeFaceSlots.pop();
	}
	faceToSlots.clear();
	faceToIndx.clear();
	updateFacelist.clear();

	for (unsigned int i = maxFaceCount; i > 0; i--) {
		freeFaceSlots.push(i - 1);
	}
}
void C_Mesh::Draw(Shader& shader) {
	if (init) {
		Renderer& renderer = Renderer::Instance();

		for (unsigned int i = 0; i < textures.size(); i++) {
			if (textures[i]) {
				textures[i]->activeTex(i);
				textures[i]->bind();
			}
		}
		if (VAO) {
			renderer.Draw(*VAO, shader);
		}
		else {
			std::wcerr << L"\033[33m[WARNING]\033[0m [c_mesh] VAO是空指针\n";
		}
	}
	else {
		std::wcerr << L"\033[33m[WARNING]\033[0m [c_mesh] 未初始化\n";
	}
}
void C_Mesh::Clean()
{
	if (VAO) {delete VAO;}
	if(VBO){delete VBO;}
	if(EBO){delete EBO;}
	if(layout){delete layout;}
}

void C_Mesh::SetTexture(std::vector<std::pair<TextureSlot, Texture*>>& data)
{
	for (const auto& pair : data) {
		int slot = static_cast<int>(pair.first);

		if (slot >= 0 && slot < textures.size()) {
			textures[slot] = pair.second;
		}
		else {
			std::wcerr << L"\033[31m[ERROR]\033[0m [c_mesh] SetTexture: 无效的纹理槽位 " << slot << std::endl;
		}
	}
}



void C_Mesh::addface(const glm::ivec4& pos, const std::array<glm::vec2, 4>& uvOffset)
{
	unsigned int faceIndex;
	if (freeFaceSlots.empty()) [[likely]] {
		faceIndex = maxFaceCount++;
		expandCount++;
	}
	else {
		faceIndex = freeFaceSlots.top();
		freeFaceSlots.pop();
	}

	std::array<Vertex, 4> vertices;
	const glm::vec3 posVec(pos.x, pos.y, pos.z);
	const int base = pos.w * 4;

	vertices[0] = {
		blockVertex[base + 0].Position + posVec,
		blockVertex[base + 0].Normal,
		uvOffset[0]
	};
	vertices[1] = {
		blockVertex[base + 1].Position + posVec,
		blockVertex[base + 1].Normal,
		uvOffset[1]
	};
	vertices[2] = {
		blockVertex[base + 2].Position + posVec,
		blockVertex[base + 2].Normal,
		uvOffset[2]
	};
	vertices[3] = {
		blockVertex[base + 3].Position + posVec,
		blockVertex[base + 3].Normal,
		uvOffset[3]
	};

	const unsigned int idxBase = faceIndex * 4;
	const std::array<unsigned int, 6> indices = {
		idxBase + 0, idxBase + 1, idxBase + 2,
		idxBase + 0, idxBase + 2, idxBase + 3
	};

	faceToSlots.try_emplace(pos, faceIndex);
	faceToIndx.try_emplace(pos, indices);

	updateFacelist.emplace_back(faceIndex, std::move(vertices));
}

void C_Mesh::buildIndexBuffer()
{
	indices.clear();
	for (const auto& pair : faceToIndx) {
		const auto& arr = pair.second;
		indices.insert(indices.end(), arr.begin(), arr.end());
	}
	VAO->SetCurrentIndexCount(indices.size());

	VAO->Unbind();
	EBO->UpdateData(indices.data(), 0, indices.size());
}

void C_Mesh::buildVertexBuffer()
{
	expandVBOEBO(expandCount);
	expandCount = 0;
	//std::cout << "[c_mesh]buildVertexBuffer: " << faceToSlots.size() << " faces, " << freeFaceSlots.size() << " free slots, maxFaceCount: " << maxFaceCount << std::endl;
	for (const auto& update : updateFacelist) {
		VBO->UpdateData(&update.second, update.first * 4 * sizeof(Vertex), sizeof(Vertex) * 4);
	}
	updateFacelist.clear();

	buildIndexBuffer();
}

void C_Mesh::expandVBOEBO(unsigned int face_count)
{
	if(face_count==0) return;
	VBO->Expand(face_count * 4 * sizeof(Vertex), GL_DYNAMIC_DRAW);
	EBO->Expand(face_count * 6, GL_DYNAMIC_DRAW);

	VAO->AddBuffer(*VBO, *EBO, *layout);
}