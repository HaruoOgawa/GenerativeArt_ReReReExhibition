#pragma once
#include <memory>
#include <vector>

class MeshRendererComponent;
namespace graphic { class RenderBuffer; }

namespace app
{
	class MoonSea
	{
		std::shared_ptr<MeshRendererComponent> m_MoonSeaMeshRenderer;
		std::shared_ptr<MeshRendererComponent> m_RaySpaceShip;

		std::vector<std::shared_ptr<graphic::RenderBuffer>> m_RenderBufferList;
		std::shared_ptr<MeshRendererComponent> m_ShipTrailRenderer;
		std::shared_ptr<MeshRendererComponent> m_ShipTrailResultRenderer;

		bool								   m_IsLeaveEarth;
		int									   m_UseTextIndex;
		float								   m_Alpha;
		bool								   m_DrawRaySpaceShip;

		float								   m_MoveH;
		bool								   m_IsDrawShipTrail;
		int									   m_RefMapIndex;
		float								   m_CorrectionValue;

		int									   m_CameraID;
	private:
	public:
		MoonSea();
		virtual ~MoonSea() = default;

		void Update(float time);
		void Draw(bool IsRaymarching);
		void UpdateTimeLine(float time);
	};
}