#include "TextureCubemap.h"

#include "stb_image.h"
#include <iostream>
TextureCubemap::TextureCubemap(const std::array<std::string, 6>& facePaths, TextureType type)
{
	spec.width = -1;
	spec.height = -1;

	stbi_set_flip_vertically_on_load(true);

	std::array<unsigned char*, 6> data;

	for (int i = 0; i < 6; ++i) {
		data[i] = stbi_load(facePaths[i].c_str(), &spec.width, &spec.height, &nrChannels, 0);
		if (!data[i]) {
			std::wcerr << L"\033[31m[ERROR]\033[0m 立方体纹理加载失败！" << std::endl;
			std::wcerr << L"[->]文件路径: " << facePaths[i].c_str() << std::endl;
			std::wcerr << L"[->]失败原因: " << stbi_failure_reason() << std::endl;
			spec.width = -1;
			spec.height = -1;
			nrChannels = -1;
			return;
		}
	}
	loadToGPU(data);
	for (int i = 0; i < 6; ++i) {
		stbi_image_free(data[i]);
	}
}

TextureCubemap::~TextureCubemap()
{
}

void TextureCubemap::loadToGPU(std::array<unsigned char*, 6> data)
{
	if (isLoadGPU) {
		std::cout << "[INFO] [TextureCubemap]纹理已加载到GPU" << std::endl;
		return;
	}
	Texture::bind();
	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, spec.minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, spec.magFilter);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, spec.wrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, spec.wrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, spec.wrapT); // R轴也设置为T轴的包裹模式
	// 加载每个面
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, spec.internalFormat,
			spec.width, spec.height, 0, spec.format, spec.type, data[i]);
	}
	Texture::unbind();
	isLoadGPU = true;
}
