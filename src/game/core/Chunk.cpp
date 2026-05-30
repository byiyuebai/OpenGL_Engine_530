#include "Chunk.h"

#include <iostream>
#include <unordered_set>
#include "register_block.h"
#include "Shader.h"
#include "World.h"
Chunk::Chunk(glm::ivec3 pos) {

	if (mesh == nullptr) mesh = new C_Mesh();

	setWorldPos(pos);
	std::fill(&getBlockData()[0], &getBlockData()[0] + sizeof(int) * ChunkSize * ChunkSize * ChunkSize / sizeof(int), 0);

	BlockState state = RegisterBlock::getInstance()->getBlockState_ID(0);
	blockStatesData.push_back(std::make_pair(state, ChunkSize * ChunkSize * ChunkSize));
}

Chunk::~Chunk()
{
	if (mesh) {
		delete mesh;
	}
}

void Chunk::Draw(Shader& shader, glm::dvec3 cameraPos) {
	if (mesh) {
		shader.use();
		glm::vec3 chunkRelativePos = glm::dvec3(static_cast<int64_t>(X) * ChunkSize, static_cast<int64_t>(Y) * ChunkSize, static_cast<int64_t>(Z) * ChunkSize) - glm::dvec3(cameraPos);
		shader.uniformsetVec3("chunkRelativePos", chunkRelativePos);

		mesh->Draw(shader);
	}
	else
	{
		std::wcerr << L"\033[31m[ERROR]\033[0m [Chunk] Chunk at (" << X << ", " << Y << ", " << Z << ") has no mesh to draw." << std::endl;
	}
}

void Chunk::buildMeshData() {
	int count = 0;


	std::vector<Texture*> textures;

	std::unordered_set<Texture*> textureSet;
	// 第一步：遍历 blockStates，将不重复的 Texture* 存入集合
	for (const auto& blockState : blockStatesData) {
		Texture* tex = blockState.first.data->tex;
		if (tex != nullptr) {  // 避免空指针（可选，根据你的业务需求）
			textureSet.insert(tex);  // 集合自动去重，重复元素不会插入
		}
	}

	// 第二步：将去重后的集合转为向量（按需）
	textures.assign(textureSet.begin(), textureSet.end());

	if(textures.empty())
	{
		return;// 没有纹理需要绑定，说明没有方块,直接返回
	}

	std::vector<std::pair<TextureSlot, Texture*>> textureBindings = {
		{TextureSlot::BaseColor, textures[0]}
	};
	mesh->SetTexture(textureBindings);

	mesh->ClearData();

	// 4. 遍历区块内所有方块（16×16×16）
	for (int x = 0; x < ChunkSize; x++) {
		for (int y = 0; y < ChunkSize; y++) {
			for (int z = 0; z < ChunkSize; z++) {
				// 获取当前方块的状态索引（blockData存储的是blockState的索引）
				int stateIndex = blockData[PosToIndex(x,y,z)];
				if (stateIndex == 0 || stateIndex >= blockStatesData.size()) {
					continue; // 空气方块/无效索引，跳过
				}

				// 获取当前方块的实际ID和UV偏移
				BlockState currentState = blockStatesData[stateIndex].first;
				std::array<glm::vec2, 4> uvOffset = currentState.data->uvOffset;

				glm::vec3 blockWorldPos = glm::vec3((float)x,(float)y,(float)z);

				// 5. 面剔除：只渲染暴露的面
				// 顶面（z+1方向）
				if (!isBlockPresent(x, y, z + 1)) {
					mesh->addface(glm::ivec4(blockWorldPos.x, blockWorldPos.y, blockWorldPos.z, 0), uvOffset);
				}
				// 底面（z-1方向）
				if (!isBlockPresent(x, y, z - 1)) {
					mesh->addface(glm::ivec4(blockWorldPos.x, blockWorldPos.y, blockWorldPos.z, 1), uvOffset);
				}
				// 左面（y-1方向）
				if (!isBlockPresent(x, y - 1, z)) {
					mesh->addface(glm::ivec4(blockWorldPos.x, blockWorldPos.y, blockWorldPos.z, 2), uvOffset);
				}
				// 右面（y+1方向）
				if (!isBlockPresent(x, y + 1, z)) {
					mesh->addface(glm::ivec4(blockWorldPos.x, blockWorldPos.y, blockWorldPos.z, 3), uvOffset);
				}
				// 前面（x+1方向）
				if (!isBlockPresent(x + 1, y, z)) {
					mesh->addface(glm::ivec4(blockWorldPos.x, blockWorldPos.y, blockWorldPos.z, 4), uvOffset);
				}
				// 后面（x-1方向）
				if (!isBlockPresent(x - 1, y, z)) {
					mesh->addface(glm::ivec4(blockWorldPos.x, blockWorldPos.y, blockWorldPos.z, 5), uvOffset);
				}
			}
		}
	}
}

void Chunk::buildMeshIndex()
{
	if (mesh != nullptr) mesh->buildIndexBuffer();
	else std::wcerr << L"\033[31m[ERROR]\033[0m [Chunk] build index buffer: mesh is null." << std::endl;
}

void Chunk::buildMeshVertex()
{
	if (mesh != nullptr) {
		if (!mesh->init) { mesh->initGPU();}
		mesh->buildVertexBuffer(); 
	}
	else std::wcerr << L"\033[31m[ERROR]\033[0m [Chunk] build vertex buffer: mesh is null." << std::endl;
}

