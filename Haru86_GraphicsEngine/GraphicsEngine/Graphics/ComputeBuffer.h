#pragma once
#include <vector>
#include <glew.h>
#include <vector>

class ComputeBuffer
{
public:
	ComputeBuffer(GLsizeiptr size);
	~ComputeBuffer();

	void SetActive();
	void SetEactive();

	// �ȉ��̃��[�������Ȃ��� (nvoglv64.dll) �ŗ�O���X���[����܂��� �ƃG���[���ł�
	// * SetData<�^>(data) ��<�^>���w�肵�Ȃ���
	// �����v���ƂȂ�
	// * �|�C���^�͎g���Ȃ��B�X�^�b�N�������n���̌^�w��̂�(new���g��Ȃ����)
	// ������O����GPU��shader_ptr�Ȃ�ĂȂ�
	// ��. SetData<std::vector<StemVertex>>(initStemVertex)
	// * �܂������̌^(struct �� class)�̃����o�ϐ��̓v���O�����̈�ʓI�Ȍ^�����g���Ȃ�(int,float �ȂǂƂ���������)
	// GPU�ɂ͂����������^�����Ȃ���ˁH�Ă��J�X�^���^��`�����i�͂Ȃ���ˁH
	// ��. glm::vec3 val--> float val[3] �̂悤�ɕ����^�͔z��ɂ���
	// * ���������̃f�[�^���Ə����l�Z�b�g���̃f�[�^�������낦��
	// OpenGL ComputeBuffer (std430/std140)��vec3��16�o�C�g���ƂɃI�t�Z�b�g����(vec4��16�o�C�g)
	// ����Ɠ�����O������Dispatch�̑O��ComputeShader��SetActive���Ȃ���GPGPU�����������A�Ȃ�Buffer���������Ȃ��ƂɂȂ�̂Œ���!!!!
	template<typename T>
	void SetData(T data) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, data.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// Buffer�̒�����w��������x�f�[�^���擾����
	template<typename T>
	void GetBufferData(T *data,int offset,int num) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(T)*num,data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	void DeleteBuffer();

	friend class ComputeShader;
	friend class Material;

private:
	void CreateBuffer(GLsizeiptr size);
	GLuint ssbo;
	GLsizeiptr bufferSize;
};

