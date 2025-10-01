#include "Engine/Engine.h"
#include "Events/MouseMoveEvent.h"
#include "Events/KeyDownEvent.h"
#include "Engine/Util/JsonFile.h"

Engine::Engine(const char* configPath) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	
	this->config = LoadConfig(configPath);

	this->window = SDL_CreateWindow(
		this->config.win_name.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		this->config.win_width,
		this->config.win_height,
		SDL_WINDOW_VULKAN);

	SDL_SetWindowResizable(this->window, SDL_TRUE);

	if (!window) {
		assert(false && "SDL window not created");
	}

	this->vulkan.Init(this->window);
	this->materialManager.Init(this->vulkan);

	GECS::Init();

	Level level = this->LoadLevel(this->config.scene_name);

	this->sceneSystem = GECS::GECSInstance->GetSystemManager()->AddSystem<SceneSystem>(this->vulkan, this->meshManager, this->materialManager);
	this->sceneSystem->LoadScene(level.scenePath);

	InputSystem* is = GECS::GECSInstance->GetSystemManager()->AddSystem<InputSystem>();

	this->renderSystem = GECS::GECSInstance->GetSystemManager()->AddSystem<RenderSystem>(this->vulkan, this->meshManager, this->materialManager, config, level);
	
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
				this->config.win_width = event.window.data1;
				this->config.win_height = event.window.data2;
				break;
			default:
				break;
			}
		}

		if (this->vulkan.SwapchainNeedsRecreation()) {
			this->vulkan.RecreateSwapchain(this->config.win_width, this->config.win_height);
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

const Level Engine::LoadLevel(std::string file) {
	Level level;

	JsonFile jsonLevel(std::filesystem::path{ "default.json" });
	level.scenePath = jsonLevel.GetPath("scene");
	level.skyboxPath = jsonLevel.GetPath("skybox");

	level.cameraPos = jsonLevel.GetVec3("camera");

	level.ambientIntensity = jsonLevel.GetFloat("ambient");
	level.fogIntensity = jsonLevel.GetFloat("fog");

	glm::vec3 ac = jsonLevel.GetVec3("ambient_color");
	glm::vec3 fc = jsonLevel.GetVec3("fog_color");

	level.ambientColor = {
		ac[0] / 255,
		ac[1] / 255,
		ac[2] / 255
	};
	level.fogColor = {
		fc[0] / 255,
		fc[1] / 255,
		fc[2] / 255
	};

	return level;
}

const EngineConfig Engine::LoadConfig(const char* configPath) {
	EngineConfig config;

	JsonFile jsonConfig(std::filesystem::path{configPath});
	config.win_name = jsonConfig.GetString("win_name");
	config.win_width = jsonConfig.GetFloat("win_width");
	config.win_height = jsonConfig.GetFloat("win_height");
	config.use_a2c = static_cast<bool>(jsonConfig.GetFloat("use_a2c"));

	config.scene_name = jsonConfig.GetString("scene_name");
	
	return config;
}