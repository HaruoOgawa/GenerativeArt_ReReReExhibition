#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <vector>

class MeshRendererComponent;
namespace graphic { class RenderBuffer; }

namespace app {
	class FeatherRobe
	{
		std::shared_ptr<MeshRendererComponent> m_MeshRenderer;
		glm::vec3 m_AuraPos;

		std::shared_ptr<MeshRendererComponent> m_AuraTrailRenderer;
		std::vector<std::shared_ptr<graphic::RenderBuffer>> m_RenderBufferList;

		std::shared_ptr<MeshRendererComponent> m_Voxel;
	public:
		FeatherRobe();
		virtual ~FeatherRobe() = default;

		void Update(float time);
		void Draw(bool IsRaymarching);
		void UpdateTimeLine(float time);
	};
}
