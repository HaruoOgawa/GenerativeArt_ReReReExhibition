#include "CShaderCode.h"
#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

namespace editor
{
	CShaderCode::CShaderCode(const std::string& Code)
	{
		// ���ꏈ��(���o�p�Ȃ̂ł�����x�̌덷�͋��e�͈�)
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

				// �C���f���g�����炷
				if (InputChar == '}') indent--;

				// �C���f���g�̒ǉ�
				for (int n = 0; n < indent; n++)
				{
					result = "	" + result;
				}

				// �\����͂��s���n�C���C�g�J���[�����߂�
				std::vector<glm::vec4> ColorList; 
				std::string TextStack;
				int ReadStartIndex = 0;
				for (int nInLine = 0; nInLine < result.size(); nInLine++)
				{
					// ���݂̕������擾
					const auto& C = result[nInLine];

					// �X�y�[�X���L���Ȃ牽�����Ȃ�
					if (std::isspace(C) || std::find(m_SymbolList.begin(), m_SymbolList.end(), C) != m_SymbolList.end())
					{
						// ���̑��͔�
						ColorList.push_back(glm::vec4(1.0f));

						// ���Z�b�g
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}
					
					// �����X�^�b�N����ł����l�̏ꍇ�͐��l�Ƃ���1��������������
					// �����X�^�b�N���󂩂��`�F�b�N����̂�vec3�Ƃ������邩��
					if (TextStack.empty() && std::find(m_BuiltInNumber.begin(), m_BuiltInNumber.end(), C) != m_BuiltInNumber.end())
					{
						// ���l�͉��F(0 ~ 9 �� . )
						ColorList.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

						// ���Z�b�g
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}

					// �X�^�b�N�ɕ����𗭂߂�
					TextStack += C;

					// ���݂̃e�L�X�g�X�^�b�N���o�^���ꂽ�\���ɂ��邩�`�F�b�N
					// ���v�����ꍇ�A���̐F���w�肵�ĕ����X�^�b�N�����Ăу��Z�b�g
					if (std::find(m_BuiltInTypes.begin(), m_BuiltInTypes.end(), TextStack) != m_BuiltInTypes.end())
					{
						// ���v�����u�Ԃ̕����̕��������ƒǉ����Ă���
						// �]�����v���Ȃ������ꍇ�A�\����͂̍Ō�̍s��ColorList.push_back�̉�������s���Ă��邪�A
						// ���v�����u�Ԃ̕����ɂ͂��̏������͂��Ȃ����߂��̂悤�Ȃ��Ƃ����Ă���
						ColorList.push_back(glm::vec4(0.655f, 0.341f, 0.658f, 1.0f));

						// �g�ݍ��݌^�͎�
						for (int m = 0; m < (nInLine - ReadStartIndex + 1); m++)
						{
							int ReadIndex = ReadStartIndex + m;
							ColorList[ReadIndex] = glm::vec4(0.655f, 0.341f, 0.658f, 1.0f);
						}

						// ���Z�b�g
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}
					else if (std::find(m_BuiltInVal.begin(), m_BuiltInVal.end(), TextStack) != m_BuiltInVal.end())
					{
						// ���v�����u�Ԃ̕����̕��������ƒǉ����Ă���
						// �]�����v���Ȃ������ꍇ�A�\����͂̍Ō�̍s��ColorList.push_back�̉�������s���Ă��邪�A
						// ���v�����u�Ԃ̕����ɂ͂��̏������͂��Ȃ����߂��̂悤�Ȃ��Ƃ����Ă���
						ColorList.push_back(glm::vec4(0.937f, 0.54f, 0.29f, 1.0f));

						// �g�ݍ��ݕϐ��̓I�����W
						for (int m = 0; m < (nInLine - ReadStartIndex + 1); m++)
						{
							int ReadIndex = ReadStartIndex + m;
							ColorList[ReadIndex] = glm::vec4(0.937f, 0.54f, 0.29f, 1.0f);
						}

						// ���Z�b�g
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					}
					else if (std::find(m_BuiltInFunc.begin(), m_BuiltInFunc.end(), TextStack) != m_BuiltInFunc.end())
					{
						// ���v�����u�Ԃ̕����̕��������ƒǉ����Ă���
						// �]�����v���Ȃ������ꍇ�A�\����͂̍Ō�̍s��ColorList.push_back�̉�������s���Ă��邪�A
						// ���v�����u�Ԃ̕����ɂ͂��̏������͂��Ȃ����߂��̂悤�Ȃ��Ƃ����Ă���
						ColorList.push_back(glm::vec4(0.937f, 0.54f, 0.29f, 1.0f));

						// �g�ݍ��݊֐��̓I�����W
						for (int m = 0; m < (nInLine - ReadStartIndex + 1); m++)
						{
							int ReadIndex = ReadStartIndex + m;
							ColorList[ReadIndex] = glm::vec4(0.937f, 0.54f, 0.29f, 1.0f);
						}

						// ���Z�b�g
						TextStack.clear();
						ReadStartIndex = nInLine + 1;

						continue;
					} 
					// �ǂ�ɂ����v���Ȃ������ꍇ(���̑�) �ЂƂ܂����F�ɂ���(���Ƃō��v���ď���������ꍇ������)
					else
					{
						ColorList.push_back(glm::vec4(1.0f));
					}
				}

				// CodeLine���쐬
				m_CodeLineList.push_back(CShaderCodeLine{ result, glm::vec3(0.0f, 0.0f, 0.0f), ColorList });

				// �C���f���g�𑝂₷
				if (InputChar == '{') indent++;

				startIndex = i + 1;
			}
		}
	}
}