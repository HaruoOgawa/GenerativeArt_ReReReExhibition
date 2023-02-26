#include "Material.h"
#include "GraphicsEngine/Graphics/ComputeBuffer.h"
#include "GraphicsEngine/Graphics/Texture.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

Material::Material(RenderingSurfaceType SurfaceType,const std::string& vert, const std::string& frag, const std::string& geom, const std::string& tc, const std::string& tv, const std::string& cs)
	: vertShaderData(-1),
	tessControlShaderData(-1),
	tessEvalShaderData(-1),
	geometryShaderData(-1),
	fragShaderData(-1),
	shaderPrg(-1),
	depthVertShaderData(-1),
	depthTessControlShaderData(-1),
	depthTessEvalShaderData(-1),
	depthGeometryShaderData(-1),
	depthFragShaderData(-1),
	depthShaderPrg(-1),
	computeShaderData(-1),
	computeShaderDepthData(-1)
{
	if (cs.empty()) 
	{
		if (SurfaceType == RenderingSurfaceType::RASTERIZER) {
			LoadShader(vert, frag, geom, tc, tv, cs, shaderPrg
				, vertShaderData, fragShaderData, geometryShaderData, tessControlShaderData, tessEvalShaderData, computeShaderData);
			//LoadShader(vert, shaderlib::DepthColor_frag, geom, tc, tv, cs, depthShaderPrg
				//, depthVertShaderData, depthFragShaderData, depthGeometryShaderData, depthTessControlShaderData, depthTessEvalShaderData, computeShaderDepthData);
		}
		else if (SurfaceType == RenderingSurfaceType::RAYMARCHING) {
			LoadShader(vert, frag, geom, tc, tv, cs, shaderPrg
				, vertShaderData, fragShaderData, geometryShaderData, tessControlShaderData, tessEvalShaderData, computeShaderData);
			LoadShader(vert, frag, geom, tc, tv, cs, depthShaderPrg
				, depthVertShaderData, depthFragShaderData, depthGeometryShaderData, depthTessControlShaderData, depthTessEvalShaderData, computeShaderDepthData);
		}
	}
	else
	{
		LoadShader(vert, frag, geom, tc, tv, cs, shaderPrg
			, vertShaderData, fragShaderData, geometryShaderData, tessControlShaderData, tessEvalShaderData, computeShaderData);
	}
}

Material::~Material() {
	UnLoadData();
}

void Material::LoadShader(const std::string& vert, const std::string& frag, const std::string& geom, const std::string& tc, const std::string& tv, const std::string& cs,
	GLuint& prg, GLuint& vertData, GLuint& fragData, GLuint& geomData, GLuint& tcData, GLuint& tvData, GLuint& csData)
{
	//
	prg = glCreateProgram();

	// 各Shaderのコンパイルとアタッチ
	if (CompileShader(vert, GL_VERTEX_SHADER, vertData)) {
		glAttachShader(prg, vertData);
	}

	if (CompileShader(frag, GL_FRAGMENT_SHADER, fragData)) {
		glAttachShader(prg, fragData);
	}

	if (CompileShader(geom, GL_GEOMETRY_SHADER, geomData)) {
		glAttachShader(prg, geomData);
	}

	if (CompileShader(tc, GL_TESS_CONTROL_SHADER, tcData)) {
		glAttachShader(prg, tcData);
	}

	if (CompileShader(tv, GL_TESS_EVALUATION_SHADER, tvData)) {
		glAttachShader(prg, tvData);
	}
	
	if (CompileShader(cs, GL_COMPUTE_SHADER, csData)) {
		glAttachShader(prg, csData);
	}

	// program dataを完成させる
	glLinkProgram(prg);

	GLint status;
	glGetProgramiv(prg, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(prg, 511, nullptr, buffer);

#ifdef _DEBUG
		Console::Log("Cannot Load Program Data");
		Console::Log("GLSL Link Status:\n%s", buffer);
#endif // _DEBUG
	}
}

bool Material::CompileShader(const std::string shaderCode, GLenum shaderType, GLuint& outShader){
	if (!shaderCode.empty()) {
		outShader = glCreateShader(shaderType);
		const char* contentsChar = shaderCode.c_str();
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);
	}
	else {
		return false;
	}

	GLint status;
	glGetShaderiv(outShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(outShader, 511, nullptr, buffer);
		
#ifdef _DEBUG
		Console::Log("Cannnot Load Shader: %s", shaderCode.c_str());
		Console::Log("GLSL Compile Failed: %s", buffer);
#endif // _DEBUG


		return false;
	}

	return true;
}

