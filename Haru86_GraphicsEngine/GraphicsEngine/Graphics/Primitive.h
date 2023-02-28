#pragma once
#include <vector>
#include <string>
#include <glew.h>

class Primitive
{
public:
	Primitive(const std::vector<std::vector<float>>& vertices,const std::vector<int>& dimention,const std::vector<unsigned short>& indices);
	~Primitive();
	void CreatePrimitive(const std::vector<std::vector<float>>& vertices,const std::vector<int>& dimention,const std::vector<unsigned short>& indices);
	void Draw(GLenum DrawVertexWay);
	
	void SetActive();

	static void CreateBoard(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices);
	static void CreatePoint(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices);
	static void CreateCube(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices);
	static void CreateCircle(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices, float row = 32.0f, float rad = 1.0f);
	static void CreateArrow(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices);

	unsigned int mNumIndices;
private:
	unsigned int mVertexArray;
	int vertNum;

	std::vector<std::vector<float>> m_vertices;
	std::vector<int> m_dimention;
	std::vector<unsigned short> m_indices;
};

