#include "TextRenderer.h"
#include "GraphicsEngine/Component/MeshRendererComponent.h"
#include "GraphicsEngine/Component/TransformComponent.h"
#include "GraphicsEngine/Graphics/ShaderLib.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "TTFFactory.h"

namespace text
{
	TextRenderer::TextRenderer()
	{
		m_TextMeshRenderer = std::make_shared<MeshRendererComponent>(
			std::make_shared<TransformComponent>(),
			PrimitiveType::BOARD,
			RenderingSurfaceType::RASTERIZER,
			shaderlib::Standard_vert,
			shaderlib::Standard_frag
		);

		m_TextMeshRenderer->useAlphaTest = true;
		m_TextMeshRenderer->useZTest = false;
	}
	
	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::Draw(const std::string& Text, float FontSize, float WAdjust, float HAdjust, const glm::vec3& Pos, const glm::vec4& Color,
		bool IsTextAlignLeft, bool IsUseBack, int LineNumber, const std::vector<glm::vec4>& ColorList, const float ShrinkRate)
	{
		// テキストデータの整理
		const char* TextData = Text.data();
		std::vector<std::vector<char>> StructuredText;
		int PushIndex = 0;
		StructuredText.push_back(std::vector<char>());
		std::vector<glm::vec4> UseColorList;

		for (float n = 0.0f; n < static_cast<float>(Text.size()); n++)
		{
			const auto PushText = TextData[static_cast<size_t>(n)];
			if (PushText == '\n') // 改行なので行を次に進める
			{
				if(n < static_cast<float>(Text.size()) - 1.0f) StructuredText.push_back(std::vector<char>());
				PushIndex++;
			}
			else
			{
				StructuredText[PushIndex].push_back(PushText);
				if(ColorList.size() > 0) UseColorList.push_back(ColorList[n]);
			}
		}
		const float NumOfRowChar = static_cast<float>(StructuredText.size());

		//
		for (float row = 0.0f; row < NumOfRowChar; row++)
		{
			const auto& Arr = StructuredText[row];
			const float NumOfColChar = static_cast<float>(Arr.size());

			// 背景の描画
			if (IsUseBack)
			{
				{
					float xOffAdj = -0.01f * ShrinkRate, xSAdj = 0.7f;
					float xSize = NumOfColChar * FontSize * xSAdj;
					float xOffset = -1.0f * (1.0f - xSize);
					glm::vec3 scale = glm::vec3(xSize, 1.5f * FontSize, 0.0f);
					m_TextMeshRenderer->m_transform->m_scale = scale;
					m_TextMeshRenderer->m_transform->m_position = glm::vec3(xOffset + xOffAdj, 0.0f + FontSize * 2.0f, 0.0f) + Pos;

					m_TextMeshRenderer->Draw([&]() {
						m_TextMeshRenderer->m_material->SetVec4Uniform("_Color", glm::vec4(0.0f, 0.0f, 0.0f, 0.9f));
						m_TextMeshRenderer->m_material->SetIntUniform("_IsMOnly", 1);
						m_TextMeshRenderer->m_material->SetIntUniform("_IsMulMatOnVert", 0);
						m_TextMeshRenderer->m_material->SetIntUniform("_UseLighting", 0);
						m_TextMeshRenderer->m_material->SetIntUniform("_UseMainTex", 0);
						m_TextMeshRenderer->m_material->SetIntUniform("_UseColor", 1);

						}, GL_TRIANGLES, false, 0);
				}

				// 行番号の描画
				{
					std::string LineNumberStr = std::to_string(LineNumber);
					for (int i = 0; i< LineNumberStr.size(); i++)
					{
						const auto& Num = LineNumberStr[i];
						const auto& CharTex = GraphicsMain::GetInstance()->m_TTFFactory->GetFTChar(Num);
						float xPos = -1.0f + 0.001f + FontSize * static_cast<float>(i);

						m_TextMeshRenderer->m_transform->m_scale = glm::vec3(FontSize * 0.5f);
						m_TextMeshRenderer->m_transform->m_position = glm::vec3(xPos, FontSize * 2.0f + Pos.y, 0.0f);

						m_TextMeshRenderer->Draw([&]() {
							m_TextMeshRenderer->m_material->SetVec4Uniform("_Color", glm::vec4(1.0f));
							m_TextMeshRenderer->m_material->SetIntUniform("_IsMOnly", 1);
							m_TextMeshRenderer->m_material->SetIntUniform("_IsMulMatOnVert", 0);
							m_TextMeshRenderer->m_material->SetIntUniform("_UseLighting", 0);
							m_TextMeshRenderer->m_material->SetIntUniform("_UseMainTex", 1);
							m_TextMeshRenderer->m_material->SetIntUniform("_UseColor", 0);

							if (CharTex)
							{
								CharTex->SetActive(GL_TEXTURE0);
								m_TextMeshRenderer->m_material->SetIntUniform("_MainTex", 0);
							}
							}, GL_TRIANGLES, false, 0);
						if (CharTex)CharTex->SetEnactive(GL_TEXTURE0);
					}
				}
			}

			// テキストの描画
			for (float col = 0.0f; col < NumOfColChar; col++)
			{
				const auto& CharData = Arr[static_cast<size_t>(col)];
				
				// スペースの場合は描画から除く(位置の計算には重要なのでStructuredTextには入れている) 
				if (std::isspace(CharData)) continue;
				
				//
				const auto& CharTex = GraphicsMain::GetInstance()->m_TTFFactory->GetFTChar(CharData);
				const float PixelSize = static_cast<float>(GraphicsMain::GetInstance()->m_TTFFactory->GetPixelSize());
				const float Left = static_cast<float>(CharTex->GetLeft()) / PixelSize;
				const float Top = static_cast<float>(CharTex->GetTop()) / PixelSize;
				const float Width = static_cast<float>(CharTex->GetWidth()) / PixelSize;
				const float Height = static_cast<float>(CharTex->GetHeight()) / PixelSize;
				const auto& DrawColor = (UseColorList.size() > 0) ? UseColorList[col] : Color;

				//Console::Log("Char: %c / PixelSize: %f / Left: %d / Top: %d / Width: %d / Height: %d\n", Arr[static_cast<size_t>(col)], PixelSize, CharTex->GetLeft(), CharTex->GetTop(), CharTex->GetWidth(), CharTex->GetHeight());

				//
				glm::vec3 scale = glm::vec3(Width, Height, 0.0f) * FontSize;
				m_TextMeshRenderer->m_transform->m_scale = scale;

				// センター揃えか左揃えにする
				float XVal = (IsTextAlignLeft)? -1.0f + col * FontSize * WAdjust : (col - NumOfColChar * 0.5f) * FontSize * WAdjust;
				float YVal = (row - NumOfRowChar * 0.5f) * FontSize * HAdjust * (-1.0f);
				float LVal = (Left * scale.x)			 ;
				float TVal = (Top * scale.y)			 ;

				m_TextMeshRenderer->m_transform->m_position = glm::vec3(XVal + LVal, YVal + TVal, 0.0f) + Pos;

				//
				m_TextMeshRenderer->Draw([&]() {
					m_TextMeshRenderer->m_material->SetVec4Uniform("_Color", DrawColor);
					m_TextMeshRenderer->m_material->SetIntUniform("_IsMOnly", 1);
					m_TextMeshRenderer->m_material->SetIntUniform("_IsMulMatOnVert", 0);
					m_TextMeshRenderer->m_material->SetIntUniform("_UseLighting", 0);
					m_TextMeshRenderer->m_material->SetIntUniform("_UseMainTex", 1);
					m_TextMeshRenderer->m_material->SetIntUniform("_UseColor", 0);

					if (CharTex)
					{
						CharTex->SetActive(GL_TEXTURE0);
						m_TextMeshRenderer->m_material->SetIntUniform("_MainTex", 0);
					}
					}, GL_TRIANGLES, false, 0);
				if (CharTex)CharTex->SetEnactive(GL_TEXTURE0);
			}
		}
	}
}