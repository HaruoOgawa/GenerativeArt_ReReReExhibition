#include "PostProcess.h"
#include "GraphicsEngine/Graphics/Texture.h"
#include <glew.h>
#include "GraphicsEngine/Graphics/Mesh.h"
#include "GraphicsEngine/Graphics/Material.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"

//instance‚ð’è‹`‚·‚é
PostProcess* PostProcess::instance = nullptr;

void PostProcess::CreateInstance() {
	if (!instance)instance = new PostProcess();
}

PostProcess* PostProcess::GetInstance() {
	if (instance) return instance;
}

void PostProcess::DestroyInstance() {
	if (instance) {
		delete instance;
		instance = nullptr;
	}
}

PostProcess::PostProcess():
	m_UsePostProcess(false),
	m_UseSSR(false),
	m_LatePostProcesCallBack([]() {})
{
	m_LateMeshRenderer = std::make_shared<MeshRendererComponent>(
		std::make_shared<TransformComponent>(),
		PrimitiveType::BOARD,
		RenderingSurfaceType::RASTERIZER,
		shaderlib::StandardRenderBoard_vert,
		shaderlib::LatePostProcess_frag
	);

	m_LateMeshRenderer->useZTest = false;
}

// SSR
void PostProcess::DrawLatePostProcess(const std::shared_ptr<Texture>& SrcTexture, const unsigned int& DestBuffer)const {
	glBindFramebuffer(GL_FRAMEBUFFER, DestBuffer);
	glViewport(0, 0, static_cast<int>(GraphicsRenderer::GetInstance()->GetScreenSize().x), static_cast<int>(GraphicsRenderer::GetInstance()->GetScreenSize().y));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_LateMeshRenderer->Draw([&]() {
		m_LatePostProcesCallBack();

		SrcTexture->SetActive(GL_TEXTURE0);
		m_LateMeshRenderer->m_material->SetTexUniform("_SrcTexture", 0);
	
	}, GL_TRIANGLES, false, 0);
	SrcTexture->SetEnactive(GL_TEXTURE0);
}
