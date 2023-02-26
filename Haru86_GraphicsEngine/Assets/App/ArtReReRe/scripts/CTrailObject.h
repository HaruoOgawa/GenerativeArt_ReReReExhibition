#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class MeshRendererComponent;
class ComputeBuffer;
class Material;

namespace app
{
	struct STrailData
	{
		float m_Pos[4];
		float m_Rotate[4];
		float m_Scale[4];
		float m_Color[4];
		float m_Velocity[4];
	};

	class CTrailObject
	{
		std::shared_ptr<MeshRendererComponent> m_Mesh;
		std::shared_ptr<ComputeBuffer> m_TrailBuffer;
		std::shared_ptr<Material> m_TrailGPGPU;

		glm::vec4 m_WallHalfSize;
		unsigned int m_DomainCount;
		unsigned int m_InstanceNumPerDomain;
	private:
		static float Noise(glm::vec2 st);

		static float PerlinNoise(glm::vec2 st);

		void InitBuffer();
		void LinkBuffer();
	public:
		CTrailObject();
		virtual ~CTrailObject();

		void Init();
		void Update();
		void Draw();
	};
}