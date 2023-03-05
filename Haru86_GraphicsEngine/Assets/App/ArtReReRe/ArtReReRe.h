#pragma once
#include <memory>

namespace app
{
    class CTrailObject;

    class ArtReReRe
    {
        unsigned int m_SceneIndex;
        float        m_SceneStartTime;
        float        m_SceneEndTime;
        float        m_LocalTime;
        float        m_LoopedNum;

        std::shared_ptr<CTrailObject> m_TrailObject;
    public:
        ArtReReRe();
        ~ArtReReRe() = default;

        void Start();
        void Update();
        void Draw(bool IsRaymarching);
        void UpdateTimeline();

        unsigned int GetSceneIndex()const { return m_SceneIndex; }
        float        GetSceneStartTime() const { return m_SceneStartTime; }
        float        GetSceneEndTime() const { return m_SceneEndTime; }
    };
}

