#pragma once
#include <string>

namespace shaderlib {
	const std::string Standard_vert = std::string({
		#include "GraphicsEngine/Graphics/Shader/Standard_VertComp.h"
	});

	const std::string Standard_frag = std::string({
		#include "GraphicsEngine/Graphics/Shader/Standard_FragComp.h"
	});

	const std::string StandardRenderBoard_vert = std::string({
		#include "GraphicsEngine/Graphics/Shader/StandardRenderBoard_VertComp.h"
	});

	const std::string LatePostProcess_frag = std::string({
		#include "GraphicsEngine/Graphics/Shader/LatePostProcess_FragComp.h"
	});
}