#pragma once
#include <memory>

class MeshRendererComponent;
namespace text {class TextObject;}

namespace app
{
    class MoonSea;
    class EarthAndMoon;
    class MoonTravel;
    class ChangeOfMind;
    class RoughMikado;
    class FeatherRobe;
    class FindKaguya;
    class MountFuji;

    class GradDemo
    {
        std::shared_ptr<MoonSea> m_MoonSea;
        std::shared_ptr<EarthAndMoon> m_EarthAndMoon;
        std::shared_ptr<MoonTravel> m_MoonTravel;
        std::shared_ptr<ChangeOfMind> m_ChangeOfMind;
        std::shared_ptr<RoughMikado> m_RoughMikado;
        std::shared_ptr<FeatherRobe> m_FeatherRobe;
        std::shared_ptr<FindKaguya> m_FindKaguya;
        std::shared_ptr<MountFuji> m_MountFuji;

        unsigned int m_SceneIndex;
        float        m_SceneStartTime;
        float        m_SceneEndTime;
        float        m_LocalTime;
    public:
        GradDemo();
        ~GradDemo() = default;

        void Start();
        void Update();
        void Draw(bool IsRaymarching);
        void UpdateTimeline();

        unsigned int GetSceneIndex()const { return m_SceneIndex; }
        float        GetSceneStartTime() const { return m_SceneStartTime; }
        float        GetSceneEndTime() const { return m_SceneEndTime; }
    };
}

