#pragma once
#include <memory>

class MeshRendererComponent;

namespace app {
	class MountFuji
	{
		std::shared_ptr<MeshRendererComponent> m_MeshRenderer;
		int									   m_UseTextIndex;
		float								   m_Alpha;
	public:
		MountFuji();
		virtual ~MountFuji() = default;

		void Update(float time);
		void Draw(bool IsRaymarching);
		void UpdateTimeLine(float time);
	};
}