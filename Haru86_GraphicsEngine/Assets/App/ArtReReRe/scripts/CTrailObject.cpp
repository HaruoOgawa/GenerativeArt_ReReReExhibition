#include "CTrailObject.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Graphics/ComputeBuffer.h"

namespace app
{
	CTrailObject::CTrailObject() :
#ifdef _DEBUG
		m_FlowFieldsMesh(nullptr),
#endif // _DEBUG
		m_FlowFieldsBuffer(nullptr),
		m_FlowFieldsGPGPU(nullptr),

		m_TrailMesh(nullptr),
		m_TrailBuffer(nullptr),
		m_TrailGPGPU(nullptr),

		m_SegmentMesh(nullptr),
		m_SegmentBuffer(nullptr),
		m_SegmentGPGPU(nullptr),

		m_ThreadNum(1024, 1, 1),
		m_FlowThreads(8, 8, 8),
		m_WallHalfSize(glm::vec4(200.0f, 100.0f, 50.0f, 1.0f)),

		m_FlowGridX(128.0f),
		m_FlowGridY(128.0f),
		m_FlowGridZ(128.0f),
		m_FlowCellSize(1.0f),
		m_NoiseScale(glm::vec2(1.0f, 1.0f)),
		m_NoiseOctaves(glm::vec2(2.0f, 2.0f)),
		m_NoiseOffset(glm::vec2(0.0f, 0.0f)),
		m_AngleScale(glm::vec2(2.0f, 2.0f)),
		m_Seed(glm::vec4(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f))),
		
		m_DomainCount(16),
		m_TrailNumPerDomain(1024),
		m_TrailSegmentNum(64),
		m_StepLength(1.0f),
		m_StepSpeed(1.0f),
		m_CurveAlpha(1.0f),
		m_CurveTickness(0.25f)
	{
	}

	CTrailObject::~CTrailObject()
	{
	}

	float CTrailObject::Noise(glm::vec2 st)
	{
		return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
	}

	void CTrailObject::Init()
	{
		// FlowFields
#ifdef _DEBUG
		m_FlowFieldsMesh = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::ARROW,
			RenderingSurfaceType::RASTERIZER,
			std::string({
				#include "../shaders/Flow.vert"	
				}),
				shaderlib::Standard_frag
				);
