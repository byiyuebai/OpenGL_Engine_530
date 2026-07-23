#pragma once

#include "Texture_base.h"

#include <array>
#include <string>
class TextureCubemap : public Texture {
public:
	TextureCubemap(const std::array<std::string, 6>& facePaths, TextureType type = TextureType::opaque);

	~TextureCubemap();

	void loadToGPU(std::array<unsigned char*, 6> data);

	std::array<std::string, 6> getFacePaths() const { return facePaths; }
	TextureType getType() const { return type; }

private:
	std::array<std::string, 6> facePaths;
	TextureType type = TextureType::opaque;
};
/*
传入路径为 上下左右前后 的顺序
*/