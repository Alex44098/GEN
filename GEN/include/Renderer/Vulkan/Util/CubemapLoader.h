#pragma once

#include <filesystem>

#include "Renderer/Vulkan/Vulkan.h"

namespace Util {
	ImageId LoadCubemap(gvk::Vulkan& vulkan, const std::filesystem::path& cubemapDir);
}