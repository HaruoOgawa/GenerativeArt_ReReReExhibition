#include "GraphicsMain.h"
#include <string>
#include "../Graphics/Mesh.h"
#include <vector>
#include <algorithm>
#include "Assets/App/ArtReReRe/ArtReReRe.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Sound/SoundPlayer.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include <time.h>

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

#include "GraphicsEngine/Graphics/ShaderLib.h"

GraphicsMain* GraphicsMain::s_pInstance = nullptr;

void GraphicsMain::Create()
{
	if (!s_pInstance)s_pInstance = new GraphicsMain;
}

void GraphicsMain::Destroy()
{
	delete s_pInstance;
	s_pInstance = nullptr;
}

GraphicsMain::GraphicsMain()
	: 
	isRunning(true),
	m_SecondsTime(0.0f),
	m_SecondsTimeOffset(0.0f),
	m_MilliSecondsTime(0.0f),
	m_DeltaTime(0.0f),
	previousTime(0.0f),
	mouseStateBool(false),
#ifdef _DEBUG
	m_ShowDebugLog(false),
#endif // _DEBUG

	renderingTarget(ERerderingTarget::COLOR),
	m_TargetFrameIndex(0),
	m_MainCamera(nullptr),
	m_UsingCamera(nullptr),
	m_SoundPlayer(nullptr),
	m_LoadingWaitTime(0.0f),
	m_GroabalLightPosition(nullptr),
	m_DirectionalLightDir(glm::vec3(-1.0f, -1.0f, -1.0f))
{
}

GraphicsMain::~GraphicsMain() {
	GraphicsRenderer::Destroy();
}

bool GraphicsMain::CreateApp() {
	if (glfwInit() == GL_FALSE)return false;
	GraphicsRenderer::Create();
	if (!GraphicsRenderer::GetInstance()->Initialize(500, 500))return false;

	return true;
}

bool GraphicsMain::Initialize() {
	// メモリ確保
	m_App = new app::ArtReReRe();
	LoadData();
	
	return true;
}

void GraphicsMain::LoadData() {
	//
	m_SoundPlayer = std::make_shared<sound::SoundPlayer>();

	//
	m_App->Start();
	// Appのロードにかかった時間
	//float AppLoadWatiTime = static_cast<float>(clock());
	float AdjustAppLoadWatitTime = 846.0f; 
	//AdjustAppLoadWatitTime = (AppLoadWatiTime < AdjustAppLoadWatitTime) ? AppLoadWatiTime : AdjustAppLoadWatitTime;

	if (m_MainCamera == nullptr) m_MainCamera = std::make_shared<TransformComponent>(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	if (!m_GroabalLightPosition) m_GroabalLightPosition = std::make_shared<TransformComponent>(glm::vec3(10.0f));

	// ロードにかかった時間を記録しておく(エディタ実装以前の待ち時間と辻褄を合わせる)
	m_LoadingWaitTime = static_cast<float>(clock()) - AdjustAppLoadWatitTime;

	//
	if (m_SecondsTimeOffset != 0.0f && m_SecondsTimeOffset > 0.0f)
	{
		m_SoundPlayer->Skip(m_SecondsTimeOffset);
	}
	else
	{
		m_SoundPlayer->Play();
	}
}

bool GraphicsMain::RunLoop() {
	while (isRunning)
	{
		UpdateTimeline();
		InputProcess();
		Update();
		Draw();
	}

	return false;
}

unsigned int GraphicsMain::GetAppSceneIndex()const
{
	return m_App->GetSceneIndex();
}

float GraphicsMain::GetSceneStartTime() const
{
	return m_App->GetSceneStartTime();
}

float GraphicsMain::GetSceneEndTime() const
{
	return m_App->GetSceneEndTime();
}

void GraphicsMain::UpdateTimeline() {
	m_App->UpdateTimeline();
}

void GraphicsMain::InputProcess() {
	// イベントを発行
	glfwPollEvents();
	glfwSetKeyCallback(GraphicsRenderer::GetInstance()->GetWindow(), key_callback);
}

void GraphicsMain::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		GraphicsMain::GetInstance()->isRunning=false;
	}
#ifdef _DEBUG
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		//Console::Log("Space Push\n");
		GraphicsMain::GetInstance()->m_ShowDebugLog = true;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		//Console::Log("Space Release\n");
		GraphicsMain::GetInstance()->m_ShowDebugLog = false;
	}
#endif // _DEBUG

}

void GraphicsMain::Update() {
	// 現在の経過時間(ロードにかかった時間分は引いておく)
	m_MilliSecondsTime = static_cast<float>(clock()) + m_SecondsTimeOffset * 1000.0f - m_LoadingWaitTime;
	m_SecondsTime = m_MilliSecondsTime * 0.001f;
	m_DeltaTime = (m_MilliSecondsTime - previousTime) * 0.001f;

#ifdef _DEBUG
	// FPSの計測と表示(60FPSを基準とする)
	float FPS = 60.0f / (m_DeltaTime * 60.0f);
	//Console::Log("[FPS] %f fps / [CurrentTime] %f s\n", FPS, m_SecondsTime);
#endif // _DEBUG

	previousTime = m_MilliSecondsTime;
	if (m_App)m_App->Update();
}

// ここのDrawではカメラ位置を変える
void GraphicsMain::Draw() {
	// 垂直同期の待機時間
	glfwSwapInterval(1);

	// 通常の描画(画面に表示される部分)
	GraphicsRenderer::GetInstance()->Draw(m_MainCamera, true,0, []() {},GraphicsRenderer::GetInstance()->GetScreenSize().x, GraphicsRenderer::GetInstance()->GetScreenSize().y);

	//カラーバッファを入れ替える
	glfwSwapBuffers(GraphicsRenderer::GetInstance()->GetWindow());
}
