#pragma once
#include <glad/glad.h>
#include <vector>

#include "glm/glm.hpp"


class Texture;

class FrameBuffer
{
private:
	unsigned int FBO_ID;
	int m_Width, m_Height;

	// 用 Texture 对象管理颜色附件
	std::vector<Texture*> m_ColorTextures;

	// 深度附件
	Texture* m_DepthTexture;
	bool m_HasDepthTexture;

	// 深度 RBO（如果使用 RBO 形式的深度附件）
	unsigned int m_DepthRBO;
	bool m_HasDepthRBO;

	std::vector<GLenum> m_DrawBuffers;
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	// 添加颜色附件，返回 Texture 指针（可以直接当普通纹理使用）
	Texture* AddColorAttachment(GLenum internalFormat, GLenum format, GLenum type);

	// 添加深度附件（基于 Texture 类）
	Texture* AddDepthTexture();

	// 添加深度附件（RBO形式，不可采样但性能更好）
	void AddDepthRenderBuffer();

	// 外部传入的深度纹理（用于共享不透明FBO的深度）
	void AttachExternalDepthTexture(Texture* depthTexture);

	// MRT设置
	void SetDrawBuffer(std::vector<GLenum> drawBuffers);

	void Bind() const;
	void Unbind() const;

	// 获取 Texture 指针
	Texture* GetColorTexture(int index = 0) const;
	Texture* GetDepthTexture() const;

	unsigned int getID() const { return FBO_ID; }

	// 调整大小（重建所有附件）
	void Resize(int width, int height);
	glm::ivec2 getSize() const { return glm::ivec2(m_Width, m_Height); }



	void Clean();
};