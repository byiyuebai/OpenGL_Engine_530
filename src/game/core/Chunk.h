#pragma once
#include <atomic>
#include "block_best.h"
#include "c_mesh.h"

#include <mutex>
class World;

class Chunk {
public:
	const static int ChunkSize = 32;

	Chunk(glm::ivec3 pos);
	~Chunk();


	void Draw(Shader& shader, glm::dvec3 cameraPos);

	void buildMeshData();

	void buildMeshVertex();

	void buildMeshIndex();

	void setBlock(int x, int y, int z, int blockID);

	int getBlockStateIndex(int blockID);

	void setWorldPos(const glm::ivec3& pos);

	bool isBlockPresent(int x, int y, int z);

	std::vector<std::pair<BlockState, int>>& getBlockStatesData() { return blockStatesData; }

	std::array<uint16_t, ChunkSize * ChunkSize * ChunkSize>& getBlockData() { return blockData; }

	int getBlockDataSafe(int x, int y, int z);

	glm::ivec3 getChunkPos() const;

	void setWorld(World* world) { this->world = world;}

	void fill0();

	//三维坐标转数组索引
	static constexpr inline int PosToIndex(int x, int y, int z) noexcept{return (x * ChunkSize + y) * ChunkSize + z;}

	//================================
	void setUpdateBlock(int x, int y, int z, int blockID);

	std::mutex chunkMutex;//操作区块时上锁

	void out();
private:

	std::vector<std::pair<BlockState, int>> blockStatesData;

	std::array<uint16_t, ChunkSize* ChunkSize* ChunkSize> blockData;

	int X, Y, Z;

	C_Mesh* mesh = nullptr;

	World* world = nullptr;
};