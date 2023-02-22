#include "PolygonRaymarchingMixer.h"
#include "GraphicsEngine/Graphics/Material.h"
#include "GraphicsEngine/Graphics/Mesh.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Graphics/Texture.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"

PolygonRaymarchingMixer::PolygonRaymarchingMixer() {
	m_MixerRenderer = std::make_shared<MeshRendererComponent>(
		std::make_shared<TransformComponent>(),
		PrimitiveType::BOARD,
		RenderingSurfaceType::RASTERIZER,
		shaderlib::StandardRenderBoard_vert,
		shaderlib::LatePostProcess_frag
	);
}

void PolygonRaymarchingMixer::Draw(bool IsDepthMix) {
	m_MixerRenderer->Draw([&]() {
		m_MixerRenderer->m_material->SetIntUniform("_UseMixer", 1);

		if (GraphicsRenderer::GetInstance()->polygon_frameTexture != nullptr) {
			GraphicsRenderer::GetInstance()->polygon_frameTexture->SetActive(GL_TEXTURE0);
			m_MixerRenderer->m_material->SetTexUniform("polygon_frameTexture", 0);

		}

		{
			if (GraphicsRenderer::GetInstance()->raymarching_frameTexture != nullptr) {
				GraphicsRenderer::GetInstance()->raymarching_frameTexture->SetActive(GL_TEXTURE2);
				m_MixerRenderer->m_material->SetTexUniform("raymarching_frameTexture", 2);
			}
		}
	}, GL_TRIANGLES, false, 0);

	GraphicsRenderer::GetInstance()->polygon_frameTexture->SetEnactive(GL_TEXTURE0);
	GraphicsRenderer::GetInstance()->raymarching_frameTexture->SetEnactive(GL_TEXTURE2);
}