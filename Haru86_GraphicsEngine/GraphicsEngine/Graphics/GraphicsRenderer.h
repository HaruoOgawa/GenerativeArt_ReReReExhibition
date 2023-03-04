#pragma once
#include "../GraphicsMain/GraphicsMain.h"
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <functional>
#include "../Component/TransformComponent.h"

enum class ERenderTargetType {
	COLOR_TEXTURE_BUFFER,
	COLOR_RENDER_BUFFER,
};

enum class EDepthTargetType {
	DEPTH_RENDER_BUFFER,
	DEPTH_TEXTURE_BUFFER,
};

class Texture;

class GraphicsRenderer
{
	GLFWwindow* sWindow;
	float sWindowWidth;
	float sWindowHeight;
	class GraphicsMain* mgame;

	//FrameBuffer
	unsigned int polygon_frameBuffer;
	unsigned int polygon_frameBuffer_MSAA;
	unsigned int raymarching_frameBuffer;
	unsigned int p_r_BlendingBuffer;
	unsigned int p_r_DepthBlendingBuffer;
	//PostProcess FrameBuffer
	unsigned int m_PolygonPostProcess_FrameBuffer;
	unsigned int m_LatePostProcess_FrameBuffer;

	//ポリゴンとレイマーチングのミックス
	std::unique_ptr<class PolygonRaymarchingMixer> m_Mixer;

	// 背景色
	glm::vec4 m_BackgroudColor;
public:
	static GraphicsRenderer* GetInstance() {
		return renderer_instance;
	}
	static void Create();
	static void Destroy();
#ifdef _DEBUG
	static int CheckError();
	static int CheckFrameBufferError();
#endif // _DEBUG

	GraphicsRenderer(class GraphicsMain* game);
	~GraphicsRenderer();
	
	bool Initialize(float width, float height);
	void Draw(const std::shared_ptr<TransformComponent>& UsingCamera,bool IsDrawRay,int ResultFrameBufferIndex,std::function<void(void)> callback,int width,int height);
	
	bool CreateFrameBuffer(int width, int height,std::shared_ptr<Texture> fTex,unsigned int& fBuffer, GLint internalformat, GLint format, 
		GLenum type = GL_UNSIGNED_BYTE, 
		ERenderTargetType RenderTargetType = ERenderTargetType::COLOR_TEXTURE_BUFFER, 
		EDepthTargetType DepthTargetType = EDepthTargetType::DEPTH_RENDER_BUFFER,bool UseMSAA=false);
	void CopyFrameBuffer(unsigned int ReadBuffer, unsigned int DrawBuffer, int width, int height);

	glm::vec2 GetScreenSize();
	GLFWwindow* GetWindow();
	
	float deltaTime;
	float frameResolusion;

	//FrameTextures
	std::shared_ptr<Texture> polygon_frameTexture;
	std::shared_ptr<Texture> polygon_ShadowTexture;
	std::shared_ptr<Texture> raymarching_frameTexture;
	std::shared_ptr<Texture> p_r_BlendingTexture;
	std::shared_ptr<Texture> p_r_DepthBlendingTexture;
	//PostProcess FrameTexture
	std::shared_ptr<Texture> m_PolygonPostProcess_FrameTexture;
	std::shared_ptr<Texture> m_LatePostProcess_FrameTexture;

	// 背景色
	void SetBackgroudColor(glm::vec4 BackgroudColor);

	friend class PolygonRaymarchingMixer;
private:

protected:
	static GraphicsRenderer* renderer_instance;
};