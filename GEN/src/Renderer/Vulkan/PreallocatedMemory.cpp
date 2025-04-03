#include "Renderer/Vulkan/PreallocatedBuffer.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

void PreallocatedBuffer::Init(gvk::Vulkan& vulkan, VkBufferUsageFlags usage, std::size_t dataSize, const char* label) {
	this->gpuBufferSize = dataSize;

	this->gpuBuffer = vulkan.CreateBuffer(dataSize, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	Debug::AddDebugLabel4Buffer(vulkan.GetDevice(), this->gpuBuffer.vkBuffer, label);

	for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		this->stagingBuffers.push_back(vulkan.CreateBuffer(dataSize, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO_PREFER_HOST));
}

void PreallocatedBuffer::Cleanup(gvk::Vulkan& vulkan) {
	for (const Buffer& statingBuffer : this->stagingBuffers)
		vulkan.DestroyBuffer(statingBuffer);
	this->stagingBuffers.clear();

	vulkan.DestroyBuffer(this->gpuBuffer);
}

void PreallocatedBuffer::UploadNewFrameData(VkCommandBuffer cmdBuffer, GECS::u32 currentFrame, void* data,
	std::size_t dataSize, std::size_t offset, bool sync) const {

	assert(this->stagingBuffers.size() != 0 && "Preallocated buffer didn't init"); // checking for initialization
	assert(offset + dataSize <= this->gpuBufferSize && "Preallocated buffer: out of bounds");
	if (dataSize == 0)
		return;

	if (sync) {
		const VkBufferMemoryBarrier2 bufferBarrier{
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			.srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
			.buffer = this->gpuBuffer.vkBuffer,
			.offset = 0,
			.size = VK_WHOLE_SIZE,
		};
		const VkDependencyInfo dependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.bufferMemoryBarrierCount = 1,
			.pBufferMemoryBarriers = &bufferBarrier,
		};
		vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
	}

	const Buffer& stagingBuffer = this->stagingBuffers[currentFrame];
	std::uint8_t* mappedData = reinterpret_cast<std::uint8_t*>(stagingBuffer.allocInfo.pMappedData);
	memcpy((void*)&mappedData[offset], data, dataSize);
	
	const VkBufferCopy2 region{
		.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
		.srcOffset = (VkDeviceSize)offset,
		.dstOffset = (VkDeviceSize)offset,
		.size = dataSize
	};

	const VkCopyBufferInfo2 bufCopyInfo{
		.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
		.srcBuffer = stagingBuffer.vkBuffer,
		.dstBuffer = this->gpuBuffer.vkBuffer,
		.regionCount = 1,
		.pRegions = &region,
	};

	vkCmdCopyBuffer2(cmdBuffer, &bufCopyInfo);

	if (sync) {
		const VkBufferMemoryBarrier2 bufferBarrier{
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
			.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
			.buffer = this->gpuBuffer.vkBuffer,
			.offset = 0,
			.size = VK_WHOLE_SIZE,
		};
		const VkDependencyInfo dependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.bufferMemoryBarrierCount = 1,
			.pBufferMemoryBarriers = &bufferBarrier,
		};
		vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
	}
}