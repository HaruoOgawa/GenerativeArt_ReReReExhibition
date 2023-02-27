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

	// 以下のルールを守らないと (nvoglv64.dll) で例外がスローされました とエラーがでる
	// * SetData<型>(data) で<型>を指定しないと
	// ↓も要因となる
	// * ポインタは使えない。スタックメモリ系統の型指定のみ(newを使わないやつ)
	// 当たり前だがGPUにshader_ptrなんてない
	// 例. SetData<std::vector<StemVertex>>(initStemVertex)
	// * またそれらの型(struct や class)のメンバ変数はプログラムの一般的な型した使えない(int,float などといったもの)
	// GPUにはそういった型しかないよね？てかカスタム型を伝える手段はないよね？
	// 例. glm::vec3 val--> float val[3] のように複数型は配列にする
	// * 初期化時のデータ数と初期値セット時のデータ数をそろえる
	// OpenGL ComputeBuffer (std430/std140)のvec3は16バイトごとにオフセットする(vec4も16バイト)
	// それと当たり前だけどDispatchの前にComputeShaderをSetActiveしないとGPGPUが発生せず、なんかBufferがおかしなことになるので注意!!!!
	template<typename T>
	void SetData(T data) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, data.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// Bufferの中から『一つ分だけ』データを取得する
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

