#include "Engine/Engine.h"

#include <GECSHeaders.h>

int main(int argc, char* argv[]) {

	WindowParams params{glm::ivec2(1280, 720), "Window"};
	Engine* engine = new Engine(params);

	engine->run();

	engine->~Engine();
	return 0;
}