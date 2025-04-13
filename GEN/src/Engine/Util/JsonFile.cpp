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

const std::filesystem::path JsonFile::GetPath(std::string value) {
	std::string s = this->file[value];
	const std::filesystem::path p{ s };

	return p;
}