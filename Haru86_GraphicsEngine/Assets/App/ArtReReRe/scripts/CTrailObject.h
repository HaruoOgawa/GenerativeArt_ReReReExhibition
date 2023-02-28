#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class MeshRendererComponent;
class ComputeBuffer;
class Material;

namespace app
{
	struct SFlowData
	{
		float Data[4]; // pos.x, pos.y, pos.z, angle
	};

	struct STrailData
	{
		float m_Pos[4];
		float m_Rotate[4];
		float m_Scale[4];
		float m_Color[4];
		float m_Velocity[4];
		float m_TargetPos[4];
		float m_Data[4]; // IsRelocation, none, none, none
	};

	// 今回はセグメントの順番は固定で単純にひとつ前のセグメントの位置を参照する形にしてみる
	// そしてセグメントの0番目のみメインの円の位置を参照するようにする
	// セグメントが後ろであるほど透過する
	struct SSegmentData 
	{
		float m_Pos[4];
		float m_Rotate[4];
		float m_Scale[4];
		int   m_TrailIndex;
		int   m_SegmentIndex;
		int   m_Padding0;
		int   m_Padding1;
	};

	class CTrailObject
	{
		// FlowFields
		std::shared_ptr<MeshRendererComponent> m_FlowFieldsMesh;
		std::shared_ptr<ComputeBuffer> m_FlowFieldsBuffer;
		std::shared_ptr<Material> m_FlowFieldsGPGPU;

		const float m_FlowGridX = 64;
		const float m_FlowGridY = 64;
		float		m_FlowCellSize;
		const glm::ivec3	m_FlowThreads = glm::ivec3(32, 32, 1);

		const float m_StepLength = 1.0f;
		const float m_StepSpeed = 1.0f;

		// Trail
		std::shared_ptr<MeshRendererComponent> m_TrailMesh;
		std::shared_ptr<ComputeBuffer> m_TrailBuffer;
		std::shared_ptr<Material> m_TrailGPGPU;

		// Segment
		std::shared_ptr<MeshRendererComponent> m_SegmentMesh;
		std::shared_ptr<ComputeBuffer> m_SegmentBuffer;
		std::shared_ptr<Material> m_SegmentGPGPU;

		//
		glm::vec4 m_WallHalfSize;
		glm::ivec3 m_ThreadNum;
		unsigned int m_DomainCount; // これをドローコール数としてもいいのかも？
		unsigned int m_TrailNumPerDomain;
		unsigned int m_TrailSegmentNum;

		float m_Radius;
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

		const glm::vec4& GetWallHalfSize() const { return m_WallHalfSize; }
	};
}