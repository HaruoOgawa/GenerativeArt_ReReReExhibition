#include "Texture.h"
#include <glew.h>
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"

Texture::Texture()
	:mTextureID(-1),
	 mWidth(0),
	 mHeight(0),
	 m_Left(0),
	 m_Top(0)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateForRendering(int width, int height, GLint internalformat, GLint format, GLenum type, unsigned char* pData)
{
	mWidth = width;
	mHeight = height;
	
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, mWidth, mHeight, 0, format,type, pData);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetActive(GLenum slot, GLenum texType)
{
	glActiveTexture(slot);
	glBindTexture(texType, mTextureID);
}

void Texture::SetEnactive(GLenum slot, GLenum texType) {
	glActiveTexture(slot);
	glBindTexture(texType, 0);
}