#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

enum class TextureType
{
	opaque = 0,
	translucent = 1,
};

struct TextureSpec
{
	int width = -1;
	int height = -1;

	GLenum internalFormat = GL_RGBA8;
	GLenum format = GL_RGBA;

	GLenum type = GL_UNSIGNED_BYTE;

	GLenum minFilter = GL_NEAREST;
	GLenum magFilter = GL_NEAREST;

	GLenum wrapS = GL_REPEAT;
	GLenum wrapT = GL_REPEAT;

	GLenum baseLevel = 0;
	GLenum maxLevel = 4;
};

class Texture {
protected:
	unsigned int textureID = 0;

	TextureSpec spec; // 纹理规格
	int nrChannels = -1;

	bool isLoadGPU = false;
public:
	Texture();
	Texture(const TextureSpec& spec);
	virtual ~Texture();
	void Clean();

	// 绑定/激活
	void bind() const;
	static void unbind();
	static void activeTex(unsigned int unit);

	unsigned int getID() const { return textureID; }
	const TextureSpec& getSpec() const { return spec; }
	glm::ivec2 getSize() const { return { spec.width, spec.height }; }

	bool isValid() const { return isLoadGPU && textureID != 0; }

	void setSpec(const TextureSpec& newSpec) { spec = newSpec; }

	// GPU操作
	virtual void loadToGPU();

};