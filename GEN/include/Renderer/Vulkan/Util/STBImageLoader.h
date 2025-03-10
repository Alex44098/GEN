#pragma once

#include <filesystem>

struct STBImage {
	STBImage() = default;

	// move only
	STBImage(STBImage&& o) = default;
	STBImage& operator=(STBImage&& o) = default;
	STBImage(const STBImage& o) = delete;
	STBImage& operator=(const STBImage& o) = delete;

	unsigned char* data{ nullptr };
	int width;
	int height;
	int channels;

	// wow, HDR
	float* hdrData{ nullptr };
	bool hasHdr;
	int comp;

	void Destroy();
};

namespace Util {
	STBImage LoadSTBImage(const std::filesystem::path& path);
}