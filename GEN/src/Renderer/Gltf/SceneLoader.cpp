#include "Renderer/Gltf/SceneLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14
#include <tiny_gltf.h>

namespace {
	// Empty tinygltf load image callbacks

	bool LoadImageData(
		tinygltf::Image* image,
		const int image_idx,
		std::string* err,
		std::string* warn,
		int req_width,
		int req_height,
		const unsigned char* bytes,
		int size,
		void*)
	{
		return true;
	}

	bool WriteImageData(
		const std::string* basepath,
		const std::string* filename,
		const tinygltf::Image* image,
		bool embedImages,
		const tinygltf::FsCallbacks* fs_cb,
		const tinygltf::URICallbacks* uri_cb,
		std::string* out_uri,
		void*)
	{
		return true;
	}

	Transform FormGltfNodeToTransform(const tinygltf::Node& node) {
		Transform transform;
		if (!node.translation.empty())
			transform.SetPosition(Transform::Tg2GlmVec3(node.translation));
		if (!node.scale.empty())
			transform.SetScale(Transform::Tg2GlmVec3(node.scale));
		if (!node.rotation.empty())
			transform.SetRotation(Transform::Tg2GlmQuat(node.rotation));

		return transform;
	}

	void LoadModel(tinygltf::Model& model, const std::filesystem::path& path) {
		
		tinygltf::TinyGLTF loader;

		// We don't need to upload images via tinygltf
		// Because we have STBImage loader
		loader.SetImageLoader(LoadImageData, nullptr);
		loader.SetImageWriter(WriteImageData, nullptr);

		std::string err;
		std::string warn;

		bool result = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());
		if (!result) {
			assert(false && "Scene loader: failed to load glTF scene");
		}
	}

	GECS::f32 GetEmissiveFactor(const tinygltf::Material& gltfMaterial) {
		if (gltfMaterial.extensions.contains("KHR_materials_emissive_strength")) {
			return static_cast<GECS::f32>(gltfMaterial.extensions.at("KHR_materials_emissive_strength").Get("emissiveStrength").GetNumberAsDouble());
		}
		return 1.f;
	}
}

namespace Gltf {
	//// MESH ////
	template<typename T>
	std::span<const T> LoadGLTFBufferToVector(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView]; // without coping
		assert(accessor.ByteStride(bufferView) == sizeof(T) && "Mesh loading: types in scene and buffer are different");

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
				//mesh.vertices[i].texCoords.x = texCoords[i].x;
				//mesh.vertices[i].texCoords.y = texCoords[i].y;
				mesh.vertices[i].texCoordX = texCoords[i].x;
				mesh.vertices[i].texCoordY = texCoords[i].y;
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

	//// MATERIAL ////
	Material LoadMaterial(gvk::Vulkan& vulkan, const tinygltf::Model& model, const std::filesystem::path& fileDir, const tinygltf::Material& gltfMaterial) {
		Material material{
			.baseColor = GetDiffuseColor(gltfMaterial),
			.metallicFactor = static_cast<GECS::f32>(gltfMaterial.pbrMetallicRoughness.metallicFactor),
			.roughnessFactor = static_cast<GECS::f32>(gltfMaterial.pbrMetallicRoughness.roughnessFactor),
			.name = gltfMaterial.name
		};

		if (HasDiffuseTexture(gltfMaterial)) {
			const std::filesystem::path diffusePath = GetDiffuseTexturePath(model, gltfMaterial, fileDir);
			material.diffuseTexture = vulkan.GetImageManager().LoadImageFromFile
			(
				diffusePath,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				true
			);
		}

		if (HasNormalMapTexture(gltfMaterial)) {
			const std::filesystem::path normalMapPath = GetNormalMapTexturePath(model, gltfMaterial, fileDir);
			material.normalMapTexture = vulkan.GetImageManager().LoadImageFromFile
			(
				normalMapPath,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				true
			);
		}

		if (HasMetallicRoughTexture(gltfMaterial)) {
			const std::filesystem::path metalRoughPath = GetMetallicRoughTexturePath(model, gltfMaterial, fileDir);
			material.metallicRoughnessTexture = vulkan.GetImageManager().LoadImageFromFile
			(
				metalRoughPath,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				true
			);
		}

		if (HasEmissiveTexture(gltfMaterial)) {
			const std::filesystem::path emissivePath = GetEmissiveTexturePath(model, gltfMaterial, fileDir);
			material.emissiveTexture = vulkan.GetImageManager().LoadImageFromFile
			(
				emissivePath,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				true
			);

			material.emissiveFactor = GetEmissiveFactor(gltfMaterial);
		}

		return material;
	}

