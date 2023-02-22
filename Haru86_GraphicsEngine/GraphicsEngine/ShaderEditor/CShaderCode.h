#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace editor
{
	struct CShaderCodeLine
	{
		std::string m_TextLine;
		glm::vec3   m_Pos;
		std::vector<glm::vec4> m_ColorList;
	};

	class CShaderCode
	{
		std::vector<CShaderCodeLine> m_CodeLineList;

		// ç\ï∂âêÕópÇÃîzóÒ
		const std::vector<std::string> m_BuiltInTypes = { "int", "bool", "float",  "vec2", "vec3", "vec4", "mat2", "mat3", "mat4", "void",
			"version", "define", "uniform", "layout","in", "out", "flat", "extension", "if", "else"};
		const std::vector<std::string> m_BuiltInVal = { "gl_FragColor", "gl_FragCoord", "true", "false"};
		const std::vector<std::string> m_BuiltInFunc = { "abs", "sign", "floor", "ceil", "fract", "mod",
			"min", "max", "clamp", "mix", "step", "smoothstep", "sin", "cos", "asin", "acos", "atan", "pow", "exp", "log", "exp2", "log2", "sqrt",
			"length", "distance", "normalize", "dot", "cross", "reflect", "refract" };
		const std::vector<char> m_SymbolList = { '!', '#', '%', '&', '(', ')', '*', '+', ',', '-', '/', ':', ';', '<', '=', '>', '?',
			'{' , '|' , '}' };
		const std::vector<char> m_BuiltInNumber = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
	public:
		CShaderCode(const std::string& Code);
		std::vector<CShaderCodeLine>& GetCodeLineList() { return m_CodeLineList; }
	};
}