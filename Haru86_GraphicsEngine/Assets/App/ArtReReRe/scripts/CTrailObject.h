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
		float Pos[4]; // pos.x, pos.y, pos.z, 0.0
		float Angle[4]; // x axis angle, 0.0, z axis angle, 0.0
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
		float m_Color[4];
		int   m_TrailIndex;
		int   m_SegmentIndex;
		int   m_Padding0;
		int   m_Padding1;
	};

	class CTrailObject
	{
		// GPGPU Param
		glm::ivec3 m_ThreadNum;
		glm::ivec3	m_FlowThreads;
		glm::vec4 m_WallHalfSize;

		// FlowFields Param
#ifdef _DEBUG
		std::shared_ptr<MeshRendererComponent> m_FlowFieldsMesh;
#endif // _DEBUG
		std::shared_ptr<ComputeBuffer> m_FlowFieldsBuffer;
		std::shared_ptr<Material> m_FlowFieldsGPGPU;

		float m_FlowGridX;
		float m_FlowGridY;
		float m_FlowGridZ;
		float m_FlowCellSize;

		glm::vec2 m_NoiseScale;
		glm::vec2 m_NoiseOctaves;
		glm::vec2 m_NoiseOffset;
		glm::vec2 m_AngleScale;
		glm::vec4 m_Seed;

		// Trail Param
		std::shared_ptr<MeshRendererComponent> m_TrailMesh;
		std::shared_ptr<ComputeBuffer> m_TrailBuffer;
		std::shared_ptr<Material> m_TrailGPGPU;

		// Segment Param
		std::shared_ptr<MeshRendererComponent> m_SegmentMesh;
		std::shared_ptr<ComputeBuffer> m_SegmentBuffer;
		std::shared_ptr<Material> m_SegmentGPGPU;

		// Art Param
		unsigned int m_DomainCount;
		unsigned int m_TrailNumPerDomain; // Curve Count
		unsigned int m_TrailSegmentNum;

		float m_StepLength; // Curve Length
		float m_StepSpeed;
		float m_CurveAlpha; // Curve Alpha
		float m_CurveTickness; // Curve Tickness
	private:
		static float Noise(glm::vec2 st);

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