	LinearColor GetDiffuseColor(const tinygltf::Material& gltfMaterial) {
		const std::vector<double> color = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
		assert(color.size() == 4);
		return LinearColor{ (float)color[0], (float)color[1], (float)color[2], (float)color[3] };
	}

	std::filesystem::path GetDiffuseTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir) {
		const GECS::i32 textureIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
		const tinygltf::Texture& texture = model.textures[textureIndex];
		const tinygltf::Image& image = model.images[texture.source];

		return fileDir / image.uri;
	}

	std::filesystem::path GetNormalMapTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir) {
		const GECS::i32 textureIndex = gltfMaterial.normalTexture.index;
		const tinygltf::Texture& texture = model.textures[textureIndex];
		const tinygltf::Image& image = model.images[texture.source];

		return fileDir / image.uri;
	}

	std::filesystem::path GetMetallicRoughTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir) {
		const GECS::i32 textureIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
		const tinygltf::Texture& texture = model.textures[textureIndex];
		const tinygltf::Image& image = model.images[texture.source];

		return fileDir / image.uri;
	}

	std::filesystem::path GetEmissiveTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir) {
		const GECS::i32 textureIndex = gltfMaterial.emissiveTexture.index;
		const tinygltf::Texture& texture = model.textures[textureIndex];
		const tinygltf::Image& image = model.images[texture.source];

		return fileDir / image.uri;
	}

	bool HasDiffuseTexture(const tinygltf::Material& gltfMaterial) {
		const GECS::i32 textureIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
		return textureIndex != -1;
	}

	bool HasNormalMapTexture(const tinygltf::Material& gltfMaterial) {
		const GECS::i32 textureIndex = gltfMaterial.normalTexture.index;
		return textureIndex != -1;
	}

	bool HasMetallicRoughTexture(const tinygltf::Material& gltfMaterial) {
		const GECS::i32 textureIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
		return textureIndex != -1;
	}

	bool HasEmissiveTexture(const tinygltf::Material& gltfMaterial) {
		const GECS::i32 textureIndex = gltfMaterial.emissiveTexture.index;
		return textureIndex != -1;
	}

	//// LIGHT ////
	LinearColor Tg2LinearColor(const std::vector<double>& vec)
	{
		if (vec.size() == 4) {
			return LinearColor{ (float)vec[0], (float)vec[1], (float)vec[2], (float)vec[3] };
		}

		assert(vec.size() == 3 && "Scene loader: unxpected light color");
		return LinearColor{ (float)vec[0], (float)vec[1], (float)vec[2], 1.f };
	}

	GLTFLightType FromGLTF2builtInLight(const std::string& lightType) {
		if (lightType == GLTF_LIGHTS_PUNCTUAL_POINT_NAME)
			return GLTFLightType::Point;
		else if (lightType == GLTF_LIGHTS_PUNCTUAL_DIRECTIONAL_NAME)
			return GLTFLightType::Directional;
		else if (lightType == GLTF_LIGHTS_PUNCTUAL_SPOT_NAME)
			return GLTFLightType::Spot;

		assert(false && "Scene loader: unexpected light");
		return GLTFLightType::None;
	}

	GLTFLight LoadLight(const tinygltf::Light& light) {
		const float pbrWatts2Lumens = 683.f;

		GLTFLight gltfLight;
		gltfLight.name = light.name;
		gltfLight.type = FromGLTF2builtInLight(light.type);
		gltfLight.color = Tg2LinearColor(light.color);
		gltfLight.intensity = static_cast<float>(light.intensity) / pbrWatts2Lumens;
		gltfLight.range = static_cast<float>(light.range);
		gltfLight.SetConeAngles(static_cast<float>(light.spot.innerConeAngle),
			static_cast<float>(light.spot.outerConeAngle));

		return gltfLight;
	}

	//// NODE ////
	void BuildNodeTree(GLTFSceneNode& rootNode, const tinygltf::Node& gltfNode, const tinygltf::Model& model) {
		rootNode.name = gltfNode.name;
		rootNode.transform = FormGltfNodeToTransform(gltfNode);

		rootNode.meshIndex = gltfNode.mesh;
		rootNode.lightIndex = gltfNode.light;

		rootNode.children.resize(gltfNode.children.size());
		for (std::size_t i = 0; i < gltfNode.children.size(); i++) {
			const tinygltf::Node& childNode = model.nodes[gltfNode.children[i]];
			std::unique_ptr<GLTFSceneNode>& childPtr = rootNode.children[i];
			childPtr = std::make_unique<GLTFSceneNode>();
			GLTFSceneNode& child = *childPtr;
			BuildNodeTree(child, childNode, model);
		}
	}

	//// SCENE ////
	GLTFScene LoadGLTFScene(const std::filesystem::path& path, gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager) {
		const std::filesystem::path& filePath = path.parent_path();
		GLTFScene scene{ .path = path };
		tinygltf::Model model;

		LoadModel(model, path);

		const tinygltf::Scene& gltfScene = model.scenes[model.defaultScene];

		// Loading materials

		// first - gltf material id
		// second - material id for manager
		std::unordered_map<std::size_t, MaterialId> materialMaaping;
		for (std::size_t i = 0; i < model.materials.size(); i++) {
			const tinygltf::Material& gltfMaterial = model.materials[i];
			const MaterialId materialId = materialManager.AddMaterial(
				vulkan,
				LoadMaterial(vulkan, model, filePath, gltfMaterial)
			);
			materialMaaping.emplace(i, materialId);
		}

		// Loading meshes
		for (const tinygltf::Mesh& gltfMesh : model.meshes) {
			GLTFSceneMesh sceneMesh;
			sceneMesh.primitives.resize(gltfMesh.primitives.size());
			sceneMesh.primitiveMaterials.resize(gltfMesh.primitives.size());
			for (std::size_t i = 0; i < gltfMesh.primitives.size(); i++) {
				const tinygltf::Primitive& primitive = gltfMesh.primitives[i];
				Mesh mesh = CreateMeshFromPrimitive(model, gltfMesh.name, primitive);

				MeshId meshId = meshManager.AddMesh(vulkan, mesh);

				MaterialId materialId = materialManager.GetPlaceholder();
				if (primitive.material != -1)
					materialId = materialMaaping.at(primitive.material);

				sceneMesh.primitives[i] = meshId;
				sceneMesh.primitiveMaterials[i] = materialId;
			}
			scene.sceneMeshes.push_back(std::move(sceneMesh));
		}

		// Loading lights
		scene.lights.reserve(model.lights.size());
		for (const tinygltf::Light& light : model.lights)
			scene.lights.push_back(LoadLight(light));

		// Loading nodes
		scene.nodes.resize(gltfScene.nodes.size());
		for (std::size_t i = 0; i < gltfScene.nodes.size(); i++) {
			const tinygltf::Node gltfNode = model.nodes[gltfScene.nodes[i]];

			std::unique_ptr<GLTFSceneNode>& nodePtr = scene.nodes[i];
			nodePtr = std::make_unique<GLTFSceneNode>();
			GLTFSceneNode& node = *nodePtr;
			BuildNodeTree(node, gltfNode, model);
		}

		return scene;
	}
}