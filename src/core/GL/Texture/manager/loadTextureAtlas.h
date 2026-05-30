#pragma once
#include <string>
#include "TextureAtlas.h"

class LoadTextureAtlas {
private:
	static LoadTextureAtlas* instance;
public:
	static LoadTextureAtlas* getInstance() {
		if (instance == nullptr) {
			instance = new LoadTextureAtlas();
		}
		return instance;
	}

	LoadTextureAtlas(const LoadTextureAtlas&) = delete;
	LoadTextureAtlas& operator=(const LoadTextureAtlas&) = delete;
	LoadTextureAtlas(LoadTextureAtlas&&) = delete;
	LoadTextureAtlas& operator=(LoadTextureAtlas&&) = delete;

	LoadTextureAtlas() {}
	~LoadTextureAtlas() {}

	TextureAtlas* loadTextureAtlas(const std::string& jsonPath);
	glm::dvec4 uvOffset(glm::ivec2 tileCount, glm::ivec2 tileSize);

};
