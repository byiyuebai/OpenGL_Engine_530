#pragma once

#include <atomic>
#include <condition_variable>
#include <glm/fwd.hpp>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Chunk.h"
#include "PerlinNoise.h"
#include "Shader.h"


// 自定义比较器：只比较 pair 的第一个值（int 优先级）
struct ComparePair
{
	bool operator()(const std::pair<glm::ivec3,int>& a, const std::pair<glm::ivec3,int>& b)
	{
		// 小顶堆：数字小的优先
		return a.second > b.second;
	}
};

class World {

public:
	World(int seed, glm::dvec3 cameraPos);
	~World();
	void Clean();

	void Draw(Shader& shader, glm::dvec3 cameraPos);

	void update(double dt, glm::dvec3 cameraPos);


	Chunk* getChunk(const glm::ivec3& chunkPos, bool outcerr = true);

	Chunk* getChunkorCreate(const glm::ivec3& chunkPos);

	Chunk* createChunk(const glm::ivec3& chunkPos);

	Chunk* generateChunk(Chunk* chunk);

	void addChunks(Chunk* chunk);

	void fillChunk(Chunk* chunk);

	// 更新需要渲染的区块列表
	void updateRenderChunks();
	// 将区块加入渲染列表
	void addRenderChunk(Chunk* chunk);

	void addCreatChunk(glm::ivec3 chunkPos);

	//使用主线程将区块数据传入GPU端
	void uploadChunkDataToGPU();

	void addchunkMeshUpdateQueue(glm::ivec3 pos);
private:

	void threadGenerateChunks();
	void threadbuildMeshDataChunks();




	int seed;
	PerlinNoise noise;

	glm::ivec3 lastChunkPos = { 0,0,0 };

	std::unordered_map<glm::ivec3, Chunk*> chunks;
	std::mutex chunksMutex;

	//需要渲染的Chunk列表
	std::unordered_set<Chunk*> renderChunks;
	std::mutex renderMutex;

	//需要创建的Chunk队列(可以在子线程生成区块)
	std::queue<glm::ivec3> chunkCreateQueue;
	std::unordered_set<glm::ivec3> chunkCreateSet;//现在仅用于addCreatChunk去重复需要修改请谨慎  ,子线程内chunkCreateQueue立刻出队,chunkCreateSet创建好后才会删除
	std::mutex createMutex;
	std::condition_variable createCV;

	//需要更新数据的Chunk队列(给子线程使用)
	std::priority_queue<std::pair<glm::ivec3,int>,std::vector<std::pair<glm::ivec3,int>>,ComparePair> chunkBiuldMeshDataQueue;
	std::mutex biuldMeshMutex;
	std::condition_variable BiuldMeshCV;

	//需要更新GPU端网格的Chunk队列(只能在主线程中操作)
	std::queue<glm::ivec3> chunkMeshUpdateQueue;
	std::mutex meshUpdateMutex;

	std::vector<std::thread> CreateThreads;

	std::vector<std::thread> BiuldMeshThread;

	std::atomic<bool> stopThread{ false };

};
/*


*/