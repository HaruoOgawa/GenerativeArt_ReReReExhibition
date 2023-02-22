#include "EnergyField.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/RenderBuffer.h"

namespace app
{
	EnergyField::EnergyField() :
		m_RayCoreRenderer(nullptr),
		m_RayTrailRenderer(nullptr)
	{
		// 演出用のShaderEditorにコードを追加する
		std::string EnergyField = std::string({
				#include "../Shader/EnergyField_FragComp.h"
			});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("EnergyField.Frag", EnergyField);

		//

		m_RayCoreRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			EnergyField
		);
		
		m_RayTrailRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			std::string({
				#include "../Shader/EnergyTrail_FragComp.h"
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

		m_RenderBufferList[0]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), []() {}, true);
		m_RenderBufferList[1]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), []() {}, true);
	}

	void EnergyField::Update()
	{
	}

	void EnergyField::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::COLOR)
			{
				m_RenderBufferList[0]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), [&]() {
					m_RayTrailRenderer->Draw([&]() {
						m_RenderBufferList[1]->GetFrameTexture()->SetActive(GL_TEXTURE0);
						m_RayTrailRenderer->m_material->SetTexUniform("_BufferA", 0);
						});
					m_RenderBufferList[1]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
					}, true);

				m_RenderBufferList[1]->Draw(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), [&]() {
					m_RayTrailRenderer->Draw([&]() {
						m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
						m_RayTrailRenderer->m_material->SetTexUniform("_BufferA", 0);
						});
					m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
					}, true);
			}

			m_RayCoreRenderer->Draw([&]() {
				m_RenderBufferList[0]->GetFrameTexture()->SetActive(GL_TEXTURE0);
				m_RayCoreRenderer->m_material->SetTexUniform("_BufferA", 0);
				});
			m_RenderBufferList[0]->GetFrameTexture()->SetEnactive(GL_TEXTURE0);
		}
	}
}