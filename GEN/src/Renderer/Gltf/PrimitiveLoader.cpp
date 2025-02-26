#include "Renderer/Gltf/PrimitiveLoader.h"

template<typename T>
std::span<const T> LoadGLTFBufferToVector(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
	const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView]; // without coping
	const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer]; // without coping

	const T* data = reinterpret_cast<const T*>(&buffer.data.at(0) + bufferView.byteOffset + accessor.byteOffset);
	return std::span{ data, accessor.count };
}

GECS::i32 GetAccessorIndex(const tinygltf::Primitive& primitive, const std::string& name) {
	for (const std::pair<std::string, GECS::i32>& attibute : primitive.attributes)
		if (attibute.first == name)
			return attibute.second;
	return -1;
}

Mesh CreateMeshFromPrimitive(const tinygltf::Model& model, const std::string& name, const tinygltf::Primitive& primitive) {
	Mesh mesh;
	mesh.name = name;
	
	// POSITIONS
	GECS::i32 positionsAccessorIndex = GetAccessorIndex(primitive, GLTF_POSITIONS_ACCESSOR);
	const std::span<const glm::vec3> positions = LoadGLTFBufferToVector<glm::vec3>(model, model.accessors[positionsAccessorIndex]);
	mesh.vertices.resize(positions.size());
	for (GECS::u32 i = 0; i < positions.size(); i++)
		mesh.vertices[i].position = positions[i];

	// NORMALS
	GECS::i32 normalsAccessorIndex = GetAccessorIndex(primitive, GLTF_NORMALS_ACCESSOR);
	if (normalsAccessorIndex != -1) {
		const std::span<const glm::vec3> normals = LoadGLTFBufferToVector<glm::vec3>(model, model.accessors[normalsAccessorIndex]);
		for (GECS::u32 i = 0; i < normals.size(); i++)
			mesh.vertices[i].normal = normals[i];
	}

	// TANGENTS
	GECS::i32 tangentsAccessorIndex = GetAccessorIndex(primitive, GLTF_TANGENTS_ACCESSOR);
	if (tangentsAccessorIndex != -1) {
		const std::span<const glm::vec4> tangents = LoadGLTFBufferToVector<glm::vec4>(model, model.accessors[tangentsAccessorIndex]);
		for (GECS::u32 i = 0; i < tangents.size(); i++)
			mesh.vertices[i].tangent = tangents[i];
	}

	// TEXCOORDS
	GECS::i32 texAccessorIndex = GetAccessorIndex(primitive, GLTF_TEXCOORD_ACCESSOR);
	if (texAccessorIndex != -1) {
		const std::span<const glm::vec2> texCoords = LoadGLTFBufferToVector<glm::vec2>(model, model.accessors[texAccessorIndex]);
		for (GECS::u32 i = 0; i < texCoords.size(); i++) {
			mesh.vertices[i].texCoord.x = texCoords[i].x;
			mesh.vertices[i].texCoord.y = texCoords[i].y;
		}
	}

	// INDICES
	if (primitive.indices != -1) {
		const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
		const std::span<const GECS::u16> indices = LoadGLTFBufferToVector<GECS::u16>(model, accessor);
		mesh.indices.assign(indices.begin(), indices.end());
	}

	return mesh;
}