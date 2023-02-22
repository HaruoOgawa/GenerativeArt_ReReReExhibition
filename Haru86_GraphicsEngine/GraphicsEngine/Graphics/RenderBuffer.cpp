#include "RenderBuffer.h"
#include "Texture.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"

namespace graphic
{
	RenderBuffer::RenderBuffer(int width, int height, GLint internalformat, GLint format, GLenum type, ERenderTargetType RenderTargetType, EDepthTargetType DepthTargetType, bool UseMSAA):
		m_FrameBufferIndex(0),
		m_FrameTexture(std::make_shared<Texture>()),
		m_Width(width),
		m_Height(height)
	{
		CreateFrameBuffer(width, height, m_FrameTexture, m_FrameBufferIndex, internalformat, format, type, RenderTargetType, DepthTargetType, UseMSAA);
	}

	RenderBuffer::~RenderBuffer()
	{
		if(m_FrameBufferIndex != 0) glDeleteFramebuffers(1, &m_FrameBufferIndex);
	}

	bool RenderBuffer::CreateFrameBuffer(int width, int height, std::shared_ptr<Texture> fTex, unsigned int& fBuffer,
		GLint internalformat, GLint format, GLenum type, ERenderTargetType RenderTargetType, EDepthTargetType DepthTargetType, bool UseMSAA) {

		// フレームバッファ生成
		glGenFramebuffers(1, &fBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, fBuffer);

		// デプスバッファ生成
		if (DepthTargetType == EDepthTargetType::DEPTH_TEXTURE_BUFFER)
		{
			fTex->CreateForRendering(width, height, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fTex->GetTextureID(), 0);
		}
		else
		{
			GLuint depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

			// アンチエイリアシングを使用
			if (UseMSAA)
			{
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH_COMPONENT32F, width, height);
			}
			else
			{
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
			}

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		// カラーバッファ生成
		if (RenderTargetType == ERenderTargetType::COLOR_TEXTURE_BUFFER) // カラーテクスチャバッファ
		{
			fTex->CreateForRendering(width, height, internalformat, format, type);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fTex->GetTextureID(), 0);
		}
		else if (RenderTargetType == ERenderTargetType::COLOR_RENDER_BUFFER) // カラーレンダーバッファ
		{
			GLuint colorRenderBuffer;
			glGenRenderbuffers(1, &colorRenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);

			// アンチエイリアシングを使用
			if (UseMSAA)
			{
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, internalformat, width, height);
			}
			else
			{
				glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
			}

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		else {
			return false;
		}

		// バインド解除
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;

	}

	void RenderBuffer::CopyFrameBuffer(unsigned int ReadBuffer, unsigned int DrawBuffer, int width, int height)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawBuffer);

		glBlitFramebuffer(
			0, 0, width, height,
			0, 0, width, height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST
		);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void RenderBuffer::Draw(const glm::vec4& InitColor, std::function<void(void)> callback, bool IsClear)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferIndex);
		glViewport(0, 0, m_Width,m_Height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (IsClear)
		{
			glClearColor(InitColor.r, InitColor.g, InitColor.b, InitColor.a);
		}

		callback();

		glBindFramebuffer(GL_FRAMEBUFFER, GraphicsMain::GetInstance()->m_TargetFrameIndex);
		glViewport(0, 0, m_Width, m_Height);
	}
}