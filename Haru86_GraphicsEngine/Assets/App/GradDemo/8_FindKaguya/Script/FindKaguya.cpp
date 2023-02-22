#include "FindKaguya.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/PostProcess.h"

namespace app
{
	FindKaguya::FindKaguya():
		m_Voxel(nullptr)
	{
		// 演出用のShaderEditorにコードを追加する
		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("Voxel.Frag", shaderlib::Voxel_frag);

		//
		m_Voxel = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RAYMARCHING,
			shaderlib::StandardRenderBoard_vert,
			shaderlib::Voxel_frag
		);
	}

	void FindKaguya::Update(float time)
	{
	}

	void FindKaguya::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			m_Voxel->Draw([&]() {
				m_Voxel->m_material->SetIntUniform("_MapIndex", 0);
				m_Voxel->m_material->SetIntUniform("_UseTex", 1);
			});
		}
	}

	void FindKaguya::UpdateTimeLine(float time)
	{
		if (GraphicsMain::GetInstance()->GetAppSceneIndex() == 8)
		{
			if (time > 195.0f)
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					float VignetteBrightness = (time > 207.0f) ? 0.25f + 0.75 * (time - 207.0f) / 2.0f : 0.25f * glm::clamp((time - 195.0f) / 5.0f, 0.0f, 1.0f);

					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseVignette", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_VignetteRadius", 0.9f);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_VignetteLateRadius", 0.0f);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_VignetteBrightness", VignetteBrightness);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_VignetteBlendRate", glm::clamp((time - 195.0f) / 2.0f, 0.0f, 1.0f));
				};
			}
		}
	}
}