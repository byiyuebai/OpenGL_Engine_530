#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Texture_base.h"



class ImageTexture : public Texture {
public:
	ImageTexture(const std::string path, TextureType type= TextureType::opaque);

	~ImageTexture();


	std::string getPath() const { return path; }
	TextureType getType() const { return type; }

	void loadToGPU(unsigned char* data);
protected:
	std::string path;
	TextureType type = TextureType::opaque;
};