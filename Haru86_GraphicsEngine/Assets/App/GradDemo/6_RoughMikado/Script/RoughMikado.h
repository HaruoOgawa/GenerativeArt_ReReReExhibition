#pragma once
#include <memory>

class MeshRendererComponent;

namespace app
{
	class EnergyField;

	class RoughMikado
	{
		std::shared_ptr<EnergyField> m_EnergyField;
	public:
		RoughMikado();
		~RoughMikado() = default;

		void Update(float time);
		void Draw(bool IsRaymarching);
		void UpdateTimeLine(float time);
	};
}
