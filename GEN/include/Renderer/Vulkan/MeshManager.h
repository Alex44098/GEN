#pragma once

#include <tiny_gltf.h>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Mesh.h"
#include "Renderer/Vulkan/Vulkan.h"

class MeshManager {
	std::vector<Mesh> meshes;

public:
	MeshId AddMesh(gvk::Vulkan& vulkan, Mesh& mesh);
	void CleanMeshes(gvk::Vulkan& vulkan);

private:
	void LoadToBuffer(gvk::Vulkan& vulkan, Mesh& mesh) const;
};