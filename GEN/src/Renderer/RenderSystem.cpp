#include "Renderer/RenderSystem.h"

#include "Renderer/Vulkan/Util/CubemapLoader.h"

RenderSystem::RenderSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager, const glm::ivec2& drawImageSize)
			:vulkan(vulkan), meshManager(meshManager), materialManager(materialManager), renderer(meshManager, materialManager) {

	this->renderer.Init(vulkan, drawImageSize);

	ImageId skyboxId = Util::LoadCubemap(vulkan, "skybox");
	this->renderer.SetSkyboxImage(skyboxId);

	const GECS::f32 aspectRatio =
		static_cast<GECS::f32>(drawImageSize.x) / static_cast<GECS::f32>(drawImageSize.y);
	//this->camera.SetUseInverseDepth(true);
	this->camera.Init(glm::radians(90.f), 0.1f, 75.f, aspectRatio);
	this->camera.SetPosition(glm::vec3{0.f, 0.f, -4.f});
	//this->camera.SetRotation(glm::quat{ 0.9622502f, 0.084186f, -0.2578342f, -0.0225576f });
}

RenderSystem::~RenderSystem() {
	this->renderer.Destroy(this->vulkan);
}

void RenderSystem::Update(GECS::f32 delta) {
	// Build drawing order
	this->CreateDraws();

	// Calc window params for bliting
	{
		const glm::ivec2 drawImageSize = this->renderer.GetDrawImage(this->vulkan).getSize2D();
		const glm::ivec2 swapchainSize = this->vulkan.GetSwapchainSize();
		const glm::vec4 blitRect = this->CalcLetterBox(drawImageSize, swapchainSize, 1);

		// x, y - window position
		// z, w - window width and height
		this->windowPos.x = blitRect.x;
		this->windowPos.y = blitRect.y;

		this->windowSize.x = blitRect.z;
		this->windowSize.y = blitRect.w;
	}

	Gltf::GLTFSceneData sceneData{
		.camera = this->camera
	};

	VkCommandBuffer cmdBuffer = this->vulkan.StartFrameBuilding();
	this->renderer.RenderFrame(cmdBuffer, this->vulkan, sceneData);

	this->vulkan.EndFrameBuilding
	(
		cmdBuffer,
		this->renderer.GetPostFXImageId(),
		LinearColor{0.f, 0.f, 0.f, 1.f}, // Black
		true,
		glm::vec4{this->windowPos.x, this->windowPos.y, this->windowSize.x, this->windowSize.y}
	);

	//float pastZ = this->camera.GetPosition().z + 0.01f;
	//this->camera.SetPosition(glm::vec3{ 0.f, 0.f, pastZ});
	
	//glm::quat oc = this->camera.GetRotation();
	//oc.z += 0.001f;
	//oc.y += 0.001f;
	//this->camera.SetRotation(oc);
}

void RenderSystem::CreateDraws() {
	this->renderer.StartFrameBuilding();

	GECS::ComponentManager* cm = GECS::GECSInstance->GetComponentManager();
	for (auto it = cm->begin<RenderComponent>(); it != cm->end<RenderComponent>(); ++it) {
		for (std::size_t i = 0; i < it->meshes.size(); i++) {
			MeshId m = (it->meshes[i]);
			this->renderer.AddRenderingUnit
			(
				it->meshes[i],
				it->materials[i],
				it->transforms[i].GetMatrix(),
				it->castShadow
			);
		}
	}

	this->renderer.EndFrameBuilding();
}

glm::vec4 RenderSystem::CalcLetterBox(const glm::ivec2 srcSize, const glm::ivec2 dstSize, GECS::i32 scale) {
	if (scale != 0 && dstSize.x >= srcSize.x && dstSize.y >= srcSize.y) {
		const GECS::f32 widthScale = dstSize.x / static_cast<float>(srcSize.x);
		const GECS::f32 heightScale = dstSize.y / static_cast<float>(srcSize.y);

		GECS::f32 minScale = std::min(widthScale, heightScale);

		if (minScale != 0.f) {
			const glm::ivec2 realSrcSize = srcSize * static_cast<int>(minScale);
			const glm::ivec2 position = (dstSize - realSrcSize) / 2;
			return glm::vec4{position.x, position.y, srcSize.x * minScale, srcSize.y * minScale};
		}
	}

	const GECS::f32 srcAspect = srcSize.x / static_cast<float>(srcSize.y);
	const GECS::f32 dstAspect = dstSize.x / static_cast<float>(dstSize.y);

	const GECS::f32 resWidth = (dstAspect > srcAspect) ?
		(dstSize.y * srcAspect) :
		dstSize.x;

	const GECS::f32 resHeight = (dstAspect < srcAspect) ?
		(dstSize.x / srcAspect) :
		dstSize.y;

	return glm::vec4{
		(dstSize.x - resWidth) / 2.0f, // Centering
		(dstSize.y - resHeight) / 2.0f, // Centering
		resWidth,
		resHeight
	};
}