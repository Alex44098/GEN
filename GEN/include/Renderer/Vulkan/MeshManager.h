#pragma once

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Mesh.h"
#include "Renderer/Vulkan/Vulkan.h"

class MeshManager {
	std::vector<Mesh> meshes;

public:
	MeshId AddMesh(gvk::Vulkan& vulkan, Mesh& mesh);
	void CleanMeshes(gvk::Vulkan& vulkan);

	const Mesh& GetMesh(MeshId mesh) const;

private:
	void LoadToBuffer(gvk::Vulkan& vulkan, Mesh& mesh) const;
};