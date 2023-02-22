#pragma once
#include <memory>

class PolygonRaymarchingMixer
{
public:
	PolygonRaymarchingMixer();
	virtual ~PolygonRaymarchingMixer()=default;
	void Draw(bool IsDepthMix);
private:
	std::shared_ptr<class MeshRendererComponent> m_MixerRenderer;
};

