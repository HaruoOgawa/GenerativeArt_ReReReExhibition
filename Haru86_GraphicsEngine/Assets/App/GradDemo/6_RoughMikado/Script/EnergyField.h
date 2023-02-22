#pragma once
#include <memory>
#include <vector>

class MeshRendererComponent;
namespace graphic { class RenderBuffer; }

namespace app
{
	class EnergyField
	{
		std::shared_ptr<MeshRendererComponent> m_RayCoreRenderer;
		std::shared_ptr<MeshRendererComponent> m_RayTrailRenderer;
		std::vector<std::shared_ptr<graphic::RenderBuffer>> m_RenderBufferList;
	public:
		EnergyField();
		virtual ~EnergyField() = default;

		void Update();
		void Draw(bool IsRaymarching);
	};
}
