#include "Renderer/RenderSystem.h"
#include "Events/KeyDownEvent.h"

#include "Renderer/Vulkan/Util/CubemapLoader.h"

RenderSystem::RenderSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager, const EngineConfig config, const Level level)
			:vulkan(vulkan), meshManager(meshManager), materialManager(materialManager), renderer(meshManager, materialManager), currentLevel(level) {

	this->renderer.Init(vulkan, config);

	ImageId skyboxId = Util::LoadCubemap(vulkan, level.skyboxPath);
	this->renderer.SetSkyboxImage(skyboxId);

	const GECS::f32 aspectRatio =
		static_cast<GECS::f32>(config.win_width) / static_cast<GECS::f32>(config.win_height);
	//this->camera.SetUseInverseDepth(true);
	this->camera.Init(glm::radians(90.f), 0.1f, 300.f, aspectRatio);
	this->camera.SetPosition(level.cameraPos);

	GECS::GECSInstance->GetEventQueue()->AddEventHandler<KeyDownEvent>(this, &RenderSystem::ProcessKeyDown);
}

RenderSystem::~RenderSystem() {
	this->renderer.Destroy(this->vulkan);
}

void RenderSystem::Update(GECS::f32 delta) {
	Gltf::GLTFSceneData sceneData{
		.camera = this->camera,
		.ambientColor = this->currentLevel.ambientColor,
		.ambientIntensity = this->currentLevel.ambientIntensity,
		.fogColor = this->currentLevel.fogColor,
		.fogIntensity = this->currentLevel.fogIntensity
	};

	// Build drawing order
	this->CreateDraws(this->vulkan, sceneData);

	// Calc window params for bliting
	{
		const glm::ivec2 drawImageSize = this->renderer.GetDrawImage(this->vulkan).getSize2D();
		const glm::ivec2 swapchainSize = this->vulkan.GetSwapchainSize();
		// const glm::vec4 blitRect = this->CalcLetterBox(drawImageSize, swapchainSize, 1);

		// x, y - window position
		// z, w - window width and height
		this->windowPos.x = 0;
		this->windowPos.y = 0;

		this->windowSize.x = drawImageSize.x;
		this->windowSize.y = drawImageSize.y;
	}

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
}

void RenderSystem::CreateDraws(gvk::Vulkan& vulkan, const Gltf::GLTFSceneData& sceneData) {
	this->renderer.StartFrameBuilding();

	this->CollectRenderingUnits();

#ifdef _DEBUG
	this->CollectDebugDraws(vulkan, sceneData);
#endif

	this->renderer.EndFrameBuilding();
}

void RenderSystem::CollectRenderingUnits() {
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
}

#ifdef _DEBUG
void RenderSystem::CollectDebugDraws(gvk::Vulkan& vulkan, const Gltf::GLTFSceneData& sceneData) {
	this->renderer.ClearDebugDraws();
	if (drawCullingLines)
		this->renderer.AddDebugDrawsFromRenderingUnits(vulkan, sceneData.camera);
}
#endif

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

void RenderSystem::ProcessKeyDown(const GECS::Event::IEvent* e) {
	const KeyDownEvent* event = reinterpret_cast<const KeyDownEvent*>(e);

	switch (event->keyCode) {
	case SDLK_p:
		this->drawCullingLines = !drawCullingLines;
		break;
	}
}