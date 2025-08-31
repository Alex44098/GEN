#include "Renderer/Vulkan/BindlessManager.h"

void BindlessManager::Init(VkDevice device) {
	this->device = device;

	this->CreateDescriptorPool();
	this->CreateDescriptorSetLayout();
	this->AllocateDescriptorSet();
}

void BindlessManager::InitSamplers(float maxAnisotropy) {
	this->InitDefaultSamplers(maxAnisotropy);
}

void BindlessManager::CreateDescriptorPool() {
	const VkDescriptorPoolSize poolSizes[2]{
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_BINDLESS},
		{VK_DESCRIPTOR_TYPE_SAMPLER, MAX_SAMPLERS}
	};

	const VkDescriptorPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT,
		.maxSets = MAX_BINDLESS * 2,
		.poolSizeCount = 2U,
		.pPoolSizes = poolSizes
	};

	VK_CHECK(vkCreateDescriptorPool(this->device, &poolInfo, nullptr, &(this->descriptorPool)));
}

void BindlessManager::CreateDescriptorSetLayout() {
	const VkDescriptorSetLayoutBinding bindings[2]{
		{
			.binding = TEX_BIND,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.descriptorCount = MAX_BINDLESS,
			.stageFlags = VK_SHADER_STAGE_ALL
		},
		{
			.binding = SAMP_BIND,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.descriptorCount = MAX_SAMPLERS,
			.stageFlags = VK_SHADER_STAGE_ALL
		}
	};
	const VkDescriptorBindingFlags bindingFlags[2]{
		{VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT},
		{VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT}
	};
	const VkDescriptorSetLayoutBindingFlagsCreateInfo flagsCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.bindingCount = 2U,
		.pBindingFlags = bindingFlags
	};
	const VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &flagsCreateInfo,
		.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
		.bindingCount = 2,
		.pBindings = bindings
	};

	VK_CHECK(vkCreateDescriptorSetLayout(this->device, &descriptorCreateInfo, nullptr, &(this->descriptorSetLayout)));
}

void BindlessManager::AllocateDescriptorSet() {
	const VkDescriptorSetAllocateInfo descriptorAllocInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = this->descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &(this->descriptorSetLayout)
	};
	GECS::u32 maxBinding = MAX_BINDLESS - 1;
	const VkDescriptorSetVariableDescriptorCountAllocateInfo descCountAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
		.descriptorSetCount = 1,
		.pDescriptorCounts = &maxBinding,
	};

	VK_CHECK(vkAllocateDescriptorSets(this->device, &descriptorAllocInfo, &(this->descriptorSet)));
}

void BindlessManager::InitDefaultSamplers(float maxAnisotropy) {
	const VkSamplerCreateInfo nearestCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_NEAREST,
			.minFilter = VK_FILTER_NEAREST,
	};
	VK_CHECK(vkCreateSampler(this->device, &nearestCreateInfo, nullptr, &(this->nearestSampler)));
	this->AddSampler(NEAREST_SAMPLER_ID, this->nearestSampler);

	const VkSamplerCreateInfo linearCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = maxAnisotropy,
			.minLod = 0.0f,
			.maxLod = VK_LOD_CLAMP_NONE,
	};
	VK_CHECK(vkCreateSampler(this->device, &linearCreateInfo, nullptr, &(this->linearSampler)));
	this->AddSampler(LINEAR_SAMPLER_ID, this->linearSampler);
}

void BindlessManager::Clear() {
	vkDestroySampler(this->device, this->nearestSampler, nullptr);
	vkDestroySampler(this->device, this->linearSampler, nullptr);

	vkDestroyDescriptorSetLayout(this->device, this->descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(this->device, this->descriptorPool, nullptr);
}

void BindlessManager::AddImage(ImageId id, const VkImageView imageView) {
	const VkDescriptorImageInfo imageInfo{
		.imageView = imageView,
		.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
	};
	const VkWriteDescriptorSet writeSet{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = this->descriptorSet,
		.dstBinding = TEX_BIND,
		.dstArrayElement = id,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		.pImageInfo = &imageInfo,
	};
	vkUpdateDescriptorSets(this->device, 1, &writeSet, 0, nullptr);
}

void BindlessManager::AddSampler(ImageId id, VkSampler sampler) {
	const VkDescriptorImageInfo imageInfo{
		.sampler = sampler,
		.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
	};
	const VkWriteDescriptorSet writeSet{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = this->descriptorSet,
		.dstBinding = SAMP_BIND,
		.dstArrayElement = id,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
		.pImageInfo = &imageInfo,
	};
	vkUpdateDescriptorSets(this->device, 1, &writeSet, 0, nullptr);
}