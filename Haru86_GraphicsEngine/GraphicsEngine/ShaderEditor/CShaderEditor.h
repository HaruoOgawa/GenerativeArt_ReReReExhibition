#pragma once
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

class Texture;
class MeshRendererComponent;

namespace editor
{
	class CShaderCode;
	struct CShaderCodeLine;

	class CShaderEditor
	{
		//
		std::vector<std::string> m_ShaderNameList;
		std::vector<std::shared_ptr<CShaderCode>> m_ShaderCodeList;
		
		std::vector<std::shared_ptr<Texture>> m_FrameTextureList;
		std::shared_ptr<MeshRendererComponent> m_EditorRenderer;
		std::vector<int> m_BufferYRepeatNum;
		std::vector<float> m_ShrinkRateList;

		//
		float m_FontSize;
		int m_CurerentShaderIndex;
		int m_MaxDrawCount;
		float m_EditorOffset;

		bool m_IsDraw;
		float m_WriteRange = 0.3f;
		float m_wCharH;
		float m_SumOfMainDelta;
	public:
		CShaderEditor();
		virtual ~CShaderEditor();

		void AddShaderMap(const std::string& ShaderName, const std::string& ShaderCode);
		void CreatePreCodeLineTexture();

		void Update();
		void Draw();
	};
}
