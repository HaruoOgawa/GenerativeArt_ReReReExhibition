#pragma once
#include <memory>
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <functional>

class Texture;

namespace graphic
{
	enum class ERenderTargetType {
		COLOR_TEXTURE_BUFFER,
		COLOR_RENDER_BUFFER,
	};

	enum class EDepthTargetType {
		DEPTH_RENDER_BUFFER,
		DEPTH_TEXTURE_BUFFER,
	};

	class RenderBuffer
	{
		unsigned int m_FrameBufferIndex;
		std::shared_ptr<Texture> m_FrameTexture;

		int m_Width;
		int m_Height;
	private:
		bool CreateFrameBuffer(int width, int height, std::shared_ptr<Texture> fTex, unsigned int& fBuffer, GLint internalformat, GLint format,
			GLenum type, ERenderTargetType RenderTargetType, EDepthTargetType DepthTargetType, bool UseMSAA);
	public:
		RenderBuffer(int width, int height, GLint internalformat, GLint format, GLenum type = GL_UNSIGNED_BYTE,
			ERenderTargetType RenderTargetType = ERenderTargetType::COLOR_TEXTURE_BUFFER,
			EDepthTargetType DepthTargetType = EDepthTargetType::DEPTH_RENDER_BUFFER, bool UseMSAA = false);
		virtual ~RenderBuffer();

		void CopyFrameBuffer(unsigned int ReadBuffer, unsigned int DrawBuffer, int width, int height);

		unsigned int GetFrameBufferIndex()const { return m_FrameBufferIndex; }
		const std::shared_ptr<Texture>& GetFrameTexture()const { return m_FrameTexture; }

		void Draw(const glm::vec4& InitColor, std::function<void(void)> callback, bool IsClear);
	};
}