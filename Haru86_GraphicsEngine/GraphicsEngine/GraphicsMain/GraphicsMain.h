#pragma once

#include "../Graphics/GraphicsRenderer.h"
#include <vector>
#include <memory>
#include <map>
#include <glm/glm.hpp>

#include <glew.h>
#include <glfw3.h>

#include "GraphicsEngine/ShaderEditor/CShaderEditor.h"

enum class ERerderingTarget
{
	COLOR,
	DEPTH,
};

namespace sound { class SoundPlayer;}
namespace app { class GradDemo; }
namespace text { class TTFFactory; }
class TransformComponent;
class MeshRendererComponent;

class GraphicsMain
{
public:
	static GraphicsMain* GetInstance()
	{
		return s_pInstance;
	}

	static void Create();
	static void Destroy();

	GraphicsMain();
	~GraphicsMain();
	bool CreateApp();
	bool Initialize();
	bool RunLoop();

	unsigned int GetAppSceneIndex()const;
	float        GetSceneStartTime() const;
	float        GetSceneEndTime() const;

	float m_SecondsTime;
	float m_MilliSecondsTime;
	float m_SecondsTimeOffset;
	float m_DeltaTime;
	float m_LoadingWaitTime;

	//frame board
	std::shared_ptr<class CameraObject> game_camera_instance;
	
	//
	app::GradDemo* m_App;
	ERerderingTarget renderingTarget;
	unsigned int m_TargetFrameIndex;
	bool isRunning;
	float previousTime;

	bool mouseStateBool;

#ifdef _DEBUG
	bool m_ShowDebugLog;
#endif // _DEBUG


	// ライト
	std::shared_ptr<TransformComponent> m_GroabalLightPosition;

	// カメラ
	std::shared_ptr<TransformComponent> m_MainCamera;
	std::shared_ptr<TransformComponent> m_UsingCamera;

	// サウンド
	std::shared_ptr<sound::SoundPlayer> m_SoundPlayer;

	// テキスト
	std::shared_ptr<text::TTFFactory>	m_TTFFactory;

	// シェーダーエディター
	std::shared_ptr<editor::CShaderEditor> m_ShaderEditor;
private :
	void UpdateTimeline();
	void InputProcess();
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void Update();
	void Draw();
	void LoadData();
protected:
	static GraphicsMain* s_pInstance;
};

