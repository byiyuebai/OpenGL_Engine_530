#include "World.h"

#include "chunkMash.h"
#include "register_block.h"

#include <chrono> 

World::World(int seed, glm::dvec3 cameraPos)
{
	this->seed = seed;
	noise = PerlinNoise(seed);

	lastChunkPos = worldToChunk(cameraPos);


	for (int i = 0; i < 2; i++)
	{
		CreateThreads.emplace_back(&World::threadGenerateChunks, this);
	}
	for (int i = 0; i < 1; i++) {
		BiuldMeshThread.emplace_back(&World::threadbuildMeshDataChunks, this);
	}
	addCreatChunk(glm::ivec3(0, 0, -1));
	addCreatChunk(glm::ivec3(-2, 0, -1));
}

World::~World()
{
}

void World::Clean()
{
}

void World::Draw(Shader& shader, glm::dvec3 cameraPos)
{
	// 加锁，复制一份渲染列表
	std::vector<Chunk*> renderList;
	{
		std::lock_guard<std::mutex> lock(renderMutex);
		renderList.assign(renderChunks.begin(), renderChunks.end());
	}

	//【无锁】遍历复制出来的列表，绘制
	shader.use();
	for (auto& chunk : renderList) {
		if (!chunk) continue;
		std::lock_guard<std::mutex> lock(chunk->chunkMutex);
		chunk->Draw(shader, cameraPos);
	}
}

void World::update(double dt, glm::dvec3 cameraPos)
{
	if (lastChunkPos != worldToChunk(cameraPos))
	{
		lastChunkPos = worldToChunk(cameraPos);
		updateRenderChunks();
	}

	uploadChunkDataToGPU();

}



Chunk* World::createChunk(const glm::ivec3& chunkPos)
{

	Chunk* newChunk = new Chunk(chunkPos);
	newChunk->setWorld(this);

	return newChunk;
}

Chunk* World::getChunk(const glm::ivec3& chunkPos, bool outcerr)
{
	{
		std::lock_guard<std::mutex> lock(chunksMutex);

		auto it = chunks.find(chunkPos);
		if (it != chunks.end()) { return it->second; }
	}

	if (outcerr) { std::wcerr << L"\033[33m[WARNING]\033[0m [World2]: 区块不存在" << chunkPos.x << L", " << chunkPos.y << L", " << chunkPos.z << std::endl; }

	return nullptr;
}

Chunk* World::getChunkorCreate(const glm::ivec3& chunkPos)
{
	Chunk* chunk = getChunk(chunkPos, false);

	if (!chunk) {
		chunk = createChunk(chunkPos);
		if (chunk) {
			addChunks(chunk);
		}
	}


	return chunk;
}

Chunk* World::generateChunk(Chunk* chunk)
{

	if (chunk == nullptr) {
		std::wcerr << L"\033[33m[WARNING]\033[0m [World2] 生成区块的指针为空" << std::endl;
		return nullptr;
	}

	chunk->fill0();
	fillChunk(chunk);


	return chunk;
}

void World::addChunks(Chunk* chunk)
{
	if (!chunk) return;

	glm::ivec3 pos = chunk->getChunkPos();

	std::lock_guard<std::mutex> lock(chunksMutex);
	if (chunks.find(pos) != chunks.end()) {
		std::wcerr << L"\033[33m[WARNING]\033[0m [World2] 尝试添加已存在的区块，位置：" << pos.x << L", " << pos.y << L", " << pos.z << std::endl;
		delete chunk;
		return;
	}

	chunks[pos] = chunk;
}

void World::fillChunk(Chunk* chunk) {
	if (!chunk) return; // 空指针保护
	glm::ivec3 chunkPos = chunk->getChunkPos();
	const int chunkSize = Chunk::ChunkSize;
	const int MAX_HEIGHT = chunkSize; // 地形最大高度（0-15）
	const int MIN_HEIGHT = 0;  // 地形最小高度

	// 噪声缩放系数（越小地形越平滑，0.01是经验值，可微调）
	const float NOISE_SCALE = 0.01f;
	// 分层高度阈值（控制不同方块的分布）
	const int GRASS_LAYER = 1;   // 顶层1层草皮
	const int DIRT_LAYER = 3;    // 草皮下3层泥土

	// 预先生成高度图
	uint8_t heightMap[chunkSize][chunkSize] = { 0 };

	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {
			int gx = chunkPos.x * chunkSize + x;
			int gy = chunkPos.y * chunkSize + y;
			float n = noise.octaveNoise((float)gx * NOISE_SCALE, (float)gy * NOISE_SCALE, 2, 0.5f);
			int h = (int)((n + 1) / 2.0f * MAX_HEIGHT + 0.1f);
			heightMap[x][y] = glm::clamp(h, MIN_HEIGHT, MAX_HEIGHT);
		}
	}


	// 遍历Chunk的每个X/Y坐标
	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {


			// ========== 3. 生成分层方块柱（Z轴为高度） ==========
			for (int z = 0; z < heightMap[x][y]; z++) {
				// 边界检查
				if (x < 0 || x >= chunkSize || y < 0 || y >= chunkSize || z < 0 || z >= MAX_HEIGHT) {
					continue;
				}
				// 确定当前高度的方块类型（分层逻辑）
				std::string blockName;
				int heightFromTop = heightMap[x][y] - z - 1; // 距离顶部的高度

				if (heightFromTop < GRASS_LAYER) {
					// 顶层：草皮（grass_up）
					blockName = "grass_up";
				}
				else if (heightFromTop < GRASS_LAYER + DIRT_LAYER) {
					// 草皮下：泥土（需确保register_block中有dirt的定义）
					blockName = "bricks_mc";
				}
				else {
					// 底层：石头（stone）
					blockName = "stone";
				}

				// 设置方块（兼容你的RegisterBlock逻辑）
				chunk->setBlock(x, y, z, RegisterBlock::getInstance()->getBlockState_NAME(blockName.c_str()).ID);
			}
		}
	}
}

