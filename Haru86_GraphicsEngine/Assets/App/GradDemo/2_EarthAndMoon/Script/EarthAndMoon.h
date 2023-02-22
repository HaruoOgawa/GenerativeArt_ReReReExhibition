#pragma once
#include <memory>

class MeshRendererComponent;

namespace app {
	class EarthAndMoon
	{
		std::shared_ptr<MeshRendererComponent> m_MeshRenderer;
		std::shared_ptr<MeshRendererComponent> m_RaySpaceShip;
	public:
		EarthAndMoon();
		virtual ~EarthAndMoon() = default;

		void Update(float time);
		void Draw(bool IsRaymarching);
		void UpdateTimeLine(float time);
	};
}