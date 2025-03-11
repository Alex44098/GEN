#pragma once

// Read more: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#materials

#include <filesystem>
#include <tiny_gltf.h>

#include "Renderer/Vulkan/Material.h"
#include "Renderer/Vulkan/Vulkan.h"

Material LoadMaterial(gvk::Vulkan& vulkan, const tinygltf::Model& model, const std::filesystem::path& path, const tinygltf::Material& gltfMaterial);

LinearColor GetDiffuseColor(const tinygltf::Material& gltfMaterial);
std::filesystem::path GetDiffuseTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir);
std::filesystem::path GetNormalMapTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir);

bool HasDiffuseTexture(const tinygltf::Material& gltfMaterial);
bool HasNormalMapTexture(const tinygltf::Material& gltfMaterial);
