#pragma once

#include <assimp/scene.h>
#include <vector>

#include "m_mesh.h"
#include "Shader.h"
#include <assimp/mesh.h>
#include <glm/fwd.hpp>
#include <string>
#include <imageTexture.h>
#include <unordered_map>

class Model {
public:
	Model(const std::string& path);
	~Model();

	void Draw(Shader& shade);
	void setTransform(const glm::mat4& mat);
	glm::mat4 getTransform() const;

	void Clean();
private:
	const aiScene* scene = nullptr;
	std::string directory;
	std::vector<M_Mesh> meshes;

	std::vector<Texture*> textures;
	std::unordered_map<std::string, Texture*> textureMap;

	glm::mat4 alltransform = glm::mat4(1.0f);

	void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
	M_Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& currentTransform);
};
