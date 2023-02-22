#pragma once

enum class RenderQueue
{
	Background = 1000,
	Geometry = 3000,
	Transparent = 4000,
	UI = 5000
};

enum class RenderType {
	DefaultBuffer,
	FrameBuffer,
	PostProcess
};

enum class RenderingSurfaceType
{
	RASTERIZER,
	RAYMARCHING,
};