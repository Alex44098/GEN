#pragma once

#include <filesystem>
#include <glm.hpp>

#include "json.hpp"

class JsonFile {
	nlohmann::json file;
	std::filesystem::path path;
	bool good{ true };

public:
	JsonFile(const std::filesystem::path& path);

	const std::filesystem::path GetPath(std::string value);
	const float GetFloat(std::string value);
	const glm::vec3 GetVec3(std::string value);
};