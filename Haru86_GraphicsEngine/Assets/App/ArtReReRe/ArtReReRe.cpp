#include "ArtReReRe.h"
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"
#include "GraphicsEngine/Graphics/GraphicsRenderer.h"
#include "scripts/CTrailObject.h"
#include "GraphicsEngine/Sound/SoundPlayer.h"

#ifdef _DEBUG
#include "GraphicsEngine/Message/Console.h"
#endif // _DEBUG


namespace app
{
    ArtReReRe::ArtReReRe() :
        m_SceneIndex(0),
        m_SceneStartTime(0.0f),
        m_SceneEndTime(0.0f),
        m_LocalTime(0.0f),
        m_LoopedNum(1.0f),
        m_TrailObject(std::make_shared<CTrailObject>())
    {
    }

    void ArtReReRe::Start()
    {
        m_TrailObject->Init();
    }
    void ArtReReRe::Update()
    {
        GraphicsMain::GetInstance()->m_GroabalLightPosition->m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        GraphicsRenderer::GetInstance()->SetBackgroudColor(glm::vec4(glm::vec3(0.01f), 1.0f));

        GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(0.0f, 0.0f, 100.0f);
        float r = 75.0f, s = 0.1f, t = GraphicsMain::GetInstance()->m_SecondsTime;
        //GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(r * glm::cos(t * s), 0.0f, r * glm::sin(t * s));
        //GraphicsMain::GetInstance()->m_MainCamera->m_position = glm::vec3(100.0f * glm::cos(GraphicsMain::GetInstance()->m_SecondsTime), 0.0f, 100.0f * glm::sin(GraphicsMain::GetInstance()->m_SecondsTime));

        //
        m_TrailObject->Update();

        //
        if (t >= m_LoopedNum * (4.0f * 60.0f))
        {
            GraphicsMain::GetInstance()->m_SoundPlayer->Skip(0.0f);

            m_LoopedNum++;

            if (m_LoopedNum > 15.0f)
            {
                GraphicsMain::GetInstance()->isRunning = false;
                return;
            }

#ifdef _DEBUG
            Console::Log("[AppLog] m_LoopedNum: %f\n", m_LoopedNum);
#endif // _DEBUG

        }
    }

    void ArtReReRe::Draw(bool IsRaymarching)
    {
        if (!IsRaymarching)
        {
            m_TrailObject->Draw();
        }
    }

    void ArtReReRe::UpdateTimeline()
    {
    }
}