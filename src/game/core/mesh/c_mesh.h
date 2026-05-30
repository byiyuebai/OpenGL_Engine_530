#pragma once

#include "best_mesh.h"
#include <unordered_map>

#include <array>
#include <glm/fwd.hpp>
#include <Shader.h>
#include <stack>
#include <utility>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
class Texture;
class C_Mesh : public Mesh {
public:
	//基类:std::vector<int>indices

	std::vector<Texture*> textures;

	C_Mesh(std::vector<Texture*>& textures);
	C_Mesh();
	~C_Mesh();

	void initGPU();

	void ClearData();

	void Draw(Shader& shader);

	void Clean();
	void CleanTextures();

	void SetTexture(std::vector<std::pair<TextureSlot, Texture*>>& data);

	void addface(const glm::ivec4& pos, const std::array<glm::vec2, 4>& uvOffset);

	void buildIndexBuffer();
	void buildVertexBuffer();
	void expandVBOEBO(unsigned int face_count);



	//===========================================================================
	void ResetMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures);
	void deleteface(glm::ivec4 pos);
	//===========================================================================
	bool init = false;
private:
	VertexBufferLayout* layout = nullptr;

	//当前最大面数 & freeFaceSlots总空间
	unsigned int maxFaceCount = 0;
	//扩容计数
	unsigned int expandCount = 0;

	//顶点显存空位映射(单位Vertex)
	std::stack<unsigned int> freeFaceSlots;
	//面到顶点显存位置的映射(单位Vertex*4)
	std::unordered_map<glm::vec4, unsigned int> faceToSlots;
	//面到索引映射-更新时直接重构索引数据
	std::unordered_map<glm::vec4, std::array<unsigned int, 6>> faceToIndx;
	//顶点更新队列(偏移+顶点数据)
	std::vector<std::pair<unsigned int, std::array<Vertex, 4>>> updateFacelist;
};
/*
区块更新面可以仅修改对应顶点数据,索引必须重构gpu端不能留空位

顶面为z+, 底面为z-, 左面为y-, 右面为y+, 前面为x+, 后面为x-
表示面的vec4: (x,y,z,faceIndex) 其中faceIndex:0-顶面 1-底面 2-左面 3-右面 4-前面 5-后面,
*/