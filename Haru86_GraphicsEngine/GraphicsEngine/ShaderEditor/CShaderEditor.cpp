#include "CShaderEditor.h"
#include "CShaderCode.h"
#include "GraphicsEngine/Text/TextObject.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/Texture.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

namespace editor
{
	CShaderEditor::CShaderEditor():
		m_FontSize(0.01f),
		m_CurerentShaderIndex(0),
		m_MaxDrawCount(75),
		m_EditorRenderer(nullptr),
		m_IsDraw(false),
		m_EditorOffset(0.0f),
		m_wCharH(m_WriteRange),
		m_SumOfMainDelta(0.0f)
	{
		m_EditorRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,RenderingSurfaceType::RASTERIZER,
			shaderlib::Standard_vert, shaderlib::Standard_frag
		);

		m_EditorRenderer->useZTest = false;
		m_EditorRenderer->useAlphaTest = true;
	}

	CShaderEditor::~CShaderEditor()
	{
		m_ShaderNameList.clear();
		m_ShaderCodeList.clear();
	}

	void CShaderEditor::AddShaderMap(const std::string& ShaderName, const std::string& ShaderCode)
	{
		m_ShaderNameList.push_back(ShaderName);
		m_ShaderCodeList.push_back(std::make_shared<CShaderCode>(ShaderCode));
	}

	// ���A���^�C���Ńe�L�X�g���C����`�悷��̂͗��΂ɏd������̂Ńv�������_���Ă���
	void CShaderEditor::CreatePreCodeLineTexture()
	{
		//
		for (auto& ShaderCode : m_ShaderCodeList)
		{
			// �ϐ�����
			int CodeLineSize = static_cast<int>(ShaderCode->GetCodeLineList().size());
			float xOffset = 0.05f, yAdjust = 1.75f;
			std::vector<int> DrawLineIndexList;
			unsigned int FrameIndex = 0;

			m_FrameTextureList.push_back(std::make_shared<Texture>());
			DrawLineIndexList.push_back(0);

			// �J��Ԃ���
			m_BufferYRepeatNum.push_back(CodeLineSize / m_MaxDrawCount);

			// ���k��(�X�N���[�����W�n��-1 ~ 1�̕`��͈͂ɍ��킹�邽�߂Ɏ��k����K�v������B�t���[���o�b�t�@�̃T�C�Y�Ƃ͖��֌W)
			const float ShrinkRate = (1.0f / static_cast<float>(m_BufferYRepeatNum.back() * 2));
			m_ShrinkRateList.push_back(ShrinkRate);

			// �t���[���o�b�t�@�𐶐�
			const float Resolution = 1.0f / ShrinkRate;
			const auto& ScreenSize = GraphicsRenderer::GetInstance()->GetScreenSize();
			glm::ivec2 BufferSize = glm::ivec2(static_cast<int>(ScreenSize.x * Resolution), static_cast<int>(ScreenSize.y * Resolution));

			GraphicsRenderer::GetInstance()->CreateFrameBuffer(BufferSize.x, BufferSize.y, m_FrameTextureList.back(), FrameIndex, GL_RGBA, GL_RGBA);
		
			// �`�悷�镶����̈ʒu������
			while (DrawLineIndexList.back() < CodeLineSize)
			{
				DrawLineIndexList.push_back(DrawLineIndexList.back() + 1);

				// �X�N���[������
				for (const auto& DrawLineIndex : DrawLineIndexList)
				{
					if (DrawLineIndex >= CodeLineSize) break;
					auto& TextLine = ShaderCode->GetCodeLineList()[(CodeLineSize - 1) - DrawLineIndex];
					TextLine.m_Pos.y -= m_FontSize * 2.0f * yAdjust;
				}
			}

			// �t���[���o�b�t�@�ɕ`�悷��
			//
			GraphicsMain::GetInstance()->renderingTarget = ERerderingTarget::COLOR;
			glBindFramebuffer(GL_FRAMEBUFFER, FrameIndex);
			GraphicsMain::GetInstance()->m_TargetFrameIndex = FrameIndex;
			glViewport(0, 0, BufferSize.x, BufferSize.y);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// �`��
			for (const auto& DrawLineIndex : DrawLineIndexList)
			{
				if (DrawLineIndex >= CodeLineSize) break;

				const auto& TextLine = ShaderCode->GetCodeLineList()[DrawLineIndex];
				text::TextObject::Draw(TextLine.m_TextLine, m_FontSize * ShrinkRate, 1.15f, 3.0f, 
					glm::vec3(TextLine.m_Pos.x + xOffset * ShrinkRate, 1.0f + TextLine.m_Pos.y * ShrinkRate, TextLine.m_Pos.z),
					glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true, true, DrawLineIndex, TextLine.m_ColorList, ShrinkRate);
			}
		}

		// ShaderCode�͂����g��Ȃ��̂ŉ������
		//m_ShaderCodeList.clear();
	}

	void CShaderEditor::Update()
	{
		//
		int PrevShaderIndex = m_CurerentShaderIndex;
		m_CurerentShaderIndex = GraphicsMain::GetInstance()->GetAppSceneIndex();
		m_IsDraw = true;

		// �V�[�����؂�ς�����΂���Ȃ�l�����Z�b�g����
		if (m_CurerentShaderIndex != PrevShaderIndex)
		{
			m_IsDraw = false;
			m_EditorOffset = 0.0f;
			m_wCharH = m_WriteRange;
			m_SumOfMainDelta = 0.0f;

			return;
		}

		//
		float time = GraphicsMain::GetInstance()->m_SecondsTime, sTime = GraphicsMain::GetInstance()->GetSceneStartTime(), eTime = GraphicsMain::GetInstance()->GetSceneEndTime();
		float SceneProgress = glm::clamp((time - sTime) / (eTime - sTime), 0.0f, 1.0f);
		
		const auto& ShaderCode = m_ShaderCodeList[m_CurerentShaderIndex];
		int NumofRow = ShaderCode->GetCodeLineList().size();
		float RowDeltaTime = (eTime - sTime) / static_cast<float>(NumofRow);

		// ���C�u�R�[�f�B���O�ƃX�N���[���̉��o
		m_SumOfMainDelta += GraphicsMain::GetInstance()->m_DeltaTime;

		if (m_SumOfMainDelta >= RowDeltaTime)
		{
			m_SumOfMainDelta = 0.0f;

			// 18�s�̃��C����0.3�̒������g�p����
			m_wCharH -= (m_WriteRange / 18.0);
		}

		if (m_wCharH <= 0.0f)
		{
			m_wCharH = m_WriteRange;
			m_EditorOffset = -1.0f * SceneProgress * m_BufferYRepeatNum[m_CurerentShaderIndex] * m_ShrinkRateList[m_CurerentShaderIndex];
		}
	}

	void CShaderEditor::Draw()
	{
		if (m_EditorRenderer && m_IsDraw)
		{
			m_EditorRenderer->Draw([&]() {
				m_EditorRenderer->m_material->SetVec4Uniform("_Color", glm::vec4(1.0f));
				m_EditorRenderer->m_material->SetIntUniform("_IsMOnly", 1);
				m_EditorRenderer->m_material->SetIntUniform("_IsMulMatOnVert", 0);
				m_EditorRenderer->m_material->SetIntUniform("_UseLighting", 0);
				m_EditorRenderer->m_material->SetIntUniform("_UseMainEditorTex", 1);
				m_FrameTextureList[m_CurerentShaderIndex]->SetActive(GL_TEXTURE0);
				m_EditorRenderer->m_material->SetIntUniform("_MainTex", 0);
				m_EditorRenderer->m_material->SetFloatUniform("_EditorOffset", m_EditorOffset);
				m_EditorRenderer->m_material->SetFloatUniform("_ShrinkRate", m_ShrinkRateList[m_CurerentShaderIndex]);
				m_EditorRenderer->m_material->SetFloatUniform("_FontSize", m_FontSize);
				m_EditorRenderer->m_material->SetFloatUniform("_wCharH", m_wCharH);
			}, GL_TRIANGLES, false, 0);
			m_FrameTextureList[m_CurerentShaderIndex]->SetEnactive(GL_TEXTURE0);
		}
	}
}