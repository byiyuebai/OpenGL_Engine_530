#include "FrameBuffer.h"

#include "Texture/Texture_base.h"
#include <iostream>

FrameBuffer::FrameBuffer(int width, int height) : m_Width(width), m_Height(height), m_DepthTexture(nullptr), m_DepthRBO(0), m_HasDepthTexture(false), m_HasDepthRBO(false)
{
	glGenFramebuffers(1, &FBO_ID);
}

FrameBuffer::~FrameBuffer()
{
	Clean();
}

Texture* FrameBuffer::AddColorAttachment(GLenum internalFormat, GLenum format, GLenum type)
{
	Bind();

	// 创建一个 Texture 对象包装这个纹理ID
	Texture* tex = new Texture();

	TextureSpec spec;

	spec.width = m_Width;
	spec.height = m_Height;
	spec.internalFormat = internalFormat;
	spec.format = format;
	spec.type = type;

	spec.minFilter = GL_LINEAR;
	spec.magFilter = GL_LINEAR;

	spec.wrapS = GL_CLAMP_TO_EDGE;
	spec.wrapT = GL_CLAMP_TO_EDGE;

	tex->setSpec(spec);
	tex->loadToGPU();

	// 附加到 FBO
	GLenum attachment = GL_COLOR_ATTACHMENT0 + m_ColorTextures.size();
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->getID(), 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	Unbind();


	m_ColorTextures.push_back(tex);
	return tex;
}

Texture* FrameBuffer::AddDepthTexture()
{
	if (m_HasDepthTexture || m_HasDepthRBO)
	{
		std::wcerr << L"\033[31m[ERROR]\033[0m [AddDepthTexture] FBO已绑定深度附件！" << std::endl;
		return nullptr;
	}

	Bind();

	m_DepthTexture = new Texture();

	TextureSpec spec;
	spec.width = m_Width;
	spec.height = m_Height;
	spec.internalFormat = GL_DEPTH_COMPONENT24;
	spec.format = GL_DEPTH_COMPONENT;
	spec.type = GL_FLOAT;

	spec.minFilter = GL_NEAREST;
	spec.magFilter = GL_NEAREST;
	spec.wrapS = GL_CLAMP_TO_EDGE;
	spec.wrapT = GL_CLAMP_TO_EDGE;
	// 创建空纹理
	m_DepthTexture->setSpec(spec);
	m_DepthTexture->loadToGPU();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->getID(), 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	Unbind();



	m_HasDepthTexture = true;
	return m_DepthTexture;
}

void FrameBuffer::AddDepthRenderBuffer()
{
	if (m_HasDepthTexture || m_HasDepthRBO)
	{
		std::wcerr << L"\033[31m[ERROR]\033[0m [AddDepthRenderBuffer] FBO已绑定深度附件！" << std::endl;
		return;
	}

	Bind();

	glGenRenderbuffers(1, &m_DepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRBO);

	m_HasDepthRBO = true;

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	Unbind();
}

void FrameBuffer::AttachExternalDepthTexture(Texture* depthTexture)
{
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getID(), 0);
	Unbind();
}

void FrameBuffer::SetDrawBuffer(std::vector<GLenum> drawBuffers)
{
	m_DrawBuffers = drawBuffers;
	Bind();

	glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

	Unbind();
}

void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
}

void FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture* FrameBuffer::GetColorTexture(int index) const
{
	if (index >= 0 && index < static_cast<int>(m_ColorTextures.size()))
		return m_ColorTextures[index];

	std::wcerr << L"\033[31m[ERROR]\033[0m [FrameBuffer][GetColorTexture] 颜色附件索引越界: " << index << std::endl;
	return nullptr;
}

Texture* FrameBuffer::GetDepthTexture() const
{
	if (m_HasDepthTexture) { return m_DepthTexture; }
	else
	{
		std::wcerr << L"\033[31m[ERROR]\033[0m [FrameBuffer][GetDepthTexture] FBO未绑定深度纹理！" << std::endl;
		return nullptr;
	}

}

void FrameBuffer::Resize(int width, int height)
{

	m_Width = width;
	m_Height = height;

	Bind();

	int count = 0;
	// 重新创建颜色纹理
	for (auto& tex : m_ColorTextures)
	{
		if (tex != nullptr)
		{
			TextureSpec spec;

			spec.width = m_Width;
			spec.height = m_Height;
			spec.internalFormat = tex->getSpec().internalFormat;
			spec.format = tex->getSpec().format;
			spec.type = tex->getSpec().type;

			spec.minFilter = tex->getSpec().minFilter;
			spec.magFilter = tex->getSpec().magFilter;
			spec.wrapS = tex->getSpec().wrapS;
			spec.wrapT = tex->getSpec().wrapT;

			tex->setSpec(spec);
			tex->loadToGPU();

			// 重新附加到 FBO
			GLenum attachment = GL_COLOR_ATTACHMENT0 + count++;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->getID(), 0);
		}
	}

	// 重新创建深度纹理
	if (m_HasDepthTexture && m_DepthTexture != nullptr)
	{
		TextureSpec spec;
		spec.width = m_Width;
		spec.height = m_Height;
		spec.internalFormat = m_DepthTexture->getSpec().internalFormat;
		spec.format = m_DepthTexture->getSpec().format;
		spec.type = m_DepthTexture->getSpec().type;

		spec.minFilter = m_DepthTexture->getSpec().minFilter;
		spec.magFilter = m_DepthTexture->getSpec().magFilter;
		spec.wrapS = m_DepthTexture->getSpec().wrapS;
		spec.wrapT = m_DepthTexture->getSpec().wrapT;
		// 创建空纹理
		m_DepthTexture->setSpec(spec);
		m_DepthTexture->loadToGPU();

		// 重新附加深度纹理
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->getID(), 0);
	}

	// 重新创建深度 RBO
	if (m_HasDepthRBO && m_DepthRBO != 0)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}

void FrameBuffer::Clean()
{
	// 删除颜色纹理对象
	for (auto& tex : m_ColorTextures)
	{
		if (tex != nullptr)
		{
			delete tex;
		}
	}
	m_ColorTextures.clear();

	// 删除深度纹理对象
	if (m_DepthTexture != nullptr)
	{
		delete m_DepthTexture;
		m_DepthTexture = nullptr;
		m_HasDepthTexture = false;
	}

	// 删除深度 RBO
	if (m_DepthRBO != 0)
	{
		glDeleteRenderbuffers(1, &m_DepthRBO);
		m_DepthRBO = 0;
		m_HasDepthRBO = false;
	}

	// 删除 FBO
	if (FBO_ID != 0)
	{
		glDeleteFramebuffers(1, &FBO_ID);
		FBO_ID = 0;
	}
}