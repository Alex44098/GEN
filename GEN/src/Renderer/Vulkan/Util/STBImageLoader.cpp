#include "Renderer/Vulkan/Util/STBImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void STBImage::Destroy() {
	if (this->hasHdr)
		stbi_image_free(this->hdrData);
	else
		stbi_image_free(this->data);
}

namespace Util {
	STBImage LoadSTBImage(const std::filesystem::path& path) {
		STBImage image;
		if (stbi_is_hdr(path.string().c_str())) {
			image.hasHdr = true;
			image.hdrData = stbi_loadf(path.string().c_str(), &image.width, &image.height, &image.comp, 4);
		}
		else {
			image.data = stbi_load(path.string().c_str(), &image.width, &image.height, &image.channels, 4);
		}
		image.channels = 4;
		return image;
	}
}