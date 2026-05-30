#include "Texture.h"
#include "stb_image.h"
#include <iostream>
#include <vector>

Texture::Texture()
	: textureID(0)
	, path("")
	, data(nullptr)
	, nrChannels(-1)
	, isLoadGPU(false)
	, type(TextureType::opaque)
{
	spec.width = -1;
	spec.height = -1;

	glGenTextures(1, &textureID);
}

Texture::Texture(const std::string path, TextureType type)
	: textureID(0)
	, path(path)
	, data(nullptr)
	, nrChannels(-1)
	, isLoadGPU(false)
	, type(type)
{
	spec.width = -1;
	spec.height = -1;

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path.c_str(), &spec.width, &spec.height, &nrChannels, 0);

	// 检查加载是否失败
	if (!data) {
		std::wcerr << L"\033[31m[ERROR]\033[0m 纹理加载失败！" << std::endl;
		std::wcerr << L"[->]文件路径: " << path.c_str() << std::endl;
		std::wcerr << L"[->]失败原因: " << stbi_failure_reason() << std::endl;
		spec.width = -1;
		spec.height = -1;
		nrChannels = -1;
		return;
	}
	glGenTextures(1, &textureID);
	loadToGPU();
}

Texture::~Texture() {
	Clean();
}

void Texture::loadToGPU()
{
	if (!data || isLoadGPU) {
		return;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);

	/*环绕模式
	GL_TEXTURE_WRAP_S S 轴（X 轴）
	GL_TEXTURE_WRAP_T T 轴（Y 轴）

	- GL_REPEAT（重复，默认）
	- GL_MIRRORED_REPEAT（镜像重复）
	- GL_CLAMP_TO_EDGE（夹紧到边缘）
	- GL_CLAMP_TO_BORDER（夹紧到边界颜色）
	- GL_MIRROR_CLAMP_TO_EDGE（OpenGL 4.4+）*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/*mipmap
	GL_TEXTURE_BASE_LEVEL	mipmap 基础层级（起始层）	非负整数（默认 0）
	GL_TEXTURE_MAX_LEVEL	mipmap 最大层级（终止层）	非负整数（默认 1000）
	GL_TEXTURE_LOD_BIAS	mipmap 层级偏移（需用 glTexParameterf）	浮点值（默认 0.0）
	GL_TEXTURE_MIN_LOD	最小 LOD（层级细节）	浮点值（需用 glTexParameterf，默认 -1000）
	GL_TEXTURE_MAX_LOD	最大 LOD	浮点值（需用 glTexParameterf，默认 1000）*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

	/*纹理过滤
	- GL_NEAREST（最近点过滤）
	- GL_LINEAR（线性过滤）
	- GL_NEAREST_MIPMAP_NEAREST（最近点 mipmap 过滤）
	- GL_LINEAR_MIPMAP_NEAREST（线性 mipmap 过滤）
	- GL_NEAREST_MIPMAP_LINEAR（最近点 mipmap 线性过滤）
	- GL_LINEAR_MIPMAP_LINEAR（线性 mipmap 线性过滤）
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//// 各向异性过滤（保留，不影响核心问题）
	//GLfloat maxAnisotropy;
	//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
	//GLfloat anisotropyLevel = 8.0f;
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, std::min(anisotropyLevel, maxAnisotropy));

	// 格式设置（保留）
	GLenum format = GL_RGB;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	GLenum internalFormat = GL_RGB8;
	if (type == TextureType::translucent) {
		internalFormat = GL_RGBA8;
	}
	// 上传图像数据
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, spec.width, spec.height, 0, format, GL_UNSIGNED_BYTE, data);
	// 自动生成 mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	isLoadGPU = true;

	hasownData = true;// 默认情况下，Texture对象拥有自己的数据
}

void Texture::createEmptyTexture(TextureSpec spec,bool hasownData)
{
	this->spec = spec;

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

	glBindTexture(GL_TEXTURE_2D, 0);

	isLoadGPU = true;

	this->hasownData = hasownData;
}

void Texture::Clean() {

	if (isLoadGPU && textureID != 0) {
		glDeleteTextures(1, &textureID);
	}

	// 释放CPU端数据
	if (data) {
		stbi_image_free(data);
		// 注意：data是mutable的，但Clean是const
		// 如果编译器报错，需要把data声明为mutable
	}
}

void Texture::bind()
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

void Texture::setID(unsigned int id)
{
	// 修复：先清理旧纹理
	if (isLoadGPU && textureID != 0) {
		glDeleteTextures(1, &textureID);
	}

	textureID = id;
	isLoadGPU = true;
}