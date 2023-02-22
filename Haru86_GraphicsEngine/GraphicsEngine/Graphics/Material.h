#pragma once

#include <string>
#include <glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>
#include "GraphicsEngine/Graphics/EGraphicsParam.h"

enum CorrectionType {
    COMPUTEBUFFER,
    ARRAY
};

class Material 
{
public:
    Material(RenderingSurfaceType SurfaceType,const std::string& vert, const std::string& frag, const std::string& geom="", const std::string& tc = "", const std::string& tv = "", const std::string& cs = "");
    ~Material();
    
    void LoadShader(const std::string& vert, const std::string& frag, const std::string& geom,const std::string& tc, const std::string& tv, const std::string& cs,
        GLuint& prg, GLuint& vertData, GLuint& fragData, GLuint& geomData, GLuint& tcData, GLuint& tvData, GLuint& csData);
    bool CompileShader(const std::string shaderCode,GLenum shaderType,GLuint& outShader);
    
    void SetActive();
    void UnLoadData();
    
    void SetMatrixUniform(std::string uniformName,glm::mat4 matrix);
    void SetMatrixVectorUniform(std::string uniformName, const std::vector<glm::mat4>& matrixVector);
    void SetFloatUniform(std::string uniformName,float val);
    void SetIntUniform(std::string uniformName,int val);
    void SetVec2Uniform(std::string uniformName, glm::vec2 val);
    void SetVec3Uniform(std::string uniformName, glm::vec3 val);
    void SetVec4Uniform(std::string uniformName, glm::vec4 val);
    void SetVec4ArrayUniform(std::string uniformName, std::vector<float> val);
    void SetTexUniform(std::string uniformName, unsigned int val);
    void SetFloatVectorUniform(std::string uniformName, std::vector<float> val);
    GLuint GetCurrentShaderPrg();
private :
    //Color Render Buffer
    GLuint vertShaderData;
    GLuint tessControlShaderData;
    GLuint tessEvalShaderData;
    GLuint geometryShaderData;
    GLuint fragShaderData;
    GLuint shaderPrg;
                    
    //Depth Render Buffer
    GLuint depthVertShaderData;
    GLuint depthTessControlShaderData;
    GLuint depthTessEvalShaderData;
    GLuint depthGeometryShaderData;
    GLuint depthFragShaderData;
    GLuint depthShaderPrg;

    //
    GLuint computeShaderData;
    GLuint computeShaderDepthData;
};

