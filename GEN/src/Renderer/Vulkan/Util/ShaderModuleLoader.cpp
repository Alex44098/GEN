#include "Renderer/Vulkan/Util/ShaderModuleLoader.h"

namespace Util {
	VkShaderModule LoadShaderModule(const char* filePath, VkDevice device) {
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			std::exit(1);

		const auto fileSize = file.tellg();
		std::vector<std::uint32_t> buffer(fileSize / sizeof(std::uint32_t));
		file.seekg(0);
		file.read((char*)buffer.data(), fileSize);
		file.close();

		const VkShaderModuleCreateInfo shaderModuleInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = buffer.size() * sizeof(std::uint32_t),
			.pCode = buffer.data()
		};

		VkShaderModule module;

		vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &module);

		return module;
	}
}