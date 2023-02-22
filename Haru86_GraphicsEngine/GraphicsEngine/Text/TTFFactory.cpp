#include "TTFFactory.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "GraphicsEngine/Graphics/Texture.h"

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG

namespace text {
	TTFFactory::TTFFactory():
		m_PixelSize(48)
	{
	}

	TTFFactory::~TTFFactory()
	{
		m_TTFCharacterTexMap.clear();
	}

	bool TTFFactory::Load()
	{
		// FreeTypeを初期化する
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
#ifdef _DEBUG
			Console::Log("FreeType Initialize Error\n");
#endif // _DEBUG

			return false;
		}

		FT_Face face;
#ifdef _DEBUG
		FT_Error Error = FT_New_Face(ft, "Assets\\Font\\georgiai.ttf", 0, &face);
#else
		FT_Error Error = FT_New_Face(ft, "bin\\georgiai.ttf", 0, &face);
#endif
		
		if (Error)
		{
#ifdef _DEBUG
			Console::Log("Face Create Error / ErrorCode: %d\n", Error);
#endif // _DEBUG

			return false;
		}

		FT_Set_Pixel_Sizes(face, 0, m_PixelSize);

		// TrueTypeFontのロード
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++)
		{
			// 文字をロード
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
#ifdef _DEBUG
				Console::Log("FT_Load_Char Error\n");
#endif // _DEBUG

				continue;
			}

			// ロードしたデータをTexMapに登録する
			std::shared_ptr<Texture> CharTexture = std::make_shared<Texture>();
			CharTexture->CreateForRendering(face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			CharTexture->SetLeftTop(face->glyph->bitmap_left, face->glyph->bitmap_top);
			
			m_TTFCharacterTexMap.insert({ c, CharTexture });
		}

		// FreeTypeのメモリを解放しておく
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		return true;
	}

	const std::shared_ptr<Texture>& TTFFactory::GetFTChar(unsigned char character)const
	{
		const auto& it = m_TTFCharacterTexMap.find(character);
		if (it == m_TTFCharacterTexMap.end()) return nullptr;
		
		return it->second;
	}
}