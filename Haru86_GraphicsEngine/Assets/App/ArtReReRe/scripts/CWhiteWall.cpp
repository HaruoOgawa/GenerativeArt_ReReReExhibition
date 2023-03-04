#include "CWhiteWall.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"

namespace app
{
	CWhiteWall::CWhiteWall()
	{
	}

	void CWhiteWall::Init(const glm::vec4& WallHalfSize)
	{
		float Shrink = 0.75f;
		glm::vec4 Size = WallHalfSize * Shrink;

		for (int i = 0; i < 4; i++)
		{
			std::shared_ptr<MeshRendererComponent> Wall = std::make_shared<MeshRendererComponent>(
				std::make_shared<TransformComponent>(),
				PrimitiveType::CUBE,
				RenderingSurfaceType::RASTERIZER,
				shaderlib::Standard_vert,
				shaderlib::Standard_frag
			);

			float val = static_cast<float>(i), sign = (i % 2 == 0) ? 1.0f : -1.0f;
			const float WallDepth = 1.25f, WallTickness = 1.0f;

			Wall->m_transform->m_position = glm::vec3(
				Size.x * ((i >= 0 && i < 2) ? sign : 0.0f),
				Size.y * ((i >= 2 && i < 4) ? sign : 0.0f),
				Size.z * ((i >= 4 && i < 6) ? sign : 0.0f)
			);

			Wall->m_transform->m_scale = glm::vec3(
				!(i >= 0 && i < 2) ? Size.x * 2.0f : WallTickness,
				!(i >= 2 && i < 4) ? Size.y * 2.0f : WallTickness,
				!(i >= 4 && i < 6) ? Size.z * 2.0f * WallDepth : WallTickness
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