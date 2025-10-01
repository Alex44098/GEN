#include "Engine/Engine.h"

#include <GECSHeaders.h>

int main(int argc, char* argv[]) {
	Engine* engine = new Engine("config.json");

	engine->run();

	engine->~Engine();
	return 0;
}