#endif // _DEBUG

		m_FlowFieldsGPGPU = std::make_shared<Material>(
			RenderingSurfaceType::RASTERIZER,
			"", "", "", "", "",
			std::string({
				#include "../shaders/Flow.comp"	
			})
		);

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

		m_TrailMesh->useAlphaTest = false;

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

		m_SegmentMesh->useAlphaTest = false;

		m_SegmentGPGPU = std::make_shared<Material>(
			RenderingSurfaceType::RASTERIZER,
			"", "", "", "", "",
			std::string({
				#include "../shaders/Segment.comp"	
			})
		);

		// Buffer
		m_FlowFieldsBuffer = std::make_shared<ComputeBuffer>(m_FlowGridX * m_FlowGridY * m_FlowGridZ * sizeof(SFlowData));
		m_TrailBuffer = std::make_shared<ComputeBuffer>(m_DomainCount * m_TrailNumPerDomain * sizeof(STrailData));
		m_SegmentBuffer = std::make_shared<ComputeBuffer>(m_DomainCount * m_TrailNumPerDomain * m_TrailSegmentNum * sizeof(SSegmentData));

		InitBuffer();
		LinkBuffer();
	}

	void CTrailObject::InitBuffer()
	{
		// Flow ///////////////////////////////////
		std::vector<SFlowData> InitFlowData;
		for (float z = 0.0f; z < m_FlowGridZ; z++)
		{
			for (float y = 0.0f; y < m_FlowGridY; y++)
			{
				for (float x = 0.0f; x < m_FlowGridX; x++)
				{
					glm::vec3 pos = glm::vec3(x / (m_FlowGridX - 1.0f), y / (m_FlowGridY - 1.0f), z / (m_FlowGridZ - 1.0f));
					pos = pos * 2.0f - 1.0f;
					pos.x *= m_WallHalfSize.x;
					pos.y *= m_WallHalfSize.y;
					pos.z *= m_WallHalfSize.z;

					SFlowData data = {
						{pos.x, pos.y, pos.z, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f}
					};

					InitFlowData.push_back(data);
				}
			}
		}
		

		m_FlowCellSize = m_WallHalfSize.x / m_FlowGridX;
		m_FlowFieldsBuffer->SetData<std::vector<SFlowData>>(InitFlowData);

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
				glm::vec4 scl = glm::vec4(0.5f);
				glm::vec4 col = glm::vec4(Noise(glm::vec2(id, id + 11.1f)), Noise(glm::vec2(id + 95.0f, id)), Noise(glm::vec2(id, 66.6)), 0.5f);
				//glm::vec4 vel = glm::vec4(Noise(glm::vec2(id + 2.622f)) * 2.0f - 1.0f, Noise(glm::vec2(id + 55.12f, id  + id)) * 2.0f - 1.0f, Noise(glm::vec2(id + 66.6)) * 2.0f - 1.0f, 1.0f);
				glm::vec4 vel = glm::vec4(0.0f);

				STrailData data = {
					{pos.x, pos.y, pos.z, pos.w},
					{0.0f, 0.0f, 0.0f, 0.0f},
					{scl.x, scl.y, scl.z, scl.w},
					{col.x, col.y, col.z, col.w},
					{vel.x, vel.y, vel.z, vel.w},
					{pos.x, pos.y, pos.z, pos.w},
					{0.0f, 0.0f, 0.0f, 0.0f}
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
						{0.0f, 0.0f, 0.0f, 0.0f},
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
		// FlowFields
#ifdef _DEBUG
		m_FlowFieldsMesh->m_material->SetBufferToMat(m_FlowFieldsBuffer, 2);
#endif // _DEBUG
		m_FlowFieldsGPGPU->SetBufferToCS(m_FlowFieldsBuffer, 2);

		// Trail
		m_TrailMesh->m_material->SetBufferToMat(m_TrailBuffer, 0);
		m_TrailGPGPU->SetBufferToCS(m_TrailBuffer, 0);
		m_TrailGPGPU->SetBufferToCS(m_FlowFieldsBuffer, 2);

		// Segment
		m_SegmentMesh->m_material->SetBufferToMat(m_SegmentBuffer, 1);
		m_SegmentGPGPU->SetBufferToCS(m_TrailBuffer, 0);
		m_SegmentGPGPU->SetBufferToCS(m_SegmentBuffer, 1);
	}

	void CTrailObject::Update()
	{
		// シードを更新
		float t = GraphicsMain::GetInstance()->m_SecondsTime;
		m_Seed = glm::vec4(
			glm::vec2(t * 0.1f, Noise(glm::vec2(glm::floor(t * 0.1f), 0.945f))), 
			glm::vec2(Noise(glm::vec2(8.77f, glm::floor(t * 0.25f))), t * 0.25f));

		// FlowFields
		m_FlowFieldsGPGPU->SetActive();
		m_FlowFieldsGPGPU->SetFloatUniform("_time", t);
		m_FlowFieldsGPGPU->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
		m_FlowFieldsGPGPU->SetVec4Uniform("_WallHalfSize", m_WallHalfSize);
		m_FlowFieldsGPGPU->SetIntUniform("_FlowGridX", static_cast<int>(m_FlowGridX));
		m_FlowFieldsGPGPU->SetIntUniform("_FlowGridY", static_cast<int>(m_FlowGridY));
		m_FlowFieldsGPGPU->SetIntUniform("_FlowGridZ", static_cast<int>(m_FlowGridZ));

		m_FlowFieldsGPGPU->SetVec2Uniform("_NoiseScale", m_NoiseScale);
		m_FlowFieldsGPGPU->SetVec2Uniform("_NoiseOctaves", m_NoiseOctaves);
		m_FlowFieldsGPGPU->SetVec2Uniform("_NoiseOffset", m_NoiseOffset);
		m_FlowFieldsGPGPU->SetVec2Uniform("_AngleScale", m_AngleScale);
		m_FlowFieldsGPGPU->SetVec4Uniform("_Seed", m_Seed);
		m_FlowFieldsGPGPU->Dispatch(m_FlowGridX / m_FlowThreads.x, m_FlowGridY / m_FlowThreads.y, m_FlowGridZ / m_FlowThreads.z);

		// Trail
		m_TrailGPGPU->SetActive();
		m_TrailGPGPU->SetFloatUniform("_time", t);
		m_TrailGPGPU->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
		m_TrailGPGPU->SetVec4Uniform("_WallHalfSize", m_WallHalfSize);
		m_TrailGPGPU->SetFloatUniform("_FlowGridX", m_FlowGridX);
		m_TrailGPGPU->SetFloatUniform("_FlowGridY", m_FlowGridY);
		m_TrailGPGPU->SetFloatUniform("_FlowGridZ", m_FlowGridZ);
		m_TrailGPGPU->SetFloatUniform("_StepLength", m_StepLength);
		m_TrailGPGPU->SetFloatUniform("_StepSpeed", m_StepSpeed);
		m_TrailGPGPU->Dispatch(m_DomainCount * m_TrailNumPerDomain / m_ThreadNum.x, 1, 1);
		
		// Segment
		m_SegmentGPGPU->SetActive();
		m_SegmentGPGPU->SetFloatUniform("_time", t);
		m_SegmentGPGPU->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
		m_SegmentGPGPU->SetIntUniform("_SegmentNum", m_TrailSegmentNum);
		m_SegmentGPGPU->Dispatch(m_DomainCount * m_TrailNumPerDomain * m_TrailSegmentNum / m_ThreadNum.x, 1, 1);
	}
	void CTrailObject::Draw()
	{
		// Debug FlowFields
#ifdef _DEBUG
		/*m_FlowFieldsMesh->Draw([&]() {
			m_FlowFieldsMesh->m_material->SetIntUniform("_Use2FColor", 1);
			m_FlowFieldsMesh->m_material->SetFloatUniform("_FlowCellSize", m_FlowCellSize);
		}, GL_TRIANGLES, true, static_cast<int>(m_FlowGridX * m_FlowGridY * m_FlowGridZ));*/
#endif // _DEBUG

		// Trail
		m_TrailMesh->Draw([&]() {
			m_TrailMesh->m_material->SetIntUniform("_Use2FColor", 1);
		}, GL_TRIANGLES, true, m_DomainCount * m_TrailNumPerDomain);

		// Segment
		m_SegmentMesh->Draw([&]() {
			m_SegmentMesh->m_material->SetIntUniform("_SegmentNum", m_TrailSegmentNum);
			m_SegmentMesh->m_material->SetFloatUniform("_Tickness", m_CurveTickness);
			m_SegmentMesh->m_material->SetIntUniform("_Use2FColor", 1);
			m_SegmentMesh->m_material->SetVec4Uniform("_WallHalfSize", m_WallHalfSize);
		}, GL_POINTS, true, m_DomainCount * m_TrailNumPerDomain * m_TrailSegmentNum);
	}
}