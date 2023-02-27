#include "ComputeBuffer.h"

ComputeBuffer::ComputeBuffer(GLsizeiptr size) {
	CreateBuffer(size);
}

ComputeBuffer::~ComputeBuffer() {
	DeleteBuffer();
}

void ComputeBuffer::SetActive() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
}

void ComputeBuffer::SetEactive() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ComputeBuffer::CreateBuffer(GLsizeiptr size) {
	glGenBuffers(1, &ssbo);
	bufferSize = size;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ComputeBuffer::DeleteBuffer(){
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glDeleteBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}