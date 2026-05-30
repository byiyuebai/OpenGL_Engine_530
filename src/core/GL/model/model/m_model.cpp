#include "m_model.h"
#include "texture_manager.h"
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/matrix4x4.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <best_mesh.h>
#include <exception>
#include <filesystem>
#include <glm/ext/matrix_transform.inl>
#include <glm/fwd.hpp>
#include <iostream>
#include <m_mesh.h>
#include <Shader.h>
#include <string>
#include <Texture.h>
#include <vector>
static glm::mat4 aiMat4ToGlmMat4(const aiMatrix4x4& aiMat) {
	glm::mat4 glmMat = glm::mat4(0.0f);
	// 逐行逐列手动转换（关键：处理行列顺序）
	glmMat[0][0] = aiMat.a1; glmMat[1][0] = aiMat.a2; glmMat[2][0] = aiMat.a3; glmMat[3][0] = aiMat.a4;
	glmMat[0][1] = aiMat.b1; glmMat[1][1] = aiMat.b2; glmMat[2][1] = aiMat.b3; glmMat[3][1] = aiMat.b4;
	glmMat[0][2] = aiMat.c1; glmMat[1][2] = aiMat.c2; glmMat[2][2] = aiMat.c3; glmMat[3][2] = aiMat.c4;
	glmMat[0][3] = aiMat.d1; glmMat[1][3] = aiMat.d2; glmMat[2][3] = aiMat.d3; glmMat[3][3] = aiMat.d4;
	return glmMat;
}

Model::Model(const std::string& path) {

	//保存路径
	try {
		// 1. 将字符串转为 filesystem::path 对象
		std::filesystem::path fsPath(path);
		// 2. 获取父目录（核心API）
		std::filesystem::path dirPath = fsPath.parent_path();

		directory = dirPath.string();
	}
	catch (const std::exception& e) {
		std::wcerr << L"获取模型目录失败: " << e.what() << std::endl;
		directory = "";
	}

	Assimp::Importer importer;

	// 1. 定义后处理标志位
	unsigned int importFlags =
		aiProcess_Triangulate          // 三角化
		| aiProcess_FlipUVs            // 翻转UV
		| aiProcess_GenNormals         // 生成法线
		| aiProcess_ValidateDataStructure;

	// 2. 调用 ReadFile 加载模型
	scene = importer.ReadFile(path, importFlags);

	// 3. 检查加载结果
	try {
		if (!scene) {
			std::wcerr << L"模型加载失败: " << importer.GetErrorString() << std::endl;
			return;
		}
		if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
			std::wcerr << L"模型加载不完整，可能是文件损坏或格式不支持" << std::endl;
		}
		if (!scene->mRootNode) {
			std::wcerr << L"模型没有根节点，数据为空" << std::endl;
			return;
		}
	}
	catch (const std::exception& e) {
		std::wcerr << L"模型加载异常: " << e.what() << std::endl;
	}
	// 4. 处理节点递归构建网格
	processNode(scene->mRootNode, scene, glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)));
}

Model::~Model()
{
	Clean();
}

void Model::setTransform(const glm::mat4& mat)
{
	alltransform = mat;
}

glm::mat4 Model::getTransform() const
{
	return alltransform;
}

void Model::Clean() {
	for (auto& mesh : meshes) {
		mesh.Clean();
	}
}

void Model::Draw(Shader& shader) {
	shader.use();
	for (unsigned int i = 0; i < meshes.size(); i++) {

		// 3. 设置模型矩阵到着色器
		shader.uniformsetMat4("model", alltransform * meshes[i].transform);
		meshes[i].Draw(shader);
	}
}


void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
	// 1. 计算当前节点的变换矩阵
	aiMatrix4x4 aiNodeMatrix = node->mTransformation;
	glm::mat4 nodeMatrix = aiMat4ToGlmMat4(aiNodeMatrix);

	// 2. 计算累计变换矩阵（父节点矩阵 * 当前节点矩阵）
	glm::mat4 currentTransform = parentTransform * nodeMatrix;

	// 处理当前节点的所有网格
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, currentTransform));
	}

	// 递归处理子节点
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, currentTransform);
	}
}

M_Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& currentTransform) {
	std::vector<Vertex> newVertices;
	std::vector<unsigned int> newIndices;
	std::vector<Texture*> newTextures;


	// 处理顶点数据
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};

		// 1.1 顶点位置（必选，所有模型都有）
		vertex.Position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		// 1.2 法线（可选，无则设为0）
		if (mesh->HasNormals()) {
			vertex.Normal = glm::vec3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);
		}
		else { vertex.Normal = glm::vec3(0.0f); }



		// 1.4 纹理坐标（可选，取第一套UV）
		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				1.0f - mesh->mTextureCoords[0][i].y
			);
		}
		else { vertex.TexCoords = glm::vec2(0.0f); }

		newVertices.push_back(vertex);
	}

	// 处理索引数据
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			newIndices.push_back(face.mIndices[j]);
		}
	}

	// 处理材质和纹理
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
			aiString aiTexPath;
			mat->GetTexture(aiTextureType_DIFFUSE, i, &aiTexPath);

			std::string relativePath = aiTexPath.C_Str();

			std::filesystem::path basePath(directory);
			std::filesystem::path relPath(relativePath);
			std::filesystem::path fullPath = basePath / relPath;
			// 3. 标准化路径（处理 ../、./、重复分隔符等问题）
			fullPath = fullPath.lexically_normal();


			// 查缓存：存在则复用，不存在则加载
			auto it = textureMap.find(fullPath.string());
			if (it != textureMap.end()) {
				newTextures.push_back(it->second);
			}
			else
			{
				//Texture* newTexture = new Texture(fullPath.string());
				Texture* newTexture = TextureManager::getInstance()->loadTexture(fullPath.string());
				newTextures.push_back(newTexture);
				textureMap.insert({ fullPath.string(), newTexture });
			}
		}
	}




	M_Mesh outmesh(newVertices, newIndices, newTextures);
	outmesh.transform = currentTransform;

	return outmesh;
}
