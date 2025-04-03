#pragma once

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Buffer.h"
#include "Renderer/Vulkan/Vulkan.h"

class PreallocatedBuffer {
	std::vector<Buffer> stagingBuffers;
	Buffer gpuBuffer;
	std::size_t gpuBufferSize{ 0 };

public:
	void Init(gvk::Vulkan& vulkan, VkBufferUsageFlags usage, std::size_t dataSize, const char* label);
	void Cleanup(gvk::Vulkan& vulkan);

	void UploadNewFrameData(VkCommandBuffer cmdBuffer, GECS::u32 currentFrame, void* data,
		std::size_t dataSize, std::size_t offset, bool sync) const;

	inline const Buffer& GetBuffer() const { return this->gpuBuffer; }
};