#include <cmath>
#include <cstdint>
#include <glm/fwd.hpp>
#include <iostream>
#include <World.h>
#include "Camera.h"
#include "Chunk.h"
inline int64_t worldToBlockPos(const double worldCoord) {
	return static_cast<int64_t>(std::floor(worldCoord + 0.5));
}

// 世界坐标 → 区块坐标
inline int worldToChunk(double worldCoord) {
	int64_t pos = worldToBlockPos(worldCoord);
	int64_t chunk = 0;
	if (pos >= 0) {
		chunk = pos / Chunk::ChunkSize;
	}
	else {
		chunk = (pos - Chunk::ChunkSize + 1) / Chunk::ChunkSize;
	}

	return static_cast<int>(chunk);
}

// 世界坐标 → 区块内本地坐标 [0,15]
inline int worldToChunkLocal(double worldCoord)
{
	int64_t pos = worldToBlockPos(worldCoord);

	int local = (pos % Chunk::ChunkSize + Chunk::ChunkSize) % Chunk::ChunkSize;

	if (local < 0) { std::wcerr << L"\033[31m[ERROR]\033[0m [chunkMash] local坐标小于0;pos:" << pos << "coord:" << worldCoord << std::endl; }
	return static_cast<int>(local);
}

inline glm::i64vec3 worldToBlockPos(const glm::dvec3& worldPos) {
	return glm::i64vec3(
		worldToBlockPos(worldPos.x),
		worldToBlockPos(worldPos.y),
		worldToBlockPos(worldPos.z)
	);
}

inline glm::ivec3 worldToChunk(const glm::dvec3& worldPos) {
	return glm::ivec3(
		worldToChunk(worldPos.x),
		worldToChunk(worldPos.y),
		worldToChunk(worldPos.z)
	);
}

inline glm::ivec3 worldToChunkLocal(const glm::dvec3& worldPos) {
	return glm::ivec3(
		worldToChunkLocal(worldPos.x),
		worldToChunkLocal(worldPos.y),
		worldToChunkLocal(worldPos.z)
	);
}



glm::i64vec4 RaycastBlock(Camera* camera, World* world);