#include "EarthAndMoon.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/PostProcess.h"

namespace app {
	EarthAndMoon::EarthAndMoon():
		m_MeshRenderer(nullptr),
		m_RaySpaceShip(nullptr)
	{
		// 演出用のShaderEditorにコードを追加する
		std::string EarthAndMoon = std::string({
					#include "../Shader/EarthAndMoon_FragComp.h"
		});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("EarthAndMoon.Frag", EarthAndMoon);

		//
		m_MeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			EarthAndMoon
		);

		m_MeshRenderer->useZTest = false;
		m_MeshRenderer->useAlphaTest = true;

		m_RaySpaceShip = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			std::string({
				#include "../../0_MoonSea/Shader/RaySpaceShip_FragComp.h"
			})
		);
		m_RaySpaceShip->useZTest = false;
		m_RaySpaceShip->useAlphaTest = true;
	}

	void EarthAndMoon::Update(float time)
	{

	}

	void EarthAndMoon::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			m_MeshRenderer->Draw();

			m_RaySpaceShip->Draw([&]() {
				m_RaySpaceShip->m_material->SetIntUniform("_TRSIndex", 1);
				m_RaySpaceShip->m_material->SetFloatUniform("_GoMoonTime", 73.0f);
				m_RaySpaceShip->m_material->SetIntUniform("_RefMapIndex", 1);
				m_RaySpaceShip->m_material->SetFloatUniform("_CorrectionValue", 0.1f);
			});
		}
	}

	void EarthAndMoon::UpdateTimeLine(float time)
	{
		unsigned int SceneIndex = GraphicsMain::GetInstance()->GetAppSceneIndex();

		if (SceneIndex == 2)
		{
			if (time < 62.0f)
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteFade", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WhiteFadeVal", 62.0f - time);
				};
			}
			else
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteFade", 0);
				};
			}

			m_MeshRenderer->m_calllback = [=]() {
				m_MeshRenderer->m_material->SetFloatUniform("_MoveVal", (time - 61.0f)/13.0f);
			};

			GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f,0.0f,3.0f);
			GraphicsMain::GetInstance()->m_MainCamera->m_center = glm::vec3(0.0f,0.0f,0.0f);
		}
	}
}