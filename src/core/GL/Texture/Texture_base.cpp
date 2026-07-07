// Texture.cpp
#include "Texture_base.h"
#include <iostream>

Texture::Texture() {
    glGenTextures(1, &textureID);
}

Texture::Texture(const TextureSpec& spec) : spec(spec) {
    glGenTextures(1, &textureID);
}

Texture::~Texture() {
	Clean();
}

void Texture::Clean()
{
	if (isLoadGPU && textureID != 0) {
		glDeleteTextures(1, &textureID);
	}
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::activeTex(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
}

void Texture::loadToGPU()
{
	bind();

	// mipmap设置只有基础层
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	// 设置默认参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, spec.magFilter);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, spec.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, spec.wrapT);

	glTexImage2D(GL_TEXTURE_2D, 0, spec.internalFormat, spec.width, spec.height, 0, spec.format, spec.type, nullptr);

	unbind();

	isLoadGPU = true;
}
