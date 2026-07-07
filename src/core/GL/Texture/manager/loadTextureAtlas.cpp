#include "loadTextureAtlas.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

LoadTextureAtlas* LoadTextureAtlas::instance = nullptr;

TextureAtlas* LoadTextureAtlas::loadTextureAtlas(const std::string& jsonPath)
{
	std::filesystem::path pathObj(jsonPath);
	// 1. 获取父目录（目录路径）
	std::filesystem::path dirPath = pathObj.parent_path();

	// 1. 打开JSON文件（使用std::ifstream）
	std::ifstream jsonFile(jsonPath);
	if (!jsonFile.is_open()) {
		throw std::runtime_error("无法打开JSON文件: " + jsonPath);
	}

	// 2. 解析JSON内容
	nlohmann::json jsonData;
	try {
		jsonFile >> jsonData;
	}
	catch (const nlohmann::json::parse_error& e) {
		throw std::runtime_error("JSON解析失败: " + std::string(e.what()) +
			" (位置: " + std::to_string(e.byte) + ")");
	}

	jsonFile.close();

	std::string imagePath = dirPath.string() + "/" + jsonData["image_path"].get<std::string>();


	glm::ivec2 tileCount = glm::ivec2(0, 0);
	glm::ivec2 tileSize = glm::ivec2(0, 0);

	if (jsonData.contains("tile_count") && jsonData.contains("tile_size")) {
		tileCount = glm::ivec2(jsonData["tile_count"][0], jsonData["tile_count"][1]);
		tileSize = glm::ivec2(jsonData["tile_size"][0], jsonData["tile_size"][1]);
	}

	std::string name;
	std::unordered_map<std::string, glm::ivec2> tilesPos;

	if (jsonData.contains("tiles")) {
		for (int i = 0; i < jsonData["tiles"].size(); i++) {
			const auto& tile = jsonData["tiles"][i];

			name = tile["name"].get<std::string>();
			tilesPos[name] = glm::ivec2(tile["tile_pos"][0], tile["tile_pos"][1]);
		}
	}
	glm::dvec4 uvOffsets = uvOffset(tileCount, tileSize);
	glm::dvec2 uvTileSize = glm::dvec2(uvOffsets.x, uvOffsets.y);
	glm::dvec2 uvPxSize = glm::dvec2(uvOffsets.z, uvOffsets.w);

	return new TextureAtlas(imagePath, tileCount, tileSize, tilesPos, uvTileSize, uvPxSize);
}

glm::dvec4 LoadTextureAtlas::uvOffset(glm::ivec2 tileCount, glm::ivec2 tileSize)
{
	int TexWidth = tileCount.x * tileSize.x;
	int TexHeight = tileCount.y * tileSize.y;

	// 计算单个像素对应的UV比例（基于总纹理尺寸）
	double uvPxW = 1.0 / static_cast<double>(TexWidth);
	double uvPxH = 1.0 / static_cast<double>(TexHeight);


	// 计算单个tile的有效UV宽高（仅tile本身，不含间隙）
	double uvTileWidth = static_cast<double>(tileSize.x) * uvPxW;
	double uvTileHeight = static_cast<double>(tileSize.y) * uvPxH;

	// 返回UV偏移量（tileSize, pxSize）
	return glm::dvec4(uvTileWidth, uvTileHeight,uvPxW, uvPxH);
}
