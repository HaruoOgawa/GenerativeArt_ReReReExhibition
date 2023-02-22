#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <vector>

class MeshRendererComponent;

namespace text
{
	class TextRenderer
	{
		std::shared_ptr<MeshRendererComponent> m_TextMeshRenderer;
	private:
	public:
		TextRenderer();
		virtual ~TextRenderer();

		void Draw(const std::string& Text, float FontSize, float WAdjust, float HAdjust, const glm::vec3& Pos, const glm::vec4& Color, 
			bool IsTextAlignLeft, bool IsUseBack, int LineNumber, const std::vector<glm::vec4>& ColorList, const float ShrinkRate);
	};
}

