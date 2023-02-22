#include "MoonSea.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/PostProcess.h"
#include "GraphicsEngine/Text/TextObject.h"
#include "GraphicsEngine/Graphics/RenderBuffer.h"

namespace app
{
	MoonSea::MoonSea():
		m_MoonSeaMeshRenderer(nullptr),
		m_RaySpaceShip(nullptr),
		m_IsLeaveEarth(false),
		m_UseTextIndex(-1),
		m_Alpha(0.0f),
		m_DrawRaySpaceShip(false),
		m_MoveH(0.0f),
		m_ShipTrailRenderer(nullptr),
		m_ShipTrailResultRenderer(nullptr),
		m_IsDrawShipTrail(false),
		m_RefMapIndex(0),
		m_CorrectionValue(0.1f),
		m_CameraID(-1)
	{
		// 演出用のShaderEditorにコードを追加する
		std::string MoonSeaCode = std::string({
					#include "../Shader/MoonSea_FragComp.h"
		});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("MoonSea.Frag", MoonSeaCode);
		// Shader数とシーンインデックスの互換性のためにもう一つ追加しておく
		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("MoonSea.Frag", MoonSeaCode);

		//
		m_MoonSeaMeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			MoonSeaCode
		);
		m_MoonSeaMeshRenderer->useZTest = false;
		m_MoonSeaMeshRenderer->useAlphaTest = false;
		
