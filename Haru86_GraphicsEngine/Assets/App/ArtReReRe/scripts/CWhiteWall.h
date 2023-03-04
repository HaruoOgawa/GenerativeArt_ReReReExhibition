#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class MeshRendererComponent;

namespace app
{
	class CWhiteWall
	{
		std::vector<std::shared_ptr<MeshRendererComponent>> m_WallList;
	public:
		CWhiteWall();
		virtual ~CWhiteWall() = default;

		void Init(const glm::vec4& WallHalfSize);
		void Update();
		void Draw();
	};
}