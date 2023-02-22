#include "ChangeOfMind.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/Primitive.h"
#include "GraphicsEngine/Graphics/PostProcess.h"

namespace app
{
	ChangeOfMind::ChangeOfMind() :
		m_NegativeSphereMeshRenderer(nullptr),
		m_NegativeSphereCoreMeshRenderer(nullptr),
		m_Voxel(nullptr),
		m_Triangle(nullptr)
	{
		// 演出用のShaderEditorにコードを追加する
		std::string Triangle = std::string({
				#include "../Shader/Triangle_FragComp.h"
			});

		GraphicsMain::GetInstance()->m_ShaderEditor->AddShaderMap("Triangle.Frag", Triangle);

		//

		// m_NegativeSphereMeshRenderer
		{
			std::vector<std::vector<float>> VertexData; std::vector<int> Dimention; std::vector<unsigned short> Indices;
			Primitive::CreateSphere(&VertexData, &Dimention, &Indices, 32.0f, 32.0f, 1.0f);

			m_NegativeSphereMeshRenderer = std::make_shared<MeshRendererComponent>(
				std::make_shared<TransformComponent>(),
				RenderingSurfaceType::RASTERIZER,
				VertexData, Dimention, Indices,
				shaderlib::Standard_vert,
				std::string({
					#include "../Shader/NegativeSphere_FragComp.h"
				}),
				std::string(
					#include "../Shader/NegativeSphere.geom"
				)
			);

			m_NegativeSphereMeshRenderer->useZTest = true;
			m_NegativeSphereMeshRenderer->useAlphaTest = true;
			m_NegativeSphereMeshRenderer->IsMulMatOnVert = false;
		}

		// m_NegativeSphereCoreMeshRenderer
		{
			m_NegativeSphereCoreMeshRenderer = std::make_shared<MeshRendererComponent>(
				std::make_shared<TransformComponent>(),
				PrimitiveType::SPHERE,
				RenderingSurfaceType::RASTERIZER,
				std::string({
					#include "../Shader/NegativeSphereCore_VertComp.h."
				}),
				shaderlib::Standard_frag
			);

			m_NegativeSphereCoreMeshRenderer->m_transform->m_scale = glm::vec3(0.75f);
			m_NegativeSphereCoreMeshRenderer->useDoubleSlided = false;
		}

		// m_Voxel
		{
			m_Voxel = std::make_shared<MeshRendererComponent>(
				std::make_shared<TransformComponent>(),
				PrimitiveType::BOARD,
				RenderingSurfaceType::RAYMARCHING,
				shaderlib::StandardRenderBoard_vert,
				shaderlib::Voxel_frag
			);

			m_Voxel->useAlphaTest = true;
		}
		
		// m_Triangle
		{
			m_Triangle = std::make_shared<MeshRendererComponent>(
				std::make_shared<TransformComponent>(),
				PrimitiveType::BOARD,
				RenderingSurfaceType::RAYMARCHING,
				shaderlib::StandardRenderBoard_vert,
				Triangle
			);

			m_Triangle->useAlphaTest = true;
			m_Triangle->useZTest = false;
		}
	}

	void ChangeOfMind::Update(float time)
	{

	}

	void ChangeOfMind::Draw(bool IsRaymarching)
	{
		bool DebugDraw = (glm::mod(GraphicsMain::GetInstance()->m_SecondsTime, 2.0f) < 1.0);
		DebugDraw = true;
		//DebugDraw = false;
		float Alpha = abs(sin(GraphicsMain::GetInstance()->m_SecondsTime));

		if (IsRaymarching)
		{
			//if (DebugDraw)
			{
				m_Voxel->Draw([=]() {
					m_Voxel->m_material->SetIntUniform("_MapIndex", 1);
					m_Voxel->m_material->SetFloatUniform("_Alpha", Alpha);
				});
			}
			//else
			{
				m_Triangle->Draw([=]() {
					m_Triangle->m_material->SetFloatUniform("_Alpha",1.0f - Alpha);
				});
			}
		}
		else
		{
			

			if (Alpha > 0.5f)
			{
				m_NegativeSphereMeshRenderer->Draw([=]() {
					m_NegativeSphereMeshRenderer->m_material->SetFloatUniform("_Alpha", Alpha);
				}, GL_POINTS);
				m_NegativeSphereCoreMeshRenderer->Draw([&]() {
					m_NegativeSphereCoreMeshRenderer->m_material->SetIntUniform("_UseRim", 1);
					m_NegativeSphereCoreMeshRenderer->m_material->SetVec4Uniform("_RimColor", glm::vec4(0.25f, 0.0f, 0.0f, Alpha));
					m_NegativeSphereCoreMeshRenderer->m_material->SetFloatUniform("_RimPower", 1.75f);
					m_NegativeSphereCoreMeshRenderer->m_material->SetFloatUniform("_RimMulVal", 1.5f);
					});
			}
		}
	}

	void ChangeOfMind::UpdateTimeLine(float time)
	{
		if (GraphicsMain::GetInstance()->GetAppSceneIndex() == 5)
		{
			GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 0.0f, 3.0f);
			GraphicsMain::GetInstance()->m_MainCamera->m_center = glm::vec3(0.0f, 0.0f, 0.0f);

			if (time < 138.5f)
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					float Rate = 1.0f - glm::clamp((time - 138.0f) * 2.0f, 0.0f, 1.0f);

					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseGridWave", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWaveCustom", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WavePower", Rate);

					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteNoise", 1);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetFloatUniform("_WhiteNoisePower", Rate);
				};
			}
			else
			{
				PostProcess::GetInstance()->m_LatePostProcesCallBack = [=]() {
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseGridWave", 0);
					PostProcess::GetInstance()->m_LateMeshRenderer->m_material->SetIntUniform("_UseWhiteNoise", 0);
				};
			}
		}
	}

}