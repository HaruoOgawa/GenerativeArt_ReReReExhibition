#pragma once
#include <memory>
#include <vector>

class MeshRendererComponent;

namespace app
{
	class CWhiteWall
	{
		std::vector<std::shared_ptr<MeshRendererComponent>> m_WallList;
	public:
		CWhiteWall();
		virtual ~CWhiteWall() = default;

		void Start();
		void Update();
		void Draw();
	};
}