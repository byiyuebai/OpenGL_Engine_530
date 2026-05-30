#pragma once
#include <string>
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

	GLenum internalFormat = GL_RGBA;
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
public:
	Texture();
	Texture(const std::string path, TextureType type= TextureType::opaque);

	virtual ~Texture();

	void bind();
	static void unbind();

	void activeTex(unsigned int unit);

	void setID(unsigned int id);  // 别随便设置ID，除非你知道自己在干什么

	unsigned char* getData() { return data; }

	virtual void loadToGPU();

	void createEmptyTexture(TextureSpec spec,bool hasownData = true);

	void Clean();

	unsigned int getID() const { return textureID; }

	glm::ivec2 getSize() const { return glm::ivec2(spec.width, spec.height); }

	std::string getPath() const { return path; }

	TextureType getType() const { return type; }

	TextureSpec getSpec() const { return spec; }

	// 新增：检查纹理是否有效
	bool isValid() const { return isLoadGPU && textureID != 0; }


protected:
	unsigned int textureID = 0;
	std::string path;
	unsigned char* data = nullptr;
	int nrChannels = -1;
	bool isLoadGPU = false;
	TextureType type = TextureType::opaque;

	TextureSpec spec; // 纹理规格

	bool hasownData = false; // 是否拥有所有权
};