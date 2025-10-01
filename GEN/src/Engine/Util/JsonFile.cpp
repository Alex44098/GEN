#include "Engine/Util/JsonFile.h"
#include <fstream>

JsonFile::JsonFile(const std::filesystem::path& path) {
	std::ifstream f(path);
	if (!f.good()) {
		this->good = false;
		return;
	}

	f >> this->file;
	this->path = path;
}

const std::string JsonFile::GetString(std::string value) {
	std::string s = this->file[value];

	return s;
}

const std::filesystem::path JsonFile::GetPath(std::string value) {
	std::string s = this->file[value];
	const std::filesystem::path p{ s };

	return p;
}

const float JsonFile::GetFloat(std::string value) {
	const auto& f = this->file.at(value);
	return f.get<float>();
}

const glm::vec3 JsonFile::GetVec3(std::string value) {
	const auto& vec = this->file.at(value);
	if (vec.size() == 3) {
		return glm::vec3{
			vec[0].get<std::uint32_t>(),
			vec[1].get<std::uint32_t>(),
			vec[2].get<std::uint32_t>()
		};
	}
	else {
		return glm::vec3{0, 0, 0};
	}
}