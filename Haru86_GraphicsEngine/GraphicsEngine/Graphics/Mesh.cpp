#include "Mesh.h"
#include "./Primitive.h"
#include <vector>

Mesh::Mesh(PrimitiveType primType) {
	CreateMesh(primType);
}

Mesh::Mesh(const std::vector<std::vector<float>>& VertexData, const std::vector<int>& Dimention, const std::vector<unsigned short>& Indices)
{
	std::shared_ptr<Primitive> prim = std::make_shared<Primitive>(VertexData, Dimention, Indices);
	m_primitives.push_back(prim);
}

void Mesh::CreateMesh(PrimitiveType primType) {
	
	std::vector<std::vector<float>> VertexData;
	std::vector<int> Dimention;
	std::vector<unsigned short> Indices;

	switch (primType)
	{
	case PrimitiveType::BOARD:
		Primitive::CreateBoard(&VertexData, &Dimention,&Indices);
		break;
	case PrimitiveType::POINT:
		Primitive::CreatePoint(&VertexData, &Dimention, &Indices);
		break;
	case PrimitiveType::CUBE:
		Primitive::CreateCube(&VertexData, &Dimention, &Indices);
		break;
	case PrimitiveType::CIRCLE:
		Primitive::CreateCircle(&VertexData, &Dimention, &Indices);
		break;
	case PrimitiveType::ARROW:
		Primitive::CreateArrow(&VertexData, &Dimention, &Indices);
		break;
	default:
		Primitive::CreateBoard(&VertexData, &Dimention, &Indices);
		break;
	}
	
	std::shared_ptr<Primitive> prim = std::make_shared<Primitive>(VertexData, Dimention, Indices);
	m_primitives.push_back(prim);

}

void Mesh::Draw(GLenum DrawVertexWay) {
	for (int i = 0; i < m_primitives.size();i++) {
		m_primitives[i]->Draw(DrawVertexWay);
	}
}

void Mesh::DrawInstancedWithMesh(int count, GLenum rendermode) {
	for (int i = 0; i < m_primitives.size(); i++) {
		m_primitives[i]->SetActive();
		glDrawElementsInstanced(rendermode, m_primitives[i]->mNumIndices, GL_UNSIGNED_SHORT, nullptr, count);
	}
}