#pragma once

#include <Volk/volk.h>

#include "Renderer/RenderTypes.h"

class BindlessManager {
	VkDevice device;

	VkDescriptorPool descriptorPool{};
	VkDescriptorSetLayout descriptorSetLayout{};
	VkDescriptorSet descriptorSet{};

	VkSampler nearestSampler{};
	VkSampler linearSampler{};

public:
	void Init(VkDevice device);

	void InitSamplers(float maxAnisotropy);
	void Clear();

	VkDescriptorSetLayout getDescriptorSetLayout() const { return this->descriptorSetLayout; }
	const VkDescriptorSet& getDescriptorSet() const { return this->descriptorSet; }

	void AddImage(ImageId id, const VkImageView imageView);
	void AddSampler(ImageId id, VkSampler sampler);

private:
	void CreateDescriptorPool();
	void CreateDescriptorSetLayout();
	void AllocateDescriptorSet();

	void InitDefaultSamplers(float maxAnisotropy);
};