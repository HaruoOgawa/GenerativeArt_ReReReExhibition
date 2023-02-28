#include "Primitive.h"
#include <glm/glm.hpp>
#include <glew.h>
#include "GraphicsEngine/Graphics/Mesh.h"

Primitive::Primitive(const std::vector<std::vector<float>>& vertices, const std::vector<int>& dimention, const std::vector<unsigned short>& indices):
	m_vertices(vertices),
	m_dimention(dimention),
	m_indices(indices)
{
	CreatePrimitive(vertices,dimention, indices);
}
Primitive::~Primitive() {
	glDeleteVertexArrays(1, &mVertexArray);
}

void Primitive::CreatePrimitive(const std::vector<std::vector<float>>& vertices, const std::vector<int>& dimention, const std::vector<unsigned short>& indices) {
	
	unsigned int mVertexBuffer;
	unsigned int mIndexBuffer;
	mNumIndices = indices.size();
	vertNum = vertices[0].size();

	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	
	for (int i=0;i<vertices.size();i++)
	{
		auto vert = vertices[i];
		auto dim = dimention[i];

		glGenBuffers(1, &mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, dim, GL_FLOAT, GL_FALSE, dim * sizeof(float), 0);

		
	}

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

}

void Primitive::Draw(GLenum DrawVertexWay) {
	SetActive();
	glDrawElements(DrawVertexWay, mNumIndices, GL_UNSIGNED_SHORT, nullptr);
}

void Primitive::SetActive() {
	glBindVertexArray(mVertexArray);
}


void Primitive::CreateBoard(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices) {
	/*std::vector<float> vertex = {
		-0.5f, 0.5f, 0.f,  // top left
		0.5f, 0.5f, 0.f, // top right
		0.5f,-0.5f, 0.f,// bottom right
		-0.5f,-0.5f, 0.f,    // bottom left
	};*/
	
	std::vector<float> vertex= {
		-1.0f, 1.0f, 0.f,  // top left
		1.0f, 1.0f, 0.f, // top right
		1.0f,-1.0f, 0.f,// bottom right
		-1.0f,-1.0f, 0.f,    // bottom left
	};

	std::vector<float> normal = {
		0.f, 0.f, 1.0f,
		0.f, 0.f, 1.0f,
		0.f, 0.f, 1.0f,
		0.f, 0.f, 1.0f,
	};
	
	std::vector<float> texcoord = {
		 0.f, 1.f,
		 1.f, 1.f,
		 1.f, 0.f,
		 0.f, 0.f
	};

	std::vector<unsigned int> i= {
		0, 1, 2,
		2, 3, 0
	};

	vertices->push_back(vertex);
	vertices->push_back(normal);
	vertices->push_back(texcoord);

	dimention->push_back(3);
	dimention->push_back(3);
	dimention->push_back(2);

	for (auto data : i) {
		indices->emplace_back(data);
	}
}

void Primitive::CreatePoint(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices) {
	std::vector<float> vertex = {
		0.0f, 0.0f, 0.0f
	};

	std::vector<float> normal = {
		0.f, 0.f, 0.0f
	};

	std::vector<float> texcoord = {
		 0.0f,0.0f
	};

	std::vector<unsigned int> i = {
		0
	};

	vertices->push_back(vertex);
	vertices->push_back(normal);
	vertices->push_back(texcoord);

	dimention->push_back(3);
	dimention->push_back(3);
	dimention->push_back(2);

	for (auto data : i) {
		indices->emplace_back(data);
	}
}

