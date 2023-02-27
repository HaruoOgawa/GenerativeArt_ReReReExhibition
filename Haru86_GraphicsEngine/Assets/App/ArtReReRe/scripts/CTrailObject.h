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

	// ����̓Z�O�����g�̏��Ԃ͌Œ�ŒP���ɂЂƂO�̃Z�O�����g�̈ʒu���Q�Ƃ���`�ɂ��Ă݂�
	// �����ăZ�O�����g��0�Ԗڂ̂݃��C���̉~�̈ʒu���Q�Ƃ���悤�ɂ���
	// �Z�O�����g�����ł���قǓ��߂���
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
		unsigned int m_DomainCount; // ������h���[�R�[�����Ƃ��Ă������̂����H
		unsigned int m_TrailNumPerDomain;
		unsigned int m_TrailSegmentNum;
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