static constexpr int RANGE_XY = 8;
static constexpr int RANGE_Z = 4;

void World::updateRenderChunks() {
	const glm::ivec3 center = lastChunkPos;
	std::unordered_set<Chunk*> newRender;
	std::vector<glm::ivec3> needCreateChunks;


	{
		std::lock_guard<std::mutex> lock(chunksMutex);

		for (int x = -RANGE_XY; x <= RANGE_XY; ++x)
			for (int y = -RANGE_XY; y <= RANGE_XY; ++y)
				for (int z = -RANGE_Z; z <= RANGE_Z; ++z) {
					glm::ivec3 pos = center + glm::ivec3(x, y, z);
					auto it = chunks.find(pos);
					if (it != chunks.end() && it->second) {
						newRender.insert(it->second);
					}
					else {
						if (pos.z == -1)
							needCreateChunks.push_back(pos);
					}
				}
	}

	for (auto& pos : needCreateChunks) {
		addCreatChunk(pos);
	}

	// 3. 安全替换渲染列表
	{
		std::lock_guard<std::mutex> lock(renderMutex);
		renderChunks = std::move(newRender);
	}
}

void World::addRenderChunk(Chunk* chunk)
{
	if (chunk == nullptr) { return; }

	glm::ivec3 chunkPos = chunk->getChunkPos();

	glm::ivec3 delta = chunkPos - lastChunkPos;

	bool inRange = (abs(delta.x) <= RANGE_XY) && (abs(delta.y) <= RANGE_XY) && (abs(delta.z) <= RANGE_Z);

	// 在范围内才插入
	if (inRange) {
		std::lock_guard<std::mutex> lock(renderMutex);
		renderChunks.insert(chunk);
	}
}

void World::addCreatChunk(glm::ivec3 chunkPos)
{


	{
		std::lock_guard<std::mutex> lock(chunksMutex);
		if (chunks.find(chunkPos) != chunks.end()) {
			//std::wcerr << L"\033[33m[WARNING]\033[0m [World2] 队列添加的区块已存在，位置：" << chunkPos.x << L", " << chunkPos.y << L", " << chunkPos.z << std::endl;
			return;
		}
	}
	{
		std::unique_lock<std::mutex> lock(createMutex);
		if (chunkCreateSet.find(chunkPos) == chunkCreateSet.end()) {
			chunkCreateQueue.push(chunkPos);
			chunkCreateSet.insert(chunkPos);

			createCV.notify_one();
		}
		else {
			//std::wcerr << L"\033[33m[WARNING]\033[0m [World2] 队列添加的区块已在队列中，位置：" << chunkPos.x << L", " << chunkPos.y << L", " << chunkPos.z << std::endl;
			return;
		}
	}
}

void World::uploadChunkDataToGPU()
{
	std::lock_guard<std::mutex> lock(meshUpdateMutex);

	// 一次最多上传 4 个，避免卡顿
	int count = 0;
	while (!chunkMeshUpdateQueue.empty() && count < 16) {
		glm::ivec3 pos = chunkMeshUpdateQueue.front();


		Chunk* chunk = getChunk(pos);
		if (chunk) {
			std::lock_guard<std::mutex> chunkLock(chunk->chunkMutex);
			chunk->buildMeshVertex();
		}
		count++;
		chunkMeshUpdateQueue.pop();

		addRenderChunk(chunk);
	}
}

void World::threadGenerateChunks()
{
	while (!stopThread) {
		glm::ivec3 pos;

		{
			std::unique_lock<std::mutex> lock(createMutex);
			createCV.wait(lock, [this]() {
				return !chunkCreateQueue.empty() || stopThread;
				});
			if (stopThread) return;

			pos = chunkCreateQueue.front();
			chunkCreateQueue.pop();
		}



		Chunk* c = createChunk(pos);
		c = generateChunk(c);
		pos = c->getChunkPos();

		if (c) {
			addChunks(c);
			{
				std::lock_guard<std::mutex> lock(biuldMeshMutex);
				chunkBiuldMeshDataQueue.push({pos,1});
				BiuldMeshCV.notify_one();
			}
		}

		{
			std::unique_lock<std::mutex> lock(createMutex);
			chunkCreateSet.erase(pos);
		}

	}
}

void World::threadbuildMeshDataChunks()
{
	while (!stopThread) {
		glm::ivec3 pos;
		{
			std::unique_lock<std::mutex> lock(biuldMeshMutex);
			BiuldMeshCV.wait(lock, [this] {
				return !chunkBiuldMeshDataQueue.empty() || stopThread;
				});
			if (stopThread) return;


			pos = chunkBiuldMeshDataQueue.top().first;
			chunkBiuldMeshDataQueue.pop();
		}

		Chunk* c = getChunk(pos);

		if (c) {
			{
				std::lock_guard<std::mutex> chunkLock(c->chunkMutex);
				c->buildMeshData();
			}
			{
				std::lock_guard<std::mutex> lock(meshUpdateMutex);
				chunkMeshUpdateQueue.push(pos);
			}
		}
	}
}
void World::addchunkMeshUpdateQueue(glm::ivec3 pos) {
	std::lock_guard<std::mutex> lock(meshUpdateMutex);
	chunkMeshUpdateQueue.push(pos);
}