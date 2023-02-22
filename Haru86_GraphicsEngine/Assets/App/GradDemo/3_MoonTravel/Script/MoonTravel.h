#pragma once
#include <memory>
#include <glm/glm.hpp>

class MeshRendererComponent;

namespace app {
	class MoonTravel
	{
		std::shared_ptr<MeshRendererComponent> m_MeshRenderer;
		std::shared_ptr<MeshRendererComponent> m_RaySpaceShip;
		std::shared_ptr<MeshRendererComponent> m_CyberMeshRenderer;

		glm::vec3 m_CustomRo;
		glm::vec3 m_CustomTa;
		float m_MoveSpeed;
		bool m_DrawEarth;
	public:
		MoonTravel();
		virtual ~MoonTravel() = default;

		void Update(float time);
		void Draw(bool IsRaymarching);
		void UpdateTimeLine(float time);
	};
}