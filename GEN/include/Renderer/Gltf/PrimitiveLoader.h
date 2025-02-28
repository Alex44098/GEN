#pragma once

#include <span>
#include "tiny_gltf.h"

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Mesh.h"

#define GLTF_POSITIONS_ACCESSOR "POSITION"
#define GLTF_NORMALS_ACCESSOR "NORMAL"
#define GLTF_TANGENTS_ACCESSOR "TANGENT"
#define GLTF_TEXCOORD_ACCESSOR "TEXCOORD_0"

GECS::i32 GetAccessorIndex(const tinygltf::Primitive& primitive, const std::string& name);
Mesh CreateMeshFromPrimitive(const tinygltf::Model& model, const std::string& name, const tinygltf::Primitive& primitive);