#include "GraphicsRenderer.h"

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

#include "../Component/MeshRendererComponent.h"

#include "../GraphicsMain/GraphicsMain.h"
#include "Texture.h"
#include "GraphicsEngine/Graphics/PolygonRaymarchingMixer.h"
#include "GraphicsEngine/Graphics/PostProcess.h"
#include "Assets/App/ArtReReRe/ArtReReRe.h"

GraphicsRenderer* GraphicsRenderer::renderer_instance = nullptr;

void GraphicsRenderer::Create() {
	if (!renderer_instance) {
		renderer_instance = new GraphicsRenderer(GraphicsMain::GetInstance());
	}
}

void GraphicsRenderer::Destroy() {
	delete renderer_instance;
	renderer_instance = nullptr;
}

#ifdef _DEBUG
int GraphicsRenderer::CheckError() {
	int Error = 0;
#ifdef _DEBUG
	// https://qiita.com/_ydah/items/da56763e94ba58af3d91
	GLenum err;
	err = glGetError();

	switch (err)
	{
	case 0: // 0x0000
		Console::Log("ErrorCode %d / GL_NO_ERROR\n", err);
		break;

	case 1280: // 0x0500
		Console::Log("ErrorCode %d / GL_INVALID_ENUM\n", err);
		break;

	case 1281: // 0x0501
		Console::Log("ErrorCode %d / GL_INVALID_VALUE\n", err);
		break;

	case 1282: // 0x0502
		Console::Log("ErrorCode %d / GL_INVALID_OPERATION\n", err);
		break;

	case 1283: // 0x0503
		Console::Log("ErrorCode %d / GL_STACK_OVERFLOW\n", err);
		break;

	case 1284: // 0x0504
		Console::Log("ErrorCode %d / GL_STACK_UNDERFLOW\n", err);
		break;

	case 1285: // 0x0505
		Console::Log("ErrorCode %d / GL_OUT_OF_MEMORY\n", err);
		break;

	case 1286: // 0x0506
		Console::Log("ErrorCode %d / GL_INVALID_FRAMEBUFFER_OPERATION\n", err);
		CheckFrameBufferError();
		break;

	case 1287: // 0x0507
		Console::Log("ErrorCode %d / GL_CONTEXT_LOST\n", err);
		break;

	default: // 0x????
		Console::Log("ErrorCode %d / Unknown Error\n", err);
		break;
	}

	Error = static_cast<int>(err);
	//Console::Log("CheckError GLErr: %d\n", err);
#endif // _DEBUG

	return Error;
}

int GraphicsRenderer::CheckFrameBufferError() {
	int Error = 0;
#ifdef _DEBUG
	GLenum err;
	err = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	switch (err)
	{
	case GL_FRAMEBUFFER_COMPLETE: // 0x8CD5
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_COMPLETE\n", err);
		break;

	case GL_FRAMEBUFFER_UNDEFINED: // 0x8219
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_UNDEFINED \n", err);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: // 0x8CD6
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT \n", err);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: // 0x8CD7
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT \n", err);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: // 0x8CDB
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER \n", err);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: // 0x8CDC
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER \n", err);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: // 0x8D56
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE \n", err);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: // 0x8DA8
		Console::Log("ErrorCode %d / GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS \n", err);
		break;

	default: // 0x????
		Console::Log("ErrorCode %d / Unknown Error\n", err);
		break;
	}

	Error = static_cast<int>(err);
#endif // _DEBUG

	return Error;
}

#endif // _DEBUG


GraphicsRenderer::GraphicsRenderer(GraphicsMain* game)
	: mgame(game),
	sWindow(nullptr),
	sWindowWidth(1920),
	sWindowHeight(1024),
	deltaTime(0.0f),
	frameResolusion(0.7),
	polygon_frameTexture(std::make_shared<Texture>()),
	raymarching_frameTexture(std::make_shared<Texture>()),
	polygon_ShadowTexture(std::make_shared<Texture>()),
	p_r_BlendingTexture(std::make_shared<Texture>()),
	p_r_DepthBlendingTexture(std::make_shared<Texture>()),
	m_PolygonPostProcess_FrameTexture(std::make_shared<Texture>()),
	m_LatePostProcess_FrameTexture(std::make_shared<Texture>()),
	m_BackgroudColor(glm::vec4(0.0f,0.0f,0.0f,0.0f)),
	polygon_frameBuffer(0),
	raymarching_frameBuffer(0),
	p_r_BlendingBuffer(0),
	p_r_DepthBlendingBuffer(0),
	m_PolygonPostProcess_FrameBuffer(0),
	m_LatePostProcess_FrameBuffer(0)
{
}

