#pragma once
#include <glm/glm.hpp>

class TransformComponent
{
    
public:
    TransformComponent(glm::vec3 pos=glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 s = glm::vec3(1.0f),glm::vec3 center=glm::vec3(0.0f));
    ~TransformComponent() = default;
    
    void CalMatrix();
    void ComputeModelMatrix();
    void ComputeViewMatrix();
    void ComputePerspectiveMatrix();

    glm::vec3 m_center;
    glm::vec3 m_up;

    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_mMatrix;
    glm::mat4 m_vMatrix;
    glm::mat4 m_pMatrix;

};

