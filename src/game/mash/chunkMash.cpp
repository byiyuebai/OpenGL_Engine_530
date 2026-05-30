#include "chunkMash.h"
#include "World.h"

#include <cstdlib>
#include <glm/fwd.hpp>


glm::i64vec4 RaycastBlock(Camera* camera, World* world) {
	if (!camera || !world)
		return glm::i64vec4(0, 0, 0, -1);

	glm::i64vec4 res = glm::i64vec4(0, 0, 0, -1);

	glm::dvec3 camPos = camera->getPos();
	glm::dvec3 camFront = camera->getFront();

	glm::i64vec3 blockPos = worldToBlockPos(camPos);

	Chunk* initChunk = world->getChunk(worldToChunk(blockPos), false);
	if (initChunk) {
		glm::ivec3 local = worldToChunkLocal(glm::dvec3(blockPos.x, blockPos.y, blockPos.z));
		int blockID = initChunk->getBlockDataSafe(local.x, local.y, local.z);
		if (blockID != 0) {
			return glm::i64vec4(blockPos.x, blockPos.y, blockPos.z, -1);
		}
	}

	glm::ivec3 stepDir;
	stepDir.x = camFront.x > 0 ? 1 : (camFront.x < 0 ? -1 : 0);
	stepDir.y = camFront.y > 0 ? 1 : (camFront.y < 0 ? -1 : 0);
	stepDir.z = camFront.z > 0 ? 1 : (camFront.z < 0 ? -1 : 0);


	glm::dvec3 delta;
	delta.x = (camFront.x != 0) ? 1.0 / std::abs(camFront.x) : 1e30;
	delta.y = (camFront.y != 0) ? 1.0 / std::abs(camFront.y) : 1e30;
	delta.z = (camFront.z != 0) ? 1.0 / std::abs(camFront.z) : 1e30;

	glm::dvec3 tMax;

	if (camFront.x > 0) tMax.x = ((double)blockPos.x + 0.5 - camPos.x) * delta.x;
	else if (camFront.x < 0) tMax.x = (camPos.x - ((double)blockPos.x - 0.5)) * delta.x;
	else tMax.x = 1e30;

	if (camFront.y > 0) tMax.y = ((double)blockPos.y + 0.5 - camPos.y) * delta.y;
	else if (camFront.y < 0) tMax.y = (camPos.y - ((double)blockPos.y - 0.5)) * delta.y;
	else tMax.y = 1e30;

	if (camFront.z > 0) tMax.z = ((double)blockPos.z + 0.5 - camPos.z) * delta.z;
	else if (camFront.z < 0) tMax.z = (camPos.z - ((double)blockPos.z - 0.5)) * delta.z;
	else tMax.z = 1e30;


	double totalDistance = 0.0;
	int face = -1;

	while (totalDistance < 50.0) {
		if (tMax.x < tMax.y && tMax.x < tMax.z) {
			face = (stepDir.x > 0) ? 5 : 4;
			blockPos.x += stepDir.x;
			totalDistance = tMax.x;
			tMax.x += delta.x;
		}
		else if (tMax.y < tMax.z) {
			face = (stepDir.y > 0) ? 2 : 3;
			blockPos.y += stepDir.y;
			totalDistance = tMax.y;
			tMax.y += delta.y;
		}
		else {
			face = (stepDir.z > 0) ? 1 : 0;
			blockPos.z += stepDir.z;
			totalDistance = tMax.z;
			tMax.z += delta.z;
		}

		Chunk* chunk = world->getChunk(worldToChunk(blockPos), false);
		if (chunk != nullptr) {
			glm::dvec3 wp = glm::dvec3(blockPos.x, blockPos.y, blockPos.z);
			glm::ivec3 local = worldToChunkLocal(wp);
			int blockID = chunk->getBlockDataSafe(local.x, local.y, local.z);
			if (blockID != 0) {
				res = glm::i64vec4(blockPos.x, blockPos.y, blockPos.z, face);
				return res;
			}
		}
	}
	return res;
}