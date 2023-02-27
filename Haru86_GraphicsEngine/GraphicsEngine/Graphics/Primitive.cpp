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

void Primitive::CreateSphere(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices, float row, float column, float rad) {
	float pi = 3.14159265f;

	std::vector<float> vertex;
	std::vector<float> normal;
	std::vector<float> texcoord;

	for (auto i = 0; i <= row; i++) {
		auto r = pi / row * i;
		auto ry = glm::cos(r);
		auto rr = glm::sin(r);
		for (auto ii = 0; ii <= column; ii++) {
			float tr = pi * 2 / column * ii;
			float tx = rr * rad * glm::cos(tr);
			float ty = ry * rad;
			float tz = rr * rad * glm::sin(tr);
			float rx = rr * glm::cos(tr);
			float rz = rr * glm::sin(tr);

			vertex.push_back(tx);
			vertex.push_back(ty);
			vertex.push_back(tz);
			normal.push_back(rx);
			normal.push_back(ry);
			normal.push_back(rz);
			texcoord.push_back(static_cast<float>(1 - 1 / column * ii));
			texcoord.push_back(static_cast<float>(1 / row * i));
		}
	}

	vertices->push_back(vertex);
	vertices->push_back(normal);
	vertices->push_back(texcoord);

	dimention->push_back(3);
	dimention->push_back(3);
	dimention->push_back(2);

	for (int i = 0; i < row; i++) {
		for (int ii = 0; ii < column; ii++) {
			int r = (column + 1) * i + ii;

			indices->emplace_back(static_cast<int>(r));
			indices->emplace_back(static_cast<int>(r + 1));
			indices->emplace_back(static_cast<int>(r + column + 2));
			indices->emplace_back(static_cast<int>(r));
			indices->emplace_back(static_cast<int>(r + column + 2));
			indices->emplace_back(static_cast<int>(r + column + 1));
		}
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

void Primitive::CreateIcosahedron(std::vector<std::vector<float>>* vertices, std::vector<int>* dimention, std::vector<unsigned short>* indices)
{
	//
	float phi = (1.0f + sqrt(5.0f)) * 0.5f; // â©ã‡î‰
	float a = 1.0f;
	//float b = 1.0f / phi;
	float b = phi;
	float mulv = (1.0f / sqrt(1.0f + pow(b, 2.0f)));

	//
	/*glm::vec3 v0 = glm::vec3(0.0f, b, -a);
	glm::vec3 v1 = glm::vec3(b, a, 0.0f);
	glm::vec3 v2 = glm::vec3(-b, a, 0.0f);
	glm::vec3 v3 = glm::vec3(0.0f, b, a);
	glm::vec3 v4 = glm::vec3(0.0f, -b, a);
	glm::vec3 v5 = glm::vec3(-a, 0, b);
	glm::vec3 v6 = glm::vec3(0.0f, -b, -a);
	glm::vec3 v7 = glm::vec3(a, 0.0f, -b);
	glm::vec3 v8 = glm::vec3(a, 0.0f, b);
	glm::vec3 v9 = glm::vec3(-a, 0.0f, -b);
	glm::vec3 v10 = glm::vec3(b, -a, 0.0f);
	glm::vec3 v11 = glm::vec3(-b, -a, 0.0f);*/
	
	glm::vec3 v0 = glm::vec3(b, a, 0.0f) * mulv;
	glm::vec3 v1 = glm::vec3(-b, a, 0.0f) * mulv;
	glm::vec3 v2 = glm::vec3(b, -a, 0.0f) * mulv;
	glm::vec3 v3 = glm::vec3(-b, -a, 0.0f) * mulv;
	glm::vec3 v4 = glm::vec3(a, 0.0f, b) * mulv;
	glm::vec3 v5 = glm::vec3(a, 0.0f, -b) * mulv;
	glm::vec3 v6 = glm::vec3(-a, 0.0f, b) * mulv;
	glm::vec3 v7 = glm::vec3(-a, 0.0f, -b) * mulv;
	glm::vec3 v8 = glm::vec3(0.0f, b, a) * mulv;
	glm::vec3 v9 = glm::vec3(0.0f, -b, a) * mulv;
	glm::vec3 v10 = glm::vec3(0.0f, b, -a) * mulv;
	glm::vec3 v11 = glm::vec3(0.0f, -b, -a) * mulv;

	//
	std::vector<float> vertex = {
		v0.x, v0.y, v0.z,
		v1.x, v1.y, v1.z,
		v2.x, v2.y, v2.z,
		v3.x, v3.y, v3.z,
		v4.x, v4.y, v4.z,
		v5.x, v5.y, v5.z,
		v6.x, v6.y, v6.z,
		v7.x, v7.y, v7.z,
		v8.x, v8.y, v8.z,
		v9.x, v9.y, v9.z,
		v10.x, v10.y, v10.z,
		v11.x, v11.y, v11.z,
	};

	std::vector<unsigned int> i = {
		0,4,
		4,8, // 0
		8,0,

		0,2,
		2,4, // 1
		4,0,

		0,5,
		5,2, // 2
		2,0,

		0,10,
		10,5, // 3
		5,0,

		0,8,
		8,10, // 4
		10,0,

		////////////////////

		1,8,
		8,4, // 5
		8,6,

		4,6,
		6,1, // 6
		8,6,

		///////////////

		6,4,
		4,2, // 7
		4,9,

		2,9,
		9,6, // 8
		4,9,

		////////

		9,2,
		2,5, // 9
		2,11,

		5,11,
		11,9, // 10
		2,11,

		11,5,
		5,10, // 11
		5,7,

		10,7,
		7,11, // 12
		5,7,

		7,10,
		10,8, // 13
		10,1,

		8,1,
		1,7, // 14
		10,1,

		3,1,
		1,6, // 15
		1,6,

		3,6,
		6,9, // 16
		1,6,

		3,9,
		9,11, // 17
		11,3,

		3,11,
		11,7, // 18
		7,3,

		3,7,
		7,1, // 19
		1,3,

	};
	
	/*std::vector<unsigned int> i = {
		2, 1, 0,
		1, 2, 3,
		5, 4, 3,
		4, 8, 3,
		7, 6, 0,
		6, 9, 0,
		11, 10, 4,
		10, 11, 6,
		9, 5, 2,
		5, 9, 11,
		8, 7, 1,
		7, 8, 10,
		2, 5, 3,
		8, 1, 3,
		9, 2, 0,
		1, 7, 0,
		11, 9, 6,
		7, 10, 6,
		5, 11, 4,
		10, 8, 4,
	};*/

	//
	vertices->push_back(vertex);

	dimention->push_back(3);

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
