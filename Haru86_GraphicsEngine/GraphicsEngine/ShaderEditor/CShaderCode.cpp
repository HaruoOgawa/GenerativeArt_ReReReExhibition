#include "CShaderCode.h"
#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

namespace editor
{
	CShaderCode::CShaderCode(const std::string& Code)
	{
		// 言語処理(演出用なのである程度の誤差は許容範囲)
		for (int i = 0, startIndex = 0, indent = 0; i < Code.length(); i++)
		{
			const auto& InputChar = Code[i];
			const auto& InputNextChar = Code[i + 1];
			if (
				InputChar == '\n' ||
				InputChar == ';' ||
				InputChar == '{' ||
				(InputChar == '}') ||
				(InputChar == ')' && InputNextChar != ';' && InputNextChar != ',' && InputNextChar != ')' && InputNextChar != '*' && InputNextChar != '+' && InputNextChar != '-' && InputNextChar != '/')
				)
			{
				//
				std::string result;
				result.resize(i - startIndex + 1);
				std::memcpy(&result[0], &Code[startIndex], result.size());

				// インデントを減らす
				if (InputChar == '}') indent--;

				// インデントの追加
				for (int n = 0; n < indent; n++)
				{
					result = "	" + result;
				}

				// 構文解析を行いハイライトカラーを決める
				std::vector<glm::vec4> ColorList; 
				std::string TextStack;
				int ReadStartIndex = 0;
				for (int nInLine = 0; nInLine < result.size(); nInLine++)
				{
					// 現在の文字を取得
					const auto& C = result[nInLine];

					// スペースか記号なら何もしない
					if (std::isspace(C) || std::find(m_SymbolList.begin(), m_SymbolList.end(), C) != m_SymbolList.end())
					{
						// その他は白
						ColorList.push_back(glm::vec4(1.0f));

						// リセット
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}
					
					// 文字スタックが空でかつ数値の場合は数値として1文字ずつ処理する
					// 文字スタックが空かをチェックするのはvec3とかがあるから
					if (TextStack.empty() && std::find(m_BuiltInNumber.begin(), m_BuiltInNumber.end(), C) != m_BuiltInNumber.end())
					{
						// 数値は黄色(0 ~ 9 と . )
						ColorList.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

						// リセット
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}

					// スタックに文字を溜める
					TextStack += C;

					// 現在のテキストスタックが登録された構文にあるかチェック
					// 合致した場合、その色を指定して文字スタックをを再びリセット
					if (std::find(m_BuiltInTypes.begin(), m_BuiltInTypes.end(), TextStack) != m_BuiltInTypes.end())
					{
						// 合致した瞬間の文字の分もちゃんと追加しておく
						// 従来合致しなかった場合、構文解析の最後の行でColorList.push_backの仮決定を行っているが、
						// 合致した瞬間の文字にはその処理が届かないためこのようなことをしている
						ColorList.push_back(glm::vec4(0.655f, 0.341f, 0.658f, 1.0f));

						// 組み込み型は紫
						for (int m = 0; m < (nInLine - ReadStartIndex + 1); m++)
						{
							int ReadIndex = ReadStartIndex + m;
							ColorList[ReadIndex] = glm::vec4(0.655f, 0.341f, 0.658f, 1.0f);
						}

						// リセット
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}
					else if (std::find(m_BuiltInVal.begin(), m_BuiltInVal.end(), TextStack) != m_BuiltInVal.end())
					{
						// 合致した瞬間の文字の分もちゃんと追加しておく
						// 従来合致しなかった場合、構文解析の最後の行でColorList.push_backの仮決定を行っているが、
						// 合致した瞬間の文字にはその処理が届かないためこのようなことをしている
						ColorList.push_back(glm::vec4(0.937f, 0.54f, 0.29f, 1.0f));

						// 組み込み変数はオレンジ
						for (int m = 0; m < (nInLine - ReadStartIndex + 1); m++)
						{
							int ReadIndex = ReadStartIndex + m;
							ColorList[ReadIndex] = glm::vec4(0.937f, 0.54f, 0.29f, 1.0f);
						}

						// リセット
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}
					else if (std::find(m_BuiltInFunc.begin(), m_BuiltInFunc.end(), TextStack) != m_BuiltInFunc.end())
					{
						// 合致した瞬間の文字の分もちゃんと追加しておく
						// 従来合致しなかった場合、構文解析の最後の行でColorList.push_backの仮決定を行っているが、
						// 合致した瞬間の文字にはその処理が届かないためこのようなことをしている
						ColorList.push_back(glm::vec4(0.937f, 0.54f, 0.29f, 1.0f));

						// 組み込み関数はオレンジ
						for (int m = 0; m < (nInLine - ReadStartIndex + 1); m++)
						{
							int ReadIndex = ReadStartIndex + m;
							ColorList[ReadIndex] = glm::vec4(0.937f, 0.54f, 0.29f, 1.0f);
						}

						// リセット
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					} 
					// どれにも合致しなかった場合(その他) ひとまず白色にする(あとで合致して書き換える場合もある)
					else
					{
						ColorList.push_back(glm::vec4(1.0f));
					}
				}

				// CodeLineを作成
				m_CodeLineList.push_back(CShaderCodeLine{ result, glm::vec3(0.0f, 0.0f, 0.0f), ColorList });

				// インデントを増やす
				if (InputChar == '{') indent++;

				startIndex = i + 1;
			}
		}
	}
}