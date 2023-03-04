#include "ArtReReRe.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "scripts/CWhiteWall.h"
#include "scripts/CTrailObject.h"

namespace app
{
    ArtReReRe::ArtReReRe() :
        m_SceneIndex(0),
        m_SceneStartTime(0.0f),
        m_SceneEndTime(0.0f),
        m_LocalTime(0.0f),
        m_WhiteWall(std::make_shared<CWhiteWall>()),
        m_TrailObject(std::make_shared<CTrailObject>())
    {
    }

    void ArtReReRe::Start()
    {
        m_WhiteWall->Init(m_TrailObject->GetWallHalfSize());
        m_TrailObject->Init();
    }
    void ArtReReRe::Update()
    {
        GraphicsMain::GetInstance()->m_GroabalLightPosition->m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        GraphicsRenderer::GetInstance()->SetBackgroudColor(glm::vec4(glm::vec3(0.01f), 1.0f));
        //GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 0.0f, -50.0f);
        //GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 0.0f, -65.0f);
        GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 0.0f, 60.0f);
        //GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(100.0f * glm::cos(GraphicsMain::GetInstance()->m_SecondsTime), 0.0f, 100.0f * glm::sin(GraphicsMain::GetInstance()->m_SecondsTime));
        //GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 0.0f, -10.0f);

        //
        m_WhiteWall->Update();
        m_TrailObject->Update();
    }

    void ArtReReRe::Draw(bool IsRaymarching)
    {
        if (IsRaymarching)
        {
        }
        else
        {
            //m_WhiteWall->Draw();
            m_TrailObject->Draw();
        }
    }

    void ArtReReRe::UpdateTimeline()
    {

    }
}