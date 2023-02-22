#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Graphics/Primitive.h"
#include "GraphicsEngine/Graphics/Texture.h"
#include <functional>
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

class Mesh;
class Material;
class TransformComponent;

class MeshRendererComponent
{
public:
    MeshRendererComponent(const std::shared_ptr<TransformComponent>& TRS,PrimitiveType primType, RenderingSurfaceType SurfaceType,
        std::string vert, std::string frag, std::string geom="",
        std::string tc = "", std::string tv = "", std::string cs = "", std::function<void(void)> calllback = []() {});
    
    MeshRendererComponent(const std::shared_ptr<TransformComponent>& TRS, RenderingSurfaceType SurfaceType,
        const std::vector<std::vector<float>>& VertexData, const std::vector<int>& Dimention, const std::vector<unsigned short>& Indices,
        std::string vert, std::string frag, std::string geom="",
        std::string tc = "", std::string tv = "", std::string cs = "", std::function<void(void)> calllback = []() {});

    ~MeshRendererComponent()=default;

    void Draw(std::function<void(void)> TemporaryCallBack = []() {}, GLenum DrawVertexWay = GL_TRIANGLES, bool IsInstancing = false, int InstanceNum = 16);
    
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<TransformComponent> m_transform;

    bool useZTest;
    bool useAlphaTest;
    bool useAddBlend;
    bool useDoubleSlided;
    bool IsMulMatOnVert;
    RenderingSurfaceType m_SurfaceType;
    std::function<void(void)> m_calllback;
    
};