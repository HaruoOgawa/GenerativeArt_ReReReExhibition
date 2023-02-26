#include "CTrailObject.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Graphics/ComputeBuffer.h"

namespace app
{
	CTrailObject::CTrailObject() :
		m_Mesh(nullptr),
		m_TrailBuffer(nullptr),
		m_TrailGPGPU(nullptr),
		m_DomainCount(8),
		m_InstanceNumPerDomain(1024),
		m_WallHalfSize(glm::vec4(25.0f, 25.0f, 25.0f,1.0f))
	{
	}

	CTrailObject::~CTrailObject()
	{
		
	}

	float CTrailObject::Noise(glm::vec2 st)
	{
		return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
	}

	float CTrailObject::PerlinNoise(glm::vec2 st)
	{
		glm::vec2 i = glm::floor(st);
		glm::vec2 f = glm::fract(st);

		// Four corners in 2D of a tile
		float a = Noise(i);
		float b = Noise(i + glm::vec2(1.0f, 0.0f));
		float c = Noise(i + glm::vec2(0.0f, 1.0f));
		float d = Noise(i + glm::vec2(1.0f, 1.0f));

		// Smooth Interpolation

		// Cubic Hermine Curve.  Same as SmoothStep()
		glm::vec2 u = f * f * (3.0f - 2.0f * f);
		// u = smoothstep(0.,1.,f);

		// Mix 4 coorners percentages
		return glm::mix(a, b, u.x) +
			(c - a) * u.y * (1.0f - u.x) +
			(d - b) * u.x * u.y;
	}

	void CTrailObject::Init()
	{
		m_Mesh = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::SPHERE,
			RenderingSurfaceType::RASTERIZER,
			std::string({
				#include "../shaders/Trail.vert"	
			}),
			std::string({
				#include "../shaders/Trail.frag"	
			})
		);

		m_TrailGPGPU = std::make_shared<Material>(
			RenderingSurfaceType::RASTERIZER,
			"", "", "", "", "",
			std::string({
				#include "../shaders/Trail.comp"	
			})
		);

		m_TrailBuffer = std::make_shared<ComputeBuffer>(m_DomainCount * m_InstanceNumPerDomain * sizeof(STrailData));
		InitBuffer();
		LinkBuffer();
	}

	void CTrailObject::InitBuffer()
	{
		std::vector<STrailData> InitTrailDataList;

		for (int d = 0; d < m_DomainCount; d++)
		{
			for (int i = 0; i < m_InstanceNumPerDomain; i++)
			{
				float id = static_cast<float>(m_InstanceNumPerDomain * d + i);
				glm::vec4 pos = glm::vec4(
					m_WallHalfSize.x * (Noise(glm::vec2(id, 0.741f)) * 2.0f - 1.0f),
					m_WallHalfSize.y * (Noise(glm::vec2(id, 942.0f)) * 2.0f - 1.0f),
					m_WallHalfSize.z * (Noise(glm::vec2(id, 77.7f)) * 2.0f - 1.0f),
					1.0f
				);
				glm::vec4 s = glm::vec4(0.25f);
				glm::vec4 col = glm::vec4(Noise(glm::vec2(id, id + 11.1f)), Noise(glm::vec2(id + 95.0f, id)), Noise(glm::vec2(id, 66.6)), 1.0f);
				glm::vec4 vel = glm::vec4(Noise(glm::vec2(id + 2.622f)) * 2.0f - 1.0f, Noise(glm::vec2(id + 55.12f, id  + id)) * 2.0f - 1.0f, Noise(glm::vec2(id + 66.6)) * 2.0f - 1.0f, 1.0f);

				STrailData data = {
					{pos.x, pos.y, pos.z, pos.w},
					{0.0f, 0.0f, 0.0f, 0.0f},
					{s.x, s.y, s.z, s.w},
					{col.x, col.y, col.z, col.w},
					{vel.x, vel.y, vel.z, vel.w}
				};

				InitTrailDataList.push_back(data);
			}
		}

		//
		m_TrailBuffer->SetData<std::vector<STrailData>>(InitTrailDataList);
	}
	
	void CTrailObject::LinkBuffer()
	{
		m_Mesh->m_material->SetBufferToMat(m_TrailBuffer, 0);
		m_TrailGPGPU->SetBufferToCS(m_TrailBuffer, 0);
	}

	void CTrailObject::Update()
	{
		/*float t = GraphicsMain::GetInstance()->m_SecondsTime, s = 2.0f;
		float n = PerlinNoise(glm::vec2(t, 0.957841f)) * 2.0f - 1.0f;
		float r = 10.0f * n;
		m_Mesh->m_transform->m_position = glm::vec3(r * glm::cos(t * s), r * glm::sin(t * s), 0.0f);*/

		m_TrailGPGPU->SetActive();
		m_TrailGPGPU->SetFloatUniform("_time", GraphicsMain::GetInstance()->m_SecondsTime);
		m_TrailGPGPU->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
		m_TrailGPGPU->SetVec4Uniform("_WallHalfSize", m_WallHalfSize);
		m_TrailGPGPU->Dispatch(m_DomainCount * m_InstanceNumPerDomain / 256, 1, 1);
	}
	void CTrailObject::Draw()
	{
		m_Mesh->Draw([]() {}, GL_TRIANGLES, true, m_DomainCount * m_InstanceNumPerDomain);
	}
}