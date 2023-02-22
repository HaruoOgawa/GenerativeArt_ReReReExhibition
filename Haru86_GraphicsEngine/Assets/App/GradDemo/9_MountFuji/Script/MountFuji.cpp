#include "MountFuji.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/Text/TextObject.h"
#include "GraphicsEngine/Graphics/PostProcess.h"

namespace app {
	MountFuji::MountFuji() :
		m_MeshRenderer(nullptr),
		m_UseTextIndex(-1),
		m_Alpha(0.0f)
	{
		// 演出用のShaderEditorにコードを追加する
			std::string MountFuji = std::string({
				#include "../Shader/MountFuji_FragComp.h"
				});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("MountFuji.Frag", MountFuji);

		//
		m_MeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			MountFuji
		);

		m_MeshRenderer->useZTest = false;
		m_MeshRenderer->useAlphaTest = false;
	}

	void MountFuji::Update(float time)
	{

	}

	void MountFuji::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			m_MeshRenderer->Draw();
		}
		else
		{
			if (m_UseTextIndex == 0) text::TextObject::Draw("DemoScene 64k Intro", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, m_Alpha));
			else if (m_UseTextIndex == 1) text::TextObject::Draw("The Tale of\nthe Bamboo-Cutter", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, m_Alpha));
			else if (m_UseTextIndex == 2) text::TextObject::Draw("CG Engineer\nHaru86_", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, m_Alpha));
			else if (m_UseTextIndex == 3) text::TextObject::Draw("Music\nMubert", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, m_Alpha));
		}
	}

	void MountFuji::UpdateTimeLine(float time)
	{
		unsigned int SceneIndex = GraphicsMain::GetInstance()->GetAppSceneIndex();
		if (SceneIndex == 9)
		{
			float pi = 3.1415f; glm::vec3 CameraPos = glm::vec3(27.5f * cos(45.5f * 0.1f), 3.5f, 27.5f * sin(45.5f * 0.1f)), CameraCen = glm::vec3(0.0f);
			if (time >= 210.0f && time < 214.0f) { CameraPos.y = 11.5f - glm::max(0.0f, time - 210.0f) * 2.0; m_UseTextIndex = 0; m_Alpha = sin(glm::clamp((time >= 213.0f) ? 214.0f - time : time - 210.0f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 214.0f && time < 218.0f) { float t = (glm::clamp((time - 214.0f) / 4.0f, 0.0f, 1.0f) * 0.5f - 0.25f);  CameraPos = glm::vec3(27.5f * cos(45.5f * 0.1f + t), 3.5f, 27.5f * sin(45.5f * 0.1f + t)); m_UseTextIndex = 1; m_Alpha = sin(glm::clamp((time >= 217.0f) ? 218.0f - time : time - 214.0f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 218.0f && time < 222.0f) { CameraPos.z -= (time - 211.0f); m_UseTextIndex = 2; m_Alpha = sin(glm::clamp((time >= 221.0f) ? 222.0f - time : time - 218.0f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 222.0f && time < 226.0f) { CameraPos.z -= (time - 211.0f); m_UseTextIndex = 3; m_Alpha = sin(glm::clamp((time >= 225.0f) ? 226.0f - time : time - 222.0f, 0.0f, 1.0f) * pi * 0.5f); }
			else { CameraPos.z -= (time - 211.0f); }
			GraphicsMain::GetInstance()->m_MainCamera->m_center = CameraCen;
			GraphicsMain::GetInstance()->m_MainCamera->m_position = CameraPos;

			if (time < 211.0f)
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseVignette", 0);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteFade", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WhiteFadeVal", 1.0f - (time - 210.0f));
				};
			}
			else
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseVignette", 0);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteFade", 0);
				};
			}
		}
	}
}