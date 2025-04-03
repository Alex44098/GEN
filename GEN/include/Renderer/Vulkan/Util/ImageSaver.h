#pragma once

#include <filesystem>

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/RenderTypes.h"

namespace Util {
	void SaveImage(gvk::Vulkan& vulkan, const Image& drawImage);
}