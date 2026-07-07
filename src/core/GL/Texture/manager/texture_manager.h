#pragma once
#include <memory>       // 用于 std::unique_ptr
#include <stdexcept>    // 用于 std::runtime_error
#include <string>       // 用于 std::string
#include <unordered_map> // 用于 std::unordered_map
#include <glm/glm.hpp>
#include <mutex>        // 用于 std::mutex
#include "imageTexture.h"
#include "TextureAtlas.h"

class TextureManager final {
private:

	static TextureManager* instance;
	static std::mutex mtx;

	// 存储文件路径和指向管理纹理的指针映射。
	std::unordered_map<std::string, Texture*> textures_image;
	std::unordered_map<std::string, Texture*> textures_atlas;

public:
	TextureManager() = default;


	Texture* loadTexture(const std::string& file_path, TextureType type = TextureType::opaque);
	TextureAtlas* loadTextureAtlas(const std::string& file_path);

	void unloadTexture(const std::string& file_path);

	void clearTextures();
	static TextureManager* getInstance() {
		if (instance == nullptr) {
			std::lock_guard<std::mutex> lock(mtx);
			if (instance == nullptr) {
				instance = new TextureManager();
			}
		}
		return instance;
	}

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator=(TextureManager&&) = delete;
};
