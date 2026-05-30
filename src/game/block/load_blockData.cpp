#include "load_blockData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "register_block.h"
#include "texture_manager.h"
void LoadBlockData::loadBlockDataFromFile(const std::string& jsonPath) {

	RegisterBlock* registerBlock = RegisterBlock::getInstance();

	std::string name;
	glm::ivec2 tilePos = glm::ivec2(0, 0);

	TextureAtlas* textureAtlas = TextureManager::getInstance()->loadTextureAtlas(jsonPath);

	for (auto& pair : textureAtlas->getTilesPos()) {
		name = pair.first;
		tilePos = pair.second;

		BlockState blockState = registerBlock->getBlockState_NAME(name);
		if (blockState.data != nullptr&& blockState != registerBlock->getBlockState_NAME("error")) {
			blockState.data->tex = textureAtlas;
			std::vector<glm::dvec2> uvCoords = LoadBlockData::uvOffset(tilePos, textureAtlas->getUvTileSize(), textureAtlas->getUvPxSize());
			for (int i = 0; i < 4; ++i) {
				blockState.data->uvOffset[i] = uvCoords[i];
			}
			//std::cout << uvCoords[0].x << ", " << uvCoords[0].y << "|" << uvCoords[1].x << ", " << uvCoords[1].y << "|" << uvCoords[2].x << ", " << uvCoords[2].y << "|" << uvCoords[3].x << ", " << uvCoords[3].y << std::endl;
		}
		else {
			blockState.data->tex = textureAtlas;
			std::vector<glm::dvec2> uvCoords = LoadBlockData::uvOffset(glm::ivec2(0,0), textureAtlas->getUvTileSize(), textureAtlas->getUvPxSize());
			for (int i = 0; i < 4; ++i) {
				blockState.data->uvOffset[i] = uvCoords[i];
			}
		}
	}
}

std::vector<glm::dvec2> LoadBlockData::uvOffset(glm::ivec2 tilePos, glm::dvec2 uvTileSize, glm::dvec2 uvPxSize) {

	double startU = tilePos.x * uvTileSize.x;
	double startV = 1.0 - (tilePos.y + 1.0) * uvTileSize.y;

	// 四个 UV 全部向内缩进，避免落在边界上
	double halfPixelU = uvPxSize.x * 0.0001;
	double halfPixelV = uvPxSize.y * 0.0001;


	std::vector<glm::dvec2> uvCoords;
	uvCoords.reserve(4);

	// 左上
	uvCoords.emplace_back(startU + halfPixelU, startV + uvTileSize.y - halfPixelV);
	// 左下
	uvCoords.emplace_back(startU + halfPixelU, startV + halfPixelV);
	// 右下
	uvCoords.emplace_back(startU + uvTileSize.x - halfPixelU, startV + halfPixelV);
	// 右上
	uvCoords.emplace_back(startU + uvTileSize.x - halfPixelU, startV + uvTileSize.y - halfPixelV);

	return uvCoords;
}