#include "FeatherRobe.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/Graphics/RenderBuffer.h"

namespace app {
	FeatherRobe::FeatherRobe() :
		m_MeshRenderer(nullptr),
		m_AuraPos(glm::vec3(0.0f)),
		m_Voxel(nullptr)
	{
		// 演出用のShaderEditorにコードを追加する
		std::string FeatherRobe = std::string({
				#include "../Shader/FeatherRobe_FragComp.h"
			});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("FeatherRobe.Frag", FeatherRobe);

		//
		m_MeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			FeatherRobe
		);

		m_MeshRenderer->useZTest = false;
		m_MeshRenderer->useAlphaTest = false;

		//
		m_AuraTrailRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			std::string({
				#include "../Shader/FeatherRobeTrail_FragComp.h"
			})
		);

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

		m_Voxel = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			shaderlib::Voxel_frag
		);
		m_Voxel->useAlphaTest = true;
		m_Voxel->useZTest = true;
	}

	void FeatherRobe::Update(float time)
	{
	}

	void FeatherRobe::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::COLOR)
			{
				m_RenderBufferList[0]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), [&]() {
					m_AuraTrailRenderer->Draw([&]() {
						m_RenderBufferList[1]->GetFrameTexture()->SetActive(GL_TEXTURE0);
						m_AuraTrailRenderer->m_material->SetTexUniform("_BufferA", 0);
						});
					m_RenderBufferList[1]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
					}, true);

				m_RenderBufferList[1]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), [&]() {
					m_AuraTrailRenderer->Draw([&]() {
						m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
						m_AuraTrailRenderer->m_material->SetTexUniform("_BufferA", 0);
						});
					m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
					}, true);
			}

			//
			float time = GraphicsMain::GetInstance()->m_SecondsTime;
			float Alpha = (time < 180.0f)? 0.0f : glm::clamp((time - 180.0f) / 2.0f, 0.0f, 1.0f);

			//
			m_MeshRenderer->Draw([&]() {
				m_MeshRenderer->m_material->SetVec3Uniform("_AuraPos", m_AuraPos);
				m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
				m_MeshRenderer->m_material->SetTexUniform("_BufferA", 0);

				m_MeshRenderer->m_material->SetFloatUniform("_Alpha", 1.0f - Alpha);
			});
			m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);

			if (time >= 180.0f)
			{
				m_Voxel->Draw([&]() {
					m_Voxel->m_material->SetIntUniform("_MapIndex", 0);
					m_Voxel->m_material->SetIntUniform("_UseTex", 1);
					m_Voxel->m_material->SetFloatUniform("_Alpha", Alpha);
					});
			}
		}
	}

	void FeatherRobe::UpdateTimeLine(float time)
	{
		if (GraphicsMain::GetInstance()->GetAppSceneIndex() == 7)
		{
			m_AuraPos = glm::vec3(0.0f, 2.0f, time - 166.0f);
			glm::vec3 CameraPos = glm::vec3(0.0f), CameraCenter = glm::vec3(0.0f, -3.0f, 0.0f) + m_AuraPos;
			//if (time >= 166.0f && time < 172.0f) { CameraCenter.y -= 2.0f; CameraPos = glm::vec3(3.0f * cos(time), -4.0f, 3.0f * sin(time)) + m_AuraPos; }
			/*else */if (time >= 164 && time < 180.0f) { CameraPos = glm::vec3(0.0f, -3.0f, 0.1f) + m_AuraPos; }
			GraphicsMain::GetInstance()->m_MainCamera->m_position = CameraPos;
			GraphicsMain::GetInstance()->m_MainCamera->m_center = CameraCenter;
		}
	}
}