void Primitive::CreateCube(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices) {

	std::vector<float> vertex = {
		//0 +Z
		-0.5f,-0.5f,0.5f,
		-0.5f,0.5f,0.5f,
		0.5f,0.5f,0.5f,
		0.5f,-0.5f,0.5f,
		//1 -Z
		-0.5f,-0.5f,-0.5f,
		-0.5f,0.5f,-0.5f,
		0.5f,0.5f,-0.5f,
		0.5f,-0.5f,-0.5f,
		//2 -X
		-0.5f,-0.5f,0.5f,
		-0.5f,0.5f,0.5f,
		-0.5f,0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		//3 +X
		0.5f,-0.5f,0.5f,
		0.5f,0.5f,0.5f,
		0.5f,0.5f,-0.5f,
		0.5f,-0.5f,-0.5f,
		//4  -Y
		-0.5f,-0.5f,0.5f,
		-0.5f,-0.5f,-0.5f,
		0.5f,-0.5f,-0.5f,
		0.5f,-0.5f,0.5f,
		//5 +Y
		-0.5f,0.5f,0.5f,
		-0.5f,0.5f,-0.5f,
		0.5f,0.5f,-0.5f,
		0.5f,0.5f,0.5f,
	};

	std::vector<float> normal = {
		//0 +Z
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		//1 -Z
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		//2 -X
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		//3 +X
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		//4 -Y
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		//5 +Y
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
	};

	std::vector<float> texcoord = {
		//0 +Z
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		//1 -Z
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		//2 -X
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		//3 +X
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		//4 -Y
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		//5 +Y
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
	};

	std::vector<unsigned int> i = {
		//0 +Z
		0,1,2,
		2,3,0,
		//1 -Z
		4,5,6,
		6,7,4,
		//2 -X
		8,9,10,
		10,11,8,
		//3 +X
		12,13,14,
		14,15,12,
		//4 -Y
		16,17,18,
		18,19,16,
		//5 +Y
		20,21,22,
		22,23,20,
	};

	vertices->push_back(vertex);
	vertices->push_back(normal);
	vertices->push_back(texcoord);

	dimention->push_back(3);
	dimention->push_back(3);
	dimention->push_back(2);

	for (auto data : i) {
		indices->emplace_back(data);
	}
}


void Primitive::CreateCircle(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices, float row, float rad)
{
	float pi = 3.14159265f;

	std::vector<float> vertex;
	std::vector<float> normal;
	std::vector<float> texcoord;

	// íÜêS
	vertex.push_back(0.0f);
	vertex.push_back(0.0f);
	vertex.push_back(0.0f);

	normal.push_back(0.0f);
	normal.push_back(0.0f);
	normal.push_back(1.0f);

	texcoord.push_back(0.0f);
	texcoord.push_back(0.0f);

	//
	for (float i = 0.0f; i <= row; i++)
	{
		float a = 2.0f * pi / row * i;
		glm::vec3 p = glm::vec3(rad * glm::cos(a), rad * glm::sin(a), 0.0f);
		glm::vec3 n = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec2 st = glm::vec2(glm::cos(a), glm::sin(a)) * 0.5f + 0.5f;

		vertex.push_back(p.x);
		vertex.push_back(p.y);
		vertex.push_back(p.z);

		normal.push_back(n.x);
		normal.push_back(n.y);
		normal.push_back(n.z);

		texcoord.push_back(st.x);
		texcoord.push_back(st.y);
	}

	vertices->push_back(vertex);
	vertices->push_back(normal);
	vertices->push_back(texcoord);

	dimention->push_back(3);
	dimention->push_back(3);
	dimention->push_back(2);

	for (int i = 1; i < row + 1; i++) {
		indices->push_back(0);
		indices->push_back(i);
		indices->push_back(i + 1);
	}
}

void Primitive::CreateArrow(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices)
{
	std::vector<float> vertex;
	std::vector<float> normal;
	std::vector<float> texcoord;

	std::vector<glm::vec3> points = {
		glm::vec3(0.0f, 1.0f, 0.0f) * 0.5f,
		glm::vec3(-1.0f, 0.0f, 0.0f) * 0.5f,
		glm::vec3(1.0f, 0.0f, 0.0f) * 0.5f,
		glm::vec3(-0.15f, 0.0f, 0.0f) * 0.5f,
		glm::vec3(-0.15f, -1.0f, 0.0f) * 0.5f,
		glm::vec3(0.15f, 0.0f, 0.0f) * 0.5f,
		glm::vec3(0.15f, -1.0f, 0.0f) * 0.5f,
	};

	for (const auto& p : points)
	{
		vertex.push_back(p.x); vertex.push_back(p.y); vertex.push_back(p.z);
		normal.push_back(0.0f); normal.push_back(0.0f); normal.push_back(1.0f);
		texcoord.push_back(0.0f); texcoord.push_back(0.0f);
	}

	vertices->push_back(vertex);
	vertices->push_back(normal);
	vertices->push_back(texcoord);

	dimention->push_back(3);
	dimention->push_back(3);
	dimention->push_back(2);

	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);

	indices->push_back(3);
	indices->push_back(4);
	indices->push_back(5);

	indices->push_back(5);
	indices->push_back(4);
	indices->push_back(6);
}
