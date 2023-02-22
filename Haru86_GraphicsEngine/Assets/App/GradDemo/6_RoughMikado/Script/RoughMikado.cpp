#include "RoughMikado.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "EnergyField.h"

namespace app
{
	RoughMikado::RoughMikado():
		m_EnergyField(nullptr)
	{
		m_EnergyField = std::make_shared<EnergyField>();
	}

	void RoughMikado::Update(float time)
	{
		m_EnergyField->Update();
	}

	void RoughMikado::Draw(bool IsRaymarching)
	{
		if (IsRaymarching)
		{
			m_EnergyField->Draw(IsRaymarching);
		}
	}

	void RoughMikado::UpdateTimeLine(float time)
	{
	}
}