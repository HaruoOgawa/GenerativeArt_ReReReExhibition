#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>

namespace text {
	static class TextObject
	{
	public:
		static void Draw(const std::string& Text, float FontSize = 0.1f, float WAdjust = 1.0f, float HAdjust = 1.0f,
			const glm::vec3& Pos = glm::vec3(0.0f), const glm::vec4& Color = glm::vec4(1.0f), bool IsTextAlignLeft = false, bool IsUseBack = false,
			int LineNumber = 0, const std::vector<glm::vec4>& ColorList = std::vector<glm::vec4>(), const float ShrinkRate = 1.0f);
	};
}