#include "MoonTravel.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/Graphics/PostProcess.h"

namespace app {
	MoonTravel::MoonTravel():
		m_MeshRenderer(nullptr),
		m_RaySpaceShip(nullptr),
		m_CyberMeshRenderer(nullptr),
		m_CustomRo(glm::vec3(0.0f, 0.0f, 1.0f)),
		m_CustomTa(glm::vec3(0.0f)),
		m_MoveSpeed(50.0f),
		m_DrawEarth(true)
	{
		// 演出用のShaderEditorにコードを追加する
		std::string MoonTravel = std::string({
				#include "../Shader/MoonTravel_FragComp.h"
		});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("MoonTravel.Frag", MoonTravel);
		
		std::string CyberpunkSpaceRay = std::string({
				#include "../Shader/CyberpunkSpaceRay_FragComp.h"
			});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("CyberpunkSpaceRay.Frag", CyberpunkSpaceRay);

		//
		m_MeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			MoonTravel
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

		m_CyberMeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			CyberpunkSpaceRay
		);

		m_CyberMeshRenderer->useZTest = false;
		m_CyberMeshRenderer->useAlphaTest = true;
	}

	void MoonTravel::Update(float time)
	{

	}

	void MoonTravel::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			float time = GraphicsMain::GetInstance()->m_SecondsTime;
			float Alpha = glm::clamp(time - 117.5f, 0.0f, 1.0f);

			//if (time < 119.0f)
			{
				m_MeshRenderer->Draw([&]() {
					m_MeshRenderer->m_material->SetFloatUniform("_ShowStartTime", 103.0f);
					m_MeshRenderer->m_material->SetFloatUniform("_ShowTimeDuration", 5.0f);
					m_MeshRenderer->m_material->SetFloatUniform("_Alpha", 1.0f - Alpha);
					m_MeshRenderer->m_material->SetFloatUniform("_MoveSpeed", m_MoveSpeed);
					m_MeshRenderer->m_material->SetIntUniform("_DrawEarth", (m_DrawEarth)? 1 : 0);
				});
			}

			if (time >= 117.5f)
			{
				m_CyberMeshRenderer->Draw([&]() {
					m_CyberMeshRenderer->m_material->SetFloatUniform("_Alpha", Alpha);
					});
			}

			m_RaySpaceShip->Draw([&]() {
				m_RaySpaceShip->m_material->SetIntUniform("_TRSIndex", -1);
				m_RaySpaceShip->m_material->SetIntUniform("_RefMapIndex", 1);
				m_RaySpaceShip->m_material->SetFloatUniform("_CorrectionValue", 0.1f);

				m_RaySpaceShip->m_material->SetIntUniform("_UseCustomPos", 1);
				m_RaySpaceShip->m_material->SetVec3Uniform("_CustomRo", m_CustomRo);
				m_RaySpaceShip->m_material->SetVec3Uniform("_CustomTa", m_CustomTa);
			});
		}
	}

	void MoonTravel::UpdateTimeLine(float time)
	{
		if (GraphicsMain::GetInstance()->GetAppSceneIndex() == 3)
		{
			int CamID = (time >= 102.0f)? 0 : static_cast<int>(floorf(glm::mod(time / 3.0f, 3.0f)));
			if (time >= 103.0f && time < 117.5f)
			{
				glm::vec3 pos = glm::vec3(1.0f, 2.0f, -1.0f);
				GraphicsMain::GetInstance()->m_MainCamera->m_position = pos;
				GraphicsMain::GetInstance()->m_MainCamera->m_center = pos + glm::vec3(0.0f, 0.0f, 1.0f);
				m_CustomRo = glm::vec3(0.0f, 0.0f, 0.0f);
				m_CustomTa = glm::vec3(0.0f, 0.0f, 0.0f);
				m_MoveSpeed = 2.0f;
				m_DrawEarth = true;
			}
			else if (time >= 117.5f)
			{
				m_CustomRo = glm::vec3(0.0f, 0.0f, -5.0f * glm::clamp(time - 117.5f, 0.0f, 1.0f)); m_CustomTa = glm::vec3(0.0f, 0.0f, 0.0f);
			}
			else
			{
				if (CamID == 0) {
					float r = 10.0f, s = 0.5f; m_MoveSpeed = 50.0f; m_DrawEarth = true;
					GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(r * cos(time * s), r + 1.0f, r * sin(time * s));
					GraphicsMain::GetInstance()->m_MainCamera->m_center = glm::vec3(0.0f, 2.0f, 0.0f);
					m_CustomRo = glm::vec3(r * 0.125f * cos(time * s), r * 0.125f, r * 0.125f * sin(time * s));
					m_CustomTa = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				else if (CamID == 1) {
					m_MoveSpeed = 50.0f; m_DrawEarth = true;
					GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 2.0f, 2.0f);
					GraphicsMain::GetInstance()->m_MainCamera->m_center = glm::vec3(0.0f, 2.0f, 0.0f);
					m_CustomRo = glm::vec3(0.0f, 0.0f, 0.25f);
					m_CustomTa = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				else if (CamID == 2) {
					m_MoveSpeed = 50.0f; float s = 1.0f, r = 0.25f; m_DrawEarth = false;
					GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 2.0f, -2.0f) + glm::vec3(0.1f * cos(time), 0.1f * sin(time), 0.0f);
					GraphicsMain::GetInstance()->m_MainCamera->m_center = glm::vec3(0.0f, 2.0f, 0.0f);
					glm::vec3 move = glm::vec3(0.125f * r * cos(time * s), 0.125f * r * sin(time * s), 0.0f);
					m_CustomRo = glm::vec3(0.0f, 0.0f, -0.15f) + move;
					m_CustomTa = glm::vec3(0.0f, 0.0f, 0.0f) + move;
				}
			}
		}
		else if (GraphicsMain::GetInstance()->GetAppSceneIndex() == 4)
		{
			glm::vec3 pos = glm::vec3(1.0f, 0.5f, -1.0f);
			GraphicsMain::GetInstance()->m_MainCamera->m_position = pos;
			GraphicsMain::GetInstance()->m_MainCamera->m_center = pos + glm::vec3(0.0f, 0.0f, 1.0f);
			m_CustomRo = glm::vec3(0.0f, 0.0f, -0.25f - 4.75f * glm::clamp(time - 117.5f, 0.0f, 1.0f)); m_CustomTa = glm::vec3(0.0f, 0.0f, 0.0f);

			if (time >= 137.5f)
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					float Rate = glm::clamp((time - 137.5f) * 2.0f, 0.0f, 1.0f);

					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseGridWave", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWaveCustom", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WavePower", Rate);

					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteNoise", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WhiteNoisePower", Rate * 2.0f);
				};
			}

		}
	}
}