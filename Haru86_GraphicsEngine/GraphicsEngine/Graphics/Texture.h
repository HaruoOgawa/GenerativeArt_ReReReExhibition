#pragma once
#include <string>

#include <glew.h>

class Texture
{
public:
	Texture();
	~Texture();
	void CreateForRendering(int width, int height,GLint internalformat,GLint format,GLenum type= GL_UNSIGNED_BYTE, unsigned char* pData = 0);
	unsigned int GetTextureID() const { return mTextureID; }
	int GetWidth()const { return mWidth; }
	int GetHeight()const { return mHeight; }
	void SetActive(GLenum slot,GLenum texType=GL_TEXTURE_2D);
	void SetEnactive(GLenum slot, GLenum texType = GL_TEXTURE_2D);

	void SetLeftTop(int Left, int Top) { m_Left = Left; m_Top = Top; }
	int GetLeft() const { return m_Left; }
	int GetTop() const { return m_Top; }

	friend class Material;
private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
	int m_Left;
	int m_Top;
	std::string texUniformName;
};

