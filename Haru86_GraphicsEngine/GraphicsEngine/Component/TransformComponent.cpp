#include "TransformComponent.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../GraphicsMain/GraphicsMain.h"

TransformComponent::TransformComponent(glm::vec3 pos,glm::vec3 rot,glm::vec3 s, glm::vec3 center)
	: m_center(center), m_up(glm::vec3(0.0f,1.0f,0.0f)),m_position(pos), m_rotation(rot), m_scale(s)
{
	CalMatrix();
}

void TransformComponent::CalMatrix() {
	ComputeModelMatrix();
	ComputeViewMatrix();
	ComputePerspectiveMatrix();
}

void TransformComponent::ComputeModelMatrix() {
	glm::mat4 translationMatirx = glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, m_position.z));
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat(m_rotation));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(m_scale.x, m_scale.y, m_scale.z));

	m_mMatrix = translationMatirx* rotationMatrix * scaleMatrix;
}

void TransformComponent::ComputeViewMatrix() {
	if(GraphicsMain::GetInstance()->m_UsingCamera)
	{
		m_vMatrix = glm::lookAt(
			GraphicsMain::GetInstance()->m_UsingCamera->m_position,
			GraphicsMain::GetInstance()->m_UsingCamera->m_center,
			GraphicsMain::GetInstance()->m_UsingCamera->m_up
		);
	}
	else {
		m_vMatrix = glm::lookAt(
			glm::vec3(0.0f, 0.0f, -3.0f),
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}
}

void TransformComponent::ComputePerspectiveMatrix() {
	float width = static_cast<float>(GraphicsRenderer::GetInstance()->GetScreenSize().x);
	float height = static_cast<float>(GraphicsRenderer::GetInstance()->GetScreenSize().y);
	
	m_pMatrix = glm::perspective(
		glm::radians(90.0f),
		width/height,
		0.1f,
		10000.0f
	);
}