# GEN
A little silly engine that uses the ECS library (GECS), the **Vulkan API** and renders scenes in **glTF 2.0** format in real time.

## Building a project
1. Place the compiled library files in the **3rdparty/** folder:

* GECS/bin/x64/release/GECS.dll
* GECS/lib/x64/release/GECS.lib
* SDL2/bin/SDL2.dll
* SDL2/lib/SDL2.lib
* Vulkan/1.3.290.0/Lib/release/vulkan-1.lib
* Vulkan/1.3.290.0/Lib/release/vk-bootstrap.lib

2. Open the **GEN.sln** project file using Microsoft Visual Studio and build solution.

3. Compile the shaders from the **GEN/src/Shaders/** folder using the compiler **glslc.exe** from the Vulkan SDK.

## Usage
In the same folder with the executable application, you must place the folder **shaders/** all compiled shaders.

You also need to create a file with a description of the scene **default.json**, which should contain:
* The path to a three-dimensional scene in glTF 2.0 format;
* The path to the skybox (the folder should contain 6 textures with names: back, bottom, front, left, right, top in the format .jpg);
* Ambient light and fog settings;
* Camera position.

Example of description:
```json
{
    "scene": "scenes/scene1/scene.gltf",
    "skybox": "skybox",

    "ambient": 0.1,
    "ambient_color": [0, 0, 0],
    "fog": 0.05,
    "fog_color": [193, 180, 180],

    "camera": [0, 0, 0]
}
```

## Third-party libraries
* **GECS**: https://github.com/Alex44098/GECS
* **VkBootstrap**: https://github.com/charles-lunarg/vk-bootstrap
* **Volk**: https://github.com/zeux/volk
* **VMA**: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
* **SDL**: https://github.com/libsdl-org/SDL
* **tinyglTF**: https://github.com/syoyo/tinygltf
* **glm**: https://github.com/icaven/glm
* **nlohmann/json**: https://github.com/nlohmann/json

## Other
Special thanks to the author of the article, which simplified the introduction to Vulkan: https://edw.is/learning-vulkan/
