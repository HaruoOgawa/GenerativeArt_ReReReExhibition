#include "MeshRendererComponent.h"
#include "./TransformComponent.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "GraphicsEngine/Component/TransformComponent.h"

MeshRendererComponent::MeshRendererComponent(const std::shared_ptr<TransformComponent>& TRS, PrimitiveType primType, RenderingSurfaceType SurfaceType,
	std::string vert, std::string frag, std::string geom,
	std::string tc, std::string tv, std::string cs, std::function<void(void)> calllback) : 
	m_mesh(nullptr), 
	m_material(nullptr), 
	m_transform(TRS),
	useZTest(true), 
	useDoubleSlided(true),
	IsMulMatOnVert(true),
	m_calllback(calllback),
	useAddBlend(false)
{
	m_SurfaceType = SurfaceType;
	m_mesh = std::make_shared<Mesh>((primType));
	m_material = std::make_shared<Material>(SurfaceType, vert, frag, geom, tc, tv,cs);

	// アルファブレンドの初期値
	if (m_SurfaceType==RenderingSurfaceType::RASTERIZER)
	{
		useAlphaTest = true;
	}
	else
	{
		useAlphaTest = false;
	}
}

MeshRendererComponent::MeshRendererComponent(const std::shared_ptr<TransformComponent>& TRS, RenderingSurfaceType SurfaceType,
	const std::vector<std::vector<float>>& VertexData, const std::vector<int>& Dimention, const std::vector<unsigned short>& Indices,
	std::string vert, std::string frag, std::string geom ,std::string tc, std::string tv, std::string cs, std::function<void(void)> calllback) :
	m_mesh(nullptr),
	m_material(nullptr),
	m_transform(TRS),
	useZTest(true),
	useDoubleSlided(true),
	IsMulMatOnVert(true),
	m_calllback(calllback)
{
	m_SurfaceType = SurfaceType;
	m_mesh = std::make_shared<Mesh>(VertexData,Dimention,Indices);
	m_material = std::make_shared<Material>(SurfaceType, vert, frag, geom, tc, tv, cs);

	// アルファブレンドの初期値
	if (m_SurfaceType == RenderingSurfaceType::RASTERIZER)
	{
		useAlphaTest = true;
	}
	else
	{
		useAlphaTest = false;
	}
}

void MeshRendererComponent::Draw(std::function<void(void)> TemporaryCallBack, GLenum DrawVertexWay, bool IsInstancing, int InstanceNum) {

	if (useZTest) 
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (useAlphaTest)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (useAddBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}
	else 
	{
		glDisable(GL_BLEND);
	}

	if (useDoubleSlided)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	m_transform->CalMatrix();

	m_material->SetActive();
	m_material->SetIntUniform("_IsMulMatOnVert", (IsMulMatOnVert) ? 1 : 0);
	m_material->SetMatrixUniform("MVPMatrix", m_transform->m_pMatrix * m_transform->m_vMatrix * m_transform->m_mMatrix);
	m_material->SetMatrixUniform("MMatrix", m_transform->m_mMatrix);
	m_material->SetMatrixUniform("VMatrix", m_transform->m_vMatrix);
	m_material->SetMatrixUniform("PMatrix", m_transform->m_pMatrix);
	m_material->SetMatrixUniform("VPMatrix", m_transform->m_pMatrix * m_transform->m_vMatrix);
	m_material->SetMatrixUniform("InvVPMatrix", glm::inverse(m_transform->m_pMatrix * m_transform->m_vMatrix));
	m_material->SetFloatUniform("_time", GraphicsMain::GetInstance()->m_SecondsTime);
	m_material->SetFloatUniform("_deltaTime", GraphicsMain::GetInstance()->m_DeltaTime);
	m_material->SetVec2Uniform("_resolution", GraphicsRenderer::GetInstance()->GetScreenSize());
	m_material->SetFloatUniform("_frameResolusion", GraphicsRenderer::GetInstance()->frameResolusion);
	m_material->SetVec3Uniform("_LightDir", GraphicsMain::GetInstance()->m_DirectionalLightDir);
	m_material->SetVec3Uniform("_LightPos", GraphicsMain::GetInstance()->m_GroabalLightPosition->m_position);
	m_material->SetIntUniform("_UseLighting", 1);

	// レイマーチングのデプスマップ用
	if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::COLOR) {
		m_material->SetFloatUniform("_RenderingTarget", 1.0);
	}
	else if (GraphicsMain::GetInstance()->renderingTarget == ERerderingTarget::DEPTH) {
		m_material->SetFloatUniform("_RenderingTarget", 2.0);
	}

	// カメラが定義されているなら情報を渡す
	if(GraphicsMain::GetInstance()->m_UsingCamera)
	{
		m_material->SetVec3Uniform("_WorldCameraPos", GraphicsMain::GetInstance()->m_UsingCamera->m_position);
		m_material->SetVec3Uniform("_WorldCameraCenter", GraphicsMain::GetInstance()->m_UsingCamera->m_center);
	}

	// Custom Uniform
	m_calllback();

	// Temporary Uniform
	TemporaryCallBack();

	//
	if (IsInstancing)
	{
		m_mesh->DrawInstancedWithMesh(InstanceNum, DrawVertexWay);
	}
	else
	{
		m_mesh->Draw(DrawVertexWay);
	}
}
