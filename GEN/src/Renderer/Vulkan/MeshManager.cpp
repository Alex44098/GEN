#include "Renderer/Vulkan/MeshManager.h"

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

	VkCommandBuffer cmd = vulkan.BeginCommandBufferRecord();

	VkBufferCopy vertexCopy;
	vertexCopy.srcOffset = 0;
	vertexCopy.dstOffset = 0;
	vertexCopy.size = vertexBufferSize;
	vkCmdCopyBuffer(cmd, stagingBuffer.vkBuffer, mesh.vertexBuffer.vkBuffer, 1, &vertexCopy);

	VkBufferCopy indexCopy;
	indexCopy.srcOffset = vertexBufferSize;
	indexCopy.dstOffset = 0;
	indexCopy.size = indexBufferSize;
	vkCmdCopyBuffer(cmd, stagingBuffer.vkBuffer, mesh.indexBuffer.vkBuffer, 1, &indexCopy);

	vulkan.EndCommandBufferRecord(cmd);

	vulkan.DestroyBuffer(stagingBuffer);
}

void MeshManager::CleanMeshes(gvk::Vulkan& vulkan) {
	for (const Mesh& mesh : this->meshes) {
		vulkan.DestroyBuffer(mesh.vertexBuffer);
		vulkan.DestroyBuffer(mesh.indexBuffer);
	}
}