GraphicsRenderer::~GraphicsRenderer() {
	PostProcess::DestroyInstance();
	if (polygon_frameBuffer != 0)glDeleteFramebuffers(1, &polygon_frameBuffer);
	if (polygon_frameBuffer_MSAA != 0)glDeleteFramebuffers(1, &polygon_frameBuffer_MSAA);
	if (raymarching_frameBuffer != 0)glDeleteFramebuffers(1, &raymarching_frameBuffer);
	if (p_r_BlendingBuffer != 0)glDeleteFramebuffers(1, &p_r_BlendingBuffer);
	if (p_r_DepthBlendingBuffer != 0)glDeleteFramebuffers(1, &p_r_DepthBlendingBuffer);
	if (m_PolygonPostProcess_FrameBuffer != 0)glDeleteFramebuffers(1, &m_PolygonPostProcess_FrameBuffer);
	if (m_LatePostProcess_FrameBuffer != 0)glDeleteFramebuffers(1, &m_LatePostProcess_FrameBuffer);
}

bool GraphicsRenderer::Initialize(float width,float height) {
	// OpenGLの設定
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	// フルスクリーンに関する設定
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

	// ウィンドウ生成
	bool IsFullScreen = true;
#ifdef _DEBUG
	IsFullScreen = false;
#endif

	sWindow = glfwCreateWindow(
		sWindowWidth,
		sWindowHeight,
		"Haru86_GraphicsEngine",
		(IsFullScreen)? glfwGetPrimaryMonitor() : NULL,
		NULL
	);

	if (!sWindow) {
#ifdef _DEBUG
		Console::Log("Error : glfwCreateWindow\n");
#endif // _DEBUG
		return false;
	}

	// マウスを描画しない
#ifndef _DEBUG
	glfwSetInputMode(sWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif // !_DEBUG

	// プログラム終了時の処理
	atexit(glfwTerminate);
	
	// 現在のサイズを取得
	int w, h;
	//glfwGetWindowSize(sWindow, &w, &h);
	glfwGetFramebufferSize(sWindow, &w, &h);
	sWindowWidth = static_cast<float>(w);
	sWindowHeight = static_cast<float>(h);

	// コンテキストを作成
	glfwMakeContextCurrent(sWindow);

	// glewの初期化
	glewExperimental = GL_TRUE;
	const GLenum error = glewInit();
	if (error != GLEW_OK) {
#ifdef _DEBUG
		Console::Log("Failure! Can not initialize Glew: %s", glewGetErrorString(error));
#endif // _DEBUG

		return false;
	}
	glGetError();

	// Create PostProcess
	PostProcess::CreateInstance();

	//CreateFrameBuffer
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y),polygon_frameTexture, polygon_frameBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y),nullptr, polygon_frameBuffer_MSAA,
		GL_RGBA16F, GL_RGBA, GL_FLOAT,ERenderTargetType::COLOR_RENDER_BUFFER,EDepthTargetType::DEPTH_RENDER_BUFFER,true);
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y), raymarching_frameTexture, raymarching_frameBuffer, GL_RGBA, GL_RGBA);
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y), p_r_BlendingTexture, p_r_BlendingBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y), p_r_DepthBlendingTexture, p_r_DepthBlendingBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y), m_PolygonPostProcess_FrameTexture, m_PolygonPostProcess_FrameBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	CreateFrameBuffer(static_cast<int>(GetScreenSize().x), static_cast<int>(GetScreenSize().y), m_LatePostProcess_FrameTexture, m_LatePostProcess_FrameBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT);

	//
	m_Mixer = std::make_unique<PolygonRaymarchingMixer>();

	return true;
}

