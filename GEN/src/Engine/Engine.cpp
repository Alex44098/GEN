#include "Engine/Engine.h"
#include "Events/MouseMoveEvent.h"
#include "Events/KeyDownEvent.h"
#include "Engine/Util/JsonFile.h"

Engine::Engine(const WindowParams& params) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	this->wParams = params;
	this->window = SDL_CreateWindow(
		this->wParams.name.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		this->wParams.size.x,
		this->wParams.size.y,
		SDL_WINDOW_VULKAN);

	SDL_SetWindowResizable(this->window, SDL_TRUE);

	if (!window) {
		assert(false && "SDL window not created");
	}

	this->vulkan.Init(this->window);
	this->materialManager.Init(this->vulkan);

	GECS::Init();

	Level level = this->LoadLevel("default.json");

	this->sceneSystem = GECS::GECSInstance->GetSystemManager()->AddSystem<SceneSystem>(this->vulkan, this->meshManager, this->materialManager);
	this->sceneSystem->LoadScene(level.scenePath);

	InputSystem* is = GECS::GECSInstance->GetSystemManager()->AddSystem<InputSystem>();

	this->renderSystem = GECS::GECSInstance->GetSystemManager()->AddSystem<RenderSystem>(this->vulkan, this->meshManager, this->materialManager, this->wParams.size, level);
	
	CameraSystem* cs = GECS::GECSInstance->GetSystemManager()->AddSystem<CameraSystem>(this->renderSystem->GetCamera());

	cs->AddDependency(is);
	this->renderSystem->AddDependency(cs);
	this->sceneSystem->AddDependency(is);

	GECS::GECSInstance->GetSystemManager()->RebuildSystemsOrder();
}

void Engine::run() {
	const GECS::f32 FPS = 60.f;
	const GECS::f32 dt = 1.f / FPS;

	bool running = true;
	SDL_Event event;

	while (running) {
		SDL_PumpEvents();
		while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_SYSWMEVENT)) {
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				running = false;
				return;
				break;
			case SDL_WINDOWEVENT_RESIZED:
				this->wParams.size = { event.window.data1, event.window.data2 };
				break;
			default:
				break;
			}
		}

		if (this->vulkan.SwapchainNeedsRecreation()) {
			this->vulkan.RecreateSwapchain(wParams.size.x, wParams.size.y);
		}

		GECS::GECSInstance->Update(dt);
	}
}

Engine::~Engine() {
	this->vulkan.WaitIdle();

	GECS::Destroy();

	this->meshManager.CleanMeshes(this->vulkan);
	this->materialManager.Destroy(this->vulkan);
	this->vulkan.Destroy();

	SDL_DestroyWindow(this->window);
	SDL_Quit();
}

Level Engine::LoadLevel(std::string file) {
	Level level;

	JsonFile jsonLevel(std::filesystem::path{ "default.json" });
	level.scenePath = jsonLevel.GetPath("scene");
	level.skyboxPath = jsonLevel.GetPath("skybox");

	level.ambientIntensity = jsonLevel.GetFloat("ambient");
	level.fogIntensity = jsonLevel.GetFloat("fog");

	level.ambientColor = jsonLevel.GetVec3("ambient_color");
	level.fogColor = jsonLevel.GetVec3("fog_color");

	return level;
}