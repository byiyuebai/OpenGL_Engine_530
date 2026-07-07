#pragma once

#include "Texture_base.h"
#include "AtlasMipmap.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
class TextureAtlas :public Texture {
private:
	glm::ivec2 tileCount;
	glm::ivec2 tileSize;
	std::unordered_map<std::string, glm::ivec2> tilesPos;
	glm::dvec2 uvTileSize; // 每个tile的UV尺寸
	glm::dvec2 uvPxSize;   // 每个像素的UV尺寸
	int mipLevels = 4; // mipmap层级数量

	AtlasMipmap* mipmap = nullptr;
public:
	TextureAtlas(const std::string& path, glm::ivec2 tileCount, glm::ivec2 tileSize, std::unordered_map<std::string, glm::ivec2> tilesPos, glm::dvec2 uvTileSize, glm::dvec2 uvPxSize);
	~TextureAtlas();
	void loadToGPU();
	std::unordered_map<std::string, glm::ivec2> getTilesPos() const { return tilesPos; }

	glm::ivec2 getTileCount() const { return tileCount; }
	glm::ivec2 getTileSize() const { return tileSize; }
	glm::dvec2 getUvTileSize() const { return uvTileSize; }
	glm::dvec2 getUvPxSize() const { return uvPxSize; }

	AtlasMipmap* getMipmap() const { return mipmap; }
protected:
	std::string path;
	TextureType type = TextureType::opaque;
};