glm::vec2 GraphicsRenderer::GetScreenSize() {
	return glm::vec2(sWindowWidth, sWindowHeight);
}

GLFWwindow* GraphicsRenderer::GetWindow() {
	return sWindow;
}

// 背景色
void GraphicsRenderer::SetBackgroudColor(glm::vec4 BackgroudColor) {
	m_BackgroudColor = BackgroudColor;
}

bool GraphicsRenderer::CreateFrameBuffer(int width, int height, std::shared_ptr<Texture> fTex, unsigned int& fBuffer,
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
	if (RenderTargetType==ERenderTargetType::COLOR_TEXTURE_BUFFER) // カラーテクスチャバッファ
	{ 
		fTex->CreateForRendering(width, height, internalformat, format, type);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fTex->GetTextureID(), 0);
	}
	else if(RenderTargetType == ERenderTargetType::COLOR_RENDER_BUFFER) // カラーレンダーバッファ
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

void GraphicsRenderer::CopyFrameBuffer(unsigned int ReadBuffer, unsigned int DrawBuffer, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawBuffer);

	glBlitFramebuffer(
		0,0, width, height,
		0,0, width, height,
		GL_COLOR_BUFFER_BIT,GL_NEAREST
	);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GraphicsRenderer::Draw(const std::shared_ptr<TransformComponent>& UsingCamera, bool IsDrawRay,int ResultFrameBufferIndex, std::function<void(void)> callback, int width, int height)
{
	// これからレンダリングするカメラをセット
	GraphicsMain::GetInstance()->m_UsingCamera = UsingCamera;

	//ポリゴンオブジェクトのカラーマップをレンダリング///////////////////
	GraphicsMain::GetInstance()->renderingTarget = ERerderingTarget::COLOR;
	glBindFramebuffer(GL_FRAMEBUFFER, polygon_frameBuffer_MSAA);
	GraphicsMain::GetInstance()->m_TargetFrameIndex = polygon_frameBuffer_MSAA;
	glViewport(0, 0, static_cast<int>(GetScreenSize().x * frameResolusion), static_cast<int>(GetScreenSize().y * frameResolusion));

	glClearColor(m_BackgroudColor.r, m_BackgroudColor.g, m_BackgroudColor.b, m_BackgroudColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	if (mgame->m_App) {
		mgame->m_App->Draw(false);
	}

	// MSAAカラーマップを通常のカラーマップにコピー
	CopyFrameBuffer(polygon_frameBuffer_MSAA, polygon_frameBuffer, polygon_frameTexture->GetWidth(), polygon_frameTexture->GetHeight());

	// レイマーチングをレンダリングするのはデフォルトバッファのみ(リフレクションプローブでは無視)
	//レイマーチングオブジェクトのカラーマップをレンダリング///////////////
	if (IsDrawRay) {
		GraphicsMain::GetInstance()->renderingTarget = ERerderingTarget::COLOR;
		glBindFramebuffer(GL_FRAMEBUFFER, raymarching_frameBuffer);
		GraphicsMain::GetInstance()->m_TargetFrameIndex = raymarching_frameBuffer;
		glViewport(0, 0, static_cast<int>(GetScreenSize().x * frameResolusion), static_cast<int>(GetScreenSize().y * frameResolusion));

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // デバッグでわざとこの色にしている
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		if (mgame->m_App) {
			mgame->m_App->Draw(true);
		}
	}
	
	//ポリゴンオブジェクトとレイマーチングオブジェクトのカラーバッファをブレンドする
	GraphicsMain::GetInstance()->renderingTarget = ERerderingTarget::COLOR;
	glBindFramebuffer(GL_FRAMEBUFFER, p_r_BlendingBuffer);
	GraphicsMain::GetInstance()->m_TargetFrameIndex = p_r_BlendingBuffer;
	glViewport(0, 0, static_cast<int>(GetScreenSize().x * frameResolusion), static_cast<int>(GetScreenSize().y * frameResolusion));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// ミックス
	m_Mixer->Draw(false);

	//ミキシングしたフレームバッファのポストプロセス////////////////////////////////////////
	PostProcess::GetInstance()->DrawLatePostProcess(p_r_BlendingTexture, 0);
}