void Material::SetActive() {
	auto prg = GetCurrentShaderPrg();
	glUseProgram(prg);
}

void Material::SetMatrixUniform(std::string uniformName, glm::mat4 matrix) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const float*>(&matrix[0][0]));
}

void Material::SetMatrixVectorUniform(std::string uniformName, const std::vector<glm::mat4>& matrixVector) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniformMatrix4fv(location, static_cast<unsigned int>(matrixVector.size()), GL_FALSE, reinterpret_cast<const float*>(&matrixVector[0][0]));
}

void Material::SetFloatUniform(std::string uniformName, float val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform1f(location, val);
}

void Material::SetIntUniform(std::string uniformName, int val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform1i(location, val);
}

void Material::SetVec2Uniform(std::string uniformName, glm::vec2 val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform2fv(location,1, reinterpret_cast<GLfloat*>(&val));
}

void Material::SetVec3Uniform(std::string uniformName, glm::vec3 val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform3fv(location,1, reinterpret_cast<GLfloat*>(&val));
}

void Material::SetVec4Uniform(std::string uniformName, glm::vec4 val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform4fv(location,1, reinterpret_cast<GLfloat*>(&val));
}

void Material::SetVec4ArrayUniform(std::string uniformName, std::vector<float> val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform4fv(location, 1, &val[0]);
}

void Material::SetTexUniform(std::string uniformName, unsigned int val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform1i(location, static_cast<GLuint>(val));
}

void Material::SetFloatVectorUniform(std::string uniformName, std::vector<float> val) {
	auto prg = GetCurrentShaderPrg();
	GLuint location = glGetUniformLocation(prg, uniformName.c_str());
	glUniform1fv(location, val.size(), reinterpret_cast<GLfloat*>(&val[0]));
}

// 普通のShaderにバッファーをアタッチ
void Material::SetBufferToMat(std::shared_ptr<ComputeBuffer> buffer, int bufferIndex) {
	SetActive();
	buffer->SetActive();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferIndex, buffer->ssbo);
	buffer->SetEactive();
}

GLuint Material::GetCurrentShaderPrg() {
	if (computeShaderData == -1) 
	{
		if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::COLOR) {
			return shaderPrg;
		}
		else if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::DEPTH) {
			return depthShaderPrg;
		}
	}
	else 
	{
		return shaderPrg;
	}
}

void Material::UnLoadData() {
	//Delete Color
	glDeleteProgram(shaderPrg);
	glDeleteShader(vertShaderData);
	if (tessControlShaderData) {
		glDeleteShader(tessControlShaderData);
	}
	if (tessEvalShaderData) {
		glDeleteShader(tessEvalShaderData);
	}
	if (geometryShaderData) {
		glDeleteShader(geometryShaderData);
	}
	glDeleteShader(fragShaderData);

	//Delete Depth
	glDeleteProgram(depthShaderPrg);
	glDeleteShader(depthVertShaderData);
	if (depthTessControlShaderData) {
		glDeleteShader(depthTessControlShaderData);
	}
	if (depthTessEvalShaderData) {
		glDeleteShader(depthTessEvalShaderData);
	}
	if (depthGeometryShaderData) {
		glDeleteShader(depthGeometryShaderData);
	}
	glDeleteShader(depthFragShaderData);
}

// Compute Shader Func
void Material::Dispatch(int xGroupNum, int yGroupNum, int zGroupNum) {
	glDispatchCompute(xGroupNum, yGroupNum, zGroupNum);
}

void Material::BindComputeBuffer(CorrectionType correctionType) {
	if (correctionType == CorrectionType::COMPUTEBUFFER) {
		for (int i = 0; i < m_buffers.size(); i++) {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, m_buffers[i]->ssbo);
		}
	}
	else if (correctionType == CorrectionType::ARRAY) {
		for (int i = 0; i < m_buffers.size(); i++) {
			glBindBuffer(GL_ARRAY_BUFFER, m_buffers[i]->ssbo);
		}
	}
}

void Material::DisBindComputeBuffer(CorrectionType correctionType) {
	if (correctionType == CorrectionType::COMPUTEBUFFER) {
		for (int i = 0; i < m_buffers.size(); i++) {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, 0);
		}
	}
	else if (correctionType == CorrectionType::ARRAY) {
		for (int i = 0; i < m_buffers.size(); i++) {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}

// ComputeShaderにバッファをアタッチ
void Material::SetBufferToCS(std::shared_ptr<ComputeBuffer> buffer, int bufferindex) {
	m_buffers.push_back(buffer);
	SetActive();
	buffer->SetActive();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferindex, buffer->ssbo);
	buffer->SetEactive();
}