		m_RaySpaceShip = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			std::string({
				#include "../Shader/RaySpaceShip_FragComp.h"
			})
		);
		m_RaySpaceShip->useZTest = false;
		m_RaySpaceShip->useAlphaTest = true;
		//m_RaySpaceShip->useAddBlend = true;

		m_ShipTrailRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			std::string({
				#include "../Shader/ShipParticle_FragComp.h"
			})
		);

		m_ShipTrailRenderer->useZTest = false;
		m_ShipTrailRenderer->useAlphaTest = true;
		
		m_ShipTrailResultRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			shaderlib::LatePostProcess_frag
		);

		m_ShipTrailResultRenderer->useZTest = false;
		m_ShipTrailResultRenderer->useAlphaTest = false;
		m_ShipTrailResultRenderer->useAddBlend = true;

		m_RenderBufferList.push_back(std::make_shared<graphic::RenderBuffer>(
			static_cast<int>(GraphicsRenderer::GetInstance()->GetScreenSize().x * GraphicsRenderer::GetInstance()->frameResolusion),
			static_cast<int>(GraphicsRenderer::GetInstance()->GetScreenSize().y * GraphicsRenderer::GetInstance()->frameResolusion),
			GL_RGBA16F, GL_RGBA, GL_FLOAT
			));

		m_RenderBufferList.push_back(std::make_shared<graphic::RenderBuffer>(
			static_cast<int>(GraphicsRenderer::GetInstance()->GetScreenSize().x * GraphicsRenderer::GetInstance()->frameResolusion),
			static_cast<int>(GraphicsRenderer::GetInstance()->GetScreenSize().y * GraphicsRenderer::GetInstance()->frameResolusion),
			GL_RGBA16F, GL_RGBA, GL_FLOAT
			));
	}

	void MoonSea::Update(float time)
	{
	}

	void MoonSea::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			m_MoonSeaMeshRenderer->Draw([this]() {
				m_MoonSeaMeshRenderer->m_material->SetFloatUniform("_LeaveStartTime", 42.0f);
				m_MoonSeaMeshRenderer->m_material->SetIntUniform("_CameraID", m_CameraID);
			});

			if (m_IsDrawShipTrail)
			{
				if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::COLOR)
				{
					m_RenderBufferList[0]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), [&]() {
						m_ShipTrailRenderer->Draw([&]() {
							m_RenderBufferList[1]->GetFrameTexture()->SetActive(GL_TEXTURE0);
							m_ShipTrailRenderer->m_material->SetTexUniform("_BufferA", 0);
							});
						m_RenderBufferList[1]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
						}, true);

					m_RenderBufferList[1]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), [&]() {
						m_ShipTrailRenderer->Draw([&]() {
							m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
							m_ShipTrailRenderer->m_material->SetTexUniform("_BufferA", 0);
							});
						m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
						}, true);
				}
			}

			if (m_ShipTrailResultRenderer)
			{
				m_ShipTrailResultRenderer->Draw([&]() {
					m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
					m_ShipTrailResultRenderer->m_material->SetIntUniform("_NotUseFraRes", 1);
					m_ShipTrailResultRenderer->m_material->SetTexUniform("_SrcTexture", 0);
				});
				m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
			}

			if (m_DrawRaySpaceShip)
			{
				m_RaySpaceShip->Draw([&]()
				{
					m_RaySpaceShip->m_material->SetIntUniform("_IsUseShowing", 1);
					m_RaySpaceShip->m_material->SetFloatUniform("_ShowingFinTime", 30.0f);
					m_RaySpaceShip->m_material->SetFloatUniform("_ShowDuration", 14.0f);
					m_RaySpaceShip->m_material->SetFloatUniform("_MoveStartTime", 31.0f);
					m_RaySpaceShip->m_material->SetFloatUniform("_MoveTimeDuration", 11.0f);

					m_RaySpaceShip->m_material->SetFloatUniform("_MoveH", m_MoveH);

					m_RaySpaceShip->m_material->SetIntUniform("_TRSIndex", 0);
					m_RaySpaceShip->m_material->SetIntUniform("_RefMapIndex", m_RefMapIndex);

					m_RaySpaceShip->m_material->SetFloatUniform("_CorrectionValue", m_CorrectionValue);

					if (m_IsDrawShipTrail)
					{
						m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
						m_RaySpaceShip->m_material->SetTexUniform("_BufferA", 0);
					}
				});

				if (m_IsDrawShipTrail) m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
			}
		}
		else
		{
			if(m_UseTextIndex == 0) text::TextObject::Draw("DemoScene 64k Intro", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(1.0f, 1.0f, 1.0f,m_Alpha));
			else if (m_UseTextIndex == 1) text::TextObject::Draw("The Tale of\nthe Bamboo-Cutter", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(1.0f, 1.0f, 1.0f, m_Alpha));
			else if (m_UseTextIndex == 2) text::TextObject::Draw("CG Engineer\nHaru86_", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(1.0f, 1.0f, 1.0f, m_Alpha));
			else if (m_UseTextIndex == 3) text::TextObject::Draw("Music\nMubert", 0.05f, 1.35f, 3.0f, glm::vec3(0.0f), glm::vec4(1.0f, 1.0f, 1.0f, m_Alpha));
		}
	}

	void MoonSea::UpdateTimeLine(float time)
	{
		unsigned int SceneIndex = GraphicsMain::GetInstance()->GetAppSceneIndex();
		if (SceneIndex == 0)
		{
			m_IsLeaveEarth = false;

			//m_Alpha = 1.0f;
			float pi = 3.1415f;
			if (time >= 1.0 && time < 4.5f) { m_UseTextIndex = 0; m_Alpha = sin(glm::clamp((time>=4.0)? 4.5f - time : time - 1.0f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 4.5f && time < 8.2f) { m_UseTextIndex = 1; m_Alpha = sin(glm::clamp((time>= 7.2f)? 8.2f - time : time - 4.5f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 8.2f && time < 12.3f) { m_UseTextIndex = 2; m_Alpha = sin(glm::clamp((time>= 11.3f)? 12.3f - time : time - 8.2f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 12.3f && time < 15.5f) { m_UseTextIndex = 3; m_Alpha = sin(glm::clamp((time>= 14.5f)? 15.5f - time : time - 12.3f, 0.0f, 1.0f) * pi * 0.5f); }
			else if (time >= 16.0f && time < 19.75f || time >= 23.5f && time < 27.25f) {
				m_DrawRaySpaceShip = true;
				float r = 1.0f;
				float s = 0.25f;
				m_CorrectionValue = 0.1f;
				GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(
					glm::cos(time * s) * r,
					r,
					glm::sin(time * s) * r
				);

			}
			else if (time >= 19.75f && time < 23.5f || time >= 27.25f && time < 31.0f) {
				m_DrawRaySpaceShip = true;
				m_CorrectionValue = 0.1f;
				float rate = glm::clamp( (time - ((time < 23.5f) ? 19.75f : 27.25f))/3.75f, 0.0f, 1.0f);
				GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(
					0.0f,
					-1.0f,
					1.0f + 2.0f * rate
				);
			}
			else if (time >= 31.0f) 
			{
				m_DrawRaySpaceShip = true;
				m_IsDrawShipTrail = true;
				m_RefMapIndex = 1;
				m_CorrectionValue = 0.1f;

				GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(
					glm::cos(-time) * 3.0f,
					0.0f,
					glm::sin(-time) * 3.0f
				);
			}
			else { m_UseTextIndex = -1; m_Alpha = 0.0f; }
			
			PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
				PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseFilmFilter", 1);
				PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_FilmWidthReduction", 0.0f);
			};
		}
		else if (SceneIndex == 1)
		{
			m_IsLeaveEarth = true;

			{
				m_MoveH = time - 42.0f;
				float pi = 3.1415f, sp = 0.5f;
				glm::vec3 ro = glm::vec3(0.0f, m_MoveH, 1.5f), ta = glm::vec3(0.0f, m_MoveH, 0.0f);
				m_CameraID = int(glm::floor(glm::mod(time * sp + 0.5f + 2.0f, 3.0f))); // 0,1,2
				
				if (m_CameraID == 0)
				{
					// down side camera
					float r = 0.5f;
					ta += 4.0f;
					ro = glm::vec3(cos(time) * r, ro.y, sin(time) * r);
				}
				else if (m_CameraID == 1) // ここの時、背景に何も映らなくて変
				{
					float r = 25.0f;
					ro = glm::vec3(cos(pi /2.0f + time * sp) * r, ro.y, sin(pi / 2.0f + time * sp) * r);
				}
				else if (m_CameraID == 2)
				{
					// upside camera
					float r = 0.5f, a = abs(fmod(time, pi * 2.0f) - pi);
					ro += glm::vec3(cos(a) * r, 2.0, sin(a) * r);
				}

				GraphicsMain::GetInstance()->m_MainCamera->m_position = ro;
				GraphicsMain::GetInstance()->m_MainCamera->m_center = ta;
			}

			if (time >= 60.0f)
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteFade", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WhiteFadeVal", time - 60.0f);

					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseFilmFilter", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_FilmWidthReduction", 0.0f);
				};
			}
			else
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseFilmFilter", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_FilmWidthReduction", 0.0f);
				};
			}
		}
	}
}