void Chunk::setBlock(int x, int y, int z, int blockID)
{
	if (static_cast<uint32_t>(x) >= ChunkSize ||
		static_cast<uint32_t>(y) >= ChunkSize ||
		static_cast<uint32_t>(z) >= ChunkSize)
	{
		return;
	}

	int stateIndex = getBlockStateIndex(blockID);

	blockStatesData[blockData[PosToIndex(x, y, z)]].second--;
	blockData[PosToIndex(x, y, z)] = stateIndex;
	blockStatesData[stateIndex].second++;
}



int Chunk::getBlockStateIndex(int blockID)
{
	for (int i = 0; i < blockStatesData.size(); i++) {
		if (blockStatesData[i].first.ID == blockID) {
			return i;
		}
	}

	// 没有就创建新状态
	BlockState newState = RegisterBlock::getInstance()->getBlockState_ID(blockID);

	// 【优化】优先复用空计数的位置（你原来的逻辑，但更安全）
	int emptyIndex = -1;
	for (int i = 1; i < blockStatesData.size(); i++) {
		if (blockStatesData[i].second == 0) {
			emptyIndex = i;
			break;
		}
	}

	if (emptyIndex != -1) {
		blockStatesData[emptyIndex] = std::make_pair(newState, 0);
		return emptyIndex;
	}

	// 没有空位就追加新的
	blockStatesData.push_back(std::make_pair(newState, 0));

	int newIndex = blockStatesData.size() - 1;

	return newIndex;
}


void Chunk::setWorldPos(const glm::ivec3& pos)
{
	X = pos.x;
	Y = pos.y;
	Z = pos.z;
}

// 辅助函数1：检查指定位置是否有方块（是否需要渲染相邻面）
bool Chunk::isBlockPresent(int x, int y, int z) {

	//是空气返回false,渲染相邻面

	if (x >= 0 && x < ChunkSize && y >= 0 && y < ChunkSize && z >= 0 && z < ChunkSize) {
		// 区块内：直接检查方块状态
		int stateIndex = blockData[PosToIndex(x, y, z)];
		return (stateIndex > 0 && stateIndex < blockStatesData.size());
	}
	if (world == nullptr) {
		return false; // 无世界 → 视为空气
	}


	// 计算邻居区块坐标和本地坐标
	glm::ivec3 neighborChunkPos = glm::ivec3(X, Y, Z);
	int localX = x, localY = y, localZ = z;

	if (x < 0) {
		neighborChunkPos.x -= 1;
		localX += ChunkSize;

	}
	else if (x >= ChunkSize) {
		neighborChunkPos.x += 1;
		localX -= ChunkSize;
	}

	if (y < 0) {
		neighborChunkPos.y -= 1;
		localY += ChunkSize;
	}
	else if (y >= ChunkSize) {
		neighborChunkPos.y += 1;
		localY -= ChunkSize;
	}

	if (z < 0) {
		neighborChunkPos.z -= 1;
		localZ += ChunkSize;
	}
	else if (z >= ChunkSize) {
		neighborChunkPos.z += 1;
		localZ -= ChunkSize;
	}

	// 获取邻居区块并递归检查（带空指针保护）
	Chunk* neighborChunk = world->getChunk(neighborChunkPos,false);

	if (neighborChunk == nullptr) {
		return false; // 邻居区块不存在 → 视为空气
	}
	return neighborChunk->isBlockPresent(localX, localY, localZ);
}

int Chunk::getBlockDataSafe(int x, int y, int z)
{
	if (x < 0 || x >= ChunkSize || y < 0 || y >= ChunkSize || z < 0 || z >= ChunkSize){
		std::wcerr << L"\033[31m[ERROR]\033[0m [Chunk] 区块本地坐标越界！" << L" x=" << x << L" y=" << y << L" z=" << z << L" | ChunkSize=" << ChunkSize << std::endl;

		// 返回空气块，避免崩溃
		return 0;
	}
	return blockData[PosToIndex(x, y, z)];
}

glm::ivec3 Chunk::getChunkPos() const
{
	return glm::ivec3(X, Y, Z);
}

void Chunk::fill0()
{
	blockStatesData.clear();
	blockStatesData.shrink_to_fit();

	std::fill(&getBlockData()[0], &getBlockData()[0] + sizeof(int) * ChunkSize * ChunkSize * ChunkSize / sizeof(int), 0);
	BlockState state = RegisterBlock::getInstance()->getBlockState_ID(0);
	blockStatesData.push_back(std::make_pair(state, ChunkSize * ChunkSize * ChunkSize));
}


void Chunk::setUpdateBlock(int x, int y, int z, int blockID)
{
	if (x < 0 || x >= ChunkSize || y < 0 || y >= ChunkSize || z < 0 || z >= ChunkSize) {
		std::wcout << L"\033[31m[ERROR]\033[0m [Chunk] 局部坐标越界: (" << x << L", " << y << L", " << z << L")" << std::endl;
		return;
	}
	int stateIndex = getBlockStateIndex(blockID);

	blockStatesData[blockData[PosToIndex(x, y, z)]].second--;
	blockData[PosToIndex(x, y, z)] = stateIndex;
	blockStatesData[stateIndex].second++;

	
	buildMeshData();
}

void Chunk::out()
{
}
