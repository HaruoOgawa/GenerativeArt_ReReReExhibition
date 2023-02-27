#include "CTrailObject.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Graphics/ComputeBuffer.h"

namespace app
{
	CTrailObject::CTrailObject() :
		m_TrailMesh(nullptr),
		m_TrailBuffer(nullptr),
		m_TrailGPGPU(nullptr),
		m_SegmentMesh(nullptr),
		m_SegmentBuffer(nullptr),
		m_SegmentGPGPU(nullptr),
		m_DomainCount(1),
		m_TrailNumPerDomain(1024),
		m_TrailSegmentNum(64),
		m_WallHalfSize(glm::vec4(50.0f, 25.0f, 25.0f,1.0f)),
		m_ThreadNum(1024, 1, 1)
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
		// Trail
		m_TrailMesh = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::CIRCLE,
			RenderingSurfaceType::RASTERIZER,
			std::string({
				#include "../shaders/Trail.vert"	
			}),
			shaderlib::Standard_frag
		);

		m_TrailGPGPU = std::make_shared<Material>(
			RenderingSurfaceType::RASTERIZER,
			"", "", "", "", "",
			std::string({
				#include "../shaders/Trail.comp"	
			})
		);

		// Segment
		m_SegmentMesh = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::POINT,
			RenderingSurfaceType::RASTERIZER,
			std::string({
				#include "../shaders/Segment.vert"	
			}),
			shaderlib::Standard_frag,
			std::string({
				#include "../shaders/Segment.geom"	
			})
		);

		m_SegmentGPGPU = std::make_shared<Material>(
			RenderingSurfaceType::RASTERIZER,
			"", "", "", "", "",
			std::string({
				#include "../shaders/Segment.comp"	
			})
		);

		// Buffer
		m_TrailBuffer = std::make_shared<ComputeBuffer>(m_DomainCount * m_TrailNumPerDomain * sizeof(STrailData));
		m_SegmentBuffer = std::make_shared<ComputeBuffer>(m_DomainCount * m_TrailNumPerDomain * m_TrailSegmentNum * sizeof(SSegmentData));

		InitBuffer();
		LinkBuffer();
	}

	void CTrailObject::InitBuffer()
	{
		// Trail ///////////////////////////////////
		std::vector<STrailData> InitTrailDataList;

		for (int d = 0; d < m_DomainCount; d++)
		{
			for (int t = 0; t < m_TrailNumPerDomain; t++)
			{
				float id = static_cast<float>(m_TrailNumPerDomain * d + t);
				glm::vec4 pos = glm::vec4(
					m_WallHalfSize.x * (Noise(glm::vec2(id, 0.741f)) * 2.0f - 1.0f),
					m_WallHalfSize.y * (Noise(glm::vec2(id, 942.0f)) * 2.0f - 1.0f),
					m_WallHalfSize.z * (Noise(glm::vec2(id, 77.7f)) * 2.0f - 1.0f),
					1.0f
				);
				glm::vec4 scl = glm::vec4(0.25f);
				glm::vec4 col = glm::vec4(Noise(glm::vec2(id, id + 11.1f)), Noise(glm::vec2(id + 95.0f, id)), Noise(glm::vec2(id, 66.6)), 1.0f);
				glm::vec4 vel = glm::vec4(Noise(glm::vec2(id + 2.622f)) * 2.0f - 1.0f, Noise(glm::vec2(id + 55.12f, id  + id)) * 2.0f - 1.0f, Noise(glm::vec2(id + 66.6)) * 2.0f - 1.0f, 1.0f);

				STrailData data = {
					{pos.x, pos.y, pos.z, pos.w},
					{0.0f, 0.0f, 0.0f, 0.0f},
					{scl.x, scl.y, scl.z, scl.w},
					{col.x, col.y, col.z, col.w},
					{vel.x, vel.y, vel.z, vel.w}
				};

				InitTrailDataList.push_back(data);
			}
		}

		m_TrailBuffer->SetData<std::vector<STrailData>>(InitTrailDataList);

		// Segment ////////////////////////////////////////////////
		std::vector<SSegmentData> InitSegmentData;

		for (int d = 0; d < m_DomainCount; d++)
		{
			for (int t = 0; t < m_TrailNumPerDomain; t++)
			{
				for (int s = 0; s < m_TrailSegmentNum; s++)
				{
					int TrailIndex = m_TrailNumPerDomain * d + t;
					int SegmentIndex = s;
					const auto& Trail = InitTrailDataList[TrailIndex];

					SSegmentData data = {
						{Trail.m_Pos[0], Trail.m_Pos[1], Trail.m_Pos[2], Trail.m_Pos[3]},
						{Trail.m_Rotate[0], Trail.m_Rotate[1], Trail.m_Rotate[2], Trail.m_Rotate[3]},
						{Trail.m_Scale[0], Trail.m_Scale[1], Trail.m_Scale[2], Trail.m_Scale[3]},
						TrailIndex,
						SegmentIndex,
						0,
						0
					};

					InitSegmentData.push_back(data);
				}
			}
		}

		m_SegmentBuffer->SetData<std::vector<SSegmentData>>(InitSegmentData);
	}
	
	void CTrailObject::LinkBuffer()
	{
		// Trail
		m_TrailMesh->m_material->SetBufferToMat(m_TrailBuffer, 0);
		m_TrailGPGPU->SetBufferToCS(m_TrailBuffer, 0);

		// Segment
		m_SegmentMesh->m_material->SetBufferToMat(m_SegmentBuffer, 1);
		m_SegmentGPGPU->SetBufferToCS(m_TrailBuffer, 0);
		m_SegmentGPGPU->SetBufferToCS(m_SegmentBuffer, 1);
	}

	void CTrailObject::Update()
	{
		// Trail
		m_TrailGPGPU->SetActive();
		m_TrailGPGPU->SetFloatUniform("_time", GraphicsMain::GetInstance()->m_SecondsTime);
		m_TrailGPGPU->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
		m_TrailGPGPU->SetVec4Uniform("_WallHalfSize", m_WallHalfSize);
		m_TrailGPGPU->Dispatch(m_DomainCount * m_TrailNumPerDomain / m_ThreadNum.x, 1, 1);
		
		// Segment
		m_SegmentGPGPU->SetActive();
		m_SegmentGPGPU->SetFloatUniform("_time", GraphicsMain::GetInstance()->m_SecondsTime);
		m_SegmentGPGPU->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
		m_SegmentGPGPU->SetIntUniform("_SegmentNum", m_TrailSegmentNum);
		m_SegmentGPGPU->Dispatch(m_DomainCount * m_TrailNumPerDomain * m_TrailSegmentNum / m_ThreadNum.x, 1, 1);
	}
	void CTrailObject::Draw()
	{
		// Trail
		m_TrailMesh->Draw([&]() {
			m_TrailMesh->m_material->SetIntUniform("_Use2FColor", 1);
		}, GL_TRIANGLES, true, m_DomainCount * m_TrailNumPerDomain);

		// Segment
		m_SegmentMesh->Draw([&]() {
			m_SegmentMesh->m_material->SetIntUniform("_SegmentNum", m_TrailSegmentNum);
		}, GL_POINTS, true, m_DomainCount * m_TrailNumPerDomain * m_TrailSegmentNum);
	}
}