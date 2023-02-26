#include "CWhiteWall.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"

namespace app
{
	CWhiteWall::CWhiteWall()
	{
		Start();
	}

	void CWhiteWall::Start()
	{
		for (int i = 0; i < 4; i++)
		{
			std::shared_ptr<MeshRendererComponent> Wall = std::make_shared<MeshRendererComponent>(
				std::make_shared<TransformComponent>(),
				PrimitiveType::CUBE,
				RenderingSurfaceType::RASTERIZER,
				shaderlib::Standard_vert,
				shaderlib::Standard_frag
			);

			float val = static_cast<float>(i), sign = (i % 2 == 0) ? 1.0f : -1.0f, size = 50.0f, smin = 0.1f;
			
			Wall->m_transform->m_position = glm::vec3(
				0.5f * size * ((i >= 0 && i < 2) ? sign : 0.0f),
				0.5f * size * ((i >= 2 && i < 4) ? sign : 0.0f),
				0.5f * size * ((i >= 4 && i < 6) ? sign : 0.0f)
			);

			Wall->m_transform->m_scale = glm::vec3(
				!(i >= 0 && i < 2) ? size : smin,
				!(i >= 2 && i < 4) ? size : smin,
				!(i >= 4 && i < 6) ? size : smin
			);
			
			m_WallList.push_back(Wall);
		}
	}

	void CWhiteWall::Update()
	{

	}

	void CWhiteWall::Draw()
	{
		for (const auto& Wall : m_WallList)
		{
			Wall->Draw([&]() {
				Wall->m_material->SetIntUniform("_UseEnvColor", 1);
				Wall->m_material->SetVec4Uniform("_EnvColor", glm::vec4(glm::vec3(0.05f), 1.0f));
			});
		}
	}
}