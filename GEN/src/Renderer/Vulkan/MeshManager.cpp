#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

MeshId MeshManager::AddMesh(gvk::Vulkan& vulkan, Mesh& mesh) {
	this->LoadToBuffer(vulkan, mesh);

	const GECS::u32 id = this->meshes.size();
	this->meshes.push_back(std::move(mesh));

	return id;
}

const Mesh& MeshManager::GetMesh(MeshId id) const {
	return this->meshes.at(id);
}

void MeshManager::LoadToBuffer(gvk::Vulkan& vulkan, Mesh& mesh) const {
	const GECS::u32 vertexBufferSize = mesh.vertices.size() * sizeof(Mesh::Vertex);
	mesh.vertexBuffer = vulkan.CreateBuffer(vertexBufferSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_AUTO);

	const GECS::u32 indexBufferSize = mesh.indices.size() * sizeof(GECS::u32);
	mesh.indexBuffer = vulkan.CreateBuffer(indexBufferSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO);

	const Buffer stagingBuffer = vulkan.CreateBuffer(vertexBufferSize + indexBufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO);

	// move data to staging buffer
	void* data = stagingBuffer.allocInfo.pMappedData;
	memcpy(data, mesh.vertices.data(), vertexBufferSize);
	memcpy((char*)data + vertexBufferSize, mesh.indices.data(), indexBufferSize);

	VkCommandBuffer cmdBuffer = vulkan.BeginCommandBufferRecord();

	const VkBufferCopy vertexCopy{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = vertexBufferSize
	};
	vkCmdCopyBuffer(cmdBuffer, stagingBuffer.vkBuffer, mesh.vertexBuffer.vkBuffer, 1, &vertexCopy);

	const VkBufferCopy indexCopy{
		.srcOffset = vertexBufferSize,
		.dstOffset = 0,
		.size = indexBufferSize
	};
	vkCmdCopyBuffer(cmdBuffer, stagingBuffer.vkBuffer, mesh.indexBuffer.vkBuffer, 1, &indexCopy);

	vulkan.EndCommandBufferRecord(cmdBuffer);

	Debug::AddDebugLabel4Buffer(vulkan.GetDevice(), mesh.vertexBuffer.vkBuffer, "Mesh (Vertex)");
	Debug::AddDebugLabel4Buffer(vulkan.GetDevice(), mesh.indexBuffer.vkBuffer, "Mesh (Index)");

	vulkan.DestroyBuffer(stagingBuffer);
}

void MeshManager::CleanMeshes(gvk::Vulkan& vulkan) {
	for (const Mesh& mesh : this->meshes) {
		vulkan.DestroyBuffer(mesh.vertexBuffer);
		vulkan.DestroyBuffer(mesh.indexBuffer);
	}
}